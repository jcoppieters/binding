// ComApplicationFactory.cpp: implementation of the CComApplicationFactory class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "ComApplicationFactory.h"
#include "ComApplication.h"
#include "RequestForNodeStatus.h"
#include "RenameNodeUnitsEngine.h"
#include "App/AppCfg/AppCfgMM/ConfigMultiMediaFSMMaster.h"
#include "BindingsReadWriteFSM.h"
#include "NodeLCDScreenmapFSMMaster.h"
#include "MemoryDumpMasterFSM.h"
#include "FSMNicErrors.h"

#include "App/AppBrowsePnp/AppBrowsePnP.h"

#include "App\AppCfg\AppCfgUnits\ConfigUnitFSMMaster.h"				// since 0x0560
#include "App\AppCfg\AppCfgMoods\ConfigMoodFSMMaster.h"				// since 0x0560
#include "App\AppCfg\AppCfgIRRX\ConfigIRRXFSMMaster.h"				// since 0x0560
#include "App\AppCfg\AppCfgschedule\ConfigScheduleFSMMaster.h"		// since 0x0560
#include "App\AppCfg\AppCfgCanFtp\ConfigCanFtp.h"					// since 0x0562
#include "App\AppCfg\AppCfgsmap\configsmappageinfofsmmaster.h"		// since 0x0571
#include "App\AppCfg\AppCfgsmap\configsmapmenufsmmaster.h"			// since 0x0571
#include "App\AppCfg\AppCfgsmap\configsmapvroomfsmmaster.h"			// since 0x0610
#include "app\AppCfg\AppCfgGatewayU\ConfigGatewayUFsmMaster.h"		// since 0x0620
#include "app\AppCfg\AppCfgRoomCtrlAV\configroomctrlavfsmmaster.h"	// since 0x0625
#include "app\appcfg\appcfgbosest\configbosestfsmmaster.h"			// since 0x0802
#include "app\appcfg\appcfguictrl\configappuictrlfsmmaster.h"		// since 0x0804
#include "app/appcfg/appcfgsmaplite/appcfgsmaplitefsmmaster.h"		// since 0x0C05

#include "communication/communicationconnectfsm.h"

// ---------------------------------------
#include "TMGlobals.h"
using namespace TMNodeDatabase;
// ---------------------------------------


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CComApplicationFactory* CComApplicationFactory::m_pThis = NULL;
//////////////////////////////////////////////////////////////////////

CComApplicationFactory* CComApplicationFactory::Create(IComApplication* ptr)
{
	if (m_pThis == NULL)
	{
		m_pThis = new CComApplicationFactory(ptr);
	}	
	return m_pThis;
}

void CComApplicationFactory::Destroy()
{
	if (m_pThis != NULL)
	{
		delete m_pThis;
		m_pThis = NULL;
	}
}

CComApplicationFactory* CComApplicationFactory::Get()
{
	ASSERT( m_pThis );
	return m_pThis;
}

//////////////////////////////////////////////////////////////////////
CComApplicationFactory::CComApplicationFactory(IComApplication* ptr) :
m_pComApplication(ptr),
m_pRequestForStatus(0),
m_pRenameEngine(0),
m_pMultiMediaFSMMaster(0),
m_pBindingsRWEngine(0),
m_pScreenmapsUpdateLCD(0),
m_pMemoryDumpMasterFSM(0),
m_pBrowsePnpEngine(0),
m_pModemConnectFSM(0),
m_pNicErrorsFsm(0),
m_pUnitCreateFSMMaster(0),				
m_pMoodUploadFSMMaster(0),				
m_pIRRXUploadFSMMaster(0),				
m_pScheduleUploadFSMMaster(0),			
m_pConfigCanFTP(0),						
m_pConfigSmapPageInfoFSMMaster(0),
m_pConfigSmapMenuFSMMaster(0),
m_pConfigSmapAVRoomFSMMaster(0),
m_pConfigGatewayUFsmMaster(0),		
m_pConfigRoomCtrlAVFsmMaster(0),
m_pConfigBoseSTFsmMaster(0),
m_pConfigAppLedsCfgFsmMaster(0),
m_pAppCfgSmapLiteFSMMaster(0)
{	
	ASSERT(m_pComApplication);
}

