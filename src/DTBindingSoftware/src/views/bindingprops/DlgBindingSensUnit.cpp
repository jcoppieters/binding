// DlgBindingSensUnit.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgBindingSensUnit.h"
#include "Mainfrm.h"
#include "Nodemess.h"
#include "app\appbindings\appbindingunit\bindingunitsens.h"
#include "bindingUIElements.h"
#include "TMGlobals.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
using namespace DUOTECNO::BINDINGS::UI;
/////////////////////////////////////////////////////////////////////////////
// CDlgBindingSensUnit dialog


CDlgBindingSensUnit::CDlgBindingSensUnit(CWnd* pParent /*=NULL*/)
	: CDlgBindingUnitProp(pParent)
{
	//{{AFX_DATA_INIT(CDlgBindingSensUnit)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	pBindingSensUnit = 0;
	m_pStringEntries = 0;
}


void CDlgBindingSensUnit::DoDataExchange(CDataExchange* pDX)
{
	CDlgBindingUnitProp::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgBindingSensUnit)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgBindingSensUnit, CDlgBindingUnitProp)
	//{{AFX_MSG_MAP(CDlgBindingSensUnit)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgBindingSensUnit message handlers
void CDlgBindingSensUnit::InitControls(void)
{
	pBindingSensUnit = (CBindingSensUnit*) GetBindingUnit();
	ASSERT(pBindingSensUnit);

	CBindingStringEntries_Col stringCol;

	if (pBindingSensUnit->GetInOutType() == UNIT_TYPE_INPUT)
	{
		const BYTE bType = pBindingSensUnit->GetBindingUnitType();
		
		switch(bType)
		{			
			case BINDING_UNIT_TYPE_SENS:
			{
				m_pStringEntries = stringCol.Get(
					pBindingSensUnit,
					CBindingStringEntries_Col::SENS_UNITS_INPUT);	
			} break;
			case BINDING_UNIT_TYPE_SENSUNIV:
			{	
				m_pStringEntries = stringCol.Get(
					pBindingSensUnit,
					CBindingStringEntries_Col::SENSUNIV_UNITS_INPUT);	
			} break;
			default:
			{
				ASSERT(0);
			} break;
		}		
	}
	else
	{			
		m_pStringEntries = stringCol.Get(pBindingSensUnit,CBindingStringEntries_Col::SENS_UNITS_OUTPUT);
	}
	
	ASSERT(m_pStringEntries);

	CBindingStringEntries_Helper Helper(m_pStringEntries);
	Helper.Insert(m_CmbMethod);
	m_CmbMethod.ShowWindow(TRUE);

	UpdateControls();
}

/////////////////////////////////////////////////////////////////////////////
void CDlgBindingSensUnit::SetMode(CEdit& Ctrl)
{
	ASSERT(pBindingSensUnit);

	unsigned long ulResourceID;

	switch(pBindingSensUnit->Get_Mode() & 0x03) 
	{
		case SENS_MODE_SUN: 
			ulResourceID = IDS_SENS_SUN; 
		break;
		case SENS_MODE_HALFSUN: 
			ulResourceID = IDS_SENS_HALFSUN; 
		break;
		case SENS_MODE_MOON: 
			ulResourceID = IDS_SENS_MOON; 
		break;
		case SENS_MODE_HALFMOON: 
			ulResourceID = IDS_SENS_HALFMOON; 
		break;	
	}

	Ctrl.SetWindowText(CString((LPCSTR) ulResourceID));
}

