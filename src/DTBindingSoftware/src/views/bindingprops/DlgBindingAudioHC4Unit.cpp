// DlgBindingAudioHC4Unit.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgBindingAudioHC4Unit.h"
#include "Mainfrm.h"

#include "Nodemess.h"

#include "app\appbindings\appbindingunit\bindingunitaudiohc4.h"


#include "bindingUIElements.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
using namespace DUOTECNO::BINDINGS::UI;
/////////////////////////////////////////////////////////////////////////////
// CDlgBindingAudioHC4Unit dialog


CDlgBindingAudioHC4Unit::CDlgBindingAudioHC4Unit(CWnd* pParent /*=NULL*/)
	: CDlgBindingUnitProp(pParent)
{
	//{{AFX_DATA_INIT(CDlgBindingAudioHC4Unit)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pBindingHC4AudioUnit = 0;
	m_pStringEntries = 0;
}


void CDlgBindingAudioHC4Unit::DoDataExchange(CDataExchange* pDX)
{
	CDlgBindingUnitProp::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgBindingAudioHC4Unit)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgBindingAudioHC4Unit, CDlgBindingUnitProp)
	//{{AFX_MSG_MAP(CDlgBindingAudioHC4Unit)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgBindingAudioHC4Unit message handlers
void CDlgBindingAudioHC4Unit::InitControls(void)
{
	// TM_AUDIOHC4_BINDING_INPUT
	// Ifv het InOutType bepalen welke strings er moeten geladen worden.

	if (RetrieveBindingUnit())
	{
		CBindingStringEntries_Col::COLLECTION_KEY_UNITTYPE key = CBindingStringEntries_Col::MATRIXAUDIO_UNITS;

		if (m_pBindingHC4AudioUnit->GetInOutType() == UNIT_TYPE_OUTPUT)
		{
			key = CBindingStringEntries_Col::MATRIXAUDIO_UNITS;
		}
		else
		{
			key = CBindingStringEntries_Col::MATRIXAUDIO_INPUT_UNITS;
		}

		if (LoadStringEntryResources(key))
		{
			CBindingStringEntries_Helper Helper(m_pStringEntries);
			Helper.Insert(m_CmbMethod);

			m_CmbMethod.ShowWindow(TRUE);
		}
	}

	UpdateControls();
}

BOOL CDlgBindingAudioHC4Unit::RetrieveBindingUnit( )
{
	m_pBindingHC4AudioUnit = static_cast<CBindingHC4AudioUnit*>(GetBindingUnit());
	ASSERT(m_pBindingHC4AudioUnit);

	return (m_pBindingHC4AudioUnit == 0 ? FALSE : TRUE);
}

BOOL CDlgBindingAudioHC4Unit::LoadStringEntryResources(int key)
{
	CBindingStringEntries_Col stringCol;
	m_pStringEntries = stringCol.Get(m_pBindingHC4AudioUnit,(CBindingStringEntries_Col::COLLECTION_KEY_UNITTYPE) key);

	return (m_pStringEntries == 0 ? FALSE : TRUE);
}

