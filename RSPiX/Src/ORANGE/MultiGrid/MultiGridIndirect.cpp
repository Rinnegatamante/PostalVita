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
//////////////////////////////////////////////////////////////////////
//
// MULTIGRIDINDIRECT.CPP 
//
// Created on	05/01/97 JRD
// Implemented	05/01/97 JRD
//
//		05/08/97	JMI	Added #include <string.h> for strcmp*().  I guess
//							that, in VC <= 4.1, the strcmp*() protos are not
//							in stdlib.h.
//
//		06/28/97 MJR	Minor changes to get it working on the mac.
//
//////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <string.h>
#include "System.h"	

#ifdef PATHS_IN_INCLUDES
	#include "ORANGE/MultiGrid/MultiGrid.h"
	#include "ORANGE/MultiGrid/MultiGridIndirect.h"
#else
	#include "MULTIGRID.H"
	#include "MULTIGRIDINDIRECT.H"
#endif	//PATHS_IN_INCLUDES

//////////////////////////////////////////////////////////////////////
//
// CURRENT CONSTRAINTS (03/23/97)
//
// - Only one level of grid hierarchy
// - Supports only 15-bit data
// - Coarse Grid scale must be in powers of two from (2 - 16384)
// - Supports only 8-bit palette values
// - Supports a maximum of four planes (16 tables)
//
// BACKWARDS COMPATIBILITY (03/23/97)
//
// - will try to still load conventional RMultiGrids
//
// PLANNED ENHANCEMENTS
//
// - Template support for data of any type
// - Multiple hierarchical levels
// - Disjoint grids (hierarchy only where detail is needed)
//
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
//
// RMultiGridIndirect class
//
// This class provides efficient, high speed compression of a 2d data
// field in a way that is transparent to the user.  It supports load,
// save, compress, and decompress within the class to facilitate
// utilities.
//
// It compresses by breaking a 2d data field into a coarse grid, and
// attempts to compress the data by 1) replicating tiles wherever
// possible, and 2) describing blocks which are all one value by a 
// single value, like a 2d run length encoding.
//
// Then, as further compression compared with MultiGrids, it then
// creates a local palette for each coarse grid, so the values
// within each grid stay small.
//
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////

// Produce a valid OR mask to se a plane number
// (ZERO IS A VALID PLANE NUMBER)
int16_t	RMultiGridIndirect::ms_asColorToPlane[MGI_MAX_PLANES] = 
	{1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384};
//  0 1 2 3  4  5  6   7   8   9   10   11   12   13    14
// PLANE NUMBER ^ ^ ^ ^ ^

//////////////////////////////////////////////////////////////////////
//  Internal Support Methods:
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
//  User Methods:
//////////////////////////////////////////////////////////////////////

// For Debugging
void	RMultiGridIndirect::DumpPalette(RImage* pimDst,int16_t sSrcX,int16_t sSrcY,int16_t sDstX,int16_t sDstY,
					int16_t sW,int16_t sH)
	{
	//short i,j;

	}