void CDlgBindingSensUnit::UpdateControls(void)
{
	ASSERT(pBindingSensUnit);
		
	CBindingStringEntries_Helper Helper(m_pStringEntries);

	if (pBindingSensUnit->GetInOutType() == UNIT_TYPE_INPUT) 	
	{	
		const BYTE bMethodData = pBindingSensUnit->Get_Event();
		Helper.SetCurrSel(m_CmbMethod,bMethodData);		
		EnableControls();						
	}
	else 
	{	
		// UNIT_TYPE_OUTPUT

		const BYTE bMethodData = pBindingSensUnit->Get_Function();	
		Helper.SetCurrSel(m_CmbMethod,bMethodData);
			
		switch (bMethodData)
		{
			case SENS_OBJECT_METHOD_FSONOFF:
			case SENS_OBJECT_METHOD_INC:
			case SENS_OBJECT_METHOD_DEC:
			case SENS_OBJECT_METHOD_FSINC_DEC:
			{
				EnableControls();

			} break;

			case SENS_OBJECT_ATTR_CONTROL_ONOFF :
			{			
				unsigned long ulResourceID;

				EnableControls(TRUE);
					
				if ((pBindingSensUnit->Get_State() & 0x01)==0x01) 
					ulResourceID = IDS_ON;
				else 
					ulResourceID = IDS_OFF;
					
				m_EditSpin1.SetWindowText(CString((LPCSTR)ulResourceID));

			} break;

			case SENS_OBJECT_ATTR_SAMPLE_INTERVAL:	// Sedert versie 0x0D00
			{
				CString s;
				EnableControls(TRUE);
				CBindingSensUnit_Utils::getStrSampleTime(*pBindingSensUnit,&s);
				m_EditSpin1.SetWindowText(s);
			} break;

			case SENS_OBJECT_ATTR_PRESET:
			{
				EnableControls(TRUE);
				SetMode(m_EditSpin1);				

			} break;

			case SENS_OBJECT_ATTR_WORKING_SET_POINT:
			{
				EnableControls(TRUE,TRUE);
				
				CString szEdit1;
				const unsigned short usTemp = pBindingSensUnit->Get_SetPoint();	
				const BYTE bType = pBindingSensUnit->GetBindingUnitType();	

				switch(bType)
				{			
					case BINDING_UNIT_TYPE_SENS:
					{															
						szEdit1.Format("%02d,%01d°C",usTemp/10,usTemp%10);
					} break;
					case BINDING_UNIT_TYPE_SENSUNIV:
					{														
						szEdit1.Format("%02d,%01d",usTemp/10,usTemp%10);
					} break;
				};
				
				SetMode(m_EditSpin2);
				m_EditSpin1.SetWindowText(szEdit1);				

			} break;	

			case SENS_OBJECT_ATTR_WORKING_MODE:
			{
				// Sedert versie 0x0642:
				// weergeven als mode 1 tem mode 5
				long lResourceID = -1;

				EnableControls(TRUE);
				
				const BYTE bTempWorkingMode = pBindingSensUnit->Get_WorkingMode();

				CString szEdit;
				if (bTempWorkingMode != 0xFF)
				{
					szEdit.Format("mode %d  (see help)",bTempWorkingMode+1);
				}
				else
				{
					szEdit.Format("mode off");
				}
				m_EditSpin1.SetWindowText(szEdit);	

			} break;

			case SENS_OBJECT_ATTR_FAN_SPEED:
			{
				CString strTemp;
				EnableControls(TRUE);

				// Sedert versie 0x0642:
				// weergeven als Speed 1 tem Speed 5 + auto.

				const BYTE bTempFanSpeed = pBindingSensUnit->Get_FanSpeed();
				if (bTempFanSpeed == SENS_FAN_SPEED_AUTO)
				{
					strTemp = "Auto";
				}
				else
				{
					strTemp.Format("Speed %d  (see help)",bTempFanSpeed+1);
				}
				m_EditSpin1.SetWindowText(strTemp);

			} break;

			case SENS_OBJECT_ATTR_SWING_ANGLE:
			{
				CString strTemp;
				EnableControls(TRUE);

				// Sedert versie 0x0642:
				// Weergave als 0 tem 360.
				
				const unsigned short usTempSwingAngle = pBindingSensUnit->Get_Angle();

				strTemp.Format("%d  (see help)",usTempSwingAngle);

				m_EditSpin1.SetWindowText(strTemp);

			} break;

			case SENS_OBJECT_ATTR_SWING_MODE:
			{
				// TM,20210111: Uitbreiding van de swing modes.
				// Vroeger was enkel on/off mogelijk.
				CString strTemp;
				EnableControls(TRUE);
				const int state = pBindingSensUnit->Get_SwingMode();
				if (0 == state) {
					CString s;
					s.LoadString(IDS_OFF);
					strTemp.Format("%d - (%s) (see help)",state,s);
				}
				else if (1 == state) {
					CString s;
					s.LoadString(IDS_ON);
					strTemp.Format("%d - (%s) (see help)",state,s);
				}
				else {
					strTemp.Format("%d (see help)",state);
				}
				m_EditSpin1.SetWindowText(strTemp);
			} break;

			case SENS_OBJECT_ATTR_PIDCONTROLUNIT: {
			
				// Mode: Cooling / Heating + dimmer unit.
				EnableControls(TRUE,TRUE);		

				const int mode = getPIDWorkingMode();
				m_EditSpin1.SetWindowText(getStrPIDWorkingMode());

				// Updaten van de naam van de unit.
				int nodeAddress, unitAddress = 0xFF;
				if (!pBindingSensUnit->getPIDControlUnit(mode,&nodeAddress,&unitAddress)) {
					nodeAddress = -1;
					unitAddress = -1;
				}
				m_EditSpin2.SetWindowText(getStrPIDControlUnit(nodeAddress,unitAddress));
			
			} break;
			case SENS_OBJECT_ATTR_PIDCONTROLPARAMS: {				
				// Mode: Cooling/heating + kp,ki & kd
				EnableControls(TRUE,TRUE,TRUE,TRUE);		

				const int mode = getPIDWorkingMode();
				m_EditSpin1.SetWindowText(getStrPIDWorkingMode());

				int kp,ki,kd = 0;
				if (pBindingSensUnit->getPIDControlParams(mode,&kp,&ki,&kd)) {
					{
						CString str;
						str.Format("kp: %d",kp);
						m_EditSpin2.SetWindowText(str);
					}	
					{
						CString str;
						str.Format("ki: %d",ki);
						m_EditSpin3.SetWindowText(str);
					}	
					{
						CString str;
						str.Format("kd: %d",kd);
						m_EditSpin4.SetWindowText(str);
					}	
				}
			} break;
		}		
	}
}

