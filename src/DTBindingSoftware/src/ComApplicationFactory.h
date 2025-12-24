// ComApplicationFactory.h: interface for the CComApplicationFactory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMAPPLICATIONFACTORY_H__3CFD3ACF_BA5D_4B86_80FF_6F99CB1EE095__INCLUDED_)
#define AFX_COMAPPLICATIONFACTORY_H__3CFD3ACF_BA5D_4B86_80FF_6F99CB1EE095__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// ---------------------------------------------------
#include "ClassFactory.h"
// ---------------------------------------------------
class IComApplication;
class CRequestForNodeStatus;
class CRenameNodeUnitsEngine;
class CConfigMultiMediaFSMMaster;
class CBindingsReadWriteFSM;
class CNodeLCDScreenmapFSMMaster;
class CMemoryDumpMasterFSM;
class CBrowsePnPEngine;
class CCommunicationConnectFsm;
class CFSMNicErrors;
class CConfigUnitFSMMaster;
class CConfigMoodFSMMaster;
class CConfigIRRXFSMMaster;
class CConfigScheduleFSMMaster;
class CConfigCanFTP;
class CConfigSmapPageInfoFSMMaster;
class CConfigSmapMenuFSMMaster;
class CConfigSmapAVRoomFSMMaster;
class CConfigGatewayUFsmMaster;
class CConfigRoomCtrlAVFSMMaster;
class CConfigBoseStFSMMaster;
class CConfigAppUiCtrlFSMMaster;
class CAppCfgSmapLiteFSMMaster;		// since 0x0C05
// ---------------------------------------------------

// ---------------------------------------------------
class CComApplicationFactory  
{
private:
	CRequestForNodeStatus*			m_pRequestForStatus;
	CRenameNodeUnitsEngine*			m_pRenameEngine;
	CConfigMultiMediaFSMMaster*		m_pMultiMediaFSMMaster;
	CBindingsReadWriteFSM*			m_pBindingsRWEngine;
	CNodeLCDScreenmapFSMMaster*		m_pScreenmapsUpdateLCD;
	CMemoryDumpMasterFSM*			m_pMemoryDumpMasterFSM;
	CBrowsePnPEngine*				m_pBrowsePnpEngine;		
	CCommunicationConnectFsm*		m_pModemConnectFSM;
	CFSMNicErrors*					m_pNicErrorsFsm;
	CConfigUnitFSMMaster*			m_pUnitCreateFSMMaster;				// since 0x0560
	CConfigMoodFSMMaster*			m_pMoodUploadFSMMaster;				// since 0x0560
	CConfigIRRXFSMMaster*			m_pIRRXUploadFSMMaster;				// since 0x0560
	CConfigScheduleFSMMaster*		m_pScheduleUploadFSMMaster;			// since 0x0560
	CConfigCanFTP*					m_pConfigCanFTP;					// since 0x0562
	CConfigSmapPageInfoFSMMaster*	m_pConfigSmapPageInfoFSMMaster;		// since 0x0571
	CConfigSmapMenuFSMMaster*		m_pConfigSmapMenuFSMMaster;			// since 0x0571
	CConfigSmapAVRoomFSMMaster*		m_pConfigSmapAVRoomFSMMaster;		// since 0x0610
	CConfigGatewayUFsmMaster*		m_pConfigGatewayUFsmMaster;			// since 0x0620
	CConfigRoomCtrlAVFSMMaster*		m_pConfigRoomCtrlAVFsmMaster;		// since 0x0625
	CConfigBoseStFSMMaster*			m_pConfigBoseSTFsmMaster;			// since 0x0802
	CConfigAppUiCtrlFSMMaster*		m_pConfigAppLedsCfgFsmMaster;		// since 0x0804
	CAppCfgSmapLiteFSMMaster*		m_pAppCfgSmapLiteFSMMaster;			// since 0x0C05

protected:	
	static CComApplicationFactory* m_pThis;
	IComApplication* const m_pComApplication;

public:
	static CComApplicationFactory* Create(IComApplication* ptr);
	static void Destroy();
	static CComApplicationFactory* Get();

protected:
	CComApplicationFactory(IComApplication* ptr);
	virtual ~CComApplicationFactory();

public:
	/**
	 * CRequestForNodeStatus : m_pRequestForStatus
	 */
	CRequestForNodeStatus* Create_RequestForNodeStatussen();
	void Destroy_RequestForNodeStatussen();
	CRequestForNodeStatus* Get_RequestForNodeStatussen();

