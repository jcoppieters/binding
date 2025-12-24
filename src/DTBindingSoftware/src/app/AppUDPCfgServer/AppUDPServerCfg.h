#pragma once
/************************************************************************************/
namespace DUOTECNO {
	struct IDTDeviceList;
	struct IDTDevice;
};
/************************************************************************************/

class AppUDPDevice;
class AppUDPDevices;
/************************************************************************************/
/************************************************************************************/
/************************************************************************************/
class AppUDPServerCfg
{
public:
	typedef enum {
		SCANMODE_NORMAL = 0,
		SCANMODE_FAST = 1	
	} ScanMode_t;

	AppUDPServerCfg(void);
	~AppUDPServerCfg(void);

	void setTimingsScanMode(ScanMode_t mode);

	void selectNetworkAdapter(const CString& strNwkAdapter);

	// Aanroepen bij het afsluiten van een installatie / openen van een nieuwe installatie.
	// UDP Cfg device list wissen.
	// AppUDPDevices devices verwijderen.
	void eraseData( );

	// UDP Cfg scan vragen.
	void requestScan( );

	// updaten van de lijst.
	// param pListModified: Bevat een lijst van physical node address van de devices die gewijzijgd werden.
	// Oppassen: Er kunnen ook physical addresses 0xffffff voorkomen.
	// returnwaarde true: Item gewijzigd in de lijst.
	bool updateDeviceList(CList<AppUDPDevice*>* pDevicesModified);	// CList<int>* pListModified);
	
	// Wissen van de lijst.
	void clearDeviceList( );

	bool getScanEnabled( ) {
		return m_scanEnabled;
	}
	int getIntervalScanMsec( ) const {
		return m_intervalScanMsec;
	}
	int getIntervalUpdateWaitMsec( ) const {
		return m_intervalUpdateWaitMsec;
	}
	AppUDPDevices& getDeviceList();

protected:
	bool updateNodes(DUOTECNO::IDTDeviceList* pList); 
	bool updateNode(DUOTECNO::IDTDevice* pDevice); 
	bool updateNodesFound(DUOTECNO::IDTDeviceList* pList); 
	bool getNodeListChanged(CList<AppUDPDevice*>* pDevicesModified);				


private:
	enum {
		// Normale tijden...
		INTERVAL_SCAN_TIME_MSEC		= (15 * 1000),
		INTERVAL_SCAN_TIME_WAIT		= (10 * 1000),

		// Snelle scan
		INTERVAL_SCAN_FASTMODE_TIME_MSEC = ( 3 * 1000),
		INTERVAL_SCAN_FASTMODE_TIME_WAIT = ( 8 * 1000),
	};

	// Todo: Bij het starten van de scan deze veel sneller uitvoeren zodat alle 
	// devices gevonden worden.
	AppUDPDevices* m_pList;
	bool m_scanEnabled;
	int m_intervalScanMsec;
	int m_intervalUpdateWaitMsec;
};
/************************************************************************************/

/************************************************************************************/
/************************************************************************************/
/************************************************************************************/
class CAppSupportedFlags
{
public:
	CAppSupportedFlags(void) : m_ulFlags(0) { };
	~CAppSupportedFlags() { };

	static const int CFG_SERVER;
	static const int CFG_DDNS;
	static const int CFG_SYSMODE;
	static const int CFG_TCPIPCLIENT;
	static const int CFG_DNSPROVIDER;
	static const int CFG_SNTP;
	static const int CFG_TIME;
	static const int CFG_API;

	void setFlags(unsigned long v) {
		m_ulFlags = v;
	}

	BOOL IsSupported(unsigned long flag) {
		return (((m_ulFlags & flag) == flag) ? TRUE : FALSE);
	}

private:
	unsigned long m_ulFlags;
};

/************************************************************************************/
/************************************************************************************/
/************************************************************************************/
// De instellingen voor Homeserver Applicatie.
class AppUDPDeviceHomeServerCfg
{
public:
	AppUDPDeviceHomeServerCfg( );
	virtual ~AppUDPDeviceHomeServerCfg( );

	void clear();
	bool changed() const;	
	void resetChanged( );

	const CString& getPassword( ) const {
		return m_szPassword;
	}
	void setPassword(const CString& password);
	
	int getIPPort( ) const {
		return m_ipPort;
	}
	void setIPPort(int port);

