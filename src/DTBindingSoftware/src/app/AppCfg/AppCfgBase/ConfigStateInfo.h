/**********************************************************//**
 * @file	ConfigStateInfo
 * @brief	header bestand voor de CConfigStateInfo klasse.
 *
 * @author	Mitchell Tom
 * @date	20090424
 *
 * @since	0x0506
 *************************************************************/
#ifndef DUOTECNO_CONFIGSTATEINFO_H
#define DUOTECNO_CONFIGSTATEINFO_H
/*************************************************************/

struct CConfigStateInfo_States
{
	enum 
	{
		/**
		 * @brief	configuratie is idle.
		 */
		STATE_CONFIG_IDLE = 0,

		/**
		 * @brief	Initialise commando versturen.
		 */
		STATE_CONFIG_INITIALISE,

		/**
		 * @brief	Gedaan.
		 */
		STATE_CONFIG_DONE,

		/**
		 * @brief	Deze macro moet door de user gebruikt worden.
		 */
		STATE_CONFIG_USER	
	};
};

/*************************************************************/

struct CConfigStateInfo
{
	int m_currentState;
	int m_nextState;
	int m_configClassIndex;
	BYTE m_bMethodData;
	const char* m_szInfo;	
};


/*************************************************************/
#endif
/*************************************************************/
