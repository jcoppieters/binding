// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__31536D49_354B_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_MAINFRM_H__31536D49_354B_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "views/solver/propertywizard.h"					// Nodig voor de WizardTask_t
#include "views/intro/PropertySheetNewAppContext.h"			// Nodig voor PropertyPageNewAppMode_t

//////////////////////////////////////////////////////////////////////////////////

class CPropertyWizard;
class CDlgCommTracer;
class CBindingEntry;
class CDlgBindingUnitProp;
class CDlgBindingOperator;
class CMyException;
class AppUDPServerCfg;


// Deze bevat de login gegevens die opgeslagen worden in projectinfo.dat.
// AppCfgRemote library.
namespace DUOTECNO {
	class AppRemoteCfgLoginInfo;		// Library...
	class AppRemoteCfgNodes;			// Library...
};

//////////////////////////////////////////////////////////////////////////////////

class CMainFrame : public CFrameWnd
{
	friend class CQHSMApplicationGlobal;
	friend class CQHSMBindingConversionFromNodes;

private:
	void ClearViews(void);

	// sedert V16.49: Mogelijkheid om een node adres mee te geven.
	// De boom structuur van de nodes/units zal enkel deze node updaten. (inclusief units/bindings.)
	void UpdateViews(int nodeAddress = -1);		

protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:
		
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual void ActivateFrame(int nCmdShow = -1);
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL
// Implementation
public:
	virtual ~CMainFrame();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:	
	CBindingEntry* GetBindingEntry(void);		
	void showBindingsForm( );	/* TM20120830 */

protected:
	//void DisplayUploadScreenmapInfo();
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnButtonViewBindings();
	afx_msg void OnButtonViewNodes();
	afx_msg void OnButtonViewUnits();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnButtonAnd();
	afx_msg void OnButtonOr();
	afx_msg void OnButtonTimer();
	afx_msg void OnButtonXor();
	afx_msg void OnDebugBindingconversion();
	afx_msg void OnButtonAdd();
	afx_msg void OnButtonDelete();
	afx_msg void OnButtonNew();
	afx_msg void OnButtonDisconnect();
	afx_msg void OnUpdateButtonDisconnect(CCmdUI* pCmdUI);
	afx_msg void OnButtonConnect();
	afx_msg void OnUpdateButtonConnect(CCmdUI* pCmdUI);
	afx_msg void OnButtonTracer();
	afx_msg void OnUpdateButtonViewBindings(CCmdUI* pCmdUI);
	afx_msg void OnUpdateButtonViewNodes(CCmdUI* pCmdUI);
	afx_msg void OnUpdateButtonViewUnits(CCmdUI* pCmdUI);
	afx_msg void OnUpdateButtonAnd(CCmdUI* pCmdUI);
	afx_msg void OnUpdateButtonOr(CCmdUI* pCmdUI);
	afx_msg void OnUpdateButtonTimer(CCmdUI* pCmdUI);
	afx_msg void OnUpdateButtonXor(CCmdUI* pCmdUI);
	afx_msg void OnUpdateButtonAdd(CCmdUI* pCmdUI);
	afx_msg void OnUpdateButtonNew(CCmdUI* pCmdUI);
	afx_msg void OnUpdateButtonDelete(CCmdUI* pCmdUI);
	afx_msg void OnButtonBindingsSendall();
	afx_msg void OnUpdateButtonBindingsSendall(CCmdUI* pCmdUI);
	afx_msg void OnButtonBindingsSendmodified();
	afx_msg void OnUpdateButtonBindingsSendmodified(CCmdUI* pCmdUI);
	afx_msg void OnViewSortnodesbyname();
	afx_msg void OnViewSortnodesbyphaddress();
	afx_msg void OnViewSortnodesbylogicaladdress();
	afx_msg void OnViewSortBindings();
	afx_msg void OnButtonCollapse();
	afx_msg void OnFileExport();
	afx_msg void OnButtonExpand();
	afx_msg void OnButtonSortUnits();
	afx_msg void OnButtonSortNodes();
	afx_msg void OnViewUnitsbyaddress();
	afx_msg void OnViewUnitsbyname();
	afx_msg void OnViewUnitsonlyname();
	afx_msg void OnUpdateButtonSortNodes(CCmdUI* pCmdUI);
	afx_msg void OnUpdateButtonSortUnits(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewSortBindings(CCmdUI* pCmdUI);
	afx_msg void OnUpdateButtonCollapse(CCmdUI* pCmdUI);
	afx_msg void OnUpdateButtonExpand(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewUnitsonlyname(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewUnitsbyname(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewUnitsbyaddress(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewSortnodesbylogicaladdress(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewSortnodesbyname(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewSortnodesbyphaddress(CCmdUI* pCmdUI);
	afx_msg void OnViewSortnodesonlyname();
	afx_msg void OnUpdateViewSortnodesonlyname(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTreeviewBindingsNameandnumber(CCmdUI* pCmdUI);
	afx_msg void OnTreeviewBindingsNameandnumber();
	afx_msg void OnTreeviewBindingsNumberandname();
	afx_msg void OnUpdateTreeviewBindingsNumberandname(CCmdUI* pCmdUI);
	afx_msg void OnTreeviewBindingsOnlyname();
	afx_msg void OnUpdateTreeviewBindingsOnlyname(CCmdUI* pCmdUI);
	afx_msg void OnButtonAuto();
	afx_msg void OnUpdateButtonAuto(CCmdUI* pCmdUI);
	afx_msg void OnDimmersOnoff();
	afx_msg void OnDimmersPirtimed();
	afx_msg void OnUpdateDimmersPirtimed(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDimmersOnoff(CCmdUI* pCmdUI);
	afx_msg void OnRelaisOnoff();
	afx_msg void OnRelaisPirtimedon();
	afx_msg void OnUpdateRelaisOnoff(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRelaisPirtimedon(CCmdUI* pCmdUI);
	afx_msg void OnAutobindingEnable();
	afx_msg void OnUpdateAutobindingEnable(CCmdUI* pCmdUI);
	afx_msg void OnSensCooling();
	afx_msg void OnUpdateSensCooling(CCmdUI* pCmdUI);
	afx_msg void OnSensHeating();
	afx_msg void OnUpdateSensHeating(CCmdUI* pCmdUI);
	afx_msg void OnMoodsLong();
	afx_msg void OnUpdateMoodsLong(CCmdUI* pCmdUI);
	afx_msg void OnMoodsShort();
	afx_msg void OnUpdateMoodsShort(CCmdUI* pCmdUI);
	afx_msg void OnConfiguration();
	afx_msg void OnUpdateConfiguration(CCmdUI* pCmdUI);
	afx_msg void OnMultimedia();
	afx_msg void OnClose();
	afx_msg void OnModemstatus();
	afx_msg void OnUpdateModemstatus(CCmdUI* pCmdUI);
	afx_msg void OnNetworkCommunication();
	afx_msg void OnNetworkPhbrowse();
	afx_msg void OnNetworkRequeststates();
	afx_msg void OnNetworkAutopnp();
	afx_msg void OnNetworkReadallbindings();
	afx_msg void OnNetworkWriteallbindings();
	afx_msg void OnToolsImport();
	afx_msg void OnToolsConversion();
	afx_msg void OnToolsNicmedium();
	afx_msg void OnUpdateToolsNicmedium(CCmdUI* pCmdUI);
	afx_msg void OnNetworkMessagetransfer();
	afx_msg void OnUpdateNetworkMessagetransfer(CCmdUI* pCmdUI);
	afx_msg void OnDebugShowTaskdialog();
	afx_msg void OnUpdateDebugShowTaskdialog(CCmdUI* pCmdUI);
	afx_msg void OnViewModeAdvanced();
	afx_msg void OnUpdateViewModeAdvanced(CCmdUI* pCmdUI);
	afx_msg void OnViewModeNormal();
	afx_msg void OnUpdateViewModeNormal(CCmdUI* pCmdUI);
	afx_msg void OnViewModeSimple();
	afx_msg void OnUpdateViewModeSimple(CCmdUI* pCmdUI);
	afx_msg void OnHelpBindingexamples();
	afx_msg void OnDebugGeneral();
	afx_msg void OnUpdateDebugGeneral(CCmdUI* pCmdUI);
	afx_msg void OnNetworkStatus();
	afx_msg void OnUpdateNetworkStatus(CCmdUI* pCmdUI);
	afx_msg void OnNetworkCanftp();
	afx_msg void OnUpdateNetworkCanftp(CCmdUI* pCmdUI);
	afx_msg void OnToolsConfigurator();
	afx_msg void OnUpdateToolsConfigurator(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMultimedia(CCmdUI* pCmdUI);
	afx_msg void OnUpdateHelpBindingexamples(CCmdUI* pCmdUI);
	afx_msg void OnAutobindingAdd();
	afx_msg void OnUpdateAutobindingAdd(CCmdUI* pCmdUI);
	afx_msg void OnViewUnitExtProp();
	afx_msg void OnUpdateViewUnitExtProp(CCmdUI *pCmdUI);
	afx_msg void OnViewIrtxcodes();
	afx_msg void OnUpdateViewIrtxcodes(CCmdUI *pCmdUI);
	afx_msg void OnButtonEngineerCanLog();
	afx_msg void OnUpdateButtonEngineerCanLog(CCmdUI* pCmdUI);
	//}}AFX_MSG
	afx_msg void OnUpdateIndicatorConnected(CCmdUI *pCmdUI);
	afx_msg void OnUpdateIndicatorCommunication(CCmdUI *pCmdUI);
	afx_msg void OnHelpModbusfunctions();
	afx_msg void OnUpdateHelpModbusfunctions(CCmdUI *pCmdUI);
	afx_msg void OnUpdateMenuToolsRemoteCfg(CCmdUI *pCmdUI);
	afx_msg void OnMenuToolsRemoteCfg();
	afx_msg void OnAutoaudioSimple();
	afx_msg void OnAutoaudioSrcinc();
	afx_msg void OnUpdateAutoaudioSimple(CCmdUI *pCmdUI);
	afx_msg void OnUpdateAutoaudioSrcinc(CCmdUI *pCmdUI);
	afx_msg void OnViewOptionsSoftwareUpdate();
	afx_msg void OnUpdateViewOptionsSoftwareUpdate(CCmdUI *pCmdUI);
	afx_msg void OnHelpSoftwareupdates();
	afx_msg void OnNetworkLanDevices();
	afx_msg void OnNetworkLanSettings();
	afx_msg void OnHelpManualHardware();
	afx_msg void OnHelpManualSoftwarePart1();
	afx_msg void OnHelpManualSoftwarePart2();
	afx_msg void OnHelpManualSoftwareExpert();
	afx_msg void OnUpdateHelpManualHardware(CCmdUI* pCmdUI);
	afx_msg void OnUpdateHelpManualSoftwarePart1(CCmdUI* pCmdUI);
	afx_msg void OnUpdateHelpManualSoftwarePart2(CCmdUI* pCmdUI);
	afx_msg void OnUpdateHelpManualSoftwareExpert(CCmdUI* pCmdUI);
	afx_msg void OnViewEnergymanagementconfig();
	afx_msg void OnViewDaliBusconfig();
	afx_msg void OnUpdateViewDaliBusconfig(CCmdUI* pCmdUI);	
	afx_msg BOOL OnToolTipNeedText(UINT id, NMHDR *pNMHDR, LRESULT *pResult);

//	DECLARE_EVENTSINK_MAP()
	DECLARE_MESSAGE_MAP()

	LONG onApplicationConfiguratorDaliOpenCfg(WPARAM wParam, LPARAM lParam);

	LONG OnNodeNameChanged(WPARAM wParam,LPARAM lParam);
	LONG OnUnitNameChanged(WPARAM wParam,LPARAM lParam);

	// Send by CNodeUnitListCtrl 
	LONG OnShowNodeProperties(WPARAM wParam,LPARAM lParam);
	LONG OnShowUnitProperties(WPARAM wParam,LPARAM lParam);	
	LONG OnShowNodeUDPLanCfg(WPARAM wParam,LPARAM lParam);

	LONG OnUnitStatusChanged(WPARAM wParam,LPARAM lParam);
	LONG OnNodeStatusChanged(WPARAM wParam,LPARAM lParam);
	LONG onNodeEthernetNwkStatus(WPARAM wParam,LPARAM lParam);		// sedert V16.03
	
	LONG OnDisplayMessage(WPARAM wParam,LPARAM lParam);
	LONG OnClearMessages(WPARAM wParam,LPARAM lParam);
	LONG OnDisplayTracerMessageError(WPARAM wParam,LPARAM lParam);

	LONG OnTreeNodeCollapsed(WPARAM wParam,LPARAM lParam);
	LONG OnTreeNodeExpanded(WPARAM wParam,LPARAM lParam);

	LONG OnShowDlgUnitProperties(WPARAM wParam,LPARAM lParam);

	LONG OnAddFlowAddUnit(WPARAM wParam,LPARAM lParam);
	LONG OnAddFlowDraggingUnit(WPARAM wParam,LPARAM lParam);
	LONG OnAddFlowDropUnit(WPARAM wParam,LPARAM lParam);
	LONG OnAddFlowRemoveControl(WPARAM wParam,LPARAM lParam);

	LONG OnBindingEntryAdd(WPARAM wParam,LPARAM lParam);
	LONG OnBindingEntryDelete(WPARAM wParam,LPARAM lParam);
	LONG OnBindingEntryEdit(WPARAM wParam,LPARAM lParam);
	LONG OnBindingEntryAuto(WPARAM wParam,LPARAM lParam);

	LONG OnShowDlgBindingUnit(WPARAM wParam,LPARAM lParam);
	LONG OnDestroyDlgBindingUnit(WPARAM wParam,LPARAM lParam);
	LONG OnShowDlgBindingTimer(WPARAM wParam,LPARAM lParam);

	LONG OnBindingDatabaseNameChanged(WPARAM wParam,LPARAM lParam);

	// BROWSE + PNP EVENTS ...
	LONG OnBrowsePnpBrowseOk(WPARAM wParam,LPARAM lParam);
	LONG OnBrowsePnpPnpOK(WPARAM wParam,LPARAM lParam);
	LONG OnBrowsePnpError(WPARAM wParam,LPARAM lParam);
	LONG OnBrowsePnpCancel(WPARAM wParam,LPARAM lParam);
	LONG OnBrowsePnpNoResponse(WPARAM wParam,LPARAM lParam);
	LONG OnBrowsePnpDuplicates(WPARAM wParam,LPARAM lParam);
	LONG OnBrowsePnpFactoryAddress(WPARAM wParam,LPARAM lParam);

	// BINDINGS READ/WRITE EVENTS ....
	LONG OnBindingsNodeEraseStart(WPARAM wParam,LPARAM lParam);
	LONG OnBindingsNodeEraseDone(WPARAM wParam,LPARAM lParam);
	LONG OnBindingsNodeEraseError(WPARAM wParam,LPARAM lParam);
	LONG OnBindingsNodeWriteStart(WPARAM wParam,LPARAM lParam);
	LONG OnBindingsNodeWriteDone(WPARAM wParam,LPARAM lParam);
	LONG OnBindingsNodeWriteError(WPARAM wParam,LPARAM lParam);
	LONG OnBindingsNodeReadStart(WPARAM wParam,LPARAM lParam);
	LONG OnBindingsNodeReadDone(WPARAM wParam,LPARAM lParam);
	LONG OnBindingsNodeReadError(WPARAM wParam,LPARAM lParam);
	LONG OnBindingsNodeFSMWriteDone(WPARAM wParam,LPARAM lParam);
	LONG OnBindingsNodeFSMReadDone(WPARAM wParam,LPARAM lParam);
	LONG OnApplicationFileNewPre(WPARAM wParam,LPARAM lParam);
	LONG OnApplicationFileNew(WPARAM wParam,LPARAM lParam);
	LONG OnApplicationFileSavePre(WPARAM wParam,LPARAM lParam);
	LONG OnApplicationFileSavePost(WPARAM wParam,LPARAM lParam);
	LONG OnApplicationTaskDone(WPARAM wParam,LPARAM lParam);
	LONG OnApplicationTaskCancel(WPARAM wParam,LPARAM lParam);
	LONG OnApplicationTaskError(WPARAM wParam,LPARAM lParam);
	LONG OnApplicationNotifyBindingAdd(WPARAM wParam,LPARAM lParam);

	//Multimedia config ..		
	LONG OnMMConfigFSMDoneOk(WPARAM wParam,LPARAM lParam);
	LONG OnMMConfigFSMDoneError(WPARAM wParam,LPARAM lParam);
	LONG OnMMConfigFSMUserCancel(WPARAM wParam,LPARAM lParam);

	// Generic Task: Done,Error,Cancelled
	// - Request For Node Statussen...
	LONG OnWmGenericTaskDone(WPARAM wParam,LPARAM lParam);
	LONG OnWmGenericTaskError(WPARAM wParam,LPARAM lParam);
	LONG OnWmGenericTaskCancelled(WPARAM wParam,LPARAM lParam);

	// Communication Notifications
	LONG OnCommunicationMessageReceived(WPARAM wParam,LPARAM lParam);
	LONG OnCommunicationMessageSend(WPARAM wParam,LPARAM lParam);
	LONG OnCommunicationCommandReceived(WPARAM wParam,LPARAM lParam);
	LONG OnCommunicationError(WPARAM wParam,LPARAM lParam);
	// LONG OnCommunicationEvMessageError(WPARAM wParam,LPARAM lParam);

	LONG OnCommunicationModemStatus(WPARAM wParam,LPARAM lParam);
	LONG OnCommunicationConnectionLost(WPARAM wParam,LPARAM lParam);
	LONG OnCommunicationModemLoggerStatus(WPARAM wParam,LPARAM lParam);

	// Convert From Nodes ...
	LONG OnWmConvertFromNodesEvent(WPARAM wParam,LPARAM lParam);
	LONG OnWmConvertFromNodesInitialise(WPARAM wParam,LPARAM lParam);

	LONG OnApplicationOnlineIdle(WPARAM wParam,LPARAM lParam);
	LRESULT OnWmQhsmInitialise(WPARAM wParam,LPARAM lParam);
	LRESULT OnWmQhsmEvent(WPARAM wParam,LPARAM lParam);
	LRESULT OnPropertySheetEnter(WPARAM wParam,LPARAM lParam);
	LRESULT OnPropertySheetExit(WPARAM wParam,LPARAM lParam);
	LONG OnApplicationUserMessage(WPARAM wParam,LPARAM lParam);
	LONG OnShowDialogFirmwareUpdate(WPARAM wParam,LPARAM lParam);		// Firmware update dialoog tonen.
	LONG OnApplicationNodeShowServices(WPARAM wParam,LPARAM lParam);

	// Sedert V16.40
	LONG onApplicationConfiguratorDaliReload(WPARAM wParam,LPARAM lParam);	// WM_APPLICATION_CONFIGURATOR_DALI_RELOAD + node address

	// Sedert V16.43: Mogelijkheid om een individuele node te browsen.
	LONG onApplicationBrowseSingleNode(WPARAM wParam, LPARAM lParam);			// WM_APPLICATION_BROWSE_SINGLE_NODE
	LONG OnApplicationUnitConfigDone(WPARAM wParam, LPARAM lParam);				// WM_APPLICATION_UNITCONFIG_DONE
	LONG OnApplicationDaliBridgeConfigDone(WPARAM wParam, LPARAM lParam);		// WM_APPLICATION_DALIBRIDGECONFIG_DONE

	// Sedert V16.43: Ontvangen van opdrachten vanuit de configurator.
	LONG OnCopyData(WPARAM wParam, LPARAM lParam);

	// Sedert V16.44
	LONG OnApplicationNotifyAssignedNodeAddress(WPARAM wParam, LPARAM lParam);				// WM_APPLICATION_NOTIFY_ASSIGNED_LA

	// sedert V16.47: zie CHANGES_UPLOAD_UNITS
	LONG OnApplicationNotifyProblemSolveRestoredNodeAddress(WPARAM wParam, LPARAM lParam);	// WM_NOTIFY_PROBLEMSOLVED_RESTORED_NODEADDRESS

	BOOL findNodeIPAddress(unsigned long ulPhysicalNodeAddress,BYTE bNodeAddress,CString* pstrIPAddress);
	void DisplayScrollMessage(const CString& szMessage);
	void ClearScrollView(void);
	void SetModifiedFlag(BOOL flag = TRUE);
	void DisplayScrollBindingInfo(CBindingEntry *pEntry);
	int InitialiseApplication(LPCREATESTRUCT lpCreateStruct);
	void HandleFatalError(CMyException *e = NULL);

	// Na het uitvoeren van de browse for nodes (request for states)
	void CompareNodeDatabases(void);
	void CompareNodes(int nodeAddress);

	BOOL ShowBrowseInfo(void);
    void ShowNodesView(void);
	// Return waarde TRUE wanneer er op de 'finish' knop gedrukt werd
	BOOL showPropertySheetNewApp(PropertyPageNewAppMode_t mode);
	BOOL ShowApplicationStartDialog(void);
	void AddToCommunicationTracer(const CString& s);
	void AddToErrorTracer(const CString& s);
	void AddToEngineerCanLogger(const CString& s);
	BOOL ShowProblemSolveWizard(BOOL processDatabaseActions,WizardTasks_t task=WIZARD_TASK_UNKNOWN);	
	void processProblemSolveWizardActions(CPropertyWizard& ref,INT_PTR retVal);
	void DestroyApplication();
	BOOL StoreMemoryDumpInfo(void);	
	BOOL TaskSetDateToMasterLcd();
	void ShutDownApplication();

	void showDaliSubbusConfiguration(BYTE bNodeAddress);
#if(0)
	void IPCUpdateMgrSendUpdateString();
#endif 

	// Sedert V16.10: Databases + alle inlog gegevens wissen.
	void initNewApplicationData( );		

	// Sedert V16.10: De node services updaten met de gegevens van de server login.
	// Wanneer we connecteren ontvangen we het logisch adres van de node. Deze wordt gebruikt als 
	// key in de node services.
	void UpdateServerLoginCfg( );	

	///////////////////////////////////////
	// Sedert V16.28
	// Device nodes updaten met de laatste netwerk instellingen.
	// Deze lijst wordt in een Json bestand opgeslagen en kan gebruikt worden voor de configurator software.
	//
	// De functie wordt niet direct aangeroepen vanuit de FSM (wanneer we IDLE zijn) maar gebeurd via een vlag.
	// zie setRequestUpdateServerDeviceNodes( )
	void UpdateServerDeviceNodes();
	void UpdateServerDeviceNodes(const AppUDPDevice& r);

	void setRequestUpdateServerDeviceNodes( ) {
		m_flagUpdateNodeServices = TRUE;
	}
	/////////////////////////////////////////

	// Sedert V16.36:
	// Wanneer we in idle toestand komen conroleren van de node database.
	// Afhankelijk van de node database controleren of er extra/nieuwe features moeten enabled worden.
	// Toepassing:
	// - Wanneer er smartbox ultra modules gedecteerd worden dan moet de menu optie voor de DALI subbus
	//   configuratie enabled worden.
	void updateAppStateViewModes( );

public:
	BOOL LoadDocumentFile(CStringArray* pStrArray);
	BOOL VerifyDocumentBindingDatabases(const CStringArray* pStrArray);		
	BOOL loadCfgLoginInfo( );			// Sedert V16.09: inladen van de inlog gegevens - wordt aangeroepen door CTimerDoc.
	BOOL saveCfgLoginInfo( );			// Sedert V16.09: opslaan van de inlog gegevens - wordt aangeroepen door CTimerDoc.
	BOOL saveCfgLoginInfoExport( );		// Sedert V16.19: export van de inlog gegevens voor de cfg'er - Runtime bij een connect / disconnect / ...
	BOOL loadCfgNodeServices( );		// Sedert V16.10: inladen van de nodeservices gegevens - wordt aangeroepen door CTimerDoc.
	BOOL saveCfgNodeServices( );		// Sedert V16.10: opslaan van de nodeservices gegevens - wordt aangeroepen door CTimerDoc.
	BOOL saveCfgNodeServicesExport( );	// Sedert V16.27: export van de nodeservice gegevens.
	DUOTECNO::AppRemoteCfgLoginInfo& getAppRemoteCfgLoginInfo();
	DUOTECNO::AppRemoteCfgNodes& getAppRemoteCfgNodeServices();
	AppUDPServerCfg& getAppUDPServerCfg();

protected:  // control bar embedded members	
	CStatusBar  m_wndStatusBar;
	//CToolBar    m_wndToolBar;	
	CToolBar	m_wndViewToolBar;		
	CToolBar	m_wndControlsToolBar;

private:			
	// Sedert V16.24: Wordt gebruikt bij het opstarten van de applicatie.
	// Principe van het splash screen herwerkt.
	// Tijdens het tonen van het splash screen kunnen er inialisaties uitgevoerd worden +
	// wordt er gezocht naar software updates.
	typedef enum {
		START_APP_INIT = 0,
		START_APP_CREATE,		
		START_APP_UPDATE_CHECK,
		START_APP_UPDATE_BUSY,
		START_APP_GUI,
		START_APP_DONE	
	} StateStartApp_t;

	StateStartApp_t m_stateStartApp;

	// Sedert 0x0E10:
	// Waneer bindings aan het doorsturen dan zijn de knoppen ADD,DELETE & AUTO disabled.
	BOOL m_fWriteBindingsBusy;	

	typedef enum {
		STATUS_OFFLINE=0,
		STATUS_ERROR,
		STATUS_DATA_RECIEVED,
		STATUS_DATA_SEND,
		STATUS_RX_ACK,
	} GuiCommunicationStatus_t;

	GuiCommunicationStatus_t m_CommStatusIndicator;
	DUOTECNO::AppRemoteCfgLoginInfo* m_pAppRemoteCfgLoginInfo;	// Deze bevat de login gegevens die opgeslagen worden in projectinfo.dat.
	DUOTECNO::AppRemoteCfgNodes* m_pAppRemoteCfgNodes;			// Deze bevat de login gegevens die opgeslagen worden in projectnodeservices.dat
	AppUDPServerCfg* m_pAppUDPServerCfg;
	BOOL m_flagUpdateNodeServices;								// Sedert V16.28: Vlag wordt gezet wanneer de node service device list mag upgedate worden.
	CToolTipCtrl m_tooltipViewToolBar;							// Sedert V16.43: Weergave van tooltip info.
	CPropertyWizard* m_problemSolveWizard;						// Sedert V16.44:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__31536D49_354B_11D8_B865_0050BAC412B1__INCLUDED_)
