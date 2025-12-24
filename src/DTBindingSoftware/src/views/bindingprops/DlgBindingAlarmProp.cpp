// DlgBindingAlarmProp.cpp : implementation file
//
// TM_CHANGES_ALARM_ARITECH: CDlgBindingAlarmPropAritech en CDlgBindingAlarmPropGalaxy
// Bij de galaxy centrale kan het systeem volledig of gedeeltelijk gewapend worden.
// Bij de Aritech kan enkel volledig gewapend worden.
// De benaming van de keypad is anders. 
// Galaxy = 'A' en 'B', Aritech = 'On' en 'Off'


#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgBindingAlarmProp.h"
#include "Mainfrm.h"

#include "Nodemess.h"
#include "App/AppCfg/AppCfgMM/ConfigMultiMediaClass.h"

#include "app\appbindings\appbindingunit\bindingunitalarm.h"

#include "bindingUIElements.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
using namespace DUOTECNO::BINDINGS::UI;
/////////////////////////////////////////////////////////////////////////////

#define TRACER_EVENT(x)			TRACE x


//////////////////////////////////////////////////////////////////////////////

namespace 
{
	const char* _groupNR[] =
	{
		"0000: No Groups", 
		"0001: Group 1", 
		"0010: Group 2", 
		"0011: Group 1+2", 
		"0100: Group 3",
		"0101: Group 1+3", 
		"0110: Group 2+3", 
		"0111: Group 1+2+3", 
		"1000: Group 4", 
		"1001: Group 1+4",
		"1010: Group 2+4", 
		"1011: Group 1+2+4", 
		"1100: Group 3+4", 
		"1101: Group 1+3+4", 
		"1110: Group 2+3+4",
		"1111: All Groups"	
	};
};


/////////////////////////////////////////////////////////////////////////////
// CDlgBindingAlarmProp dialog


CDlgBindingAlarmProp::CDlgBindingAlarmProp(CWnd* pParent /*=NULL*/)
	: CDlgBindingUnitProp(pParent)
{
	//{{AFX_DATA_INIT(CDlgBindingAlarmProp)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pBindingAlarmUnit = NULL;
	m_pStringEntries = 0;
}


void CDlgBindingAlarmProp::DoDataExchange(CDataExchange* pDX)
{
	CDlgBindingUnitProp::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgBindingAlarmProp)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgBindingAlarmProp, CDlgBindingUnitProp)
	//{{AFX_MSG_MAP(CDlgBindingAlarmProp)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgBindingAlarmProp message handlers

// TM_CHANGES_BINDINGS_ALARM: Andere implementatie.
void CDlgBindingAlarmProp::InitControls(void)
{	
	CBindingStringEntries_Col stringCol;

	m_pBindingAlarmUnit = (CBindingAlarmUnit*) GetBindingUnit();
	ASSERT(m_pBindingAlarmUnit);

	if (m_pBindingAlarmUnit->GetInOutType() == PM_INOUTTYPE_INPUT) 
	{		
		m_pStringEntries = stringCol.Get(m_pBindingAlarmUnit,CBindingStringEntries_Col::ALARM_INPUT);
	}
	else
	{	
		m_pStringEntries = stringCol.Get(m_pBindingAlarmUnit,CBindingStringEntries_Col::ALARM_OUTPUT);
	}

	ASSERT(m_pStringEntries);

	CBindingStringEntries_Helper Helper(m_pStringEntries);
	Helper.Insert(m_CmbMethod);
	m_CmbMethod.ShowWindow(TRUE);

	UpdateControls();
}


