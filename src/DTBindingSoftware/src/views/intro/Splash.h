// CG: This file was added by the Splash Screen component.

#ifndef _SPLASH_SCRN_
#define _SPLASH_SCRN_

// Splash.h : header file
//

/////////////////////////////////////////////////////////////////////////////
//   Splash Screen class


// Sedert V16.23:
// Splash screen window wordt niet meer via een timer verwijderd maar wordt verwijderd via
// de CMainFrame klasse.

class CSplashWnd : public CWnd
{
// Construction
protected:
	CSplashWnd();

// Attributes:
public:
	CBitmap m_bitmap;

// Operations
public:
	static void EnableSplashScreen(BOOL bEnable = TRUE);
	static void ShowSplashScreen(CWnd* pParentWnd = NULL);
	static BOOL PreTranslateAppMessage(MSG* pMsg);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSplashWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	~CSplashWnd();
	virtual void PostNcDestroy();

	static BOOL IsVisible() {
		return (c_pSplashWnd != NULL);
	}
	static void Close( );

protected:
	BOOL Create(CWnd* pParentWnd = NULL);
	void HideSplashScreen();	


// Generated message map functions
protected:
	//{{AFX_MSG(CSplashWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	// afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


private:
	static BOOL c_bShowSplashWnd;
	static CSplashWnd* c_pSplashWnd;
	static const CRect rcTextRect;
	CString szSoftwareVersion;
};

#endif
