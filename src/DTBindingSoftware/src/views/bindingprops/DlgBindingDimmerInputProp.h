#pragma once

#include "dlgbindingdimmerprop.h"


class CDlgBindingDimmerInputProp :
	public CDlgBindingDimmerProp
{
public:
	CDlgBindingDimmerInputProp(CWnd* pParent = NULL);
	virtual ~CDlgBindingDimmerInputProp(void);

	virtual void InitControls(void);
	// virtual void OnSpin1DataChanged(int iStep);
	void UpdateControls(void);

protected:
	BOOL LoadStringEntryResources(int Key);

private:
	typedef CDlgBindingDimmerProp Base;
};