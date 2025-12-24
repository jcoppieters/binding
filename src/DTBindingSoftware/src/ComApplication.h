#if !defined(AFX_COMAPPLICATION_H__1F3612A1_57EE_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_COMAPPLICATION_H__1F3612A1_57EE_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ComApplication.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CComApplication window

// #include "mscomm.h"
#include "IComApplication.h"
#include "communication/communication.h"
///////////////////////////////////////////////////////////////////////////////
#include "MyWindowTimers.h"				//TEMP ...
#include "MyWindowMessages.h"
///////////////////////////////////////////////////////////////////////////////
class CMainFrame;

class CCommunicationObject;
class CComApplicationFactory;
class CComApplication;
class CCommunicationStatus;
class CCanNode;
///////////////////////////////////////////////////////////////////////////////
#define MESSAGE_RX_ACK					1
///////////////////////////////////////////////////////////////////////////////
class CMessageHandler
{
typedef void (CComApplication::*Action) (void);

public:
	CMessageHandler(BYTE bMessageCode,Action action) {
		this->action = action;
		this->bMessageCode = bMessageCode;
	}
public:	
	Action action;
	BYTE bMessageCode;
};

///////////////////////////////////////////////////////////////////////////////

class CComApplication : public CWnd , public IComApplication
{
protected:
	static const CMessageHandler MessageHandlers[256];

	static CComApplication*				m_pThis;
	static CComApplicationFactory*		m_pFactory;

	typedef enum
	{
		CLASSFACTORY_ID_RENAME_ENGINE				= 0,
		CLASSFACTORY_ID_MULTIMEDIA_CONFIG			= 1,
		CLASSFACTORY_ID_REQUEST_FOR_STATUSSEN		= 2,
		CLASSFACTORY_ID_BINDINGS_READWRITE_ENGINE	= 3,
		CLASSFACTORY_ID_SCREENMAP_UPDATE_ENGINE		= 4,
		CLASSFACTORY_ID_MEMORYDUMP_ENGINE			= 5,
		CLASSFACTORY_ID_BROWSEPNP_ENGINE			= 6,
		CLASSFACTORY_ID_MODEMCONNECT_ENGINE			= 7,
		CLASSFACTORY_ID_NICERRORSFSM_ENGINE			= 8,
		CLASSFACTORY_ID_UNITCREATE_CONFIG			= 9,		
		CLASSFACTORY_ID_UPLOAD_MOODS				= 10,		
		CLASSFACTORY_ID_UPLOAD_IRRX					= 11,		
		CLASSFACTORY_ID_UPLOAD_SCHEDULE				= 12,		
		CLASSFACTORY_ID_FTP							= 13,		
		CLASSFACTORY_ID_UPLOAD_SMAP_PAGEINFO		= 14,		
		CLASSFACTORY_ID_UPLOAD_SMAP_MENU			= 15,		
		CLASSFACTORY_ID_UPLOAD_SMAP_AVROOM			= 16,		
		CLASSFACTORY_ID_UPLOAD_GATEWAY_CFG			= 17,
		CLASSFACTORY_ID_UPLOAD_ROOMCTRL_AUDIOCFG	= 18,
		CLASSFACTORY_ID_UPLOAD_BOSEST_CFG			= 19,
		CLASSFACTORY_ID_UPLOAD_LEDSCFG				= 20,
		CLASSFACTORY_ID_UPLOAD_SMAPLITE				= 21,	// since 0x0C05

		CLASSFACTORY_ID_UNDEFINED					= 0xFF

	}ENUM_CLASSFACTORY_ID;

	typedef enum
	{
		WM_COMM_MESSAGE_RECEIVED				  = WM_USER+1,
		WM_COMM_MESSAGE_SEND					  = WM_USER+2,
		WM_CLASSFACTORY_DESTROY					  = WM_USER+3
	
	}ENUM_WINDOWS_MESSAGES; 

	CCommunicationObject*  m_pCommunicationObject;
	CMainFrame*	const m_pControl;

public:
	static void Initialise(CWnd *parent);
	static void Destroy();
	
