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
///////////////////////////////////////////////////////////////////////////////
//
//	snd.cpp
// 
// History:
//		04/28/95 JMI	Started.
//
//		01/29/97	JMI	Added callback on done and GetSample() query.
//
//		05/09/97	JMI	Added ability to loop sub samples.
//
//		06/12/97	JMI	Added user value.
//
//		07/15097 JRD	Added members to hold sound volume information
//
//		08/05/97	JMI	Added Pause(), IsPaused(), and Resume().
//
//////////////////////////////////////////////////////////////////////////////
//
// See CPP for description of this API.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef SND_H
#define SND_H

#include <stdio.h> // For FILE*

#include "System.h"

// If PATHS_IN_INCLUDES macro is defined, we can utilized relative
// paths to a header file.  In this case we generally go off of our
// RSPiX root directory.  System.h MUST be included before this macro
// is evaluated.  System.h is the header that, based on the current
// platform (or more so in this case on the compiler), defines 
// PATHS_IN_INCLUDES.  Blue.h includes system.h so you can include that
// instead.
#ifdef PATHS_IN_INCLUDES
	#include "GREEN/Mix/mix.h"
	#include "GREEN/Sample/sample.h"
#else
	#include "mix.h"
	#include "sample.h"
#endif // PATHS_IN_INCLUDES

//////////////////////////////////////////////////////////////////////////////
// Macros.
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// Types.
//////////////////////////////////////////////////////////////////////////////
// Forward declare RSnd for typedef.
class RSnd;

// Handy-dandy typedef.
typedef RSnd* PSND;