CComApplicationFactory::~CComApplicationFactory()
{
	Destroy_RequestForNodeStatussen();
	Destroy_RenameNodeUnitsEngine();
	Destroy_MultiMediaConfigEngine();
	Destroy_BindingsReadWriteEngine();
	Destroy_ScreenmapsUpdateEngine();
	Destroy_MemoryDumpEngine();
	Destroy_BrowsePnpEngine();
	Destroy_ModemConnectEngine();
	Destroy_NicErrorsFSMEngine();
	Destroy_UnitCreateFSMEngine();		
	Destroy_UploadMoodFSMEngine();		
	Destroy_UploadIRRXFSMEngine();		
	Destroy_UploadScheduleFSMEngine();	
	Destroy_ConfigCanFTP();				
	Destroy_ConfigSmapPageInfoFSMEngine();
	Destroy_ConfigSmapMenuFSMEngine();
	Destroy_ConfigSmapAVRoomFSMEngine();
	Destroy_ConfigGatewayUFSMEngine();	
	Destroy_ConfigRoomCtrlAVFSMEngine();
	Destroy_ConfigBoseSTFSMEngine();
	Destroy_ConfigLedsCfgFSMEngine( );
	Destroy_ConfigSmapLiteFSMEngine( );
}


///////////////////////////////////////////////////////////////////////

/**
 * CRequestForNodeStatus : m_pRequestForStatus
 */
CRequestForNodeStatus* CComApplicationFactory::Create_RequestForNodeStatussen()
{
	try
	{
		if (m_pRequestForStatus == NULL)
		{
			m_pRequestForStatus = new CRequestForNodeStatus(m_pComApplication,
														    TMGetRuntimeNodes());	
		}		
	}
	catch(CMemoryException *e)
	{
		e->ReportError();
		e->Delete();
		return NULL;
	}

	return m_pRequestForStatus;
}

void CComApplicationFactory::Destroy_RequestForNodeStatussen()
{
	if (m_pRequestForStatus != NULL)
	{
		delete m_pRequestForStatus;
		m_pRequestForStatus = NULL;
	}
}

CRequestForNodeStatus* CComApplicationFactory::Get_RequestForNodeStatussen()
{
	return m_pRequestForStatus;
}


/**
 * CRenameNodeUnitsEngine : m_pRenameEngine
 */
CRenameNodeUnitsEngine* CComApplicationFactory::Create_RenameNodeUnitsEngine()
{
	try
	{
		if (m_pRenameEngine == NULL)
		{
			m_pRenameEngine = new CRenameNodeUnitsEngine(m_pComApplication,TMGetDocumentNodes());	
		}		
	}
	catch(CMemoryException *e)
	{
		e->ReportError();
		e->Delete();
		return NULL;
	}

	return m_pRenameEngine;
}

void CComApplicationFactory::Destroy_RenameNodeUnitsEngine()
{
	if (m_pRenameEngine != NULL)
	{
		delete m_pRenameEngine;
		m_pRenameEngine = NULL;
	}
}

CRenameNodeUnitsEngine* CComApplicationFactory::Get_RenameNodeUnitsEngine()
{
	return m_pRenameEngine;
}

/**
 * 	CConfigMultiMediaFSMMaster : m_pMultiMediaFSMMaster;
 */
CConfigMultiMediaFSMMaster* CComApplicationFactory::Create_MultiMediaConfigEngine()
{
	try
	{
		if (m_pMultiMediaFSMMaster == NULL)
		{
			m_pMultiMediaFSMMaster = new CConfigMultiMediaFSMMaster(m_pComApplication);									
		}		
	}
	catch(CMemoryException *e)
	{
		e->ReportError();
		e->Delete();
		return NULL;
	}

	return m_pMultiMediaFSMMaster;
}

