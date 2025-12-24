// DlgBindingBoseRsUnit.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgBindingBoseRsUnit.h"
#include "Mainfrm.h"

#include "Nodemess.h"

#include "app\appbindings\appbindingunit\bindingunitbosers.h"


#define BROADCAST_UNIT	0xFF

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgBindingBoseRsUnit dialog


CDlgBindingBoseRsUnit::CDlgBindingBoseRsUnit(CWnd* pParent /*=NULL*/)
	: CDlgBindingUnitProp(pParent)
{
	//{{AFX_DATA_INIT(CDlgBindingBoseRsUnit)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	pBindingBoseRsUnit = NULL;

}


void CDlgBindingBoseRsUnit::DoDataExchange(CDataExchange* pDX)
{
	CDlgBindingUnitProp::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgBindingBoseRsUnit)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgBindingBoseRsUnit, CDlgBindingUnitProp)
	//{{AFX_MSG_MAP(CDlgBindingBoseRsUnit)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgBindingBoseRsUnit message handlers
void CDlgBindingBoseRsUnit::InitControls(void)
{
	int i;
	
	pBindingBoseRsUnit = (CBindingBoseRsUnit*) GetBindingUnit();
	ASSERT(pBindingBoseRsUnit);

	//Fill ComboBox Strings ...
	CString szString;
	m_CmbMethod.ResetContent();
	m_CmbMethod.ShowWindow(TRUE);

	if (pBindingBoseRsUnit->GetInOutType() == UNIT_TYPE_OUTPUT) {	//OUTPUT
		for (i=0;i<MAX_BOSERSOUTPUT_FUNCTIONS;i++) {	
			szString.LoadString(BoseRsOutputStrings[i].ulStringID);
			m_CmbMethod.InsertString(i,szString);	
		}
	}
	else {
		if (pBindingBoseRsUnit->GetUnitAddress() == BROADCAST_UNIT) { //ALL ROOMS
			for (i=0;i<MAX_BOSERSVIRTUALINPUT_FUNCTIONS;i++) {	
				szString.LoadString(BoseRsVirtualInputStrings[i].ulStringID);
				m_CmbMethod.InsertString(i,szString);	
			}
		}
		else {	//INDIVIDUAL ROOMS
			for (i=0;i<MAX_BOSERSINPUT_FUNCTIONS;i++) {	
				szString.LoadString(BoseRsInputStrings[i].ulStringID);
				m_CmbMethod.InsertString(i,szString);	
			}
		}
	}
	UpdateControls();
}