int CDlgBindingSensUnit::getPIDWorkingMode() {
	int workingsmode = pBindingSensUnit->getPIDWorkingMode();
	if (workingsmode != CBindingSensUnit::PIDCONTROL_WORKINGMODE_COOLING &&
		workingsmode != CBindingSensUnit::PIDCONTROL_WORKINGMODE_HEATING) 
	{
		workingsmode = CBindingSensUnit::PIDCONTROL_WORKINGMODE_COOLING;
		// Oppassen: Moeten deze standaard instellen op deze workingsmode...
		pBindingSensUnit->setPIDWorkingMode(workingsmode);
	}
	return workingsmode;
}

CString CDlgBindingSensUnit::getStrPIDWorkingMode() {
	const int workingsmode = getPIDWorkingMode();
	CString str = "Workingsmode";
	if (CBindingSensUnit::PIDCONTROL_WORKINGMODE_COOLING == workingsmode) {
		str = "Cooling";
	}
	else if (CBindingSensUnit::PIDCONTROL_WORKINGMODE_HEATING == workingsmode) {
		str = "Heating";
	}				
	else { ASSERT(0); }	
	return str;
}

void CDlgBindingSensUnit::OnSpin_UpdatePIDWorkingMode(int iStep) {
	int workingsmode = pBindingSensUnit->getPIDWorkingMode();
	if (workingsmode == CBindingSensUnit::PIDCONTROL_WORKINGMODE_COOLING) {
		workingsmode = CBindingSensUnit::PIDCONTROL_WORKINGMODE_HEATING;
	}
	else if (workingsmode = CBindingSensUnit::PIDCONTROL_WORKINGMODE_HEATING) {
		workingsmode = CBindingSensUnit::PIDCONTROL_WORKINGMODE_COOLING;
	}
	else {
		workingsmode = CBindingSensUnit::PIDCONTROL_WORKINGMODE_COOLING;
	}
	pBindingSensUnit->setPIDWorkingMode(workingsmode);
}