void CDlgBindingAlarmProp::UpdateControls(void)
{
	CBindingStringEntries_Helper Helper(m_pStringEntries);

	ASSERT(m_pBindingAlarmUnit);

	CString szEdit1,szEdit2;
	BYTE bTemp;
		
	if (m_pBindingAlarmUnit->GetInOutType() == UNIT_TYPE_INPUT) 
	{	
		// TM_CHANGES_BINDINGS_ALARM: verschillende soorten events.

		const BYTE bEvent = m_pBindingAlarmUnit->Get_Event();
		Helper.SetCurrSel(m_CmbMethod,bEvent);		

		TRACER_EVENT(("UpdateControls - Event=%d\n",bEvent));
						
		switch(bEvent)
		{
			case ALARM_EVENT_ALARM_INPUT:
			{					
				EnableControls(TRUE);

				// Set Zone ...
				const BYTE bMaxInputs = m_pBindingAlarmUnit->Get_MaxInputs( );	
				bTemp = m_pBindingAlarmUnit->Get_ZoneNr();
				if (bTemp >= bMaxInputs) 
				{
					bTemp = bMaxInputs-1;
				}
				
				char szZoneName[16];
				GetZoneName(bTemp,16,szZoneName);
				szEdit1.Format(_T("%s"),szZoneName);

				m_EditSpin1.SetWindowText(szEdit1);			

			} break;

			case ALARM_EVENT_ALARM_ARM:
			{
				EnableControls(TRUE,TRUE);

				// Set GroupNR
				// GroupNR 1 tem 4
				BYTE bGroupNR = m_pBindingAlarmUnit->Get_GroupNr();

				if (bGroupNR<1)
				{
					bGroupNR=1;
				}

				if (bGroupNR>4)
				{
					bGroupNR=4;
				}

				char szGroupName[16];
				sprintf(szGroupName,
						_T("Group %d"),
						bGroupNR);
				szEdit1.Format(_T("%s"),szGroupName);
				m_EditSpin1.SetWindowText(szEdit1);			


				// Armed/Disarmed...
				char szDescr[16];
				sprintf(szDescr,
						_T("Status %s"),
						m_pBindingAlarmUnit->Get_ControlState()==0 ? 
						"disarmed" : "armed");				
				szEdit2.Format(_T("%s"),szDescr);
				m_EditSpin2.SetWindowText(szEdit2);	

			} break;


			case ALARM_EVENT_ALARM_BURGLARY:
			{
				EnableControls(TRUE,TRUE);

				// Set GroupNR
				// GroupNR 1 tem 4
				BYTE bGroupNR = m_pBindingAlarmUnit->Get_GroupNr();

				if (bGroupNR<1)
				{
					bGroupNR=1;
				}
				if (bGroupNR>4)
				{
					bGroupNR=4;
				}

				char szGroupName[16];
				sprintf(szGroupName,_T("Group %d"),bGroupNR);
				szEdit1.Format(_T("%s"),szGroupName);
				m_EditSpin1.SetWindowText(szEdit1);			


				// Burglary/No Burglary...
				char szDescr[16];
				sprintf(szDescr,
						_T("%s alarm"),
						m_pBindingAlarmUnit->Get_ControlState()==0 ? 
						"No burglary" : "Burglary");
				
				szEdit2.Format(_T("%s"),szDescr);
				m_EditSpin2.SetWindowText(szEdit2);	

			} break;

			default:
			{
				ASSERT(0);
			} break;

		} /* switch(bEvent) */
	}
	else
	{	
		// UNIT_TYPE_OUTPUT

		
		const BYTE bMethodData = m_pBindingAlarmUnit->Get_Function();

		Helper.SetCurrSel(m_CmbMethod,bMethodData);

		switch (bMethodData)
		{
			case ALARM_OBJECT_METHOD_ARM :
			{
				// TM_CHANGES_ALARM_ARITECH:
				// Bij de galaxy centrale kan het systeem volledig of gedeeltelijk gewapend worden.
				// Bij de Aritech kan enkel volledig gewapend worden.

				displayObjMethodArm( );

			} break;

			case ALARM_OBJECT_METHOD_KEYPAD :
			{
				// TM_CHANGES_ALARM_ARITECH:
				// Andere benaming van de key's:
				// Galaxy: 'A' en 'B', Aritech: 'On' en 'Off'					

				displayObjMethodKeypad( );	

			} break;

			default:
				ASSERT(0);
			break;
		}
	}
}


