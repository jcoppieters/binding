#pragma once

class CAppLicActCode;

// Class CAppFileIoActLicCode
class CAppFileIoActLicCode
{
public:	
	static BOOL Save(CFile* f,const CAppLicActCode& r);
	static BOOL Load(CFile* f,CAppLicActCode* p);
};
