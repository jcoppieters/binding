#pragma once

struct IAppDlgStartDialog_Host;	
class AppUDPDevice;


/////////////////////////////////////////////////////////////////////////////
// Publieke enum
typedef enum {

	DIALOGMODE_APPLICATION_START = 0,		// Bij het starten/inladen van een nieuwe applicatie.
	DIALOGMODE_APPLICATION_CONNECT = 1		// Wanneer in offline mode de connect button.

} PropertyPageNewAppMode_t;

/////////////////////////////////////////////////////////////////////////////
class CPropertyPageNewAppContext
{
public:
	typedef enum {
		PROJECT_TYPE_NEW = 0,
		PROJECT_TYPE_EXISTING
	} ProjectType_t;

	typedef enum { 
		MODE_CONNECT_OFFLINE = 0,
		MODE_CONNECT_USB,
		MODE_CONNECT_TCPIP 
	} ModeConnect_t;

	typedef enum {
		STATE_DISCONNECTED = 0,
		STATE_CONNECTED_USB,
		STATE_CONNECTED_TCPIP
	} StateConnected_t;
	

	CPropertyPageNewAppContext(PropertyPageNewAppMode_t mode, IAppDlgStartDialog_Host& refHost);
	~CPropertyPageNewAppContext();

	// Opvragen van de host acties...
	IAppDlgStartDialog_Host& getHost();

	// Type van het project.
	ProjectType_t getProjectType( ) const {
		return m_projectType;
	} 
	void setProjectType(ProjectType_t type) {
		m_projectType = type;
	}
	
	// Project ingeladen.
	BOOL getProjectLoaded( ) const {
		return m_projectLoaded;
	}
	void setProjectLoaded(BOOL flag) {
		m_projectLoaded = flag;
	}

	// Naam van het project.
	CString getProjectName( ) const {
		return m_projectName;
	}
	void setProjectName(const CString& projectName) {
		m_projectName = projectName;
	}

	// Mode connectie...
	ModeConnect_t getModeConnect() const {
		return m_modeConnect;
	}
	void setModeConnect(ModeConnect_t mode) {
		m_modeConnect = mode;
	}

	// Connect local/remote.
	BOOL getTcpIpConnectLocal() const {
		return m_fConnectLocal;
	}
	void setTcpIpConnectLocal(BOOL flag) {
		m_fConnectLocal = flag;
	}

	// Local Ip-Address
	CString getLocalHostAddress() const {
		return m_hostAddrSettings[0].m_hostAddress;
	}
	void setLocalHostAddress(const CString& addr) {
		m_hostAddrSettings[0].m_hostAddress = addr;
	}
	int getLocalIPPort() const {
		return m_hostAddrSettings[0].m_ipPort;
	}
	void setLocalIPPort(int port) {
		m_hostAddrSettings[0].m_ipPort = port;
	}

	// Remote Ip-Address
	CString getRemoteHostAddress() const {
		return m_hostAddrSettings[1].m_hostAddress;
	}
	void setRemoteHostAddress(const CString& addr) {
		m_hostAddrSettings[1].m_hostAddress = addr;
	}
	int getRemoteIPPort() const {
		return m_hostAddrSettings[1].m_ipPort;
	}
	void setRemoteIPPort(int port) {
		m_hostAddrSettings[1].m_ipPort = port;
	}

	// Password.
	CString getPassword() const {
		return m_Password;
	}
	void setPassword(const CString& password) {
		m_Password = password;
	}

	// State connected...
	StateConnected_t getStateConnected( ) const {
		return m_stateConnected;
	}
	void setStateConnected(StateConnected_t state) {
		m_stateConnected = state;
	}

	// Sedert V16.26
	// Opvragen van de proxy - oppassen deze kan 0 zijn...
	AppUDPDevice* getUDPDevice();
	const AppUDPDevice* getUDPDevice() const;

	// Sedert V16.26
	// Instellen van de proxy - indien NULL dan wordt de huidige proxy verwijderd.
	// Maakt een copy van een UDP Device...
	// Verwijdert alle data van de proxy...
	void setUDPDevice(AppUDPDevice* pDevice);

	// Sedert V16.26: Wizard
	void setFlagSearchAgain(BOOL value);
	BOOL getFlagSearchAgain() const;

	// Sedert V16.26: mode - zie enum
	// Dialoog kan geopend worden als start dialoog maar ook als dialoog om van
	// offline mode naar online mode te gaan.
	const PropertyPageNewAppMode_t getDlgMode() const {
		return m_mode;
	}

protected:
	void dealocate(AppUDPDevice** ppDevice);

private:
	enum {
		FLAG_WIZARD_SEARCH_AGAIN	=	(1 << 0)	// Sedert v16.26: Wanneer er een fout is in de nwk config.
	}; 

	typedef struct {
		CString m_hostAddress;					// ip-address of dns name.
		int m_ipPort;
	} STRUCT_HOSTADDRESS_SETTINGS;


	const PropertyPageNewAppMode_t m_mode;
	IAppDlgStartDialog_Host& m_refHost;
	ProjectType_t m_projectType;				// Type van het project.
	BOOL m_projectLoaded;						// Project ingeladen.
	CString m_projectName;						// Naam van het project
	ModeConnect_t m_modeConnect;				// Connectie methode: offline,usb,tcpip
	BOOL m_fConnectLocal;						// Connect tcpip local.	
	STRUCT_HOSTADDRESS_SETTINGS m_hostAddrSettings[2];	// 0 is local, 1 is remote. 
	CString m_Password;
	StateConnected_t m_stateConnected;
	AppUDPDevice* m_pProxyUDPDevice;			// Sedert v16.26: Bevat een proxy naar een UDP Device.
	int m_flags;								// Sedert v16.26: zie enum flags.
};
/*****************************************************/

struct CPropertyPageNewAppContext_Util 
{
	static void updatePassword(CPropertyPageNewAppContext* pDest,IAppDlgStartDialog_Host &refSrc);
	static void updateLocalIPsettings(CPropertyPageNewAppContext* pDest,IAppDlgStartDialog_Host &refSrc);
	static void updateRemoteIPsettings(CPropertyPageNewAppContext* pDest,IAppDlgStartDialog_Host &refSrc);
	static void updatePreferedTCPIPMethod(CPropertyPageNewAppContext* pDest,IAppDlgStartDialog_Host &refSrc);
	static void updateDefaultConnectMode(CPropertyPageNewAppContext* pDest,IAppDlgStartDialog_Host &refSrc);
};