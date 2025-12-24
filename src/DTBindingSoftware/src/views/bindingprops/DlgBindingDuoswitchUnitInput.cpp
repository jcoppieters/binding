#include "stdafx.h"
#include "BindingSoftware.h"
#include "dlgbindingduoswitchunitinput.h"


#include "Mainfrm.h"

#include "Nodemess.h"
#include "app\appbindings\appbindingunit\bindingunitduoswitch.h"

#include "BindingUIElements.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
using namespace DUOTECNO::BINDINGS::UI;
/////////////////////////////////////////////////////////////////////////////
// CDlgBindingDimmerInputProp dialog

CDlgBindingDuoSwitchUnitInput::CDlgBindingDuoSwitchUnitInput(CWnd* pParent /*=NULL*/)
	: Base(pParent)
{

}

CDlgBindingDuoSwitchUnitInput::~CDlgBindingDuoSwitchUnitInput(void)
{

}

BOOL CDlgBindingDuoSwitchUnitInput::LoadStringEntryResources(int key)
{
	CBindingStringEntries_Col stringCol;
	m_pStringEntries = 
		stringCol.Get(getBindingDuoSwitchUnit(),
					  (CBindingStringEntries_Col::COLLECTION_KEY_UNITTYPE)key);

	return (m_pStringEntries == 0 ? FALSE : TRUE);
}


void CDlgBindingDuoSwitchUnitInput::InitControls(void)
{
	if (RetrieveBindingUnit())
	{		
		if (getBindingDuoSwitchUnit()->GetInOutType() == UNIT_TYPE_OUTPUT)
		{
			this->Base::InitControls( );
		}
		else
		{
			const CBindingStringEntries_Col::COLLECTION_KEY_UNITTYPE key = 
				CBindingStringEntries_Col::DUOSWITCH_UNITS_INPUT;

			if (LoadStringEntryResources(key))
			{
				CBindingStringEntries_Helper Helper(getStringEntries());
				Helper.Insert(m_CmbMethod);

				m_CmbMethod.ShowWindow(TRUE);
			}

			UpdateControls();	
		}	
	}	
}

void CDlgBindingDuoSwitchUnitInput::UpdateControls(void)
{
	if (0!=getBindingDuoSwitchUnit())
	{
		if (getBindingDuoSwitchUnit()->GetInOutType() == UNIT_TYPE_OUTPUT)
		{
			this->Base::UpdateControls();
		}
		else
		{
			CBindingDuoSwitchUnitInput* const pBindingDuoSwitchInputUnit=
				static_cast<CBindingDuoSwitchUnitInput*>(getBindingDuoSwitchUnit());

			if (0!=pBindingDuoSwitchInputUnit)
			{
				const BYTE bMethodData = pBindingDuoSwitchInputUnit->Get_Event();

				EnableControls( );

				CBindingStringEntries_Helper Helper(getStringEntries());		
				Helper.SetCurrSel(m_CmbMethod,bMethodData);
			}
		}
	}
}

void CDlgBindingDuoSwitchUnitInput::OnCmbBoxDataChanged(void)
{
	if (0!=getBindingDuoSwitchUnit())
	{
		if (getBindingDuoSwitchUnit()->GetInOutType() == UNIT_TYPE_OUTPUT)
		{
			this->Base::OnCmbBoxDataChanged();
		}
		else
		{
			CBindingStringEntries_Helper Helper(getStringEntries());

			CBindingDuoSwitchUnitInput* const pBindingDuoSwitchInputUnit=
					static_cast<CBindingDuoSwitchUnitInput*>(getBindingDuoSwitchUnit());

			if (0!=pBindingDuoSwitchInputUnit)
			{
				const BYTE bMethodData = Helper.GetCurrSelData(m_CmbMethod);

				if (bMethodData == 0xFF)
					pBindingDuoSwitchInputUnit->Set_Event(EV_UNITCONTROLPULSTOGGLE);
				else
					pBindingDuoSwitchInputUnit->Set_Event(bMethodData);
			}
		}
	}
}