// Low level alloc, JUST of the palette grid.
// Width and height should match that of the RMultiGrid
int16_t	RMultiGridIndirect::Alloc(int16_t sW, int16_t sH, int16_t sMaxPlanes, 
										  int16_t sTileW, int16_t sTileH)
	{
	ASSERT(!m_pucPalette);
	ASSERT(!m_ppucAccessY);
	ASSERT(!m_plAccessX);
	ASSERT(sW > 1);
	ASSERT(sH > 1);
	ASSERT((sMaxPlanes > 0) && (sMaxPlanes <= MGI_MAX_PLANES));

	int16_t sGridW = (sW / sTileW) + 1;
	int16_t sGridH = (sH / sTileH) + 1;

	m_pucPalette = (uint8_t*) calloc(sMaxPlanes,int32_t(sGridW) * sGridH);
	ASSERT(m_pucPalette);
	m_plAccessX = (uint32_t*) calloc(sizeof(int32_t),sGridW * sTileW);
	ASSERT(m_plAccessX);
	m_ppucAccessY = (uint8_t**) calloc( sizeof(uint8_t*),int32_t(sGridH)*sTileH );
	ASSERT(m_ppucAccessY);

	// Populate the pointer list:
	uint8_t*	pY = m_pucPalette;
	int32_t	lOffset = int32_t(sGridW)*sMaxPlanes;
	int16_t i,j;

	for (i=0,j=0;i<sGridH;pY += lOffset,i++)
		{
		for (int16_t k=0;k < sTileH;k++)
			{
			m_ppucAccessY[j++] = pY;
			}
		}

	int32_t lX = 0;
	for (i=0,j=0;i<sGridW;lX += sMaxPlanes,i++) 
		{
		for (int16_t k=0;k < sTileW;k++)
			{
			m_plAccessX[j++] = lX;
			}
		}

	m_sWidth = sW;
	m_sHeight = sH;
	m_sTileW = sTileW;
	m_sTileH = sTileH;
	m_sGridW = sGridW;
	m_sGridH = sGridH;
	m_sMaxPlanes = sMaxPlanes;

	m_pimTempTile = new RImage;
	// Keep pitch equal to width:
	m_pimTempTile->CreateImage(m_sTileW,m_sTileH,RImage::BMP8,m_sTileW);
	m_lTileLen = int32_t(m_sTileW) * m_sTileH;

	return SUCCESS;
	}

/////////////////////////////////////////////////////////////////////
//  Some of the following functions operate on both RMultiAlphas
//  AND the palette grid.
/////////////////////////////////////////////////////////////////////

// This is at the pure app level.  How it could be a valid part of
// an orange function is unclear:
//

