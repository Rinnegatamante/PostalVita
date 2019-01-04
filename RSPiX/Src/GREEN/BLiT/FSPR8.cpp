////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2016 RWS Inc, All Rights Reserved
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of version 2 of the GNU General Public License as published by
// the Free Software Foundation
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
//
//********************************************
// Here is the basic FSPR8 IO, Convert, & BLiT
//********************************************
#include "System.h"

#ifdef PATHS_IN_INCLUDES
	#include "GREEN/BLiT/BLIT.H"
	#include "GREEN/BLiT/_BlitInt.H"
	#include "ORANGE/QuickMath/Fractions.h"
	#include "GREEN/Image/SpecialTyp.h"
#else
	#include "BLIT.H"
	#include "_BlitInt.H" 
	#include "Fractions.h" 
	#include "specialtyp.h"
#endif

#include <string.h>

//***************************************************************************
// NOTES:  The FSPR8 compression format is NOT based around the idea of
// data compression, but around the sheer speed of assembly blitting.
// In short, the compression op codes are designed to have the fastest
// decoding possible.  In emprical testing, this FSP8 format outperformed
// five other more "advanced" algorithms which took longer to read their
// OP-CODES.  It provides semi-random access by compressing scan lines as
// autonomous units, which makes things like clipping much, much faster.
//***************************************************************************
// There is currently no convert options for FSPR8.
// Color index zero is transparancy.  Mild performance degredation may occur
// for objects with visual details larger than 256 pixels across.

//----------------  Predeclare linklate functions ---------------------------
int16_t		DeleteFSPR8(RImage* pImage);
int16_t		ConvertToFSPR8(RImage* pImage);
int16_t		ConvertFromFSPR8(RImage* pImage);
int16_t		LoadFSPR8(RImage* pImage, RFile* pcf);
int16_t		SaveFSPR8(RImage* pImage, RFile* pcf);

//----------------  EXTRA Conversion Information ----------------------------

// Currently none.

//-------------------  HOOK into the CImage world ------------------------

IMAGELINKLATE(FSPR8,ConvertToFSPR8,ConvertFromFSPR8,
				  LoadFSPR8,SaveFSPR8,NULL,DeleteFSPR8);

//------------------------------------------------------------------------

int16_t		DeleteFSPR8(RImage* pImage)
	{
	if (pImage->m_pSpecial != NULL)
		{
		delete (RSpecialFSPR8*) pImage->m_pSpecial;
		}
	
	return SUCCESS;
	}

//------------------------------------------------------------------------

// SAVES ONLY the pSpecial information!
//
int16_t		SaveFSPR8(RImage* pImage, RFile* pcf)
	{
	// Assume valid pImage and cnfile:
	RSpecialFSPR8* pSpec = (RSpecialFSPR8*) pImage->m_pSpecialMem;

	if (!pSpec)
		{
		TRACE("Save FSPR8: Bad FSPR8!\n");
		return -1;
		}

	//------------------
	// Initial Security:
	//------------------

	pcf->Write("__FSPR8__"); // image type
	U16 version = (U16)(6); // Sprite incarnation 3, File Format 5
	pcf->Write(&version);

	//------------------
	// Info
	//------------------

	// NOTE: Some font info is stored here:
	pcf->Write((U16*)(&(pSpec->m_usSourceType)));
	pcf->Write(&(pSpec->m_lBufSize));
	pcf->Write(&(pSpec->m_lCodeSize));

	// Reserved for future expansion
	U32 reserved[4] = {0,0,0,0};
	pcf->Write(reserved,4); // 16 bytes reserved as of version 3.5

	// Write the pixel data:
	pcf->Write(pSpec->m_pCompBuf,pSpec->m_lBufSize);
	// Write the code data
	pcf->Write(pSpec->m_pCodeBuf,pSpec->m_lCodeSize);

	// Write the Line Array and Control array as 32-bit offsets:
	// Both are (H+1) long:
	int16_t i;
	U32	lOffset;
	// Do pointers into pixel data:
	for (i=0;i <= pImage->m_sHeight;i++)
		{
		lOffset = U32(pSpec->m_pBufArry[i] - pSpec->m_pCompBuf);
		pcf->Write(&lOffset);
		}

	// Do pointers into control data:
	for (i=0;i <= pImage->m_sHeight;i++)
		{
		lOffset = U32(pSpec->m_pCodeArry[i] - pSpec->m_pCodeBuf);
		pcf->Write(&lOffset);
		}

	return SUCCESS;
	}