void CDlgBindingBoseRsUnit::UpdateControls(void)
{
	ASSERT(pBindingBoseRsUnit);
	int i;
	CString szEdit1,szEdit2,szText;
	BYTE bMethodData;
	BYTE bVolume,bSource,bTemp;
	//unsigned short usTemp;
		
	if (pBindingBoseRsUnit->GetInOutType() == UNIT_TYPE_OUTPUT) {	//OUTPUT
		bMethodData = pBindingBoseRsUnit->Get_Function();
		for (i=0;i<MAX_BOSERSOUTPUT_FUNCTIONS;i++) {
			if (bMethodData == BoseRsOutputStrings[i].bMethodData) break;	
		}
		if (i<MAX_BOSERSOUTPUT_FUNCTIONS) {
			m_CmbMethod.SetCurSel(i);
			switch (bMethodData)
			{
				case AUDIO_OBJECT_ATTR_SOURCE_ON:
					EnableControls(TRUE);				
					szEdit1 = pBindingBoseRsUnit->Get_strSource();
					// Todo ifv source set name : FM ...
					//szText.LoadString(IDS_SOURCE);
					//szEdit1.Format(_T("%s %d"),szText,pBindingBoseRsUnit->Get_Source());
					m_EditSpin1.SetWindowText(szEdit1);
				break;

				case AUDIO_OBJECT_ATTR_VOLUME:
					EnableControls(TRUE);
					bVolume = pBindingBoseRsUnit->Get_Volume();
					if (bVolume >BOSELF50_MAX_VOLUME) bVolume = BOSELF50_MAX_VOLUME;
					szEdit1.Format(_T("Volume %02d"),bVolume);
					m_EditSpin1.SetWindowText(szEdit1);
				break;

				case AUDIO_OBJECT_METHOD_SOURCEINC:
				case AUDIO_OBJECT_METHOD_SOURCEDEC:
				case AUDIO_OBJECT_METHOD_CSON:
				case AUDIO_OBJECT_METHOD_OFF:
				case AUDIO_OBJECT_METHOD_FSCSONOFF:
				case AUDIO_OBJECT_METHOD_VOLUP:
				case AUDIO_OBJECT_METHOD_VOLDOWN:
				case AUDIO_OBJECT_METHOD_FSVOLUPDOWN:
				case AUDIO_OBJECT_METHOD_PRESUP:
				case AUDIO_OBJECT_METHOD_PRESDOWN:
					EnableControls();
				break;

				case AUDIO_OBJECT_ATTR_ONOFF:
					EnableControls(TRUE);
					if ((pBindingBoseRsUnit->Get_State() & 0x01) == 0x01) szEdit1.LoadString(IDS_ON);
					else szEdit1.LoadString(IDS_OFF);
					m_EditSpin1.SetWindowText(szEdit1);
				break;

				case AUDIO_OBJECT_METHOD_DESTINATION_FUNCTION:
					EnableControls(TRUE);
					szText.LoadString(IDS_FUNCTION);
					szEdit1.Format(_T("%s %d"),szText,
					pBindingBoseRsUnit->Get_DestFunction() & IRAUDIO_FUNCTION_MASK);
					m_EditSpin1.SetWindowText(szEdit1);
				break;

				case AUDIO_OBJECT_METHOD_SOURCE_FUNCTION:
					EnableControls(TRUE);
					szText.LoadString(IDS_FUNCTION);
					szEdit1.Format(_T("%s %d"),szText,
					pBindingBoseRsUnit->Get_SrcFunction() & IRAUDIO_FUNCTION_MASK);
					m_EditSpin1.SetWindowText(szEdit1);
				break;

				case AUDIO_OBJECT_ATTR_TUNERFREQ:	
				case AUDIO_OBJECT_ATTR_DISC:
				case AUDIO_OBJECT_ATTR_STATE:
				case AUDIO_OBJECT_USE_SELECTOR_DATA:				
				case AUDIO_OBJECT_ATTR_CONFIG:
				default:
					EnableControls();
				break;	
			}
		}
	}
	else {
		if (pBindingBoseRsUnit->GetUnitAddress() == BROADCAST_UNIT) {	// INPUT-Virtual
			bMethodData = pBindingBoseRsUnit->Get_Event();
			for (i=0;i<MAX_BOSERSVIRTUALINPUT_FUNCTIONS;i++) {
				if (bMethodData == BoseRsVirtualInputStrings[i].bMethodData) break;	
			}
			if (i<MAX_BOSERSVIRTUALINPUT_FUNCTIONS) {
				m_CmbMethod.SetCurSel(i);
				switch (bMethodData)
				{
					case BOSELF50_EVENT_SOURCEOFF:
					case BOSELF50_EVENT_SOURCEON:
						EnableControls(TRUE);
						// Set Source AUX,V1,V2,TAPE
						bSource = pBindingBoseRsUnit->Get_Source();
						if (bSource == BOSELF50_SOURCE_TUNER) {
							bSource = BOSELF50_SOURCE_AUX;
							pBindingBoseRsUnit->Set_Source(bSource);
						}
						szEdit1.LoadString(IDS_BOSERS_FM + bSource);
						m_EditSpin1.SetWindowText(szEdit1);
					break;

					case BOSELF50_EVENT_PRESET_AUX :		
					case BOSELF50_EVENT_TUNE_AUX :	
					case BOSELF50_EVENT_PRESET_VIDEO1 :
					case BOSELF50_EVENT_TUNE_VIDEO1 :
					case BOSELF50_EVENT_PRESET_VIDEO2 :
					case BOSELF50_EVENT_TUNE_VIDEO2 :
					case BOSELF50_EVENT_PRESET_TAPE :
					case BOSELF50_EVENT_TUNE_TAPE :
						EnableControls(TRUE);		
						// Set Button 0 ... 9
						bTemp = pBindingBoseRsUnit->Get_Button();
						szEdit1.Format("Button %d",bTemp);
						m_EditSpin1.SetWindowText(szEdit1);
					break;

					case BOSELF50_EVENT_TRANSPORT_AUX:
					case BOSELF50_EVENT_TRANSPORT_VIDEO1:
					case BOSELF50_EVENT_TRANSPORT_VIDEO2:
					case BOSELF50_EVENT_TRANSPORT_TAPE:
						EnableControls(TRUE);					
						// Set Transport key Function
						bTemp = pBindingBoseRsUnit->Get_TKFunction();
						szEdit1.LoadString(IDS_BOSERS_TK_PLAY + bTemp);
						m_EditSpin1.SetWindowText(szEdit1);
					break;
				}
			}
		}
		else {	// Input
			bMethodData = pBindingBoseRsUnit->Get_Event();
			for (i=0;i<MAX_BOSERSINPUT_FUNCTIONS;i++) {
				if (bMethodData == BoseRsInputStrings[i].bMethodData) break;	
			}
			if (i<MAX_BOSERSINPUT_FUNCTIONS) {
				m_CmbMethod.SetCurSel(i);
				switch (bMethodData)
				{
					case BOSELF50_EVENT_BOSERS_DESTON:
					case BOSELF50_EVENT_BOSERS_DESTOFF:
						EnableControls();
					break;
					case BOSELF50_EVENT_SOURCEOFF:
					case BOSELF50_EVENT_SOURCEON:
						EnableControls(TRUE);
						bSource = pBindingBoseRsUnit->Get_Source();
						if (bSource < BOSELF50_SOURCE_AUX) {
							bSource = BOSELF50_SOURCE_AUX;
							pBindingBoseRsUnit->Set_Source(bSource);
						}
						szEdit1.LoadString(IDS_BOSERS_FM + bSource);
						m_EditSpin1.SetWindowText(szEdit1);
					break;
				}// end switch
			}// end if (i<MAX_BOSERSINPUT_FUNCTIONS)
		}// end else input
	}

}

