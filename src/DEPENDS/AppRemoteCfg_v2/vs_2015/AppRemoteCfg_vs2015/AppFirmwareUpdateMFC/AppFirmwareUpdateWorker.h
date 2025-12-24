#pragma once

#include "appremotecfg/AppRemoteCfgApi.h"
#include "sshfirmwareupdate_api.h"			// SSH Library

using namespace DUOTECNO;
class AppFirmwareUpdaterWorker_AppRemoteCfgHost;


// Interface die moet geimplementeerd worden door de windows dialoog.
struct IAppFirmwareUpdaterWorkerWindowHost
{
public:
	virtual ~IAppFirmwareUpdaterWorkerWindowHost() = 0;
	virtual void guiAddListboxMsg(const char* msg, bool eraseContent = false) = 0;
	virtual void guiSetMsgError(const char* msg) = 0;								
	virtual void guiSetMsgStatus(const char* msg) = 0;
	virtual void guiUpdate() = 0;
	virtual void startAssyncTask() = 0;											
	virtual void startTimerPoll() = 0;
	virtual void setStatusFirmwareUpdata(bool statusSuccess) = 0;
};

// Worker...
class AppFirmwareUpdaterWorker
{
public:
	AppFirmwareUpdaterWorker(DUOTECNO::IAppRemoteCfgApi& refWorkerHTTP, IAppFirmwareUpdaterWorkerWindowHost& refGuiWindow);
	virtual ~AppFirmwareUpdaterWorker();

	typedef enum {		
		HARDWARE_UNKNOWN = -1,
		HARDWARE_RPI3 = 0,
		HARDWARE_RPI1 = 1,
		HARDWARE_CM4S = 2
	} HardwarePlatform_t;

	typedef enum {
		PRODUCT_UNKNOWN = -1,
		PRODUCT_TOUCHSCREEN = 0,
		PRODUCT_AUDIO_UNIVERSAL,
		PRODUCT_TCPIP_PRO
	} Product_t;
	
	typedef enum {
		VERSION_RELEASE = 0,
		VERSION_BETA = 1
	} Version_t;
	
	void init(const CString& hostAddress);						// Starten met Opvragen van de firmware cloud versies + fsm starten opvragen nodeinfo.
	bool startRequestNodeInfo(const CString& ipAddress);		// Opvragen van de nodeinfo.
	bool startTargetFirmwareUpdate(const CString& hostAddress, const AppRemoteCfgFirmwareVersion& refVersion); // Starten met updaten firmware.	
	bool recievedCloudFirmwareVersions( ) const;				// Opvragen of de cloud firmware versies ontvangen werden.
	bool recievedNodeInfo() const;								// Opvragen of de server (=node) info ontvangen werd.
	bool busy() const;											// Opvragen of de fsm bezig is.
	bool disconnect();											// Disconnect met de server (= node)

	void startAssyncFsmTask() {									// Gui dialoog roept deze aan bij een timerExpiry- Starten van een taak via een assync timer.
		fsm(AppFirmwareUpdaterWorker::EVENT_START_TASK);
	}
	bool poll(int msec);										// Gui dialoog roept deze aan bij een timerExpiry
	bool getReceivedNodeInfo() const;							// Geconnecteerd met de server node & info ontvangen.	

	// ............................................................
	// Opvragen van de properties van de node.
	bool getNodeInfoVersionInfo(int* firmwareVersion);	
	bool getNodeInfoHardwarePlatform(HardwarePlatform_t* platform);
	bool getNodeInfoProduct(Product_t* product);
	bool getNodeInfoPhysicalNodeAddress(int* physicalNodeAddress);
	bool getNodeInfoStrVersionInfo(CString* str);
	bool getNodeInfoStrHardwarePlatform(CString* s);
	bool getNodeInfoStrProductName(CString* s);

	// ............................................................
	// Opvragen van de firmware versies.
	bool getFirmwareVersion(const AppRemoteCfgFirmwareVersion** ppVersion,
		HardwarePlatform_t platform, Product_t product, Version_t versionType);
	static bool getNodeInfoFirmwareUpdateVersion(AppFirmwareUpdaterWorker& ref, Version_t versionType, const AppRemoteCfgFirmwareVersion** ppVersion);

	
	// ............................................................
	// Utilty functies...

	// Bepalen van hardware platform.
	// Info opgehaald via nodeinfo.json
	static HardwarePlatform_t getHardwarePlatform(const CString& processor, const CString& qtVersion);

	// Bepalen van hardware platform.
	// Info opgehaald via firmwareupdate.json
	static HardwarePlatform_t getHardwarePlatform(const CString& platform);
	static const CString getHardwarePlatform(HardwarePlatform_t platform);

	// Bepalen van het type product a.d.h.v. product string.
	// Strings zijn dezelfde in nodeinfo.json & firmwareupdate.json
	static Product_t getProduct(const CString& productName);
	static const CString getProductName(Product_t productType);

	// Conversie van Vxx.xx.xx naar een integer.
	static int getSoftwareVersion(const CString& strVersion);

	// Bepalen van de applicatie naam ifv het product.
	static const CString getApplicationName(Product_t productType);

