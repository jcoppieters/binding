#if !defined(AFX_DLGSENSPROPCHILD_H__5CE194E2_71B0_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_DLGSENSPROPCHILD_H__5CE194E2_71B0_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSensPropChild.h : header file
//

#include "DlgUnitChildProp.h"
#include "uicontrols/controls/MyEditCtrl.h"
#include "App\Appbindings\appbindingproperty\BindingPropertyUnit.h"

class CCanNode;		// voorwaartse declrarue

/////////////////////////////////////////////////////////////////////////////
// CDlgSensPropChild dialog
/////////////////////// SENS PROP CHILD DIALOGS //////////////////////

#define SENS_TOPLEFT_CTRL_OFFSET_Y				30

#define SENS_TOPLEFT_GROUP_HYSHIGH_X			0
#define SENS_TOPLEFT_GROUP_HYSHIGH_Y			(0+SENS_TOPLEFT_CTRL_OFFSET_Y)
#define SENS_TOPLEFT_GROUP_HYSLOW_X				140
#define SENS_TOPLEFT_GROUP_HYSLOW_Y				(0+SENS_TOPLEFT_CTRL_OFFSET_Y)

#define SENS_GROUP_SIZE_X						135
#define SENS_GROUP_HYS_SIZE_Y					95

#define SENS_TOPLEFT_HYSVAL_HIGH_X				10
#define SENS_TOPLEFT_HYSVAL_HIGH_Y				(30+SENS_TOPLEFT_CTRL_OFFSET_Y)
#define SENS_TOPLEFT_HYSVAL_LOW_X				150
#define SENS_TOPLEFT_HYSVAL_LOW_Y				(30+SENS_TOPLEFT_CTRL_OFFSET_Y)
#define SENS_OFFSET_HYSVAL_X					60
#define SENS_OFFSET_HYSVAL_Y					37

#define SENS_GROUP_HYSOFF_SIZE_X				135
#define SENS_GROUP_HYSOFF_SIZE_Y				60
#define SENS_TOPLEFT_GROUP_HYSOFF_X				0
#define SENS_TOPLEFT_GROUP_HYSOFF_Y				(100+SENS_TOPLEFT_CTRL_OFFSET_Y)
#define SENS_TOPLEFT_HYSOFF_HIGH_X				10
#define SENS_TOPLEFT_HYSOFF_HIGH_Y				(130+SENS_TOPLEFT_CTRL_OFFSET_Y)

#define SENS_TOPLEFT_SAMPLE_X					150
#define SENS_TOPLEFT_SAMPLE_Y					(175 +SENS_TOPLEFT_CTRL_OFFSET_Y)

#define SENS_TOPLEFT_COMBO_X					0
#define SENS_TOPLEFT_COMBO_Y					(170+SENS_TOPLEFT_CTRL_OFFSET_Y)
#define SENS_COMBO_SIZE_X						100
#define SENS_COMBO_SIZE_Y						15

#define SENS_TOPLEFT_COMBO_SENSPROFILE_X		150

#define SENS_STATIC_X_SIZE						75
#define SENS_STATIC_Y_SIZE						15
#define SENS_EDIT_BUDDY_X_SIZE					45
#define SENS_EDIT_BUDDY_Y_SIZE					20

#define SENS_TOPLEFT_OFFSET_X					10
#define SENS_TOPLEFT_OFFSET_Y					(175+SENS_TOPLEFT_CTRL_OFFSET_Y)

#define SENS_CHECK_BUTTON_X_SIZE				120 
#define SENS_CHECK_BUTTON_Y_SIZE				20

#define SENS_GROUP_SP_RANGE_SIZE_Y				60
#define SENS_TOPLEFT_GROUP_SP_RANGE_X			140
#define SENS_TOPLEFT_GROUP_SP_RANGE_Y			(100+SENS_TOPLEFT_CTRL_OFFSET_Y)
#define SENS_TOPLEFT_SP_RANGE_MIN_X				SENS_TOPLEFT_HYSVAL_LOW_X
#define SENS_TOPLEFT_SP_RANGE_MIN_Y				(130+SENS_TOPLEFT_CTRL_OFFSET_Y)
#define SENS_TOPLEFT_SP_RANGE_MAX_X				SENS_TOPLEFT_HYSVAL_LOW_X + 60
#define SENS_TOPLEFT_SP_RANGE_MAX_Y				(130+SENS_TOPLEFT_CTRL_OFFSET_Y)

