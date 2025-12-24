// QHSMApplicationGlobal.h: interface for the CQHSMApplicationGlobal class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QHSMAPPLICATIONGLOBAL_H__1751EB61_745A_4DAE_A2A1_11E9883D5E41__INCLUDED_)
#define AFX_QHSMAPPLICATIONGLOBAL_H__1751EB61_745A_4DAE_A2A1_11E9883D5E41__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////
#include "qf\include\qf_win32.h"
/////////////////////////////////////////////////////////////////////////
class CMainFrame;
class CQHSMGenericTask;
class CQHSMCompositeTask;
class CQHSMTaskBuilder;
class CAppFsmNodeTaskEntries;
/////////////////////////////////////////////////////////////////////////
#define WM_QHSM_EVENT						(WM_USER + 2000)
#define WM_QHSM_INITIALISE					(WM_USER + 2001)
/////////////////////////////////////////////////////////////////////////
typedef enum 
{
	// Assysnchronous Events ...
	EV_SIGNAL_TASK_DONE = Q_USER_SIG,
	EV_SIGNAL_TASK_ERROR,
	EV_SIGNAL_TASK_CANCEL,
	EV_SIGNAL_COMMUNICATION_OFFLINE,
	EV_SIGNAL_COMMUNICATION_ONLINE,
	EV_SIGNAL_COMMUNICATION_NOTIFY_ACK,
	// Operations ...
	EV_OPERATION_NEWAPPLICATION,
	EV_OPERATION_SHUTDOWN,
	EV_OPERATION_BROWSE_FOR_PHNODES,
	EV_OPERATION_REQUEST_FOR_STATES,
	EV_OPERATION_AUTOMATIC_PNP,
	EV_OPERATION_UPLOAD_BITMAPS,
	EV_OPERATION_UPLOAD_ALLBINDINGS,
	EV_OPERATION_UPLOAD_PROGFILE,
	//EV_OPERATION_DOWNLOAD_PROGRAMMATION,
	EV_OPERATION_UPLOAD_MODIFIEDBINDINGS,
	EV_OPERATION_DOWNLOAD_BINDINGS,
	EV_OPERATION_UPLOAD_SCREENMAPS,
	EV_OPERATION_UPLOAD_MMCONFIG,
	EV_OPERATION_UPLOAD_MEMORY,
	EV_OPERATION_UPLOAD_NAMES,
	EV_OPERATION_DOWNLOAD_MEMORY,
	EV_OPERATION_COMMUNICATION_ONLINE,
	EV_OPERATION_COMMUNICATION_OFFLINE,	
	EV_OPERATION_TASKDIALOG_USERBUTTON,			// TaskDialog --> User presses button ...
	EV_OPERATION_CONVERT_TO_BINDINGDATABASE,
	EV_OPERATION_DOCUMENT,
	EV_OPERATION_MESSAGEBOX_RESPONSE,
	EV_OPERATION_NICNETWORK_START,
	EV_OPERATION_UPLOAD_UNITCONFIG,
	EV_OPERATION_UPLOAD_MOODS,
	EV_OPERATION_UPLOAD_IRRX,
	EV_OPERATION_UPLOAD_SCHEDULE,
	EV_OPERATION_FTP,							// Since v0562
	EV_OPERATION_UPLOAD_SMAP_PAGEINFO,			// Since v0571:
	EV_OPERATION_UPLOAD_SMAP_MYHOME,			// Since v0571: composite task : gewoon gebruik.
	EV_OPERATION_UPLOAD_SMAP_MENU,				// Since v0571: 
	EV_OPERATION_UPLOAD_SMAP_AVROOM,			// Since v0610
	EV_OPERATION_UPLOAD_GATEWAYCFG,				// Since v0620
	EV_OPERATION_UPLOAD_ROOMCTRL_AUDIOCFG,		// since v0625
	EV_OPERATION_UPLOAD_BOSESTCFG,				// sinve v0802
	EV_OPERATION_UPLOAD_LEDSCFG,				// since v0804
	EV_OPERATION_UPLOAD_SMAPLITE,				// since v0c05
	EV_OPERATION_CHECK_POSTBROWSE_ERRORS,		// Sedert V16.16
	EV_OPERATION_BROWSE_SINGLE_NODE				

}EV_QHSM_APPLICATIONGLOBAL;
/////////////////////////////////////////////////////////////////////////
typedef enum
{
	EV_OPERATION_LOAD = 0,
	EV_OPERATION_SAVE,
	EV_OPERATION_NEW

} EV_OPERATION_DOCUMENT_SPECIFIC;

