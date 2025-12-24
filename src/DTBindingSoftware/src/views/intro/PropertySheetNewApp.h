#pragma once

#include "PropertySheetNewAppContext.h"

class CPropertyPageNewApp;

// CPropertySheetNewApp
// Sedert V16.26:
// Property pages ingevoegd voor zoeken en configureren van TCP/IP devices in het LAN-netwerk.
class CPropertySheetNewApp : public CPropertySheet
{
	DECLARE_DYNAMIC(CPropertySheetNewApp)

public:	
	CPropertySheetNewApp(LPCTSTR pszCaption, CWnd* pParentWnd, PropertyPageNewAppMode_t mode, IAppDlgStartDialog_Host& refHost, UINT iSelectPage = 0);
	virtual ~CPropertySheetNewApp();

	static CPropertySheetNewApp* get();
	static CPropertySheetNewApp* create(CWnd* pParent,PropertyPageNewAppMode_t mode,IAppDlgStartDialog_Host& refHost);
	static void destroy();

	void setPage(int pageNR);
	virtual BOOL OnInitDialog();
	virtual INT_PTR DoModal();
	const PropertyPageNewAppMode_t getDlgMode() const;

protected:
	DECLARE_MESSAGE_MAP()
	LONG OnCommunicationModemStatus(WPARAM wParam,LPARAM lParam);
	void createPages(CPropertyPageNewAppContext& ref);

	void initContext();

	// Updaten van de applicatie toestand: offline,online, new installation, ...
	void updateAppState(IAppDlgStartDialog_Host& refHost,CPropertyPageNewAppContext& refCtx);

	// Updaten van de login: offline,usb,tcp + serverinfo...
	void updateLoginInfo(IAppDlgStartDialog_Host& refHost,CPropertyPageNewAppContext& refCtx);	

private:
	enum { MAX_PAGES = 8 };		// Sedert V16.26	
	static CPropertySheetNewApp* m_pInstance;
	CPropertyPageNewAppContext* m_pContext;
	CPropertyPageNewApp* m_pages[MAX_PAGES];
};


