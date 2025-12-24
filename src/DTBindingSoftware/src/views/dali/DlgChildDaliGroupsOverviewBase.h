#pragma once

class CAppDlgDaliWorker;
#include "app/appdali/appdalidevices.h"

class CDlgChildDaliGroupsOverviewBase : public CDialog, public DUOTECNO::DALI::IAppDlgDaliGui_Host {
public:
	CDlgChildDaliGroupsOverviewBase(CAppDlgDaliWorker& refWorker, UINT nIDTemplate, CWnd* pParent = NULL);   // standard constructor

	virtual void init(int groupNumber) = 0;

	// Parent dialoog (=property dialoog) toestand gewijzigd.
	// Gebruiken om de dialogen up te daten of te melden dat er properties veranderd werdebn die
	// nog moeten doorgestuurd worden.
	virtual void OnParentPropertyPageActive(int groupNumber,bool activeDialog) = 0;		

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
	enum { WM_UPDATE_CONTROLS = (WM_USER + 150),
		   WM_UPDATE_MODEL = (WM_UPDATE_CONTROLS + 1)		// zie BUG-0184
	};

	CAppDlgDaliWorker& getWorker() {
		return m_refWorker;
	}
	
	// De viewmode van de dialoog.
	typedef enum {
		VIEWMODE_OFFLINE = 0,	
		VIEWMODE_ONLINE,
		VIEWMODE_ONLINE_BUSY 			
	} ViewMode_t;

	BOOL updateViewMode( );
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

	void setInitDone(bool flagStateOnOff);
	bool getInitDone();

	void setPageCurrActive(bool flagStateOnOff);
	bool getPageCurrActive( );

	void setBusyGuiControlsUpdate(bool flagStateOnOff);
	bool getBusyGuiControlsUpdate( );

	void setRequestParentUpdateGUI(bool flagStateOnOff);
	bool getRequestParentUpdateGUI( );
	
	void setSelectedGroup(int groupNr);
	int getSelectedGroup( ) const;

private:	
	enum {
		// verandering van een instelling. Kan gebruikt worden om de gebruiker te waarcshuwen bij verlaten dialoog.
		FLAG_PROPERTY_CHANGED				=	(1<<0),		

		// Er werd data uitgeload. Kan gebruikt worden om een melding te geven.
		FLAG_UPLOAD_DONE					=	(1<<1),

		// Bezig met data upload. 
		FLAG_UPLOAD_BUSY					=	(1<<2),

		// Init gedaan (of moet opnieuw uitgevoerd worden). 
		FLAG_INIT_DONE						=	(1<<3),

		// Dialoog/pagina is actief.
		FLAG_PAGE_ACTIVE					=	(1<<4),

		// Bezig met updaten van de GUI, Nodig voor de Edit controls OnChange handlers.
		FLAG_BUSY_CONTROLS_UPDATE			=	(1<<5),

		// Vragen aan de andere property dialogen om up te daten.
		// Dit is nodig wanneer de namen veranderen in de DALI instances overview.
		FLAG_REQUEST_PARENT_UPDATE_GUI		=	(1<<6)
	} Flags;

	CAppDlgDaliWorker& m_refWorker;								// De worker.	
	ViewMode_t m_viewMode;										// De viewmode (zie enum).
	unsigned short m_flags;										// Applicatie vlaggen 								
	int m_selectedGroup;										// De huidige groep die geselecteerd is.
};