typedef enum
{
	EV_OPERATION_SHOW = 0,
	EV_SIGNAL_DESTROY,
	EV_OPERATION_START,
	EV_OPERATION_CANCEL,
	EV_OPERATION_CLEAR

}EV_CONVERT_TO_BINDINGDATABASE;

typedef enum
{
	EV_OPERATION_USER_NO = 0,
	EV_OPERATION_USER_YES = 1

}ENUM_OPERATION_MESSAGEBOX;
/////////////////////////////////////////////////////////////////////////

class CQHSMApplicationGlobal : public QHsm  
{
	friend class CQHSMTaskBuilder;

public:
	static CQHSMApplicationGlobal* GetInstance();

	CQHSMApplicationGlobal() : QHsm((QPseudoState)&CQHSMApplicationGlobal::initial)	// CHANGES_MVS_2008
	{	
		m_pGenericTask = NULL;
	}  	
	virtual ~CQHSMApplicationGlobal();

public:	// State Handlers ...
	void initial(QEvent const *e);
	QSTATE MainApplication(QEvent const *e);
		QSTATE NewApplication(QEvent const *e);		
		QSTATE Installation(QEvent const *e);
			QSTATE Installation_Online(QEvent const *e);
				QSTATE Installation_Online_Idle(QEvent const *e);
				QSTATE Installation_Online_Busy(QEvent const *e);				
					QSTATE Installation_Online_Busy_TaskBrowseSingleNode(QEvent const *e);									// V16.43
						QSTATE Installation_Online_Busy_TaskBrowseSingleNode_Browse(QEvent const *e);									// V16.43
						QSTATE Installation_Online_Busy_TaskBrowseSingleNode_RequestForStates(QEvent const *e);					// V16.43
						QSTATE Installation_Online_Busy_TaskBrowseSingleNode_CompareNodes(QEvent const *e);						// V16.43
						QSTATE Installation_Online_Busy_TaskBrowseSingleNode_HandlePostBrowseErrors(QEvent const *e);			// V16.43
					QSTATE Installation_Online_Busy_GenericTask(QEvent const *e);
					QSTATE Installation_Online_Busy_CompositeTask(QEvent const *e);
					QSTATE Installation_Online_Busy_New(QEvent const *e);
						QSTATE Installation_Online_Busy_New_BrowseForNodes(QEvent const *e);
						QSTATE Installation_Online_Busy_New_RequestForStates(QEvent const *e);
						QSTATE Installation_Online_Busy_New_HandlePreBrowseErrors(QEvent const *e);
						QSTATE Installation_Online_Busy_New_AutomaticPnp(QEvent const *e);					
					QSTATE Installation_Online_Busy_Continue(QEvent const *e);
						QSTATE Installation_Online_Busy_Continue_BrowseForNodes(QEvent const *e);
						QSTATE Installation_Online_Busy_Continue_RequestForStates(QEvent const *e);
						QSTATE Installation_Online_Busy_Continue_HandlePreBrowseErrors(QEvent const *e);
						QSTATE Installation_Online_Busy_Continue_HandlePostBrowseErrors(QEvent const *e);
						QSTATE Installation_Online_Busy_Continue_AutomaticPnp(QEvent const *e);
						QSTATE Installation_Online_Busy_Continue_CompareDbases(QEvent const *e);
						QSTATE Installation_Online_Busy_Continue_NodeFsmTasks(QEvent const *e);	// V16.47: Toestand om units door te sturen voor het browsen.
					QSTATE Installation_Online_Busy_HandleLicActCodeNeeded(QEvent const *e);
				QSTATE Installation_Online_Error(QEvent const *e);
				QSTATE Installation_Online_Cancelled(QEvent const *e);
			QSTATE Installation_Offline(QEvent const *e);
				QSTATE Installation_Offline_Idle(QEvent const *e);
				QSTATE Installation_Offline_HandleLicActCodeNeeded(QEvent const *e);
		QSTATE Engineering(QEvent const *e);			
			QSTATE Engineering_Normal(QEvent const *e);
				QSTATE Engineering_Online(QEvent const *e);
					QSTATE Engineering_Online_Idle(QEvent const *e);
						QSTATE Engineering_Online_Idle_Ready(QEvent const *e);
							//QSTATE Engineering_Online_Idle_Ready_ConvertToDatabase(QEvent const *e);
					QSTATE Engineering_Online_Busy(QEvent const *e);
						QSTATE Engineering_Online_Busy_GenericTask(QEvent const *e);
						QSTATE Engineering_Online_Busy_CompositeTask(QEvent const *e);
				QSTATE Engineering_Offline(QEvent const *e);
					QSTATE Engineering_Offline_Ready(QEvent const *e);
			QSTATE Engineering_Tools(QEvent const *e);
				QSTATE Engineering_Tools_ConvertToDatabase(QEvent const *e);
		QSTATE Simulation(QEvent const *e);

private:
	CMainFrame *m_pParent;

