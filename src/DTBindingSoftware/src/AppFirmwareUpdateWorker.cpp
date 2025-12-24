#include "stdafx.h"
#include "AppFirmwareUpdateWorker.h"
//#include "sshfirmwareupdate_api.h"			// SSH Library


/****************************************************************************************************/
// Bij de audio interface RPI1 V66.07.01 wordt er nog geen info teruggegeven ifv het hardware platform.
// Daarom als het een CM3 audio V66.07.01 is gaan we het platform wijzigen naar RPI1
//
#undef USE_WORKAROUND_AUDIO_RPI1
/****************************************************************************************************/
#define USE_WORKAROUND_AUDIO_RPI1			1
/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
AppFirmwareUpdaterWorker::AppFirmwareUpdaterWorker(DUOTECNO::IAppRemoteCfgApi& refWorkerHTTP, IAppFirmwareUpdaterWorkerWindowHost& refGuiWindow) :
	m_refWorkerHTTP(refWorkerHTTP), m_refGuiWindow(refGuiWindow), m_flagsStatusApp(0),
	m_localFileName( ), m_pRemoteCfgHost(NULL), m_pWorkerSSH(NULL), m_pServerInfo(NULL), m_pFirmwareVersionInfo(NULL),
	m_CurrrentUpdateFirmwareVersion( ), m_state(STATE_IDLE), m_pollingCounter(0),
	m_thressholdCounterValueReboot(RELOADVALUE_REBOOT_TARGET_CM3)
{ 
	// App Remote Cfg worker...
	m_pRemoteCfgHost = new AppFirmwareUpdaterWorker_AppRemoteCfgHost(*this);	
	m_refWorkerHTTP.setSubscriberGui(m_pRemoteCfgHost);

	// SSH Worker....
	m_pWorkerSSH = sshfirmwareupdate_create();
}

AppFirmwareUpdaterWorker::~AppFirmwareUpdaterWorker() {
	if (NULL != m_pWorkerSSH) {
		sshfirmwareupdate_destroy(&m_pWorkerSSH);
	}
	if (NULL != m_pRemoteCfgHost) {
		delete m_pRemoteCfgHost;
		m_pRemoteCfgHost = NULL;
	}
}

void AppFirmwareUpdaterWorker::init(const CString& hostAddress) {	
	clearAppStatusFlag(FLAG_RECIEVED_FIRMWARE_VERSIONS);
	clearAppStatusFlag(FLAG_RECIEVED_NODE_INFO);
	m_hostAddress = hostAddress;
	m_state = STATE_CHECK_FIRMWARE_CLOUD;
	m_refGuiWindow.startAssyncTask();
}

bool AppFirmwareUpdaterWorker::recievedCloudFirmwareVersions() const {
	return (FLAG_RECIEVED_FIRMWARE_VERSIONS == (m_flagsStatusApp & FLAG_RECIEVED_FIRMWARE_VERSIONS));
}
bool AppFirmwareUpdaterWorker::recievedNodeInfo() const {
	return (FLAG_RECIEVED_NODE_INFO == (m_flagsStatusApp & FLAG_RECIEVED_NODE_INFO));
}
bool AppFirmwareUpdaterWorker::busy() const {
	return (FLAG_TASK_STATUS_BUSY == (m_flagsStatusApp & FLAG_TASK_STATUS_BUSY));
}

