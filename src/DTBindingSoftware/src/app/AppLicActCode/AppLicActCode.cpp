#include "stdafx.h"
#include "AppLicActCode.h"
	

CAppLicActCode::CAppLicActCode(void) :
m_code(0),
m_flags(FLAG_CODE_NEEDED)
{

}
CAppLicActCode::~CAppLicActCode(void)
{

}

void CAppLicActCode::setCode(unsigned long v)
{
	m_code = v;
}

unsigned long CAppLicActCode::getCode(void) const
{
	return m_code;
}

void CAppLicActCode::setFlags(BYTE v)
{
	m_flags = v;
}

BYTE CAppLicActCode::getFlags(void) const
{
	return m_flags;
}

#ifdef SKIP_NOT_USED
	void CAppLicActCode::setFlagNeeded(BOOL f)
	{
		if (f)
		{
			m_flags |= FLAG_CODE_NEEDED;
		}
		else
		{
			m_flags &= ~FLAG_CODE_NEEDED;
		}
	}
#endif

BOOL CAppLicActCode::getFlagNeeded(void) const
{
	return ((m_flags & FLAG_CODE_NEEDED) == FLAG_CODE_NEEDED);
}

void CAppLicActCode::setFlagActivated(BOOL f)
{
	if (f)
	{
		m_flags |= FLAG_ACTIVATED;
	}
	else
	{
		m_flags &= ~FLAG_ACTIVATED;
	}
}

BOOL CAppLicActCode::getFlagActivated(void) const
{
	return ((m_flags & FLAG_ACTIVATED) == FLAG_ACTIVATED);
}	

