// ConfigSmapPageInfoClass.cpp: implementation of the CConfigSmapPageInfoClass class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ConfigSmapPageInfoClass.h"

#include "app\appcfg\appcfgbase\configexternalhelper.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define DEBUG_LOCAL_ON		0

//////////////////////////////////////////////////////////////////////
#if (DEBUG_LOCAL_ON == 1)
	#define TRACER(x)				TRACE(x)
	#define TRACER_2(x,y)			TRACE(x,y)
	#define TRACER_3(x,y,z)			TRACE(x,y,z)
#else
	#define TRACER(x)				
	#define TRACER_2(x,y)		
	#define TRACER_3(x,y,z)		
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSmapPageInfoData::CSmapPageInfoData( ) : 
m_data( )
{
	init( );
}

CSmapPageInfoData::~CSmapPageInfoData( )
{
	removeAll( );
}

void CSmapPageInfoData::init( )
{
	m_data.m_pageNR;

	for (int i=0;i<DUOTECNO::DATA::SMAP_PAGE_MAX_NAME_LENGTH;i++)
	{
		m_data.m_name[i] = '\0';
	}

	strcpy(m_data.m_name, "Undefined");
}

BOOL CSmapPageInfoData::parse(const char* szAttr, const char* szData)
{
	BOOL result = FALSE;

	if (szData)
	{
		enum { MAX_TEMP_BUFFER = 256 };
		char szTempAttribute[ MAX_TEMP_BUFFER ];

		CConfigExternalHelper::toLowerString(szAttr, szTempAttribute, MAX_TEMP_BUFFER-1);

		static const char* const PAGENR = "page";		// lower case !!!
		static const char* const NAME = "szname";		// lower case !!!
				
		if (strcmp(szTempAttribute, PAGENR) == 0)
		{
			int value = 0;

			TRACER_2("CSmapPageInfoData::parse: %s\n", PAGENR);

			sscanf(szData,"%d;",&value);				// CHECKED_SSCANF_OK
			m_data.m_pageNR = (BYTE) value;
			result = TRUE;
		}
		else if (strcmp(szTempAttribute, NAME) == 0)
		{
			TRACER_2("CSmapPageInfoData::parse: %s\n", NAME);
		
			enum { MAX_STRING_LENGTH = 16 };
			char szTempName[ MAX_STRING_LENGTH ];
		
			if (CConfigExternalHelper::parseString(szData,
												   szTempName,
												   MAX_STRING_LENGTH-1))
			{	
				CConfigExternalHelper::stripEndOfString(szTempName,
														MAX_STRING_LENGTH-1,
														';');
			
				sprintf(m_data.m_name, szTempName);	
				
			}
			else
			{
				strcpy(m_data.m_name, "");
			}
			
			result = TRUE;
		}
	}

	return result;
}

void CSmapPageInfoData::dump( )
{
	char szBuffer[255];
	
	sprintf(szBuffer,
		    "pageNR=%02d;\nName=%s;\n",
			m_data.m_pageNR,
			m_data.m_name);

	TRACE(szBuffer);
}

// Protected 

void CSmapPageInfoData::removeAll( )
{
}

int CSmapPageInfoData::GetData(BYTE *pbData)
{
	const int length = sizeof( DUOTECNO::DATA::SerSmapPageInfo );
	memcpy(pbData, &m_data, length);
	return length;	
}


//////////////////////////////////////////////////////////////////////


CSmapPageInfoHeader::CSmapPageInfoHeader( ) : 
m_header( ),
m_list( )
{
	init( );
}

CSmapPageInfoHeader::~CSmapPageInfoHeader( )
{
	removeAll( );
}

void CSmapPageInfoHeader::init( )
{
	m_header.m_count = 0;
	m_header.m_screenmapID = 0;	
}

void CSmapPageInfoHeader::add(CSmapPageInfoData* pEntry)
{
	TRACER("CSmapPageInfoHeader::add()\n");

	m_list.addTail(pEntry);
	m_header.m_count++;
}

CSmapPageInfoData* CSmapPageInfoHeader::get(int index)
{
	ASSERT( index < m_list.getCount( ) );
	return m_list.getEntry( index );
}

void CSmapPageInfoHeader::dump( )
{
	char szBuffer[255];

	const int count = getCount( );

	sprintf(szBuffer,
			"SCREENMAP=%02d,count=%d\n",
			m_header.m_screenmapID,
			m_header.m_count);


	TRACE(szBuffer);

	for (int i=0;i<count;i++)
	{
		CSmapPageInfoData* const pEntry = get( i );
		if (pEntry)
		{
			pEntry->dump( );
		}
	}
}

int CSmapPageInfoHeader::GetData(BYTE *pbData)
{
	const int length = sizeof(DUOTECNO::DATA::SerSmapPageInfoHeader);
	memcpy(pbData, &m_header, length);
	return length;
}