	int getProtocolFlags( ) const {
		return m_protocolFlags;
	}
	void setProtocolFlags(int flags);

private:
	enum {
		FLAG_CHANGED_PASSWORD			= (1 << 0),
		FLAG_CHANGED_IPPORT				= (1 << 1),
		FLAG_CHANGED_PROTOCOLFLAGS		= (1 << 2),
	};
	int m_changedFlags;				// Houdt bij of er een member veranderd werd.
	CString m_szPassword;
	int m_ipPort;
	int m_protocolFlags;
};

/************************************************************************************/
/************************************************************************************/
/************************************************************************************/
// De instellingen voor DynDns.
class AppUDPDeviceDDNSCfg
{
public:
	AppUDPDeviceDDNSCfg( );
	virtual ~AppUDPDeviceDDNSCfg( );

	typedef enum {
		PROVIDER_NONE = 0,
		PROVIDER_NOIP = 1
	} Provider_t;

	typedef enum {
		STATUS_DISABLED	= 0,   		
		STATUS_GOOD		= 1,       
		STATUS_ERROR	= 2,		
		STATUS_WAIT 	= 3			
	} Status_t;	// Runtime status

	void clear();
	bool changed() const;	
	void resetChanged( );

	// properties.
	void setStateOnOff(int state);
	int getStateOnOff() const {
		return m_stateOnOff;
	}

	void setProvider(Provider_t provider);
	Provider_t getProvider() const {
		return m_provider;
	}

	const CString& getHostname( ) const {
		return m_hostName;
	}
	void setHostName(const CString& hostName);

	const CString& getUsername( ) const {
		return m_userName;
	}
	void setUserName(const CString& username);
	
	const CString& getPassword( ) const {
		return m_szPassword;
	}
	void setPassword(const CString& password);

	Status_t getStatus() const {
		return m_status;
	}
	void setStatus(Status_t status);

	// Utilities.
	static void convertToString(CString* pStr, Status_t status);

private:
	enum {
		FLAG_CHANGED_ONOFF				= (1 << 0),
		FLAG_CHANGED_PROVIDER			= (1 << 1),
		FLAG_CHANGED_HOSTNAME			= (1 << 2),
		FLAG_CHANGED_USERNAME			= (1 << 3),
		FLAG_CHANGED_PASSWORD			= (1 << 4),
		FLAG_CHANGED_STATUS				= (1 << 5)
	};

	int m_changedFlags;				// Houdt bij of er een member veranderd werd.
	int m_stateOnOff;
	Provider_t m_provider;					
	Status_t m_status;				// runtime status
	CString m_hostName;
	CString m_userName;
	CString m_szPassword;
};
/************************************************************************************/

/************************************************************************************/
/************************************************************************************/
/************************************************************************************/
// De instellingen voor de SystemMode.
class AppUDPDeviceSystemModeCfg
{
public:
	AppUDPDeviceSystemModeCfg( );
	virtual ~AppUDPDeviceSystemModeCfg( );

	typedef enum {
		BASIC = 0,
		EXTENDED = 1
	} SystemMode_t;

	void clear();
	bool changed() const;	
	void resetChanged( );

	// properties.
	SystemMode_t getSystemMode() const {
		return m_systemMode;
	}
	void setSystemMode(SystemMode_t mode); 

private:
	enum {
		FLAG_CHANGED					= (1 << 0)
	};

	int m_changedFlags;				// Houdt bij of er een member veranderd werd.
	SystemMode_t m_systemMode;
};
/************************************************************************************/

/************************************************************************************/
/************************************************************************************/
/************************************************************************************/
// De instellingen voor de DNS.
class AppUDPDeviceDnsServerCfg
{
public:
	AppUDPDeviceDnsServerCfg( );
	virtual ~AppUDPDeviceDnsServerCfg( );

	void clear();
	bool changed() const;	
	void resetChanged( );

	void setDNSServerAddress(const CString& serverAddress);
	const CString& getDNSServerAddress( ) const {
		return m_dnsServerAddress;
	}

private:
	enum {
		FLAG_CHANGED					= (1 << 0)
	};

	int m_changedFlags;					// Houdt bij of er een member veranderd werd.
	CString m_dnsServerAddress;			// dns server ip address
};
/************************************************************************************/

/************************************************************************************/
/************************************************************************************/
/************************************************************************************/
// De instellingen voor de tcp/ip client (alarm / audio).
class AppUDPDeviceTCPIPClientCfg
{
public:
	AppUDPDeviceTCPIPClientCfg( );
	virtual ~AppUDPDeviceTCPIPClientCfg( );

