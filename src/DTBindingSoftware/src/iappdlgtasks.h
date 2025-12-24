#ifndef INCLUDED_IAPPDLGTASKS_H
#define INCLUDED_IAPPDLGTASKS_H
/*****************************************************************/
#include "communication\base\communication_def.h"
/*****************************************************************/
class CCanNode;
class AppUDPDevices;
class AppUDPDevice;

/*****************************************************************/

typedef enum {
	HOMECLIENT_STATE_DISCONNECTED = 0,
	HOMECLIENT_STATE_CONNECTED_LOCAL_IP,
	HOMECLIENT_STATE_CONNECTED_REMOTE_DNS			// Momenteel nog dezelfde voor DNS en Cloud
} HomeClientConnectionState_t;

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
struct IAppDlgStartDialog_Host
{
	virtual ~IAppDlgStartDialog_Host() = 0;

	virtual void setStateAppIdle( ) = 0;
	virtual void SetStateAppModeOfflineNew( ) = 0;
	virtual void SetStateAppModeOfflineContinue( ) = 0;
	virtual void SetStateAppModeOnlineNew( ) = 0;
	virtual void SetStateAppModeOnlineContinue( ) = 0;

	virtual void setAppUsbConnect(int comport) = 0;
	virtual void setAppTcpConnect(ConnectionMethod_t method, const CString& hostName, int port, const CString& pwd, 
		const CString& cloudServerLoginToken) = 0;
	virtual void setAppDisconnect() = 0;
	
	virtual void setDefaultUSBPort(int comport) = 0;
	virtual int getDefaultUSBPort( ) = 0;

	virtual BOOL isConnected() = 0;

	// Inladen van het projectBestand.
	// Return waarde: True: ingeladen, False: niet ingeladen.
	// Param: pszProjectName naam van het ingeladen project...
	virtual BOOL loadDocumentFile(CString* pszProjectName) = 0;

	// Opvragen en updaten van de properties van de dialoog...
	typedef enum {
		OFFLINE = 0, USB = 1, TCP = 2
	} ConnectMode_t;

	// Opvragen van de ingeladen login properties.
	virtual ConnectMode_t getDefaultConnectMode() = 0;
	virtual BOOL getServerInfoLocal(CString* hostName,int* port) = 0;
	virtual BOOL getServerInfoRemote(CString* hostName,int* port) = 0;
	virtual BOOL getPreferedServerLocal() = 0;
	virtual BOOL getServerPassword(CString* password) = 0;

	// Sedert V16.28:
	// Zoeken naar een remote DNS.
	// Eerst zoeken of er ene gevonden wordt voor dit node address.
	virtual BOOL searchRemoteDns(int nodeAddress,CString* hostAddress) = 0;

	// Sedert V16.26
	// Is de LAN Server info ingevuld?
	// Wordt gebruikt om te bepalen of de optie "Search for devices in LAN-network" moet 
	// standaard aangelegd worden.
	virtual BOOL getServerInfoLocalValid( ) = 0;	

	// Updaten van de login properties.
	virtual void setDefaultConnectMode(ConnectMode_t mode) = 0;
	virtual void setServerTCPIPInfo(const CString& localHostName, int localPort, 
		const CString& remoteHostName, int remotePort, const CString& pwd, BOOL connectLocal) = 0;

	// Opvragen van de gevonden tcp/ip devices in het LAN netwerk.
	virtual AppUDPDevices& getUDPDeviceList() = 0;
	// Alle data van de UDP devices wissen - Nodig wanneer er opnieuw moet gezocht worden naar devices.
	virtual void clearUDPDeviceList() = 0;
	virtual bool updateNetworkSettings(AppUDPDevice* pDevice) = 0;
	virtual bool removeDevice(const CString& macAddress) = 0;

	// Opvragen of een device kan gebruikt worden voor TCP/IP cnfiguratie.
	// Doet een controle op de productcode.
	// Oppassen: Enkel devices vanaf een bepaalde firmware versie (>=V66.10) ondersteunen 
	// deze info.
	// Versies onder deze software versie worden niet gedeceteerd.
	virtual bool getTCPIPCfgSupported(const AppUDPDevice& r) = 0;

	// Een andere netwerk adapter selecteren.
	virtual void selectNetworkAdapter(const CString& strNwkAdapter) = 0;

	// Opvragen of de node een 0xFE (factory address heeft.)
	virtual bool getServerNodeAddressFactory( ) const = 0;

	// Opvragen van het node address van de server.
	virtual BYTE getServerNodeAddress() const = 0;
	virtual unsigned long getServerPhysicalAddress() const = 0;

	// Opvragen of de server de master is.
	virtual bool getServerNodeMaster() const = 0;

