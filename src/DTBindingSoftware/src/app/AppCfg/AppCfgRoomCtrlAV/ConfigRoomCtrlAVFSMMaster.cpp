#include "stdafx.h"
#include "ConfigRoomCtrlAVFSMMaster.h"

#include "ConfigRoomCtrlAVInfo.h"
#include "ConfigRoomCtrlAVClass.h"
#include "ConfigRoomCtrlAVFile.h"

#include "app\appcfg\appcfgbase\ConfigStateInfo.h"
#include "Nodemess.h"

#include "IComApplication.h"			// temp

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
		STATE_CONFIG_ROOMCTRL_INITIALISE = CConfigStateInfo_States::STATE_CONFIG_INITIALISE,
		STATE_CONFIG_ROOMCTRL_DONE = CConfigStateInfo_States::STATE_CONFIG_DONE,

		STATE_CONFIG_ROOMCTRL_DEST = CConfigStateInfo_States::STATE_CONFIG_USER,	
		STATE_CONFIG_ROOMCTRL_SRC,
		STATE_CONFIG_ROOMCTRL_FUNC,
		STATE_CONFIG_ROOMCTRL_TNRPRESET,
		STATE_CONFIG_ROOMCTRL_CONFIGDONE
	};

	enum /* Index in configStateInfo[ ] */
	{
		CONFIGSTATEINFO_INDEX_INITIALISE			= 0,
		CONFIGSTATEINFO_INDEX_DEST					= 1,
		CONFIGSTATEINFO_INDEX_SOURCE				= 2,
		CONFIGSTATEINFO_INDEX_FUNCTION				= 3,
		CONFIGSTATEINFO_INDEX_TUNERPRESET			= 4,
		CONFIGSTATEINFO_INDEX_CONFIGDONE			= 5,
	};

	const CConfigStateInfo configStateInfo[ ] =
	{
		{	// CONFIGSTATEINFO_INDEX_INITIALISE - Index 0
			STATE_CONFIG_ROOMCTRL_INITIALISE,					// m_currentState
			STATE_CONFIG_ROOMCTRL_DONE,							// m_nextState
			0xFF,												// m_configClassIndex
			ROOMCTRL_AUDIOCFG_INITIALISE,						// m_bMethodData
			"Initialise roomcontroller audio config."			// m_szInfo
		},
		{	// CONFIGSTATEINFO_INDEX_DEST - Index = 1
			STATE_CONFIG_ROOMCTRL_DEST,							// m_currentState
			STATE_CONFIG_ROOMCTRL_DONE,							// m_nextState
			0,													// m_configClassIndex
			ROOMCTRL_AUDIOCFG_DEST_SET,							// m_bMethodData
			"Uploading audio destination:02X"					// m_szInfo
		},
		{	// CONFIGSTATEINFO_INDEX_SOURCE - Index = 2
			STATE_CONFIG_ROOMCTRL_SRC,							// m_currentState
			STATE_CONFIG_ROOMCTRL_DONE,							// m_nextState
			0,													// m_configClassIndex
			ROOMCTRL_AUDIOCFG_SRC_SET,							// m_bMethodData
			"Uploading audio source:02X"						// m_szInfo
		},
		{	// CONFIGSTATEINFO_INDEX_FUNCTION - Index = 3
			STATE_CONFIG_ROOMCTRL_FUNC,							// m_currentState
			STATE_CONFIG_ROOMCTRL_DONE,							// m_nextState
			0,													// m_configClassIndex
			ROOMCTRL_AUDIOCFG_SRCFUNC_SET,						// m_bMethodData
			"Uploading audio function:02X"						// m_szInfo
		},
		{	// CONFIGSTATEINFO_INDEX_TUNERPRESET - Index = 4
			STATE_CONFIG_ROOMCTRL_TNRPRESET,					// m_currentState
			STATE_CONFIG_ROOMCTRL_DONE,							// m_nextState
			0,													// m_configClassIndex
			ROOMCTRL_AUDIOCFG_TNRPRESET_SET,					// m_bMethodData
			"Uploading audio tuner info:02X"					// m_szInfo
		},
		{	// CONFIGSTATEINFO_INDEX_CONFIGDONE - Index = 5
			STATE_CONFIG_ROOMCTRL_CONFIGDONE,					// m_currentState
			STATE_CONFIG_ROOMCTRL_DONE,							// m_nextState
			0xFF,												// m_configClassIndex
			ROOMCTRL_AUDIOCFG_DONE,								// m_bMethodData
			"Uploading roomcontroller audio config done"		// m_szInfo
		}
	};
};

/*************************************************************/