static BOOL findUnit(CPtrList& list,int nodeAddress,int unitAddress,STRUCT_UNIT** ppUnit) {
	POSITION pos = list.GetHeadPosition();
	while(pos!=NULL) {
		STRUCT_UNIT* const pUnit = reinterpret_cast<STRUCT_UNIT*>(list.GetNext(pos));
		if (0 != pUnit) {
			if (pUnit->bNodeLogicalAddress == nodeAddress && pUnit->bUnitAddress == unitAddress) {
				*ppUnit = pUnit;
				return TRUE;
			}
		}	
	}
	return FALSE;
}

static BOOL getIndex(CPtrList& list,int nodeAddress,int unitAddress,int* index) {
	int i = 0;
	POSITION pos = list.GetHeadPosition();	
	while(pos!=NULL) {
		STRUCT_UNIT* const pUnit = reinterpret_cast<STRUCT_UNIT*>(list.GetNext(pos));
		if (0 != pUnit) {
			// TRACE3("getIndex(): index=%d, nodeAddress=0x%02x,unitAddress=0x%02x\n",i, pUnit->bNodeLogicalAddress, pUnit->bUnitAddress);
			if (pUnit->bNodeLogicalAddress == nodeAddress && pUnit->bUnitAddress == unitAddress) {
				*index = i;
				return TRUE;
			}
		}	
		i++;
	}
	return FALSE;
}

static BOOL getAt(CPtrList& list,int index,int* nodeAddress,int* unitAddress) {
	if (index >= list.GetCount()) {
		return FALSE;
	}

	int i = 0;
	POSITION pos = list.GetHeadPosition();	
	while(pos!=NULL) {
		STRUCT_UNIT* const pUnit = reinterpret_cast<STRUCT_UNIT*>(list.GetNext(pos));
		if (0 != pUnit) {		
			if (index == i) {				
				*nodeAddress = pUnit->bNodeLogicalAddress;
				*unitAddress = pUnit->bUnitAddress;
				// TRACE3("getat(%d) nodeAddress=0x%02x,unitAddress=0x%02x\n",i, *nodeAddress, *unitAddress);
				return TRUE;
			}
			++i;
		}		
	}
	return FALSE;
}


CString CDlgBindingSensUnit::getStrPIDControlUnit(int nodeAddress,int unitAddress) {
	if (nodeAddress < 0 || unitAddress < 0) {
		return CString("ERROR: Undefined");
	}
	if (0xFF == nodeAddress && 0xFF == unitAddress) {
		return CString("PID Control - Disable");
	}
	STRUCT_UNIT* pUnit = 0;
	if (findUnit(g_listUnitsDimPIDControl,nodeAddress,unitAddress,&pUnit)) {
		ASSERT(0 != pUnit);
		return CString(pUnit->szUnitName);
	}
	return CString("ERROR: Not found");
}

