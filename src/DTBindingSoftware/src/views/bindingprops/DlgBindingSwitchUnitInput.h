#pragma once

#include "dlgbindingswitchunit.h"


class CDlgBindingSwitchUnitInput :
	public CDlgBindingSwitchUnit
{
public:
	CDlgBindingSwitchUnitInput(CWnd* pParent = NULL);
	virtual ~CDlgBindingSwitchUnitInput(void);

	virtual void InitControls(void);
	void UpdateControls(void);

protected:
	BOOL LoadStringEntryResources(int Key);

private:
	typedef CDlgBindingSwitchUnit Base;
};