#include "stdafx.h"
#include "ConfigBoseStFSMMaster.h"

#include "ConfigBoseStCfgInfo.h"
#include "ConfigBoseSTRenderClass.h"
#include "ConfigBoseSTCfgFile.h"

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
		STATE_CONFIG_BOSEST_INITIALISE = CConfigStateInfo_States::STATE_CONFIG_INITIALISE,
		STATE_CONFIG_BOSEST_DONE = CConfigStateInfo_States::STATE_CONFIG_DONE,

		STATE_CONFIG_BOSEST_RENDER = CConfigStateInfo_States::STATE_CONFIG_USER,	
		STATE_CONFIG_BOSEST_CONFIGDONE
	};

	enum /* Index in configStateInfo[ ] */
	{
		CONFIGSTATEINFO_INDEX_INITIALISE			= 0,
		CONFIGSTATEINFO_INDEX_RENDER				= 1,
		CONFIGSTATEINFO_INDEX_CONFIGDONE			= 2,
	};

	const CConfigStateInfo configStateInfo[ ] =
	{
		{	// CONFIGSTATEINFO_INDEX_INITIALISE - Index 0
			STATE_CONFIG_BOSEST_INITIALISE,						// m_currentState
			STATE_CONFIG_BOSEST_DONE,							// m_nextState
			0xFF,												// m_configClassIndex
			BOSEST_CONFIG_INITIALISE,							// m_bMethodData
			"Initialise Bose Soundtouch config."				// m_szInfo
		},
		{	// CONFIGSTATEINFO_INDEX_DEST - Index = 1
			STATE_CONFIG_BOSEST_RENDER,							// m_currentState
			STATE_CONFIG_BOSEST_DONE,							// m_nextState
			0,													// m_configClassIndex
			BOSEST_CONFIG_RENDER,								// m_bMethodData
			"Uploading audio render:02X"						// m_szInfo
		},		
		{	// CONFIGSTATEINFO_INDEX_CONFIGDONE - Index = 2
			STATE_CONFIG_BOSEST_CONFIGDONE,						// m_currentState
			STATE_CONFIG_BOSEST_DONE,							// m_nextState
			0xFF,												// m_configClassIndex
			BOSEST_CONFIG_DONE,									// m_bMethodData
			"Uploading Bose Soundtouch config done"				// m_szInfo
		}
	};
};

/*************************************************************/

CConfigBoseStFSMMaster::CConfigBoseStFSMMaster(IComApplication& parent) :
Base( parent ), 
m_currentIndex(0), 
m_maxIndex(0), 
m_currentState(STATE_INIT),
m_currentClass(CONFIG_CLASS_INDEX_RENDER)
{ 
}
CConfigBoseStFSMMaster::~CConfigBoseStFSMMaster() 
{ 
}
/*************************************************************/
BOOL CConfigBoseStFSMMaster::Start(CConfigBoseSTCfgInfo& info)
{	
	CConfigBoseSTCfgFile& refConfigFile = info.getConfigFile();

	// Configuratie klassen initialiseren.
	m_pCurrentItems[CONFIG_CLASS_INDEX_RENDER] = 
		refConfigFile.GetClass(ConfigBoseSTCfgClasses::ENUM_CONFIGCLASS_BOSESTCFG_RENDER);

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
		m_pfStartChildFsm[CONFIG_CLASS_INDEX_RENDER] = &CConfigBoseStFSMMaster::startChildFSMCfgRender;	

		setCurrentNode(info.getNodeAddress( ));
		return FSM( EVENT_START );	
	}

	return FALSE;
} 
BOOL CConfigBoseStFSMMaster::Cancel( )
{
	return (FSM(EVENT_CANCEL));
}

BOOL CConfigBoseStFSMMaster::startFSMInit( )
{
	const BOOL fResult =
			getFSM().Start(getCurrentNode(),						 
						   const_cast<CConfigStateInfo*>
							(&configStateInfo[CONFIGSTATEINFO_INDEX_INITIALISE]));
	return fResult;
}

BOOL CConfigBoseStFSMMaster::startFSMDone( )
{	
	const BOOL fResult =
			getFSM().Start(getCurrentNode(),						 
						   const_cast<CConfigStateInfo*>
							(&configStateInfo[CONFIGSTATEINFO_INDEX_CONFIGDONE]));
	return fResult;
}

BOOL CConfigBoseStFSMMaster::startChildFSM(
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

BOOL CConfigBoseStFSMMaster::startChildFSMCfgRender( )
{
	CConfigExternalClass* const pConfigClass = m_pCurrentItems[CONFIG_CLASS_INDEX_RENDER];
	const CConfigStateInfo* const pStateInfo = &configStateInfo[CONFIGSTATEINFO_INDEX_RENDER];

	ASSERT(pConfigClass);
	ASSERT(pStateInfo);

	return startChildFSM(pConfigClass,pStateInfo);
}

/*************************************************************/
BOOL CConfigBoseStFSMMaster::FSM(BYTE event)
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
						m_currentClass = CONFIG_CLASS_INDEX_RENDER;
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

void CConfigBoseStFSMMaster::notifyFSMDone( )
{
	getParent().NotifyOnTaskDone( COMAPPLICATION_TASK_ID_UPLOAD_BOSEST_CFG );
}

void CConfigBoseStFSMMaster::notifyFSMError( )
{
	getParent().NotifyOnTaskError( COMAPPLICATION_TASK_ID_UPLOAD_BOSEST_CFG );
}

void CConfigBoseStFSMMaster::onChildFSMDone( )
{
	FSM( EVENT_DONE );
}

void CConfigBoseStFSMMaster::onChildFSMError( )
{
	FSM( EVENT_ERROR );
}

/*************************************************************/

/*********************** END OF FILE *************************/