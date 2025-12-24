#pragma once

class CFileIoData;

class CNodeDatabase;
class CBindingEntries;
class CAppLicActCode;

// Class CAppFileIoAPI

class CAppFileIoAPI
{
public:
	CAppFileIoAPI(CNodeDatabase *pNodeDatabase,
				  CBindingEntries *pBindings,
				  CAppLicActCode* pAppLicActCode);

	virtual ~CAppFileIoAPI();

	BOOL Save(CFile *f);
	BOOL Load(CFile *f);

private:
	CFileIoData* m_pImpl;
};