void AppFirmwareUpdaterWorker::fsm(Events_t ev) {
	switch (m_state) {
		case STATE_CHECK_FIRMWARE_CLOUD: {
			switch (ev) {				
				case EVENT_START_TASK: {
					clearAppStatusFlag(FLAG_RECIEVED_FIRMWARE_VERSIONS);										
					if (this->startTaskRequestCloudFirmwareVersions()) {		// Opvragen van de firmware versie info aan de cloud server.
						m_state = STATE_CHECK_FIRMWARE_CLOUD;
					}
					else {
						m_refGuiWindow.guiSetMsgError("Checking cloud server firmware versions info failed.");
						m_state = STATE_IDLE;
					}
				} break;				
				case EVENT_TIMER_POLL: {
					if (0 == (m_pollingCounter%10)) {
						m_refGuiWindow.guiAddListboxMsg("Checking cloud server firmware versions info busy, please wait...");
					}
				} break;
				case EVENT_NOTIFY_TASK_DONE: {
					setAppStatusFlag(FLAG_RECIEVED_FIRMWARE_VERSIONS);
					m_refGuiWindow.guiAddListboxMsg("Checking cloud server firmware versions info done.");	
					m_refGuiWindow.guiSetMsgStatus("Checking cloud server firmware versions info done.");					
					if (m_hostAddress.IsEmpty()) {
						clearAppStatusFlag(FLAG_TASK_STATUS_BUSY);
						m_state = STATE_IDLE;
					}
					else {
						setAppStatusFlag(FLAG_TASK_STATUS_BUSY);
						m_refGuiWindow.startAssyncTask();
						m_state = STATE_CHECK_FIRMWARE_NODE;
					}
					m_refGuiWindow.guiUpdate();
				} break;
				case EVENT_NOTIFY_TASK_ERROR: {
					clearAppStatusFlag(FLAG_RECIEVED_FIRMWARE_VERSIONS);
					m_refGuiWindow.guiAddListboxMsg("Checking cloud server firmware versions info failed.");
					m_refGuiWindow.guiSetMsgError("Checking cloud server firmware versions info failed.");
					m_refGuiWindow.guiSetMsgStatus("Checking cloud server firmware versions info failed.");
					clearAppStatusFlag(FLAG_TASK_STATUS_BUSY);
					m_refGuiWindow.guiUpdate();
					m_state = STATE_IDLE;
				} break;
			}
		} break;

		case STATE_CHECK_FIRMWARE_NODE: {
			switch (ev) {
				case EVENT_START_TASK: {				
					m_refGuiWindow.guiSetMsgStatus("Checking node firmware version.");
					m_refGuiWindow.guiAddListboxMsg("Checking node firmware version.");
					if (!this->startRequestNodeInfo(m_hostAddress)) {
						m_refGuiWindow.guiSetMsgError("Checking node firmware version failed");
						m_refGuiWindow.guiSetMsgStatus("Checking node firmware version failed");
						clearAppStatusFlag(FLAG_TASK_STATUS_BUSY);
						m_refGuiWindow.guiUpdate();
						m_state = STATE_IDLE;
					}										
				} break;
				case EVENT_TIMER_POLL: {
					if ((m_pollingCounter % 10) == 0) {
						m_refGuiWindow.guiAddListboxMsg("Checking node firmware version busy, please wait...");
					}
				} break;
				case EVENT_NOTIFY_TASK_DONE: {
					if (this->getReceivedNodeInfo()) {
						m_refGuiWindow.guiSetMsgStatus("Checking node firmware version done.");
						m_refGuiWindow.guiAddListboxMsg("Checking node firmware version done.");
						setAppStatusFlag(FLAG_RECIEVED_NODE_INFO);						
					}					
					clearAppStatusFlag(FLAG_TASK_STATUS_BUSY);
					m_refGuiWindow.guiUpdate();
					m_state = STATE_IDLE;
				} break;
				case EVENT_NOTIFY_TASK_ERROR: {
					m_refGuiWindow.guiSetMsgStatus("Checking node firmware version failed");
					m_refGuiWindow.guiAddListboxMsg("Checking node firmware version failed.");
					m_refGuiWindow.guiSetMsgError("Checking node firmware version failed.");
					clearAppStatusFlag(FLAG_RECIEVED_NODE_INFO);
					clearAppStatusFlag(FLAG_TASK_STATUS_BUSY);
					m_refGuiWindow.guiUpdate();					
					m_state = STATE_IDLE;
				} break;
			};
		} break;

		case STATE_FIRMWARE_DOWNLOAD: {
			switch (ev) {
				case EVENT_NOTIFY_TASK_DONE: {					
					m_refGuiWindow.guiAddListboxMsg("Downloading firmware from the cloud done.");
					m_refGuiWindow.startAssyncTask();
					m_state = STATE_FIRMWARE_SSH_UPLOAD;
				} break;
				case EVENT_TIMER_POLL: {
					if ((m_pollingCounter % 10) == 0) {
						m_refGuiWindow.guiAddListboxMsg("Downloading firmware from the cloud busy, please wait...");
					}
				} break;
				case EVENT_NOTIFY_TASK_ERROR: {
					m_refGuiWindow.guiAddListboxMsg("Downloading firmware from the cloud failed.");
					m_refGuiWindow.guiSetMsgError("Downloading firmware from the cloud failed!");
					clearAppStatusFlag(FLAG_TASK_STATUS_BUSY);
					m_refGuiWindow.guiUpdate();					
					m_state = STATE_IDLE;
				} break;
			}
		} break;

		case STATE_FIRMWARE_SSH_UPLOAD: {
			switch (ev) {
				case EVENT_START_TASK: {					
					m_refGuiWindow.guiAddListboxMsg("Updating target firmware.");
					if (startTaskInstallTargetFirmware(m_hostAddress, m_CurrrentUpdateFirmwareVersion)) {
						setAppStatusFlag(FLAG_TASK_STATUS_BUSY);
						m_state = STATE_FIRMWARE_SSH_UPLOAD_BUSY;
					}			
					else { 
						m_refGuiWindow.guiAddListboxMsg("Updating target firmware failed.");
						m_refGuiWindow.guiSetMsgError("Updating target firmware failed.");
						clearAppStatusFlag(FLAG_TASK_STATUS_BUSY);
						m_refGuiWindow.guiUpdate();
						m_state = STATE_IDLE;
					}
				} break;
			}
		} break;

		case STATE_FIRMWARE_SSH_UPLOAD_BUSY: {
			switch (ev) {
				case EVENT_NOTIFY_TASK_DONE: {
					m_refGuiWindow.guiAddListboxMsg("Updating target firmware done. Wait for target rebooting");		
					m_refGuiWindow.guiSetMsgStatus("Installing firmware done. Wait for target rebooting");
					this->disconnect();
					this->clearContext();				
					setAppStatusFlag(FLAG_TASK_STATUS_BUSY);
					m_pollingCounter = 0;	// Oppassen: Deze op 0 zetten - wordt gebruikt om de tijd nodig voor rebooten te bepalen.
					m_refGuiWindow.setStatusFirmwareUpdata(true);	// Status ok = true.
					m_state = STATE_FIRMWARE_DONE_WAIT_REBOOT;
				} break;
				case EVENT_NOTIFY_TASK_ERROR: {									
					m_refGuiWindow.guiSetMsgError("Updating target firmware failed.");
					m_refGuiWindow.guiSetMsgStatus("Installing firmware failed.");
					this->disconnect();
					this->clearContext();
					clearAppStatusFlag(FLAG_TASK_STATUS_BUSY);
					m_refGuiWindow.setStatusFirmwareUpdata(false);	// Status failed = false.
					m_refGuiWindow.guiUpdate();
					m_state = STATE_IDLE;					
				} break;
				case EVENT_TIMER_POLL: {
					if (0 == (m_pollingCounter % 5)) {
						m_refGuiWindow.guiSetMsgStatus("Installing target firmware.");
					}				
					else if (0 == (m_pollingCounter % 9)) {
						m_refGuiWindow.guiSetMsgStatus("Installing target firmware busy, please wait...");
					}
				} break;
			}
		} break;

		case STATE_FIRMWARE_DONE_WAIT_REBOOT: {
			switch (ev) {
				case EVENT_TIMER_POLL: {
					if (m_pollingCounter >= m_thressholdCounterValueReboot) {
						m_pollingCounter = 0;
						if (m_hostAddress.IsEmpty()) {
							clearAppStatusFlag(FLAG_TASK_STATUS_BUSY);
							m_state = STATE_IDLE;
						}
						else {
							m_refGuiWindow.guiAddListboxMsg("Checking node firmware version.");
							setAppStatusFlag(FLAG_TASK_STATUS_BUSY);
							m_refGuiWindow.startAssyncTask();
							m_state = STATE_CHECK_FIRMWARE_NODE;
						}
					}
					else if ((m_pollingCounter % 20) == 0) {
						m_refGuiWindow.guiAddListboxMsg("Target rebooting, please wait...");
					}
				} break;
			}
		} break;
	}
}