// TM_CHANGES_BINDINGS_ALARM: Andere implementatie.
void CDlgBindingAlarmProp::OnCmbBoxDataChanged(void)
{	
	CBindingStringEntries_Helper Helper(m_pStringEntries);
	const BYTE bMessageCode = Helper.GetCurrSelData(m_CmbMethod);

	ASSERT(m_pBindingAlarmUnit);

	if (m_pBindingAlarmUnit->GetInOutType() == UNIT_TYPE_INPUT) 
	{			
		if (bMessageCode == 0xFF)
			m_pBindingAlarmUnit->Set_Event(ALARM_EVENT_ALARM_INPUT);
		else
			m_pBindingAlarmUnit->Set_Event(bMessageCode);		

		TRACER_EVENT(("OnCmbBoxDataChanged - Event=%d\n",bMessageCode));

	}
	else 
	{			
		if (bMessageCode == 0xFF)
			m_pBindingAlarmUnit->Set_Function(ALARM_OBJECT_METHOD_ARM);
		else
			m_pBindingAlarmUnit->Set_Function(bMessageCode);		
	}
}	

// TM_CHANGES_BINDINGS_ALARM: Andere implementatie
void CDlgBindingAlarmProp::OnSpin1DataChanged(int iStep)
{
	ASSERT(m_pBindingAlarmUnit);
	BYTE bTemp;

	if (m_pBindingAlarmUnit->GetInOutType() == UNIT_TYPE_INPUT) 
	{	
		const BYTE bEvent = m_pBindingAlarmUnit->Get_Event();

		TRACER_EVENT(("OnSpin1DataChanged - Event=%d\n",bEvent));

		switch(bEvent)
		{
			case ALARM_EVENT_ALARM_INPUT:
			{
				const BYTE bMaxInputs = m_pBindingAlarmUnit->Get_MaxInputs( );	
				
				bTemp = m_pBindingAlarmUnit->Get_ZoneNr();
				if (iStep > 0) 
				{
					bTemp += iStep;
					if (bTemp >= bMaxInputs) 
					{
						bTemp = bMaxInputs-1;
					}
				}
				else 
				{
					bTemp += iStep;
					if (bTemp > bMaxInputs) 
					{
						bTemp = 0;
					}
				}
				m_pBindingAlarmUnit->Set_ZoneNr(bTemp);

			} break;

			case ALARM_EVENT_ALARM_BURGLARY:
			case ALARM_EVENT_ALARM_ARM:
			{
				// Group nr aanpassen: groupNR 1 tem 4

				int groupNR = m_pBindingAlarmUnit->Get_GroupNr();
				
				groupNR+=iStep;
				if (groupNR<1) 
				{
					groupNR=1;	
				}
				else if (groupNR>4)
				{
					groupNR=4;
				}
					
				ASSERT(groupNR>=1&&groupNR<=4);
				m_pBindingAlarmUnit->Set_GroupNr((BYTE) groupNR);

			} break;

			default:
			{
				ASSERT(0);

			} break;
		} /* switch(bMethodData) */
	}
	else 
	{
		const BYTE bMethodData = m_pBindingAlarmUnit->Get_Function();

		switch(bMethodData)
		{
			case ALARM_OBJECT_METHOD_ARM:
			{
				bTemp = m_pBindingAlarmUnit->Get_GroupNr();			
				
				if (iStep > 0) 
				{
					bTemp += iStep;
					
					if (bTemp > 0x0F)  
						bTemp = 0x0F;
				}
				else
				{
					bTemp += iStep;
					
					if (bTemp > 0x0f) 
						bTemp = 1;
				}

				m_pBindingAlarmUnit->Set_GroupNr(bTemp);

			} break;

			case ALARM_OBJECT_METHOD_KEYPAD :
			{
				bTemp = m_pBindingAlarmUnit->Get_Keypress();
				
				if (iStep > 0) 
				{
					bTemp += iStep;
					
					if (bTemp > 0x0d) 
						bTemp = 0x0d;
				}
				else 
				{
					bTemp += iStep;
					
					if (bTemp > 0x0d) 
						bTemp = 0;
				}
				
				m_pBindingAlarmUnit->Set_Keypress(bTemp);

			} break;

			default :
				ASSERT(0);
			break;
		}
	}
}

