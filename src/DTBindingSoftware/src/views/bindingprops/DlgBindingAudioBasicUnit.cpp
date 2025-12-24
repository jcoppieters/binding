// DlgBindingAudioBasicUnit.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgBindingAudioBasicUnit.h"
#include "Mainfrm.h"

#include "Nodemess.h"

#include "app\appbindings\appbindingunit\bindingunitaudiobasic.h"
#include "bindingUiElements.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
using namespace DUOTECNO::BINDINGS::UI;
/////////////////////////////////////////////////////////////////////////////
// CDlgBindingAudioBasicUnit dialog


CDlgBindingAudioBasicUnit::CDlgBindingAudioBasicUnit(CWnd* pParent /*=NULL*/)
	: CDlgBindingUnitProp(pParent)
{
	//{{AFX_DATA_INIT(CDlgBindingAudioBasicUnit)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	pBindingBasicAudioUnit = NULL;
	m_pStringEntries = 0;
}


void CDlgBindingAudioBasicUnit::DoDataExchange(CDataExchange* pDX)
{
	CDlgBindingUnitProp::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgBindingAudioBasicUnit)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgBindingAudioBasicUnit, CDlgBindingUnitProp)
	//{{AFX_MSG_MAP(CDlgBindingAudioBasicUnit)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgBindingAudioBasicUnit message handlers
void CDlgBindingAudioBasicUnit::InitControls(void)
{
	pBindingBasicAudioUnit = (CBindingBasicAudioUnit*) GetBindingUnit();
	ASSERT(pBindingBasicAudioUnit);

	CBindingStringEntries_Col stringCol;
	m_pStringEntries = stringCol.Get(pBindingBasicAudioUnit,
									 CBindingStringEntries_Col::IRAUDIO_UNITS_OUTPUT);

	CBindingStringEntries_Helper Helper(m_pStringEntries);
	Helper.Insert(m_CmbMethod);
	m_CmbMethod.ShowWindow(TRUE);

	UpdateControls();
}

void CDlgBindingAudioBasicUnit::UpdateControls(void)
{
	ASSERT(pBindingBasicAudioUnit);

	const BYTE bMethodData = pBindingBasicAudioUnit->Get_Function();
	
	CBindingStringEntries_Helper Helper(m_pStringEntries);
	Helper.SetCurrSel(m_CmbMethod,bMethodData);
	
	switch(bMethodData)
	{
		case AUDIO_OBJECT_ATTR_SOURCE_ON:
		{
			CString szEdit1;

			EnableControls(TRUE);
					
			szEdit1.Format(_T("%s %d"),CString((LPCSTR) IDS_SOURCE),
								pBindingBasicAudioUnit->Get_Source());
				
			m_EditSpin1.SetWindowText(szEdit1);

		} break;

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
		{
			EnableControls();
		} break;

		case AUDIO_OBJECT_ATTR_ONOFF:
		{
			CString szEdit1;

			EnableControls(TRUE);

			if ((pBindingBasicAudioUnit->Get_State() & 0x01) == 0x01) 
				szEdit1.LoadString(IDS_ON);
			else 
				szEdit1.LoadString(IDS_OFF);
				
			m_EditSpin1.SetWindowText(szEdit1);

		} break;

		case AUDIO_OBJECT_METHOD_DESTINATION_FUNCTION:
		{
			CString szEdit1;

			EnableControls(TRUE);
						
			szEdit1.Format(_T("%s %d"),CString((LPCSTR) IDS_FUNCTION),
				pBindingBasicAudioUnit->Get_DestFunction() & IRAUDIO_FUNCTION_MASK);
				
			m_EditSpin1.SetWindowText(szEdit1);

		} break;

		case AUDIO_OBJECT_METHOD_SOURCE_FUNCTION:
		{
			CString szEdit1;

			EnableControls(TRUE);
							
			szEdit1.Format(_T("%s %d"),CString((LPCSTR) IDS_FUNCTION),
				pBindingBasicAudioUnit->Get_SrcFunction() & IRAUDIO_FUNCTION_MASK);

			m_EditSpin1.SetWindowText(szEdit1);

		} break;

		case AUDIO_OBJECT_ATTR_TUNERFREQ:	
		case AUDIO_OBJECT_ATTR_DISC:
		case AUDIO_OBJECT_ATTR_STATE:
		case AUDIO_OBJECT_USE_SELECTOR_DATA:
		case AUDIO_OBJECT_ATTR_VOLUME :
		case AUDIO_OBJECT_ATTR_CONFIG:
		default:
		{
			EnableControls();
		} break;
	}
}

/////////////////////////////////////////////////////////////////////////////

void CDlgBindingAudioBasicUnit::OnCmbBoxDataChanged(void)
{
	ASSERT(pBindingBasicAudioUnit);
	ASSERT(m_pStringEntries);

	CBindingStringEntries_Helper Helper(m_pStringEntries);
	const BYTE bMessageCode = Helper.GetCurrSelData(m_CmbMethod);
	
	if (bMessageCode == 0xFF)
		pBindingBasicAudioUnit->Set_Function(AUDIO_OBJECT_METHOD_FSVOLUPDOWN);
	else
		pBindingBasicAudioUnit->Set_Function(bMessageCode);
}

void CDlgBindingAudioBasicUnit::OnSpin1DataChanged(int iStep)
{
	ASSERT(pBindingBasicAudioUnit);
	
	const BYTE bMethodData = pBindingBasicAudioUnit->Get_Function();
	switch(bMethodData)
	{
		case AUDIO_OBJECT_ATTR_SOURCE_ON:
		{
			BYTE bSource = pBindingBasicAudioUnit->Get_Source();
			if (iStep > 0) 
			{
				bSource ++;
				if (bSource >= IRAUDIO_MAX_SOURCES) 
					bSource = IRAUDIO_MAX_SOURCES-1;
			}
			else 
			{
				bSource --;
				if (bSource >= IRAUDIO_MAX_SOURCES) 
					bSource = 0;
			}
			pBindingBasicAudioUnit->Set_Source(bSource);
		} break;

		case AUDIO_OBJECT_ATTR_ONOFF:		
		{
			pBindingBasicAudioUnit->Set_State((pBindingBasicAudioUnit->Get_State()&0x01)^0x01);
		} break;

		case AUDIO_OBJECT_METHOD_DESTINATION_FUNCTION:		
		{
			pBindingBasicAudioUnit->Set_DestFunction((pBindingBasicAudioUnit->Get_DestFunction()&0x01)^0x01);
		} break;

		case AUDIO_OBJECT_METHOD_SOURCE_FUNCTION:		
		{
			pBindingBasicAudioUnit->Set_SrcFunction((pBindingBasicAudioUnit->Get_SrcFunction()&0x01)^0x01);
		} break;
	}
}