	static CComApplication* GetInstance() {
		ASSERT( m_pThis );
		return m_pThis;	
	}
	void StartTimer(UINT nIDEvent,UINT nElapse) {
		SetTimer(nIDEvent,nElapse,(TIMERPROC) NULL);				
	}
	void StopTimer(UINT nIDEvent) {
		KillTimer(nIDEvent);
	}

public:
	void OnCommEvent();
	HWND Create();

#ifdef _DEBUG	
	CCommunicationObject* GetComportObj() {
		return m_pCommunicationObject;
	}
#endif

	CCommunicationStatus& GetCommunicationStatus() {
		return m_pCommunicationObject->getCommunicationStatus();
	}
	const CCommunicationStatus& GetCommunicationStatus() const {
		return m_pCommunicationObject->getCommunicationStatus();
	}

	// HomeClient socket.
	// Sedert V16.35 homeclient socket opvragen dmv de node.
	// Mbv de node wordt bepaald welke opties er ondersteund worden...
	ITcpSocketHomeClient* getSocketHomeClient(CCanNode& refNode);
	bool isSocketHomeClientMasterSupported(CCanNode* pNode);

	// close connections...
	void closeConnections( );

	virtual void Trigger(UINT msg) {
		PostMessage(msg);
	}

	virtual void NotifyOnTaskDone(ENUM_COMAPPLICATION_TASK id);
	virtual void NotifyOnTaskError(ENUM_COMAPPLICATION_TASK id);
	virtual void NotifyOnTaskCancelled(ENUM_COMAPPLICATION_TASK id);

	virtual BYTE MessageTransfer(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bMsgCode);
	virtual BYTE MessageTransfer(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bMsgCode,
								BYTE bLength,CString szData);

	void ResetCommunication();
	void ShutDownAllTasks();

	/** Interface to CBrowsePnpEngine */
	void NotifyBrowseOk(void);
	void NotifyPnpOk(void);
	void NotifyBrowsePnpError(void);
	void NotifyBrowsePnpCancel(void);
	void NotifyBrowsePnpTimeout(void);
	void NotifyPnpDuplicatesFound(void);
	void NotifyPnpFactoryAddressFound(void);

	/** Notifications from CSerialCommObject */	
	void NotifyReceivedMessage(void);
	void NotifyMessageSend(void);
	void NotifyReceivedAck(void);			
	void NotifyReceivedNack(void);		
	//void NotifyReceivedModemCommand(void);
	void NotifyReceivedModemStatus(void);
	void NotifyReceivedModemAddress(void);
	void NotifyReceivedModemVersion(void);
	void NotifyModemCommunicationError(void);
	void NotifyModemCanLoggerStateChanged(void);

	BOOL Start_RequestForStatussen(int nodeAddress = -1);	// Indien een node address meegegeven wordt dan single node.
	BOOL Cancel_RequestForStatussen(void);
	void Start_SendAllNames(void);
	void Cancel_SendAllNames(void);
	BOOL Start_UploadBitmaps(void);
	BOOL Start_MemoryDump(void);	
	BOOL Cancel_MemoryDump(void);
	void Cancel_UploadLcdScreenmaps();
	void Start_UploadLcdScreenmaps();
	BOOL Start_UploadMultiMediaConfig();
	BOOL Cancel_UploadMultiMediaConfig();
	BOOL Start_WriteAllBindings(void);
	BOOL Start_ReadAllBindings(void);
	BOOL Start_WriteModifiedBindings(void);
	BOOL Start_NicErrorsFsm();
	BOOL Stop_NicErrorsFsm();
	BOOL Start_UploadUnitCreateConfig( );			// since 0x0560
	BOOL Cancel_UploadUnitCreateConfig( );			// since 0x0560
	BOOL Start_UploadMoodConfig( );					// since 0x0560
	BOOL Cancel_UploadMoodConfig( );				// since 0x0560
	BOOL Start_UploadIRRXConfig( );					// since 0x0560
	BOOL Cancel_UploadIRRXConfig( );				// since 0x0560
	BOOL Start_UploadScheduleConfig( );				// since 0x0560
	BOOL Cancel_UploadScheduleConfig( );			// since 0x0560
	BOOL Start_ftpConfig();							// since 0x0562
	BOOL Cancel_ftpConfig();						// since 0x0562
	BOOL Start_uploadSmapPageInfo( );				// since 0x0571
	BOOL Cancel_uploadSmapPageInfo( );				// since 0x0571
	BOOL Start_UploadSmapMenu( );					// since 0x0571
	BOOL Cancel_UploadSmapMenu( );					// since 0x0571
	BOOL Start_UploadSmapAVRoom( );					// since 0x0610
	BOOL Cancel_UploadSmapAVRoom( );				// since 0x0610
	BOOL Start_UploadGatewayCfg( );					// since 0x0620
	BOOL Cancel_UploadGatewayCfg( );				// since 0x0620
	BOOL Start_UploadRoomCtrlAudioCfg( );			// since 0x0625
	BOOL Cancel_UploadRoomCtrlAudioCfg( );			// since 0x0625
	BOOL Start_UploadBoseSTConfig( );				// since 0x0802
	BOOL Cancel_UploadBoseSTConfig( );				// since 0x0802
	BOOL Start_UploadLedsConfig( );					// since 0x0804
	BOOL Cancel_UploadLedsConfig( );				// since 0x0804
	BOOL Start_UploadSmapLiteCfg( );				// since 0x0C05
	BOOL Cancel_UploadSmapLiteCfg( );				// since 0x0C05