void CComApplicationFactory::Destroy_MultiMediaConfigEngine()
{
	if (m_pMultiMediaFSMMaster != NULL)
	{
		delete m_pMultiMediaFSMMaster;
		m_pMultiMediaFSMMaster = NULL;
	}
}

CConfigMultiMediaFSMMaster* CComApplicationFactory::Get_MultiMediaConfigEngine()
{
	return m_pMultiMediaFSMMaster;
}

/**
 * CBindingsReadWriteFSM : m_pBindingsRWEngine;
 */
CBindingsReadWriteFSM* CComApplicationFactory::Create_BindingsReadWriteEngine()
{
	try
	{
		if (m_pBindingsRWEngine == NULL)
		{
			m_pBindingsRWEngine = new CBindingsReadWriteFSM(m_pComApplication);									
		}		
	}
	catch(CMemoryException *e)
	{
		e->ReportError();
		e->Delete();
		return NULL;
	}

	return m_pBindingsRWEngine;
}

void CComApplicationFactory::Destroy_BindingsReadWriteEngine()
{
	if (m_pBindingsRWEngine != NULL)
	{
		delete m_pBindingsRWEngine;
		m_pBindingsRWEngine = NULL;
	}
}

CBindingsReadWriteFSM* CComApplicationFactory::Get_BindingsReadWriteEngine()
{
	return m_pBindingsRWEngine;
}

/**
 * 	CNodeLCDScreenmapFSMMaster : m_pScreenmapsUpdateLCD;
 */
CNodeLCDScreenmapFSMMaster* CComApplicationFactory::Create_ScreenmapsUpdateEngine()
{
	try
	{
		if (m_pScreenmapsUpdateLCD == NULL)
		{
			m_pScreenmapsUpdateLCD = new CNodeLCDScreenmapFSMMaster(m_pComApplication);									
		}		
	}
	catch(CMemoryException *e)
	{
		e->ReportError();
		e->Delete();
		return NULL;
	}

	return m_pScreenmapsUpdateLCD;
}
void CComApplicationFactory::Destroy_ScreenmapsUpdateEngine()
{
	if (m_pScreenmapsUpdateLCD != NULL)
	{
		delete m_pScreenmapsUpdateLCD;
		m_pScreenmapsUpdateLCD = NULL;
	}
}
CNodeLCDScreenmapFSMMaster* CComApplicationFactory::Get_ScreenmapsUpdateEngine()
{
	return m_pScreenmapsUpdateLCD;
}

/**
 * CMemoryDumpMasterFSM : m_pMemoryDumpMasterFSM;
 */
CMemoryDumpMasterFSM* CComApplicationFactory::Create_MemoryDumpEngine()
{
	try
	{
		if (m_pMemoryDumpMasterFSM == NULL)
		{
			m_pMemoryDumpMasterFSM = new CMemoryDumpMasterFSM(m_pComApplication);									
		}		
	}
	catch(CMemoryException *e)
	{
		e->ReportError();
		e->Delete();
		return NULL;
	}

	return m_pMemoryDumpMasterFSM;
}
void CComApplicationFactory::Destroy_MemoryDumpEngine()
{
	if (m_pMemoryDumpMasterFSM != NULL)
	{
		delete m_pMemoryDumpMasterFSM;
		m_pMemoryDumpMasterFSM = NULL;
	}
}
CMemoryDumpMasterFSM* CComApplicationFactory::Get_MemoryDumpEngine()
{
	return m_pMemoryDumpMasterFSM;
}

/**
 * CBrowsePnPEngine : m_pBrowsePnpEngine;		
 */
CBrowsePnPEngine* CComApplicationFactory::Create_BrowsePnpEngine()
{
	try
	{
		if (m_pBrowsePnpEngine == NULL)
		{
			m_pBrowsePnpEngine = new CBrowsePnPEngine(m_pComApplication);									
		}		
	}
	catch(CMemoryException *e)
	{
		e->ReportError();
		e->Delete();
		return NULL;
	}

	return m_pBrowsePnpEngine;
}
void CComApplicationFactory::Destroy_BrowsePnpEngine()
{
	if (m_pBrowsePnpEngine != NULL)
	{
		delete m_pBrowsePnpEngine;
		m_pBrowsePnpEngine = NULL;
	}
}