void CDlgBindingBoseRsUnit::OnCmbBoxDataChanged(void)
{
	ASSERT(pBindingBoseRsUnit);
	int nCurrSel = m_CmbMethod.GetCurSel();
	if (pBindingBoseRsUnit->GetInOutType() == UNIT_TYPE_OUTPUT) {	//OUTPUT
		if (BoseRsOutputStrings[nCurrSel].bMethodData == 0xFF)
			pBindingBoseRsUnit->Set_Function(AUDIO_OBJECT_METHOD_FSVOLUPDOWN);
		else
			pBindingBoseRsUnit->Set_Function(BoseRsOutputStrings[nCurrSel].bMethodData);
	}
	else {
		if (pBindingBoseRsUnit->GetUnitAddress() == BROADCAST_UNIT) {	// INPUT-Virtual
			if (BoseRsVirtualInputStrings[nCurrSel].bMethodData == 0xFF)
				pBindingBoseRsUnit->Set_Event(BOSELF50_EVENT_SOURCEON);
			else
				pBindingBoseRsUnit->Set_Event(BoseRsVirtualInputStrings[nCurrSel].bMethodData);
		}
		else {	//INPUT		
			if (BoseRsInputStrings[nCurrSel].bMethodData == 0xFF)
				pBindingBoseRsUnit->Set_Event(BOSELF50_EVENT_SOURCEON);
			else
				pBindingBoseRsUnit->Set_Event(BoseRsInputStrings[nCurrSel].bMethodData);
		}
	}
}