CConfigRoomCtrlAVFSMMaster::CConfigRoomCtrlAVFSMMaster(IComApplication& parent) :
Base( parent ), 
m_currentIndex(0), 
m_maxIndex(0), 
m_currentState(STATE_INIT),
m_currentClass(CONFIG_CLASS_INDEX_DEST)
{ 
}
CConfigRoomCtrlAVFSMMaster::~CConfigRoomCtrlAVFSMMaster() 
{ 
}
/*************************************************************/
BOOL CConfigRoomCtrlAVFSMMaster::Start(CConfigRoomCtrlAVInfo& info)
{	
	CConfigRoomCtrlAVFile& refConfigFile = info.getConfigFile();

	// Configuratie klassen initialiseren.
	m_pCurrentItems[CONFIG_CLASS_INDEX_DEST] = 
		refConfigFile.GetClass(ConfigRoomCtrlAVClasses::ENUM_CONFIGCLASS_ROOMCTRLAV_DEST);
	m_pCurrentItems[CONFIG_CLASS_INDEX_SOURCE] = 
		refConfigFile.GetClass(ConfigRoomCtrlAVClasses::ENUM_CONFIGCLASS_ROOMCTRLAV_SOURCE);
	m_pCurrentItems[CONFIG_CLASS_INDEX_FUNCTION] = 
		refConfigFile.GetClass(ConfigRoomCtrlAVClasses::ENUM_CONFIGCLASS_ROOMCTRLAV_FUNC);
	m_pCurrentItems[CONFIG_CLASS_INDEX_TNRPRESET] = 
		refConfigFile.GetClass(ConfigRoomCtrlAVClasses::ENUM_CONFIGCLASS_ROOMCTRLAV_TNRPRESET);

	int i;
	for (i=0;i<CONFIG_CLASS_INDEX_MAX;i++)
	{
		if (m_pCurrentItems[i] == 0)
			break;	// lege klasse gevonden.
	}

	if (i == CONFIG_CLASS_INDEX_MAX)
	{
		// Alle klassen zijn geinitialiseerd.
		
		// Initialiseren van de functie pointers...
		m_pfStartChildFsm[CONFIG_CLASS_INDEX_DEST] = &CConfigRoomCtrlAVFSMMaster::startChildFSMCfgDest;
		m_pfStartChildFsm[CONFIG_CLASS_INDEX_SOURCE] = &CConfigRoomCtrlAVFSMMaster::startChildFSMCfgSrc;
		m_pfStartChildFsm[CONFIG_CLASS_INDEX_FUNCTION] = &CConfigRoomCtrlAVFSMMaster::startChildFSMCfgFunc;
		m_pfStartChildFsm[CONFIG_CLASS_INDEX_TNRPRESET] = &CConfigRoomCtrlAVFSMMaster::startChildFSMCfgTnrPreset;

		setCurrentNode(info.getNodeAddress( ));
		return FSM( EVENT_START );	
	}

	return FALSE;
} 
BOOL CConfigRoomCtrlAVFSMMaster::Cancel( )
{
	return (FSM(EVENT_CANCEL));
}

BOOL CConfigRoomCtrlAVFSMMaster::startFSMInit( )
{
	const BOOL fResult =
			getFSM().Start(getCurrentNode(),						 
						   const_cast<CConfigStateInfo*>(&configStateInfo[CONFIGSTATEINFO_INDEX_INITIALISE]));
	return fResult;
}

BOOL CConfigRoomCtrlAVFSMMaster::startFSMDone( )
{	
	const BOOL fResult =
			getFSM().Start(getCurrentNode(),						 
						   const_cast<CConfigStateInfo*>(&configStateInfo[CONFIGSTATEINFO_INDEX_CONFIGDONE]));
	return fResult;
}

BOOL CConfigRoomCtrlAVFSMMaster::startChildFSM(
	CConfigExternalClass* pConfigClass,
	const CConfigStateInfo* pStateInfo)
{	
	if (pConfigClass && pStateInfo)
	{
		const int temp = pConfigClass->GetMax();			// CHANGES_MVS_2008_WARNINGS
		ASSERT( temp >= 0 && temp <= 0xFF);					// CHANGES_MVS_2008_WARNINGS

		m_maxIndex = temp;									// CHANGES_MVS_2008_WARNINGS

		if (m_maxIndex > 0)
		{										
			const BOOL fResult =
				getFSM().Start(getCurrentNode(),
							   &pConfigClass,
							   const_cast<CConfigStateInfo*>(pStateInfo),
							   0);
			return fResult;
		}
	}
	return FALSE;
}