//------------------------------------------------------------------------

// Assumes all of the FSPR1 has successfully been
// loaded EXCEPT for pSpecial[Mem]
//
int16_t		LoadFSPR8(RImage* pImage, RFile* pcf)
	{
	//------------------
	// Initial Security:
	//------------------
	char szTemp[10];
	pcf->Read(&szTemp[0]);

	// Check type:
	if (strcmp(szTemp,"__FSPR8__")) // not equal
		{
		TRACE("Load FSPR8: Not correct file type!\n");
		return -1;
		}

	// Check Version:
	U16 u16Temp;
	pcf->Read(&u16Temp);

	if (u16Temp != (6))
		{
		TRACE("Load FSPR8: This is an older FSPR8 format!\n");
		return -1;
		}

	//------------------
	// Info
	//------------------

	RSpecialFSPR8* pSpec = new RSpecialFSPR8;
	pImage->m_pSpecialMem = pImage->m_pSpecial = (uint8_t*)pSpec;

	pcf->Read((U16*)(&pSpec->m_usSourceType));
	pcf->Read(&pSpec->m_lBufSize);
	pcf->Read(&pSpec->m_lCodeSize);

	//-------- ALLOCATE THE FIELDS!!!
	// No alignment needed for 8-bit:
	pSpec->m_pCompMem = pSpec->m_pCompBuf = (uint8_t*)malloc(pSpec->m_lBufSize);
	pSpec->m_pCodeBuf = (uint8_t*)malloc(pSpec->m_lCodeSize);
	pSpec->m_pBufArry = (uint8_t**)malloc(sizeof(uint8_t*) * (pImage->m_sHeight+1));
	pSpec->m_pCodeArry = (uint8_t**)malloc(sizeof(uint8_t*) * (pImage->m_sHeight+1));

	//------------------
	// Reserved Space
	//------------------
	U32 u32Temp[4];
	pcf->Read(u32Temp,4); // 16 bytes reserved as of version 3.5

	//------------------
	// The Image Data
	//------------------

	// Now the actual data, which needs no alignment:
	// (Pre ALLOCATED)
	pcf->Read(pSpec->m_pCompBuf,pSpec->m_lBufSize);
	pcf->Read(pSpec->m_pCodeBuf,pSpec->m_lCodeSize);

	// Now restore the pointer list by adding them as offsets!
	// (Pre ALLOCATED!)
	int16_t i;
	U32	lOffset;
	// Do pointers into pixel data:
	for (i=0;i <= pImage->m_sHeight;i++)
		{
		pcf->Read(&lOffset);
		pSpec->m_pBufArry[i] = pSpec->m_pCompBuf + lOffset;
		}

	// Do pointers into control data:
	for (i=0;i <= pImage->m_sHeight;i++)
		{
		pcf->Read(&lOffset);
		pSpec->m_pCodeArry[i] = pSpec->m_pCodeBuf + lOffset;
		}

	return SUCCESS;
	}

//------------------------------------------------------------------------
// Returns to previous 8-bit screen format:
//
int16_t   ConvertFromFSPR8(RImage* pImage)
	{

	// Assume it is of the correct format:
	if (pImage->m_type != RImage::FSPR8) return RImage::NOT_SUPPORTED;

	// Will try to restore the previous type of uncompressed, 8-bit buffer:

	// Generate a new buffer:
	// GuessAPitch
	pImage->m_lPitch = (pImage->m_sWidth + 15) & (~15); // 128 it!
	pImage->CreateData(pImage->m_lPitch * pImage->m_sHeight); // should be blank

	// BLiT Into the buffer!
	uint8_t*	pDstLine = pImage->m_pData; // centered at 0,0
	uint8_t*	pDst;

	int16_t	sY=0;
	RSpecialFSPR8* pInfo = (RSpecialFSPR8*) pImage->m_pSpecial;

	uint8_t		ucCode;
	uint8_t*	pSrc;
	uint8_t*	pCB;

	for (sY = 0; sY < pImage->m_sHeight; sY++,pDstLine += pImage->m_lPitch)
		{
		pSrc = pInfo->m_pBufArry[sY];
		pDst = pDstLine;
		pCB = pInfo->m_pCodeArry[sY];

	NextCode3:

		//==============================================================	
		// 1) Do the clear run:
		ucCode = *(pCB)++;	// Get Skip Length
		if (ucCode == 255) continue;// End of line code
		pDst += ucCode;		// Skip
		
		//==============================================================	
		// 2) Do the opaque run:
		ucCode = *(pCB)++;			// Get Skip Length
		if (!ucCode) goto NextCode3;
		
		do	*(pDst++) = *(pSrc++);	// Copy pixel
		while (--ucCode);	
		
		goto NextCode3;
		}
	//==============================================================	
	// Now finish your job:
	//
	pImage->m_type = (RImage::Type)pInfo->m_usSourceType; // Set back the type;

	// Remove pSpecial:
	delete (RSpecialFSPR8*) pImage->m_pSpecial;
	pImage->m_pSpecial = pImage->m_pSpecialMem = NULL;

	return (int16_t)pImage->m_type;
	}

