// DlgSensPropDlg.cpp : implementation file
//
// Changes: TM20120403: Solved BUG-0128 
//////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgSensPropDlg.h"

#include "MyWindowMessages.h"
#include "DlgSensPropChild.h"

// --------------------------------
#include "TMGlobals.h"
using namespace TMGraphics;
// ---------------------------------

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static void CreateSpin(
	CWnd* parent,
	CEdit *pEdit, 
	UINT EditIDD, 
	CSpinButtonCtrl *pSpin, 
	UINT SpinIDD, 
	CPoint TopLeft,
	CSize size)
{
	if (1)
	{
		CSize szControlSize(15,20);
		pSpin->Create(WS_CHILD | WS_VISIBLE | WS_BORDER | UDS_ARROWKEYS,
					  CRect(TopLeft,szControlSize),
					  parent,
					  SpinIDD);
	}

	if (1)
	{	
		TopLeft.x += 20;
		pEdit->Create(WS_CHILD |WS_VISIBLE | WS_BORDER | ES_CENTER ,
					  CRect(TopLeft,size),
					  parent,
					  EditIDD);
	}
			
	pEdit->SetFont(parent->GetFont());
	pSpin->SetFont(parent->GetFont());
}

/////////////////////////////////////////////////////////////////////////////
// CDlgSensPropDlg dialog


CDlgSensPropDlg::CDlgSensPropDlg(CWnd* pParent /*=NULL*/)
	: CDlgUnitProp(pParent), fControlsCreated(FALSE) { }

void CDlgSensPropDlg::DoDataExchange(CDataExchange* pDX)
{
	CDlgUnitProp::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSensPropDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSensPropDlg, CDlgUnitProp)
	//{{AFX_MSG_MAP(CDlgSensPropDlg)
	ON_WM_HSCROLL()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	//ON_MESSAGE(WM_UNIT_STATUS_CHANGED,OnUnitStatusChanged)
	ON_BN_CLICKED(IDC_BUTTON_MODES_SUN,OnBtnModesSun)
	ON_BN_CLICKED(IDC_BUTTON_MODES_HALFSUN,OnBtnModesHalfSun)
	ON_BN_CLICKED(IDC_BUTTON_MODES_MOON,OnBtnModesMoon)
	ON_BN_CLICKED(IDC_BUTTON_MODES_HALFMOON,OnBtnModesHalfMoon)
	ON_BN_CLICKED(IDC_BUTTON_CHECK_ONOFF,OnBtnCheckOnOff)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_UPDOWN_SENSOR_SETPOINT, OnSpinSetpointUpDown)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSensPropDlg message handlers

void CDlgSensPropDlg::ShowFullDialog(BOOL fShow)
{		
	CSize size;
	//const CPoint topLeft(getRectWindow().left,getRectWindow().top);
	const CPoint topLeft(this->getPointTopLeft());

	if (fShow == TRUE)
	{	
		m_BtnExpand.SetWindowText("<<<");

		size.cx=getRectFullFrame().Width();
		size.cy=getRectFullFrame().Height();
	}
	else
	{
		m_BtnExpand.SetWindowText(">>>");
				
		size.cx=getRectMainFrame().Width();
		size.cy=getRectMainFrame().Height();
	}	

	TRACE("CDlgSensPropDlg::ShowFullDialog(%d):x=%d,y=%d,width=%d,height=%d\r\n",
		  fShow,
		  topLeft.x,
		  topLeft.y,				  
		  size.cx,
		  size.cy);

	MoveWindow(topLeft.x,topLeft.y,size.cx,size.cy);	

	if (pDlgChild != NULL) 
	{
		pDlgChild->ShowWindow(fShow);
	}
}


void CDlgSensPropDlg::CleanUp(void)
{
	if (pDlgChild != NULL)
	{
		pDlgChild->DestroyWindow();
		pDlgChild = NULL;
	}
}