void CDlgBindingSensUnit::OnSpin_UpdatePIDConrolUnit(int iStep) {

	const int maxCount = g_listUnitsDimPIDControl.GetCount();
	if (maxCount <= 0) {
		return;		// Er zijn geen units - niks doen...
	}

	// Zoeken naar de huidige...
	int current = -1;					// Disabled.
	const int workingsmode = getPIDWorkingMode();
	int nodeAddress = -1, unitAddress = -1;
	if (pBindingSensUnit->getPIDControlUnit(workingsmode,&nodeAddress,&unitAddress)) {
		int index = 0;
		if (getIndex(g_listUnitsDimPIDControl,nodeAddress,unitAddress,&index)) {
			current = index;	
		}		
	}

	// TRACE3("OnSpin_UpdatePIDConrolUnit() - old Current: %d, nodeAddress=0x%02x,unitAddress=0x%02x\n",current, nodeAddress, unitAddress);
	
	current += iStep;
	if (current < -1) {
		current = -1;
	}
	else if (current>=maxCount) {
		current = maxCount-1;
	}

	{
		// Updaten van de node & unit address van de PID control unit.
		// Indien de index < 0 dan disablen van de PID controller.
		int newNodeAddress = 0xff;
		int newUnitAddress = 0xff;
		if (current>=0) {
			int n = 0xff;
			int u = 0xff;
			if (getAt(g_listUnitsDimPIDControl,current,&n,&u)) {
				newNodeAddress = n;
				newUnitAddress = u;
			}
		}
		// TRACE3("OnSpin_UpdatePIDConrolUnit() - New Current: %d, nodeAddress=0x%02x,unitAddress=0x%02x\n",current, newNodeAddress, newUnitAddress);
		pBindingSensUnit->setPIDControlUnit(workingsmode,newNodeAddress,newUnitAddress);	
	}
}
/////////////////////////////////////////////////////////////////////////////

void CDlgBindingSensUnit::OnCmbBoxDataChanged(void)
{
	ASSERT(pBindingSensUnit);

	CBindingStringEntries_Helper Helper(m_pStringEntries);
	const BYTE bMessageCode = Helper.GetCurrSelData(m_CmbMethod);

	if (pBindingSensUnit->GetInOutType() == UNIT_TYPE_INPUT) 
	{		
		if (bMessageCode == 0xFF)
			pBindingSensUnit->Set_Event(EV_HEATING);
		else
			pBindingSensUnit->Set_Event(bMessageCode);			
	}
	else 
	{	
		// UNIT_TYPE_OUTPUT
		if (bMessageCode == 0xFF)
			pBindingSensUnit->Set_Function(SENS_OBJECT_ATTR_CONTROL_ONOFF);
		else
			pBindingSensUnit->Set_Function(bMessageCode);		
	}
}

void CDlgBindingSensUnit::OnSpin_UpdateMode(int iStep)
{
	ASSERT(pBindingSensUnit);	

	BYTE bMode = pBindingSensUnit->Get_Mode() + iStep;
				
	if (iStep > 0) 
	{
		if (bMode > SENS_MODE_HALFMOON) 
			bMode = SENS_MODE_HALFMOON;
	}
	else 
	{
		if (bMode > SENS_MODE_HALFMOON) 
			bMode = SENS_MODE_SUN;
	}	
				
	pBindingSensUnit->Set_Mode(bMode);
}

void CDlgBindingSensUnit::OnSpin_UpdateWorkingMode(int iStep)
{
	ASSERT(pBindingSensUnit);	

	int workingmode = pBindingSensUnit->Get_WorkingMode() + iStep;

	if (workingmode < SENS_WORKING_MODE_AUTO)
	{
		workingmode = SENS_WORKING_MODE_AUTO;
	}
/*
	else if (workingmode > SENS_WORKING_MODE_OFF)
	{
		workingmode = SENS_WORKING_MODE_OFF;
	}
*/
	else if (workingmode > SENS_WORKING_MODE_FAN)
	{
		workingmode = SENS_WORKING_MODE_FAN;
	}

	pBindingSensUnit->Set_WorkingMode(workingmode);
}