//------------------------------------------------------------------------

// Returns NOT_SUPPORTED if conversion is not possible:
// Destroys the image's buffer (officially) and sets it to NULL
//
int16_t   ConvertToFSPR8(RImage*  pImage)
	{
	// Convert to BKD8:
#ifdef	_DEBUG
	// Can convert any standard uncompressed 8-bit buffer....
	if (!ImageIsUncompressed(pImage->m_type))
	  {
	  TRACE("ConvertFSPR8:  Can only compress an uncompressed image!\n");
	  return RImage::NOT_SUPPORTED;
	  }

	if (pImage->m_sDepth != 8)
	  {
	  TRACE("ConvertFSPR8:  Can only compress an 8-bit image!\n");
	  return RImage::NOT_SUPPORTED;
	  }

	if (pImage->m_pData == NULL)
	  {
	  TRACE("Convert:  Invalid image passed to convert to FSPR8\n");
	  return RImage::NOT_SUPPORTED;
	  }

#endif

	//*********************  DO THE CONVERSION  ************************
	RSpecialFSPR8* pHeader = new RSpecialFSPR8;
	pHeader->m_usCompType = RImage::FSPR8;
	pHeader->m_usSourceType = (uint16_t) pImage->m_type;

	//************ RUN LENGTH COMPRESSION -> 8-bit Aligned **********

	//====== CONTROL BLOCK CODES (8-bit) ====== (Only one run type)
	// if first clear run byte = 255, done the line
	// This is initial offset,
	//
	// Next byte = # of bytes to run with (opaque run)   If the run is
	// >255 pixels, then you get a 0 for next clear run, then another 
	// amt of run... This keeps max run to 255.
	//
	// if the clear run byte = 255, done line, else...repeat the concept...

	//===== Justification for continual ADDING... that so RARELY will a
	// space ever be more than 254 pixels wide

	//===== The compressed buffer:
	// This is an 8-aligned pixel buffer containing only opaque pixels

	//===== The m_pBufArry ... points at the start of each new line in the pixel array

	//===== The m_pCodeArry ... points to the start of each line in the control block.

	int16_t x,y;  // For now, clear = 0, but this can be EASILY changed.
	int32_t i;
	int16_t	sCount;
	uint8_t *pucOldMem,*pucOldBuf; // For shrinking the buffer while maintaining alignment

	//********* MALLOC all elements to maximum possible length, then shrink them. (realloc)
	//********* for now, assume MALLOC returns 32-bit aligned ptr arrays.

	// Max memory requirements:  Compresssed block = uncompressed block.
	//									  Control Block = (width+1)*height
	//	m_pCodeArry always = m_pBufArry always = 4*height bytes.

	// So worst case scenario:  MEM = Height * (2*Width+9)

	//*********************************** These are always the same size!
	// Adjustment... for saving and retrieval, I am storing the total buffer sizes
	// as the last elements in the random access arrays.
	//
	pHeader->m_pCodeArry = (uint8_t**)calloc(pImage->m_sHeight+1,sizeof(uint8_t*));
	pHeader->m_pBufArry = (uint8_t**)calloc(pImage->m_sHeight+1,sizeof(uint8_t*));

	//************** For now, set these to an optimisitically large size: 1:1 compression (could be 2:1 worst case)
	int32_t	lSizeEstimate = ((int32_t)(pImage->m_sHeight+1))*(pImage->m_sWidth*2+1) + 15;
	pHeader->m_pCompMem = (uint8_t*)malloc((size_t)pImage->m_sHeight*(size_t)pImage->m_sWidth+15);

	pHeader->m_pCompBuf = (uint8_t*)(( (S64)(pHeader->m_pCompMem) + 15) & ~ 15); // align it 128!
	pHeader->m_pCodeBuf = (uint8_t*)malloc((size_t)lSizeEstimate);

	//******** For convenience, generate the Compressed Buffer immediately:
	uint8_t*	pucCPos = pHeader->m_pCompBuf;
	uint8_t*	pucBPos = pImage->m_pData; // read the actual buffer
	int16_t	sW = pImage->m_sWidth;
	int16_t	sH = pImage->m_sHeight;
	int16_t	sP = pImage->m_lPitch;

	// WARNING:  THIS IS HARD CODED 8-bit PIXEL SIZE STUFF!
	for (y=0;y<sH;y++)
	{
	pHeader->m_pBufArry[y] = (uint8_t*)(pucCPos - pHeader->m_pCompBuf); // set line pointer
	for (x=0,pucBPos = pImage->m_pData + (int32_t)sP * (int32_t)y; x<sW; x++)
		{
		if (*pucBPos != 0) *(pucCPos++) = *pucBPos;
		pucBPos++;
		}
	}

	// WARNING:  ASSERTS WILL BE SCREWED UP BY NEGATIVE PITCH!
	ASSERT( ((pucCPos - pHeader->m_pCompBuf)) < 2*1024*1024); // just in case something flakey happens!
	ASSERT( ((pucCPos - pHeader->m_pCompBuf)) >= 0); // just in case something flakey happens!
	// NOTE THE SIZE:
	pHeader->m_lBufSize = uint32_t(pucCPos - pHeader->m_pCompBuf);
		
	// Shrink the Compressed buffer:
	if (pucCPos == pHeader->m_pCompBuf)
		{
		TRACE("Convert to FSPR8: FATAL error.  A blank buffer cannot be compressed.\n");
		delete pHeader;

		return RImage::NOT_SUPPORTED;
		}

	// The alignment will shift when the realloc blindly copied the memory.  So, realloc is
	// nolonger feasible. I will take memory management into my own hands!
	//
	pucOldMem = pHeader->m_pCompMem;
	pucOldBuf = pHeader->m_pCompBuf;
	// create a new buffer of the appropriate size:
	// NOTE: pucCPos is an open stack!
	pHeader->m_pCompMem = (uint8_t*)calloc(1,(size_t)(pucCPos - pHeader->m_pCompBuf + 15));
	// And align it:
	pHeader->m_pCompBuf = (uint8_t*)(( (S64)(pHeader->m_pCompMem) +15)&~15);
	// Store the size of the Compressed Buffer:
	pHeader->m_pBufArry[sH] = (uint8_t*)(size_t)(pucCPos - pHeader->m_pCompBuf);


	// Now copy the old into the new aligned and free it:
	for (i=0;i<pucCPos - pucOldBuf;i++) pHeader->m_pCompBuf[i] = pucOldBuf[i];
	free(pucOldMem);
	
	// Now update the indexes (m_pBufArry) which point into PCBuf:
	for (y=0;y<sH;y++) pHeader->m_pBufArry[y] += (S64)(pHeader->m_pCompBuf);

	//******** NOW, the challange... Create the Control Block!
	pucBPos = pImage->m_pData;
	uint8_t*	pucConBlk = pHeader->m_pCodeBuf;

	for (y=0;y<sH;y++)
		{
		pHeader->m_pCodeArry[y] = (uint8_t*)(pucConBlk - pHeader->m_pCodeBuf); // set Block pointer
		pucBPos = pImage->m_pData + (int32_t)sP * (int32_t)y;
		x=0;

		NextRun:
		// A> The CLEAR RUN

		// 1) Count clear space
		sCount = 0;
		while ((x<sW) && (*pucBPos == 0)) {x++;sCount++;pucBPos++;}
		if (x == sW)	// you are at the end of the line!
			{
			*(pucConBlk++)=255; // end of line code
			continue;					// goto next y-line
			}	

		// 2) If count >254, back it up!
		if (sCount > 254)
			{
			pucBPos -= (sCount - 254);
			x -= (sCount - 254);
			sCount = 254;
			}	

		// 3) place count into control block:
		*(pucConBlk++)=(uint8_t)sCount;  // The Clear Run

		// B> The OPAQUE RUN

		// 1) Count opaque space
		sCount = 0;
		while ((x<sW) && (*pucBPos != 0)) {x++;sCount++;pucBPos++;}

		// 2) If count >255, back it up!
		if (sCount > 255)
			{
			pucBPos -= (sCount - 255);
			x -= (sCount - 255);
			sCount = 255;
			}	

		// 3) place opaque count into control block:
		*(pucConBlk++)=(uint8_t)sCount;  // The Clear Run

		if (x == sW)	// you are at the end of the line!
			{
			*(pucConBlk++)=255; // end of line code
			continue;					// goto next y-line
			}	
			
		goto NextRun;
		}

	// Store the size of the Control Block Buffer:
	pHeader->m_pCodeArry[sH] = (uint8_t*)(size_t)(pucConBlk - pHeader->m_pCodeBuf);
	// NOTE THE SIZE:
	pHeader->m_lCodeSize = uint32_t(pucConBlk - pHeader->m_pCodeBuf);

	// Shrink the Control Block buffer:
	pHeader->m_pCodeBuf = (uint8_t*)realloc((void*)pHeader->m_pCodeBuf,
										(size_t)(pucConBlk - pHeader->m_pCodeBuf));	

	// Move the indexes in (m_pCodeArry)
	for (y=0;y<sH;y++) pHeader->m_pCodeArry[y] += (S64)(pHeader->m_pCodeBuf);

	//******************************************************************

	//******************************************************************
	// Install the newly compressed buffer:

	// Kill the old buffer in an Image-Safe way:
	//pImage->DestroyData(); 
	void* pTempData = pImage->DetachData();
	RImage::DestroyDetachedData(&pTempData);

	pImage->m_pSpecial = (uint8_t*) pHeader;  // Put the data in special.
	pImage->m_pSpecialMem = pImage->m_pSpecial; // So that our delete may be called!
	pImage->m_type = RImage::FSPR8;
	pImage->m_ulSize = 0;	// BLiT needs to deal with copying, etc....

	return RImage::FSPR8;
	}

