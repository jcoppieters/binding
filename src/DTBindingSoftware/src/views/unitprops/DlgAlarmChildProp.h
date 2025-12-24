#if !defined(AFX_DLGALARMCHILDPROP_H__0D7BDA10_0E04_4B29_9801_8FDA3C0C9EF8__INCLUDED_)
#define AFX_DLGALARMCHILDPROP_H__0D7BDA10_0E04_4B29_9801_8FDA3C0C9EF8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgAlarmChildProp.h : header file
//

#include "DlgUnitChildProp.h"

#include "App\Appbindings\appbindingproperty\BindingPropertyUnit.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgAlarmChildProp dialog

class CDlgAlarmChildProp : public CDlgUnitChildProp
{
// Construction
public:
	CDlgAlarmChildProp(CWnd* pParent = NULL);   // standard constructor
	virtual void OnNotifyClose(void);
	void InitControls(		
		CBindingPropertyUnit *ptr);

// Dialog Data
	//{{AFX_DATA(CDlgAlarmChildProp)
	enum { IDD = IDD_DIALOG_PROP_UNIT_CHILD };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgAlarmChildProp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	virtual void InitStaticControls(void);
	virtual void InitControlProperties(void);	
	virtual void RestoreDefaults(void);	
	virtual void UpdateControls(void);
	virtual void UpdateControlsRemoteCode(void);
	virtual BOOL UpdateRemoteCodeVal(void);

	void InitControlsToDefaults(void);
	void setNameAlarmType(const char* s);	

	// Generated message map functions
	//{{AFX_MSG(CDlgAlarmChildProp)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	//afx_msg void OnAlarmConfigType(NMHDR* pNMHDR, LRESULT* pResult); 
	afx_msg void OnAlarmConfigType(void); 
	//afx_msg void OnAlarmRemoteCode(NMHDR* pNMHDR, LRESULT* pResult);

protected:
	CBindingPropertyAlarmUnit m_bindingAlarmUnit;
	CBindingPropertyAlarmUnit *ptrOriginalAlarmUnit;
	BYTE bRemoteCode[ALARM_REMOTECODE_BUFFER_MAX];
	BYTE bGalaxyCentraleType;
	CEdit m_EditRemoteCode;
	CComboBox m_CmbAlarmType;
	CStatic m_StaticRemoteCode;
	CStatic m_StaticConfig;		

private:
	CStatic m_StaticType;			

	// Overridables ...

	/*************************************************************
	 * @brief	Alarm type weergaven in static control
	 ************************************************************/
	virtual void displayAlarmType( );

	/*************************************************************
	 * @brief	Mogelijkheid om extra init te overridden.
	 ************************************************************/
	virtual void InitStaticControls_Post(void) { };

	/*************************************************************
	 * @brief	Alarm unit type updaten ifv de selectie.
	 * @param	p [IN] ptr naar property alarm unit
	 * @param	refComboBox [IN] combo box met de entries.
	 * @return	true wnr de waarde verandered werd
	 ************************************************************/
/*
	virtual BOOL updateAlarmUnitTypeFromComboBox(
		CBindingPropertyAlarmUnit* p,
		CComboBox& refComboBox);
*/

	/**************************************************************
	 * @brief	Updaten van de alarmunit en combobox
	 * @param	p [IN] ptr naar property alarm unit
	 * @param	pComboBox [IN] ptr naar combo box met de entries.
	 * @param	fUpdateUI [IN] Updaten van de GUI of de alarm unit
	 * @return	true wnr de waarde verandered werd
	 **************************************************************/
	virtual BOOL updateAlarmUnitType(
		CBindingPropertyAlarmUnit* p,
		CComboBox* pComboBox,
		BOOL fUpdateUI = FALSE);

	/*************************************************************
	 * @brief	Combobox vullen met de verschillende types.
	 * @param	pComboBox [IN] combo box met de entries.
	 * @return	none
	 ************************************************************/
	virtual void initAlarmUnitTypeComboBox(
		CComboBox* pComboBox);
};

