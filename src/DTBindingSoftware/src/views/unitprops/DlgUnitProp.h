#if !defined(AFX_DLGUNITPROP_H__1DEEAF47_6DCB_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_DLGUNITPROP_H__1DEEAF47_6DCB_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgUnitProp.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgUnitProp dialog
#include "NodeDatabase.h"
#include "DlgDimmerChildProp.h"		// <-- ??
#include "uicontrols/controls/UnitButton.h"

class CDlgUnitChildProp;

///////////////// DIALOG FRAME HEADER OFFSET //////////////
class CDlgUnitProp : public CDialog
{
protected:
	static CDlgUnitProp *m_pDlgUnitProp;

private:
	static STRUCT_UNIT *m_pDocUnit;
	static const CCanNode* m_pDocNode;
	
public:
	// Sedert versie 0x0900: Ook de referentie naar de node meegeven.
	static BOOL Show(const CCanNode* pNode,STRUCT_UNIT *pUnit,const CString& strLabelText);
	static BOOL Destroy(void);
	static BOOL OnUnitStatusChanged(BYTE bNodeAddress,BYTE bUnitAddress);
	
public:
	void Subscribe(CDlgUnitChildProp *pChild);
	void unSubscribe(void);

protected:
	virtual void InitControls(void) = 0;

	BYTE GetTypeOfDialog() {return m_bTypeOfDialog; }
	void DeleteControls(void);
	void NotifyChild(void);

#if(0)
	void NormalizeWindowRect(CRect* rect);		// Ondersteuning van meerdere monitors. (Niet meer nodig sedert V17.02)
#endif 

// Construction
public:
	CDlgUnitProp(CWnd* pParent = NULL);   // standard constructor	
		
// Dialog Data
	//{{AFX_DATA(CDlgUnitProp)
	enum { IDD = IDD_DIALOG_PROP_UNIT };
	CButton	m_BtnReturn;
	CButton	m_BtnExpand;
	CUnitButton	m_BtnControl;
	CString	m_szAddress;
	CString	m_szUnitName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgUnitProp)
	public:
	virtual void OnSetFont(CFont* pFont);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation

private:
	BYTE m_bTypeOfDialog;
	CDlgUnitChildProp *pChildDialog;

protected:
	void InitControls(const CString& strLabelText);
	void InitStaticControls(const CString& strLabelText);

	// Oppassen: Deze functie mag maar 1 aangeroepen worden.
	// Dialogen gebruiken fChildCreated om dit te controleren.
	// Betere oplossing: zie CDlgAVMatrixProp
	// Voor het berekenen een controle doen of getRectChildFrame() == CRect(0,0,0,0) 
	void CalculateDialogCoordinates(void);	

	virtual void ShowFullDialog(BOOL fShow);	
	virtual void CleanUp(void) = 0;
	virtual void RequestRuntimeProperties(void) = 0;
	virtual void UpdateRuntimeProperties(void);
	const STRUCT_UNIT* getAssignedUnit( ) const;
	STRUCT_UNIT* getAssignedUnit( );
	const CCanNode* getAssignedNode( ) const;

	CPoint getPointTopLeft();

	// Child frame rect
	CRect getRectChildFrame( ) const { 
		return m_rcChildFrame; 
	};
	void setRectChildFrame(const CRect& rect) {
		m_rcChildFrame = rect;
	}

	// Main frame rect
	CRect getRectMainFrame( ) const { 
		return m_rcMainFrame; 
	};
	void setRectMainFrame(const CRect& rect) {
		m_rcMainFrame = rect;
	}

	// Full frame rect
	CRect getRectFullFrame( ) const { 
		return m_rcFullFrame; 
	};
	void setRectFullFrame(const CRect& rect) {
		m_rcFullFrame = rect;
	}

	// Generated message map functions
	//{{AFX_MSG(CDlgUnitProp)
	afx_msg void OnButtonReturn();
	afx_msg void OnButtonExpand();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CFont *pDefaultFont;					// TODO: DEZE PRIVATE ZETTEN
	BOOL fShowBindingProperties;			// TODO: DEZE PRIVATE ZETTEN
	STRUCT_UNIT *pRuntimeUnit;				// TODO: DEZE PRIVATE ZETTEN
	BOOL fChildCreated;						// TODO: DEZE PRIVATE ZETTEN
	BOOL fStatusReceived;					// TODO: DEZE PRIVATE ZETTEN

private:
	CRect m_rcChildFrame;					// Rectangle subdialoog.
	CRect m_rcMainFrame;					// Rectangle hoofddialoog.
	CRect m_rcFullFrame;					// Rectangle volledige dialoog.
	CStatic m_staticUnitInfoLabel;
};


inline  const STRUCT_UNIT* CDlgUnitProp::getAssignedUnit( ) const {
	return m_pDocUnit;
}

inline STRUCT_UNIT* CDlgUnitProp::getAssignedUnit( ) {
	return m_pDocUnit;
}

inline const CCanNode* CDlgUnitProp::getAssignedNode( ) const {
	return m_pDocNode;
}


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGUNITPROP_H__1DEEAF47_6DCB_11D8_B865_0050BAC412B1__INCLUDED_)
