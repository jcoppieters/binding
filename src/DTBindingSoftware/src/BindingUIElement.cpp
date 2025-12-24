#include "stdafx.h"
#include "bindingUIElement.h"

#include "BindingSoftware.h"
#include "nodemess.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



/********************************************************************************************************/
using namespace DUOTECNO::BINDINGS::UI;
/********************************************************************************************************/

const CStringEntries* CStringEntries_Collection_alg::Get(int key) const
{
	if (key >= MAX_ELEMENTS) return 0;
	return (m_pEntries[key]);	
}

const CStringEntries* CStringEntries_Collection_alg::Get(int extended,  int basic, 
														 BYTE bMethodData) const
{
	const CStringEntries* const pBasic = Get(basic);
	if (pBasic != 0)
	{
		if (pBasic->Find(bMethodData)) 
			return pBasic;		
	}
	return(Get(extended));
}

const CStringEntries* CStringEntries_Collection_alg::Get(int extended, int basic, int simple,
	 													 BYTE bMethodData) const
{
	int Keys[3] = {simple, basic, extended};

	for (int i=0;i<3;i++)
	{
		const CStringEntries* const pKey = Get(Keys[i]);	
		if (pKey != 0 && pKey->Find(bMethodData)) 
			return pKey;
	}
	return (Get(extended));
}


/********************************************************************************************************/
const CBindingStringEntries_Collection* CBindingStringEntries_Collection::Instance() 
{
	static CBindingStringEntries_Collection _Instance;
	return (&_Instance);
}

CBindingStringEntries_Collection::CBindingStringEntries_Collection() : 
	Algoritme(&m_Entries[0],MAX_COLLECTION)
{
	int i;

	for (i=0;i<MAX_COLLECTION;i++) m_Entries[i] = 0;

	m_Entries[BINDINGFLAGS_BASIC] = &m_BindingFlagsBasic;
	m_Entries[BINDINGFLAGS_EXTENDED] = &m_BindingFlagsExtended;

	for (i=0;i<MAX_COLLECTION;i++) ASSERT(m_Entries[i]);
}

/********************************************************************************************************/
CBindingUnitStringEntries_Collection* CBindingUnitStringEntries_Collection::Instance() 
{
	static CBindingUnitStringEntries_Collection _Instance;
	return (&_Instance);
}

CBindingUnitStringEntries_Collection::CBindingUnitStringEntries_Collection() : 
	Algoritme(reinterpret_cast<const CStringEntries* const*>(&m_Entries[0]),MAX)
{
	int i;

	for (i=0;i<MAX;i++) 
	{
		m_Entries[i] = 0;
	}

	m_Entries[DIMMER_STRINGS_BASIC] = &m_DimmerBasic;
	m_Entries[DIMMER_STRINGS_EXTENDED] = &m_DimmerExtended;
	m_Entries[DIMMER_STRINGS_SIMPLE] = &m_DimmerSimple;
	m_Entries[DIMMER_STRINGS_INPUT] = &m_DimmerInput;			// sedert versie 0x0B01
	m_Entries[DUOSWITCH_STRINGS_BASIC] = &m_DuoswitchBasic;
	m_Entries[DUOSWITCH_STRINGS_EXTENDED] = &m_DuoswitchExtended;
	m_Entries[DUOSWITCH_STRINGS_INPUT] =&m_DuoSwitchInput;		// sedert versie 0x0C01
	m_Entries[SWITCH_STRINGS_BASIC] = &m_SwitchBasic;
	m_Entries[SWITCH_STRINGS_EXTENDED] = &m_SwitchExtended;
	m_Entries[SWITCH_STRINGS_INPUT] = &m_SwitchInput;			// sedert versie 0x0C01
	m_Entries[SENS_STRINGS_BASIC] = &m_SensBasic;
	m_Entries[SENS_STRINGS_EXTENDED] = &m_SensExtended;
	m_Entries[SENS_STRINGS_INPUT] = &m_SensInput;
	m_Entries[SENSUNIV_STRINGS_INPUT] = &m_SensUnivInput;		// sedert versie 0x0D00
	m_Entries[IRTX_STRINGS_BASIC] = &m_IRTXBasic;
	m_Entries[IRTX_STRINGS_EXTENDED] = &m_IRTXExtended;
	m_Entries[BOSELF48_STRINGS_BASIC] = &m_BoseLf48Basic;
	m_Entries[BOSELF48_STRINGS_EXTENDED] = &m_BoseLf48Extended;
	m_Entries[BOSELF48_STRINGS_SIMPLE] = &m_BoseLf48Simple;
	m_Entries[BOSELF48_INPUTSTRINGS_BASIC] = &m_BoseLf48InputBasic;
	m_Entries[BOSELF48_INPUTSTRINGS_EXTENDED] = &m_BoseLf48InputExtended;
	m_Entries[BOSELF48_INPUTSTRINGS_VIRTUALS] = &m_BoseLf48InputVirtuals;
	m_Entries[AVMATRIX_STRINGS_EXTENDED] = &m_AvMatrixExtended;
	m_Entries[AVMATRIX_STRINGS_BASIC] = &m_AvMatrixBasic;
	m_Entries[AVMATRIX_INPUTSTRINGS_BASIC] = &m_AvMatrixInputBasic;			/* TM_AVMATRIX_INPUT */
	m_Entries[VIDEOMUX_STRINGS_EXTENDED] = &m_VideomuxExtended;
	m_Entries[VIDEOMUX_STRINGS_BASIC] = &m_VideomuxBasic;
	m_Entries[AUDIOHC4_STRINGS_SIMPLE] = &m_AudioHC4Simple;
	m_Entries[AUDIOHC4_STRINGS_BASIC] = &m_AudioHC4Basic;
	m_Entries[AUDIOHC4_STRINGS_EXTENDED] = &m_AudioHC4Extended;
	m_Entries[AUDIOHC4_INPUTSTRINGS_BASIC] = &m_AudioHC4InputBasic;			/* TM_AUDIOHC4_BINDING_INPUT */	
	m_Entries[AUDIOHC4_INPUTSTRINGS_BNO_BASIC] = &m_AudioHC4BnoInputBasic;	/* TM_AUDIOHC4_BNO */	
	m_Entries[BOSEV35_STRINGS_BASIC] = &m_BoseV35Basic;
	m_Entries[BOSEV35_STRINGS_EXTENDED] = &m_BoseV35Extended;
	m_Entries[BOSEV35_STRINGS_SIMPLE] = &m_BoseV35Simple;
	m_Entries[BOSEV35_STRINGS_VIRTUALS] = &m_BoseV35Virtuals;
	m_Entries[BOSEV35_INPUTSTRINGS_BASIC] = &m_BoseV35InputBasic;
	m_Entries[BOSEV35_INPUTSTRINGS_EXTENDED] = &m_BoseV35InputExtended;
	m_Entries[BOSEV35_INPUTSTRINGS_VIRTUALS] = &m_BoseV35InputVirtuals;
	// TM_CHANGES_BINDINGS_ALARM
	m_Entries[ALARM_OUTPUTSTRING]=&m_AlarmOutputs;
	m_Entries[ALARM_INPUTSTRINGS_BASIC]=&m_AlarmInputBasic;
	m_Entries[ALARM_INPUTSTRINGS_EXTENDED]=&m_AlarmInputExtended;

#ifdef _DEBUG
	for (i=0;i<MAX;i++) 
	{
		ASSERT(m_Entries[i]);
	}
#endif
}