	/**
	 * CRenameNodeUnitsEngine : m_pRenameEngine
	 */
	CRenameNodeUnitsEngine* Create_RenameNodeUnitsEngine();
	void Destroy_RenameNodeUnitsEngine();
	CRenameNodeUnitsEngine* Get_RenameNodeUnitsEngine();

	/**
	 * 	CConfigMultiMediaFSMMaster : m_pMultiMediaFSMMaster;
	 */
	CConfigMultiMediaFSMMaster* Create_MultiMediaConfigEngine();
	void Destroy_MultiMediaConfigEngine();
	CConfigMultiMediaFSMMaster* Get_MultiMediaConfigEngine();

	/**
	 * CBindingsReadWriteFSM : m_pBindingsRWEngine;
	 */
	CBindingsReadWriteFSM* Create_BindingsReadWriteEngine();
	void Destroy_BindingsReadWriteEngine();
	CBindingsReadWriteFSM* Get_BindingsReadWriteEngine();

	/**
	 * 	CNodeLCDScreenmapFSMMaster : m_pScreenmapsUpdateLCD;
	 */
	CNodeLCDScreenmapFSMMaster* Create_ScreenmapsUpdateEngine();
	void Destroy_ScreenmapsUpdateEngine();
	CNodeLCDScreenmapFSMMaster* Get_ScreenmapsUpdateEngine();

	/**
	 * CMemoryDumpMasterFSM : m_pMemoryDumpMasterFSM;
	 */
	CMemoryDumpMasterFSM* Create_MemoryDumpEngine();
	void Destroy_MemoryDumpEngine();
	CMemoryDumpMasterFSM* Get_MemoryDumpEngine();

	/**
	 * CBrowsePnPEngine : m_pBrowsePnpEngine;		
	 */
	CBrowsePnPEngine* Create_BrowsePnpEngine();
	void Destroy_BrowsePnpEngine();
	CBrowsePnPEngine* Get_BrowsePnpEngine();

	/**
	 * 	CCommunicationConnectFsm : m_pModemConnectFSM;
	 */
	CCommunicationConnectFsm* Create_ModemConnectEngine();
	void Destroy_ModemConnectEngine();
	CCommunicationConnectFsm* Get_ModemConnectEngine();

	/**
	 * CFSMNicErrors : m_pNicErrorsFsm;
	 */
	CFSMNicErrors* Create_NicErrorsFSMEngine();
	void Destroy_NicErrorsFSMEngine();
	CFSMNicErrors* Get_NicErrorsFSMEngine();

	/**
	 * CConfigUnitFSMMaster : m_pUnitCreateFSMMaster
	 */
	CConfigUnitFSMMaster* Create_UnitCreateFSMEngine( );
	void Destroy_UnitCreateFSMEngine( );
	CConfigUnitFSMMaster* Get_UnitCreateFSMEngine( );

	/**
	 * CConfigMoodFSMMaster : m_pMoodUploadFSMMaster
	 */
	CConfigMoodFSMMaster* Create_UploadMoodFSMEngine( );
	void Destroy_UploadMoodFSMEngine( );
	CConfigMoodFSMMaster* Get_UploadMoodFSMEngine( );

	/**
	 * CConfigIRRXFSMMaster : m_pIRRXUploadFSMMaster
	 */
	CConfigIRRXFSMMaster* Create_UploadIRRXFSMEngine( );
	void Destroy_UploadIRRXFSMEngine( );
	CConfigIRRXFSMMaster* Get_UploadIRRXFSMEngine( );