void CDlgSensPropDlg::InitControls(void) // Sedert versie 0x0900 zonder params
{
	// CDlgUnitProp::InitControls( );
	
	for (int i=0;i<4;i++) {
		pRuntimeUnit->UnitSpecs.SensorUnit.sSensSetPointValue[i] = 0;
	}
		
	RequestRuntimeProperties();
	InitStaticControls();
	UpdateRuntimeProperties();

	if (fChildCreated == FALSE) {
		CalculateDialogCoordinates();
		pDlgChild = new CDlgSensPropChild();
		pDlgChild->Create(NULL,NULL,0,getRectChildFrame(),this,IDD_DIALOG_PROP_UNIT_CHILD);
		fChildCreated = TRUE;
	}

	if (pDlgChild != NULL) {
		pDlgChild->InitControls(getAssignedUnit()->m_pBindingUnit);
	}

	ShowFullDialog(fShowBindingProperties);
	UpdateData(FALSE);
}


/////////////////////////////////////////////////////////////////////////////

BYTE CDlgSensPropDlg::getBindingUnitType( ) const {
	BYTE bRetVal = BINDING_UNIT_TYPE_SENS;
	const STRUCT_UNIT* const pUnit = this->getAssignedUnit();	
	if (NULL!=pUnit->m_pBindingUnit) {
		bRetVal = pUnit->m_pBindingUnit->GetBindingUnitType();
	}
	ENSURE(bRetVal == BINDING_UNIT_TYPE_SENS || bRetVal == BINDING_UNIT_TYPE_SENSUNIV);
	return bRetVal;
}

void CDlgSensPropDlg::InitStaticControls()
{	
	if (this->fControlsCreated == FALSE)
	{
		CButton *pBtn;		

		{	// Create static Measured Value
			static const CPoint TopLeft(5,70);
			static const CSize szControlSize(150,25);		
			m_StaticMeasured.Create("",WS_CHILD | WS_VISIBLE | SS_LEFT,
								 CRect(TopLeft,szControlSize),
								 this);

			m_StaticMeasured.SetFont(pDefaultFont);
		}
		
		{	// Create Icons Measured Value
			CPoint TopLeft(5,95);
			CSize szControlSize(36,36);

			pBtn = &m_btnModes[0];
			pBtn->Create(NULL,
						 WS_CHILD | WS_VISIBLE | BS_ICON | BS_PUSHBUTTON ,
						 CRect(TopLeft,szControlSize),
						 this,
						IDC_BUTTON_MODES_SUN);
			
			pBtn->SetIcon(TMGetGraphics()->gIconModes[0]);		
		

			TopLeft.x += 40;
			pBtn = &m_btnModes[1];
			pBtn->Create(NULL,
						 WS_CHILD | WS_VISIBLE | BS_ICON | BS_PUSHBUTTON ,
						 CRect(TopLeft,szControlSize),
						 this,
						IDC_BUTTON_MODES_HALFSUN);
			
			pBtn->SetIcon(TMGetGraphics()->gIconModes[1]);	
	

			TopLeft.x -= 40;
			TopLeft.y += 40;
			pBtn = &m_btnModes[2];
			pBtn->Create(NULL,
						 WS_CHILD | WS_VISIBLE | BS_ICON | BS_PUSHBUTTON,
						 CRect(TopLeft,szControlSize),
							this,
						 IDC_BUTTON_MODES_MOON);

			pBtn->SetIcon(TMGetGraphics()->gIconModes[2]);	
		
			TopLeft.x += 40;
			pBtn = &m_btnModes[3];
			pBtn->Create(NULL,
						 WS_CHILD | WS_VISIBLE | BS_ICON | BS_PUSHBUTTON ,
						 CRect(TopLeft,szControlSize),
						 this,
						 IDC_BUTTON_MODES_HALFMOON);

			pBtn->SetIcon(TMGetGraphics()->gIconModes[3]);			
		}
		
		{	// Create Check Button On/Off
			//CPoint TopLeft(125,100);
			CPoint TopLeft(90,100);
			CSize szControlSize(150,30);
			pBtn = &m_CheckBtnOnOff;
			pBtn->Create("Sensor Control On/Off",
							   WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
							   CRect(TopLeft,szControlSize),
							   this,
							   IDC_BUTTON_CHECK_ONOFF);
			pBtn->SetFont(pDefaultFont);
		}



		{	// Create Slider For Temp setpoint			
			static const CPoint TopLeft(160,130);
			static const CSize szControlSize(160,30);
			m_trackbar.Create(WS_CHILD | WS_VISIBLE |
							  TBS_AUTOTICKS | TBS_TOP | WS_TABSTOP,
							  CRect(TopLeft,szControlSize),
							  this,
							  IDC_TRACKBAR_DIMVALUE);


			// Sedert versie 0x0D00.
			// Controle op het type binding unit.
			const BYTE bindingUnitType = this->getBindingUnitType();
			switch(bindingUnitType) {
				case BINDING_UNIT_TYPE_SENS: {
					m_trackbar.SetRange(0,TEMP_MAX,TRUE);		
				} break;
				case BINDING_UNIT_TYPE_SENSUNIV: {
					m_trackbar.SetRange(0,SENSUNIV_SETPOINT_MAX,TRUE);	
				} break;
				default: {
					ASSERT(0);
				} break;
			}				
			
			m_trackbar.SetTicFreq(50);
			m_trackbar.SetLineSize(10);
			m_trackbar.SetPageSize(50);

			{			
				static const CSize ControlSpinSize(50,20);								
				static const CPoint TopLeft(90, 135);
				CreateSpin(this,
						   &m_editSetpoint,IDC_BUDDY_EDIT_SENSOR_SETPOINT,
						   &m_spinSetpoint,IDC_SPIN_UPDOWN_SENSOR_SETPOINT, 
						   TopLeft, ControlSpinSize);
				m_editSetpoint.ModifyStyle(SS_RIGHT|SS_CENTER, SS_LEFT);
			}

			// Enable window 		
			m_BtnExpand.MoveWindow(243,170,75,23);
		}

		this->fControlsCreated = TRUE;
	}
}