// TM_CHANGES_BINDINGS_ALARM: Implementatie ook voor de input unit.
void CDlgBindingAlarmProp::OnSpin2DataChanged(int iStep)
{
	ASSERT(m_pBindingAlarmUnit);

	const BYTE bType=m_pBindingAlarmUnit->GetInOutType();

	if (UNIT_TYPE_INPUT==bType)
	{
		BYTE bEvent = m_pBindingAlarmUnit->Get_Event();

		TRACER_EVENT(("OnSpin2DataChanged - Event=%d\n",bEvent));

		switch(bEvent)
		{
			case ALARM_EVENT_ALARM_BURGLARY:
			case ALARM_EVENT_ALARM_ARM:
			{
				int state=0;

				if (m_pBindingAlarmUnit->Get_ControlState()&0x01)
					state=0;
				else
					state=1;

				m_pBindingAlarmUnit->Set_ControlState(state);

			} break;		
		}
	}
	else if (UNIT_TYPE_OUTPUT==bType) 
	{			
		BYTE bMethodData = m_pBindingAlarmUnit->Get_Function();
		
		if (bMethodData == ALARM_OBJECT_METHOD_ARM) 
		{	
			if (m_pBindingAlarmUnit->Get_ArmSet() & 0x01)
				m_pBindingAlarmUnit->Set_ArmSet(0);
			else 
				m_pBindingAlarmUnit->Set_ArmSet(1);
		}
	}
}

///////////////////////// Static Helper dialogs  ////////////////////////////
void CDlgBindingAlarmProp::displayObjMethodArmSimple(
	CDlgBindingAlarmProp* me)
{	
	me->EnableControls(TRUE);

	BYTE bTemp = me->m_pBindingAlarmUnit->Get_GroupNr();
	bTemp &= 0x0F;
		
	CString szEdit1;
	szEdit1.Format(_T("%s"),_groupNR[bTemp]);
	me->m_EditSpin1.SetWindowText(szEdit1);
}

void CDlgBindingAlarmProp::displayObjMethodKeypadSimple(
	CDlgBindingAlarmProp* me)
{
	CString szEdit1;

	me->EnableControls(TRUE);

	BYTE bTemp = me->m_pBindingAlarmUnit->Get_Keypress();
	if (bTemp > 0x0D) bTemp = 0x0D;

	if ((bTemp >= 0) && (bTemp < 0x0A)) 
	{
		szEdit1.Format(_T("Key : %d"),bTemp);					
	}
	else if (bTemp == 0x0A) 
	{
		szEdit1.Format(_T("On"));
	}
	else if (bTemp == 0x0B) 
	{
		szEdit1.Format(_T("Off"));
	}
	else if (bTemp == 0x0C) 
	{
		szEdit1.Format(_T("Clear"));
	}
	else if (bTemp == 0x0D) 
	{
		szEdit1.Format(_T("Enter"));
	}

	me->m_EditSpin1.SetWindowText(szEdit1);
}

/////////////////////////////////////////////////////////////////////////////
// CDlgBindingAlarmPropGalaxy dialog

CDlgBindingAlarmPropGalaxy::CDlgBindingAlarmPropGalaxy(CWnd* pParent) :
Base( pParent )
{

}

void CDlgBindingAlarmPropGalaxy::GetZoneName(
	int zoneNR,
	int maxLength,
	char* szName)
{
	if (!CConfigAlarmZoneNameUtil::GetAlarmZoneName(
			CConfigAlarmZoneNameUtil::TYPE_ALARM_GALAXY,
			zoneNR,
			maxLength,
			szName))
	{
		sprintf(szName,"AlarmZone ?");
	}		
}

