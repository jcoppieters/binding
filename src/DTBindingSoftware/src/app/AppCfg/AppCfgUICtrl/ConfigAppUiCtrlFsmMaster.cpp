/**********************************************************//**
 * @file	ConfigAppUiCtrlFsmMaster
 * @brief	implementatie bestand voor de File klasse
 *
 * @author	Mitchell Tom
 * @date	20150806
 *
 * @since	0x0804
 *************************************************************/

#include "stdafx.h"
#include "configappuictrlfsmmaster.h"

#include "configappuictrlclasses.h"
#include "configappuictrlfile.h"
#include "configappuictrlinfo.h"

#include "app\appcfg\appcfgbase\ConfigStateInfo.h"
#include "Nodemess.h"

#include "IComApplication.h"			

/*************************************************************/
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*************************************************************/
namespace
{
	enum /* Dit zijn toestanden */
	{	
		STATE_CONFIG_APPUICTRL_INITIALISE = CConfigStateInfo_States::STATE_CONFIG_INITIALISE,
		STATE_CONFIG_APPUICTRL_DONE = CConfigStateInfo_States::STATE_CONFIG_DONE,

		STATE_CONFIG_APPUICTRL_GENERAL = CConfigStateInfo_States::STATE_CONFIG_USER,
		STATE_CONFIG_APPUICTRL_LED = CConfigStateInfo_States::STATE_CONFIG_USER+1,
		STATE_CONFIG_APPUICTRL_CONFIGDONE
	};

	enum /* Index in configStateInfo[ ] */
	{
		CONFIGSTATEINFO_INDEX_INITIALISE			= 0,
		CONFIGSTATEINFO_INDEX_GENERAL				= 1,
		CONFIGSTATEINFO_INDEX_LED					= 2,
		CONFIGSTATEINFO_INDEX_CONFIGDONE			= 3,
	};

	const CConfigStateInfo configStateInfo[ ] =
	{
		{	// CONFIGSTATEINFO_INDEX_INITIALISE - Index 0
			STATE_CONFIG_APPUICTRL_INITIALISE,					// m_currentState
			STATE_CONFIG_APPUICTRL_DONE,						// m_nextState
			0xFF,												// m_configClassIndex
			LEDCFG_INIT,										// m_bMethodData
			"Initialise LED configuration"						// m_szInfo
		},
		{	// CONFIGSTATEINFO_INDEX_GENERAL - Index = 1
			STATE_CONFIG_APPUICTRL_GENERAL,						// m_currentState
			STATE_CONFIG_APPUICTRL_DONE,						// m_nextState
			0,													// m_configClassIndex
			LEDCFG_DATA,										// m_bMethodData
			"Uploading LED general configuration"				// m_szInfo
		},	
		{	// CONFIGSTATEINFO_INDEX_GENERAL - Index = 2
			STATE_CONFIG_APPUICTRL_LED,							// m_currentState
			STATE_CONFIG_APPUICTRL_DONE,						// m_nextState
			0,													// m_configClassIndex
			LEDCFG_DATA_LED,									// m_bMethodData
			"Uploading LED configuration:02X"					// m_szInfo
		},	
		{	// CONFIGSTATEINFO_INDEX_CONFIGDONE - Index = 3
			STATE_CONFIG_APPUICTRL_CONFIGDONE,					// m_currentState
			STATE_CONFIG_APPUICTRL_DONE,						// m_nextState
			0xFF,												// m_configClassIndex
			LEDCFG_DONE,										// m_bMethodData
			"Uploading LED configuration done"					// m_szInfo
		}
	};
};

/*************************************************************/

CConfigAppUiCtrlFSMMaster::CConfigAppUiCtrlFSMMaster(IComApplication& parent) :
Base( parent ), 
m_currentIndex(0), 
m_maxIndex(0), 
m_pLedCfgGeneral(0),
m_pLedCfgLeds(0),
m_currentState(STATE_INIT)
{ 
}
CConfigAppUiCtrlFSMMaster::~CConfigAppUiCtrlFSMMaster() 
{ 
}

/*************************************************************/

BOOL CConfigAppUiCtrlFSMMaster::Start(
	CConfigAppUiCtrlInfo& info)
{	
	m_pLedCfgGeneral =
		reinterpret_cast<CConfigLedCfgGeneralClass*>
			(info.getConfigFile().GetClass(ConfigAppUiCtrlClasses::ENUM_CONFIGCLASS_LEDCFG_GENERAL));

	m_pLedCfgLeds = 
		reinterpret_cast<CConfigLedCfgLedClass*>
			(info.getConfigFile().GetClass(ConfigAppUiCtrlClasses::ENUM_CONFIGCLASS_LEDCFG_LEDDATA));
	
	
	if ((0!=m_pLedCfgGeneral)&&(0!=m_pLedCfgLeds))
	{
		setCurrentNode(info.getNodeAddress());
		return FSM( EVENT_START );	
	}
	return FALSE;
} 

BOOL CConfigAppUiCtrlFSMMaster::Cancel( )
{
	return (FSM(EVENT_CANCEL));
}
BOOL CConfigAppUiCtrlFSMMaster::startFSMInit( )
{
	const BOOL fResult =
			getFSM().Start(getCurrentNode(),						 
						   const_cast<CConfigStateInfo*>(&configStateInfo[0]));
	return fResult;
}
BOOL CConfigAppUiCtrlFSMMaster::startFSMDone( )
{
	const BOOL fResult =
			getFSM().Start(getCurrentNode(),						 
						   const_cast<CConfigStateInfo*>(&configStateInfo[3]));
	return fResult;
}