	// Opvragen of de server als master kan ingesteld worden.
	virtual bool getServerNodeMasterSupported() const = 0;
	
	// Opvragen van de node info (via tcp/ip protocol.)
	// Dit gebeurd in een verify toestand in de dialoog.
	virtual bool sendServerTCPIPGetNodeInfo( ) = 0;
	
	// Functie om de node als master in te stellen.
	virtual bool sendServerTCPIPSetMaster( ) = 0;
	
	// Functie om een node address in te stellen (via de CAN-bus.)
	virtual bool sendServerTCPIPNodeAddress(BYTE bNodeAddress) = 0;	 

	// Controle voor het toekennen van een logisch adres.
	// Kunnen deze functie gebruiken voor het zoeken naar een master 0xFC of wanneer er ene
	// toegekend wordt voor een slave.
	virtual bool SearchDbaseNodeLogicalAddress(int nodeAddress,CString* pstrName,int* physicalAddress) = 0;
};
/*****************************************************************/

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/

// Sedert V16.28:
// Bij verandering in configuratie worden de gegevens door de applicatie automatisch
// aangepast of ingevuld. 
struct IAppDlgUDPLanCfg_Host
{
	virtual ~IAppDlgUDPLanCfg_Host() = 0;

	virtual bool removeDevice(const CString& macAddress) = 0;
	virtual bool updateNetworkSettings(AppUDPDevice* pDevice) = 0;
	virtual void setUDPScanModeFast() = 0;
	virtual void setUDPScanModeNormal() = 0;
	virtual void updateNodeHomeserverService(AppUDPDevice& r) = 0;	// Sedert V16.28: Updaten van de gegevens in de node service list
	virtual void updateNodeDynDnHostname(AppUDPDevice& r) = 0;		// Sedert V16.28: Updaten van de gegevens in de node service list
};
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
struct IAppDlgHomeClientMasterTasks_Host
{
	virtual ~IAppDlgHomeClientMasterTasks_Host() = 0;

	struct IAppDlgEvents_Host {
		virtual ~IAppDlgEvents_Host() = 0;
		virtual void setStateIdle() = 0;
		virtual void setStateBusy() = 0;
		virtual void setStateConnected(bool connected) = 0;
		virtual void addInfoMsg(const CString& msg) = 0;
		virtual void addErrorMsg(const CString& msg) = 0;
		virtual void update() = 0;
	};

	virtual void init(CCanNode* pNode) = 0;
	virtual void close() = 0;

#if(0)
	virtual bool connect(const char* hostName,int port,const char* pwd) = 0;		
	virtual bool getCommSettings(CString& hostName,int& port,CString& pwd) = 0;
#endif 
	virtual bool disconnect( ) = 0;			
	virtual CString getStrAppTaskState( ) = 0;
	virtual CString getStrNodeDbaseState( ) = 0;

	virtual bool isConnected() = 0;
	virtual bool isStateIdle() = 0;
	virtual bool startTaskPnp() = 0;
	virtual bool startTaskBrowse() = 0;
	virtual bool startTaskBrowseClear() = 0;
	virtual bool startTaskUpdateDbase() = 0;
	virtual bool cancelTask() = 0;
	virtual void subscribe(IAppDlgEvents_Host* pHost) = 0;
	virtual void unsubscribe(IAppDlgEvents_Host* pHost) = 0;

	// Sedert V16.10
	virtual void showCfgNodeServices( ) = 0;
	virtual bool connectLocalPossible( ) = 0;
	virtual bool connectRemotePossible( ) = 0;
/*
	virtual bool connectLocal( ) = 0;
	virtual bool connectRemote( ) = 0;
*/

	virtual bool connectTCPHomeClient(ConnectionMethod_t method) = 0;
	virtual HomeClientConnectionState_t getTypeConnected( ) = 0;
};
/*****************************************************************/

struct IAppDlgNodeServicesCfg
{
	virtual ~IAppDlgNodeServicesCfg( ) = 0;

	virtual bool getNodePassword(int nodeAddress,CString* pwd) = 0;
	virtual bool getLocalIPSettings(int nodeAddress,CString* hostAddress,int* port) = 0;
	virtual bool getRemoteIPSettings(int nodeAddress,CString* hostAddress,int* port) = 0;

	// Sedert V16.28:
	// Zoeken naar een remote DNS.
	// Eerst zoeken of er ene gevonden wordt voor dit node address.
	virtual bool searchRemoteDns(int nodeAddress,CString* hostAddress) = 0;

	virtual bool updateNodeSettings(int nodeAddress,const CString& pwd,const CString& ipAddress,int portLocal,
		const CString& hostAddress,int portRemote) = 0;
};

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
#endif	/* INCLUDED_IAPPDLGTASKS_H */
/*****************************************************************/