	/**
	 * CConfigScheduleFSMMaster : m_pScheduleUploadFSMMaster
	 */
	CConfigScheduleFSMMaster* Create_UploadScheduleFSMEngine( );
	void Destroy_UploadScheduleFSMEngine( );
	CConfigScheduleFSMMaster* Get_UploadScheduleFSMEngine( );	
	
	/**
	 * CConfigCanFTP : m_pConfigCanFTP
	 */
	CConfigCanFTP* Create_ConfigCanFTP( );
	void Destroy_ConfigCanFTP( );
	CConfigCanFTP* Get_ConfigCanFTP( );	

	/**
	 * CConfigSmapPageInfoFSMMaster	: m_pConfigSmapPageInfoFSMMaster;
	 */
	CConfigSmapPageInfoFSMMaster* Create_ConfigSmapPageInfoFSMEngine( );
	void Destroy_ConfigSmapPageInfoFSMEngine( );
	CConfigSmapPageInfoFSMMaster* Get_ConfigSmapPageInfoFSMEngine( );	

	/**
	 * CConfigSmapMenuFSMMaster* m_pConfigSmapMenuFSMMaster;
	 */
	CConfigSmapMenuFSMMaster* Create_ConfigSmapMenuFSMEngine( );
	void Destroy_ConfigSmapMenuFSMEngine( );
	CConfigSmapMenuFSMMaster* Get_ConfigSmapMenuFSMEngine( );	

	/**
	 * CConfigSmapAVRoomFSMMaster* m_pConfigSmapAVRoomFSMMaster;
	 */
	CConfigSmapAVRoomFSMMaster* Create_ConfigSmapAVRoomFSMEngine( );
	void Destroy_ConfigSmapAVRoomFSMEngine( );
	CConfigSmapAVRoomFSMMaster* Get_ConfigSmapAVRoomFSMEngine( );

	/**
	 * CConfigGatewayUFsmMaster* m_pConfigGatewayUFsmMaster
	 */
	CConfigGatewayUFsmMaster* Create_ConfigGatewayUFSMEngine( );
	void Destroy_ConfigGatewayUFSMEngine( );
	CConfigGatewayUFsmMaster* Get_ConfigGatewayUFSMEngine( );

	/**
	 * CConfigRoomCtrlAVFSMMaster* m_pConfigRoomCtrlAVFsmMaster
	 */
	CConfigRoomCtrlAVFSMMaster* Create_ConfigRoomCtrlAVFSMEngine( );
	void Destroy_ConfigRoomCtrlAVFSMEngine( );
	CConfigRoomCtrlAVFSMMaster* Get_ConfigRoomCtrlAVFSMEngine( );

	/**
	 * CConfigBoseStFSMMaster* m_pConfigBoseSTFsmMaster; - Since 0x0802
	 */
	CConfigBoseStFSMMaster*	Create_ConfigBoseSTFSMEngine( );
	void Destroy_ConfigBoseSTFSMEngine( );
	CConfigBoseStFSMMaster* Get_ConfigBoseSTFSMEngine( );

	/**
	 * CConfigAppUiCtrlFSMMaster* m_pConfigAppLedsCfgFsmMaster; - since 0x0804
	 */
	CConfigAppUiCtrlFSMMaster* Create_ConfigLedsCfgFSMEngine( );
	void Destroy_ConfigLedsCfgFSMEngine( );
	CConfigAppUiCtrlFSMMaster* Get_ConfigLedsCfgFSMEngine( );

	/**
	 * CAppCfgSmapLiteFSMMaster* m_pAppCfgSmapLiteFSMMaster - Since 0x0C05
	 */ 
	 CAppCfgSmapLiteFSMMaster* Create_ConfigSmapLiteFSMEngine( );
	 void Destroy_ConfigSmapLiteFSMEngine( );
	 CAppCfgSmapLiteFSMMaster* Get_ConfigSmapLiteFSMEngine( );
};

#endif // !defined(AFX_COMAPPLICATIONFACTORY_H__3CFD3ACF_BA5D_4B86_80FF_6F99CB1EE095__INCLUDED_)




















