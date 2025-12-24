#pragma once

#include "DlgUnitProp.h"

// CDlgEnergyMeasureProp dialog

class CDlgEnergyMeasureProp : public CDlgUnitProp
{
//	DECLARE_DYNAMIC(CDlgEnergyMeasureProp)

public:
	CDlgEnergyMeasureProp(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgEnergyMeasureProp();
	virtual void InitControls(void); // Sedert versie 0x0900 zonder params

// Dialog Data
	enum { IDD = IDD_DIALOG_PROP_UNIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void ShowFullDialog(BOOL fShow);
	virtual void CleanUp(void);
	virtual void RequestRuntimeProperties(void);

	DECLARE_MESSAGE_MAP()

private:
	BOOL m_fControlsCreated;
};
