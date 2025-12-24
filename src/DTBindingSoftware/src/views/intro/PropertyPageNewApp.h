#pragma once

/*****************************************************/
#include "PropertySheetNewAppContext.h"
#include "afxwin.h"
/*****************************************************/
class CPropertyPageNewAppContextUdpDevice;

namespace DUOTECNO {
	class CDlgHelperFont;
};

/*****************************************************/
class CPropertyPageNewApp : public CPropertyPage
{
public:
	CPropertyPageNewApp(CPropertySheet* parent,unsigned short nID,CPropertyPageNewAppContext& refCtx);		
	~CPropertyPageNewApp(void);

	virtual BOOL OnInitDialog();
	virtual void UpdateCommunicationStatus() = 0;
	virtual void StoreData( ) = 0;

protected:
	enum { WM_UPDATE_CONTROLS = (WM_USER + 150) };

	virtual BOOL OnSetActive();			// Wanneer de pagina aktief komt.
	virtual BOOL OnKillActive();		// Wanneer de pagina verlaten wordt.

	virtual BOOL OnWizardFinish() {
		StoreData();
		return CPropertyPage::OnWizardFinish();
	}
	virtual LRESULT OnWizardNext() {
		StoreData();
		return CPropertyPage::OnWizardNext();
	}
	virtual LRESULT OnWizardBack() {
		StoreData();
		return CPropertyPage::OnWizardBack();
	}
	
	// Sedert V16.26
	// Deze kan gebruikt worden in de functies: OnWizardNext() en OnWizardBack.
	// Door een resource id als returnwaarde te geven wordt niet naar de standaard 
	// pagina gebruikt maar wordt deze gebruikt van de resource ID.
	typedef enum {
		DIALOG_START_PROJECTTYPE = 0,
		DIALOG_SELECT_COMMUNICATION,
		DIALOG_TCIP_PROPERTIES,
		DIALOG_SEARCH_DEVICES,
		DIALOG_CONFIG_CANBUS_DEVICE
	} DialogPage_t;

	int getPageResourceID(DialogPage_t page);

	// Sedert V16.26
	// Bepalen of we de active pagina zijn.
	BOOL isActivePage();

protected:
	IAppDlgStartDialog_Host& getHost();

	CPropertyPageNewAppContext& getCtx() {
		return m_refCtx;
	}

	// helper functies.
	void showWindow(CWnd* pDialog,int id,BOOL fState);
	void enableWindow(CWnd* pDialog,int id,BOOL fState);
	void setWindowText(CWnd* pDialog,int id,const CString& text);

	// zie NOTE_PROPERTYPAGE_WIZARDBUTTONS
	void setWizardButtons(DWORD dwButtons);

	// Deze moet aangeroepen worden na het inladen van de JsonLoggin settings...
	void updateContext( );
	void clearContext( );	// Context opnieuw wissen ???

private:
	DWORD m_dwWizardButtons;						// Sedert V16.26: zie NOTE_PROPERTYPAGE_WIZARDBUTTONS
	CPropertySheet* const m_parent;
	const int m_dialogID;
	DUOTECNO::CDlgHelperFont* m_pDlgFontHelper;
	CPropertyPageNewAppContext& m_refCtx;
};
/*****************************************************/



