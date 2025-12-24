/**********************************************************//**
 * @file	AppCfgSmapLiteFSMMaster
 * @brief	header bestand voor de CAppCfgSmapLiteFSMMaster klasse.
 *
 * @author	Mitchell Tom
 * @date	20161128
 *
 * @since	0x0C05
 *************************************************************/
#include "stdafx.h"
#include "AppCfgSmapLiteFSMMaster.h"

#include "AppCfgSmapLiteInfo.h"
#include "AppCfgSmapLiteClass.h"
#include "AppCfgSmapLiteFile.h"

#include "app\appcfg\appcfgbase\ConfigStateInfo.h"
#include "Nodemess.h"

#include "IComApplication.h"			// temp

/*************************************************************/


namespace
{
	enum /* Dit zijn toestanden */
	{	
		STATE_CONFIG_SMAPLITE_INITIALISE = CConfigStateInfo_States::STATE_CONFIG_INITIALISE,
		STATE_CONFIG_SMAPLITE_DONE = CConfigStateInfo_States::STATE_CONFIG_DONE,

		STATE_CONFIG_SMAPLITE_ENTRIES = CConfigStateInfo_States::STATE_CONFIG_USER,	
		STATE_CONFIG_SMAPLITE_CONFIGDONE
	};

	enum /* Index in configStateInfo[ ] */
	{
		CONFIGSTATEINFO_INDEX_INITIALISE			= 0,
		CONFIGSTATEINFO_INDEX_ENTRY					= 1,
		CONFIGSTATEINFO_INDEX_CONFIGDONE			= 2,
	};

	const CConfigStateInfo configStateInfo[ ] =
	{
		{	// CONFIGSTATEINFO_INDEX_INITIALISE - Index 0
			STATE_CONFIG_SMAPLITE_INITIALISE,					// m_currentState
			STATE_CONFIG_SMAPLITE_DONE,							// m_nextState
			0xFF,												// m_configClassIndex
			SMAPLITE_CONFIG_INITIALISE,							// m_bMethodData
			"Initialise screenmapping config."					// m_szInfo
		},
		{	// STATE_CONFIG_SMAPLITE_ENTRIES - Index = 1
			STATE_CONFIG_SMAPLITE_ENTRIES,						// m_currentState
			STATE_CONFIG_SMAPLITE_DONE,							// m_nextState
			0,													// m_configClassIndex
			SMAPLITE_CONFIG_ENTRY,								// m_bMethodData
			"Uploading screenmapping:02X"					// m_szInfo
		},
		{	// CONFIGSTATEINFO_INDEX_CONFIGDONE - Index = 2
			STATE_CONFIG_SMAPLITE_CONFIGDONE,					// m_currentState
			STATE_CONFIG_SMAPLITE_DONE,							// m_nextState
			0xFF,												// m_configClassIndex
			SMAPLITE_CONFIG_DONE,								// m_bMethodData
			"Uploading screenmapping config done"				// m_szInfo
		}
	};
};


/*************************************************************/

CAppCfgSmapLiteFSMMaster::CAppCfgSmapLiteFSMMaster(IComApplication& parent) :
Base(parent), 
m_currentIndex(0), 
m_maxIndex(0), 
m_currentState(STATE_INIT),
m_currentClass(CONFIG_CLASS_INDEX_SMAPLITE)
{ 
}

CAppCfgSmapLiteFSMMaster::~CAppCfgSmapLiteFSMMaster() 
{ 
}

/**************************************************************/

BOOL CAppCfgSmapLiteFSMMaster::Start(
	CAppCfgSmapLiteInfo& info)
{	
	CAppCfgSmapLiteFile& refConfigFile = info.getConfigFile();

	// Configuratie klassen initialiseren.
	m_pCurrentItems[CONFIG_CLASS_INDEX_SMAPLITE] = 
		refConfigFile.GetClass(AppCfgSmapLiteClasses::ENUM_CONFIGCLASS_SMAPLITE);


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
		m_pfStartChildFsm[CONFIG_CLASS_INDEX_SMAPLITE] = &CAppCfgSmapLiteFSMMaster::startChildFSMCfgSmapLite;
	
		setCurrentNode(info.getNodeAddress( ));
		return FSM( EVENT_START );	
	}

	return FALSE;
} 

BOOL CAppCfgSmapLiteFSMMaster::Cancel( )
{
	return (FSM(EVENT_CANCEL));
}

BOOL CAppCfgSmapLiteFSMMaster::startFSMInit( )
{
	const BOOL fResult =
			getFSM().Start(getCurrentNode(),						 
						   const_cast<CConfigStateInfo*>(&configStateInfo[CONFIGSTATEINFO_INDEX_INITIALISE]));
	return fResult;
}

BOOL CAppCfgSmapLiteFSMMaster::startFSMDone( )
{	
	const BOOL fResult =
			getFSM().Start(getCurrentNode(),						 
						   const_cast<CConfigStateInfo*>(&configStateInfo[CONFIGSTATEINFO_INDEX_CONFIGDONE]));
	return fResult;
}

BOOL CAppCfgSmapLiteFSMMaster::startChildFSM(
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

BOOL CAppCfgSmapLiteFSMMaster::startChildFSMCfgSmapLite( )
{
	CConfigExternalClass* const pConfigClass = m_pCurrentItems[CONFIG_CLASS_INDEX_SMAPLITE];
	const CConfigStateInfo* const pStateInfo = &configStateInfo[CONFIGSTATEINFO_INDEX_ENTRY];

	ASSERT(pConfigClass);
	ASSERT(pStateInfo);

	return startChildFSM(pConfigClass,pStateInfo);
}

/*************************************************************/
BOOL CAppCfgSmapLiteFSMMaster::FSM(BYTE event)
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
						m_currentClass = CONFIG_CLASS_INDEX_SMAPLITE;
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

void CAppCfgSmapLiteFSMMaster::notifyFSMDone( )
{
	getParent().NotifyOnTaskDone( COMAPPLICATION_TASK_ID_UPLOAD_SMAPLITECFG );
}

void CAppCfgSmapLiteFSMMaster::notifyFSMError( )
{
	getParent().NotifyOnTaskError( COMAPPLICATION_TASK_ID_UPLOAD_SMAPLITECFG );
}

void CAppCfgSmapLiteFSMMaster::onChildFSMDone( )
{
	FSM( EVENT_DONE );
}

void CAppCfgSmapLiteFSMMaster::onChildFSMError( )
{
	FSM( EVENT_ERROR );
}

/*************************************************************/