class RSnd
	{
	public:
		// Default constructor.
		RSnd();
		// Destructor.
		~RSnd();

	/////////////////////////// Typedefs ///////////////////////////////////////
	public:
		typedef enum
			{	// States for m_sState and GetState().
			Stopped,		// Stopped completely.
			Starting,	// Just started, no data sent yet.
			Queueing,	// Sending data, none yet played.
			Playing,		// Sending data, data playing.
			Ending		// Done sending, still playing.
			} State;

		// Callback when done playing/streaming.
		typedef void (*DoneCall)(	// Returns nothing.
			RSnd*	psnd);				// This RSnd.

	////////////////////////// Querries ///////////////////////////////////////
	public:
		// Returns the current state of the play/stream process.
		State GetState(void)	{ return m_sState; }
		
		// Gets/returns the current position of the audio in bytes.
		int32_t GetPos(void);

		// Gets/returns the current time of the audio in milliseconds.
		int32_t GetTime(void);

		// Gets/returns the current sample.
		RSample* GetSample(void)
			{
			return m_psample;
			}

		// Query paused status of current play or stream.
		int16_t IsPaused(void)
			{
			return m_mix.IsChannelPaused();
			}

	////////////////////////// Methods ////////////////////////////////////////
	public:
		// Reset object.  Release RSample and reset variables.
		void Reset(void);
		
		// Load a sound file a lReadBufSize byte piece at a time and play 
		// lPlayBufSize byte pieces at a time.
		// Returns 0 on success.
		int16_t Stream(						// Returns 0 on success.
			char* pszSampleName,			// In:  Name of sample file.
			int32_t lPlayBufSize,			// In:  Size of play buffer in bytes.
			int32_t lReadBufSize,			// In:  Size of file read buffer in bytes.
			uint8_t	ucMainVolume = 255,	// In:  Primary Volume (0 - 255)
			uint8_t ucVolume2 = 255);		// In:  Secondary Volume (0 - 255)

			// Plays RSample supplied via ptr with buffer size of lPlayBufSize
		// (this is the size of the chunks sent to RMix).
		// Note on looping:
		//	 Start																		  End
		//		1-----------------------------------------------------------4
		//				2=======================================3
		//			lLoopStartTime									lLoopEndTime
		//
		// In a looping scenario, 1..2 of the sample is played, then 2..3
		// is repeated until m_sLoop is FALSE, at which time, once 3 is reached, 
		// 3..4 is played.
		int16_t Play(							// Returns 0 on success.
			RSample* psample,				// In:  Sample to play.
			int32_t lPlayBufSize,			// In:  Size of play buffer in bytes.
			uint8_t	ucMainVolume = 255,	// In:  Primary Volume (0 - 255)
			uint8_t ucVolume2 = 255,		// In:  Secondary Volume (0 - 255)
			int32_t lLoopStartTime = -1,	// In:  Where to loop back to in milliseconds.
												//	-1 indicates no looping (unless m_sLoop is
												// explicitly set).
			int32_t lLoopEndTime = 0);		// In:  Where to loop back from in milliseconds.
												// In:  If less than 1, the end + lLoopEndTime is used.

		// Aborts current play or stream.
		// Returns 0 on success.
		int16_t Abort(void);

		// Pause current play or stream.
		void Pause(void)
			{
			m_mix.PauseChannel();
			}

		// Resume current play or stream.
		void Resume(void)
			{
			m_mix.ResumeChannel();
			}

		// Set or clear (if psndfx is NULL) a RSndFx for this.
		void SetFx(				// Returns nothing.
			RSndFx* psndfx)	// FX for this.  Clears current, if NULL.
			{
			m_mix.SetFx(psndfx);
			}

	////////////////////////// Internal Methods ///////////////////////////////
	protected:
		// Initialize instantiable members.
		void Init(void);

		// Called from StreamCallStatic.
		// Sends back current volume information to RMix
		// Returns pointer to next buffer to play or NULL to end.
		void* StreamCall(	RMix::Msg msg, 
								void* pData, 
								uint32_t* pulNewBufSize,
								uintptr_t	 ulUser,
								uint8_t* pucVolume = NULL,
								uint8_t* pucVol2 = NULL);
								

		// Callback from blue regarding playing buffer(s).
		// Returns pointer to next buffer to play or NULL to end.
		static void* StreamCallStatic(RMix::Msg msg, 
												 void* pData,
												 uint32_t* pulNewBufSize,
												 uintptr_t ulUser,
												 uint8_t* pucVolume = NULL,
												 uint8_t* pucVol2 = NULL);

										// Called from PlayCallStatic.
		// Returns pointer to next buffer to play or NULL to end.
		void* PlayCall(RMix::Msg msg, 
							void* pData,
							uint32_t* pulNewBufSize,
							uint8_t*		pucVolume = NULL,
							uint8_t*		pucVol2 = NULL);

		// Callback from blue regarding playing buffer(s).
		// Sends back current volume information to RMix
		// Returns pointer to next buffer to play or NULL to end
		static void* PlayCallStatic(RMix::Msg msg, 
											 void* pData,
											 uint32_t* pulNewBufSize,
											 uintptr_t ulUser,
											 uint8_t* pucVolume = NULL,
											 uint8_t* pucVol2 = NULL);

	////////////////////////// Member vars ////////////////////////////////////
	public:
		int16_t			m_sLoop;				// If TRUE, Play() will loop until FALSE.
		int32_t			m_lLoopStartPos;	// Where to loop back to in bytes.
												// Play() only.
		int32_t			m_lLoopEndPos;		// Where to loop back from in bytes.
												// Play() only.

		DoneCall		m_dcUser;		// User callback when done playing/streaming
											// a sample.

		uintptr_t			m_ulUser;		// User value -- set as you please.

		int16_t			m_sChannelVolume;// 0-255 = Primary (local) Volume
		int16_t			m_sTypeVolume;	// 0-255	= Secondary (category) Volume
				
	protected:
		int32_t			m_lBufSize;		// Buffer unit to stream in.
		State			m_sState;		// One of the enums above representing 
											// this RSnd's state.
		RMix			m_mix;			// For playing/mixing sound data.
		PSAMPLE		m_psample;		// Sample to be streamed.
		int16_t			m_sOwnSample;	// TRUE if RSnd allocated m_psample, FALSE
											// otherwise.
		uint32_t			m_ulRemaining;	// Amount left of sample data to be played.
		uint32_t			m_ulSampleSize;// Overall sample size.

		/////////////////////// Static members /////////////////////////////////
	
	};

#endif // SND_H
//////////////////////////////////////////////////////////////////////////////
// EOF
//////////////////////////////////////////////////////////////////////////////
