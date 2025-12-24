#ifndef INCLUDED_PROPERTYSHEET_DALI_H
#define INCLUDED_PROPERTYSHEET_DALI_H
/*********************************************************************/
#include "PropertyPageDaliBaseClass.h"
/*********************************************************************/
class CPropertySheetDali;
class CAppDlgDaliWorker;

/*********************************************************************/
/*********************************************************************/
/*********************************************************************/
class CPropertySheetDali_Holder
{
public:
	static CPropertySheetDali* Get();
	static CPropertySheetDali* Create(CWnd* pParent,CAppDlgDaliWorker& refAppDaliWorker);
	static void Destroy();

private:
	static CPropertySheetDali* m_pInstance;
};
/*********************************************************************/


/*********************************************************************/
/*********************************************************************/
/*********************************************************************/
class CPropertySheetDali : public CPropertySheet, public DUOTECNO::DALI::IAppDlgDaliGui_Host	
{
	DECLARE_DYNAMIC(CPropertySheetDali)	

// Construction
public:
	CPropertySheetDali(LPCTSTR pszCaption, CWnd* pParentWnd, CAppDlgDaliWorker& refWorker, UINT iSelectPage = 0);

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertyNodesSheet)
	public:
	virtual BOOL OnInitDialog();
	protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

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

// Implementation
public:
	virtual ~CPropertySheetDali();	
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	// Generated message map functions
protected:
	//{{AFX_MSG(CPropertyNodesSheet)
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT nIDEvent);
	LONG onPropertySheetDaliDialogErrorsOpen(WPARAM wParam,LPARAM lParam);
	LONG onPropertySheetDaliDialogErrorsClose(WPARAM wParam,LPARAM lParam);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
	void DestroyPropertySheet(void);
	void initContext( );
	void createPages( );
	void initTabCtrlToolTips( );
	BOOL isCloseDialog( );
	BOOL promptUserChangesOpenConfiguratorCloseDialog();		// TRUE = Sluiten van de dialoog, veranderingen negeren.
	BOOL promptUserCfgTaskBusyAskCloseDialog();					// TRUE = Sluiten van de dialoog

	// Child dialoog met fout weergave...
	void openDialogDaliErrors( );

private:
	enum { 
		MAX_PAGES = 5,
	};	
	enum {
		TIMER_ID_VISIBILITY_CHECK = 1
	};

	CPropertyPageDaliContext* const m_pContext;
	CAppDlgDaliWorker& m_refWorker;
	CPropertyPageDaliBaseClass* m_pPages[MAX_PAGES];
	CButton m_Button;	

	CToolTipCtrl m_ToolTip;		
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYNODESSHEET_H__910595C8_A980_11D8_B865_0050BAC412B1__INCLUDED_)