BOOL CConfigRoomCtrlAVFSMMaster::startChildFSMCfgDest( )
{
	CConfigExternalClass* const pConfigClass = m_pCurrentItems[CONFIG_CLASS_INDEX_DEST];
	const CConfigStateInfo* const pStateInfo = &configStateInfo[CONFIGSTATEINFO_INDEX_DEST];

	ASSERT(pConfigClass);
	ASSERT(pStateInfo);

	return startChildFSM(pConfigClass,pStateInfo);
}

BOOL CConfigRoomCtrlAVFSMMaster::startChildFSMCfgSrc( )
{
	CConfigExternalClass* const pConfigClass = m_pCurrentItems[CONFIG_CLASS_INDEX_SOURCE];
	const CConfigStateInfo* const pStateInfo = &configStateInfo[CONFIGSTATEINFO_INDEX_SOURCE];

	ASSERT(pConfigClass);
	ASSERT(pStateInfo);

	return startChildFSM(pConfigClass,pStateInfo);
}

BOOL CConfigRoomCtrlAVFSMMaster::startChildFSMCfgFunc( )
{
	CConfigExternalClass* const pConfigClass = m_pCurrentItems[CONFIG_CLASS_INDEX_FUNCTION];
	const CConfigStateInfo* const pStateInfo = &configStateInfo[CONFIGSTATEINFO_INDEX_FUNCTION];

	ASSERT(pConfigClass);
	ASSERT(pStateInfo);

	return startChildFSM(pConfigClass,pStateInfo);
}

BOOL CConfigRoomCtrlAVFSMMaster::startChildFSMCfgTnrPreset( )
{
	CConfigExternalClass* const pConfigClass = m_pCurrentItems[CONFIG_CLASS_INDEX_TNRPRESET];
	const CConfigStateInfo* const pStateInfo = &configStateInfo[CONFIGSTATEINFO_INDEX_TUNERPRESET];

	ASSERT(pConfigClass);
	ASSERT(pStateInfo);

	return startChildFSM(pConfigClass,pStateInfo);
}

/*************************************************************/
BOOL CConfigRoomCtrlAVFSMMaster::FSM(BYTE event)
{
	switch (m_currentState)
	{
		case STATE_INIT:
		{
			switch (event)
			{
				case EVENT_START:
				{	
					// Versturen van het config init commando.
					if (startFSMInit())
					{						
						// Initialisatie van de toestanden.
						//m_currentState = STATE_IDLE;
						m_currentState = STATE_BUSY;
						m_currentClass = CONFIG_CLASS_INDEX_DEST;
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
		
		//case STATE_IDLE:
		case STATE_BUSY:
		{
			switch(event)
			{
				case EVENT_CANCEL:
				{
					return (getFSM().Cancel());
					
				} break;

				case EVENT_DONE:
				{				
					// Nieuwe configuratie starten. 
					// Maximum aantal klassen = CONFIG_CLASS_INDEX_MAX

					for (/* */;m_currentClass<CONFIG_CLASS_INDEX_MAX;m_currentClass++)
					{
						ASSERT(this->m_pfStartChildFsm[m_currentClass] != 0);

						if ((this->*m_pfStartChildFsm[m_currentClass])( ))
						{				
							m_currentIndex = 0;
							m_currentState = STATE_BUSY;
							m_currentClass++;
							return TRUE;		// OK 
						}
					}

					// Alle configuratie gedaan.
					// FSM afsluiten.

					if (m_currentClass == CONFIG_CLASS_INDEX_MAX)
					{
						startFSMDone( );
						m_currentState = STATE_DONE;
						return TRUE;											
					}

				} break;

				case EVENT_ERROR:
				{				
					notifyFSMError( );
										
				} return TRUE;

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
					notifyFSMDone( );
					return TRUE;

				} break;

				case EVENT_ERROR:
				{
					notifyFSMError( );
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

void CConfigRoomCtrlAVFSMMaster::notifyFSMDone( )
{
	getParent().NotifyOnTaskDone( COMAPPLICATION_TASK_ID_UPLOAD_ROOMCTRL_AUDIOCFG );
}

void CConfigRoomCtrlAVFSMMaster::notifyFSMError( )
{
	getParent().NotifyOnTaskError( COMAPPLICATION_TASK_ID_UPLOAD_ROOMCTRL_AUDIOCFG );
}

void CConfigRoomCtrlAVFSMMaster::onChildFSMDone( )
{
	FSM( EVENT_DONE );
}

void CConfigRoomCtrlAVFSMMaster::onChildFSMError( )
{
	FSM( EVENT_ERROR );
}

/*************************************************************/

/*********************** END OF FILE *************************/