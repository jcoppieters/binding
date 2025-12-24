#if !defined(AFX_DLGBINDINGALARMPROP_H__325315CB_085C_40DC_81D6_27CAE854429F__INCLUDED_)
#define AFX_DLGBINDINGALARMPROP_H__325315CB_085C_40DC_81D6_27CAE854429F__INCLUDED_

// TM_CHANGES_ALARM_ARITECH + TM_CHANGES_ALARM_ANB + TM_CHANGES_ALARM_INTEGRA: 
// CDlgBindingAlarmPropAritech en CDlgBindingAlarmPropGalaxy
// displayObjMethodArm() : Bij de galaxy centrale kan het systeem volledig of 
// gedeeltelijk gewapend worden. Bij de Aritech en ANB kan enkel volledig gewapend worden.
// displayObjMethodKeypad() : De benaming van de keypad is anders. 
// Galaxy = 'A' en 'B', Aritech en ANB = 'On' en 'Off'


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgBindingAlarmProp.h : header file
//

#include "DlgBindingUnitProp.h"
#include "BindingUiElement.h"


class CBindingAlarmUnit;


/////////////////////////////////////////////////////////////////////////////
// CDlgBindingAlarmProp dialog
class CDlgBindingAlarmProp : public CDlgBindingUnitProp
{
// Construction
public:
	CDlgBindingAlarmProp(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgBindingAlarmProp)
	enum { IDD = IDD_DIALOG_PROP_BINDINGUNIT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgBindingAlarmProp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CBindingAlarmUnit* m_pBindingAlarmUnit;

	// TM_CHANGES_BINDINGS_ALARM: Andere implementatie die gebruik maakt van de StringEntries 
	// collecties.
	const DUOTECNO::BINDINGS::UI::CStringEntries* m_pStringEntries;

	virtual void InitControls(void);
	virtual void OnCmbBoxDataChanged(void);
	virtual void OnSpin1DataChanged(int iStep);
	virtual void OnSpin2DataChanged(int iStep);
	//virtual void OnSpin3DataChanged(int iStep);
	void UpdateControls(void);
	virtual void displayObjMethodArm( )			= 0;	// TM_CHANGES_ALARM_ARITECH
	virtual void displayObjMethodKeypad( )		= 0;	// TM_CHANGES_ALARM_ARITECH
	virtual void GetZoneName(
		int zoneNR,
		int maxLength,
		char* szName)							= 0;	// TM_CHANGES_ALARM_ARITECH

	// Generated message map functions
	//{{AFX_MSG(CDlgBindingAlarmProp)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


protected:
	static void displayObjMethodArmSimple(CDlgBindingAlarmProp* me);
	static void displayObjMethodKeypadSimple(CDlgBindingAlarmProp* me);
};


/////////////////////////////////////////////////////////////////////////////
// CDlgBindingAlarmPropGalaxy dialog

class CDlgBindingAlarmPropGalaxy : public CDlgBindingAlarmProp
{
public:
	CDlgBindingAlarmPropGalaxy(CWnd* pParent = NULL);   

protected:
	virtual void GetZoneName(int zoneNR,int maxLength,char* szName);
	virtual void displayObjMethodArm( );
	virtual void displayObjMethodKeypad( );

private:
	typedef CDlgBindingAlarmProp Base;
};
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CDlgBindingAlarmPropAritech dialog

class CDlgBindingAlarmPropAritech : public CDlgBindingAlarmProp
{
public:
	CDlgBindingAlarmPropAritech(CWnd* pParent = NULL);   

protected:
	virtual void GetZoneName(int zoneNR,int maxLength,char* szName);
	virtual void displayObjMethodArm( );
	virtual void displayObjMethodKeypad( );

private:
	typedef CDlgBindingAlarmProp Base;
};
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// CDlgBindingAlarmPropAnbBibus dialog

class CDlgBindingAlarmPropAnbBibus : public CDlgBindingAlarmProp
{
public:
	CDlgBindingAlarmPropAnbBibus(CWnd* pParent = NULL);   

protected:
	virtual void GetZoneName(int zoneNR,int maxLength,char* szName);
	virtual void displayObjMethodArm( );
	virtual void displayObjMethodKeypad( );

private:
	typedef CDlgBindingAlarmProp Base;
};
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CDlgBindingAlarmPropTexecom dialog

class CDlgBindingAlarmPropTexecom : public CDlgBindingAlarmProp
{
public:
	CDlgBindingAlarmPropTexecom(CWnd* pParent = NULL);   

protected:
	virtual void GetZoneName(int zoneNR,int maxLength,char* szName);
	virtual void displayObjMethodArm( );
	virtual void displayObjMethodKeypad( );

private:
	typedef CDlgBindingAlarmProp Base;
};
/////////////////////////////////////////////////////////////////////////////
// CDlgBindingAlarmPropIntegra

class CDlgBindingAlarmPropIntegra : public CDlgBindingAlarmProp
{
public:
	CDlgBindingAlarmPropIntegra(CWnd* pParent = NULL);   

protected:
	virtual void GetZoneName(int zoneNR,int maxLength,char* szName);
	virtual void displayObjMethodArm( );
	virtual void displayObjMethodKeypad( );

	virtual void OnSpin2DataChanged(int iStep);

private:

	enum
	{
		INTEGRA_ALARM_ARM_MODE0 =0,
		INTEGRA_ALARM_ARM_MODE1 =1,
		INTEGRA_ALARM_ARM_MODE2 =2,
		INTEGRA_ALARM_ARM_MODE3 =3,	
	};

	typedef CDlgBindingAlarmProp Base;
};
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGBINDINGALARMPROP_H__325315CB_085C_40DC_81D6_27CAE854429F__INCLUDED_)