void CDlgSensPropDlg::RequestRuntimeProperties()
{
	int i;

	ASSERT(getAssignedUnit());

	const BYTE bNodeAddress = getAssignedUnit()->bNodeLogicalAddress;
	const BYTE bUnitAddress = getAssignedUnit()->bUnitAddress;

	CTXSensUnitMessage* const pTXUnitMsg = static_cast<CTXSensUnitMessage*>(TMGetGuiUnit()->GetMessagePtr());

	ASSERT(pTXUnitMsg);
	
	pTXUnitMsg->RequestSensType(bNodeAddress,bUnitAddress);
	pTXUnitMsg->RequestOnOff(bNodeAddress,bUnitAddress);
	pTXUnitMsg->RequestMeasuredVal(bNodeAddress,bUnitAddress);
	pTXUnitMsg->RequestPreset(bNodeAddress,bUnitAddress);
	
	for (i=0;i<4;i++) {
		pTXUnitMsg->RequestSetPoint(bNodeAddress,bUnitAddress,i);
	}
}

void CDlgSensPropDlg::setStaticSetpointValue(short sSetpointValue)
{
	CString szValue("");
	const BYTE bindingUnitType = this->getBindingUnitType();

	switch(bindingUnitType) {
		case BINDING_UNIT_TYPE_SENS: {
			szValue.Format("%.1f C",(float)((float)(sSetpointValue)/10));	
		} break;
		case BINDING_UNIT_TYPE_SENSUNIV: {
			szValue.Format("%.1f %%",(float)((float)(sSetpointValue)/10));	
		} break;
		default: {
			ASSERT(0);
		} break;
	}
	m_editSetpoint.SetWindowText(szValue);
}

