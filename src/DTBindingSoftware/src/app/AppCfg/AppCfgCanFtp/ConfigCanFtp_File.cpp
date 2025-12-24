#include "stdafx.h"
#include "ConfigCanFtp_File.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

///////////////////////////////////////////////////////////////////////
namespace
{
	inline void dumpError(int ec)
	{
		TRACE("File error:%d\n",ec);		
	}
};

///////////////////////////////////////////////////////////////////////
////////////////////////// CConfigCanFTP //////////////////////////////
///////////////////////////////////////////////////////////////////////
CConfigCanFTP_File::CConfigCanFTP_File( ) :
m_fp( 0 ),
m_fileLength( -1 )
{
}
CConfigCanFTP_File::~CConfigCanFTP_File( )
{
	if (m_fp)
	{
		fclose(m_fp);
	}
}
///////////////////////////////////////////////////////////////////////
bool CConfigCanFTP_File::openFileWrite(
	const char* szFileName)
{
	ASSERT( m_fp == 0);

	bool result = false;
	m_fp = fopen(szFileName,"wb");
	if (m_fp != 0)
	{			
		result = true;
	}
	return result;
}

bool CConfigCanFTP_File::openFileRead(
	const char* szFileName)
{
	ASSERT( m_fp == 0);

	bool result = false;
	m_fp = fopen(szFileName,"rb");
	if (m_fp != 0)
	{	
		const int res2 = fseek(m_fp,0,SEEK_END);
		if (res2 == 0)
		{
			m_fileLength = ftell(m_fp);
			fseek(m_fp,0,SEEK_SET);
		}
		result = true;
	}
	return result;
}

bool CConfigCanFTP_File::getData(
	BYTE* pbData, 
	int length,
	int* numRead)
{	
	ASSERT( pbData );

	if (m_fp)
	{
		bool retVal = true;
		int read = fread(pbData, 
						 sizeof(char),
						 length,
						 m_fp);

		if (read == 0 ||
			read != length)
		{
			const int ec = ferror(m_fp);
			if (ec != 0)
			{
				dumpError( ec );
				retVal = false;
			}
		}
		*numRead = read;
		return retVal;
	}
	return false;
}

bool CConfigCanFTP_File::addData(
	const BYTE* pbData,
	int length)
{
	ASSERT( pbData );

	bool retVal = false;
	if (m_fp)
	{	
		int numWritten = fwrite(pbData,
							    sizeof(char),
								length,
								m_fp);

		if (numWritten == length)
		{
			retVal = true;
		}
		else
		{
			const int ec = ferror(m_fp);
			if (ec != 0)
			{
				dumpError( ec );
			}
			retVal = false;
		}	
	}
	return retVal;
}

bool CConfigCanFTP_File::isDone( ) const
{
	return (m_fp == 0 || feof(m_fp) != 0);
}

bool CConfigCanFTP_File::isBusy( ) const
{
	return m_fp != 0;
}

bool CConfigCanFTP_File::close( )
{
	bool retVal = false;
	if (m_fp)
	{
		if (fclose(m_fp) == 0)
		{
			m_fp = 0;
			retVal = true;
		}
	}
	return retVal;
}

int CConfigCanFTP_File::getLength( )
{
	int retVal = -1;
	if (m_fp)
	{
		retVal = m_fileLength; 
	}
	return retVal;
}

int CConfigCanFTP_File::getPosition( )
{
	int retVal = -1;
	if (m_fp)
	{
		retVal = ftell(m_fp);	
	}
	return retVal;
}
