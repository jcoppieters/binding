#if !defined(AFX_MYFORMVIEW_H__0C79AFCF_46C6_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_MYFORMVIEW_H__0C79AFCF_46C6_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyFormView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMyFormView form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

/////////////////////////////////////////////////////////////////////////////
#include "MyTreeNodes.h"
#include "MyTreeBindings.h"
#include "MytreeTemplates.h"
/////////////////////////////////////////////////////////////////////////////
enum FormState
{
	STATE_VIEW_NODES = 0,
	STATE_VIEW_BINDINGS,
	STATE_VIEW_TEMPLATES,
	STATE_VIEW_DISABLED = 0xff
};

/////////////////////////////////////////////////////////////////////////////

class CMyFormView : public CFormView
{
private:
	BOOL fTreeCreated;
	BOOL fTabCreated;
	FormState CurrFormState;

protected:
	CMyFormView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CMyFormView)

	void DisplayFormState(FormState State);

// Form Data
public:
	//{{AFX_DATA(CMyFormView)
	enum { IDD = IDD_MYFORMVIEW_FORM };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


	CMyTreeNodes* GetNodesTree(void) {return (&m_treeNodes); };
	CMyTreeBindings* GetBindingsTree(void) {return (&m_treeBindings); };
	CMyTreeTemplates* GetTemplatesTree(void) {return (&m_treeTemplates); };

	BOOL IsTreeNodesActive(void) {
		if (CurrFormState == STATE_VIEW_NODES) return TRUE;
		else return FALSE;
	}

	BOOL IsTreeBindingsActive(void) {
		if (CurrFormState == STATE_VIEW_BINDINGS) return TRUE;
		else return FALSE;
	}

	BOOL IsTreeTemplatesActive(void) {
		if (CurrFormState == STATE_VIEW_TEMPLATES) return TRUE;
		else return FALSE;
	}

// Attributes
public:
			
	CMyTreeNodes		m_treeNodes;
	CMyTreeBindings		m_treeBindings;
	CMyTreeTemplates	m_treeTemplates;
	CTabCtrl			m_TabCtrl;

	//CFont				*pDefaultfont;

// Operations
public:

	//void CreateTreeView();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyFormView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL
// Implementation
protected:
	virtual ~CMyFormView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CMyFormView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	LONG OnApplicationClearViews(WPARAM wParam,LPARAM lParam);
	LONG OnApplicationUpdateViews(WPARAM wParam,LPARAM lParam);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYFORMVIEW_H__0C79AFCF_46C6_11D8_B865_0050BAC412B1__INCLUDED_)