void CDlgSensPropDlg::setStaticMeasuredValue(short sValue) {
	CString szValue("Error");
	const CString szStatic("Measured Value");
	const BYTE bindingUnitType = this->getBindingUnitType();	
	switch(bindingUnitType) {
		case BINDING_UNIT_TYPE_SENS: {
			if ((sValue<=TEMP_MAX)&&(sValue >=TEMP_MIN)) {
				/* Solved BUG-0128 */
				szValue.Format("%s : %.1f C", (LPCTSTR) szStatic, (float)((float)sValue/10));
			}

		} break;
		case BINDING_UNIT_TYPE_SENSUNIV: {
			if ((sValue<=SENSUNIV_VALUE_MAX)&&(sValue>=SENSUNIV_VALUE_MIN))  {
				/* Solved BUG-0128 */
				szValue.Format("%s : %.1f %%", (LPCTSTR) szStatic, (float)((float)sValue/10));
			}	
		} break;
		default: {
			ASSERT(0);
		} break;
	}
	m_StaticMeasured.SetWindowText(szValue);	
}

short CDlgSensPropDlg::NormalizeSetpointValue(short value) {
	const BYTE bindingUnitType = this->getBindingUnitType();
	int maxSetpointValue = TEMP_MAX;
	int minSetpointValue = 0;
	switch(bindingUnitType) {
		case BINDING_UNIT_TYPE_SENS: {
			maxSetpointValue = TEMP_MAX;
			minSetpointValue = 0;
		} break;
		case BINDING_UNIT_TYPE_SENSUNIV: {
			maxSetpointValue = SENSUNIV_SETPOINT_MAX;
			minSetpointValue = SENSUNIV_SETPOINT_MIN;
		} break;
		default: {
			ASSERT(0);
		} break;
	}

	if (value>=maxSetpointValue) {
		value=maxSetpointValue;
	}
	if (value<minSetpointValue) {
		value=minSetpointValue;
	}
	return value;
}

void CDlgSensPropDlg::setTrackbarSetpointValue(short sValue) {
	this->m_trackbar.SetPos(sValue);
}


void CDlgSensPropDlg::UpdateRuntimeProperties() {	
	ASSERT(NULL != this->pRuntimeUnit);

	CDlgUnitProp::UpdateRuntimeProperties();

	const BYTE bindingUnitType = this->getBindingUnitType();
	if (fStatusReceived == TRUE) {		
		// Update Mode
		const BYTE bTempMode = ((pRuntimeUnit->UnitSpecs.SensorUnit.bSensStatus2 & SENS_PRESET_MASK)>>5);
		const short sMeasuredValue = pRuntimeUnit->UnitSpecs.SensorUnit.sSensCurrentValue;
		const short sSetpointValue = pRuntimeUnit->UnitSpecs.SensorUnit.sSensSetPointValue[bTempMode];	

		UpdateTempMode(bTempMode);
		setStaticMeasuredValue(sMeasuredValue);

		const int setpointValue = NormalizeSetpointValue(sSetpointValue);
		setStaticSetpointValue(setpointValue);
		setTrackbarSetpointValue(setpointValue);
		this->m_valueSpinCtrl = setpointValue;

		// Update Checkbox On/off
		if (pRuntimeUnit->UnitSpecs.SensorUnit.bSensStatus2 & SENS_CONTROL_ON)
			m_CheckBtnOnOff.SetCheck(TRUE);
		else
			m_CheckBtnOnOff.SetCheck(FALSE);
	}
	else {
		m_StaticMeasured.SetWindowText("N/A");
		UpdateTempMode(0xFF);
		this->m_valueSpinCtrl = 0;
		setStaticSetpointValue(0);
		setTrackbarSetpointValue(0);
		m_CheckBtnOnOff.SetCheck(FALSE);
	}
}