void CDlgBindingAlarmPropGalaxy::displayObjMethodArm( )
{
	CString szEdit1, szEdit2;

	EnableControls(TRUE,TRUE);

	BYTE bTemp = m_pBindingAlarmUnit->Get_GroupNr();
	bTemp &= 0x0F;
					
	szEdit1.Format(_T("%s"),_groupNR[bTemp]);
	m_EditSpin1.SetWindowText(szEdit1);

	bTemp = m_pBindingAlarmUnit->Get_ArmSet();
	if (bTemp & 0x01) 
	{
		szEdit2.Format(_T("Part Set"));
	}
	else 
	{
		szEdit2.Format(_T("Full Set"));
	}
	m_EditSpin2.SetWindowText(szEdit2);
}

void CDlgBindingAlarmPropGalaxy::displayObjMethodKeypad( )
{
	CString szEdit1;

	EnableControls(TRUE);

	BYTE bTemp = m_pBindingAlarmUnit->Get_Keypress();
	if (bTemp > 0x0D) bTemp = 0x0D;

	if ((bTemp >= 0) && (bTemp < 0x0A)) 
	{
		szEdit1.Format(_T("Key : %d"),bTemp);					
	}
	else if (bTemp == 0x0A) 
	{
		szEdit1.Format(_T("Full Set ('A')"));
	}
	else if (bTemp == 0x0B) 
	{
		szEdit1.Format(_T("Part Set ('B')"));
	}
	else if (bTemp == 0x0C) 
	{
		szEdit1.Format(_T("Delete ('C')"));
	}
	else if (bTemp == 0x0D) 
	{
		szEdit1.Format(_T("Enter ('M')"));
	}

	m_EditSpin1.SetWindowText(szEdit1);
}

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CDlgBindingAlarmPropAritech dialog

CDlgBindingAlarmPropAritech::CDlgBindingAlarmPropAritech(CWnd* pParent) :
Base( pParent )
{
	// TM_CHANGES_ALARM_ANB:
	// ANB heeft dezelfde implementatie als deze van Aritech maar is wel 
	// in de CDlgBindingAlarmPropAnbBibus klasse geimplementeerd.
}

void CDlgBindingAlarmPropAritech::GetZoneName(
	int zoneNR,
	int maxLength,
	char* szName)
{
	// TM_CHANGES_ALARM_ANB:
	// ANB heeft dezelfde implementatie als deze van Aritech maar is wel 
	// in de CDlgBindingAlarmPropAnbBibus klasse geimplementeerd.

	if (!CConfigAlarmZoneNameUtil::GetAlarmZoneName(
			CConfigAlarmZoneNameUtil::TYPE_ALARM_ARITECH,
			zoneNR,
			maxLength,
			szName))
	{
		sprintf(szName,"AlarmZone ?");
	}		
}

void CDlgBindingAlarmPropAritech::displayObjMethodArm( )
{
	displayObjMethodArmSimple(this);
}

void CDlgBindingAlarmPropAritech::displayObjMethodKeypad( )
{
	displayObjMethodKeypadSimple(this);
}

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CDlgBindingAlarmPropAnbBibus dialog

CDlgBindingAlarmPropAnbBibus::CDlgBindingAlarmPropAnbBibus(CWnd* pParent) :
Base( pParent )
{	
	// TM_CHANGES_ALARM_ANB:
	// Klasse heeft dezelfde implementatie als Aritech.
}

void CDlgBindingAlarmPropAnbBibus::GetZoneName(
	int zoneNR,
	int maxLength,
	char* szName)
{
	// TM_CHANGES_ALARM_ANB:
	// Klasse heeft dezelfde implementatie als Aritech.
	// Oppassen --> TYPE_ALARM_ANB

	if (!CConfigAlarmZoneNameUtil::GetAlarmZoneName(
			CConfigAlarmZoneNameUtil::TYPE_ALARM_ANB,
			zoneNR,
			maxLength,
			szName))
	{
		sprintf(szName,"AlarmZone ?");
	}		
}

void CDlgBindingAlarmPropAnbBibus::displayObjMethodArm( )
{
	displayObjMethodArmSimple(this);
}