bool AppFirmwareUpdaterWorker::startRequestNodeInfo(const CString& ipAddress) {
	static const int port = 8080;
	m_localFileName.Empty();	
	m_refGuiWindow.guiSetMsgStatus(NULL);		// Empty
	m_pServerInfo = NULL;
	m_hostAddress = ipAddress;
	m_refWorkerHTTP.setHostInfo(ipAddress, port);

	if (AppRemoteCfgApi::RESULT_SUCCESS == m_refWorkerHTTP.startTask(APPREMOTE_CFG_TASK_CONNECT, NULL)) {
		m_refGuiWindow.guiSetMsgStatus("Checking node firmware version.");
		clearAppStatusFlag(FLAG_RECIEVED_NODE_INFO);
		setAppStatusFlag(FLAG_TASK_STATUS_BUSY);
		m_refGuiWindow.startTimerPoll();
		m_state = STATE_CHECK_FIRMWARE_NODE;
		return true;
	}
	return false;
}

bool AppFirmwareUpdaterWorker::disconnect() {
	m_refGuiWindow.guiSetMsgStatus(NULL);		// Empty
	m_refWorkerHTTP.disConnect();
	m_pServerInfo = NULL;
	return true;
}

bool AppFirmwareUpdaterWorker::startTaskRequestCloudFirmwareVersions() {		
	if (AppRemoteCfgApi::RESULT_SUCCESS == m_refWorkerHTTP.startTask(APPREMOTE_CFG_TASK_REQUEST_FIRMWAREVERSIONS, NULL)) {
		m_refGuiWindow.guiSetMsgStatus("Checking cloud server firmware versions info.");
		m_refGuiWindow.guiAddListboxMsg("Checking cloud server firmware versions info.");
		m_refGuiWindow.startTimerPoll();
		setAppStatusFlag(FLAG_TASK_STATUS_BUSY);	
		return true;
	}
	return false;
}

