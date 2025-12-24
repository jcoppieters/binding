#pragma once
//#include "afxcmn.h"


class CConfigIRTXFile;			// Voorwaartse declaratie.
class CConfigIRTXFileEntry;		// Voorwaartse declaratie.

// CDlgViewIRTXCodes dialog

class CDlgViewIRTXCodes : public CDialog
{
	//enum { WM_UPDATE_CONTROLS	=  (WM_USER + 150) };

	#define WM_UPDATE_CONTROLS	(WM_USER + 150)

	enum
	{
		HDR_LISTCTRL_INDEX		= 0,
		HDR_LISTCTRL_DEVICE		= 1,
		HDR_LISTCTRL_FUNCTION   = 2,
		HDR_LISTCTRL_MAXITEMS
	};

	DECLARE_DYNAMIC(CDlgViewIRTXCodes)

public:
	CDlgViewIRTXCodes(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgViewIRTXCodes();

	virtual BOOL OnInitDialog();
	static CDlgViewIRTXCodes* getInstance( );
	static bool isActive( );
	static BOOL Destroy(void);
	void Show(void);
	void attach(CConfigIRTXFile* pCfg);
	void Update( );

// Dialog Data
	enum { IDD = IDD_DIALOG_VIEW_IRTX };

public:
	afx_msg void OnBnClickedOk();
	void OnNotifyHdrItem(NMHDR* pNMHDR, LRESULT* pResult);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	afx_msg void OnClose();		// BUG-0139
	void Add(CListCtrl* p,int index,CConfigIRTXFileEntry* pEntry);
	BOOL Create();
	static int CALLBACK SortFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

	DECLARE_MESSAGE_MAP()

private:
	static CDlgViewIRTXCodes* m_pInstance;

	CConfigIRTXFile* m_pConfigFile;
	BOOL m_fSortMethod[HDR_LISTCTRL_MAXITEMS];
	CListCtrl m_listCtrl;
	int m_nID;
	void InitStaticControls(void);
	LONG OnUpdateControls(WPARAM wParam,LPARAM lParam);
};
