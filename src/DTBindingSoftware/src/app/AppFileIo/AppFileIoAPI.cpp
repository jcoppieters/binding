#include "stdafx.h"
#include "appfileioapi.h"

#include "fileiodata.h"

// Class CAppFileIoAPI

CAppFileIoAPI::CAppFileIoAPI(CNodeDatabase *pNodeDatabase,CBindingEntries *pBindings,CAppLicActCode* pAppLicActCode):
	m_pImpl(new CFileIoData(pNodeDatabase,pBindings,pAppLicActCode)) { }

CAppFileIoAPI::~CAppFileIoAPI( )
{
	if (m_pImpl) {
		delete m_pImpl;
		m_pImpl = 0;
	}
}

BOOL CAppFileIoAPI::Save(CFile *f)
{
	BOOL fResult = FALSE;
	if (m_pImpl)
	{
		fResult = m_pImpl->Save(f);
	}
	return fResult;
}

BOOL CAppFileIoAPI::Load(CFile *f)
{
	BOOL fResult = FALSE;
	if (m_pImpl)
	{
		fResult = m_pImpl->Load(f);
	}
	return fResult;
}
