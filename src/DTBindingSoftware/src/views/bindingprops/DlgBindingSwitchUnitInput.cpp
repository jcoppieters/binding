#include "stdafx.h"
#include "BindingSoftware.h"
#include "dlgbindingswitchunitinput.h"


#include "Mainfrm.h"

#include "Nodemess.h"
#include "app\appbindings\appbindingunit\bindingunitrelais.h"

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

CDlgBindingSwitchUnitInput::CDlgBindingSwitchUnitInput(CWnd* pParent /*=NULL*/)
	: Base(pParent)
{

}

CDlgBindingSwitchUnitInput::~CDlgBindingSwitchUnitInput(void)
{

}

BOOL CDlgBindingSwitchUnitInput::LoadStringEntryResources(int key)
{
	CBindingStringEntries_Col stringCol;
	m_pStringEntries = 
		stringCol.Get(getBindingSwitchUnit(),
					  (CBindingStringEntries_Col::COLLECTION_KEY_UNITTYPE)key);

	return (m_pStringEntries == 0 ? FALSE : TRUE);
}


void CDlgBindingSwitchUnitInput::InitControls(void)
{
	if (RetrieveBindingUnit())
	{		
		if (getBindingSwitchUnit()->GetInOutType() == UNIT_TYPE_OUTPUT)
		{
			this->Base::InitControls( );
		}
		else
		{
			const CBindingStringEntries_Col::COLLECTION_KEY_UNITTYPE key = CBindingStringEntries_Col::SWITCH_UNITS_INPUT;

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

void CDlgBindingSwitchUnitInput::UpdateControls(void)
{
	if (0!=getBindingSwitchUnit())
	{
		if (getBindingSwitchUnit()->GetInOutType() == UNIT_TYPE_OUTPUT)
		{
			this->Base::UpdateControls();
		}
		else
		{
			CBindingRelaisUnitInput* const pBindingRelaisInputUnit=
				static_cast<CBindingRelaisUnitInput*>(getBindingSwitchUnit());

			if (0!=pBindingRelaisInputUnit)
			{
				const BYTE bMethodData = pBindingRelaisInputUnit->Get_Event();

				EnableControls( );

				CBindingStringEntries_Helper Helper(getStringEntries());		
				Helper.SetCurrSel(m_CmbMethod,bMethodData);
			}
		}
	}
}