CBrowsePnPEngine* CComApplicationFactory::Get_BrowsePnpEngine()
{
	return m_pBrowsePnpEngine;
}


/**
 * 	CModemConnectFSM : m_pModemConnectFSM;
 */
CCommunicationConnectFsm* CComApplicationFactory::Create_ModemConnectEngine()
{
	try
	{
		if (m_pModemConnectFSM == NULL)
		{
			m_pModemConnectFSM = new CCommunicationConnectFsm(m_pComApplication);									
		}		
	}
	catch(CMemoryException *e)
	{
		e->ReportError();
		e->Delete();
		return NULL;
	}

	return m_pModemConnectFSM;
}
void CComApplicationFactory::Destroy_ModemConnectEngine()
{
	if (m_pModemConnectFSM != NULL)
	{
		delete m_pModemConnectFSM;
		m_pModemConnectFSM = NULL;
	}
}
CCommunicationConnectFsm* CComApplicationFactory::Get_ModemConnectEngine()
{
	return m_pModemConnectFSM;
}


/**
 * CFSMNicErrors : m_pNicErrorsFsm;
 */
CFSMNicErrors* CComApplicationFactory::Create_NicErrorsFSMEngine()
{
	try
	{
		if (m_pNicErrorsFsm == NULL)
		{
			m_pNicErrorsFsm = new CFSMNicErrors(m_pComApplication);									
		}		
	}
	catch(CMemoryException *e)
	{
		e->ReportError();
		e->Delete();
		return NULL;
	}

	return m_pNicErrorsFsm;
}
void CComApplicationFactory::Destroy_NicErrorsFSMEngine()
{
	if (m_pNicErrorsFsm != NULL)
	{
		delete m_pNicErrorsFsm;
		m_pNicErrorsFsm = NULL;
	}
}
CFSMNicErrors* CComApplicationFactory::Get_NicErrorsFSMEngine()
{
	return m_pNicErrorsFsm;
}

/**
 * CConfigUnitFSMMaster : m_pUnitCreateFSMMaster: Since 0x0560
 */
CConfigUnitFSMMaster* CComApplicationFactory::Create_UnitCreateFSMEngine( )
{
	try
	{
		if (m_pUnitCreateFSMMaster == 0)
		{
			m_pUnitCreateFSMMaster = new CConfigUnitFSMMaster(*m_pComApplication);
		}
	}
	catch(CMemoryException *e)
	{
		e->ReportError();
		e->Delete();
		return NULL;
	}
	return m_pUnitCreateFSMMaster;
}
void CComApplicationFactory::Destroy_UnitCreateFSMEngine( )
{
	if (m_pUnitCreateFSMMaster)
	{
		delete m_pUnitCreateFSMMaster;
		m_pUnitCreateFSMMaster = 0;
	}
}
CConfigUnitFSMMaster* CComApplicationFactory::Get_UnitCreateFSMEngine( )
{
	return m_pUnitCreateFSMMaster;	
}

/**
 * CConfigMoodFSMMaster : m_pMoodUploadFSMMaster
 */
CConfigMoodFSMMaster* CComApplicationFactory::Create_UploadMoodFSMEngine( )
{
	try
	{
		if (m_pMoodUploadFSMMaster == 0)
		{
			m_pMoodUploadFSMMaster = new CConfigMoodFSMMaster(*m_pComApplication);
		}
	}
	catch(CMemoryException *e)
	{
		e->ReportError();
		e->Delete();
		return NULL;
	}
	return m_pMoodUploadFSMMaster;
}
void CComApplicationFactory::Destroy_UploadMoodFSMEngine( )
{
	if (m_pMoodUploadFSMMaster)
	{
		delete m_pMoodUploadFSMMaster;
		m_pMoodUploadFSMMaster = 0;
	}
}
CConfigMoodFSMMaster* CComApplicationFactory::Get_UploadMoodFSMEngine( )
{
	return m_pMoodUploadFSMMaster;	
}