void CDlgBindingAudioHC4Unit::UpdateControls(void)
{
	ASSERT(m_pBindingHC4AudioUnit);
	ASSERT(m_pStringEntries);
	
	CBindingStringEntries_Helper Helper(m_pStringEntries);

	if (m_pBindingHC4AudioUnit->GetInOutType() == UNIT_TYPE_OUTPUT)
	{
		const BYTE bMethodData = m_pBindingHC4AudioUnit->Get_Function();

		Helper.SetCurrSel(m_CmbMethod,bMethodData);

		BYTE bDest = m_pBindingHC4AudioUnit->Get_Destination();
		if (bDest > 3) bDest = 3;
		
		switch(bMethodData)
		{
			case AUDIO_MATRIX_OBJECT_ATTR_SOURCE_ON:
			{
				CString szEdit1,szEdit2;

				// Dest + Source
				EnableControls(TRUE,TRUE);
					
				szEdit1.Format(_T("Destination %d"),bDest);
				m_EditSpin1.SetWindowText(szEdit1);

				BYTE bTemp = m_pBindingHC4AudioUnit->Get_Source();
				if (bTemp > 7) 
					bTemp = 7;

				szEdit2.Format(_T("Source %d"),bTemp);
				m_EditSpin2.SetWindowText(szEdit2);

			} break;

			case AUDIO_MATRIX_OBJECT_METHOD_FSCSONOFF :
			case AUDIO_MATRIX_OBJECT_METHOD_CSON:
			case AUDIO_MATRIX_OBJECT_METHOD_OFF :
			case AUDIO_MATRIX_OBJECT_METHOD_FSVOLUPDOWN :
			case AUDIO_MATRIX_OBJECT_METHOD_VOLUP :
			case AUDIO_MATRIX_OBJECT_METHOD_VOLDOWN :
			case AUDIO_MATRIX_OBJECT_METHOD_VOLSTOP :
			case AUDIO_MATRIX_OBJECT_METHOD_PLAY :
			case AUDIO_MATRIX_OBJECT_METHOD_STOP :
			case AUDIO_MATRIX_OBJECT_METHOD_PAUSE :
			case AUDIO_MATRIX_OBJECT_METHOD_FF :
			case AUDIO_MATRIX_OBJECT_METHOD_RR :
			case AUDIO_MATRIX_OBJECT_METHOD_FFWD :
			case AUDIO_MATRIX_OBJECT_METHOD_FRWD :
			case AUDIO_OBJECT_METHOD_SOURCEDEC :
			case AUDIO_OBJECT_METHOD_SOURCEINC :
			case AUDIO_MATRIX_OBJECT_METHOD_MUTE_UNMUTE :
			{
				CString szEdit1;
					
				EnableControls(TRUE);

				szEdit1.Format(_T("Destination %d"),bDest);
				m_EditSpin1.SetWindowText(szEdit1);

			} break;

			case AUDIO_MATRIX_OBJECT_ATTR_VOLUME :
			{
				CString szEdit1,szEdit2;
			
				EnableControls(TRUE,TRUE);
					
				szEdit1.Format(_T("Destination %d"),bDest);
				m_EditSpin1.SetWindowText(szEdit1);

				BYTE bTemp = m_pBindingHC4AudioUnit->Get_Volume();
				if (bTemp > 100) 
					bTemp = 100;

				szEdit2.Format(_T("Vol. %d"),bTemp);
				m_EditSpin2.SetWindowText(szEdit2);
			} break;

			case AUDIO_MATRIX_OBJECT_ATTR_ONOFF :
			{
				CString szEdit1,szEdit2;

				// Dest + On/Off
				EnableControls(TRUE,TRUE);

				szEdit1.Format(_T("Destination %d"),bDest);
				m_EditSpin1.SetWindowText(szEdit1);

				if (m_pBindingHC4AudioUnit->Get_State() & 0x01) 
					szEdit2.LoadString(IDS_ON);
				else 
					szEdit2.LoadString(IDS_OFF);
					
				m_EditSpin2.SetWindowText(szEdit2);
			} break;

			case AUDIO_MATRIX_OBJECT_METHOD_EXTRAIR:
			{
				CString szEdit1,szEdit2;

				// Dest + Extra Keycode ...
				EnableControls(TRUE,TRUE);

				szEdit1.Format(_T("Destination %d"),bDest);
				m_EditSpin1.SetWindowText(szEdit1);

				BYTE bTemp = m_pBindingHC4AudioUnit->Get_Keypress();
				if (bTemp > 0x18) 
					bTemp = 0x18;
			
				szEdit2.LoadString(IDS_AUDIO_KPBITMAP1+bTemp);
				m_EditSpin2.SetWindowText(szEdit2);

			} break;

			default :
				ASSERT(0);
			break;
		}	
	}
	else
	{
		EnableControls( );
		m_CmbMethod.SetCurSel( 0 );

#ifdef SKIP

		/* Todo: Input type */

		const BYTE bMethodData = m_pBindingHC4AudioUnit->Get_Event();
		Helper.SetCurrSel(m_CmbMethod,bMethodData);			

		switch (bMethodData)
		{
			case EV_UNITCONTROLPULSTOGGLE:
			case EV_UNITCONTROLPULS:
			case EV_UNITCONTROLTOGGLE:
			{
				CString szEdit1;
				
				EnableControls(TRUE);

				const BYTE bMacro = m_pBindingHC4AudioUnit->Get_InpMacro( );
				ASSERT( bMacro < 128 );

				szEdit1.Format(_T("macro %d"),bMacro);
				m_EditSpin1.SetWindowText(szEdit1);

			} break;
				
			default :
			{
				ASSERT(0);

			} break;
		}

#endif

	}
}

void CDlgBindingAudioHC4Unit::OnCmbBoxDataChanged(void)
{
	ASSERT(m_pBindingHC4AudioUnit);
	ASSERT(m_pStringEntries);

	CBindingStringEntries_Helper Helper(m_pStringEntries);
	const BYTE bMessageCode = Helper.GetCurrSelData(m_CmbMethod);

	if (m_pBindingHC4AudioUnit->GetInOutType() == UNIT_TYPE_OUTPUT)
	{			
		m_pBindingHC4AudioUnit->Set_Function(bMessageCode == 0xFF ? AUDIO_MATRIX_OBJECT_METHOD_FSVOLUPDOWN : bMessageCode);	
	}
	else
	{

#ifdef SKIP
		m_pBindingHC4AudioUnit->Set_Event(bMessageCode == 0xFF ? EV_UNITCONTROLPULS : bMessageCode);
#endif

	}	
}

static void Normalize(BYTE& bValue,int iStep,const BYTE MAX)
{
	bValue += iStep;

	if (iStep>0)		
	{
		if (bValue > MAX) 
			bValue = MAX;	
	}
	else	
	{
		if (bValue > MAX) 
			bValue = 0;	
	}
} 


