#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// DlgAVMatricChildProp.h : header file
//

#include "DlgUnitChildProp.h"
#include "App\Appbindings\appbindingproperty\BindingPropertyUnit.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgAvMatrixChildProp dialog

class CDlgAvMatrixChildProp : public CDlgUnitChildProp
{
// Construction
public:
	CDlgAvMatrixChildProp(CWnd* pParent = NULL);   // standard constructor

	virtual void OnNotifyClose(void);
	void InitControls(CBindingPropertyUnit *ptr);

// Dialog Data
	//{{AFX_DATA(CDlgAvMatrixChildProp)
	enum { IDD = IDD_DIALOG_PROP_UNIT_CHILD };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgAvMatrixChildProp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgAvMatrixChildProp)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()	

	afx_msg void OnAVMatrixConfigType(void); 
	
	virtual void InitStaticControls(void);
	virtual void InitControlProperties(void);
	virtual void RestoreDefaults(void);
	void InitControlsToDefaults(void);

	// Virtuele functies voor de afgeleide dialogen.
	virtual void displayAVMatrixType( ) = 0;
	virtual BOOL updateAVMatrixSubType(CBindingPropertyAVMatrixUnit* p,CComboBox* pComboBox,BOOL fUpdateUI = FALSE) = 0;
	virtual void initAVMatrixSubTypeComboBox(CComboBox* pComboBox) = 0;
	virtual void UpdateControls(void);

	void setNameAVMatrixType(const CString& name);

	// Tonen van de static die het type matrix weergeeft: Family: Audac
	void showControlsMatrixType(BOOL flag);
	
	// Tonen van de static & combo om subtype in te stellen.
	void showControlsSubType(BOOL flag);
	

private:
	CBindingPropertyAVMatrixUnit m_bindingAVMatrixUnit;
	CBindingPropertyAVMatrixUnit* m_ptrOriginalAVMatrixUnit;
	CStatic m_StaticType;	
	CStatic m_StaticSubType;	// Text label voor de comobox
	CComboBox m_CmbSubType;		// de combobox.
};

////////////////////////////////////////////////////////////////////////
// CDlgAVMatrixChildPropAudac
class CDlgAVMatrixChildPropAudac : public CDlgAvMatrixChildProp
{
public:
	CDlgAVMatrixChildPropAudac(CWnd* pParent = NULL);   // standard constructor

protected:
	virtual void displayAVMatrixType( );
	virtual BOOL updateAVMatrixSubType(CBindingPropertyAVMatrixUnit* p,CComboBox* pComboBox,BOOL fUpdateUI = FALSE);
	virtual void initAVMatrixSubTypeComboBox(CComboBox* pComboBox);
	virtual void UpdateControls(void);
};

////////////////////////////////////////////////////////////////////////
// CDlgAVMatrixChildPropIntegra
class CDlgAVMatrixChildPropIntegra : public CDlgAvMatrixChildProp
{
public:
	CDlgAVMatrixChildPropIntegra(CWnd* pParent = NULL);   // standard constructor

protected:
	virtual void displayAVMatrixType( );
	virtual BOOL updateAVMatrixSubType(CBindingPropertyAVMatrixUnit* p,CComboBox* pComboBox,BOOL fUpdateUI = FALSE);
	virtual void initAVMatrixSubTypeComboBox(CComboBox* pComboBox);
	virtual void UpdateControls(void);
};

/////////////////////////////////////////////////////////////////////////////
