#pragma once

#include "dlgbindingduoswitchunit.h"


class CDlgBindingDuoSwitchUnitInput :
	public CDlgBindingDuoSwitchUnit
{
public:
	CDlgBindingDuoSwitchUnitInput(CWnd* pParent = NULL);
	virtual ~CDlgBindingDuoSwitchUnitInput(void);
	virtual void OnCmbBoxDataChanged(void);

	virtual void InitControls(void);
	void UpdateControls(void);

protected:
	BOOL LoadStringEntryResources(int Key);

private:
	typedef CDlgBindingDuoSwitchUnit Base;
};