/////////////////////////////////////////////////////////////////////////////
// CDlgAlarmChildPropAritech 
//
// TM_CHANGES_ALARM_ARITECH:
// Deze klasse zorgt ervoor dat het type alarm centrale niet kan ingesteld worden.
// De remote code moet in beide dialogen kunnen ingesteld worden.

class CDlgAlarmChildPropAritech : public CDlgAlarmChildProp
{
public:
	CDlgAlarmChildPropAritech(CWnd* pParent = NULL);

protected:
	virtual void UpdateControls(void);

private:
	/*************************************************************
	 * @brief	Alarm type weergaven in static control
	 ************************************************************/
	virtual void displayAlarmType( );	

	/*************************************************************
	 * @brief	Alarm unit type updaten ifv de selectie.
	 * @param	p [IN] ptr naar property alarm unit
	 * @param	refComboBox [IN] combo box met de entries.
	 * @return	true wnr de waarde verandered werd
	 ************************************************************/
/*
	virtual BOOL updateAlarmUnitTypeFromComboBox(
		CBindingPropertyAlarmUnit* p,
		CComboBox& refComboBox);
*/

	/**************************************************************
	 * @brief	Updaten van de alarmunit en combobox
	 * @param	p [IN] ptr naar property alarm unit
	 * @param	pComboBox [IN] ptr naar combo box met de entries.
	 * @param	fUpdateUI [IN] Updaten van de GUI of de alarm unit
	 * @return	true wnr de waarde verandered werd
	 **************************************************************/
	virtual BOOL updateAlarmUnitType(
		CBindingPropertyAlarmUnit* p,
		CComboBox* pComboBox,
		BOOL fUpdateUI = FALSE);

	/*************************************************************
	 * @brief	Combobox vullen met de verschillende types.
	 * @param	pComboBox [IN] combo box met de entries.
	 * @return	none
	 ************************************************************/
	virtual void initAlarmUnitTypeComboBox(
		CComboBox* pComboBox);

	typedef CDlgAlarmChildProp Base;
};
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CDlgAlarmChildPropAnbBibus 
//
// TM_CHANGES_ALARM_ANB:
// Deze klasse zorgt ervoor dat het type alarm centrale en de remote code
// niet kunnen ingesteld worden.

class CDlgAlarmChildPropAnbBibus : public CDlgAlarmChildProp
{
public:
	CDlgAlarmChildPropAnbBibus(CWnd* pParent = NULL);

protected:
	virtual void UpdateControls(void);

private:
	/*************************************************************
	 * @brief	Alarm type weergaven in static control
	 ************************************************************/
	virtual void displayAlarmType( );	/* TM_CHANGES_ALARM_ANB */

	/*************************************************************
	 * @brief	Alarm unit type updaten ifv de selectie.
	 * @param	p [IN] ptr naar property alarm unit
	 * @param	refComboBox [IN] combo box met de entries.
	 * @return	true wnr de waarde verandered werd
	 ************************************************************/
/*
	virtual BOOL updateAlarmUnitTypeFromComboBox(
		CBindingPropertyAlarmUnit* p,
		CComboBox& refComboBox);
*/

	/**************************************************************
	 * @brief	Updaten van de alarmunit en combobox
	 * @param	p [IN] ptr naar property alarm unit
	 * @param	pComboBox [IN] ptr naar combo box met de entries.
	 * @param	fUpdateUI [IN] Updaten van de GUI of de alarm unit
	 * @return	true wnr de waarde verandered werd
	 **************************************************************/
	virtual BOOL updateAlarmUnitType(
		CBindingPropertyAlarmUnit* p,
		CComboBox* pComboBox,
		BOOL fUpdateUI = FALSE);

	/*************************************************************
	 * @brief	Combobox vullen met de verschillende types.
	 * @param	pComboBox [IN] combo box met de entries.
	 * @return	none
	 ************************************************************/
	virtual void initAlarmUnitTypeComboBox(
		CComboBox* pComboBox);

	typedef CDlgAlarmChildProp Base;
};
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CDlgAlarmChildPropTexecom
//
// TM_CHANGES_ALARM_TEXECOM:
// Remote code + type alarm centrale instellen.

