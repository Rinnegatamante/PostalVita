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
// MULTIGRIDINDIRECT.H
//
// Created on	05/01/97 JRD
// Implemented	05/01/97 JRD
//
//	History:
//
//
//////////////////////////////////////////////////////////////////////

#ifndef MULTIGRIDINDIRECT_H
#define MULTIGRIDINDIRECT_H

#include "System.h"

#ifdef PATHS_IN_INCLUDES
	#include "BLUE/Blue.h"
	#include "ORANGE/File/file.h"
	#include "GREEN/Image/Image.h" // For Debugging only
	#include "GREEN/BLiT/BLIT.H"
	#include "ORANGE/MultiGrid/MultiGrid.h"
#else
	#include "Blue.h"
	#include "file.h"
	#include "Image.h"
	#include "multigrid.h"
	#include "BLIT.H"
#endif // PATHS_IN_INCLUDES
 
 #define MGI_COOKIE "_MultiGridIndirect_"
 #define MGI_CURRENT_VERSION 1
 #define	MGI_MAX_PLANES	15 // to work wwith multigrid 15-bit
// NOTE: zero is NOT a valid palette value, but it IS a valid attribute value

//////////////////////////////////////////////////////////////////////
//
// CURRENT CONSTRAINTS (05/01/97)
//
// - Supports a maximum of 8 planes (bits)
// - Supports a maximum of 8 palette entries per tile.
// - Currently, the palette entries are PLANE based,
//   so a "hit" can be on all colors at once.
//
//
// PLANNED ENHANCEMENTS
//
// - Template support for data of any type
// - Switching between plane mode and linear mode
// - Pascal Strings for data
// - Higher speed access
//
//////////////////////////////////////////////////////////////////////
//
// TUNING PARAMETERS
//
// By turning off clipping, access speed is greatly enhanced
//
#define	MGI_CLIP	TRUE // undefine to turn off clip to world...
//
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
//
// RMultiGridIndirect class
//
// This class provides higher compression than the standard 
// RMultigrid by adding one layer of indirection.  It uses an 
// RMultigrid to look up a given 15-bit value.  Then the lowest
// 8 bits are viewed as 8 possible values from 0-256, which are 
// then looked up in the local "palette" table in the coarse grid.
//
// This is not as fast as RMultigrid, and should only be used in
// limited amounts.  Currently, the palette grid does NOT need to
// line up in any way with the RMultigrid.
//
// Main data chuncks are the RMultiGrid, and a 2D palette grid, with
// each entry currently 64-bits long (corresponding to 8 bytes)
//
//////////////////////////////////////////////////////////////////////

