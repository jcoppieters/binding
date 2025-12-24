#pragma once

class CAppDlgDaliWorker;
#include "app/appdali/appdalidevices.h"

class CDlgChildDali : public CDialog, public DUOTECNO::DALI::IAppDlgDaliGui_Host {
public:
	CDlgChildDali(CAppDlgDaliWorker& refWorker, UINT nIDTemplate, CWnd* pParent = NULL);   // standard constructor

	virtual void init(DUOTECNO::DALI::CAppDaliDevice* pDevice) = 0;

	// Parent dialoog (=property dialoog) toestand gewijzigd.
	// Gebruiken om de dialogen up te daten of te melden dat er properties veranderd werdebn die
	// nog moeten doorgestuurd worden.
	virtual void OnParentPropertyPageActive(DUOTECNO::DALI::CAppDaliDevice* pDevice,bool activeDialog) = 0;		// Oppassen: setDevice() aanroepen...

	// return waarde bepaalt of de parent property dialogen moeten updaten.
	// TRUE: Update, FALSE: geen update.
	virtual BOOL OnParentPropertyPageLeave() = 0;

	// Vragen of de tab mag veranderd worden naar een ander tabblad.
	virtual BOOL onTabDialogCanLeaveDialog( ) {
		return TRUE;
	}
	virtual void onTabDialogLeaveDialog( ){ 
		// Absorbe.
	}
	virtual void onTabDialogEnterDialog( ){ 
		// Absorbe.
	}

public:
	// IAppDlgDaliGui_Host	
	virtual void onNotifyGuiStatusConnected(BYTE bNodeAddress, bool statusConnected);
	virtual void onNotifyGuiMsgCommunication(BYTE bNodeAddress, Type_t typeMessage, const CString& msg);
	virtual void onNotifyGuiTaskStarted(BYTE bNodeAddress, DUOTECNO::DALI::Task_t task);
	virtual void onNotifyGuiTaskDone(BYTE bNodeAddress, DUOTECNO::DALI::Task_t task);
	virtual void onNotifyGuiTaskError(BYTE bNodeAddress, DUOTECNO::DALI::Task_t task,const CString& strMessage);
	virtual void onNotifyGuiFoundDevice(BYTE bNodeAddress, DUOTECNO::DALI::DaliDeviceType_t dalideviceType,int shortAddress, bool newDevice);
	virtual void onNotifyGuiUpdateDeviceSerialNumber(BYTE bNodeAddress, DUOTECNO::DALI::DaliDeviceType_t dalideviceType,int shortAddress, unsigned long long serialNumber);
	virtual void onNotifyGuiUpdateDeviceGTIN(BYTE bNodeAddress, DUOTECNO::DALI::DaliDeviceType_t dalideviceType,int shortAddress, unsigned long long GTIN);
	virtual void onNotifyGuiRxDaliMessage(BYTE bNodeAddress,BYTE lengthBits,uint32_t recievedFrame);

protected:
	// DECLARE_MESSAGE_MAP()
	enum { WM_UPDATE_CONTROLS = (WM_USER + 150) };

	CAppDlgDaliWorker& getWorker() {
		return m_refWorker;
	}
	DUOTECNO::DALI::CAppDaliDevice* getDevice() {
		return this->m_pDevice;
	}
	void setDevice(DUOTECNO::DALI::CAppDaliDevice* pDevice) {
		this->m_pDevice = pDevice;
	}	

	// De viewmode van de dialoog.
	typedef enum {
		VIEWMODE_OFFLINE = 0,	
		VIEWMODE_ONLINE,
		VIEWMODE_ONLINE_BUSY 			
	} ViewMode_t;

	void updateViewMode( );
	void setViewMode(ViewMode_t mode) {
		this->m_viewMode = mode;
	}
	ViewMode_t getViewMode( ) {
		return this->m_viewMode;
	}

	// Niet bruikbaar.
	// void setCursorModeTaskBusy(bool busy);

	// Applicatie vlaggen.
	void setPropertyChanged(bool flagStateOnOff);
	bool getPropertyChanged( );

	void setUploadDone(bool flagStateOnOff);
	bool getUploadDone( );

