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
// grip.h
// Project: Nostril (aka Postal)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef GRIP_H
#define GRIP_H

#include <math.h>

#include "RSPiX.h"
#include "camera.h"


// A CGrip object is typically used to control the movement of a CCamera object.
//
// The name "grip" is taken from the movie industry, where the person who moves
// around the platform the camera sits upon is called the "grip".  The camera-
// man is responsible for other camera adjustments such as pan, tilt and zoom.
// In our case, the camera doesn't have those other adjustments, so the grip is
// solely responsible for keeping the camera positioned correctly.
//
// The basic function of the grip is to move the camera so as to track some
// specified "target" in the scene.  Rather than just keeping the target within
// the camera's view, it is usually desirable to define a smaller area within
// the view, and to keep the target within (or outside of) that area.  This
// area is called the grip's "target zone".
class CGrip
	{
	//---------------------------------------------------------------------------
	// Types, enums, etc.
	//---------------------------------------------------------------------------

	//---------------------------------------------------------------------------
	// Variables
	//---------------------------------------------------------------------------
	public:
		int16_t m_sGripX;									// Grip's current x coord
		int16_t m_sGripY;									// Grip's current y coord

		int16_t m_sMinMoveX;								// Minimum move in x direction
		int16_t m_sMinMoveY;								// Minimum move in y direction
		int16_t m_sMaxMoveX;								// Maximum move in x direction
		int16_t m_sMaxMoveY;								// Maximum move in y direction
		int16_t m_sAlignX;									// Required x alignment (1 allows any alignment)
		int16_t m_sAlignY;									// Required y alignment (1 allows any alignment)

		int16_t m_sZoneX;
		int16_t m_sZoneY;
		int16_t m_sZoneR;
//		short m_sZoneW;
//		short m_sZoneH;

//		bool m_bAlwaysInView;							// Always keep target in view, regardless of
																// any other settings
//		bool m_bUseZone;
		bool m_bKeepInsideZone;
//		bool m_bKeepOutsideZone;

		CCamera* m_pCamera;								// Camera being controlled

	//---------------------------------------------------------------------------
	// Public functions
	//---------------------------------------------------------------------------
	public:
		// Default (and only) constructor
		CGrip();

		// Destructor
		~CGrip();

		// Set camera to control
		void SetCamera(
			CCamera* pCamera);							// In:  Camera to control

		// Set tracking parameters
		void SetParms(
			int16_t sZoneR,									// In:  Zone radius
			int16_t sMinMoveX,								// In:  Minimum move in x direction
			int16_t sMinMoveY,								// In:  Minimum move in y direction
			int16_t sMaxMoveX,								// In:  Maximum move in x direction
			int16_t sMaxMoveY,								// In:  Maximum move in y direction
			int16_t sAlignX,									// In:  Required x alignment (1 allows any alignment)
			int16_t sAlignY,									// In:  Required y alignment (1 allows any alignment)
			bool bKeepInsideZone)						// In:  True to keep inside zone, false otherwise
			{
			m_sZoneR = sZoneR;
			m_sMinMoveX = sMinMoveX;
			m_sMinMoveY = sMinMoveY;
			m_sMaxMoveX = sMaxMoveX;
			m_sMaxMoveY = sMaxMoveY;
			m_sAlignX = sAlignX;
			m_sAlignY = sAlignY;
			m_bKeepInsideZone = bKeepInsideZone;
			}

		// Reset target, which foces camera to be moved so the target is centered
		void ResetTarget(
			int16_t sTargetX,								// In:  Target's x coord
			int16_t sTargetY,								// In:  Target's y coord
			int16_t sTargetR)								// In:  Target's radius
			{
			m_sZoneX = sTargetX;
			m_sZoneY = sTargetY;

			int16_t sUpperLeftX = m_sZoneX - (m_pCamera->m_sViewW / 2);
			int16_t sUpperLeftY = m_sZoneY - (m_pCamera->m_sViewH / 2);

			m_pCamera->SetViewPos(sUpperLeftX, sUpperLeftY);
			}

		// Track specified target coordinates
		void TrackTarget(
			int16_t sTargetX,								// In:  Target's x coord
			int16_t sTargetY,								// In:  Target's y coord
			int16_t sTargetR)								// In:  Target's radius
			{
			if (m_bKeepInsideZone)
				KeepInside(sTargetX, sTargetY, sTargetR);
//			else
//				KeepOutside(sTargetx, sTargetY);
			}

		void KeepInside(
			int16_t sTargetX,								// In:  Target's center x coord
			int16_t sTargetY,								// In:  Target's center y coord
			int16_t sTargetR)								// In:  Target's radius
			{
			// Calculate distance from center of zone to furthest edge of target
			double dx = sTargetX - m_sZoneX;
			double dy = sTargetY - m_sZoneY;
			double dDistance = sqrt((dx * dx) + (dy * dy)) + (double)sTargetR;

			// If distance is greater than zone's radius, then we need to move the camera
			if (dDistance > m_sZoneR)
				{
				// Calculate minimum amount we need to move
				double dMoveBy = dDistance - (double)m_sZoneR;

				// Calculate angle of movement
				int16_t sDeg = rspATan(dy, dx);

				// Calculate camera x and y movement
				int16_t sMoveX = (int16_t)(rspCos(sDeg) * dMoveBy);
				int16_t sMoveY = (int16_t)(rspSin(sDeg) * dMoveBy);

//				if ((sMoveX >= m_sMinMoveX) && (sMoveY >= m_sMinMoveY))
//					{
//					if (sMoveX > m_sMaxMoveX)
//						sMoveX = m_sMaxMoveX;
//					if (sMoveY > m_sMaxMoveY)
//						sMoveY = m_sMaxMoveY;

					int16_t sModX;
					if (sMoveX >= 0)
						sModX = sMoveX % m_sAlignX;
					else
						sModX = (-sMoveX) % m_sAlignX;
					if (sModX)
						sMoveX -= sModX;

					int16_t sModY;
					if (sMoveY >= 0)
						sModY = sMoveY % m_sAlignY;
					else
						sModY = (-sMoveY) % m_sAlignY;
					if (sModY)
						sMoveY -= sModY;

					m_sZoneX += sMoveX;
					m_sZoneY += sMoveY;

					int16_t sUpperLeftX = m_sZoneX - (m_pCamera->m_sViewW / 2);
					if (sUpperLeftX < 0)
						sUpperLeftX = 0;

					int16_t sUpperLeftY = m_sZoneY - (m_pCamera->m_sViewH / 2);
					if (sUpperLeftY < 0)
						sUpperLeftY = 0;

					m_pCamera->SetViewPos(sUpperLeftX, sUpperLeftY);
//					}
				}
			}


	//---------------------------------------------------------------------------
	// Helper functions
	//---------------------------------------------------------------------------
	protected:
	};


#endif //GRIP_H
////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