// protected

void CSmapPageInfoHeader::removeAll( )
{
	while (!m_list.isEmpty())
	{
		CSmapPageInfoData* const p = m_list.removeHead( );
		ASSERT( p );
		delete p;
	}

	ASSERT( m_list.getCount( ) == 0 );
}


//////////////////////////////////////////////////////////////////////

const char* const CConfigSmapPageInfoClass::_szHeader = "SCREENMAP";

//////////////////////////////////////////////////////////////////////

CConfigSmapPageInfoClass::CConfigSmapPageInfoClass(DUOTECNO::MFC_HELPER::IReportError *ptr) :
m_pErrorReport( ptr ),
m_pCurrHeader( 0 ),
m_pCurrData( 0 ),
m_currentIndex( 0 ),
m_list( )
{

}

CConfigSmapPageInfoClass::~CConfigSmapPageInfoClass()
{
	removeAll( );
}

// protected

void CConfigSmapPageInfoClass::removeAll( )
{
	while (!m_list.isEmpty())
	{
		CSmapPageInfoHeader* const p = m_list.removeHead( );
		ASSERT( p );
		delete p;
	}
	ASSERT( m_list.getCount( ) == 0 );
}

// public 

void CConfigSmapPageInfoClass::ReportError(const char* szMessage)
{
	if (m_pErrorReport)
	{
		m_pErrorReport->reportError( szMessage );
	}
}

BOOL CConfigSmapPageInfoClass::ParseHeader(char *szAttribute,char *szData)
{
	if (strcmp(_szHeader,szAttribute) == 0) 
	{			
		if (sscanf(szData,"%d",&m_currentIndex) == 1)		// CHECKED_SSCANF_OK
		{		
			TRACER_2("CConfigSmapPageInfoClass::ParseHeader() - m_currentIndex=%d\n",m_currentIndex);

			if (m_currentIndex < 256)
			{
				m_pCurrHeader = new CSmapPageInfoHeader( );
				if (m_pCurrHeader)
				{
					int value = -1;							// CHANGES_MVS_2008_SSCANF
					sscanf(szData,"%d;",&value);			// CHANGES_MVS_2008_SSCANF

					ASSERT(value >= 0 && value <= 0xFF);	// CHANGES_MVS_2008_SSCANF

					m_pCurrHeader->setIdentifier((BYTE) value);	// CHANGES_MVS_2008_SSCANF
					m_list.addTail( m_pCurrHeader );

					TRACER_2("CConfigSmapPageInfoClass::ParseHeader() - currentHeader id=%d\n",value);

					return TRUE;
				}

				ASSERT( m_pCurrHeader );
			}
			else
			{
				char szBuffer[255];
				sprintf(szBuffer,
						"Error:Screenmap id %d:to large",
						 m_currentIndex);
				ReportError(szBuffer);	
			}
		}
		else
		{
			char szBuffer[255];
			sprintf(szBuffer,
					"Error:Screenmap id %d:to large",
					 m_currentIndex);
			ReportError(szBuffer);
		}
		return FALSE;
	}

	return FALSE;
}

BOOL CConfigSmapPageInfoClass::ParseData(char *szAttr,char *szData)
{
	ASSERT( m_pCurrHeader );

	if (m_pCurrHeader == 0)
	{
		return FALSE;
	}

	static const char* const PAGE = "page";		// lower case
	
	enum { MAX_TEMP_BUFFER = 256 };
	char szTempAttribute[ MAX_TEMP_BUFFER ];

	CConfigExternalHelper::toLowerString(szAttr, szTempAttribute, MAX_TEMP_BUFFER-1);
	
	if (strcmp(PAGE,szTempAttribute) == 0)
	{
		m_pCurrData = new CSmapPageInfoData( );
		if (m_pCurrData)
		{
			if (m_pCurrData->parse(szAttr, szData))
			{
				TRACER("CConfigSmapPageInfoClass::ParseData(): Add data to header\n");

				m_pCurrHeader->add( m_pCurrData );
				return TRUE;
			}
			else
			{
				char szBuffer[255];			
				sprintf(szBuffer,
						"Error:Screenmap id %d , attribute '%s'",
						 m_pCurrHeader->getIdentifier(),
						 PAGE);

				ReportError(szBuffer);

				delete m_pCurrData;
				m_pCurrData = 0;

				return FALSE;
			}
		}
	}
	else
	{
		if (m_pCurrData)
		{
			if (m_pCurrData->parse(szAttr, szData))
			{
				return TRUE;
			}
			else
			{
				char szBuffer[255];			
				sprintf(szBuffer,
						"Error:Screenmap id %d : Unkown attribute:'%s'",
						 m_pCurrHeader->getIdentifier(),
						 szAttr);

				ReportError(szBuffer);	
			}
		}
		else
		{
			char szBuffer[255];			
			sprintf(szBuffer,
					"Error:Screenmap id %d : Unkown attribute:'%s'",
					 m_pCurrHeader->getIdentifier(),
					 szAttr);

			ReportError(szBuffer);	
		}	
	}
	return FALSE;
}