	void setUploadBusy(bool flagStateOnOff);
	bool getUploadBusy( );

	void setDownloadDone(bool flagStateOnOff);
	bool getDownloadDone( );

	void setDownloadBusy(bool flagStateOnOff);
	bool getDownloadBusy( );

	void setInitDone(bool flagStateOnOff);
	bool getInitDone();

	void setPageCurrActive(bool flagStateOnOff);
	bool getPageCurrActive( );

	void setBusyGuiControlsUpdate(bool flagStateOnOff);
	bool getBusyGuiControlsUpdate( );

	void setRequestParentUpdateGUI(bool flagStateOnOff);
	bool getRequestParentUpdateGUI( );


	// Bijhouden van het huidige short address van het device.
	// Kan gebruikt worden om bij het toekennen van een nieuwe device te 
	// controleren of het een ander device is.
	void setCurrDeviceShortAddress(int shortAddress) {
		this->m_currDeviceShortAddress = shortAddress;
	}
	int getCurrDeviceShortAddress( ) {
		return this->m_currDeviceShortAddress;
	}

	// Een warning icon inladen en toekennen aan een static picture control.
	static BOOL initPictureControlWarningIcon(CDlgChildDali& refThis,CStatic& refPictureControl);


	// Opvragen van de string in die in de footer moet getoond worden 
	// De tekst wordt aangemaakt ifv de toestand van de dialoog.
	static CString getStrMsgInfoFooter(CDlgChildDali& refThis,bool uploadBtnNeeded = true);

	// Melding geven dat er een upload bezig is.
	static void promptUserUploadBusy(CDlgChildDali& refThis);

	// Melding geven dat er een upload bezig is.
	static void promptUserDownloadBusy(CDlgChildDali& refThis);

	// Vragen aan de gebruiker of de data moet verstuurd worden of ignore data + dialog verlaten.
	// return waarde TRUE: Upload data, FALSE: Leave dialog.
	static BOOL promptUserUploadChangesBeforeLeavingDialog(CDlgChildDali& refThis);

	// Vragen aan de gebruiker om de veranderingen te negeren.
	// return waarde TRUE: Undo changes
	static BOOL promptUserUndoChanges(CDlgChildDali& refThis);

	// Vragen aan de gebruiker om een restore to defaults uit te voeren.
	// return waarde TRUE: restore defaults.
	static BOOL promptUserRestoreDefaults(CDlgChildDali& refThis);
	
	
private:	
	enum {
		// verandering van een instelling. Kan gebruikt worden om de gebruiker te waarcshuwen bij verlaten dialoog.
		FLAG_PROPERTY_CHANGED				=	(1<<0),		

		// Er werd data uitgeload. Kan gebruikt worden om een melding te geven.
		FLAG_UPLOAD_DONE					=	(1<<1),

		// Bezig met data upload. 
		FLAG_UPLOAD_BUSY					=	(1<<2),

		FLAG_DOWNLOAD_DONE					=	(1<<3),

		FLAG_DOWNLOAD_BUSY					=	(1<<4),

		// Init gedaan (of moet opnieuw uitgevoerd worden). 
		FLAG_INIT_DONE						=	(1<<5),

		// Dialoog/pagina is actief.
		FLAG_PAGE_ACTIVE					=	(1<<6),

		// Bezig met updaten van de GUI, Nodig voor de Edit controls OnChange handlers.
		FLAG_BUSY_CONTROLS_UPDATE			=	(1<<7),

		// Vragen aan de andere property dialogen om up te daten.
		// Dit is nodig wanneer de namen veranderen in de DALI instances overview.
		FLAG_REQUEST_PARENT_UPDATE_GUI		=	(1<<8)
	} Flags;

	CAppDlgDaliWorker& m_refWorker;								// De worker.
	DUOTECNO::DALI::CAppDaliDevice* m_pDevice;					// ref naar het device.
	ViewMode_t m_viewMode;										// De viewmode (zie enum).
	unsigned short m_flags;										// Applicatie vlaggen 
	int m_currDeviceShortAddress;								// Het huidige short address van het device.	
};