void CDlgSensPropDlg::CalculateDialogCoordinates()
{	
	CRect rcChildFrame;
	CRect rcFullFrame;
	CRect rcMainFrame;
	CRect rcWindow;
	
	(GetDlgItem(IDC_GROUP_CHILD))->GetWindowRect(&rcChildFrame);
	
	ScreenToClient(&rcChildFrame);
	GetClientRect(&rcFullFrame);
	GetWindowRect(&rcWindow);
#if(0)
	NormalizeWindowRect(&rcWindow);					// Ondersteuning van meerdere monitors. (Niet meer nodig sedert V17.02)
#endif 
	const CSize borderHeader = CSize(rcWindow.Width() - rcFullFrame.Width(), rcWindow.Height() - rcFullFrame.Height());

	rcMainFrame = rcFullFrame;
	rcMainFrame.bottom = rcChildFrame.top;

	rcMainFrame.bottom += borderHeader.cy;	
	rcFullFrame.bottom += borderHeader.cy;
	rcChildFrame.bottom += borderHeader.cy;

	// Sedert V17.02: Rechterkant breeder maken
	rcMainFrame.right += borderHeader.cx;
	rcFullFrame.right += borderHeader.cx;
	rcChildFrame.right += borderHeader.cx;

	rcMainFrame.bottom += 70;	
	rcFullFrame.bottom += 300;		// was 180		zie USE_SENSOR_PIDCONTROL
	rcChildFrame.bottom += 300;		// was 180		zie USE_SENSOR_PIDCONTROL
	rcChildFrame.top += 80; 

	setRectChildFrame(rcChildFrame);
	setRectFullFrame(rcFullFrame);
	setRectMainFrame(rcMainFrame);
}

void CDlgSensPropDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	BOOL fUpdate;		
	ASSERT(pRuntimeUnit);		
	switch(nSBCode) {
		case SB_RIGHT:
		case SB_LEFT:		
			fUpdate = FALSE;
		break;				
		case SB_ENDSCROLL:
			fUpdate = FALSE;
		break;
		case SB_LINERIGHT:
		case SB_LINELEFT:		
			fUpdate = FALSE;
		break;
		case SB_PAGELEFT:
		case SB_PAGERIGHT:		
			fUpdate = TRUE;
		break;
		case SB_THUMBPOSITION:			
			fUpdate = TRUE;
		break;
		case SB_THUMBTRACK:					
			fUpdate = FALSE;			
		break;
		default:
			fUpdate = FALSE;
		break;
	}
	if (fUpdate) {
		KillTimer(TIMER_ID_SLIDER_REQUEST_SEND_VALUE);
		SetTimer(TIMER_ID_SLIDER_REQUEST_SEND_VALUE, TIMER_VALUE_SLIDER_DELAY_SEND_VALUE, (TIMERPROC) NULL);
	}
	CDlgUnitProp::OnHScroll(nSBCode, nPos, pScrollBar);
}

///////////////////////////////////////////////////////////////////

void CDlgSensPropDlg::OnBtnModesSun(void) {
	CTXSensUnitMessage* const pTXUnitMsg = static_cast<CTXSensUnitMessage*>(TMGetGuiUnit()->GetMessagePtr());
	pTXUnitMsg->SetPreset(getAssignedUnit()->bNodeLogicalAddress, getAssignedUnit()->bUnitAddress, 0);
}

void CDlgSensPropDlg::OnBtnModesHalfSun(void) {
	CTXSensUnitMessage* const pTXUnitMsg = static_cast<CTXSensUnitMessage*>(TMGetGuiUnit()->GetMessagePtr());
	pTXUnitMsg->SetPreset(getAssignedUnit()->bNodeLogicalAddress, getAssignedUnit()->bUnitAddress, 1);
}

void CDlgSensPropDlg::OnBtnModesMoon(void) {
	CTXSensUnitMessage* const pTXUnitMsg = static_cast<CTXSensUnitMessage*>(TMGetGuiUnit()->GetMessagePtr());
	pTXUnitMsg->SetPreset(getAssignedUnit()->bNodeLogicalAddress,getAssignedUnit()->bUnitAddress,2);
}

