#include "stdafx.h"
#include "ConfigCanFtp_Info.h"
///////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

///////////////////////////////////////////////////////////////////
//////////////////// CConfigCanFTP_FileEntry //////////////////////
///////////////////////////////////////////////////////////////////
CConfigCanFTP_FileEntry::CConfigCanFTP_FileEntry( ) :
m_bNodeAddress( 0xFF ),
m_szLocalFileName( 0 ),
m_szRemoteFileName( 0 ),
m_szInfo( 0 ),
m_action( ACTION_FTP_UNDEFINED ),
m_pfCallbackStarted( 0 ),
m_pfCallbackDone( 0 ),
m_ctxFlags( 0 )
{

}
CConfigCanFTP_FileEntry::~CConfigCanFTP_FileEntry( )
{
	deallocate( &m_szLocalFileName );
	deallocate( &m_szRemoteFileName );
	deallocate( &m_szInfo );
}
/////////////////////////// protected //////////////////////////////////
void CConfigCanFTP_FileEntry::deallocate(char** s)
{
	if (*s)
	{
		delete [] *s;
		*s = 0;
	}
}
void CConfigCanFTP_FileEntry::set(char** szDest, const char* s)
{
	ASSERT( *szDest == 0 );

	const int len = strlen(s) + 1;
	*szDest = new char[len+1];

	if (*szDest)
	{
		strcpy(*szDest, s);
	}
}
////////////////////////////////////////////////////////////////////////
void CConfigCanFTP_FileEntry::setNodeAddress(BYTE nodeAddress)
{
	m_bNodeAddress = nodeAddress;
}
BYTE CConfigCanFTP_FileEntry::getNodeAddress( ) const
{
	return m_bNodeAddress;
}
void CConfigCanFTP_FileEntry::setRemoteFileName(const char* filename)
{
	deallocate(&m_szRemoteFileName);
	set(&m_szRemoteFileName, filename);	
}
const char* CConfigCanFTP_FileEntry::getRemoteFileName( ) const
{
	return m_szRemoteFileName;
}
void CConfigCanFTP_FileEntry::setLocalFileName(const char* filename)
{
	deallocate(&m_szLocalFileName);
	set(&m_szLocalFileName, filename);	
}
const char* CConfigCanFTP_FileEntry::getLocalFileName( ) const
{
	return m_szLocalFileName;
}
void CConfigCanFTP_FileEntry::setInfo(const char* info)
{
	deallocate(&m_szInfo);
	set(&m_szInfo, info);	
}
const char* CConfigCanFTP_FileEntry::getInfo( ) const
{
	return m_szInfo;
}
CConfigCanFTP_FileEntry::FtpAction 
CConfigCanFTP_FileEntry::getAction( ) const
{
	return m_action;
}
void CConfigCanFTP_FileEntry::setAction(FtpAction action)
{
	m_action = action;
}

// zie enum CtxFlags
void CConfigCanFTP_FileEntry::setCtxFlags(int flags)
{
	m_ctxFlags = flags;
}

int CConfigCanFTP_FileEntry::getCtxFlags(void)
{
	return m_ctxFlags;
}

////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
//////////////// CConfigCanFTP_FileEntryIterator ///////////////////////
////////////////////////////////////////////////////////////////////////
CConfigCanFTP_FileEntryIterator::CConfigCanFTP_FileEntryIterator(CPtrArray& array) :
m_refArray( array ),
m_currentItem( 0 )
{
}
CConfigCanFTP_FileEntryIterator::~CConfigCanFTP_FileEntryIterator( )
{
}
////////////////////////////////////////////////////////////////////////
CConfigCanFTP_FileEntry* CConfigCanFTP_FileEntryIterator::CurrentItem( )
{
	CConfigCanFTP_FileEntry* pEntry = 0;
	if( m_currentItem < m_refArray.GetSize())
	{
		pEntry = static_cast<CConfigCanFTP_FileEntry*>
					(m_refArray[ m_currentItem ]);
	}
	return pEntry;
}
void CConfigCanFTP_FileEntryIterator::Begin( )
{
	m_currentItem = 0;
}
void CConfigCanFTP_FileEntryIterator::Next( )
{
	m_currentItem++;
}
bool CConfigCanFTP_FileEntryIterator::IsDone( )
{
	return (m_currentItem >= m_refArray.GetSize() ? true : false);
}

///////////////////////////////////////////////////////////////////////


//////////////////////////// singleton /////////////////////////////////
CConfigCanFTP_Info& CConfigCanFTP_Info::getInstance( )
{
	static CConfigCanFTP_Info _instance;
	return _instance;
}
///////////////////////////////////////////////////////////////////
//////////////////////// CConfigCanFTP_Info ///////////////////////
///////////////////////////////////////////////////////////////////
CConfigCanFTP_Info::CConfigCanFTP_Info( ) :
m_array( )
{
}
CConfigCanFTP_Info::~CConfigCanFTP_Info( )
{
	destroyAll( );
}
////////////////////////// public //////////////////////////////////
void CConfigCanFTP_Info::clear( )
{
	destroyAll( );
}
bool CConfigCanFTP_Info::addTask(
	CConfigCanFTP_FileEntry* pNewEntry)
{
	bool retVal = false;
	if (pNewEntry)
	{
		try
		{
			m_array.Add(pNewEntry);
			retVal = true;
		}
		catch(CMemoryException* )
		{
			destroy(pNewEntry);			
		}
	}
	return retVal;
}
CConfigCanFTP_FileEntry* CConfigCanFTP_Info::create( )
{
	return new CConfigCanFTP_FileEntry( );
}
CConfigCanFTP_FileEntryIterator CConfigCanFTP_Info::createIterator( )
{
	return CConfigCanFTP_FileEntryIterator( m_array );
}
///////////////////////// protected ////////////////////////////////
void CConfigCanFTP_Info::destroy(CConfigCanFTP_FileEntry* pEntry)
{
	if (pEntry)
	{
		delete pEntry;
		pEntry = 0;
	}
}
void CConfigCanFTP_Info::destroyAll( )
{
	const int nCnt = m_array.GetSize();
	/* CConfigCanFTP_FileEntry *pEntry = 0; */		// CHANGES_MVS_2008_WARNINGS

	for (int i=0;i<nCnt;i++) 
	{	
		destroy(static_cast<CConfigCanFTP_FileEntry*>(m_array.GetAt(i)));			
	}
	m_array.RemoveAll();
}