class	RMultiGridIndirect
	{
public:
	//////////////////////////////////////////////////////////////////////
	//  User members:
	//////////////////////////////////////////////////////////////////////

	int16_t	m_sWidth;	// With compression, you might get huge objects!
	int16_t	m_sHeight;
	int16_t	m_sGridW;
	int16_t m_sGridH;
	int16_t m_sTileW;
	int16_t m_sTileH;

	//////////////////////////////////////////////////////////////////////
	//  User methods:
	//////////////////////////////////////////////////////////////////////

	// Due to the planar nature of the indirection, all values
	// may be present at the same time.  Currently, you supply a
	// uint8_t array that is at least m_sMaxPlanes+1 in size.  You
	// will recieve a NULL terminated string of dereferenced
	// palette hits for that point.
	//
	int16_t	GetVal(uint8_t*	pszResult,int16_t sX, int16_t sY,char ucClipVal = -1)
		{
		//-----------------------------------------------------------------
		ASSERT(m_pmg);
		ASSERT(m_pucPalette);
		ASSERT(m_pmg->m_sIsCompressed);
		// 1) Get the set of index planes:
		uint16_t usList = uint16_t(m_pmg->GetVal(sX,sY,-1));
		// Check each plane individually
		for (int16_t i=0;i < m_sMaxPlanes;i++)
			{
			if (usList & ms_asColorToPlane[i]) 
				*pszResult++ = *(GetPalette(sX,sY)+i); // look into clipping:
			}

		*pszResult = 0;
		return NULL;
		}

	// Load a compressed data set from disk
	//
	int16_t Load(RFile* prFile);
	//////////////////////////////////////////////////////////////////////

	// Save a compressed data set to disk
	//
	int16_t Save(RFile* prFile);
	
	//////////////////////////////////////////////////////////////////////
	// These user methods are for initially creating the 2d data
	//////////////////////////////////////////////////////////////////////

	// Returns SUCCESS or FAILURE
	// Sets up the UNCOMPRESSED data
	//
	int16_t	Alloc(int16_t sW, int16_t sH, int16_t sMaxPlanes, int16_t sTileW, int16_t sTileH);

	//////////////////////////////////////////////////////////////////////
	//	GetPalette (entire palette for that tile)
	//////////////////////////////////////////////////////////////////////
	// Low level palette access
	// Just get the current byte array of MGI_MAX_PLANES size:
	//
	uint8_t*	GetPalette(int16_t sX,int16_t sY, uint8_t* pucOnError = NULL)
		{
		ASSERT(m_pucPalette);

	#ifdef	MGI_CLIP
		if ( (sX < 0) || (sY < 0) || (sX >= m_sWidth) || (sY >= m_sHeight) ) 
			return pucOnError;
	#endif

		return m_ppucAccessY[sY] + m_plAccessX[sX];
		}

	// This gives you the number of taken palette entries in a tile:
	int16_t	NumPalEntries(int16_t sX,int16_t sY)
		{
		int16_t sNumEntries = m_sMaxPlanes; // A Full Palette
		uint8_t*	pPal = GetPalette(sX,sY) + m_sMaxPlanes;

		while (sNumEntries && (!*--pPal)) sNumEntries--;

		return sNumEntries;
		}

	//////////////////////////////////////////////////////////////////////
	//  "Install" the RMultiGrid which holds the index data:
	// (This must not be compressed until it is done being written to)
	//////////////////////////////////////////////////////////////////////
	int16_t	InstallMultiGrid(RMultiGrid* pmg)
		{
		ASSERT(!m_pmg);
		ASSERT(pmg);
		ASSERT( (pmg->m_sWidth == m_sWidth) && (pmg->m_sHeight == m_sHeight));
		ASSERT(!pmg->m_sIsCompressed);

		m_pmg = pmg;

		return SUCCESS;
		}

	//////////////////////////////////////////////////////////////////////
	//	SetPalette (Set an entry in that tile's palette)
	//////////////////////////////////////////////////////////////////////
	// Low level palette access
	// Just set the current byte palette entry
	//
	int16_t	SetPalette(int16_t sX,int16_t sY,int16_t sEntry,uint8_t ucVal)
		{
		ASSERT(m_pucPalette);

	#ifdef	MGI_CLIP
		if ( (sX < 0) || (sY < 0) || (sX >= m_sWidth) || (sY >= m_sHeight) ) 
			return -1;
	#endif

		*(m_ppucAccessY[sY] + m_plAccessX[sX] + sEntry) = ucVal;

		return SUCCESS;
		}


	// A visual Debug View: (Uncompressed)
	//
	void	DumpPalette(RImage* pimDst,int16_t sSrcX,int16_t sSrcY,int16_t sDstX,int16_t sDstY,
					int16_t sW,int16_t sH);

	//////////////////////////////////////////////////////////////////////
	//  Internal methods:
	//////////////////////////////////////////////////////////////////////

	void	Clear()	
		{
		m_sWidth = m_sHeight = m_sGridW = m_sGridH = m_sTileW = m_sTileH = 0;
		m_pmg = NULL;
		m_pucPalette = NULL;
		m_ppucAccessY = NULL;
		m_plAccessX = NULL;
		m_pimTempTile = NULL;
		m_lTileLen = 0;
		m_pimBuffer = NULL;
		}

	void	Free()
		{
		//if (m_pmg) delete m_pmg;// Let user delete MultiGrid
		if (m_pucPalette) free(m_pucPalette);
		if (m_ppucAccessY) free (m_ppucAccessY);
		if (m_plAccessX) free(m_plAccessX);
		if (m_pimTempTile) delete m_pimTempTile;
		if (m_pimBuffer) delete m_pimBuffer;
		if (m_pmg) delete m_pmg;
		Clear();
		}

	RMultiGridIndirect()	
		{
		Clear();
		}

	~RMultiGridIndirect()
		{
		Free();
		}

	/////////////////////////////////////////////////////////////////////
	//  Some of the following functions operate on both RMultiAlphas
	//  AND the palette grid.  Here we leave the realm of pure Orange,
	//  and the realm of general functions.  Once general use of the
	//  IndirectMultigrid is understood, this should be separated out.
	/////////////////////////////////////////////////////////////////////

	// This clear the temp tile buffer first to allow the possibility of
	// source clipping:  (It does an opaque copy)
	void	CacheTile(RImage* pimSrc,int16_t sSrcX,int16_t sSrcY)
		{
		// 1) clear buffer tile:
		memset(m_pimTempTile->m_pData,0,m_lTileLen);
		// 2) do a fully clipping opaque copy into the temp tile:
        RRect rrect(0,0,pimSrc->m_sWidth,pimSrc->m_sHeight);
		rspBlit(pimSrc,m_pimTempTile,sSrcX,sSrcY,0,0,
			m_sTileW,m_sTileH,NULL,&rrect);
		}

	// This is a linear check to see if you have a null cache:
	int16_t	Contains(uint8_t ucColor)
		{
		uint8_t *pTile = m_pimTempTile->m_pData;
		for (int16_t i=0; i < m_lTileLen; i++,pTile++)
			if (*pTile == ucColor) return TRUE;
		return FALSE;
		}

	// This is a highly specialized OR blit into the uncompressed
	// attribute map!  It goes from an 8-bit source key to a 15-bit
	// destination key:
	// It will only do RIGHT AND BOTTOM CLIPPING!
	//
	void	TileOR(uint8_t ucKey,uint16_t usValueOR,int16_t sDstX,int16_t sDstY,
		int16_t sClip = 0); // you can turn off the half clipping:

	// Drop an FSPR1 into the Attribute map:
	//
	int16_t AddFSPR1(RImage* pimSrc,int16_t sX,int16_t sY,uint8_t ucVal,
					int16_t sMaxW=0,int16_t sMaxH=0);


public:
	//////////////////////////////////////////////////////////////////////
	//  Data Structures
	//////////////////////////////////////////////////////////////////////
	RMultiGrid*	m_pmg;		// The MultiGrid inherent under the system
	uint8_t*	m_pucPalette;	// Data holding the palette entries
	uint8_t**	m_ppucAccessY;	// Array of (GridH) pointers into the Palette
	uint32_t*	m_plAccessX;	// Array of offsets into the Palette
	int16_t	m_sMaxPlanes;		// Can be custom tailored to save memory
	RImage*	m_pimTempTile;	// For construction & easy writing.
	RImage*	m_pimBuffer;	// Large conversion buffer
	int32_t	m_lTileLen;			// For linear operations
	static int16_t	ms_asColorToPlane[MGI_MAX_PLANES];
	};


#endif //MULTIGRIDINDIRECT_H

//////////////////////////////////////////////////////////////////////
// EOF
//////////////////////////////////////////////////////////////////////
