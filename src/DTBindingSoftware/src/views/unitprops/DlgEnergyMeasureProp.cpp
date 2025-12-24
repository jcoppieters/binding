// DlgEnergyMeasureProp.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgEnergyMeasureProp.h"
#include "MyWindowMessages.h"
// ------------------------------------
#include "TMGlobals.h"
using namespace TMGraphics;
// ------------------------------------

// CDlgEnergyMeasureProp dialog

// IMPLEMENT_DYNAMIC(CDlgEnergyMeasureProp, CDlgUnitProp)

CDlgEnergyMeasureProp::CDlgEnergyMeasureProp(CWnd* pParent /*=NULL*/)
	: CDlgUnitProp(pParent), m_fControlsCreated(FALSE)
{

}

CDlgEnergyMeasureProp::~CDlgEnergyMeasureProp()
{
}

void CDlgEnergyMeasureProp::DoDataExchange(CDataExchange* pDX)
{
	CDlgUnitProp::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgEnergyMeasureProp, CDlgUnitProp)
END_MESSAGE_MAP()


// CDlgEnergyMeasureProp message handlers
void CDlgEnergyMeasureProp::ShowFullDialog(BOOL fShow)
{
	CDlgUnitProp::ShowFullDialog(fShow);
}


void CDlgEnergyMeasureProp::CleanUp(void)
{

}

void CDlgEnergyMeasureProp::InitControls(void) 
{
	// this->CDlgUnitProp::InitControls( ); // Sedert versie 0x0900 zonder params

	RequestRuntimeProperties();	
	UpdateRuntimeProperties();
	if (fChildCreated == FALSE) {
		CalculateDialogCoordinates();
		fChildCreated = TRUE;
	}
	ShowFullDialog(fShowBindingProperties);
	UpdateData(FALSE);
}

void CDlgEnergyMeasureProp::RequestRuntimeProperties()
{
	STRUCT_UNIT* const pUnit = getAssignedUnit();
	if (0 != pUnit)
	{
		const BYTE bNodeAddress = pUnit->bNodeLogicalAddress;
		const BYTE bUnitAddress = pUnit->bUnitAddress;

		CTXEnergyMeasureUnitMessage* const pTXUnitMsg = 
			static_cast<CTXEnergyMeasureUnitMessage*>(TMGetGuiUnit()->GetMessagePtr());

		ASSERT(0 != pTXUnitMsg);

		if(0 != pTXUnitMsg)
		{		
			pTXUnitMsg->RequestConfig(bNodeAddress,bUnitAddress);
			pTXUnitMsg->RequestSupportedFlags(bNodeAddress,bUnitAddress);
			pTXUnitMsg->RequestSampleInterval(bNodeAddress,bUnitAddress);
			pTXUnitMsg->RequestState(bNodeAddress,bUnitAddress);

			// Kunnen ifv de suppported flags bepalen welke gegevens we gaan opvragen...
			// Moeten dit wel via een timer doen wnr we weten welke beschikbaar zijn.
		}
	}
}