	CQHSMGenericTask *m_pGenericTask;
	CQHSMCompositeTask *m_pCompositeTask;

	QState Installation_OnlineBusy_HistoryState;
	QState Installation_Online_SafeRestore;
	QState Engineering_Online_Idle_History;
	//QState Engineering_Online_Busy_History;
	QState Engineering_Normal_History;

	BOOL m_fInitialiseDatabases;	
	BOOL m_fUserDisconnect;				// Sedert V16.07: Bijhouden of disconnect actie door de gebruiker gevraagd werd.
	volatile BOOL m_Locked;
		
protected:
	void Trigger(DWORD dwMessage, DWORD lParam = 0) {
		AfxGetMainWnd()->PostMessage(WM_QHSM_EVENT, dwMessage, lParam);									
	}
	inline void SetLock(BOOL flag) {
		m_Locked = flag;
	}
	void helperGoToNextTask(CAppFsmNodeTaskEntries& refNodeTaskEntries);

protected:
	BOOL Start_BrowseSingleNode();
	BOOL Cancel_BrowseSingleNode();
	BOOL Start_BrowseAllPhysicalNodes();
	BOOL Cancel_BrowseAllPhysicalNodes();
	BOOL Start_RequestForStates();
	BOOL Start_RequestForStatesSingleNode();
	BOOL Cancel_RequestForStates();
	BOOL Start_AutomaticPnp();
	BOOL Cancel_AutomaticPnp();
	BOOL Start_HandlePreBrowseError();
	BOOL Start_HandlePostBrowseError();
	BOOL Start_HandleLicAccCodeNeeded();
	BOOL Start_WriteAllBindings();
	BOOL Start_WriteModifiedBindings();
	BOOL Start_DownloadBindings();
	BOOL Start_UploadLcdScreenmaps();
	BOOL Cancel_UploadLcdScreenmaps();
	BOOL Start_UploadBitmaps();
	BOOL Cancel_UploadBitmaps();
	BOOL Start_UploadMultimMediaConfig();
	BOOL Cancel_UploadMultimMediaConfig();
	BOOL Start_UploadMemory();
	BOOL Cancel_UploadMemory();
	BOOL Start_DownloadMemory();
	BOOL Cancel_DownloadMemory();
	BOOL Start_UploadAllNames();
	BOOL Cancel_UploadAllNames();
	BOOL Start_NicErrorsFsm();
	BOOL Cancel_NicErrorsFsm();
	BOOL Start_UploadUnitsConfig( );
	BOOL Cancel_UploadUnitsConfig( );
	BOOL Start_UploadMoodConfig( );
	BOOL Cancel_UploadMoodConfig( );
	BOOL Start_UploadIRRXConfig( );
	BOOL Cancel_UploadIRRXConfig( );
	BOOL Start_UploadScheduleConfig( );
	BOOL Cancel_UploadScheduleConfig( );
	BOOL Start_ftpConfig( );			// since v0562
	BOOL Cancel_ftpConfig( );			// since v0562
	BOOL Start_UploadSmapPageInfo( );	// since v0571
	BOOL Cancel_UploadSmapPageInfo( );	// since v0571
	BOOL Start_UploadSmapMenu( );		// since v0571
	BOOL Cancel_UploadSmapMenu( );		// since v0571
	BOOL Start_UploadSmapAVRoom( );		// since v0610
	BOOL Cancel_UploadSmapAVRoom( );	// since v0610
	BOOL Start_UploadGatewayCfg( );		// since v0620
	BOOL Cancel_UploadGatewayCfg( );	// since v0620
	BOOL Start_UploadRoomCtrlAudioCfg();// since v0625
	BOOL Cancel_UploadRoomCtrlAudioCfg();// since v0625
	BOOL Start_UploadBoseSTCfg();		// since v0802
	BOOL Cancel_UploadBoseSTCfg();		// since v0802
	BOOL Start_UploadLedsCfg();			// since v0804
	BOOL Cancel_UploadLedsCfg();		// since v0804
	BOOL Start_UploadSmapLiteCfg();		// since v0C05
	BOOL Cancel_UploadSmapLiteCfg();	// since v0C05
	
	
	void Notify_OnTaskUploadUnitsDone();		// 16.43
	void Notify_OnTaskConfigDone();
	void Notify_OnTaskConfigError();
	void Notify_OnTaskConfigCancelled();
	void Notify_OnTaskMemoryDumpDone();
	void Notify_OnTaskMemoryDumpError();
	void Notify_OnTaskMemoryDumpCancelled();
	void Notify_OnTaskUploadBitmapsDone();
	void Notify_OnTaskUploadBitmapsError();
	void Notify_OnTaskUploadBitmapsCancelled();
	void Notify_OnTaskUploadScreenmapsDone();
	void Notify_OnTaskUploadScreenmapsError();
	void Notify_OnTaskUploadScreenmapsCancelled();
	void Notify_OnTaskRequestForStatesDone();
	void Notify_OnTaskRequestForStatesCancelled();
	void Notify_OnTaskWriteBindingsDone();
	void Notify_OnTaskNicErrorsFsmDone();
	void Notify_OnTaskNicErrorsFsmError();
	void Notify_OnTaskNicErrorsFsmCancelled();
	void Notify_OnTaskSmapLiteCfgDone();
	void Notify_OnTaskUploadRoomCtrlAudioCfg();	// Since V0x0C05
	void Notify_OnTaskBrowseSingleNodeFailed();