// Tijd na de reboot dat er gewacht wordt om opnieuw de node info op te vragen.
int AppFirmwareUpdaterWorker::getRebootReloadValue(HardwarePlatform_t platform) {
	int retValue = RELOADVALUE_REBOOT_TARGET_CM3;
	switch (platform) {
		case HARDWARE_RPI1:
			retValue = RELOADVALUE_REBOOT_TARGET_RPI1;
		break;
		case HARDWARE_CM4S:
			retValue = RELOADVALUE_REBOOT_TARGET_CM4S;
		break;	
		case HARDWARE_RPI3:
			retValue = RELOADVALUE_REBOOT_TARGET_CM3;
		break;
	}
	return retValue;
}

CString AppFirmwareUpdaterWorker::getTargetPassword(HardwarePlatform_t platform) {
	CString strPassword;
	char ch = '?';
	switch (platform) {
		case HARDWARE_UNKNOWN: {
			//TODO ERROR			
		} break;
		case HARDWARE_RPI3: {
			ch = '0';
		} break;
		case HARDWARE_RPI1: {
			ch = '1';
		} break;
		case HARDWARE_CM4S: {
			ch = '4';
		} break;
	}
	strPassword.Format("du0t3cn%c%%", ch);
	return strPassword;
}

bool AppFirmwareUpdaterWorker::startTaskInstallTargetFirmware(const CString& hostAddress, const AppRemoteCfgFirmwareVersion& refVersion) {
	if (NULL == m_pWorkerSSH) {
		return false;
	}

	const HardwarePlatform_t platform = getHardwarePlatform(refVersion.getPlatform());
	const Product_t product = getProduct(refVersion.getProductName());
	if (HARDWARE_UNKNOWN == platform) {
		ASSERT(0);
		return false;
	}
	if (PRODUCT_UNKNOWN == platform) {
		ASSERT(0);
		return false;
	}

	const CString strApplicationName = getApplicationName(product);
	const CString strUserName("root");
	const CString strPassword = getTargetPassword(platform);
	if (m_pWorkerSSH->startTaskUpdate(hostAddress, strUserName, strPassword, strApplicationName, m_localFileName) >= 0) {	
		m_thressholdCounterValueReboot = getRebootReloadValue(platform);		
		setAppStatusFlag(FLAG_TASK_STATUS_BUSY);
		return true;
	}
	return false;
}

bool AppFirmwareUpdaterWorker::pollHttpLibrary(int msec) {	
	// Poll de HTTP Library...
	m_refWorkerHTTP.poll(msec);

	if (m_refWorkerHTTP.connected() && NULL == m_pServerInfo) {
		const AppRemoteCfgServerInfo* serverInfo = 0;
		if (m_refWorkerHTTP.getServerInfo(&serverInfo)) {
			m_pServerInfo = serverInfo;
		}
	}
	return true;
}

