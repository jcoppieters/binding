#pragma once
#include "dlgbindingavmatrixprop.h"

class CDlgBindingAVMatrixInput :
	public CDlgBindingAVMatrixProp
{
public:
	CDlgBindingAVMatrixInput(CWnd* pParent = NULL);
	virtual ~CDlgBindingAVMatrixInput(void);

	virtual void InitControls(void);
	virtual void OnSpin1DataChanged(int iStep);
	void UpdateControls(void);

private:
	typedef CDlgBindingAVMatrixProp Base;
};