	BOOL Go_Online();
	BOOL Go_Offline();
	BOOL CloseConnections();
	BOOL ShowDialog_TryAgain();
	BOOL ShowDialog_AreYouSure(CString s);
	BOOL ShowDialog_Info(CString s);
	BOOL StartErrorLog();
	BOOL ShowErrorLog();
	BOOL Is_CommunicationOnline();

	BOOL GenericTask_Invoke_Start(CQHSMGenericTask* m_pTask);
	BOOL GenericTask_Invoke_Cancel(CQHSMGenericTask* m_pTask);
	BOOL GenericTask_Invoke_ErrorNotify(CQHSMGenericTask* m_pTask);
	BOOL GenericTask_Invoke_DoneNotify(CQHSMGenericTask* m_pTask);
	BOOL GenericTask_Invoke_CancelNotify(CQHSMGenericTask* m_pTask);


public:		
	// Disable TX Events ...
	BOOL Locked() {
		return m_Locked;
	}

	// Events ...
	void EvSignalTaskDone() {
		Trigger( EV_SIGNAL_TASK_DONE );
	}
	void EvSignalTaskError() {
		Trigger( EV_SIGNAL_TASK_ERROR );
	}
	void EvSignalTaskCancel() {
		Trigger( EV_SIGNAL_TASK_CANCEL );
	}
	void EvSignalCommunicationOffline() {
		Trigger( EV_SIGNAL_COMMUNICATION_OFFLINE );
	}
	void EvSignalCommunicationOnline() {
		Trigger( EV_SIGNAL_COMMUNICATION_ONLINE );
	}
	void EvSignalCommunicationNotifyAck() {
		Trigger( EV_SIGNAL_COMMUNICATION_NOTIFY_ACK );
	}
	