/**
 * CConfigIRRXFSMMaster : m_pIRRXUploadFSMMaster
 */
CConfigIRRXFSMMaster* CComApplicationFactory::Create_UploadIRRXFSMEngine( )
{
	try
	{
		if (m_pIRRXUploadFSMMaster == 0)
		{
			m_pIRRXUploadFSMMaster = new CConfigIRRXFSMMaster(*m_pComApplication);
		}
	}
	catch(CMemoryException *e)
	{
		e->ReportError();
		e->Delete();
		return NULL;
	}
	return m_pIRRXUploadFSMMaster;
}
void CComApplicationFactory::Destroy_UploadIRRXFSMEngine( )
{
	if (m_pIRRXUploadFSMMaster)
	{
		delete m_pIRRXUploadFSMMaster;
		m_pIRRXUploadFSMMaster = 0;
	}
}
CConfigIRRXFSMMaster* CComApplicationFactory::Get_UploadIRRXFSMEngine( )
{
	return m_pIRRXUploadFSMMaster;	
}

/**
 * CConfigScheduleFSMMaster : m_pScheduleUploadFSMMaster
 */
CConfigScheduleFSMMaster* CComApplicationFactory::Create_UploadScheduleFSMEngine( )
{
	try
	{
		if (m_pScheduleUploadFSMMaster == 0)
		{
			m_pScheduleUploadFSMMaster = new CConfigScheduleFSMMaster(*m_pComApplication);
		}
	}
	catch(CMemoryException *e)
	{
		e->ReportError();
		e->Delete();
		return NULL;
	}
	return m_pScheduleUploadFSMMaster;
}
void CComApplicationFactory::Destroy_UploadScheduleFSMEngine( )
{
	if (m_pScheduleUploadFSMMaster)
	{
		delete m_pScheduleUploadFSMMaster;
		m_pScheduleUploadFSMMaster = 0;
	}
}
CConfigScheduleFSMMaster* CComApplicationFactory::Get_UploadScheduleFSMEngine( )
{
	return m_pScheduleUploadFSMMaster;	
}

/**
 * CConfigCanFTP : m_pConfigCanFTP
 */
CConfigCanFTP* CComApplicationFactory::Create_ConfigCanFTP( )
{
	try
	{
		if (m_pConfigCanFTP == 0)
		{
			m_pConfigCanFTP = new CConfigCanFTP(*m_pComApplication);
		}
	}
	catch(CMemoryException *e)
	{
		e->ReportError();
		e->Delete();
		return NULL;
	}
	return m_pConfigCanFTP;	 
}
void CComApplicationFactory::Destroy_ConfigCanFTP( )
{
	if (m_pConfigCanFTP)
	{
		delete m_pConfigCanFTP;
		m_pConfigCanFTP = 0;
	}
}
CConfigCanFTP* CComApplicationFactory::Get_ConfigCanFTP( )
{
	return m_pConfigCanFTP;	
}


/**
 * CConfigSmapPageInfoFSMMaster	: m_pConfigSmapPageInfoFSMMaster;
 */
CConfigSmapPageInfoFSMMaster* CComApplicationFactory::Create_ConfigSmapPageInfoFSMEngine( )
{
	try
	{
		if (m_pConfigSmapPageInfoFSMMaster == 0)
		{
			m_pConfigSmapPageInfoFSMMaster = new CConfigSmapPageInfoFSMMaster(*m_pComApplication);
		}
	}
	catch(CMemoryException *e)
	{
		e->ReportError();
		e->Delete();
		return NULL;
	}
	return m_pConfigSmapPageInfoFSMMaster;	 
}

void CComApplicationFactory::Destroy_ConfigSmapPageInfoFSMEngine( )
{
	if (m_pConfigSmapPageInfoFSMMaster)
	{
		delete m_pConfigSmapPageInfoFSMMaster;
		m_pConfigSmapPageInfoFSMMaster = 0;
	}
}

CConfigSmapPageInfoFSMMaster* CComApplicationFactory::Get_ConfigSmapPageInfoFSMEngine( )
{
	return m_pConfigSmapPageInfoFSMMaster;
}	