int16_t RMultiGridIndirect::AddFSPR1(RImage* pimSrc,int16_t sLogX,int16_t sLogY,
					uint8_t ucVal,int16_t sMaxW,int16_t sMaxH)
	{
	ASSERT(pimSrc);
	ASSERT(pimSrc->m_type == RImage::FSPR1);
	int16_t sRet = SUCCESS;

	// Note that the FSPR1 can't clip, so if the buffer size is wrong
	// it won't copy over.

	if (m_pimBuffer && (sMaxW || sMaxH))
		{
		if ( (m_pimBuffer->m_sWidth != sMaxW) || 
					(m_pimBuffer->m_sHeight != sMaxH))
			{
			delete m_pimBuffer;
			m_pimBuffer = NULL;
			}
		}

	if (!m_pimBuffer) 
		{
		m_pimBuffer = new RImage;
		m_pimBuffer->CreateImage(sMaxW,sMaxH,RImage::BMP8);
		}
	//--------------------------------------------
	// 1) clear the buffer
	rspRect(uint8_t(0),m_pimBuffer,0,0,m_pimBuffer->m_sWidth,
		m_pimBuffer->m_sHeight);

	// 2) copy in the region to be tiled
	rspBlit(uint8_t(1),pimSrc,m_pimBuffer,0,0);

	int16_t sX,sY,sX2,sY2;

	// IMPORTANT STEP 2.5!  I cannot guarantee that the chosen
	// region lies within my overall world, so I must clip it!
	// 2.5) Clip the requested region to the real region:
	int16_t sClipW = m_pimBuffer->m_sWidth;
	int16_t sClipH = m_pimBuffer->m_sHeight;
	int16_t sClip;

	sClip = -sLogX;
	if (sClip > 0) { sLogX += sClip; sClipW -= sClip; }
	sClip = -sLogY;
	if (sClip > 0) { sLogY += sClip; sClipH -= sClip; }
	sClip = sLogX + sClipW - m_sWidth;
	if (sClip > 0) { sClipW -= sClip; }
	sClip = sLogY + sClipH - m_sHeight;
	if (sClip > 0) { sClipH -= sClip; }

	if (!sClipW && !sClipH)
		{
		TRACE("RMultiGridIndirect::AddFSPR1: WARNING: added region clipped out.\n");
		return SUCCESS;
		}

	// 3) calculate the logical tile rects covered by the region:
	//*************************************************** Use DIVMODS!
	//************************** BETTER YET, USE POWERS OF 2!

	sX = sLogX & ~(m_sTileW-1); // look for powers of two!
	sY = sLogY & ~(m_sTileH-1);
	sX2 = (sLogX + sClipW + m_sTileW - 2) & ~(m_sTileW-1);
	sY2 = (sLogY + sClipH + m_sTileH - 2) & ~(m_sTileH-1);

	int16_t sTileX,sTileY,sTileW,sTileH;
	sTileX = sX / m_sTileW;
	sTileY = sY / m_sTileH;
	sTileW = (sX2 - sX + 1) / m_sTileW;
	sTileH = (sY2 - sY + 1) / m_sTileH;

	// Now process each tile individually!
	int16_t i,j;
	int16_t sCurX,sCurY = sY;

	for (j = 0; j < sTileH; j++,sCurY += m_sTileH)
		{
		sCurX = sX;
		for (i = 0;i < sTileW; i++,sCurX += m_sTileW)
			{
			//===================================================
			// Add a tile into the attribute layer!
			//===================================================
			
			// 1) Copy the tile square into the general Tile buffer:
			CacheTile(m_pimBuffer,sCurX - sLogX,sCurY - sLogY);

			// 2) if nontrivial, determine the index color
			if (Contains(1))
				{
				int16_t sIndex = NumPalEntries(sCurX,sCurY);
				ASSERT(sIndex < m_sMaxPlanes); // overflow
				if (sIndex >= m_sMaxPlanes)
					{
					TRACE("RMultiGridIndirect::AddFSPR1: "
						"Exceeded %hd overlapping regions at (%hd,%hd)\n",
						m_sMaxPlanes,sCurX,sCurY);

					sRet = FAILURE;
					}
				else
					{
					// 3) Insert the palette entry and choose a plane:
					SetPalette(sCurX,sCurY,sIndex,ucVal);
					int16_t sPlaneVal = ms_asColorToPlane[sIndex];

					// 4) Draw the tile into the attribute map:
					TileOR(uint8_t(1),uint16_t(sPlaneVal),sCurX,sCurY,1);
					}
				}
			//===================================================
			}
		}


	return sRet;
	}


	void	RMultiGridIndirect::TileOR(uint8_t ucKey,uint16_t usValueOR,int16_t sDstX,int16_t sDstY,
		int16_t sClip) // you can turn off the half clipping:
		{
		ASSERT(m_pmg);
		ASSERT(m_pmg->m_psGrid);
		ASSERT(!m_pmg->m_sIsCompressed);
		ASSERT(usValueOR < 32768);
		//-------------- half clipping ------------
		uint16_t* pusAttrib = (uint16_t*) m_pmg->m_psGrid;
		int16_t sW = m_sTileW,sH = m_sTileH;

		if (sClip)
			{
			int16_t sClip;
			sClip = m_pmg->m_sWidth - sDstX - m_sTileW;
			if (sClip < 0) sW += sClip;
			sClip = m_pmg->m_sHeight - sDstY - m_sTileH;
			if (sClip < 0) sH += sClip;

			if ( (sW < 1) || (sH < 1) ) return; // clipped out
			}

		int32_t	lDstP = m_pmg->m_sWidth; // (in shorts)
		int32_t	lSrcP = m_sTileW;
		uint8_t*	pSrc,*pSrcLine = m_pimTempTile->m_pData;
		uint16_t*  pDst,*pDstLine = (uint16_t*)m_pmg->m_psGrid;

		// Adjust for actual coordinates!!!!
		pDstLine += int32_t(m_pmg->m_sWidth) * sDstY + sDstX;
		int16_t	i,j;

		for (j=0;j < sH;j++,pSrcLine += lSrcP,pDstLine += lDstP)
			{
			pDst = pDstLine;
			pSrc = pSrcLine;
			for (i=0;i < sW;i++,pSrc++,pDst++)
				{
				if (*pSrc == ucKey) 
					*pDst |= usValueOR;
				}
			}
		}

