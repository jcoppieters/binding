#include "stdafx.h"
#include "appfileioserclicactcode.h"

#include "app\appfileioser\appFileIoSer.h"
#include "App\appfileioser\AppFileIoSerLicActCode.h"

#include "app\applicactCode\applicActCode.h"


// public:

void CAppFileIoSerCLicActCode::update(
	CAppFileIoSer* p,
	const void* pvData)
{
	CAppFileIoSerLicActCode* const pDest = reinterpret_cast<CAppFileIoSerLicActCode*>(p);
	const CAppLicActCode* const pSrc = reinterpret_cast<const CAppLicActCode*>(pvData);

	ASSERT(p->getSerID()==APPFILEIOSER_ID_LICACTCODE);

	if (pDest)
	{
		pDest->setCode(pSrc->getCode());
		pDest->setFlags(pSrc->getFlags());
	}
}

void CAppFileIoSerCLicActCode::update(
	void* pvData,
	const CAppFileIoSer& r)
{
	CAppLicActCode* const pDest = reinterpret_cast<CAppLicActCode*>(pvData);
	const CAppFileIoSerLicActCode& refSrc = reinterpret_cast<const CAppFileIoSerLicActCode&>(r);

	ASSERT(r.getSerID()==APPFILEIOSER_ID_LICACTCODE);

	if (pDest)
	{
		pDest->setCode(refSrc.getCode());
		pDest->setFlags(refSrc.getFlags());
	}
}