	// INTERFACES
	void RenameUnit(BYTE bNodeAddress,BYTE bUnitAddress);
	void RenameNode(BYTE bNodeAddress);
		
	// INTERFACES : BROWSE + PNP
	void BrowseSingleNode(BYTE bNodeAddress);				
	void BrowseAllPhysicalNodes(void);
	void BrowseAllNodes(void);
	void StartPnP(void);								
	void CancelBrowsePnp(void);

	
	// Events received from CNodeLcdScreenmapFSM ...
	void NotifyEventScreenmapFSMInfo(CString s);

	BYTE RequestModemStatus();
	BYTE RequestModemAddress();
	BYTE RequestModemVersion();		/** @since 0x0562 */
	BOOL SendModemCanLoggerOnOff(BOOL fState);

	// Sedert V16.26: Integratie DT18-PRO (master/slave).
	BOOL sendServerTCPIPGetNodeInfo();
	BOOL sendServerTCPIPSetMaster();						

	// Sedert V16.26: zie USE_WORKAROUND_MESSAGEERROR_ABSORBE
	// Wanneer er een EV_MESSAGEERROR van deze node ontvangen wordt met MESSAGE_ERROR_CAN_TX_PROTOCOL_TIMEOUT
	// dan deze niet weergeven in de scrollview onderaan.
	void setAbsorbeMessageError(int nodeAddress);

	BOOL IsConnected(void);
	void setConnect();
	void setSerialUsbConnect(int port = -1);	
	void setTcpConnect(ConnectionMethod_t method, const CString& hostName,int port, const CString& pwd, const CString& cloudServerLoginToken);
	void setDisconnect();
	
	// Events from BindingsReadWriteFSM ...
	void NotifyEraseBindingsNodeStart(BYTE bNodeAddress);
	void NotifyEraseBindingsNodeDone(BYTE bNodeAddress);
	void NotifyWriteBindingsNodeStart(BYTE bNodeAddress);
	void NotifyWriteBindingsNodeDone(BYTE bNodeAddress);
	void NotifyReadBindingsNodeStart(BYTE bNodeAddress);
	void NotifyReadBindingsNodeDone(BYTE bNodeAddress);

	void NotifyReadBindingsNodeError(BYTE bNodeAddress);
	void NotifyWriteBindingsNodeError(BYTE bNodeAddress);
	void NotifyEraseBindingsNodeError(BYTE bNodeAddress);
	void NotifyWriteBindingsFSMDone(void);
	void NotifyReadBindingsFSMDone(void);

	// Sedert V16.07
	virtual void NotifyConnectStatusChanged(BOOL statusConnect);

	// Nic Errors ...
	virtual void NotifyNicErrorsNodeFsmDone(BYTE bNodeAddress);
	virtual void NotifyNicErrorsNodeFsmError(BYTE bNodeAddress);
	virtual void NotifyNicErrorsNodeFsmRetry(BYTE bNodeAddress);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComApplication)
		protected:
			virtual void PostNcDestroy();
	//}}AFX_VIRTUAL
	

