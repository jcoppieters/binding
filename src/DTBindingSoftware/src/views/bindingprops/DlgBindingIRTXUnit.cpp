// DlgBindingIRTXUnit.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgBindingIRTXUnit.h"
#include "Mainfrm.h"

#include "Nodemess.h"
#include "bindingUIElements.h"

#include "app\appbindings\appbindingunit\bindingunitirtx.h"
#include "app\appbindings\appbindingunit\bindingunitirtx_def.h"

#include "App\AppCfg\AppCfgIRTX\ConfigIRTXFileInfo.h"
#include "App\AppCfg\AppCfgIRTX\ConfigIRTXFileInfo_util.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
using namespace DUOTECNO::BINDINGS::UI;
/////////////////////////////////////////////////////////////////////////////
// CDlgBindingIRTXUnit dialog


CDlgBindingIRTXUnit::CDlgBindingIRTXUnit(CWnd* pParent /*=NULL*/)
	: CDlgBindingUnitProp(pParent),
	  m_fReloadIRTXFile(TRUE)
{
	//{{AFX_DATA_INIT(CDlgBindingIRTXUnit)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	pBindingIRTXUnit = 0;
	m_pStringEntries = 0;
}


void CDlgBindingIRTXUnit::DoDataExchange(CDataExchange* pDX)
{
	CDlgBindingUnitProp::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgBindingIRTXUnit)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgBindingIRTXUnit, CDlgBindingUnitProp)
	//{{AFX_MSG_MAP(CDlgBindingIRTXUnit)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgBindingIRTXUnit message handlers
void CDlgBindingIRTXUnit::InitControls(void)
{	
	pBindingIRTXUnit = (CBindingIRTXUnit*) GetBindingUnit();
	ASSERT(pBindingIRTXUnit);

	CBindingStringEntries_Col stringCol;
	m_pStringEntries = stringCol.Get(pBindingIRTXUnit,CBindingStringEntries_Col::IRTX_UNITS);

	CBindingStringEntries_Helper Helper(m_pStringEntries);
	Helper.Insert(m_CmbMethod);
	m_CmbMethod.ShowWindow(TRUE);

	m_fReloadIRTXFile = TRUE;	/* TM 20120830 */

	UpdateControls();
}

void CDlgBindingIRTXUnit::UpdateControls(void)
{
	ASSERT(pBindingIRTXUnit);
	ASSERT(m_pStringEntries);
	
	const BYTE bMethodData = pBindingIRTXUnit->Get_Function();
	CBindingStringEntries_Helper Helper(m_pStringEntries);
	Helper.SetCurrSel(m_CmbMethod, bMethodData);
	
	switch(bMethodData)
	{
		case IRTX_OBJECT_METHOD_PULS_IRTX:
		case IRTX_OBJECT_METHOD_START_IRTX:
		case IRTX_OBJECT_METHOD_STOP_IRTX:
		{
			CString szEdit1;
			EnableControls(TRUE);

			/* TM 20120830 */
			CConfigIRTXFileInfo_Util::getFunctionName(
				CConfigIRTXFileInfo::getInstance(),
				pBindingIRTXUnit->Get_IRCode(),
				&szEdit1,
				m_fReloadIRTXFile);

			m_EditSpin1.SetWindowText(szEdit1);
			m_fReloadIRTXFile = FALSE;				// eenmaal ingeladen 

		} break;
	}	
}

/////////////////////////////////////////////////////////////////////////////

void CDlgBindingIRTXUnit::OnCmbBoxDataChanged(void)
{
	ASSERT(pBindingIRTXUnit);
	ASSERT(m_pStringEntries);

	CBindingStringEntries_Helper Helper(m_pStringEntries);
	const BYTE bMethodData = Helper.GetCurrSelData(m_CmbMethod);

	if (bMethodData == 0xFF)
		pBindingIRTXUnit->Set_Function(IRTX_OBJECT_METHOD_PULS_IRTX);
	else
		pBindingIRTXUnit->Set_Function(bMethodData);
}

void CDlgBindingIRTXUnit::OnSpin1DataChanged(int iStep)
{
	ASSERT(pBindingIRTXUnit);
	
	const BYTE bMethodData = pBindingIRTXUnit->Get_Function();
	switch(bMethodData)
	{
		case IRTX_OBJECT_METHOD_PULS_IRTX:
		case IRTX_OBJECT_METHOD_START_IRTX:
		case IRTX_OBJECT_METHOD_STOP_IRTX:	
		{
			unsigned short usIRCode = pBindingIRTXUnit->Get_IRCode();

			if (iStep > 0) 
			{
				usIRCode += iStep;
				if (usIRCode > IRCODE_MAX_ENTRIES) 
					usIRCode = IRCODE_MAX_ENTRIES;
			}
			else
			{
				usIRCode += iStep;
				if (usIRCode > IRCODE_MAX_ENTRIES) 
					usIRCode = 0;
			}

			pBindingIRTXUnit->Set_IRCode(usIRCode);

		} break;
	}
}
