#pragma once
#include "afxcmn.h"

/******************************************************************************/
// CDlgUDPLanDevices dialog
class AppUDPServerCfg;
class AppUDPDevice;

namespace DUOTECNO {
	class CDlgHelperFont;
};

#define WM_UPDATE_CONTROLS	(WM_USER + 150)

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
class CDlgUDPLanDevices : public CDialog
{
	DECLARE_DYNAMIC(CDlgUDPLanDevices)

public:

	static BOOL Show(CWnd* pWndParent,AppUDPServerCfg& refWorker);
	static CDlgUDPLanDevices* getInstance();	
	static BOOL IsActive();	

	CDlgUDPLanDevices(CWnd* pParent,AppUDPServerCfg& refWorker);   // standard constructor
	virtual ~CDlgUDPLanDevices();

// Dialog Data
	enum { IDD = IDD_DIALOG_UDP_LANDEVICES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnBnClickedOk();	
	afx_msg void OnContextmenuUdplancfg();
	afx_msg void OnLvnColumnclickListdevices(NMHDR *pNMHDR, LRESULT *pResult);
	void OnContextMenu(CWnd* pWnd, CPoint point);

	DECLARE_MESSAGE_MAP()
	BOOL Create();
	LRESULT OnUpdateControls(WPARAM wParam,LPARAM lParam);

protected:

	enum {
		COLUMN_DEVICE_NAME = 0,
		COLUMN_DEVICE_IPADDRESS = 1,
		COLUMN_DEVICE_NETMASK = 2,
		COLUMN_DEVICE_GATEWAY = 3,
		COLUMN_DEVICE_DHCP = 4,
		COLUMN_DEVICE_MAC = 5,
		COLUMN_DEVICE_FOUND = 6,
	};

	// Kolom toevoegen aan list control.
	int listCtrlInitAddColumn(CListCtrl* pCtrl,int columnNR,const char* szName,int sizeX);

	// zoeken naar een device in de list ctrl ifv het MAC-Address.
	// return waarde: -1 = niet gevonden, anders de index van het item.
	int listCtrlFindDevice(CListCtrl* pList,const AppUDPDevice& refDevice);

	// Een device toevoegen aan de listCtrl.
	void listCtrlAddDevice(CListCtrl* pList,const AppUDPDevice& refDevice);

	// Een device updaten in de listCtrl.
	// itemIndex bepaalt de index van het item (zie listCtrlFindDevice)
	void listCtrlUpdateDevice(CListCtrl* pList,int itemIndex,const AppUDPDevice& refDevice);

	// helper functie.
	// Item in listCtrl editeren.
	// functie wordt gebruikt bij het toevoegen / updaten van een item.
	void listCtrlHelper_setDevice(CListCtrl* pList,int nItem,const AppUDPDevice& refDevice);

	// Ophalen van het device die momenteel geselecteerd is.
	bool listCtrlFindSelectedDevice(CListCtrl& refCtrl,const AppUDPDevice** ppDevice);

	// zoeken in de lijst van devices naar een device ifv de itemData van een listctrl.
	// de itemData is een ingekorte versie van het MAC-address.
	bool findDeviceByListCtrlItemData(DWORD dwItemData,const AppUDPDevice** ppDevice);

	// Sort functie van de listControl.
	static int CALLBACK CDlgUDPLanDevices::SortFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

private:
	static CDlgUDPLanDevices* m_pInstance;		// singleton
	AppUDPServerCfg& m_refWorker;				// ref naar de AppUDPServerCfg. Deze bevat de lijst van devices.
	DUOTECNO::CDlgHelperFont* m_pDlgFontHelper;	// font
	CListCtrl m_listDevices;					// ListCtrl waar de devices getoond worden.
	BOOL m_fAscendingOrder;						// sorteer volgorde.
	int m_sortColumnIdx;						// sorteren per kolom
};