void CConfigSmapPageInfoClass::Initialise(void)
{
	removeAll( );

	m_currentIndex = 0;
	m_pCurrHeader = 0;
	m_pCurrData = 0;
}

int CConfigSmapPageInfoClass::GetMax(void) const
{
	return m_list.getCount( );
}

void CConfigSmapPageInfoClass::dump( )
{
	const int max = m_list.getCount( );

	for (int i=0;i<max;i++)
	{
		CSmapPageInfoHeader* const p = getHeader( i );
		if (p)
		{
			p->dump( );
		}
	}
}

int CConfigSmapPageInfoClass::GetData(int index,BYTE *pbData)
{
	pbData = 0;
	return 0;		
}

BOOL CConfigSmapPageInfoClass::IsUsed(int index)
{
	return (index < m_list.getCount( ));
}

CSmapPageInfoHeader* CConfigSmapPageInfoClass::getHeader(int index)
{
	/* BUG-0126 */
	CSmapPageInfoHeader* p = 0;

	if (index < m_list.getCount( ))
	{	
		p = m_list.getEntry( index );
	}
	
	return p;
}
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
CConfigPageInfoHeader::CConfigPageInfoHeader( ) :
m_pHeader( 0 ) 
{ 
}
CConfigPageInfoHeader::~CConfigPageInfoHeader( ) 
{ 
}
//////////////////////////////////////////////////////////////////////

void CConfigPageInfoHeader::set(CSmapPageInfoHeader* pHeader)
{
	m_pHeader = pHeader;
}

int CConfigPageInfoHeader::GetData(int index,BYTE *pbData)
{
	ASSERT( m_pHeader );

	if ((index != 0) || (m_pHeader == 0))
	{
		return 0;
	}
			

	TRACER("CConfigPageInfoHeader::GetData( )\n");

#if (DEBUG_LOCAL_ON == 1)

	m_pHeader->dump( );

#endif


	const int length = m_pHeader->GetData(pbData);

#if (DEBUG_LOCAL_ON == 1)

	for (int i=0;i<length;i++)
	{
		TRACER_2("%02d ",pbData[i]);
	}
	TRACER("\n");

#endif

	return length;
}

BOOL CConfigPageInfoHeader::IsUsed(int index)
{
	return ((index == 0) && (m_pHeader != 0));
}

int CConfigPageInfoHeader::GetMax(void) const
{
	return 1;
}

void CConfigPageInfoHeader::dump( )
{
	if (m_pHeader)
	{
		m_pHeader->dump( );
	}
}

int CConfigPageInfoHeader::getStartIndex( ) const
{
	if (m_pHeader)
	{
		return m_pHeader->getIdentifier();
	}
	return 0xFF;
}

///////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////
CConfigPageInfoData::CConfigPageInfoData( ) :
m_pHeader( 0 ) 
{ 

}
CConfigPageInfoData::~CConfigPageInfoData( ) 
{ 
}
///////////////////////////////////////////////////////////////////////
void CConfigPageInfoData::set(CSmapPageInfoHeader* pHeader)
{
	m_pHeader = pHeader;
}

int CConfigPageInfoData::GetData(int index,BYTE *pbData)
{
	ASSERT( m_pHeader );

	TRACER("CConfigPageInfoData::GetData( )\n");


	const int max = m_pHeader->getCount();
	if (index < max)
	{
		CSmapPageInfoData* const pEntry =
			reinterpret_cast<CSmapPageInfoData*>(m_pHeader->get(index));

		if (pEntry)
		{
			
			#if (DEBUG_LOCAL_ON == 1)

			pEntry->dump( );

			#endif

			const int length = pEntry->GetData(pbData);
										
			#if (DEBUG_LOCAL_ON == 1)
			for (int i=0;i<length;i++)
			{
				TRACER_2("%02d ",pbData[i]);
			}

			TRACER("\n");
			#endif

			return length;			
		}
		return 0;
	}
	return 0;			
}

BOOL CConfigPageInfoData::IsUsed(int index)
{
	ASSERT( m_pHeader );

	return(index < m_pHeader->getCount());
}

int CConfigPageInfoData::GetMax(void) const
{
	ASSERT( m_pHeader );

	return m_pHeader->getCount( );
}

void CConfigPageInfoData::dump( )
{
	if (m_pHeader)
	{
		m_pHeader->dump( );
	}
}

/***************************************************************/