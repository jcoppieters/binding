/**********************************************************//**
 * @file	ConfigMultiFSMInfo.cpp
 * @brief	Implementatie bestand voor de CConfigMultiFSMInfo klasse.
 *
 * @author	Mitchell Tom
 * @date	20090427
 *
 * @since	0x0560
 *************************************************************/
#include "stdafx.h"
#include "ConfigUnitFSMInfo.h"
/*************************************************************/
#include "ConfigUnitCreateFile.h"
#include "Nodemess.h"
/*************************************************************/
namespace
{
	enum 
	{	
		STATE_CONFIG_UNITS_DONE = CConfigStateInfo_States::STATE_CONFIG_DONE,
		STATE_CONFIG_UNITS_START = CConfigStateInfo_States::STATE_CONFIG_USER,	
	};
};
/*************************************************************/

const CConfigStateInfo UnitCreateStateInfo[ ] =
{
	{
		STATE_CONFIG_UNITS_START,		// !!!!!			// m_currentState
		STATE_CONFIG_UNITS_DONE,							// m_nextState
		0x00,							// !!!!!			// m_configClassIndex
		UNIT_CONFIG_SET,									// m_bMethodData
		"Uploading unit info:02X"							// m_szInfo
	},
};