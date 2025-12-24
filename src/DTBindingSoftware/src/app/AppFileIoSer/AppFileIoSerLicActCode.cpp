#include "stdafx.h"
#include "AppFileIoSerLicActCode.h"


CAppFileIoSerLicActCode::CAppFileIoSerLicActCode( ) :
CAppFileIoSer( ),
m_SerID(APPFILEIOSER_ID_LICACTCODE),
m_LicenseCode(0),
m_Flags(0)
{
	for (int i=0;i<3;++i)
	{
		m_Reserved[i] = 0;
	}
}

CAppFileIoSerLicActCode::~CAppFileIoSerLicActCode( )
{

}

BOOL CAppFileIoSerLicActCode::Peek(CFile* fp)
{
	BOOL fResult = FALSE;

	try
	{
		const ULONGLONG lActual = fp->GetPosition();
		unsigned int serializableID = 0;

		fp->Read(&serializableID, sizeof(serializableID));
		fp->Seek(lActual,CFile::begin);

		if (serializableID == this->m_SerID)
		{
			fResult	= TRUE;
		}
	}
	catch (CFileException *e) 
	{
		e->ReportError();
		e->Delete();
		fResult = FALSE;
	}		
	return fResult;
}

BOOL CAppFileIoSerLicActCode::Save(CFile* fp)
{
	BOOL fResult = FALSE;

	try 
	{	
		fp->Write(&this->m_SerID, sizeof(this->m_SerID));
		fp->Write(&this->m_LicenseCode, sizeof(this->m_LicenseCode));
		fp->Write(&this->m_Flags, sizeof(this->m_Flags));
		fp->Write(&this->m_Reserved, sizeof(this->m_Reserved));

		fResult = TRUE;
	}
	catch (CFileException *e) 
	{
		e->ReportError();
		e->Delete();
		fResult = FALSE;
	}		

	return fResult;
}

BOOL CAppFileIoSerLicActCode::Load(CFile* fp)
{
	BOOL fResult = FALSE;

	try 
	{
		unsigned int serializableID = 0;

		fp->Read(&serializableID, sizeof(serializableID));

		if (serializableID == this->m_SerID)
		{
			fp->Read(&this->m_LicenseCode, sizeof(this->m_LicenseCode));
			fp->Read(&this->m_Flags, sizeof(this->m_Flags));
			fp->Read(&this->m_Reserved, sizeof(this->m_Reserved));
			fResult = TRUE;
		}
		else
		{
			ASSERT(serializableID == this->m_SerID);
		}
	}

	catch (CFileException *e) 
	{
		e->ReportError();
		e->Delete();
		fResult = FALSE;
	}		
	return fResult;
}

CAppFileIoSerLicActCode& CAppFileIoSerLicActCode::operator=(const CAppFileIoSerLicActCode& ref)
{
	if (this != &ref)
	{
		this->setCode(ref.getCode());
		this->setFlags(ref.getFlags());
	}
	return *this;
}