// Load a compressed data set from disk
//
int16_t RMultiGridIndirect::Load(RFile* fp)
	{
	ASSERT(!m_pmg);
	ASSERT(!m_pucPalette);

	int16_t sVer;
	char type[256];
	fp->Read(type);

	if (strcmp(type,"__MultiGridIndirect__"))
		{
		TRACE("RMultiGridIndirect::Load: Not a MGI!\n");
		return FAILURE;
		}

	fp->Read(&sVer);
	if (sVer != 1)
		{
		TRACE("RMultiGridIndirect::Load: Can't load MGI version %hd!\n",
			sVer);
		return FAILURE;
		}

	// Assume we have an initialized instance here:

	fp->Read(&m_sWidth);
	fp->Read(&m_sHeight);
	fp->Read(&m_sGridW);
	fp->Read(&m_sGridH);
	fp->Read(&m_sTileW);
	fp->Read(&m_sTileH);

	fp->Read(&m_sMaxPlanes);

	int32_t lRes = 0;
	fp->Read(&lRes);
	fp->Read(&lRes);
	fp->Read(&lRes);
	fp->Read(&lRes);

	// just the palette data:
	int32_t lLen = int32_t(m_sGridW) * m_sGridH * m_sMaxPlanes;
	m_pucPalette = (uint8_t*) calloc(1,lLen);

	fp->Read(m_pucPalette,lLen);

	// And send out the sttribute map:
	m_pmg = new RMultiGrid;

	if (m_pmg->Load(fp)!=SUCCESS)
		{
		TRACE("RMultiGridIndirect::Load: Couldn't Load Multigrid!\n");
		return FAILURE;
		}

	// Need to hook up all the access variables!
	m_plAccessX = (uint32_t*) calloc(sizeof(int32_t),m_sGridW * m_sTileW);
	ASSERT(m_plAccessX);
	m_ppucAccessY = (uint8_t**) calloc( sizeof(uint8_t*),int32_t(m_sGridH)*m_sTileH );
	ASSERT(m_ppucAccessY);

	// Populate the pointer list:
	uint8_t*	pY = m_pucPalette;
	int32_t	lOffset = int32_t(m_sGridW)*m_sMaxPlanes;

	int16_t i,j;
	for (i=0,j=0;i<m_sGridH;pY += lOffset,i++)
		{
		for (int16_t k=0;k < m_sTileH;k++)
			{
			m_ppucAccessY[j++] = pY;
			}
		}

	int32_t lX = 0;
	for (i=0,j=0;i<m_sGridW;lX += m_sMaxPlanes,i++) 
		{
		for (int16_t k=0;k < m_sTileW;k++)
			{
			m_plAccessX[j++] = lX;
			}
		}

	m_pimTempTile = new RImage;
	// Keep pitch equal to width:
	m_pimTempTile->CreateImage(m_sTileW,m_sTileH,RImage::BMP8,m_sTileW);
	m_lTileLen = int32_t(m_sTileW) * m_sTileH;

	return SUCCESS;
	}

//////////////////////////////////////////////////////////////////////

// Save a compressed data set to disk
//
int16_t RMultiGridIndirect::Save(RFile* fp)
	{
	ASSERT(m_pmg);
	ASSERT(m_pmg->m_sIsCompressed);
	ASSERT(m_pucPalette);

	int16_t sVer = 1;
	fp->Write("__MultiGridIndirect__");
	fp->Write(sVer);

	fp->Write(m_sWidth);
	fp->Write(m_sHeight);
	fp->Write(m_sGridW);
	fp->Write(m_sGridH);
	fp->Write(m_sTileW);
	fp->Write(m_sTileH);

	fp->Write(m_sMaxPlanes);

	int32_t lRes = 0;
	fp->Write(lRes);
	fp->Write(lRes);
	fp->Write(lRes);
	fp->Write(lRes);

	// just the palette data:
	int32_t lLen = int32_t(m_sGridW) * m_sGridH * m_sMaxPlanes;
	fp->Write(m_pucPalette,lLen);

	// And send out the sttribute map:
	m_pmg->Save(fp);

	return SUCCESS;
	}