void CDlgSensPropDlg::OnBtnModesHalfMoon(void) {
	CTXSensUnitMessage* const pTXUnitMsg = static_cast<CTXSensUnitMessage*>(TMGetGuiUnit()->GetMessagePtr());
	pTXUnitMsg->SetPreset(getAssignedUnit()->bNodeLogicalAddress, getAssignedUnit()->bUnitAddress, 3);
}

void CDlgSensPropDlg::OnBtnCheckOnOff(void) {
	CTXSensUnitMessage* const pTXUnitMsg = static_cast<CTXSensUnitMessage*>(TMGetGuiUnit()->GetMessagePtr());
	pTXUnitMsg->SetFsOnOff(getAssignedUnit()->bNodeLogicalAddress,getAssignedUnit()->bUnitAddress);					
}

void CDlgSensPropDlg::UpdateTempMode(BYTE bMode)
{
	for(int i=0;i<4;i++) {
		if (i == bMode) {
			m_btnModes[i].SetIcon(TMGetGraphics()->gIconModesSel[i]);
		}
		else {
			m_btnModes[i].SetIcon(TMGetGraphics()->gIconModes[i]);
		}
	}
}


BOOL CDlgSensPropDlg::sendSetpointValue(int value) {
	CTXSensUnitMessage* const pTXUnitMsg = static_cast<CTXSensUnitMessage*>(TMGetGuiUnit()->GetMessagePtr());
	if (pTXUnitMsg == NULL && this->getAssignedUnit() == NULL) {		
		return FALSE;
	}

	const BYTE bTempMode = ((pRuntimeUnit->UnitSpecs.SensorUnit.bSensStatus2 & SENS_PRESET_MASK)>>5);
	return pTXUnitMsg->SetSetpoint(getAssignedUnit()->bNodeLogicalAddress, getAssignedUnit()->bUnitAddress, value, bTempMode);
}

void CDlgSensPropDlg::OnTimer(UINT nIDEvent) {
	KillTimer(nIDEvent);

	if (nIDEvent == TIMER_ID_SLIDER_REQUEST_SEND_VALUE) {
		if (m_trackbar.GetSafeHwnd() != NULL) {
			int position = m_trackbar.GetPos();
			const BYTE bOffset = position % 5;
			position -= bOffset;
			
			const BOOL send = this->sendSetpointValue(position);
			if (send) {
				setStaticSetpointValue(position);
				this->m_valueSpinCtrl = position;
			}
			else {
				this->UpdateRuntimeProperties( );
			}
		}
	}
	else if (nIDEvent == TIMER_ID_SPINCTRL_REQUEST_SEND_VALUE) {

		this->m_valueSpinCtrl = this->NormalizeSetpointValue(this->m_valueSpinCtrl);
		const BOOL send = this->sendSetpointValue(this->m_valueSpinCtrl);
		if (FALSE == send) {
			this->UpdateRuntimeProperties( );
		}			
	}
	CDlgUnitProp::OnTimer(nIDEvent);
}


void CDlgSensPropDlg::OnSpinSetpointUpDown(NMHDR* pNMHDR, LRESULT* pResult) {
	NM_UPDOWN* const pNMUpDown = (NM_UPDOWN*)pNMHDR;
	if (NULL != pNMUpDown) {
		this->KillTimer(TIMER_ID_SPINCTRL_REQUEST_SEND_VALUE);
		this->m_valueSpinCtrl -= pNMUpDown->iDelta;		
		this->m_valueSpinCtrl = this->NormalizeSetpointValue(this->m_valueSpinCtrl);
		this->SetTimer(TIMER_ID_SPINCTRL_REQUEST_SEND_VALUE,TIMER_VALUE_SPINCTRL_DELAY_SEND_VALUE, (TIMERPROC) NULL);
		this->setStaticSetpointValue(this->m_valueSpinCtrl);
		this->setTrackbarSetpointValue(this->m_valueSpinCtrl);
	}
	*pResult = 0;
}
