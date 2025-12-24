#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CDlgAppBindingsVerify dialog


// *) Zoeken of alle binding bestanden aanwezig zijn.
// *) Zoeken of de binding bestanden compleet zijn.
// *) Controle bindingentries --> BindingFiles.
//		*) Reconversie.

class CDlgAppBindingsVerify : public CDialog
{
	DECLARE_DYNAMIC(CDlgAppBindingsVerify)

	typedef enum
	{
		STATE_INIT = 0,
		STATE_STRINGARRAY_CHECK,
		STATE_STRINGARRAY_SHOW,
		STATE_BINDINGS_VERIFY,
		STATE_BINDINGS_RESTORE,
		STATE_MANUALBINDINGS_SEARCH,
		STATE_SHUTDOWN				

	} State_t;

	typedef enum
	{
		EVENT_START			= 0,
		EVENT_NEXT_STEP,
		EVENT_BTN_YES,
		EVENT_BTN_NO,
		EVENT_BTN_CONTINUE

	} Event_t;

public:
	CDlgAppBindingsVerify(
		CWnd* pParent = NULL,
		const CStringArray* pStringArray = NULL);   // standard constructor

	virtual ~CDlgAppBindingsVerify();
	virtual BOOL OnInitDialog();

// Dialog Data
	enum { IDD = IDD_DIALOG_APP_BINDINGSVERIFY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClickedButtonYes();
	afx_msg void OnBnClickedButtonNo();
	afx_msg void OnBnClickedOk();

	LRESULT OnFsmEvent(WPARAM wParam,LPARAM lParam);
	void PostEvent(Event_t ev);
	void SetState(State_t state);

	static void progressCallback(
		void* pvUserData,
		int step,
		int max);

	void iconShow(LPSTR strIcon);
	void progressbarStart(int pos=0,int max=100);
	void progessbarEnd(int pos=100);
	void buttonsHideAll(void);

private:
	const CStringArray* const m_pStrArrErrorsBindingFiles;
	State_t m_currState;
	BOOL m_fPaintHandler;

	CButton m_btnYes;
	CButton m_btnNo;
	CButton m_btnOk;
	CStatic m_staticInfo;
	CProgressCtrl m_progressInfo;
	CStatic m_staticIcon;

public:
	afx_msg void OnEnterIdle(UINT nWhy, CWnd* pWho);
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnPaint();
	afx_msg void OnClose();
};
