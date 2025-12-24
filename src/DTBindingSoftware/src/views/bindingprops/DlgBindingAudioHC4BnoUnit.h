#pragma once
#include "dlgbindingaudiohc4unit.h"

class CDlgBindingAudioHC4BnoUnit :
	public CDlgBindingAudioHC4Unit
{
public:
	CDlgBindingAudioHC4BnoUnit(CWnd* pParent = NULL);
	virtual ~CDlgBindingAudioHC4BnoUnit(void);

	virtual void InitControls(void);
	virtual void OnSpin1DataChanged(int iStep);
	void UpdateControls(void);

private:
	typedef CDlgBindingAudioHC4Unit Base;
};