bool AppFirmwareUpdaterWorker::pollSshWorker(int msec) {
	if (NULL == m_pWorkerSSH) {
		return false;
	}

	// Berichten weergeven in de listbox...
	SshMessage msg;
	while (m_pWorkerSSH->getMessage(&msg)) {
		if (SshMessage::TYPE_MSG_ERROR == msg.getType()) {
			CString strMsg; 
			strMsg.Format("ERROR: %s", msg.getMsg());
			m_refGuiWindow.guiAddListboxMsg(msg.getMsg());			
		}
		else if (SshMessage::TYPE_MSG_INFO == msg.getType()) {
			m_refGuiWindow.guiAddListboxMsg(msg.getMsg());
		}
	}

	// Status doorgeven aan de fsm...
	const SshTaskStatus_t status = m_pWorkerSSH->poll(msec);
	if (status == TASK_STATUS_ERROR) {
		fsm(EVENT_NOTIFY_TASK_ERROR);
	}
	else if (TASK_STATUS_DONE == status) {
		fsm(EVENT_NOTIFY_TASK_DONE);
	}
	else if (status == TASK_STATUS_BUSY_CONNECT) {
		// Poll interval = 250 msec...		
		if ((m_pollingCounter % 10) == 0) {
			m_refGuiWindow.guiAddListboxMsg("Connect to the host busy, please wait...");
		}
	}
	return true;
}

bool AppFirmwareUpdaterWorker::poll(int msec) {
	m_pollingCounter++;
	fsm(AppFirmwareUpdaterWorker::EVENT_TIMER_POLL);
	const bool r1 = pollHttpLibrary(msec);
	const bool r2 = pollSshWorker(msec);
	return (r1 && r2);
}

bool AppFirmwareUpdaterWorker::getReceivedNodeInfo() const {	
	return m_refWorkerHTTP.connected();
}

bool AppFirmwareUpdaterWorker::getNodeInfoVersionInfo(int* firmwareVersion) {
	if (NULL == m_pServerInfo) {
		return false;
	}
	const CString& strVersion = m_pServerInfo->getSoftwareVersion();
	*firmwareVersion = getSoftwareVersion(strVersion);
	return true;
}

bool AppFirmwareUpdaterWorker::getNodeInfoStrVersionInfo(CString* str) {
	if (NULL == m_pServerInfo) {
		return false;
	}
	*str = m_pServerInfo->getSoftwareVersion();
	return true;
}

bool AppFirmwareUpdaterWorker::getNodeInfoStrHardwarePlatform(CString* s) {
	if (NULL == m_pServerInfo) {
		return false;
	}
	HardwarePlatform_t platform;
	if (!getNodeInfoHardwarePlatform(&platform)) {
		return false;
	}

#if (USE_WORKAROUND_AUDIO_RPI1 == 1)
	if (HARDWARE_RPI3 == platform) {
		Product_t productType = PRODUCT_UNKNOWN;
		if (getNodeInfoProduct(&productType)) {	
			if (PRODUCT_AUDIO_UNIVERSAL == productType) {
				int firmwareVersion = 0;
				if (getNodeInfoVersionInfo(&firmwareVersion)) {
					if ((((firmwareVersion & 0xFF0000) >> 16) == 0x66) &&
						(((firmwareVersion & 0xFF00) >> 8) == 0x07) &&
						((firmwareVersion & 0xFF) == 0x01)) 
					{
						platform = HARDWARE_RPI1;
					}
				}
			}
		}
	}
#endif

	*s = getHardwarePlatform(platform);
	return true;
}
bool AppFirmwareUpdaterWorker::getNodeInfoStrProductName(CString* s) {
	if (NULL == m_pServerInfo) {
		return false;
	}
	*s = m_pServerInfo->getProductName();
	return true;
}


bool AppFirmwareUpdaterWorker::getNodeInfoHardwarePlatform(HardwarePlatform_t* platform) {
	if (NULL == m_pServerInfo) {
		return false;
	}
	*platform = getHardwarePlatform(m_pServerInfo->getHardware(), m_pServerInfo->getQtVersion());
	return true;
}

bool AppFirmwareUpdaterWorker::getNodeInfoProduct(Product_t* product) {
	if (NULL == m_pServerInfo) {
		return false;
	}
	const CString& productName = m_pServerInfo->getProductName();
	*product = getProduct(productName);
	return true;
}

bool AppFirmwareUpdaterWorker::getNodeInfoPhysicalNodeAddress(int* physicalNodeAddress) {
	if (NULL == m_pServerInfo) {
		return false;
	}
	*physicalNodeAddress = (int)m_pServerInfo->getPhysicalAddress();
	return true;
}

int AppFirmwareUpdaterWorker::getSoftwareVersion(const CString& strVersion) {
	int v[3] = { 0, 0 , 0 };
	if (3 == sscanf((LPCSTR)strVersion, "V%02x.%02x.%02x", &v[0], &v[1], &v[2])) {
		return (v[0] << 16) + (v[1] << 8) + v[2];
	}
	return 0;
}

