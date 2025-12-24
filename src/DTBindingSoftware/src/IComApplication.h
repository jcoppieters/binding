#ifndef ICOMAPPLICATION_H
#define ICOMAPPLICATION_H


typedef enum
{
	COMAPPLICATION_TASK_ID_RENAME_ENGINE				= 0,
	COMAPPLICATION_TASK_ID_MULTIMEDIA_CONFIG			= 1,
	COMAPPLICATION_TASK_ID_REQUEST_FOR_STATUSSEN		= 2,
	COMAPPLICATION_TASK_ID_BINDINGS_READWRITE_ENGINE	= 3,
	COMAPPLICATION_TASK_ID_SCREENMAP_UPDATE_ENGINE		= 4,
	COMAPPLICATION_TASK_ID_MEMORYDUMP_ENGINE			= 5,
	COMAPPLICATION_TASK_ID_BROWSEPNP_ENGINE				= 6,
	COMAPPLICATION_TASK_ID_MODEMCONNECT_ENGINE			= 7,
	COMAPPLICATION_TASK_ID_NICERRORSFSM_ENGINE			= 8,
	COMAPPLICATION_TASK_ID_UNITCREATE_CONFIG			= 9,
	COMAPPLICATION_TASK_ID_UPLOAD_MOODS					= 10,
	COMAPPLICATION_TASK_ID_UPLOAD_IRRX					= 11,
	COMAPPLICATION_TASK_ID_UPLOAD_SCHEDULE				= 12,
	COMAPPLICATION_TASK_ID_FTP							= 13,		// since 0x0562
	COMAPPLICATION_TASK_ID_UPLOAD_SMAP_PAGEINFO			= 14,		// since 0x0571
	COMAPPLICATION_TASK_ID_UPLOAD_SMAP_MENU				= 15,
	COMAPPLICATION_TASK_ID_UPLOAD_SMAP_AVROOM			= 16,		// since 0x0610
	COMAPPLICATION_TASK_ID_UPLOAD_GATEWAYCFG			= 17,		// since 0x0620
	COMAPPLICATION_TASK_ID_UPLOAD_ROOMCTRL_AUDIOCFG		= 18,		// since 0x0625
	COMAPPLICATION_TASK_ID_UPLOAD_BOSEST_CFG			= 19,		// since 0x0802
	COMAPPLICATION_TASK_ID_UPLOAD_LEDSCFG				= 20,		// since 0x0804
	COMAPPLICATION_TASK_ID_UPLOAD_SMAPLITECFG			= 21,		// Since 0x0C05

} ENUM_COMAPPLICATION_TASK;


class IComApplication
{
public:
	// General ...
	virtual void StartTimer(UINT nIDEvent,UINT nElapse)					=0;
	virtual void StopTimer(UINT nIDEvent)								=0;
	virtual void Trigger(UINT msg)										=0;

	// New: 20061128
	virtual void NotifyOnTaskDone(ENUM_COMAPPLICATION_TASK id)			=0;
	virtual void NotifyOnTaskError(ENUM_COMAPPLICATION_TASK id)			=0;
	virtual void NotifyOnTaskCancelled(ENUM_COMAPPLICATION_TASK id)		=0;


	// interfaces to CNodeLcdScreenmapFSM
	virtual void NotifyEventScreenmapFSMInfo(CString s)					=0;


	// Interface to CSerialCommObject
	virtual void NotifyMessageSend(void)								=0;
	//virtual void NotifyCommandSend(void)								=0;
	virtual void NotifyReceivedAck(void)								=0;
	virtual void NotifyReceivedNack(void)								=0;
	virtual void NotifyReceivedMessage(void)							=0;
	virtual void NotifyReceivedModemStatus(void)						=0;
	virtual void NotifyReceivedModemAddress(void)						=0;
	virtual void NotifyReceivedModemVersion(void)						=0;
	virtual void NotifyModemCommunicationError(void)					=0;
	virtual void NotifyModemCanLoggerStateChanged(void)					=0;

	// Interface to CBrowsePnpEngine ...
	virtual void NotifyBrowseOk(void)									=0;
	virtual void NotifyPnpOk(void)										=0;
	virtual void NotifyBrowsePnpError(void)								=0;
	virtual void NotifyBrowsePnpCancel(void)							=0;
	virtual void NotifyBrowsePnpTimeout(void)							=0;
	virtual void NotifyPnpDuplicatesFound(void)							=0;
	virtual void NotifyPnpFactoryAddressFound(void)						=0;

	
	// Interface to RenameNode Units Engine ...
	//virtual void NotifyRenameFSMEnded(void)							=0;

	// Interface for CBindingsReadWriteFSM
	virtual void NotifyEraseBindingsNodeStart(BYTE bNodeAddress)		=0;
	virtual void NotifyEraseBindingsNodeDone(BYTE bNodeAddress)			=0;
	virtual void NotifyEraseBindingsNodeError(BYTE bNodeAddress)		=0;
	virtual void NotifyWriteBindingsNodeStart(BYTE bNodeAddress)		=0;
	virtual void NotifyWriteBindingsNodeDone(BYTE bNodeAddress)			=0;
	virtual void NotifyWriteBindingsNodeError(BYTE bNodeAddress)		=0;
	virtual void NotifyReadBindingsNodeStart(BYTE bNodeAddress)			=0;
	virtual void NotifyReadBindingsNodeDone(BYTE bNodeAddress)			=0;
	virtual void NotifyReadBindingsNodeError(BYTE bNodeAddress)			=0;
	virtual void NotifyWriteBindingsFSMDone(void)						=0;
	virtual void NotifyReadBindingsFSMDone(void)						=0;

	// Nic Errors ...
	virtual void NotifyNicErrorsNodeFsmDone(BYTE bNodeAddress)	= 0;
	virtual void NotifyNicErrorsNodeFsmError(BYTE bNodeAddress)	= 0;
	virtual void NotifyNicErrorsNodeFsmRetry(BYTE bNodeAddress) = 0;	// Sedert V16.30

	// ...
	virtual void ResetCommunication()			=0;
	virtual BYTE RequestModemStatus()			=0;
	virtual BYTE RequestModemAddress()			=0;
	virtual BYTE RequestModemVersion()			=0;	

	// Sedert V16.07
	virtual void NotifyConnectStatusChanged(BOOL statusConnect) = 0;

	// Sedert V16.43	
	virtual BYTE MessageTransfer(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bMsgCode) = 0;
	virtual BYTE MessageTransfer(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bMsgCode,
								BYTE bLength,CString szData) = 0;
};


#endif