// PID controls.
#define SENS_GROUP_PID_SIZE_Y					115
#define SENS_TOPLEFT_GROUP_PID_Y				(205+SENS_TOPLEFT_CTRL_OFFSET_Y)


class CDlgSensPropChild : public CDlgUnitChildProp
{
protected:

	#define WM_UPDATE_CONTROLS	(WM_USER + 150)

	enum
	{
		SENS_PROFILE_CUSTOM		= 0,
		SENS_PROFILE_HEATING	= 1,
		SENS_PROFILE_COOLING	= 2,
		SENS_PROFILE_BOTH		= 3	
	};

// Construction
public:

	CDlgSensPropChild(
		CWnd* pParent = NULL);   // standard constructor

	virtual void OnNotifyClose(void);
	void InitControls(CBindingPropertyUnit *ptr);

// Dialog Data
	//{{AFX_DATA(CDlgSensPropChild)
	//enum { IDD = _UNKNOWN_RESOURCE_ID_ };
	enum { IDD = IDD_DIALOG_PROP_UNIT_CHILD };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSensPropChild)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual void InitStaticControls(void);
	virtual void InitControlProperties(void);
	virtual void RestoreDefaults(void);

	void UpdateControls(void);
	void InitControlsToDefaults(void);

	// Generated message map functions
	//{{AFX_MSG(CDlgSensPropChild)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	afx_msg void OnUpDownHysValueHighSun(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUpDownHysValueHighHSun(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUpDownHysValueHighMoon(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUpDownHysValueHighHMoon(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUpDownHysValueLowSun(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUpDownHysValueLowHSun(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUpDownHysValueLowMoon(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUpDownHysValueLowHMoon(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUpDownHysOffsetHigh(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUpDownHysOffsetLow(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUpdownSampleInterval(NMHDR* pNMHDR, LRESULT* pResult);      
	afx_msg void OnUpdownMeasuredval(NMHDR* pNMHDR, LRESULT* pResult);   
	afx_msg void OnUpdownSetpointRangeMin(NMHDR* pNMHDR, LRESULT* pResult);   
	afx_msg void OnUpdownSetpointRangeMax(NMHDR* pNMHDR, LRESULT* pResult);  
	afx_msg void OnButtonResetValues(void);
	afx_msg void OnSelectSensProfile();
	afx_msg void OnCheckExternalSensor(void);
	afx_msg void OnSelectDelegate(void);
	afx_msg void OnSelectPidCoolingUnit(void);
	afx_msg void OnSelectPidHeatingUnit(void);
	afx_msg void OnUpDownPidParam(NMHDR* pNMHDR, LRESULT* pResult);


	LONG OnUpdateControls(WPARAM wParam,LPARAM lParam);

	// SpinControl wanneer de text erboven staat.
	void _CreateSpin(CEdit *pEdit, UINT EditIDD, CSpinButtonCtrl *pSpin, 
					UINT SpinIDD, CStatic *pStatic, CPoint TopLeft, CString szText);
	// SpinControl wanneer de test ernaast staat.
	void _CreateSpin2(CEdit *pEdit, UINT EditIDD, CSpinButtonCtrl *pSpin, 
					UINT SpinIDD, CStatic *pStatic, CPoint TopLeft, CString szText);
	void CreateSpin(CEdit *pEdit, UINT EditIDD, 
		CSpinButtonCtrl *pSpin, UINT SpinIDD,CPoint TopLeft,
		CSize size = CSize(SENS_EDIT_BUDDY_X_SIZE-10,SENS_EDIT_BUDDY_Y_SIZE));
	void CheckHysValBorders(BYTE *pValue,BOOL fDirection);
	void CheckHysOffsetBorders(BYTE *pValue,BOOL fDirection);
	void CheckOffsetMeasuredValBorders(short *psValue,BOOL fDirection);	
	void CheckSampleTimeBorders(BYTE* pValue,BOOL fDirection); 
	void checkSetpointRangeValue(short* psValue,BOOL fDirection);
	void SetProfile(int profile);
	virtual void CreateDefaultButton(void);

	int getBtnRestoreDefaultsPosY(void);

	// Sedert versie 0x0900: Sensor unit Delegation
	typedef enum
	{
		// Delegation is niet ondersteund door deze unit.
		MODE_DELEGATION_UNSUPPORTED = 0,

		// Sbox+: Er kan een delegator (oled) toegevoegd worden maar is nog niet gebeurd.
		MODE_DELEGATE_ATTACH_SUPPORTED,

		// Delaegation geconfigureerd: Delegator attached
		MODE_DELEGATOR_ATTACHED,

		// Delegation geconfigureerd: Delegate attached.
		MODE_DELEGATE_ATTACHED,

		// Deze sensor kan de gemeten waarden opvragen aan een andere sensor
		MODE_DELEDATION_REQUESTMEASURE_VALUES_SUPPORTED,		// sedert versie 0x0C03

		// Deze sensor is geconfigureerd om zijn gemeten waarde op te vragen aan een andere sensor.
		MODE_DELEDATION_REQUESTMEASURE_VALUES_ATTACHED,			// sedert versie 0x0C03

	} mode_t;

	void ComboBoxDelegationInit(mode_t mode);	// sedert versie 0x0C03 wordt de mode meegegeven als een param.
	void ComboBoxDelegationAddItem(BYTE bNodeAddress,BYTE bUnitAddress);
	void ComboBoxDelegationSelItem(BYTE bNodeAddress,BYTE bUnitAddress);
	void ComboBoxDelegationUpdate(mode_t mode);

	void staticDelegationUpdate(mode_t mode);

	mode_t DelegationGetMode(void);
	void DelegationAssignDelegate(int selected=-1);

	void ControlsDelegationCreate(void);
	void ControlsDelegationUpdate(mode_t mode);

	// Sedert versie 0x0D00.
	BYTE getBindingUnitType( ) const;
	void initStaticControls(BYTE bBindingUnitType);
	void setCtrlSampleTime(int value);

	// Sedert versie 0x0E00
	void ControlsSetpointRangeCreate(void);
	void ControlsSetpointRangeUpdate(void);

	// Sedert V16.00: USE_SENSOR_PIDCONTROL	
	void ControlsPidCreate(void);
	void ControlsPidUpdate(void);