/**
 * CConfigSmapMenuFSMMaster* m_pConfigSmapMenuFSMMaster;
 */
CConfigSmapMenuFSMMaster* CComApplicationFactory::Create_ConfigSmapMenuFSMEngine( )
{
	try
	{
		if (m_pConfigSmapMenuFSMMaster == 0)
		{
			m_pConfigSmapMenuFSMMaster = new CConfigSmapMenuFSMMaster(*m_pComApplication);
		}
	}
	catch(CMemoryException *e)
	{
		e->ReportError();
		e->Delete();
		return NULL;
	}
	return m_pConfigSmapMenuFSMMaster;	 
}

void CComApplicationFactory::Destroy_ConfigSmapMenuFSMEngine( )
{
	if (m_pConfigSmapMenuFSMMaster)
	{
		delete m_pConfigSmapMenuFSMMaster;
		m_pConfigSmapMenuFSMMaster = 0;
	}
}

CConfigSmapMenuFSMMaster* CComApplicationFactory::Get_ConfigSmapMenuFSMEngine( )
{
	return m_pConfigSmapMenuFSMMaster;
}	

/**
 * CConfigSmapAVRoomFSMMaster*		m_pConfigSmapAVRoomFSMMaster;
 */
CConfigSmapAVRoomFSMMaster* CComApplicationFactory::Create_ConfigSmapAVRoomFSMEngine( )
{
	try
	{
		if (m_pConfigSmapAVRoomFSMMaster == 0)
		{
			m_pConfigSmapAVRoomFSMMaster = new CConfigSmapAVRoomFSMMaster(*m_pComApplication);
		}
	}
	catch(CMemoryException *e)
	{
		e->ReportError();
		e->Delete();
		return NULL;
	}
	return m_pConfigSmapAVRoomFSMMaster;	 
}

void CComApplicationFactory::Destroy_ConfigSmapAVRoomFSMEngine( )
{
	if (m_pConfigSmapAVRoomFSMMaster)
	{
		delete m_pConfigSmapAVRoomFSMMaster;
		m_pConfigSmapAVRoomFSMMaster = 0;
	}
}

CConfigSmapAVRoomFSMMaster* CComApplicationFactory::Get_ConfigSmapAVRoomFSMEngine( )
{
	return m_pConfigSmapAVRoomFSMMaster;
}

/**
 * CConfigGatewayUFsmMaster* m_pConfigGatewayUFsmMaster
 */
CConfigGatewayUFsmMaster* CComApplicationFactory::Create_ConfigGatewayUFSMEngine( )
{
	try
	{
		if (m_pConfigGatewayUFsmMaster == 0)
		{
			m_pConfigGatewayUFsmMaster = new CConfigGatewayUFsmMaster(*m_pComApplication);
		}
	}
	catch(CMemoryException *e)
	{
		e->ReportError();
		e->Delete();
		return NULL;
	}
	return m_pConfigGatewayUFsmMaster;	 
}

void CComApplicationFactory::Destroy_ConfigGatewayUFSMEngine( )
{
	if (m_pConfigGatewayUFsmMaster)
	{
		delete m_pConfigGatewayUFsmMaster;
		m_pConfigGatewayUFsmMaster = 0;
	}
}

CConfigGatewayUFsmMaster* CComApplicationFactory::Get_ConfigGatewayUFSMEngine( )
{
	return m_pConfigGatewayUFsmMaster;
}

/**
 * CConfigRoomCtrlAVFSMMaster* m_pConfigRoomCtrlAVFsmMaster
 */
CConfigRoomCtrlAVFSMMaster* CComApplicationFactory::Create_ConfigRoomCtrlAVFSMEngine( )
{
	try
	{
		if (m_pConfigRoomCtrlAVFsmMaster == 0)
		{
			m_pConfigRoomCtrlAVFsmMaster = new CConfigRoomCtrlAVFSMMaster(*m_pComApplication);
		}
	}
	catch(CMemoryException *e)
	{
		e->ReportError();
		e->Delete();
		return NULL;
	}
	return m_pConfigRoomCtrlAVFsmMaster;	 
}

