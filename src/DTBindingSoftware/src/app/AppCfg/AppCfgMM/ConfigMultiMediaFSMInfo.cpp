/**********************************************************//**
 * @file	ConfigMultiMediaFSMInfo.cpp
 * @brief	Implementatie bestand voor de CConfigMultiMediaFSMInfo klasse.
 *
 * @author	Mitchell Tom
 * @date	20090423
 *
 * @since	0x0506
 *
 * @note	Wordt niet gebruikt.
 *************************************************************/
#include "stdafx.h"
#include "ConfigMultiMediaFSMInfo.h"
/*************************************************************/
#include "ConfigMultiMediaFile.h"
#include "Nodemess.h"
/*************************************************************/
#ifdef DONT_USE
/*************************************************************/
namespace
{
	enum 
	{
		STATE_CONFIG_MM_INITIALISE = CConfigStateInfo_States::STATE_CONFIG_INITIALISE,
		STATE_CONFIG_MM_DONE_V64 = CConfigStateInfo_States::STATE_CONFIG_DONE,

		// ...
		STATE_CONFIG_MM_SOURCE = CConfigStateInfo_States::STATE_CONFIG_USER,
		STATE_CONFIG_MM_DEST,
		STATE_CONFIG_MM_ROOM,
		STATE_CONFIG_MM_MUXSRC,
		STATE_CONFIG_MM_MUXDEST,
		STATE_CONFIG_MM_SPEECHINDOOR,
		STATE_CONFIG_MM_SPEECHOUTDOOR,
		STATE_CONFIG_MM_VIDEOMUX,
		STATE_CONFIG_MM_AVMATRIX,
		STATE_CONFIG_MM_VIDEOPHONE,
		STATE_CONFIG_MM_ALARMZONES,
	};
};
/*************************************************************/



const CConfigStateInfo MultimediaStateInfo[ ] =
{
	{
		STATE_CONFIG_MM_INITIALISE,		// !!!!!			// m_currentState
		STATE_CONFIG_MM_SOURCE,								// m_nextState
		0xFF,							// !!!!!			// m_configClassIndex
		EXTERNAL_CONFIG_INITIALIZE,							// m_bMethodData
		"Initialise external config."						// m_szInfo
	},
	{
		STATE_CONFIG_MM_SOURCE,								// m_currentState
		STATE_CONFIG_MM_DEST,								// m_nextState
		ENUM_CONFIGCLASS_AUDIOSOURCE,						// m_configClassIndex
		AUDIO_CONFIG_SET_SOURCE,							// m_bMethodData
		"Uploading source:02X"								// m_szInfo
	},
	{
		STATE_CONFIG_MM_DEST,									// m_currentState
		STATE_CONFIG_MM_ROOM,								// m_nextState
		ENUM_CONFIGCLASS_AUDIODEST,							// m_configClassIndex
		AUDIO_CONFIG_SET_DESTINATION,						// m_bMethodData
		"Uploading destination:02X"							// m_szInfo
	},
	{
		STATE_CONFIG_MM_ROOM,									// m_currentState
		STATE_CONFIG_MM_MUXSRC,									// m_nextState
		ENUM_CONFIGCLASS_AUDIOROOM,								// m_configClassIndex
		AUDIO_CONFIG_SET_ROOM,									// m_bMethodData
		"Uploading audioroom:02X"								// m_szInfo
	},
	{
		STATE_CONFIG_MM_MUXSRC,									// m_currentState
		STATE_CONFIG_MM_MUXDEST,								// m_nextState
		ENUM_CONFIGCLASS_VIDEOMUXSRC,
		VIDEOMUXSOURCE_CONFIG_SET,								// m_bMethodData
		"Uploading videomux src:02X"							// m_szInfo
	},
	{
		STATE_CONFIG_MM_MUXDEST,								// m_currentState
		STATE_CONFIG_MM_SPEECHINDOOR,							// m_nextState
		ENUM_CONFIGCLASS_VIDEOMUXDEST,
		VIDEOMUXDESTINATION_CONFIG_SET,							// m_bMethodData
		"Uploading videomux dest:02X"							// m_szInfo
	},
	{
		STATE_CONFIG_MM_SPEECHINDOOR,							// m_currentState
		STATE_CONFIG_MM_SPEECHOUTDOOR,							// m_nextState
		ENUM_CONFIGCLASS_SPEECHDEVINDOOR,						//
		SPEECHDEVINDOOR_CONFIG_SET,								// m_bMethodData
		"Uploading speech dev. indoor:02X"						// m_szInfo
	},
	{
		STATE_CONFIG_MM_SPEECHOUTDOOR,							// m_currentState
		STATE_CONFIG_MM_VIDEOMUX,								// m_nextState
		ENUM_CONFIGCLASS_SPEECHDEVOUTDOOR,
		SPEECHDEVOUTDOOR_CONFIG_SET,							// m_bMethodData
		"Uploading speech dev. outdoor:02X"						// m_szInfo
	},
	{
		STATE_CONFIG_MM_VIDEOMUX,								// m_currentState
		STATE_CONFIG_MM_AVMATRIX,								// m_nextState
		ENUM_CONFIGCLASS_VIDEOMUX,
		VIDEOMUX_CONFIG_SET,									// m_bMethodData
		"Uploading videomux configuration."						// m_szInfo
	},
	{
		STATE_CONFIG_MM_AVMATRIX,								// m_currentState
		STATE_CONFIG_MM_VIDEOPHONE,								// m_nextState
		ENUM_CONFIGCLASS_AVMATRIX,
		AVMATRIX_CONFIG_SET,									// m_bMethodData
		"Uploading AVMatrix configuration."						// m_szInfo
	},
	{
		STATE_CONFIG_MM_VIDEOPHONE,								// m_currentState
		STATE_CONFIG_MM_ALARMZONES,								// m_nextState
		ENUM_CONFIGCLASS_VIDEOPHONE,
		VIDEOPHONE_CONFIG_SET,									// m_bMethodData
		"Uploading videophone configuration."					// m_szInfo
	},
	{
		STATE_CONFIG_MM_ALARMZONES,								// m_currentState
		STATE_CONFIG_MM_DONE_V64,			//!!!!!!!!				// m_nextState
		ENUM_CONFIGCLASS_ALARMZONE,
		ALARMZONE_CONFIG_SET,									// m_bMethodData
		"Uploading Alarmzone:02X"								// m_szInfo
	},
};

/********************************************************************/
#endif
/********************************************************************/