#ifndef INCLUDED_PROPERTYSHEET_DALI_ERRORS_H
#define INCLUDED_PROPERTYSHEET_DALI_ERRORS_H
/*********************************************************************/
#include "PropertyPageDaliErrorsBaseClass.h"
/*********************************************************************/
class CPropertySheetDaliErrors;
class CAppDlgDaliWorker;

/*********************************************************************/
/*********************************************************************/
/*********************************************************************/
class CPropertySheetDaliErrors_Holder
{
public:
	static CPropertySheetDaliErrors* Get();
	static CPropertySheetDaliErrors* Create(CWnd* pParent,CAppDlgDaliWorker& refAppDaliWorker);
	static void Destroy();

private:
	static CPropertySheetDaliErrors* m_pInstance;
};
/*********************************************************************/


/*********************************************************************/
/*********************************************************************/
/*********************************************************************/
class CPropertySheetDaliErrors : public CPropertySheet // , public DUOTECNO::DALI::IAppDlgDaliGui_Host	
{
	DECLARE_DYNAMIC(CPropertySheetDaliErrors)	

// Construction
public:
	CPropertySheetDaliErrors(LPCTSTR pszCaption, CWnd* pParentWnd, CAppDlgDaliWorker& refWorker, UINT iSelectPage = 0);

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

// Implementation
public:
	virtual ~CPropertySheetDaliErrors();	


	// Generated message map functions
protected:
	//{{AFX_MSG(CPropertyNodesSheet)
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
	void DestroyPropertySheet(void);
	void initContext( );
	void createPages( );
	// void deleteModel();
	void initModel(CAppDlgDaliWorker& refWorker);

private:
	enum { MAX_PAGES = 2 };
	
	CAppDlgDaliWorker& m_refWorker;
	CPropertyPageDaliErrorsContext* m_pContext;
	CPropertyPageDaliErrorsBaseClass* m_pPages[MAX_PAGES];
	CButton m_Button;	
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYNODESSHEET_H__910595C8_A980_11D8_B865_0050BAC412B1__INCLUDED_)
