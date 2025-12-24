#include "stdafx.h"
#include "AppLicActCodeAPI.h"
#include "AppLicActCodeAPI2.h"

#include "AppLicActCode.h"
#include "AppLicActCodeNodeDbase.h"

#include "app\appnodedbase\appnodedbase.h"


/******************************************************************/
static CAppLicActCode _appLicActCode;
/******************************************************************/

/***************************** private ****************************/

// Oppassen: Gebruiken de runtime node database...
static inline
CNodeDatabase* getNodeDatabase(void)
{
	return CAppNodeDatabase::GetInstance()->GetNodesRuntime();
}

/******************************************************************/

/****************************** public ****************************/
BOOL CAppLicActCodeAPI::IsLicenseNeeded(void)
{
	BOOL fResult = FALSE;
	
	if (TRUE==CAppLicActCodeNodeDbase::CheckLicActCodeNeeded(getNodeDatabase()))
	{		
		if (TRUE==_appLicActCode.getFlagNeeded() && 
			FALSE==_appLicActCode.getFlagActivated())
		{
			fResult = TRUE;
		}
	}
	
	return fResult;	
}

void CAppLicActCodeAPI::SetLicenseActivated(void)
{
	_appLicActCode.setFlagActivated(TRUE);
}

BOOL CAppLicActCodeAPI::GetLicenseActivated(void)
{
	return _appLicActCode.getFlagActivated();
}

BOOL CAppLicActCodeAPI::CheckLicenseCodeValid(unsigned long ulCode)
{	
	BOOL fResult = FALSE;

	fResult = CAppLicActCodeNodeDbase::CheckLicenseCodeValid(getNodeDatabase(),ulCode);
	if (TRUE==fResult)
	{
		_appLicActCode.setCode(ulCode);	
	}
	return fResult;
}

int CAppLicActCodeAPI::FindNodesLicActCodeNeeded(
	CCanNode** ppNodes,
	int maxElements)
{	
	return CAppLicActCodeNodeDbase::FindNodesLicActCodeNeeded(getNodeDatabase(),ppNodes,maxElements);
}

// CAppLicActCodeAPI2

const CAppLicActCode& CAppLicActCodeAPI2::getLicActCode(void) 
{
	return _appLicActCode;
}

void CAppLicActCodeAPI2::setLicActCode(const CAppLicActCode& ref)
{
	_appLicActCode = ref;
}