void CDlgBindingAlarmPropAnbBibus::displayObjMethodKeypad( )
{
	displayObjMethodKeypadSimple(this);
}

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CDlgBindingAlarmPropTexecom dialog

CDlgBindingAlarmPropTexecom::CDlgBindingAlarmPropTexecom(CWnd* pParent) :
Base( pParent )
{	
	// TM_CHANGES_ALARM_TEXECOM:	
}

void CDlgBindingAlarmPropTexecom::GetZoneName(
	int zoneNR,
	int maxLength,
	char* szName)
{
	if (!CConfigAlarmZoneNameUtil::GetAlarmZoneName(
			CConfigAlarmZoneNameUtil::TYPE_ALARM_TEXECOM,
			zoneNR,
			maxLength,
			szName))
	{
		sprintf(szName,"AlarmZone ?");
	}		
}

void CDlgBindingAlarmPropTexecom::displayObjMethodArm( )
{
	CString szEdit1, szEdit2;

	EnableControls(TRUE,TRUE);

	BYTE bTemp = m_pBindingAlarmUnit->Get_GroupNr();
	bTemp &= 0x0F;
					
	szEdit1.Format(_T("%s"),_groupNR[bTemp]);
	m_EditSpin1.SetWindowText(szEdit1);

	bTemp = m_pBindingAlarmUnit->Get_ArmSet();
	if (bTemp & 0x01) 
	{
		szEdit2.Format(_T("Part Set"));
	}
	else 
	{
		szEdit2.Format(_T("Full Set"));
	}
	m_EditSpin2.SetWindowText(szEdit2);
}

void CDlgBindingAlarmPropTexecom::displayObjMethodKeypad( )
{
	CString szEdit1;

	EnableControls(TRUE);

	BYTE bTemp = m_pBindingAlarmUnit->Get_Keypress();
	if (bTemp > 0x0D) bTemp = 0x0D;

	if ((bTemp >= 0) && (bTemp < 0x0A)) 
	{
		szEdit1.Format(_T("Key : %d"),bTemp);					
	}
	else if (bTemp == 0x0A) 
	{
		szEdit1.Format(_T("On(A) Full"));
	}
	else if (bTemp == 0x0B) 
	{
		szEdit1.Format(_T("On(B) Part"));
	}
	else if (bTemp == 0x0C) 
	{
		szEdit1.Format(_T("Off/Clear"));
	}
	else if (bTemp == 0x0D) 
	{
		szEdit1.Format(_T("Enter"));
	}

	m_EditSpin1.SetWindowText(szEdit1);
}

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CDlgBindingAlarmPropIntegra dialog

CDlgBindingAlarmPropIntegra::CDlgBindingAlarmPropIntegra(CWnd* pParent) :
Base( pParent )
{	
	// TM_CHANGES_ALARM_INTEGRA	
}

void CDlgBindingAlarmPropIntegra::GetZoneName(
	int zoneNR,
	int maxLength,
	char* szName)
{
	if (!CConfigAlarmZoneNameUtil::GetAlarmZoneName(
			CConfigAlarmZoneNameUtil::TYPE_ALARM_INTEGRA,
			zoneNR,
			maxLength,
			szName))
	{
		sprintf(szName,"AlarmZone ?");
	}		
}



#if (ALARM_INTEGRA_USE_ARM_MODES==0)

void CDlgBindingAlarmPropIntegra::displayObjMethodArm( )
{
	displayObjMethodArmSimple(this);
}

void CDlgBindingAlarmPropIntegra::displayObjMethodKeypad( )
{
	displayObjMethodKeypadSimple(this);
}

#endif


#if (ALARM_INTEGRA_USE_ARM_MODES==1)

