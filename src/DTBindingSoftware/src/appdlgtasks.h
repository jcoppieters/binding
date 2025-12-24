#ifndef INCLUDED_APPDLGTASKS_H
#define INCLUDED_APPDLGTASKS_H
/*****************************************************************/
#ifndef INCLUDED_IAPPDLGTASKS_H
	#include "iappdlgtasks.h"
#endif 
#ifndef INCLUDED_TCPSOCKET_ITCPSOCKETHOMECLIENT_H
	#include "communication/tcpsocket/homeclient/itcpsockethomeclient.h"	//CAppDlgHomeClientMasterTasks_Host.
#endif 
/*****************************************************************/
class AppUDPServerCfg;

namespace DUOTECNO {
	class AppRemoteCfgLoginInfo;		// Library...
	class AppRemoteCfgNodes;			// Library...
	class AppRemoteCfgNode;				// Library...
};
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
class CAppDlgStartDialog_Host : public IAppDlgStartDialog_Host
{
public:
	CAppDlgStartDialog_Host(DUOTECNO::AppRemoteCfgLoginInfo& refLoginInfo,AppUDPServerCfg& refUDPServerCfg,DUOTECNO::AppRemoteCfgNodes& refCfgNodeServices);
	virtual ~CAppDlgStartDialog_Host();

	virtual void setStateAppIdle( );
	virtual void SetStateAppModeOfflineNew( );
	virtual void SetStateAppModeOfflineContinue( );
	virtual void SetStateAppModeOnlineNew( );
	virtual void SetStateAppModeOnlineContinue( );

	virtual void setAppUsbConnect(int comport);
	virtual void setAppTcpConnect(ConnectionMethod_t method, const CString& hostName, int port, const CString& pwd, const CString& cloudServerLoginToken);
	virtual void setAppDisconnect();	

	virtual void setDefaultUSBPort(int comport);
	virtual int getDefaultUSBPort( );

	// Opvragen van de status 
	virtual BOOL isConnected();

	virtual BOOL loadDocumentFile(CString* pszProjectName);

	// Opvragen van de ingeladen login properties.
	virtual ConnectMode_t getDefaultConnectMode();
	virtual BOOL getServerInfoLocal(CString* hostName,int* port);
	virtual BOOL getServerInfoRemote(CString* hostName,int* port);
	virtual BOOL getPreferedServerLocal();
	virtual BOOL getServerPassword(CString* password);

	// Sedert V16.28: Wanneer er geen remote DNS bekend is gaan we zoeken in de node service lijst 
	// om er ergens ene te vinden. Deze wordt meestal per project geconfigureerd.
	virtual BOOL searchRemoteDns(int nodeAddress, CString* hostAddress);

	// Is de LAN Server info ingevuld?
	virtual BOOL getServerInfoLocalValid( );		

	// Updaten van de login properties.
	virtual void setDefaultConnectMode(ConnectMode_t mode);
	virtual void setServerTCPIPInfo(const CString& localHostName, int localPort, 
		const CString& remoteHostName, int remotePort, const CString& pwd, BOOL connectLocal);

	// Opvragen van de gevonden tcp/ip devices in het LAN netwerk.
	virtual AppUDPDevices& getUDPDeviceList();
	virtual void clearUDPDeviceList();
	virtual bool updateNetworkSettings(AppUDPDevice* pDevice);
	virtual bool removeDevice(const CString& macAddress);
	virtual bool getTCPIPCfgSupported(const AppUDPDevice& r);
	virtual void selectNetworkAdapter(const CString& strNwkAdapter);

	virtual bool getServerNodeAddressFactory() const;
	virtual BYTE getServerNodeAddress() const;
	virtual unsigned long getServerPhysicalAddress() const;
	virtual bool getServerNodeMaster() const;
	virtual bool getServerNodeMasterSupported() const;

	virtual bool sendServerTCPIPGetNodeInfo();
	virtual bool sendServerTCPIPSetMaster();
	virtual bool sendServerTCPIPNodeAddress(BYTE bNodeAddress);

	virtual bool SearchDbaseNodeLogicalAddress(int nodeAddress,CString* pstrName,int* physicalAddress);

private:
	DUOTECNO::AppRemoteCfgLoginInfo& m_refLoginInfo;
	AppUDPServerCfg& m_refUDPServerCfg;
	DUOTECNO::AppRemoteCfgNodes& m_refCfgNodeServices;
};
/*****************************************************************/


/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
class CAppDlgUDPLanCfg_Host : public IAppDlgUDPLanCfg_Host
{
public:
	CAppDlgUDPLanCfg_Host(AppUDPServerCfg& refUDPServerCfg,DUOTECNO::AppRemoteCfgNodes& refAppRemoteCfgNodes,
						  DUOTECNO::AppRemoteCfgLoginInfo& refAppRemoteCfgLoginInfo);
	virtual ~CAppDlgUDPLanCfg_Host();