BOOL CConfigAppUiCtrlFSMMaster::startChildFSMGeneral( )
{
	if (0!=m_pLedCfgGeneral)
	{
		const int temp = m_pLedCfgGeneral->GetMax();	
		ASSERT( temp >= 0 && temp <= 0xFF);				

		m_maxIndex = temp;							

		if (m_maxIndex > 0)
		{				
			CConfigExternalClass* ptempClass = m_pLedCfgGeneral;

			const BOOL fResult =
				getFSM().Start(getCurrentNode(),
							   &ptempClass,
							   const_cast<CConfigStateInfo*>(&configStateInfo[1]),
							   0);
			return fResult;
		}
	}
	return FALSE;
}

BOOL CConfigAppUiCtrlFSMMaster::startChildFSMLeds( )
{
	if (0!=m_pLedCfgLeds)
	{
		const int temp = m_pLedCfgLeds->GetMax();	
		ASSERT( temp >= 0 && temp <= 0xFF);				

		m_maxIndex = temp;							

		if (m_maxIndex > 0)
		{				
			CConfigExternalClass* ptempClass = m_pLedCfgLeds;

			const BOOL fResult =
				getFSM().Start(getCurrentNode(),
							   &ptempClass,
							   const_cast<CConfigStateInfo*>(&configStateInfo[2]),
							   0);
			return fResult;
		}
	}
	return FALSE;
}

/*************************************************************/

BOOL CConfigAppUiCtrlFSMMaster::FSM(BYTE event)
{
	switch (m_currentState)
	{
		case STATE_INIT:
		{
			switch (event)
			{
				case EVENT_START:
				{					
					if (startFSMInit())
					{
						// Versturen van het config init commando.
						m_currentState = STATE_IDLE;
						m_currentIndex = 0;
						return TRUE;
					}
					ASSERT(0);// konden geen init versturen ?

				} break;

				default:
				{
					ASSERT( 0 );

				} break;
			}
		} break;

		case STATE_IDLE:
		{
			switch(event)
			{
				case EVENT_CANCEL:
				{
					return (getFSM().Cancel());
					
				} break;

				case EVENT_DONE:
				{				
					// config initialise is verstuurd. 
					if (startChildFSMGeneral( ))
					{
						// Schedule items versturen.
						m_currentIndex = 0;
						m_currentState = STATE_LEDCFG_GENERAL_DONE;
						return TRUE;
					}
					else
					{							
						if (startChildFSMLeds( ))
						{
							// indien er geen items te versturen zijn,
							// dan events versturen.
							m_currentIndex = 0;
							m_currentState = STATE_LEDCFG_LED_DONE;
							return TRUE;
						}			
						else
						{
							// indien er geen items of events te versturen zijn,
							// dan config done commando versturen.
							startFSMDone( );
							m_currentState = STATE_DONE;
							return TRUE;						
						}
					}
				} break;

				case EVENT_ERROR:
				{				
					getParent().NotifyOnTaskError( COMAPPLICATION_TASK_ID_UPLOAD_LEDSCFG );
										
				} return TRUE;

				case EVENT_START:
				{
					ASSERT( 0 );
					
				} return FALSE;
			}
					
		} break;

		case STATE_LEDCFG_GENERAL_DONE:
		{
			switch(event)
			{
				case EVENT_CANCEL:
				{
					return (getFSM().Cancel());
					
				} break;

				case EVENT_DONE:
				{	
					// Alle items zijn verstuurd. 
					if (startChildFSMLeds( ))
					{
						// Schedule events versturen.
						m_currentIndex = 0;
						m_currentState = STATE_LEDCFG_LED_DONE;
						return TRUE;
					}			
					else
					{
						// indien er geen events te versturen zijn,
						// dan config done commando versturen.
						startFSMDone( );
						m_currentState = STATE_DONE;
						return TRUE;					
					}				
				} break;

				case EVENT_ERROR:
				{
					getParent().NotifyOnTaskError( COMAPPLICATION_TASK_ID_UPLOAD_LEDSCFG );
										
				} return TRUE;

				case EVENT_START:
				{
					ASSERT( 0 );
					
				} return FALSE;
			}
		} break;

		case STATE_LEDCFG_LED_DONE:
		{
			switch (event)
			{
				case EVENT_CANCEL:
				{
					return (getFSM().Cancel());
					
				} break;

				case EVENT_DONE:
				{
					// Alle entries zijn verstuurd, 
					// het config done commando versturen.
					startFSMDone( );
					m_currentState = STATE_DONE;					

				} return TRUE;		// CHANGES_MVS_2008_WARNINGS

				case EVENT_ERROR:
				{
					getParent().NotifyOnTaskError( COMAPPLICATION_TASK_ID_UPLOAD_LEDSCFG );				
										
				} return TRUE;		// CHANGES_MVS_2008_WARNINGS

				case EVENT_START:
				{
					ASSERT( 0 );
					
				} return FALSE;
			}
		} break;

		case STATE_DONE:
		{
			switch (event)
			{
				case EVENT_CANCEL:
				{
					return (getFSM().Cancel());
					
				} break;

				case EVENT_DONE:
				{				
					getParent().NotifyOnTaskDone( COMAPPLICATION_TASK_ID_UPLOAD_LEDSCFG );
					return TRUE;

				} break;

				case EVENT_ERROR:
				{
					getParent().NotifyOnTaskError( COMAPPLICATION_TASK_ID_UPLOAD_LEDSCFG );
					return TRUE;
										
				} return TRUE;

				case EVENT_START:
				{
					ASSERT( 0 );
					
				} return FALSE;
			}					
		} break;
	}
	return FALSE;
}

/*************************************************************/

void CConfigAppUiCtrlFSMMaster::onChildFSMDone( )
{
	FSM( EVENT_DONE );
}

void CConfigAppUiCtrlFSMMaster::onChildFSMError( )
{
	FSM( EVENT_ERROR );
}