protected:
	CComApplication(CWnd *pCtrl);
	virtual ~CComApplication();

	LONG OnComDataReceived(WPARAM wParam,LPARAM lParam);

	void SetConnected(BOOL fConnected);
	void ParseNodeObjectRxMessage(void);

	LONG OnMessageReceived(WPARAM wParam,LPARAM lParam);
	LONG OnMessageSend(WPARAM wParam,LPARAM lParam);
	LONG OnScreenmapFSMTriggerCancel(WPARAM wParam,LPARAM lParam);
	LONG OnScreenmapFSMTriggerOk(WPARAM wParam,LPARAM lParam);
	LONG OnScreenmapFSMTriggerError(WPARAM wParam,LPARAM lParam);
	LONG OnClassFactoryDestroy(WPARAM , LPARAM);
	LONG OnCommunicationModemStatus(WPARAM, LPARAM);
	void Fire_OnNodeStatusChanged(BYTE bNodeAddress);
	void Fire_OnUnitStatusChanged(BYTE bNodeAddress,BYTE bUnitAddress);

	//{{AFX_MSG(CComApplication)
		// NOTE - the ClassWizard will add and remove member functions here.
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

#if defined(USE_MSCOMMCONTROL)
	DECLARE_EVENTSINK_MAP()
#endif

	DECLARE_MESSAGE_MAP()

	/** MessageHandlers ...*/
	void MessageHandler_EV_UNITCONTROLSTATUS();
	void MessageHandler_EV_NODETEXTSTRING();
	void MessageHandler_EV_UNITAUDIOSTATUS();
	void MessageHandler_EV_UNITSENSSTATUS();
	void MessageHandler_EV_UNITDIMSTATUS();
	void MessageHandler_EV_UNITSWITCHSTATUS();
	void MessageHandler_EV_UNITDUOSWITCHSTATUS();
	void MessageHandler_EV_UNITALARMSTATUS();		// TM_CHANGES_ALARM_ARITECH
	void MessageHandler_EV_UNITAVMATRIXSTATUS();	// TM_AVMATRIX_INPUT
	void MessageHandler_UnitStatus();
	void MessageHandler_FC_LCDINFODIALOG();
	void MessageHandler_EV_EXTERNALCONFIG();
	void MessageHandler_EV_MESSAGEERROR();
	void MessageHandler_EV_NODEBINDINGENTRY();
	void MessageHandler_EV_NODEBINDINGSFILECONTROL();
	void MessageHandler_EV_INIT();
	void MessageHandler_EV_LCDBUILDSCREENMAP();
	void MessageHandler_EV_LCDINFODIALOG();
	void MessageHandler_EV_MEMORYDATADUMP();
	void MessageHandler_EV_WRITEMEMORYDATADONE();
	void MessageHandler_EV_UNITCONFIG();
	void MessageHandler_EV_UNITNAME();
	void MessageHandler_EV_NODENAME();
	void MessageHandler_EV_NODECONFIG();
	void MessageHandler_EV_NODERESOURCES();
	void MessageHandler_EV_NODEDLLSTATUS();
	void MessageHandler_EV_NODEPHYSICALADDRESS();
	void MessageHandler_EV_NODEGATEWAYSTATUS();
	void MessageHandler_EV_PNPLOGICALADDRESSASSIGNED();
	void MessageHandler_EV_FTPFILECONTROLSTATUS( );		
	void MessageHandler_EV_FTPFILECONTROLDATA( );		
	void MessageHandler_EV_DATABASEMANAGMENT( );		
	void MessageHandler_EV_UNITENERGYMEASUREMENTSTATUS( );	// IMPLEMENT_UNIT_ENERGY

private:
	// Sedert V16.26: zie USE_WORKAROUND_MESSAGEERROR_ABSORBE
	typedef struct {
		bool m_flagAbsorbe;
		int m_nodeAddress;
	} STRUCT_ABSORBE_MSG;

	STRUCT_ABSORBE_MSG m_absorbeDisplayMsg;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMAPPLICATION_H__1F3612A1_57EE_11D8_B865_0050BAC412B1__INCLUDED_)