void CDlgBindingBoseRsUnit::OnSpin1DataChanged(int iStep)
{
	ASSERT(pBindingBoseRsUnit);
	BYTE bMethodData,bVolume,bSource,bFunction,bState;	

	if (pBindingBoseRsUnit->GetInOutType() == UNIT_TYPE_OUTPUT) {	//OUTPUT
		bMethodData = pBindingBoseRsUnit->Get_Function();
		switch(bMethodData)
		{
			case AUDIO_OBJECT_ATTR_SOURCE_ON:
				bSource = pBindingBoseRsUnit->Get_Source();
				if (iStep > 0) {
					bSource ++;
					if (bSource >= BOSELF50_MAX_SOURCES) bSource = BOSELF50_MAX_SOURCES-1;
				}
				else {
					bSource --;
					if (bSource >= BOSELF50_MAX_SOURCES) bSource = 0;
				}
				pBindingBoseRsUnit->Set_Source(bSource);
			break;

			case AUDIO_OBJECT_ATTR_VOLUME:
				bVolume = pBindingBoseRsUnit->Get_Volume();
				bVolume += iStep;
				if (iStep >0) {
					if (bVolume > BOSELF50_MAX_VOLUME) bVolume = BOSELF50_MAX_VOLUME;
				}
				else {
					if (bVolume > BOSELF50_MAX_VOLUME) bVolume = 0;
				}
				pBindingBoseRsUnit->Set_Volume(bVolume);
			break;

			case AUDIO_OBJECT_ATTR_ONOFF:
				bState = pBindingBoseRsUnit->Get_State() & 0x01;
				bState ^= 0x01;
				pBindingBoseRsUnit->Set_State(bState);
			break;

			case AUDIO_OBJECT_METHOD_DESTINATION_FUNCTION:
				bFunction = pBindingBoseRsUnit->Get_DestFunction() & 0x01;
				bFunction ^= 0x01;
				pBindingBoseRsUnit->Set_DestFunction(bFunction);
			break;

			case AUDIO_OBJECT_METHOD_SOURCE_FUNCTION:
				bFunction = pBindingBoseRsUnit->Get_SrcFunction() & 0x01;
				bFunction ^= 0x01;
				pBindingBoseRsUnit->Set_SrcFunction(bFunction);
			break;		
		}
	}
	else {
		bMethodData = pBindingBoseRsUnit->Get_Event();
		if (pBindingBoseRsUnit->GetUnitAddress() == BROADCAST_UNIT) {	// INPUT-Virtual		
			switch(bMethodData)
			{
				case BOSELF50_EVENT_SOURCEOFF:
				case BOSELF50_EVENT_SOURCEON:				
					bSource = pBindingBoseRsUnit->Get_Source();
					bSource += iStep;
					if (iStep >0) {
						if (bSource >= BOSELF50_SOURCE_TAPE)	
							bSource = BOSELF50_SOURCE_TAPE;
					}
					else {
						if ((bSource < BOSELF50_SOURCE_AUX) || (bSource > BOSELF50_SOURCE_TAPE))
							bSource = BOSELF50_SOURCE_AUX;
					}
					pBindingBoseRsUnit->Set_Source(bSource);
				break;		

				case BOSELF50_EVENT_PRESET_AUX :		
				case BOSELF50_EVENT_TUNE_AUX :	
				case BOSELF50_EVENT_PRESET_VIDEO1 :
				case BOSELF50_EVENT_TUNE_VIDEO1 :
				case BOSELF50_EVENT_PRESET_VIDEO2 :
				case BOSELF50_EVENT_TUNE_VIDEO2 :
				case BOSELF50_EVENT_PRESET_TAPE :
				case BOSELF50_EVENT_TUNE_TAPE :						
					// Set Button 0 ... 9
					bFunction = pBindingBoseRsUnit->Get_Button();
					bFunction += iStep;
					if (iStep > 0) {
						if (bFunction > 9) bFunction = 9;
					}
					else {
						if (bFunction > 9) bFunction = 0;
					}
					pBindingBoseRsUnit->Set_Button(bFunction);
				break;

				case BOSELF50_EVENT_TRANSPORT_AUX:
				case BOSELF50_EVENT_TRANSPORT_VIDEO1:
				case BOSELF50_EVENT_TRANSPORT_VIDEO2:
				case BOSELF50_EVENT_TRANSPORT_TAPE:
					bFunction = pBindingBoseRsUnit->Get_TKFunction();
					bFunction += iStep;
					if (iStep >0) {
						if (bFunction > BOSELF50_TK_FUNCTION_NEXT )
							bFunction = BOSELF50_TK_FUNCTION_NEXT;
					}
					else {
						if (bFunction > BOSELF50_TK_FUNCTION_NEXT )
							bFunction = BOSELF50_TK_FUNCTION_PLAY;
					}
					pBindingBoseRsUnit->Set_TKFunction(bFunction);
				break;
			}
		}
		else {
			switch(bMethodData) 
			{
				case BOSELF50_EVENT_SOURCEOFF:
				case BOSELF50_EVENT_SOURCEON:				
					bSource = pBindingBoseRsUnit->Get_Source();
					bSource += iStep;
					if (iStep >0) {
						if (bSource >= BOSELF50_SOURCE_TAPE)	
							bSource = BOSELF50_SOURCE_TAPE;
					}
					else {
						if ((bSource < BOSELF50_SOURCE_AUX) || (bSource > BOSELF50_SOURCE_TAPE))
							bSource = BOSELF50_SOURCE_AUX;
					}
					pBindingBoseRsUnit->Set_Source(bSource);
				break;
			}
		}
	}
}

void CDlgBindingBoseRsUnit::OnSpin2DataChanged(int iStep)
{


}