const CBindingStringEntries* CBindingUnitStringEntries_Collection::Get(COLLECTION_KEY key) const
{
	return (static_cast<const CBindingStringEntries* const>(Algoritme.Get(key)));
}

const CBindingStringEntries* CBindingUnitStringEntries_Collection::Get(COLLECTION_KEY extended, 
																   COLLECTION_KEY basic, 
																   BYTE bMethodData) const
{
	return (static_cast<const CBindingStringEntries* const>(Algoritme.Get(extended,basic,bMethodData)));
}

const CBindingStringEntries* CBindingUnitStringEntries_Collection::Get(COLLECTION_KEY extended, 															
																   COLLECTION_KEY basic,
																   COLLECTION_KEY simple,
																   BYTE bMethodData) const
{
	return (static_cast<const CBindingStringEntries* const>(Algoritme.Get(extended,basic,simple,bMethodData)));
}


/********************************************************************************************************/

void CBindingStringEntries_Helper::Insert(CComboBox& comboBox)
{
	comboBox.ResetContent();

	if (m_pStringEntries != 0)
	{
		const int Max = m_pStringEntries->NrOfEntries();
		
		for (int i=0;i<Max;i++)
		{			
			const CStringEntry* const pEntry = m_pStringEntries->Get(i);
			if (pEntry != 0)
			{
				CString szString;
				
				pEntry->Get(szString);
				comboBox.InsertString(i,szString);
				comboBox.SetItemData(i,pEntry->GetValue());
			}
		}
	}
}

BYTE CBindingStringEntries_Helper::GetCurrSelData(CComboBox& comboBox)
{
	const int nCurrSel = comboBox.GetCurSel();
	BYTE bData = 0xFF;

	if (nCurrSel != -1)
	{
		bData = static_cast<BYTE>(comboBox.GetItemData(nCurrSel));
	}
	return bData;
}

void CBindingStringEntries_Helper::SetCurrSel(CComboBox& comboBox,BYTE bMethodData)
{
	const int Max = comboBox.GetCount();
	int selection = -1;

	for (int i=0;i<Max;i++)
	{
		if (comboBox.GetItemData(i) == bMethodData)
		{
			selection = i;		
			break;
		}
	}
	comboBox.SetCurSel(selection);
}

void CBindingFlagControl_Helper::Update(CEdit& editBox,BYTE bBindingFlags)
{
	CString szText("UNDEFINED");

	if (m_pStringEntries != 0)
	{
		const int Max = m_pStringEntries->NrOfEntries();
		
		for (int i=0;i<Max;i++)
		{	
			const CStringEntry* const pEntry = m_pStringEntries->Get(i);

			if (pEntry->GetValue() == bBindingFlags)
			{
				pEntry->Get(szText);
				break;
			}				
		}	
	}
	editBox.SetWindowText(szText);
}

void CBindingFlagControl_Helper::Adjust(BYTE& bBindingFlags,int iStep)
{
	const int Max = m_pStringEntries->NrOfEntries();

	if (iStep > 0)
	{
		if (bBindingFlags < Max-1) 
			bBindingFlags++;
	}
	else 
	{
		if (bBindingFlags > 0) 
			bBindingFlags--;
	}
}