//------------------------------------------------------------------------

//*****************************  THE FSPRITE BLiT  ******************************
// currently 8-bit, but soon to be full color.
// Must deal with screen locking.
//
int16_t	rspBlit(RImage* pimSrc,RImage* pimDst,int16_t sDstX,int16_t sDstY,const RRect* prDst)
	{
	
	int16_t sClip;

	// 1) preliminary parameter validation:
#ifdef _DEBUG

	if ((pimSrc == NULL) || (pimDst == NULL))
		{
		TRACE("BLiT: null CImage* passed\n");
		return -1;
		}

	if ((!ImageIsUncompressed(pimDst->m_type)))
		{
		TRACE("BLiT: Cannot BLiT this into a compressed buffer!\n");
		return -1;
		}

	if ((pimSrc->m_type == RImage::FSPR16) || (pimSrc->m_type == RImage::FSPR32))
		{
		TRACE("BLiT: TC sprites are not YET implemented.\n");
		return -1;
		}

	if (pimSrc->m_type == RImage::FSPR1)
		{
		TRACE("BLiT: Use a different form of parameters for this type (see BLiT.DOC).\n");
		return -1;
		}

#endif

	// HOOK special cases:
	if (ImageIsUncompressed(pimSrc->m_type)) // for now do an opaque BLiT!
		{
		// Doing a background BLiT for speed, although a BlitT is more desired
		rspBlit(pimSrc,pimDst,0,0,sDstX,sDstY,pimSrc->m_sWidth,pimSrc->m_sHeight,prDst);
		return 0;
		}

	// 2) Destination Clipping is hard here:
	int16_t sClipL=0,sClipR=0,sClipT=0,sClipB=0;
	int16_t sSrcX = 0,sSrcY = 0; // clippng parameters...
	int16_t sW = pimSrc->m_sWidth; // clippng parameters...
	int16_t sH = pimSrc->m_sHeight; // clippng parameters...
	int32_t	lDstP = pimDst->m_lPitch;

	if (prDst)
		{
		// clip against user values
		sClip = prDst->sX - sDstX; // positive = clipped
		if (sClip > 0) { sW -= sClip; sSrcX += sClip; sDstX = prDst->sX; }
		sClip = prDst->sY - sDstY; // positive = clipped
		if (sClip > 0) { sH -= sClip; sSrcY += sClip; sDstY = prDst->sY; }
		sClip = sDstX + sW - prDst->sX - prDst->sW; // positive = clipped
		if (sClip > 0) { sW -= sClip; }
		sClip = sDstY + sH - prDst->sY - prDst->sH; // positive = clipped
		if (sClip > 0) { sH -= sClip; }

		if ( (sW <= 0) || (sH <= 0) ) return -1; // clipped out!
		}
	else	
		{
		// clip against full destination buffer
		if (sDstX < 0) { sW += sDstX; sSrcX -= sDstX; sDstX = 0; }
		if (sDstY < 0) { sH += sDstY; sSrcY -= sDstY; sDstY = 0; }
		sClip = sDstX + sW - pimDst->m_sWidth; // positive = clipped
		if (sClip > 0) sW -= sClip; // positive = clipped
		sClip = sDstY + sH - pimDst->m_sHeight; // positive = clipped
		if (sClip > 0) sH -= sClip; // positive = clipped

		if ((sW <= 0) || (sH <= 0)) return -1; // fully clipped
		}

		//**************  INSERT BUFFER HOOKS HERE!  ************************

	// do OS based copying!
	int16_t sNeedToUnlock = 0; // will be the name of a buffer to unlock.

	int16_t sBlitTypeDst = 0; // normal image:

	// IN RELEASE MODE, GIVE THE USER A CHANCE:
#ifndef _DEBUG

	//if (gsScreenLocked || gsBufferLocked) goto BLIT_PRELOCKED;

#endif

	// IN THIS IMPLEMENTATION, we must do LOCK, BLiT, UNLOCK, so I
	// must record which UNLOCK (if any) needs to be done AFTER the BLiT
	// has completed. (Lord help me if a blit gets interrupted)
	// NOT NECESSARY!!! THe SOURCE WILL ALWAYS BE A BUFFER!
	if (pimDst->m_type == RImage::IMAGE_STUB) sBlitTypeDst = (int16_t)((S64)pimDst->m_pSpecial);

	switch (sBlitTypeDst) // 0 = normal image
		{
		case BUF_MEMORY: // image to system buffer
/*
			// need to lock / unlock this one:
			if (rspLockBuffer()
				!=0)
				{
				TRACE("BLiT: Unable to lock the system buffer, failed!\n");
				return -1;
				}
			// Locked the system buffer, remember to unlock it:
			sNeedToUnlock = BUF_MEMORY;
*/
		break;

		case BUF_VRAM: // image to front screen
/*
			// need to lock / unlock this one:
			if (rspLockScreen()
				!=0)
				{
				TRACE("BLiT: Unable to lock the OnScreen system buffer, failed!\n");
				return -1;
				}
			// Locked the front VRAM, remember to unlock it:
			sNeedToUnlock = BUF_VRAM;	
*/
		break;

		case BUF_VRAM2: // image to back screen
			// need to lock / unlock this one:
			if (rspLockVideoFlipPage((void**)&(pimDst->m_pData),&(pimDst->m_lPitch))
				!=0)
				{
				TRACE("BLiT: Unable to lock the OffScreen system buffer, failed!\n");
				return -1;
				}
			// Locked the front VRAM, remember to unlock it:
			sNeedToUnlock = BUF_VRAM;			
		break;

		case 0: // image to image
			sNeedToUnlock = 0; // no unlock!
		break;

		default:
			TRACE("BLiT: This type of copy is not yet supported.\n");
			return -1;
		}
//BLIT_PRELOCKED:

	//*******************************************************************
	// 8-bit biased!
	// Check for locking error:
	if (!pimDst->m_pData)
		{
		TRACE("BLiT: NULL data - possible bad lock.\n");
		return FALSE;
		}


	// Right now, pDst refers to the CLIPPED start of the scanline:
	uint8_t*	pDstLine = pimDst->m_pData + sDstX + sDstY * pimDst->m_lPitch;
	uint8_t*	pDst;

	int16_t	sY=0;
	RSpecialFSPR8* pInfo = (RSpecialFSPR8*) pimSrc->m_pSpecial;

	uint8_t	ucCode;
	uint8_t*	pSrc;
	uint8_t*	pCB;
	int16_t sClipWidth;

	//
	if (sSrcX > 0)	// LClip Situation! => The slowest!
		{
		//**** THE WORST OF ALL CASES!  TWO SIDED CLIPPING! ****
		if (sW < (pimSrc->m_sWidth - sSrcX) )
			{
			for (sY = sSrcY; sY < sSrcY + sH; sY++,pDstLine += lDstP)
				{	// Clip off left side:
				pSrc = pInfo->m_pBufArry[sY];
				pDst = pDstLine;
				pCB = pInfo->m_pCodeArry[sY];
				sClipWidth = sW; //sSrcX; // initial source...
				int16_t sLeftClip = sSrcX;

			NextSkip2S:
				//==============================================================	
				// 1) Skip a clear run?
				ucCode = *(pCB++); // Get skip length
				if (ucCode == 255) continue; // End of line code

				// Don't update pDst until you are done skipping!
				if ((sLeftClip -= ucCode) < 0) 
					{
					pDst -= sLeftClip; // NOW advance pDst the remainder of the skip
					sClipWidth += sLeftClip; // and REDUCE what's left by remainder of skip!
					if (sClipWidth <= 0)	// done line already?
						{
						// must jump past rest of code line
						do	{
							ucCode = *(pCB++); // opaque run
							ucCode = *(pCB++); // clear run
							} while (ucCode != 255);

						continue; // start next line!
						}

					goto NormalOpaque2S;// We're done being clipped
					}

				//==============================================================	
				// 2) Skip an opaque run?
				ucCode = *(pCB++); // Get skip length

				if (ucCode < sLeftClip) 
					{
					sLeftClip -= ucCode; // Adance both pointers, but don't copy!
					pSrc += ucCode;		// Always need to advance source in an opaque run
					//pDst += ucCode; // don't move the pDst until done clipping...
					}
				else	// We've broken out of the Clipped region!
					{			// Do a partial opaque run:
					pSrc += sLeftClip; 			// We DO need to advance the source!
					//pDst += sClipWidth;				// and the Dest for the clipped part!
					ucCode -= sLeftClip;			// Remaining part of run is opaque
					
					if (ucCode > sClipWidth) // a Contracted Run
						{
						ucCode = (uint8_t)sClipWidth;
						sClipWidth = 0;
						}
					else	sClipWidth -= ucCode; // a full opaque run
					
					if (ucCode)	// Do an opaque run
						{
						do	*(pDst++) = *(pSrc++);	// Copy pixel
						while (--ucCode);	
						}
					
					if (sClipWidth) goto NormalClear2S;	
					else continue; // next code line!
					}
					
				goto NextSkip2S;	// Still in the clip region
				
			NormalClear2S:
				//==============================================================	
				// 1) Do a clear run?
				ucCode = *(pCB++); // Get skip length
				if (ucCode == 255) continue; // End of line code

				if ( (sClipWidth -= ucCode) <= 0) continue; // Initial skip is clipped out!
				
				pDst += ucCode;
				//==============================================================	
				// 2) Do an opaque run?
			NormalOpaque2S:
				ucCode = *(pCB++);		// Get run length
				if (ucCode > sClipWidth) // a Contracted Run
					{
					ucCode = (uint8_t)sClipWidth;
					sClipWidth = 0;
					}
				else	sClipWidth -= ucCode; // a full opaque run
				
				if (ucCode)	// Do an opaque run
					{
					do	*(pDst++) = *(pSrc++);	// Copy pixel
					while (--ucCode);	
					}
				
				if (sClipWidth) goto NormalClear2S;	
				else continue; // next code line!
				// Do next line
				}
			}
		else // This is the Left Clip ONLY case!
			{
			for (sY = sSrcY; sY < sSrcY + sH; sY++,pDstLine += lDstP)
				{	// Clip off left side:
				pSrc = pInfo->m_pBufArry[sY];
				pDst = pDstLine;
				pCB = pInfo->m_pCodeArry[sY];
				sClipWidth = sSrcX; // initial source...

			NextSkip:
				//==============================================================	
				// 1) Skip a clear run?
				ucCode = *(pCB++); // Get skip length
				if (ucCode == 255) continue; // End of line code

				// Don't update pDst until you are done skipping!
				// pDst += ucCode;
				if ((sClipWidth -= ucCode) < 0) 
					{
					pDst -= sClipWidth; // NOW advance pDst the remainder of the skip
					goto NormalOpaque;// We're done being clipped
					}

				//==============================================================	
				// 2) Skip an opaque run?
				ucCode = *(pCB++); // Get skip length

				if (ucCode < sClipWidth) 
					{
					sClipWidth -= ucCode; // Adance both pointers, but don't copy!
					pSrc += ucCode;		// Always need to advance source in an opaque run
					//pDst += ucCode; // don't move the pDst until done clipping...
					}
				else	// We've broken out of the Clipped region!
					{			// Do a partial opaque run:
					pSrc += sClipWidth; 			// We DO need to advance the source!
					//pDst += sClipWidth;				// and the Dest for the clipped part!
					ucCode -= sClipWidth;			// Remaining part of run is opaque
					if (!ucCode) goto NormalClear;
					do	*(pDst++) = *(pSrc++);	// Should be an unrolled loop
					while	(--ucCode);
						
					goto NormalClear; // continue
					}
					
				goto NextSkip;	// Still in the clip region
				
			NormalClear:
				//==============================================================	
				// 1) Do a clear run?
				ucCode = *(pCB++); // Get skip length
				if (ucCode == 255) continue; // End of line code

				pDst += ucCode;
				//==============================================================	
				// 2) Do an opaque run?
			NormalOpaque:
				ucCode = *(pCB++); // Get skip length
				
				if (!ucCode) goto NormalClear;
				do	*(pDst++) = *(pSrc++);	// Should be an unrolled loop
				while	(--ucCode);
				
				goto NormalClear;	
				// Do next line
				}
			}
		}
	else 
		if (sW < (pimSrc->m_sWidth - sSrcX) )
			for (sY = sSrcY; sY < sSrcY + sH; sY++,pDstLine += lDstP)
				{	
				// Clip Off Right Side:
				pSrc = pInfo->m_pBufArry[sY];
				pDst = pDstLine;
				pCB = pInfo->m_pCodeArry[sY];
				sClipWidth = sW;
				
			NextCodeRC:	// Go until used up sClipWidth
				//==============================================================	
				// 1) Do the clear run:
				ucCode = *(pCB++);	// Get Initial Skip Length
				if (ucCode == 255) continue; // End of line code
				
				if ( (sClipWidth -= ucCode) <= 0) continue; // Initial skip is clipped out!
				
				pDst += ucCode; // Advace destination
				//==============================================================	
				// 2) Do the opaque run:
				ucCode = *(pCB++);		// Get run length
				if (ucCode > sClipWidth) // a Contracted Run
					{
					ucCode = (uint8_t)sClipWidth;
					sClipWidth = 0;
					}
				else	sClipWidth -= ucCode; // a full opaque run
				
				if (ucCode)	// Do an opaque run
					{
					do	*(pDst++) = *(pSrc++);	// Copy pixel
					while (--ucCode);	
					}
				
				if (sClipWidth) goto NextCodeRC; // any space left?
				// Do next line!
				}	
			else	// do an unclipped BLiT
				{
				for (sY = sSrcY; sY < sSrcY + sH; sY++,pDstLine += lDstP)
					{
					pSrc = pInfo->m_pBufArry[sY];
					pDst = pDstLine;
					pCB = pInfo->m_pCodeArry[sY];

				NextCode:
					//==============================================================	
					// 1) Do the clear run:
					ucCode = *(pCB)++;	// Get Skip Length
					if (ucCode == 255) continue;// End of line code
					pDst += ucCode;		// Skip
					
					//==============================================================	
					// 2) Do the opaque run:
					ucCode = *(pCB)++;			// Get Skip Length
					if (!ucCode) goto NextCode;
					
					// Below loop is 15% of our CPU time on OSX,
					//  but MacOSX ships with a _very_ optimized memcpy()...
					//  (actually, I bet that's true everywhere vs this shitty loop.  --ryan.)
					#if 1  //PLATFORM_MACOSX
					memcpy(pDst, pSrc, ucCode);
					pDst += ucCode;
					pSrc += ucCode;
					#else
					do	*(pDst++) = *(pSrc++);	// Copy pixel
					while (--ucCode);	
					#endif

					goto NextCode;
					}
				}

	//*******************************************************************
	// IN RELEASE MODE, GIVE THE USER A CHANCE:
#ifndef _DEBUG

	//if (gsScreenLocked || gsBufferLocked) goto BLIT_DONTUNLOCK;

#endif

	//********************
	// OS_SPECIFIC:
	//********************  UNLOCK WHATEVER YOU NEED TO
	switch (sNeedToUnlock)
		{
		case 0: // nothing to unlock!
		break;

		case BUF_MEMORY:
	//		rspUnlockBuffer();
		break;
		
		case BUF_VRAM:
	//		rspUnlockScreen();
		break;
		
		case BUF_VRAM2:
			rspUnlockVideoFlipPage();
		break;
		
		default:
			TRACE("BLiT:  Unlocking error!\n");
		}

//BLIT_DONTUNLOCK:	
	return 0;
	}

//------------------------------------------------------------------------
