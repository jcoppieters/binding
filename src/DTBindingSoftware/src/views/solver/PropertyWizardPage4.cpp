// PropertyWizardPage4.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "PropertyWizardPage4.h"
#include "PropertyWizard.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropertyWizardPage4 property page

IMPLEMENT_DYNCREATE(CPropertyWizardPage4, CPropertyPage)	

CPropertyWizardPage4::CPropertyWizardPage4() : 
CPropertyWizardPageBase((UINT) CPropertyWizardPage4::IDD, (UINT) 0, (UINT) IDS_HEADER_WIZARDPAGE4)
{
	//{{AFX_DATA_INIT(CPropertyWizardPage4)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CPropertyWizardPage4::~CPropertyWizardPage4()
{
}

void CPropertyWizardPage4::DoDataExchange(CDataExchange* pDX)
{
	CPropertyWizardPageBase::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropertyWizardPage4)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropertyWizardPage4, CPropertyWizardPageBase)		// CHANGES_MVS_2008 : Afleiden van CPropertyPage
	//{{AFX_MSG_MAP(CPropertyWizardPage4)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertyWizardPage4 message handlers

BOOL CPropertyWizardPage4::OnInitDialog() 
{
	CPropertyWizardPageBase::OnInitDialog();
	return TRUE;
}

BOOL CPropertyWizardPage4::OnSetActive() 
{
	DWORD dwStyle;
	CWnd *pWnd = NULL;
	CString szText;
	szText.Empty();

	CPropertyWizard *pSheet = NULL;
	pSheet = (CPropertyWizard*) GetOwner();
	if (pSheet != NULL) 
	{
		if (pSheet->IsLastPage(WIZARDPAGE_CHOICE_PNP))
		{
			dwStyle = PSWIZB_FINISH | PSWIZB_BACK;		
		}
		else 
		{
			dwStyle = PSWIZB_NEXT | PSWIZB_BACK;
			szText.Format("When finished press continue for the next step.");
			//szText.Format("To proceed press Next");
		}
		
		pSheet->SetWizardButtons(dwStyle);
	}

	pWnd = GetDlgItem(IDC_STATIC_TEXT2);
	if (pWnd != NULL) pWnd->SetWindowText(szText);

	szText.Format("Assign addresses to new devices on the CAN-bus via plug and play.\n"
				  "Use this option when new devices were added to the CAN-bus.\n\n"
				  "Devices with a factory address will be assigned an available address,"				  
				  "other devices will keep there address.\n\n"
				  "Make sure all CAN-bus devices are powered on and connected to the CAN-bus.\n\n"
				  "Press finished to start the automatic plug and play.");
					
	pWnd = GetDlgItem(IDC_STATIC_TEXT);
	if (pWnd != NULL) pWnd->SetWindowText(szText);

	return CPropertyWizardPageBase::OnSetActive();
}

LRESULT CPropertyWizardPage4::OnWizardBack() 
{
	CPropertyWizard *pSheet = NULL;
	pSheet = (CPropertyWizard*) GetOwner();
	
	if (pSheet != NULL) 
	{
		return(pSheet->PrevPage(WIZARDPAGE_CHOICE_PNP));
	}
	return FALSE;
}

LRESULT CPropertyWizardPage4::OnWizardNext() 
{
	CPropertyWizard *pSheet = NULL;
	pSheet = (CPropertyWizard*) GetOwner();
	
	if (pSheet != NULL) 
	{
		return(pSheet->NextPage(WIZARDPAGE_CHOICE_PNP));
	}
	return CPropertyWizardPageBase::OnWizardNext();
}

BOOL CPropertyWizardPage4::OnWizardFinish() 
{
	CPropertyWizard *pSheet = NULL;
	pSheet = (CPropertyWizard*) GetOwner();
	if (pSheet != NULL) 
	{
		pSheet->SetAutoPnp();
	}
	return CPropertyWizardPageBase::OnWizardFinish();
}