// Bepalen van de applicatie naam ifv het product.
const CString AppFirmwareUpdaterWorker::getApplicationName(Product_t productType) {
	switch (productType) {
		case PRODUCT_UNKNOWN:
			return "Unknown";
		case PRODUCT_TOUCHSCREEN:
			return "qttouchscreenapp";
		case PRODUCT_AUDIO_UNIVERSAL:
			return "qtnodeavapp";
		case PRODUCT_TCPIP_PRO:
		case PRODUCT_SMARTBOX_ULTRA:
			return "qtnodegatewayapp";
	}
	return "Unknown";
}

bool AppFirmwareUpdaterWorker::getFirmwareVersion(const AppRemoteCfgFirmwareVersion** ppVersion,
	HardwarePlatform_t platform, Product_t product, Version_t versionType)
{	
	if (NULL == m_pFirmwareVersionInfo) {
		const AppRemoteCfgFirmwareVersionInfo* p = NULL;
		IAppRemoteCfgApi::Result_t res = m_refWorkerHTTP.getFirmwareVersionsInfo(&p);
		if (IAppRemoteCfgApi::RESULT_SUCCESS == res) {
			m_pFirmwareVersionInfo = p;
		}
	}
	if (!AppRemoteCfgFirmwareVersionInfo_Utils::isValid(*m_pFirmwareVersionInfo)) {
		return false;
	}

	bool typeReleaseVersion = true;
	if (VERSION_RELEASE == versionType) {
		typeReleaseVersion = true;
	}
	else {
		typeReleaseVersion = false;
	}

	const AppRemoteCfgFirmwareVersion* releaseVersion = NULL;
	const CString strPlatform = getHardwarePlatform(platform);
	const CString strProductName = getProductName(product);
	if (AppRemoteCfgFirmwareVersionInfo_Utils::find(*m_pFirmwareVersionInfo, strProductName, strPlatform, typeReleaseVersion, &releaseVersion)) {
		*ppVersion = releaseVersion;
		return true;
	}
	return false;
}

// Server node...
bool AppFirmwareUpdaterWorker::getNodeInfoFirmwareUpdateVersion(AppFirmwareUpdaterWorker& ref, Version_t versionType, const AppRemoteCfgFirmwareVersion** ppVersion) {
	HardwarePlatform_t platform = HARDWARE_UNKNOWN;
	Product_t productType = PRODUCT_UNKNOWN;

	if (!ref.getNodeInfoHardwarePlatform(&platform)) {
		platform = AppFirmwareUpdaterWorker::HARDWARE_UNKNOWN;
		return false;
	}
	if (!ref.getNodeInfoProduct(&productType)) {
		productType = AppFirmwareUpdaterWorker::PRODUCT_UNKNOWN;
		return false;
	}

#if (USE_WORKAROUND_AUDIO_RPI1 == 1)
	if (PRODUCT_AUDIO_UNIVERSAL == productType && HARDWARE_RPI3 == platform) {
		int firmwareVersion = 0;
		if (ref.getNodeInfoVersionInfo(&firmwareVersion)) {
			if ((((firmwareVersion & 0xFF0000) >> 16) == 0x66) &&
				(((firmwareVersion & 0xFF00) >> 8) == 0x07) &&
				((firmwareVersion & 0xFF) == 0x01)) 
			{
				platform = HARDWARE_RPI1;
			}
		}
	}
#endif 

	if (!ref.getFirmwareVersion(ppVersion, platform, productType, versionType)) {
		return false;
	}	
	return true;
}

static CString getFileName(CString fullpath) {
	CString resToken;
	CString fileName;
	int curPos = 0;
	resToken = fullpath.Tokenize(_T("/"), curPos);
	while (resToken != _T("")) {	
		resToken = fullpath.Tokenize(_T("/"), curPos);
		if (resToken != _T("")) {
			fileName = resToken;
		}
	};
	return fileName;
}

// Example:
// "C:\\Users\\tom\\AppData\\Roaming\\cm4s_051508_qttouchscreenapp_v66.08.06.ipk"
CString AppFirmwareUpdaterWorker::formatLocalFileName(const CString& remoteFileName) {
	CString str;
	TCHAR szPath[MAX_PATH];
	memset(szPath, 0, sizeof(szPath));
	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, szPath))) {
		str.Format("%s\\%s", szPath, getFileName(remoteFileName));
	}
	else {
		str = getFileName(remoteFileName);
	}
	return str;
}

