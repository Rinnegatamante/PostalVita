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
// goaltimer.h
// Project: Postal
//
// History:
//		
//	06/30/97 BRH	Started this file for the challenge levels.  It will
//					keep track of the goal time and be able to count up
//					or down.  It will set the bRealmOver flag when the goal
//					is met.
//
////////////////////////////////////////////////////////////////////////////////
#ifndef GOALTIMER_H
#define GOALTIMER_H

#include "RSPiX.h"
#include "realm.h"

// CGoalTimer keeps track of the time and goal for a challenge level
class CGoalTimer : public CThing
	{
	//---------------------------------------------------------------------------
	// Types, enums, etc.
	//---------------------------------------------------------------------------
	public:

	//---------------------------------------------------------------------------
	// Variables
	//---------------------------------------------------------------------------
	public:

	protected:
		double m_dX;												// x coord
		double m_dY;												// y coord
		double m_dZ;												// z coord
		RImage* m_pImage;											// Pointer to only image (replace with 3d anim, soon)
		CSprite2 m_sprite;										// Sprite (replace with CSprite3, soon)
		int32_t		m_lTimerMS;										// Timer value in Milliseconds
		int16_t		m_sKillGoal;									// Number of kills to achieve
		int16_t		m_sUpDown;										// Up or down timer

		int16_t m_sSuspend;											// Suspend flag

		// Tracks file counter so we know when to load/save "common" data 
		static int16_t ms_sFileCount;

		// "Constant" values that we want to be able to tune using the editor

	//---------------------------------------------------------------------------
	// Constructor(s) / destructor
	//---------------------------------------------------------------------------
	protected:
		// Constructor
		CGoalTimer(CRealm* pRealm)
			: CThing(pRealm, CGoalTimerID)
		{
			m_pImage = 0;
			m_sSuspend = 0;
			m_sUpDown = 1;
			m_sKillGoal = 0;
			m_lTimerMS = 120000;
		}

	public:
		// Destructor
		~CGoalTimer()
		{
			// Remove sprite from scene (this is safe even if it was already removed!)
			m_pRealm->m_scene.RemoveSprite(&m_sprite);

			// Free resources
			FreeResources();
		}

	//---------------------------------------------------------------------------
	// Required static functions
	//---------------------------------------------------------------------------
	public:
		// Construct object
		static int16_t Construct(									// Returns 0 if successfull, non-zero otherwise
			CRealm* pRealm,										// In:  Pointer to realm this object belongs to
			CThing** ppNew)										// Out: Pointer to new object
		{
			int16_t sResult = 0;
			*ppNew = new CGoalTimer(pRealm);
			if (*ppNew == 0)
			{
				sResult = -1;
				TRACE("CGoalTimer::Construct(): Couldn't construct CGoalTimer (that's a bad thing)\n");
			}
			return sResult;
		}

	//---------------------------------------------------------------------------
	// Required virtual functions (implimenting them as inlines doesn't pay!)
	//---------------------------------------------------------------------------
	public:
		// Load object (should call base class version!)
		int16_t Load(													// Returns 0 if successfull, non-zero otherwise
			RFile* pFile,											// In:  File to load from
			bool bEditMode,										// In:  True for edit mode, false otherwise
			int16_t sFileCount,										// In:  File count (unique per file, never 0)
			uint32_t	ulFileVersion);								// In:  Version of file format to load.

		// Save object (should call base class version!)
		int16_t Save(													// Returns 0 if successfull, non-zero otherwise
			RFile* pFile,											// In:  File to save to
			int16_t sFileCount);									// In:  File count (unique per file, never 0)

		// Startup object
		int16_t Startup(void);										// Returns 0 if successfull, non-zero otherwise

		// Shutdown object
		int16_t Shutdown(void);									// Returns 0 if successfull, non-zero otherwise

		// Suspend object
		void Suspend(void);

		// Resume object
		void Resume(void);

		// Update object
		void Update(void);

		// Render object
		void Render(void);

		// Called by editor to init new object at specified position
		int16_t EditNew(												// Returns 0 if successfull, non-zero otherwise
			int16_t sX,												// In:  New x coord
			int16_t sY,												// In:  New y coord
			int16_t sZ);												// In:  New z coord

		// Called by editor to modify object
		int16_t EditModify(void);									// Returns 0 if successfull, non-zero otherwise

		// Called by editor to move object to specified position
		int16_t EditMove(											// Returns 0 if successfull, non-zero otherwise
			int16_t sX,												// In:  New x coord
			int16_t sY,												// In:  New y coord
			int16_t sZ);												// In:  New z coord

		// Called by editor to update object
		void EditUpdate(void);

		// Called by editor to render object
		void EditRender(void);

		// Give Edit a rectangle around this object
		void EditRect(RRect* pRect);

		// Called by editor to get the hotspot of an object in 2D.
		void EditHotSpot(			// Returns nothiing.
			int16_t*	psX,			// Out: X coord of 2D hotspot relative to
										// EditRect() pos.
			int16_t*	psY);			// Out: Y coord of 2D hotspot relative to
										// EditRect() pos.

	//---------------------------------------------------------------------------
	// Internal functions
	//---------------------------------------------------------------------------
	protected:
		// Get all required resources
		int16_t GetResources(void);						// Returns 0 if successfull, non-zero otherwise
		
		// Free all resources
		int16_t FreeResources(void);						// Returns 0 if successfull, non-zero otherwise
	};


#endif //GOALTIMER_H
////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