void CDlgBindingAudioHC4Unit::OnSpin1DataChanged(int iStep)
{
	ASSERT(m_pBindingHC4AudioUnit);

	if (m_pBindingHC4AudioUnit->GetInOutType() == UNIT_TYPE_OUTPUT)
	{	
		const BYTE bMethodData = m_pBindingHC4AudioUnit->Get_Function();
		
		switch(bMethodData)
		{
			case AUDIO_MATRIX_OBJECT_ATTR_SOURCE_ON:

			case AUDIO_MATRIX_OBJECT_METHOD_FSCSONOFF :
			case AUDIO_MATRIX_OBJECT_METHOD_CSON:
			case AUDIO_MATRIX_OBJECT_METHOD_OFF :

			case AUDIO_MATRIX_OBJECT_METHOD_FSVOLUPDOWN :
			case AUDIO_MATRIX_OBJECT_METHOD_VOLUP :
			case AUDIO_MATRIX_OBJECT_METHOD_VOLDOWN :
			case AUDIO_MATRIX_OBJECT_METHOD_VOLSTOP :

			case AUDIO_MATRIX_OBJECT_METHOD_PLAY :
			case AUDIO_MATRIX_OBJECT_METHOD_STOP :
			case AUDIO_MATRIX_OBJECT_METHOD_PAUSE :
			case AUDIO_MATRIX_OBJECT_METHOD_FF :
			case AUDIO_MATRIX_OBJECT_METHOD_RR :
			case AUDIO_MATRIX_OBJECT_METHOD_FFWD :
			case AUDIO_MATRIX_OBJECT_METHOD_FRWD :
			case AUDIO_OBJECT_METHOD_SOURCEDEC :
			case AUDIO_OBJECT_METHOD_SOURCEINC :
			case AUDIO_MATRIX_OBJECT_METHOD_MUTE_UNMUTE :

			case AUDIO_MATRIX_OBJECT_ATTR_VOLUME :
			case AUDIO_MATRIX_OBJECT_ATTR_ONOFF :
			case AUDIO_MATRIX_OBJECT_METHOD_EXTRAIR :
			{
				BYTE bDest = m_pBindingHC4AudioUnit->Get_Destination();
				Normalize(bDest,iStep,3);					
				m_pBindingHC4AudioUnit->Set_Destination(bDest);

			} break;

			default:
			{
				ASSERT( 0 );

			} break;
		}
	}
	else
	{

#ifdef SKIP

		const BYTE bMethodData = m_pBindingHC4AudioUnit->Get_Event();
		
		switch(bMethodData)
		{	
			case EV_UNITCONTROLPULSTOGGLE:
			case EV_UNITCONTROLPULS:
			case EV_UNITCONTROLTOGGLE:			
			{
				BYTE bMacro = m_pBindingHC4AudioUnit->Get_InpMacro( );

				bMacro += iStep;

				if (iStep >0) 
				{
					if (bMacro > CBindingHC4AudioUnit::MAX_AUDIO_HC4_INPUT_MACROS) 
						bMacro = CBindingHC4AudioUnit::MAX_AUDIO_HC4_INPUT_MACROS;
				}
				else 
				{
					if (bMacro > CBindingHC4AudioUnit::MAX_AUDIO_HC4_INPUT_MACROS) 
						bMacro = 0;
				}

				m_pBindingHC4AudioUnit->Set_InpMacro( bMacro );

			} break;

			default:
			{
				ASSERT( 0 );

			} break;
		}

#endif

	}
}

void CDlgBindingAudioHC4Unit::OnSpin2DataChanged(int iStep)
{
	ASSERT(m_pBindingHC4AudioUnit);

	if (m_pBindingHC4AudioUnit->GetInOutType() == UNIT_TYPE_OUTPUT)
	{	
		const BYTE bMethodData = m_pBindingHC4AudioUnit->Get_Function();
		
		switch(bMethodData)
		{
			case AUDIO_MATRIX_OBJECT_ATTR_SOURCE_ON:
			{
				BYTE bTemp = m_pBindingHC4AudioUnit->Get_Source();
				Normalize(bTemp,iStep,7);		
				m_pBindingHC4AudioUnit->Set_Source(bTemp);

			} break;

			case AUDIO_MATRIX_OBJECT_ATTR_ONOFF :
			{
				m_pBindingHC4AudioUnit->Set_State((m_pBindingHC4AudioUnit->Get_State()&0x01)^0x01);
				
			} break;

			case AUDIO_MATRIX_OBJECT_ATTR_VOLUME :
			{
				BYTE bTemp = m_pBindingHC4AudioUnit->Get_Volume();
				Normalize(bTemp,iStep,100);				
				m_pBindingHC4AudioUnit->Set_Volume(bTemp);

			} break;

			case AUDIO_MATRIX_OBJECT_METHOD_EXTRAIR :
			{
				BYTE bTemp = m_pBindingHC4AudioUnit->Get_Keypress();
				Normalize(bTemp,iStep,0x18);					
				m_pBindingHC4AudioUnit->Set_Keypress(bTemp);

			} break;
		}
	}
	else
	{
		/* Control is niet enabled bij een input event */

		ASSERT( 0 );
	}
}