bool AppFirmwareUpdaterWorker::startTargetFirmwareUpdate(const CString& hostAddress, const AppRemoteCfgFirmwareVersion& refVersion) {
	const CString versionNumber = refVersion.getVersion();
	const CString fileNameRemote = refVersion.getIpk();
	m_hostAddress = hostAddress;
	m_CurrrentUpdateFirmwareVersion = refVersion;
	m_localFileName = formatLocalFileName(fileNameRemote);
	if (appremotecfg_startDownloadFile(&m_refWorkerHTTP, fileNameRemote, m_localFileName)) {
		CString msg;
		msg.Format("Installing firmware version %s", versionNumber);
		m_refGuiWindow.guiSetMsgStatus(msg);			
		m_refGuiWindow.guiAddListboxMsg(msg,true);
		m_refGuiWindow.guiAddListboxMsg("Downloading firmware from the cloud.");
		setAppStatusFlag(FLAG_TASK_STATUS_BUSY);
		m_refGuiWindow.startTimerPoll();
		m_refGuiWindow.guiUpdate();
		m_state = STATE_FIRMWARE_DOWNLOAD;
		return true;
	}
	return false;
}

void AppFirmwareUpdaterWorker::clearContext() {
	m_localFileName.Empty();
	m_pServerInfo = NULL;
	m_pFirmwareVersionInfo = NULL;	
	m_state = STATE_IDLE;
}	

void AppFirmwareUpdaterWorker::setAppStatusFlag(int flag) {
	m_flagsStatusApp |= flag;
}
void AppFirmwareUpdaterWorker::clearAppStatusFlag(int flag) {
	m_flagsStatusApp &= ~flag;
}


// RPI3: Eerste versie - Bij de eerste versie geen processor info & qtversie beschikbaar.
// RPI1: BCM2835 && 5.15.8
// CM4S: BCM2711 && 5.18.8
AppFirmwareUpdaterWorker::HardwarePlatform_t AppFirmwareUpdaterWorker::getHardwarePlatform(const CString& processor, const CString& qtVersion) {
	HardwarePlatform_t platform = AppFirmwareUpdaterWorker::HARDWARE_RPI3;
	if (0 == processor.Compare("BCM2835") && 0 == qtVersion.Compare("5.11.1")) {
		platform = AppFirmwareUpdaterWorker::HARDWARE_RPI3;
	}
	else if (0 == processor.Compare("BCM2835") && 0 == qtVersion.Compare("5.15.8")) {
		platform = AppFirmwareUpdaterWorker::HARDWARE_RPI1;
	}
	else if (0 == processor.Compare("BCM2711") && 0 == qtVersion.Compare("5.15.8")) {
		platform = AppFirmwareUpdaterWorker::HARDWARE_CM4S;
	}
	return platform;
}


static const char* const STRING_PLATFORM_RPI1 = "rpi1";
static const char* const STRING_PLATFORM_CM3 = "cm3";
static const char* const STRING_PLATFORM_CM4S = "cm4s";

AppFirmwareUpdaterWorker::HardwarePlatform_t AppFirmwareUpdaterWorker::getHardwarePlatform(const CString& strPlatform) {
	HardwarePlatform_t platform = AppFirmwareUpdaterWorker::HARDWARE_UNKNOWN;
	if (0 == strPlatform.Compare(STRING_PLATFORM_RPI1)) {
		platform = AppFirmwareUpdaterWorker::HARDWARE_RPI1;
	}
	else if (0 == strPlatform.Compare(STRING_PLATFORM_CM3)) {
		platform = AppFirmwareUpdaterWorker::HARDWARE_RPI3;
	}
	else if (0 == strPlatform.Compare(STRING_PLATFORM_CM4S)) {
		platform = AppFirmwareUpdaterWorker::HARDWARE_CM4S;
	}
	return platform;
}

const CString AppFirmwareUpdaterWorker::getHardwarePlatform(HardwarePlatform_t platform) {
	if (AppFirmwareUpdaterWorker::HARDWARE_RPI1 == platform) {
		return STRING_PLATFORM_RPI1;
	}
	else if (AppFirmwareUpdaterWorker::HARDWARE_RPI3 == platform) {
		return STRING_PLATFORM_CM3;
	}
	else if (AppFirmwareUpdaterWorker::HARDWARE_CM4S == platform) {
		return STRING_PLATFORM_CM4S;
	}
	return "unknown";
}