void CDlgBindingSensUnit::OnSpin_UpdateFanSpeed(int iStep)
{
	ASSERT(pBindingSensUnit);	

	int fanSpeed = pBindingSensUnit->Get_FanSpeed() + iStep;

	if (fanSpeed < SENS_FAN_SPEED_1)
	{
		fanSpeed = SENS_FAN_SPEED_1;
	}
	else if (fanSpeed > SENS_FAN_SPEED_AUTO)
	{
		fanSpeed = SENS_FAN_SPEED_AUTO;
	}
	else if (fanSpeed > SENS_FAN_SPEED_6)	// Niet nodig (&& fanSpeed > SENS_FAN_SPEED_AUTO) omdat ervoor staat.
	{
		if (iStep > 0)
		{
			fanSpeed = SENS_FAN_SPEED_AUTO;
		}
		else
		{
			fanSpeed = SENS_FAN_SPEED_6;
		}
	}
	pBindingSensUnit->Set_FanSpeed(fanSpeed);
}


void CDlgBindingSensUnit::OnSpin1DataChanged(int iStep)
{
	ASSERT(pBindingSensUnit);	
	
	if (pBindingSensUnit->GetInOutType() == UNIT_TYPE_OUTPUT) 
	{	
		const BYTE bMethodData = pBindingSensUnit->Get_Function();		
		switch(bMethodData)
		{
			case SENS_OBJECT_ATTR_CONTROL_ONOFF : {
				pBindingSensUnit->Set_State((pBindingSensUnit->Get_State() & 0x01)^0x01);
			}
			break;	
			case SENS_OBJECT_ATTR_PRESET: {
				OnSpin_UpdateMode(iStep);
			} break;
			case SENS_OBJECT_ATTR_SAMPLE_INTERVAL:	// Sedert versie 0x0D00
			{
				int val = pBindingSensUnit->Get_SampleTime() + iStep;								
				if(val<1) {
					val = 1;
				}
				else if (val>120){
					val=120;
				}
				ASSERT(val>=1 && val<=120);
				pBindingSensUnit->Set_SampleTime(val);		
			} break;

			case SENS_OBJECT_ATTR_WORKING_SET_POINT:
			{				
				unsigned short usTemp = pBindingSensUnit->Get_SetPoint()+(iStep*5);	
				const BYTE bType = pBindingSensUnit->GetBindingUnitType();	
				int tempMax=TEMP_MAX;

				switch(bType)
				{			
					case BINDING_UNIT_TYPE_SENS:
					{
						tempMax=TEMP_MAX;
					} break;
					case BINDING_UNIT_TYPE_SENSUNIV:
					{
						tempMax=SENSUNIV_SETPOINT_MAX;
					} break;
				}			

				if (iStep > 0) {
					if (usTemp > tempMax) 
						usTemp = tempMax;
				}
				else  {
					if (usTemp > tempMax) 
						usTemp = 0;
				}						
				pBindingSensUnit->Set_Setpoint(usTemp);
			} break;

			case SENS_OBJECT_ATTR_WORKING_MODE:
			{
				OnSpin_UpdateWorkingMode(iStep);
			} break;

			case SENS_OBJECT_ATTR_FAN_SPEED:
			{
				OnSpin_UpdateFanSpeed(iStep);
			} break;

			case SENS_OBJECT_ATTR_SWING_ANGLE:
			{
				int swingAngle = pBindingSensUnit->Get_Angle( );
				// Since 0x0639:
				// Ondersteuning Mitshibutshi airco.
				// 0 = Auto, 1 tem 5 = Positions
				if ((swingAngle < 5) || (swingAngle== 5 && iStep<0)) {
					swingAngle += (iStep);
				}
				else {
					swingAngle += (iStep*5);
				}

				if (swingAngle < 0)	{
					swingAngle = 0;
				}
				else if (swingAngle > 360) {
					swingAngle = 360;
				}
				pBindingSensUnit->Set_Angle(swingAngle);
			} break;

			case SENS_OBJECT_ATTR_SWING_MODE:
			{
				// TM,20210111: Uitbreiding van de swing modes.
				// Vroeger was enkel on/off mogelijk.
				int swingMode = pBindingSensUnit->Get_SwingMode() + iStep;
				if (swingMode < SENS_SWINGMODE_MIN) {
					swingMode = SENS_SWINGMODE_MIN;
				}
				else if (swingMode > SENS_SWINGMODE_MAX) {
					swingMode = SENS_SWINGMODE_MAX;
				}				
				pBindingSensUnit->Set_SwingMode(swingMode);
			} break;

			case SENS_OBJECT_ATTR_PIDCONTROLUNIT:
			case SENS_OBJECT_ATTR_PIDCONTROLPARAMS:
			{
				this->OnSpin_UpdatePIDWorkingMode(iStep);
			} break;
		}
	}
}