class CDlgAlarmChildPropTexecom : public CDlgAlarmChildProp
{
public:
	CDlgAlarmChildPropTexecom(CWnd* pParent = NULL);

protected:
	virtual void UpdateControls(void);

private:
	/*************************************************************
	 * @brief	Alarm type weergaven in static control
	 ************************************************************/
	virtual void displayAlarmType( );	/* TM_CHANGES_ALARM_ANB */

	/*************************************************************
	 * @brief	Alarm unit type updaten ifv de selectie.
	 * @param	p [IN] ptr naar property alarm unit
	 * @param	refComboBox [IN] combo box met de entries.
	 * @return	true wnr de waarde verandered werd
	 ************************************************************/
/*
	virtual BOOL updateAlarmUnitTypeFromComboBox(
		CBindingPropertyAlarmUnit* p,
		CComboBox& refComboBox);
*/

	/**************************************************************
	 * @brief	Updaten van de alarmunit en combobox
	 * @param	p [IN] ptr naar property alarm unit
	 * @param	pComboBox [IN] ptr naar combo box met de entries.
	 * @param	fUpdateUI [IN] Updaten van de GUI of de alarm unit
	 * @return	true wnr de waarde verandered werd
	 **************************************************************/
	virtual BOOL updateAlarmUnitType(
		CBindingPropertyAlarmUnit* p,
		CComboBox* pComboBox,
		BOOL fUpdateUI = FALSE);

	/*************************************************************
	 * @brief	Combobox vullen met de verschillende types.
	 * @param	pComboBox [IN] combo box met de entries.
	 * @return	none
	 ************************************************************/
	virtual void initAlarmUnitTypeComboBox(
		CComboBox* pComboBox);

	typedef CDlgAlarmChildProp Base;
};
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// CDlgAlarmChildPropIntegra
//
// TM_CHANGES_ALARM_INTEGRA:
// Remote code + type alarm centrale instellen.

class CDlgAlarmChildPropIntegra : public CDlgAlarmChildProp
{
public:
	CDlgAlarmChildPropIntegra(CWnd* pParent = NULL);

protected:
	virtual void UpdateControls(void);

#if (ALARM_INTEGRA_USE_REMOTE_CODE==1)
	virtual void UpdateControlsRemoteCode(void);
	virtual BOOL UpdateRemoteCodeVal(void);
#endif

private:
	/*************************************************************
	 * @brief	Alarm type weergaven in static control
	 ************************************************************/
	virtual void displayAlarmType( );

	/*************************************************************
	 * @brief	Mogelijkheid om extra init te overridden.
	 ************************************************************/
#if (ALARM_INTEGRA_USE_REMOTE_CODE==1)
	virtual void InitStaticControls_Post(void);
#endif

	/*************************************************************
	 * @brief	Alarm unit type updaten ifv de selectie.
	 * @param	p [IN] ptr naar property alarm unit
	 * @param	refComboBox [IN] combo box met de entries.
	 * @return	true wnr de waarde verandered werd
	 ************************************************************/
/*
	virtual BOOL updateAlarmUnitTypeFromComboBox(
		CBindingPropertyAlarmUnit* p,
		CComboBox& refComboBox);
*/

	/**************************************************************
	 * @brief	Updaten van de alarmunit en combobox
	 * @param	p [IN] ptr naar property alarm unit
	 * @param	pComboBox [IN] ptr naar combo box met de entries.
	 * @param	fUpdateUI [IN] Updaten van de GUI of de alarm unit
	 * @return	true wnr de waarde verandered werd
	 **************************************************************/
	virtual BOOL updateAlarmUnitType(
		CBindingPropertyAlarmUnit* p,
		CComboBox* pComboBox,
		BOOL fUpdateUI = FALSE);

	/*************************************************************
	 * @brief	Combobox vullen met de verschillende types.
	 * @param	pComboBox [IN] combo box met de entries.
	 * @return	none
	 ************************************************************/
	virtual void initAlarmUnitTypeComboBox(
		CComboBox* pComboBox);

	typedef CDlgAlarmChildProp Base;
};
/////////////////////////////////////////////////////////////////////////////



//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGALARMCHILDPROP_H__0D7BDA10_0E04_4B29_9801_8FDA3C0C9EF8__INCLUDED_)