// Bepalen van het type product a.d.h.v. product string.
static const char* const STRING_PRODUCT_TOUCHSCREEN = "Touchscreen 100 Virtuals";
static const char* const STRING_PRODUCT_AUDIO_IP = "AVMatrix IP + 8Channel IRTX";
static const char* const STRING_PRODUCT_HOMESERVER = "TCP/IP homeserver";
static const char* const STRING_PRODUCT_SMARTBOXULTRA = "Smartbox ultra";

AppFirmwareUpdaterWorker::Product_t AppFirmwareUpdaterWorker::getProduct(const CString& productName) {
	AppFirmwareUpdaterWorker::Product_t product = AppFirmwareUpdaterWorker::PRODUCT_UNKNOWN;
	if (0 == productName.Compare(STRING_PRODUCT_TOUCHSCREEN)) {
		product = AppFirmwareUpdaterWorker::PRODUCT_TOUCHSCREEN;
	}
	else if (0 == productName.Compare(STRING_PRODUCT_AUDIO_IP)) {
		product = AppFirmwareUpdaterWorker::PRODUCT_AUDIO_UNIVERSAL;
	}
	else if (0 == productName.Compare(STRING_PRODUCT_HOMESERVER)) {
		product = AppFirmwareUpdaterWorker::PRODUCT_TCPIP_PRO;
	}
	else if (0 == productName.Compare(STRING_PRODUCT_SMARTBOXULTRA)) {
		product = AppFirmwareUpdaterWorker::PRODUCT_SMARTBOX_ULTRA;
	}
	return product;
}

const CString AppFirmwareUpdaterWorker::getProductName(Product_t productType) {
	if (AppFirmwareUpdaterWorker::PRODUCT_TOUCHSCREEN == productType) {
		return STRING_PRODUCT_TOUCHSCREEN;
	}
	else if (AppFirmwareUpdaterWorker::PRODUCT_AUDIO_UNIVERSAL == productType) {
		return STRING_PRODUCT_AUDIO_IP;
	}
	else if (AppFirmwareUpdaterWorker::PRODUCT_TCPIP_PRO == productType) {
		return STRING_PRODUCT_HOMESERVER;
	}
	else if (AppFirmwareUpdaterWorker::PRODUCT_SMARTBOX_ULTRA == productType) {
		return STRING_PRODUCT_SMARTBOXULTRA;
	}
	return "";
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
AppFirmwareUpdaterWorker_AppRemoteCfgHost::AppFirmwareUpdaterWorker_AppRemoteCfgHost(AppFirmwareUpdaterWorker& parent) : 
	m_refParent(parent) { }
AppFirmwareUpdaterWorker_AppRemoteCfgHost::~AppFirmwareUpdaterWorker_AppRemoteCfgHost() { }

void AppFirmwareUpdaterWorker_AppRemoteCfgHost::addMessage(const char* msg, bool eraseOld) { }

void AppFirmwareUpdaterWorker_AppRemoteCfgHost::addMessages(CStringList& messages, bool eraseOld) { }

void AppFirmwareUpdaterWorker_AppRemoteCfgHost::taskStatus(AppRemoteCfgTasks_t task, AppRemoteCfgTaskStatus_t status,
	HttpError_t errorCode, const char* msgError)
{
	switch (task) {
		case APPREMOTE_CFG_TASK_CONNECT:
		case APPREMOTE_CFG_TASK_REQUEST_FIRMWAREVERSIONS:
		case APPREMOTE_CFG_TASK_DOWNLOADFILE:
		{
			switch (status) {
				case APPREMOTE_CFG_TASK_STATUS_STARTED: {
					m_refParent.fsm(AppFirmwareUpdaterWorker::EVENT_NOTIFY_TASK_STARTED);
				} break;
				case APPREMOTE_CFG_TASK_STATUS_DONE: {
					m_refParent.fsm(AppFirmwareUpdaterWorker::EVENT_NOTIFY_TASK_DONE);
				} break;
				case APPREMOTE_CFG_TASK_STATUS_FAILED: {
					if (0 != msgError) {
						m_refParent.m_refGuiWindow.guiAddListboxMsg(msgError);
					}
					m_refParent.fsm(AppFirmwareUpdaterWorker::EVENT_NOTIFY_TASK_ERROR);
				} break;
			}
		} break;
	}
}
/****************************************************************************/