void CDlgBindingSensUnit::OnSpin_UpdatePIDParam(int iStep,PidParams_t param) {
	const int workingMode = pBindingSensUnit->getPIDWorkingMode();
	int kp,ki,kd = 0;
	if (pBindingSensUnit->getPIDControlParams(workingMode,&kp,&ki,&kd)) {
		switch(param) {
			case PID_KP: {
				kp += iStep;									   
				if (kp<0) kp=0;
				else if (kp>255) kp = 255;
			} break;
			case PID_KI: {
				ki += iStep;									   
				if (ki<0) ki=0;
				else if (ki>255) ki = 255;
			} break;
			case PID_KD: {
				kd += iStep;									   
				if (kd<0) kd=0;
				else if (kd>255) kd = 255;
			} break;
		}
		pBindingSensUnit->setPIDControlParams(workingMode,kp,ki,kd);
	}
}

void CDlgBindingSensUnit::OnSpin2DataChanged(int iStep)
{
	ASSERT(pBindingSensUnit);	
	if (pBindingSensUnit->GetInOutType() == UNIT_TYPE_OUTPUT) {		
		const BYTE bMethodData = pBindingSensUnit->Get_Function();		
		switch(bMethodData) {
			case SENS_OBJECT_ATTR_WORKING_SET_POINT:{
				OnSpin_UpdateMode(iStep);
			} break;
			case SENS_OBJECT_ATTR_PIDCONTROLUNIT: {
				OnSpin_UpdatePIDConrolUnit(iStep);
			} break;
			case SENS_OBJECT_ATTR_PIDCONTROLPARAMS: {					
				OnSpin_UpdatePIDParam(iStep,PID_KP);													
			} break;
		}
	}
}

// PID
void CDlgBindingSensUnit::OnSpin3DataChanged(int iStep) {
	ASSERT(pBindingSensUnit);	
	if (pBindingSensUnit->GetInOutType() == UNIT_TYPE_OUTPUT) {		
		const BYTE bMethodData = pBindingSensUnit->Get_Function();		
		switch(bMethodData) {
/*
			case SENS_OBJECT_ATTR_PIDCONTROLUNIT: {
				// todo...				  
			} break;
*/
			case SENS_OBJECT_ATTR_PIDCONTROLPARAMS: {
				OnSpin_UpdatePIDParam(iStep,PID_KI);											  
			} break;
		}
	}
}

// PID
void CDlgBindingSensUnit::OnSpin4DataChanged(int iStep) {		
	ASSERT(pBindingSensUnit);		
	if (pBindingSensUnit->GetInOutType() == UNIT_TYPE_OUTPUT) {		
		const BYTE bMethodData = pBindingSensUnit->Get_Function();		
		switch(bMethodData) {
/*
			case SENS_OBJECT_ATTR_PIDCONTROLUNIT: {
				// todo...				  
			} break;
*/
			case SENS_OBJECT_ATTR_PIDCONTROLPARAMS: {
				OnSpin_UpdatePIDParam(iStep,PID_KD);								  
			} break;
		}
	}
}