	static int getRebootReloadValue(HardwarePlatform_t platform);
	static CString getTargetPassword(HardwarePlatform_t platform);
	static CString formatLocalFileName(const CString& remoteFileName);

protected:

	typedef enum {
		STATE_IDLE = 0,
		STATE_CHECK_FIRMWARE_NODE,
		STATE_CHECK_FIRMWARE_CLOUD,
		STATE_FIRMWARE_DOWNLOAD,
		STATE_FIRMWARE_SSH_UPLOAD,
		STATE_FIRMWARE_SSH_UPLOAD_BUSY,
		STATE_FIRMWARE_DONE_WAIT_REBOOT
	} State_t;

	typedef enum {
		EVENT_TIMER_POLL = 0,
		EVENT_START_TASK,
		EVENT_NOTIFY_TASK_DONE,
		EVENT_NOTIFY_TASK_STARTED,
		EVENT_NOTIFY_TASK_ERROR
	} Events_t;

	void fsm(Events_t ev);
	void clearContext();	
	bool startTaskRequestCloudFirmwareVersions();
	bool startTaskInstallTargetFirmware(const CString& hostAddress, const AppRemoteCfgFirmwareVersion& refVersion);
	bool pollHttpLibrary(int msec);
	bool pollSshWorker(int msec);
	void setAppStatusFlag(int flag);
	void clearAppStatusFlag(int flag);

private:	
	// Tijd dat er nodig is om een target te rebooten.
	// In de GUI dialoog wordt er een interval van 250 msec gebruikt.
	//
	// NOTE_SSH_PROBLEEM
	// Bij de CM3 is er een probleem bij de SSH connectie wanneer er een reboot uitgevoerd wordt.
	// Via een workaround wordt de SSH niet op een correct manier afgesloten. Hierdoor is er een extra 10 seconden delay.
	// Herstarten van de CM3 is trager dan de CM4S maar door deze vertraging is de waarde kleiner.
	enum {
		RELOADVALUE_REBOOT_TARGET_CM4S  = 100,			// 250 msec * 80 = 25 sec.
		RELOADVALUE_REBOOT_TARGET_CM3	= 80,			// 250 msec * 80 = 20 sec.		// NOTE_SSH_PROBLEEM (+10sec)
		RELOADVALUE_REBOOT_TARGET_RPI1  = 180			// 250 msec * 180 = 40 sec.
	};

	enum {
		FLAG_RECIEVED_FIRMWARE_VERSIONS = (1 << 0),		// Cloud server versie info ontvangen.
		FLAG_RECIEVED_NODE_INFO			= (1 << 1),		// Info ontvangen ivm de node gekoppeld aan ip-address
		FLAG_TASK_STATUS_BUSY			= (1 << 2)		// Bezig met een taak.
	};
	
	DUOTECNO::IAppRemoteCfgApi& m_refWorkerHTTP;				// http library - Worker
	IAppFirmwareUpdaterWorkerWindowHost& m_refGuiWindow;		// GUI parent window.
	int m_flagsStatusApp;						// zie enum - Helper vlaggen.
	CString m_hostAddress;						// IP-Address van de node die moet upgedate worden.
	CString m_localFileName;					// IPK lokale bestandsnaam. vb "C:\\Users\\tom\\AppData\\Roaming\\cm4s_051508_qttouchscreenapp_v66.08.06.ipk"	
	AppFirmwareUpdaterWorker_AppRemoteCfgHost* m_pRemoteCfgHost;	// http library - Worker ontvangen van events van de worker.
	DUOTECNO::ISSHFirmwareUpdate* m_pWorkerSSH;						// SSH Firmware Updater	
	const AppRemoteCfgServerInfo* m_pServerInfo;					// Server info van de Duotecno node.	
	const AppRemoteCfgFirmwareVersionInfo* m_pFirmwareVersionInfo;	// Wordt opgevraagd aan de Duotecno cloud.
	AppRemoteCfgFirmwareVersion m_CurrrentUpdateFirmwareVersion;	// Info ivm de huidige versie.	
	State_t m_state;						// De toestand.	
	int m_pollingCounter;					// polling counter - Bepaalt de frequentie waarmee sommige berichten naar de GUI verstuurd worden...	
	int m_thressholdCounterValueReboot;		// Tijdsduur die er gewacht wordt na een reboot om opnieuw de node info op te vragen.

	friend class AppFirmwareUpdaterWorker_AppRemoteCfgHost;
};
/****************************************************************************/

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
class AppFirmwareUpdaterWorker_AppRemoteCfgHost : public DUOTECNO::IAppRemoteCfgGUI
{
public:
	AppFirmwareUpdaterWorker_AppRemoteCfgHost(AppFirmwareUpdaterWorker& parent);
	virtual ~AppFirmwareUpdaterWorker_AppRemoteCfgHost();

	virtual void addMessage(const char* msg, bool eraseOld);
	virtual void addMessages(CStringList& messages, bool eraseOld);
	virtual void taskStatus(DUOTECNO::AppRemoteCfgTasks_t task, DUOTECNO::AppRemoteCfgTaskStatus_t status,
		HttpError_t errorCode = HTTP_EC_NONE, const char* msgError = 0);

private:
	AppFirmwareUpdaterWorker& m_refParent;
};
/****************************************************************************/