	void clear();
	bool changed() const;	
	void resetChanged( );

	// Properties.
	const CString& getServerAddress( ) const {
		return m_serverAddress;
	}
	void setServerAddress(const CString& str);

	int getServerPort() const {
		return m_serverPort;
	}
	void setServerPort(int port);

	int getStatusConnected() const {
		return m_statusConnected;
	}
	void setStatusConnected(int status);

private:
	enum {
		FLAG_CHANGED_ADDRESS	= (1 << 0),
		FLAG_CHANGED_PORT		= (1 << 1),
		FLAG_CHANGED_STATUS		= (1 << 2)
	};

	int m_changedFlags;					// Houdt bij of er een member veranderd werd.
	CString m_serverAddress;			// server ip address
	int m_serverPort;					// server ip port
	int m_statusConnected;				// status connected 0 of 1.
};

/************************************************************************************/
/************************************************************************************/
/************************************************************************************/
// De instellingen voor de SNTP Time server
class AppUDPDeviceNtpTimeCfg
{
public:
	AppUDPDeviceNtpTimeCfg( );
	virtual ~AppUDPDeviceNtpTimeCfg( );

	void clear();
	bool changed() const;	
	void resetChanged( );

	// properties.
	int getPoolID( ) const {
		return m_poolID;
	}
	void setPoolID(int pool);

	int getUTCZone() const {
		return m_utcZone;
	}
	void setUTCZone(int zone);

	int getDSTEnable( ) const {
		return m_dstEnabled;
	}
	void setDSTEnable(int value);

private:
	enum {
		FLAG_CHANGED_POOL		= (1 << 0),
		FLAG_CHANGED_UTCZONE	= (1 << 1),
		FLAG_CHANGED_DST		= (1 << 2)
	};

	int m_changedFlags;					// Houdt bij of er een member veranderd werd.
	int m_poolID;						// Area en pool id (zelfde waarden.)
	int m_utcZone;
	int m_dstEnabled;
};

/************************************************************************************/
/************************************************************************************/
/************************************************************************************/
class AppUDPDevice {
public:
	AppUDPDevice(const CString& macAddress);
	AppUDPDevice(const AppUDPDevice& r);					// !!!
	~AppUDPDevice( );

	AppUDPDevice& operator=(const AppUDPDevice& r);			// !!!

	// Wissen van de lijst + deallocatie.
	// void erase( );

	// getters & setters
	void setName(const char*);
	void setIpAddress(const char*);
	void setGateway(const char*);
	void setNetmask(const char*);
	void setPhysicalAddress(int v);
	void setNodeAddress(int v);
	void setSoftwareVersion(int v);
	void setProductCode(int v);
	void setDHCPOnOff(bool v);
	// void storeLastFoundFlag();		
	void setLastFound(bool flag);

	const CString& getMACAddress() const {
		return m_macAddress;
	}
	const CString& getName() const {
		return m_deviceName;
	}
	const CString& getIpAddress( ) const {
		return m_ipAddress;
	}
	const CString& getGateway( ) const {
		return m_gateway;
	}
	const CString& getNetmask( ) const {
		return m_netmask;
	}
	int getPhysicalAddress( ) const {
		return m_physicalNodeAddress;
	}
	int getNodeAddress() const {
		return m_nodeAddress;
	}
	int getProductCode( ) const {
		return m_productCode;
	}
	int getSoftwareVersion() const {
		return m_softwareVersion;
	}
	bool getDhcpOnOff( ) const {
		return ((m_flags & FLAG_DHCP) == FLAG_DHCP);
	}
	bool getLastFound() const {
		return ((m_flags & FLAG_FOUND_LASTSCAN) == FLAG_FOUND_LASTSCAN);
	}

	// Modified / changed vlag.
	bool getChanged() const {
		return ((m_flags & FLAG_CHANGED) == FLAG_CHANGED);
	}
	void setChanged(bool v);

	CAppSupportedFlags& getAppSupportedFlags();

	// Opvragen van de Specifieke App instellingen.
	AppUDPDeviceHomeServerCfg& getAppHomeServerCfg();	
	AppUDPDeviceDDNSCfg& getAppDDNSCfg();
	AppUDPDeviceSystemModeCfg& getAppSystemMode();
	AppUDPDeviceDnsServerCfg& getAppDnsServerCfg();
	AppUDPDeviceNtpTimeCfg& getAppNtpTimeCfg();
	AppUDPDeviceTCPIPClientCfg& getAppTCPIPClientCfg();

