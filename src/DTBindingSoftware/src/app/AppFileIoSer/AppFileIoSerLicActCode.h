#pragma once

#include "appfileioser.h"

// Class CAppFileIoAPI

class CAppFileIoSerLicActCode : public CAppFileIoSer
{
public:
	CAppFileIoSerLicActCode( );
	virtual ~CAppFileIoSerLicActCode( );

	virtual BOOL Save(CFile* fp);
	virtual BOOL Load(CFile* fp);
	virtual BOOL Peek(CFile* fp);

	virtual unsigned int getSerID( ) const;
	
	unsigned long getCode(void) const;
	void setCode(unsigned long code);
	BYTE getFlags(void) const;
	void setFlags(BYTE bFlags);

	CAppFileIoSerLicActCode& operator=(const CAppFileIoSerLicActCode& ref);

private:
	const unsigned int m_SerID;

	unsigned long m_LicenseCode;
	BYTE m_Flags;
	BYTE m_Reserved[3];
};

// Inline functies.
inline 
unsigned int CAppFileIoSerLicActCode::getSerID( ) const
{
	return m_SerID;
}
	
inline 
unsigned long CAppFileIoSerLicActCode::getCode(void) const
{
	return m_LicenseCode;
}

inline 
void CAppFileIoSerLicActCode::setCode(unsigned long code)
{
	m_LicenseCode = code;
}

inline 
BYTE CAppFileIoSerLicActCode::getFlags(void) const
{
	return m_Flags;
}

inline 
void CAppFileIoSerLicActCode::setFlags(BYTE bFlags)
{
	m_Flags = bFlags;
}