void CDlgBindingAlarmPropIntegra::displayObjMethodArm( )
{	
	CString szEdit1, szEdit2;

	EnableControls(TRUE,TRUE);

	BYTE bTemp = m_pBindingAlarmUnit->Get_GroupNr();
	bTemp &= 0x0F;
					
	szEdit1.Format(_T("%s"),_groupNR[bTemp]);
	m_EditSpin1.SetWindowText(szEdit1);

	bTemp = m_pBindingAlarmUnit->Get_ArmSet();

	if (bTemp>INTEGRA_ALARM_ARM_MODE3)
	{
		bTemp=INTEGRA_ALARM_ARM_MODE0;
	}
	szEdit2.Format(_T("Mode %d"),bTemp);

	m_EditSpin2.SetWindowText(szEdit2);
}

void CDlgBindingAlarmPropIntegra::OnSpin2DataChanged(int iStep)
{
	ASSERT(m_pBindingAlarmUnit);

	if (m_pBindingAlarmUnit->GetInOutType() == UNIT_TYPE_OUTPUT) 
	{			
		const BYTE bMethodData = m_pBindingAlarmUnit->Get_Function();
		
		if (bMethodData == ALARM_OBJECT_METHOD_ARM) 
		{	
			// Modes 0 tem 3
			int mode=m_pBindingAlarmUnit->Get_ArmSet()&0x03;

			mode += iStep;			
			if (mode<INTEGRA_ALARM_ARM_MODE0) 
			{
				mode=INTEGRA_ALARM_ARM_MODE0;
			}
			else if (mode>INTEGRA_ALARM_ARM_MODE3)
			{
				mode=INTEGRA_ALARM_ARM_MODE3;
			}

			ASSERT(mode>=INTEGRA_ALARM_ARM_MODE0 && mode<=INTEGRA_ALARM_ARM_MODE3);
			m_pBindingAlarmUnit->Set_ArmSet(mode);
		}
	}
	else
	{
		this->CDlgBindingAlarmProp::OnSpin2DataChanged(iStep);
	}
}


void CDlgBindingAlarmPropIntegra::displayObjMethodKeypad( )
{
	enum
	{
		INTEGRA_ALARM_KEYPAD_0      =0,
		INTEGRA_ALARM_KEYPAD_1      =1,
		INTEGRA_ALARM_KEYPAD_2      =2,
		INTEGRA_ALARM_KEYPAD_3      =3,	
		INTEGRA_ALARM_KEYPAD_4      =4,
		INTEGRA_ALARM_KEYPAD_5      =5,
		INTEGRA_ALARM_KEYPAD_6      =6,
		INTEGRA_ALARM_KEYPAD_7      =7,
		INTEGRA_ALARM_KEYPAD_8      =8,
		INTEGRA_ALARM_KEYPAD_9      =9,
		INTEGRA_ALARM_KEYPAD_ON		=10,	//0a
		INTEGRA_ALARM_KEYPAD_OFF	=11,	//0b
		INTEGRA_ALARM_KEYPAD_C      =12,	//0c
		INTEGRA_ALARM_KEYPAD_M      =13,	//0d
	};

	CString szEdit1;

	EnableControls(TRUE);

	BYTE bTemp = m_pBindingAlarmUnit->Get_Keypress();

	if (bTemp>INTEGRA_ALARM_KEYPAD_M) 
	{
		bTemp = INTEGRA_ALARM_KEYPAD_M;
	}

	if ((bTemp>=INTEGRA_ALARM_KEYPAD_0)&&
		(bTemp<INTEGRA_ALARM_KEYPAD_ON)) 
	{
		szEdit1.Format(_T("Key: %d"),bTemp);					
	}
	else if (bTemp==INTEGRA_ALARM_KEYPAD_ON) 
	{
		szEdit1.Format(_T("On"));
	}
	else if (bTemp==INTEGRA_ALARM_KEYPAD_OFF) 
	{
		szEdit1.Format(_T("Off"));
	}
	else if (bTemp==INTEGRA_ALARM_KEYPAD_C) 
	{
		szEdit1.Format(_T("Clear"));
	}
	else if (bTemp==INTEGRA_ALARM_KEYPAD_M)
	{
		szEdit1.Format(_T("Mode"));
	}

	m_EditSpin1.SetWindowText(szEdit1);
}
#endif

/////////////////////////////////////////////////////////////////////////////
