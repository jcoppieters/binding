#include "stdafx.h"
#include "BindingSoftware.h"
#include "dlgbindingdimmerinputprop.h"


#include "Mainfrm.h"

#include "Nodemess.h"
#include "app\appbindings\appbindingunit\bindingunitdim.h"

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

CDlgBindingDimmerInputProp::CDlgBindingDimmerInputProp(CWnd* pParent /*=NULL*/)
	: Base(pParent)
{

}

CDlgBindingDimmerInputProp::~CDlgBindingDimmerInputProp(void)
{

}

BOOL CDlgBindingDimmerInputProp::LoadStringEntryResources(int key)
{
	CBindingStringEntries_Col stringCol;
	m_pStringEntries = 
		stringCol.Get(getBindingDimmerUnit(),
					  (CBindingStringEntries_Col::COLLECTION_KEY_UNITTYPE)key);

	return (m_pStringEntries == 0 ? FALSE : TRUE);
}

void CDlgBindingDimmerInputProp::InitControls(void)
{
	if (RetrieveBindingUnit())
	{		
		if (getBindingDimmerUnit()->GetInOutType() == UNIT_TYPE_OUTPUT)
		{
			this->Base::InitControls( );
		}
		else
		{
			const CBindingStringEntries_Col::COLLECTION_KEY_UNITTYPE key = CBindingStringEntries_Col::DIMMER_UNITS_INPUT;

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

void CDlgBindingDimmerInputProp::UpdateControls(void)
{
	if (0!=getBindingDimmerUnit())
	{
		if (getBindingDimmerUnit()->GetInOutType() == UNIT_TYPE_OUTPUT)
		{
			this->Base::UpdateControls();
		}
		else
		{
			CBindingDimmerUnitInput* const pBindingDimmerInputUnit=
				static_cast<CBindingDimmerUnitInput*>(getBindingDimmerUnit());

			if (0!=pBindingDimmerInputUnit)
			{
				const BYTE bMethodData = pBindingDimmerInputUnit->Get_Event();

				EnableControls( );

				CBindingStringEntries_Helper Helper(getStringEntries());		
				Helper.SetCurrSel(m_CmbMethod,bMethodData);
			}
		}
	}
}


