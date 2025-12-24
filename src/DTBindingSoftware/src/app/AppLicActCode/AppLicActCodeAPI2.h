#pragma once

#include "applicactcodeapi.h"

class CAppLicActCode;

class CAppLicActCodeAPI2 : public CAppLicActCodeAPI
{
public:
	static const CAppLicActCode& getLicActCode(void);
	static void setLicActCode(const CAppLicActCode& ref);
};
