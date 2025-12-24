#pragma once

// ref: https://www.advancedinstaller.com/user-guide/updater.html#updater-return-codes
// 

class AppIPCAdvInstallerUpdater
{
public:
	typedef enum {
		RESULT_INFO_NONE = -1,
		RESULT_INFO_UPDATES_NONE = 0,
		RESULT_INFO_UPDATES_AVAILABLE	
	} ResultInfo_t;

	typedef enum {
		SEARCHVERSIONS_RELEASE_ONLY		= 0,
		SEARCHVERSIONS_RELEASE_AND_BETA,
		SEARCHVERSIONS_EXTRA_SOFTWARE
	} SearchVersionType_t;

	AppIPCAdvInstallerUpdater();


	// Aanroepen bij de start van de applicatie.
	// Zoekt in de cloud of er updates aanwezig zijn.
	// Er wordt geen user interface getoond.
	BOOL checkForUpdates(SearchVersionType_t type,ResultInfo_t* info,CStringList* pListAvailableUpdates);

	// Aanroepen als er updates beschikbaar zijn.
	// Toont een GUI dialoog met de beschikbare updates.
	BOOL startUpdate(SearchVersionType_t type);

	// Vlaggen.
	void setOptFlagCheckForBeta(BOOL flag, BOOL save = TRUE);
	BOOL getOptFlagCheckForBeta() const;

	void setOptFlagCheckAtStartup(BOOL flag, BOOL save = TRUE);
	BOOL getOptFlagCheckAtStartup() const;

protected:
	void loadRegistryKeys();

	// Aanmaken van een process en de commandline opties uitvoeren.
	BOOL doRunUpdater(const CString& commandLine, DWORD* dwResultCode);

	// Uitlezen van het info bestand welke updates er beschikbaar zijn.
	BOOL readFileInfoUpdates(CStringList* pStrListAvailableUpdates);

private:
	enum {
		FLAG_OPTION_BETA		= (1<<0),		// controle op beta.
		FLAG_OPTION_STARTUP		= (1<<1)		// Controle bij opstarten.
	};
	int m_flags;
};


// Publieke helper functies...
struct AppIPCAdvInstallerUpdater_Util {
public:
	static BOOL checkUpdatesAtStartup(CWnd* pParent,AppIPCAdvInstallerUpdater& ref);
	static BOOL checkSoftwareUpdates(CWnd* pParent,AppIPCAdvInstallerUpdater& ref,AppIPCAdvInstallerUpdater::SearchVersionType_t type);

protected:
	static BOOL getNamesAvailable(CStringList& list,CString* pStr);
};
