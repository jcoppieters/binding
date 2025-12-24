#if !defined(AFX_DLGDIMMERCHILDPROP_H__1DEEAF41_6DCB_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_DLGDIMMERCHILDPROP_H__1DEEAF41_6DCB_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgDimmerChildProp.h : header file
//

#include "DlgUnitChildProp.h"
//#include "MyEditCtrl.h"			//READ ONLY EDIT BOX !

#include "uicontrols/controls/MyEditCtrl.h"

#include "App\Appbindings\appbindingproperty\BindingPropertyUnit.h"


///////////////////////////////////////////////////////
#define DIMMER_TOPLEFT_CTRL_OFFSET_X		10
#define DIMMER_TOPLEFT_CTRL_OFFSET_Y		40	

#define DIMMER_STATIC_X_SIZE				75
#define DIMMER_STATIC_Y_SIZE				15

#define DIMMER_EDIT_BUDDY_X_SIZE			40
#define DIMMER_EDIT_BUDDY_Y_SIZE			20

#define DIMMER_CHECK_BUTTON_X_SIZE			(100)	
#define DIMMER_CHECK_BUTTON_Y_SIZE			(20)	
#define DIMMER_CHECK_BUTTON_Y_SPACE			(25)

#define DIMMER_TOPLEFT_SPEEDUP_X			(DIMMER_TOPLEFT_CTRL_OFFSET_X+0)	// 10
#define DIMMER_TOPLEFT_SPEEDUP_Y			(DIMMER_TOPLEFT_CTRL_OFFSET_Y+0)	// 20
#define DIMMER_TOPLEFT_SPEEDDOWN_X			(DIMMER_TOPLEFT_CTRL_OFFSET_X+80)	// 90
#define DIMMER_TOPLEFT_SPEEDDOWN_Y			(DIMMER_TOPLEFT_CTRL_OFFSET_Y+0)	// 20

#define DIMMER_TOPLEFT_MINRANGE_X			(DIMMER_TOPLEFT_CTRL_OFFSET_X+0)	// 10
#define DIMMER_TOPLEFT_MINRANGE_Y			(DIMMER_TOPLEFT_CTRL_OFFSET_Y+45)	// 65
#define DIMMER_TOPLEFT_MAXRANGE_X			(DIMMER_TOPLEFT_CTRL_OFFSET_X+80)	// 90
#define DIMMER_TOPLEFT_MAXRANGE_Y			(DIMMER_TOPLEFT_CTRL_OFFSET_Y+45)	// 65

#define DIMMER_TOPLEFT_SOLIDSTATE_ON_X		(DIMMER_TOPLEFT_CTRL_OFFSET_X+165)	// 175
#define DIMMER_TOPLEFT_SOLIDSTATE_ON_Y		(DIMMER_TOPLEFT_CTRL_OFFSET_Y+0)	// 20

#define DIMMER_TOPLEFT_ADDRESSCFG_X			(DIMMER_TOPLEFT_CTRL_OFFSET_X+0)
#define DIMMER_TOPLEFT_ADDRESSCFG_Y			(0)

#define DIMMER_SIZE_ADDRESSCFG_X_SIZE		(140)
#define DIMMER_SIZE_ADDRESSCFG_Y_SIZE		(20)

#define DIMMER_TOPLEFT_CMB_ADDRESSCFG_X		(DIMMER_TOPLEFT_CTRL_OFFSET_X+165)
#define DIMMER_TOPLEFT_CMB_ADDRESSCFG_Y		(0)

#define DIMMER_SIZE_ADDRESSCFG_CMB_X_SIZE	(130)
#define DIMMER_SIZE_ADDRESSCFG_CMB_Y_SIZE	(80)


/////////////////////////////////////////////////////////////////////////////
// CDlgDimmerChildProp dialog

class CDlgDimmerChildProp : public CDlgUnitChildProp
{
// Construction
public:
	CDlgDimmerChildProp(CWnd* pParent = NULL);   // standard constructor

	virtual void OnNotifyClose(void);
	void InitControls(		
		CBindingPropertyUnit *ptr);

	//void ShowWindow(BOOL fShow);

// Dialog Data
	//{{AFX_DATA(CDlgDimmerChildProp)	
	enum { IDD = IDD_DIALOG_PROP_UNIT_CHILD };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgDimmerChildProp)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//virtual void PostNcDestroy();
	//}}AFX_VIRTUAL
	
// Implementation
protected:

	typedef enum
	{
		MODE_NORMAL			= 0,
		MODE_DALI_GROUP,
		MODE_DALI_DEVICE

	} Mode_t;

	void CreateCheckBtn(CButton *pBtn,UINT ButtonIDD,CString szText,CPoint TopLeft);	
	void CreateSpin(CEdit *pEdit, UINT EditIDD, CSpinButtonCtrl *pSpin, UINT SpinIDD, CStatic *pStatic, CPoint TopLeft, CString szText);

	virtual void InitStaticControls(void);
	virtual void InitControlProperties(void);
	virtual void RestoreDefaults(void);

	void InitControlsToDefaults(void);
	void UpdateControls(void);

	// Sedert versie 0x0B00
	void controlsAddrCfgCreate(void);
	void controlsAddrCfgInit(void);
	void controlsAddrCfgUpdate(Mode_t mode);

	Mode_t getAddressCfgMode( ) const;

	afx_msg void OnCheckPirOnDelayed(void);
	afx_msg void OnCheckPirOffDelayed(void);
	afx_msg void OnCheckSolidState(void);
	afx_msg void OnSpinSettingsChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelectAddressCfg(void);

	// Generated message map functions
	//{{AFX_MSG(CDlgDimmerChildProp)
	//virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	
	CBindingPropertyDimUnit m_bindingDimUnit;		//Local ...
	CBindingPropertyDimUnit *ptrOriginalDimUnit;

	// Sedert versie 0x0B00: 
	// Vlag die bepaalt dat enkel de properties voor de address cfg aangepast werden
	BOOL m_fChangesAddressCfg;

	// Controls
	CStatic m_StaticSpeedUp,m_StaticSpeedDown;
	CMyEditCtrl m_EditBuddySpeedUp,m_EditBuddySpeedDown;
	CSpinButtonCtrl m_SpinSpeedUp,m_SpinSpeedDown;

	CStatic m_StaticMinRange,m_StaticMaxRange;
	CMyEditCtrl m_EditBuddyMinRange,m_EditBuddyMaxRange;
	CSpinButtonCtrl m_SpinMinRange,m_SpinMaxRange;

	CButton m_CheckBtnPirOn,m_CheckBtnPirOff,m_CheckBtnSolidState;

	// Sedert versie 0x0B00
	CStatic m_StaticAddrCfgType;
	CComboBox m_CmbAddrCfgType;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGDIMMERCHILDPROP_H__1DEEAF41_6DCB_11D8_B865_0050BAC412B1__INCLUDED_)