void CComApplicationFactory::Destroy_ConfigRoomCtrlAVFSMEngine( )
{
	if (m_pConfigRoomCtrlAVFsmMaster)
	{
		delete m_pConfigRoomCtrlAVFsmMaster;
		m_pConfigRoomCtrlAVFsmMaster = 0;
	}
}

CConfigRoomCtrlAVFSMMaster* CComApplicationFactory::Get_ConfigRoomCtrlAVFSMEngine( )
{
	return m_pConfigRoomCtrlAVFsmMaster;
}

/**
 * CConfigBoseStFSMMaster* m_pConfigBoseSTFsmMaster - Since 0x0802
 */
CConfigBoseStFSMMaster*	CComApplicationFactory::Create_ConfigBoseSTFSMEngine( )
{
	try
	{
		if (m_pConfigBoseSTFsmMaster == 0)
		{
			m_pConfigBoseSTFsmMaster = new CConfigBoseStFSMMaster(*m_pComApplication);
		}
	}
	catch(CMemoryException *e)
	{
		e->ReportError();
		e->Delete();
		return NULL;
	}
	return m_pConfigBoseSTFsmMaster;
}

void CComApplicationFactory::Destroy_ConfigBoseSTFSMEngine( )
{
	if (m_pConfigBoseSTFsmMaster)
	{
		delete m_pConfigBoseSTFsmMaster;
		m_pConfigBoseSTFsmMaster = 0;
	}
}

CConfigBoseStFSMMaster* CComApplicationFactory::Get_ConfigBoseSTFSMEngine( )
{
	return m_pConfigBoseSTFsmMaster;
}

/**
 * CConfigAppUiCtrlFSMMaster* m_pConfigAppLedsCfgFsmMaster; - since 0x0804
 */
CConfigAppUiCtrlFSMMaster* CComApplicationFactory::Create_ConfigLedsCfgFSMEngine( )
{
	try
	{
		if (m_pConfigAppLedsCfgFsmMaster == 0)
		{
			m_pConfigAppLedsCfgFsmMaster = new CConfigAppUiCtrlFSMMaster(*m_pComApplication);
		}
	}
	catch(CMemoryException *e)
	{
		e->ReportError();
		e->Delete();
		return NULL;
	}
	return m_pConfigAppLedsCfgFsmMaster;
}

void CComApplicationFactory::Destroy_ConfigLedsCfgFSMEngine( )
{
	if (m_pConfigAppLedsCfgFsmMaster)
	{
		delete m_pConfigAppLedsCfgFsmMaster;
		m_pConfigAppLedsCfgFsmMaster = 0;
	}
}

CConfigAppUiCtrlFSMMaster* CComApplicationFactory::Get_ConfigLedsCfgFSMEngine( )
{
	return m_pConfigAppLedsCfgFsmMaster;
}


/**
 * CAppCfgSmapLiteFSMMaster* m_pAppCfgSmapLiteFSMMaster - Since 0x0C05
 */ 

CAppCfgSmapLiteFSMMaster* CComApplicationFactory::Create_ConfigSmapLiteFSMEngine( )
{
	try
	{
		if (m_pAppCfgSmapLiteFSMMaster == 0)
		{
			m_pAppCfgSmapLiteFSMMaster = new CAppCfgSmapLiteFSMMaster(*m_pComApplication);
		}
	}
	catch(CMemoryException *e)
	{
		e->ReportError();
		e->Delete();
		return NULL;
	}
	return m_pAppCfgSmapLiteFSMMaster;
}

void CComApplicationFactory::Destroy_ConfigSmapLiteFSMEngine( )
{
	if (m_pAppCfgSmapLiteFSMMaster)
	{
		delete m_pAppCfgSmapLiteFSMMaster;
		m_pAppCfgSmapLiteFSMMaster = 0;
	}
}

CAppCfgSmapLiteFSMMaster* CComApplicationFactory::Get_ConfigSmapLiteFSMEngine( )
{
	return m_pAppCfgSmapLiteFSMMaster;
}