	static bool formatMacAddress(CString* dest,const BYTE* pbMacAddress);
	static bool convertMacAddress(BYTE* pbMacAddress, const CString& s);

	// Enkel de netwerk instellingen worden upgedate.
	static void updateNetworkSettings(AppUDPDevice* pDest,const AppUDPDevice& r);

	// Opvragen of het device NXP hardware is.
	static bool IsHardwareNxp(const AppUDPDevice& r);

private:

	enum {
		FLAG_CHANGED = (1<<0),
		FLAG_DHCP = (1<<1),
		FLAG_FOUND_LASTSCAN = (1<<2)		
	};

	const CString m_macAddress;
	CString m_deviceName;
	CString m_ipAddress;
	CString m_gateway;
	CString m_netmask;
	int m_physicalNodeAddress;
	int m_nodeAddress;
	int m_productCode;
	int m_softwareVersion;
	int m_flags;
	
	CAppSupportedFlags* m_pAppSupportedFlags;
	AppUDPDeviceHomeServerCfg* m_pAppHomeServerCfg;
	AppUDPDeviceDDNSCfg* m_pAppDDNSConfig;
	AppUDPDeviceSystemModeCfg* m_pAppSystemModeCfg;
	AppUDPDeviceDnsServerCfg* m_pAppDnsServerCfg;
	AppUDPDeviceNtpTimeCfg* m_pAppNtpTimeCfg;
	AppUDPDeviceTCPIPClientCfg* m_pAppTCPIPClientCfg;
	// OPPASSEN: ALLE MEMBERS IN COPY ASSIGNMENT + CONSTRUCTOR INVOEGEN.

	friend class AppUDPDevices;
};
/************************************************************************************/

/************************************************************************************/
/************************************************************************************/
/************************************************************************************/
class AppUDPDevices {
public:
	AppUDPDevices( );
	~AppUDPDevices( );

	void erase( );
	bool find(const CString& strMacAddress, AppUDPDevice **ppDevice) const;
	bool find(int physicalNodeAddress, const AppUDPDevice **ppDevice) const;
	bool add(AppUDPDevice* pNewDevice);
	int getCount();
	bool getAt(int index,AppUDPDevice **ppDevice);
	bool remove(const CString& strMacAddress);
	
private:
	CList<AppUDPDevice*> m_list;
};
/************************************************************************************/


/************************************************************************************/
/************************************************************************************/
/************************************************************************************/
struct AppUDPDevices_Utils {

	// Controleren of het device terug te vinden is in het netwerk met de instellingen die er geconfigureerd werden.
	// Moet een geldig ip-adres hebben. (geen 0.0.0.0 of 255.255.255.255).
	// Wanneer ingesteld is als DHCP moet de instelling DHCP aanstaan.
	// Bij een FIX ip-adres: Het ip-adres van het gevonden devie moet identiek zijn met deze die geconfigureerd werd.
	static bool updateNetworkSettings(const AppUDPDevices& refDevices, AppUDPDevice* pDest);
};

/************************************************************************************/


/************************************************************************************/
/************************************************************************************/
/************************************************************************************/
struct AppUDPDevice_Utils {

	// Utility functies die gebruikt worden om het MAC-Address te koppelen aan de item
	// data van een MFC GUI element.
	// De laatste vier bytes van het MAC-Address worden vertaald naar een 32 bit 
	// integer. (De eerste 2 gaan dus verloren).
	//
	// Bij NXP modules zijn de eerste 3 bytes specifiek voor de NXP modules, de laatste
	// 3 bytes zijn het physical node address.
	// Bij de RPI modules (of wifi) wordt het MAC-Address aangemaakt door de fabrikant.
	// 
	// Voorbeeld:
	//	1E:30:6C:0C:03:05	--> Itemdata = 0x6C0C0305
	//
	// Functies worden gebruikt in de CDlgUDPLanDevices dialoog en in CMainFrame::OnShowNodeUDPLanCfg()
	static uint32_t convertMACAddress2Uint32(BYTE* pbMACAddress);
	static uint32_t convertMACAddress2Uint32(const AppUDPDevice& refDevice);
};
/************************************************************************************/