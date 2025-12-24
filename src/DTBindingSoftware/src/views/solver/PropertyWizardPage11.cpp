// PropertyWizardPage11.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "PropertyWizardPage11.h"

#include "PropertyWizard.h"

#include "app\applicactcode\applicactcodeapi.h"

// private
static 
BOOL parseCode(
   CEdit& refEditBox,
   unsigned long* pulCode)
{
	BOOL fResult = FALSE;
	
	char szTemp[256];

	if (refEditBox.GetWindowText(szTemp,255))
	{
		unsigned int val = 0;

		if (1 == sscanf(szTemp,"%x",&val))
		{
			*pulCode = (unsigned long) val;
			fResult = TRUE;
		}
	}
	return fResult;
}

/////////////////////////////////////////////////////////////////////////////////////
// private
// Dit is een dummy functie die aangeroepen wordt voor de controle van de code.
// Resultaat van deze functie is onbelangrijk.
static
BOOL CheckLicenceCode_Dummy(
	unsigned long ulCode)
{
	BOOL fResult = FALSE;
	unsigned long ulDecodedValue = 0;
	const unsigned long ulXORValue = 0x12345678;

	const BYTE HLSB = (BYTE)((ulCode&0xFF0000)>>16)^0xf;
	const BYTE LMSB = (BYTE)((ulCode&0x00FF00)>>8)^0xf;
	const BYTE LLSB = (BYTE) (ulCode&0xFF)^0xf;

	ulDecodedValue = LLSB;
	ulDecodedValue += (LMSB<<8);
	ulDecodedValue += (HLSB+(LLSB&0xF)+((LLSB&0x0F)>>8)+(LMSB&0xF)+((LMSB&0xF0)>>8))<<16;

	ulDecodedValue ^= ulXORValue;

	if (ulDecodedValue>=ulCode)
	{
		fResult=TRUE;
	}
	else
	{
		fResult=FALSE;
	}
	return fResult;
}

// De trampoline zorgt ervoor dat er extra functie calls gebeuren...
static 
BOOL trampoline(
   BOOL (*fptr)(unsigned long),
   unsigned long ulValue,
   BOOL fPing = FALSE)
{
	BOOL fResult=FALSE;

	if (fPing)
	{
		fResult = fptr(ulValue);
	}
	else
	{
		fResult = trampoline(fptr,ulValue,TRUE);
	}
	return fResult;
}
// Dummy code tot hier ...
//////////////////////////////////////////////////////////////////////


// CPropertyWizardPage11 dialog
IMPLEMENT_DYNAMIC(CPropertyWizardPage11, CPropertyPage)

CPropertyWizardPage11::CPropertyWizardPage11() :
CPropertyWizardPageBase((UINT) CPropertyWizardPage11::IDD, (UINT) 0, (UINT) IDS_HEADER_WIZARDPAGE11)
{

}

CPropertyWizardPage11::~CPropertyWizardPage11()
{
}

void CPropertyWizardPage11::DoDataExchange(CDataExchange* pDX)
{
	CPropertyWizardPageBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_CODE, m_editCode);
	DDX_Control(pDX, IDC_STATIC_TEXT1, m_staticInfo);
}


BEGIN_MESSAGE_MAP(CPropertyWizardPage11, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_SUBMIT, &CPropertyWizardPage11::OnBnClickedButtonSubmit)
END_MESSAGE_MAP()


// CPropertyWizardPage11 message handlers

BOOL CPropertyWizardPage11::OnInitDialog() 
{
	CPropertyWizardPageBase::OnInitDialog();


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CPropertyWizardPage11::OnWizardBack() 
{
	CPropertyWizard* const pSheet = (CPropertyWizard*) GetOwner();		
	if (pSheet != NULL) 
	{
		return(pSheet->PrevPage(WIZARDPAGE_ENTER_ACCESSCODE));
	}
	return FALSE;

}

LRESULT CPropertyWizardPage11::OnWizardNext() 
{
	CPropertyWizard* const pSheet = (CPropertyWizard*) GetOwner();		
	if (pSheet != NULL) 
	{
		return(pSheet->NextPage(WIZARDPAGE_ENTER_ACCESSCODE));
	}
	return FALSE;
}

BOOL CPropertyWizardPage11::OnWizardFinish() 
{
	return CPropertyWizardPageBase::OnWizardFinish();
}

BOOL CPropertyWizardPage11::OnSetActive() 
{			
	// CHANGES_ACTIVATION_LICENSE_CODE
	const BOOL fCheckCode = CAppLicActCodeAPI::GetLicenseActivated();
	UpdateControls(fCheckCode);

	return CPropertyWizardPageBase::OnSetActive();
}

void CPropertyWizardPage11::OnBnClickedButtonSubmit()
{
	// CHANGES_ACTIVATION_LICENSE_CODE

	BOOL fCheckCode = FALSE;
	unsigned long ulCode = 0;
	if (TRUE == parseCode(m_editCode,&ulCode))
	{	
		// Extra code toegevoegd om evt hacking van de code te bemoeilijken...
		// Een dummy functie aangeroepen via een trampoline functie.
		if (trampoline(&CheckLicenceCode_Dummy,ulCode))
		{
			fCheckCode=TRUE;		
		}
		else
		{
			fCheckCode=FALSE;
		}
		// Deze code doet niks tot hier...


		// Dit is de uiteindelijke controle...
		// Controle op licentie code.
		fCheckCode = CAppLicActCodeAPI::CheckLicenseCodeValid(ulCode);
		if (TRUE == fCheckCode)
		{
			// Code ok.
			CAppLicActCodeAPI::SetLicenseActivated( );	
		}
	}
	UpdateControls(fCheckCode);
}

void CPropertyWizardPage11::UpdateControls(BOOL fCodeOk)
{
	CPropertyWizard* const pSheet = (CPropertyWizard*) GetOwner();	
	if (pSheet) 
	{
		DWORD dwStyle = PSWIZB_BACK;
		const char* pszText = 0;

		if (fCodeOk)
		{
			dwStyle = PSWIZB_BACK | PSWIZB_FINISH;			
			pszText = "License activation code accepted, press finish";			
		}
		else
		{									
			pszText = "Enter license activation code and press submit button";		
		}

		// Update text
		if (pszText)
		{
			m_staticInfo.SetWindowText(pszText);
		}

		// Update Finish btn
		pSheet->SetWizardButtons(dwStyle);
	}
}