	virtual bool removeDevice(const CString& macAddress);
	virtual bool updateNetworkSettings(AppUDPDevice* pDevice);
	virtual void setUDPScanModeFast();
	virtual void setUDPScanModeNormal();
	virtual void updateNodeHomeserverService(AppUDPDevice& r);		// Sedert V16.28: Updaten van de gegevens in de node service list		
	virtual void updateNodeDynDnHostname(AppUDPDevice& r);			// Sedert V16.28: Updaten van de gegevens in de node service list	

private:
	AppUDPServerCfg& m_refUDPServerCfg;
	DUOTECNO::AppRemoteCfgNodes& m_refAppRemoteCfgNodes;
	DUOTECNO::AppRemoteCfgLoginInfo& m_refAppRemoteCfgLoginInfo;		// Sedert V16.28: Updaten van de login info.
};

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
struct ITcpSocketHomeClient;		// Voorwaatse declaratie.
/*****************************************************************/
class CAppDlgHomeClientMasterTasks_Host : public IAppDlgHomeClientMasterTasks_Host,
										  public ITcpSocketHomeClient_Host	// events van de Homeclient...
{
public:
	CAppDlgHomeClientMasterTasks_Host( );
	virtual ~CAppDlgHomeClientMasterTasks_Host();
	
	virtual void init(CCanNode* pNode);
	virtual void close();

	virtual bool disconnect( );
	virtual CString getStrAppTaskState( ) { return m_strAppTask; };
	virtual CString getStrNodeDbaseState( ) { return m_strNodeDbaseState; }

	virtual bool isConnected();
	virtual bool isStateIdle();
	virtual bool startTaskPnp();
	virtual bool startTaskBrowse();
	virtual bool startTaskBrowseClear();
	virtual bool startTaskUpdateDbase();
	virtual bool cancelTask();
	virtual void subscribe(IAppDlgEvents_Host* pHost);
	virtual void unsubscribe(IAppDlgEvents_Host* pHost);

	// Dialoog tonen.
	virtual void showCfgNodeServices( );
	// Connectie maken.
	virtual bool connectLocalPossible( );
	virtual bool connectRemotePossible( );
	virtual bool connectTCPHomeClient(ConnectionMethod_t method);

	// Sedert V16.10: Bepalen hoe we geconnecteerd zijn: lokaal ip/dns.
	virtual HomeClientConnectionState_t getTypeConnected( );

	// ITcpSocketHomeClient_Host
	virtual void onStateConnectChanged(int id,bool connected);
	virtual void onStateChanged(int id, TcpSocketHomeClient_Tasks::State_t state);
	virtual void onSocketHomeClientMsgInfo(int id, CategoryMsg_t category, const CString& msg);
	virtual void onSocketHomeClientMsgError(int id, CategoryMsg_t category, const CString& msg);
	virtual void onTaskStatus(int id, TcpSocketHomeClient_Tasks::Task_t task, TcpSocketHomeClient_Tasks::Status_t status);
	virtual void onTaskCancelStatus(int id, TcpSocketHomeClient_Tasks::Status_t status);
	virtual void onNodeDbaseStatus(int id, TcpSocketHomeClient_NodeDbase::Status_t status);
	virtual void onStatusDALIRxMsgSubscribe(int id,bool status);
	virtual void onStatusDALIResponse(int id, int clientIdentifier, BYTE msgIdentifier, BYTE type, BYTE responseData);
	virtual void onRxDALIMessage(int id, BYTE lengthBits, uint32_t recievedFrame);

protected:
	void updateStringAppState(TcpSocketHomeClient_Tasks::State_t state);
	void updateStringNodeDatabase(TcpSocketHomeClient_NodeDbase::Status_t status);

private:
	// OPPASSEN: MOGEN GEEN INSTANTIE VAN 'ITcpSocketHomeClient' bijhouden !!!
	IAppDlgEvents_Host* m_pHost;
	CString m_strAppTask;
	CString m_strNodeDbaseState;	 
	CCanNode* m_pNode;				// Nodig voor opvragen van de homeClient socket - wordt meegegeven in de init()
};
/*****************************************************************/


/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
class CAppDlgNodeServicesCfg_Host : public IAppDlgNodeServicesCfg
{
public:
	CAppDlgNodeServicesCfg_Host(DUOTECNO::AppRemoteCfgNodes& refCfg);
	virtual ~CAppDlgNodeServicesCfg_Host();

	// Opvragen van de gegevens.
	bool getNodePassword(int nodeAddress,CString* pwd);
	bool getLocalIPSettings(int nodeAddress,CString* hostAddress,int* port);
	bool getRemoteIPSettings(int nodeAddress,CString* hostAddress,int* port);
	bool searchRemoteDns(int nodeAddress,CString* hostAddress);

	// Opslaan van de gegevens.
	virtual bool updateNodeSettings(int nodeAddress,const CString& pwd,const CString& ipAddress,int portLocal,
		const CString& hostAddress,int portRemote);
private:
	DUOTECNO::AppRemoteCfgNodes& m_refCfg;
};
/*****************************************************************/
#endif 
/*****************************************************************/