protected:	
	// Sedert versie 0x0900: Sensor unit Delegation
	CButton m_CheckBtnDelegationExtSensor;		
	CComboBox m_CmbDelegation;
	CStatic m_staticDelegation;

	CStatic m_StaticHysValue[2][4];
	CMyEditCtrl m_EditBuddyHysValue[2][4];
	CSpinButtonCtrl m_SpinHysVal[2][4];
	CButton m_BtnGroup,m_BtnGroup2;

	CStatic m_StaticHysOffset[2];
	CMyEditCtrl m_EditBuddyHysOffset[2];
	CSpinButtonCtrl m_SpinHysOffset[2];
	CButton m_BtnGroup3;

	CButton m_BtnGroupSampleInterval;
	CMyEditCtrl m_EditBuddySample;
	CSpinButtonCtrl m_SpinSample;

	CComboBox m_CmbSensType;
	CButton m_BtnResetToNull;

	CComboBox m_CmbSetProfile;

	CButton m_BtnGroupOffsetMeasuredValue;		
	CMyEditCtrl m_EditBuddyOffsetMeasuredVal;
	CSpinButtonCtrl m_SpinOffsetMeasuredVal;	

	// Controls voor beperken setpoint range.
	CStatic m_StaticSPRange[2];
	CMyEditCtrl m_EditBuddySPRange[2];
	CSpinButtonCtrl m_SpinSPRange[2];
	CButton m_BtnGroupSPRange;

	// Sedert V16.00: USE_SENSOR_PIDCONTROL	
	CButton m_btnGroupPIDControl[SENS_PIDCONTROL_WORKINGMODE_MAX];
	// de unit voor de pwm output.
	CComboBox m_CmbPIDControlUnit[SENS_PIDCONTROL_WORKINGMODE_MAX];
	// kp, ki & kd params.	
	CStatic m_StaticPIDParams[SENS_PIDCONTROL_WORKINGMODE_MAX][SENS_PIDCONTROL_PARAM_MAX];			
	CMyEditCtrl m_EditBuddyPIDParams[SENS_PIDCONTROL_WORKINGMODE_MAX][SENS_PIDCONTROL_PARAM_MAX];
	CSpinButtonCtrl m_SpinPIDParams[SENS_PIDCONTROL_WORKINGMODE_MAX][SENS_PIDCONTROL_PARAM_MAX];

	CBindingPropertySensUnit m_bindingSensUnit;
	CBindingPropertySensUnit *ptrOriginalSensUnit;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSENSPROPCHILD_H__5CE194E2_71B0_11D8_B865_0050BAC412B1__INCLUDED_)
