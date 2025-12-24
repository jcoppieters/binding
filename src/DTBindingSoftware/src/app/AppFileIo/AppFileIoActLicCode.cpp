#include "stdafx.h"
#include "appfileioactliccode.h"

#include "app\appfileioser\appfileioserlicactcode.h"
#include "app\appfileioserc\appfileioserclicactcode.h"

BOOL CAppFileIoActLicCode::Save(CFile* f,const CAppLicActCode& r)
{
	BOOL fResult = FALSE;
	CAppFileIoSerLicActCode serClass;
	CAppFileIoSerCLicActCode serConvertClass;

	serConvertClass.update(&serClass,&r);
	fResult = serClass.Save(f);

	return fResult;
}

BOOL CAppFileIoActLicCode::Load(CFile* f,CAppLicActCode* p)
{
	BOOL fResult = FALSE;
	CAppFileIoSerLicActCode serClass;
	CAppFileIoSerCLicActCode serConvertClass;

	if (serClass.Load(f))
	{
		serConvertClass.update(p,serClass);
		fResult = TRUE;
	}
	return fResult;
}