	// Operations ...
	void EvOperationBrowseSingleNode( ) {
		Trigger( EV_OPERATION_BROWSE_SINGLE_NODE );
	}
	void EvOperationUploadSmapLite( ) {
		Trigger( EV_OPERATION_UPLOAD_SMAPLITE );		// since 0x0C05
	}
	void EvOperationUploadLedsCfg( ) {
		Trigger( EV_OPERATION_UPLOAD_LEDSCFG );
	}
	void EvOperationUploadBoseSTCfg( ) {
		Trigger( EV_OPERATION_UPLOAD_BOSESTCFG );
	}
	void EvOperationUploadRoomCtrlAudioCfg( ) {
		Trigger( EV_OPERATION_UPLOAD_ROOMCTRL_AUDIOCFG );
	}
	void EvOperationUploadGatewayCfg( ) {
		Trigger( EV_OPERATION_UPLOAD_GATEWAYCFG );	
	}
	void EvOperationUploadSmapAVRoom( ) {
		Trigger( EV_OPERATION_UPLOAD_SMAP_AVROOM );	
	}
	void EvOperationUploadSmapMenu( ) {
		Trigger( EV_OPERATION_UPLOAD_SMAP_MENU );	
	}
	void EvOperationUploadSmapMyHome( )  {
		Trigger( EV_OPERATION_UPLOAD_SMAP_MYHOME );
	}
	void EvOperationUploadSmapPageInfo( )  {
		Trigger( EV_OPERATION_UPLOAD_SMAP_PAGEINFO );
	}
	void EvOperationFtpConfig( )  {
		Trigger( EV_OPERATION_FTP );
	}
	void EvOperationNewApplication() {
		Trigger( EV_OPERATION_NEWAPPLICATION );
	}
	void EvOperationUploadAllBitmaps() {
		Trigger( EV_OPERATION_UPLOAD_BITMAPS );
	}
	void EvOperationUploadAllBindings() {
		Trigger( EV_OPERATION_UPLOAD_ALLBINDINGS );
	}
	void EvOperationUploadProgFile() {
		Trigger( EV_OPERATION_UPLOAD_PROGFILE );
	}
	void EvOperationUploadModifiedBindings() {
		Trigger( EV_OPERATION_UPLOAD_MODIFIEDBINDINGS );
	}
	void EvOperationUploadScreenmaps() {
		Trigger( EV_OPERATION_UPLOAD_SCREENMAPS );
	}
	void EvOperationUploadMMConfig() {
		Trigger( EV_OPERATION_UPLOAD_MMCONFIG );
	}
	void EvOperationUploadUnitConfig( ) {
		Trigger( EV_OPERATION_UPLOAD_UNITCONFIG );
	}
	void EvOperationUploadMoodConfig( ) {
		Trigger( EV_OPERATION_UPLOAD_MOODS );
	}
	void EvOperationUploadIRRXConfig( ) {
		Trigger( EV_OPERATION_UPLOAD_IRRX );
	}
	void EvOperationUploadScheduleConfig( ) {
		Trigger( EV_OPERATION_UPLOAD_SCHEDULE );
	}
	void EvOperationUploadMemory() {
		Trigger( EV_OPERATION_UPLOAD_MEMORY );
	}
	void EvOperationDownloadMemory() {
		Trigger( EV_OPERATION_DOWNLOAD_MEMORY );
	}
	void EvOperationShutdown() {
		Trigger( EV_OPERATION_SHUTDOWN );	
	}
	void EvOperationCommunicationOnline() {
		Trigger( EV_OPERATION_COMMUNICATION_ONLINE );
	}
	void EvOperationCommunicationOffline() {
		Trigger( EV_OPERATION_COMMUNICATION_OFFLINE );
	}
	void EvOperationReadAllBindings() {
		Trigger( EV_OPERATION_DOWNLOAD_BINDINGS );
	}
	void EvOperationBrowseForPhysicalNodes() {
		Trigger( EV_OPERATION_BROWSE_FOR_PHNODES );
	}
	void EvOperationRequestForStates() {
		Trigger( EV_OPERATION_REQUEST_FOR_STATES );
	}
	void EvOperationAutomaticPnp() {
		Trigger( EV_OPERATION_AUTOMATIC_PNP);
	}
	void EvOperationNicNetworkStart() {
		Trigger( EV_OPERATION_NICNETWORK_START );
	}
	// User Button
	void EvOperationTaskDialogUserButton() {
		Trigger( EV_OPERATION_TASKDIALOG_USERBUTTON );
	}
	void EvOperationDocumentLoad() {
		Trigger( EV_OPERATION_DOCUMENT, EV_OPERATION_LOAD );
	}
	void EvConvertToBindingDatabase(LPARAM event) {
		Trigger( EV_OPERATION_CONVERT_TO_BINDINGDATABASE, event);
	}
	void EvOperationMessageBox(LPARAM event) {
		Trigger( EV_OPERATION_MESSAGEBOX_RESPONSE, event);
	}
};

#endif // !defined(AFX_QHSMAPPLICATIONGLOBAL_H__1751EB61_745A_4DAE_A2A1_11E9883D5E41__INCLUDED_)
