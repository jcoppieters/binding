#include "stdafx.h"
#include "AppCfgSmapLiteClass.h"

#include "app\appcfg\appcfgbase\ConfigExternalHelper.h"

/*************************************************************/
using DUOTECNO::MFC_HELPER::IReportError;
/*************************************************************/
const char* const CAppCfgSmapLiteClass::_szHeader = "SMAPENTRY";
/*************************************************************/
using DUOTECNO::DATA::CSerSmapLiteEntry;
/*************************************************************/

/*************************************************************/
/*********************** Helper Functies *********************/
/*************************************************************/

static 
bool helper_parse(
	const char* szData,
	int maxStringLength,
	char* pszDestination)
{
	enum { MAX_STRING_LENGTH = 64 };
	char szTempName[ MAX_STRING_LENGTH ];

	memset(szTempName,0,sizeof(szTempName));
		
	if (CConfigExternalHelper::parseString(szData,
										   szTempName,
										   maxStringLength))
	{	
		CConfigExternalHelper::stripEndOfString(szTempName,
												maxStringLength,
												';');
		
		const int stringLength = strlen(szTempName);
		if (stringLength < maxStringLength)
		{
			sprintf(pszDestination, szTempName);
			return true;
		}
	}
	return false;
}

static 
bool helper_parse_hex(
	const char* s,
	BYTE* pDest,
	int min,
	int max)
{
	int value = -1;

	if (sscanf(s,"%x;",&value) == 1)		
	{
		if (value >= min && value <= max)
		{
			*pDest = (BYTE) value;
			return true;
		}		
	}	
	return false;
}

static 
bool helper_parse_dec(	
	const char* s,
	uint16_t* pDest,
	unsigned int min,
	unsigned int max)
{
	unsigned int value = 0;

	if (sscanf(s,"%i;",&value) == 1)		
	{
		if (value >= min && value <= max)
		{
			*pDest = (uint16_t) value;
			return true;
		}		
	}	
	return false;
}


/*************************************************************/
/******************** CSerSmapLiteEntry *********************/
/*************************************************************/
static 
void clear(
	CSerSmapLiteEntry* pEntry)
{
	memset(pEntry,0,sizeof(CSerSmapLiteEntry));

	pEntry->m_bNodeAddress=0xFF;		
	pEntry->m_bUnitAddress=0xFF;
};

// Oppassen:
// Enkel de velden die momenteel nodig zijn worden gebruikt.
static 
bool format(
	const CSerSmapLiteEntry& refEntry,
	char* szBuffer,
	int maxLength,
	int *length)
{
	char szTempBuffer[512];

	sprintf(szTempBuffer,		   
			"bAddress=0x%02x;0x%02x;\n"
			"m_szName=%s;\n",		
			refEntry.m_bNodeAddress,refEntry.m_bUnitAddress,
			refEntry.m_szName);

	*length = strlen(szTempBuffer);
	if (*length < maxLength)
	{
		strcpy(szBuffer,szTempBuffer);
		return true;
	}
	return false;
}

static
bool convertToCfgStrEntry(
	DUOTECNO::DATA::CSerSmapLiteEntry& ref,
	char* pszBuffer,
	int maxLength)
{
	char szTemp[1024];
	bool fResult=false;

	REQUIRE(pszBuffer);

	memset(szTemp,0,sizeof(szTemp));

	sprintf(szTemp,
		    _T("bType=0x%02x;\n"
			   "szName=%s;\n"
			   "bNodeAddress=0x%02x;\n"
			   "bUnitAddress=0x%02x;\n"
			   "pageNR=%d;\n"
			   "buttonNR=%d;\n"),
			ref.m_bType,
			ref.m_szName,
			ref.m_bNodeAddress,
			ref.m_bUnitAddress,
			ref.m_pageNR,
			ref.m_buttonNR);

	if ((int)strlen(szTemp)<maxLength-1)
	{
		strcpy(pszBuffer,szTemp);
		fResult=true;
	}
	return fResult;
}


/*************************************************************/
/********************CAppCfgSmapLiteClass *******************/
/*************************************************************/
CAppCfgSmapLiteClass::CAppCfgSmapLiteClass(
	DUOTECNO::MFC_HELPER::IReportError* ptr) :
	m_list( ),
	m_pCurrentEntry( 0 ),
	m_pErrorReport( ptr ),
	m_currentIndex( 0 )
{

}

CAppCfgSmapLiteClass::~CAppCfgSmapLiteClass(void)
{
	removeAll( );
}
/*************************************************************/

void CAppCfgSmapLiteClass::removeAll( )
{
	while(!m_list.isEmpty())
	{
		CSerSmapLiteEntry* const p = m_list.removeHead( );
		ASSERT(p);
		delete p;
	}
	ASSERT( m_list.getCount() == 0 );
}

CSerSmapLiteEntry* CAppCfgSmapLiteClass::GetEntry(int entry)
{
	ASSERT( entry < m_list.getCount() );
	return m_list.getEntry(entry);
}

void CAppCfgSmapLiteClass::ReportError(const char* szMessage)
{
	if (m_pErrorReport)
	{
		m_pErrorReport->reportError( szMessage );
	}
}

void CAppCfgSmapLiteClass::ParseDataReportError(
	int currentIndex,
	const char* szAttribute)
{
	CString s;
	s.Format(_T("Error:Index 0x%02x - Attribute '%s' Wrong value"),
			 currentIndex,
			 szAttribute);
	ReportError(s);	
}

//////////////////////////////////////////////////////////////////

BOOL CAppCfgSmapLiteClass::addEntry(
	CSerSmapLiteEntry* pEntry)
{
	m_list.addTail( pEntry );
	return TRUE;
}

BOOL CAppCfgSmapLiteClass::ParseHeader(
	char *szAttribute,
	char *szData)
{
	if (strcmp(_szHeader,szAttribute) == 0) 
	{	
		if (sscanf(szData,"%x",&m_currentIndex) == 1)	
		{	
			if (m_currentIndex < 0xFFFF)
			{
				m_pCurrentEntry = new CSerSmapLiteEntry( );
				if (m_pCurrentEntry)
				{
					clear( m_pCurrentEntry );
				
					m_list.addTail( m_pCurrentEntry );
					return TRUE;
				}
				ASSERT( m_pCurrentEntry );
			}
			else
			{
				char szBuffer[255];
				sprintf(szBuffer,
						"Error:Entry 0x%02x to large",
						 m_currentIndex);
				ReportError(szBuffer);	
			}
		}
		else
		{
			char szBuffer[255];
			sprintf(szBuffer,
					"Error:Entry 0x%02x",
					m_currentIndex);
			ReportError(szBuffer);
		}
		return FALSE;
	}
	return FALSE;
}

BOOL CAppCfgSmapLiteClass::ParseData(
	char *szAttribute,
	char *szData)
{
	int value = -1;

	if (!m_pCurrentEntry) 
	{
		ASSERT(m_pCurrentEntry);
		return FALSE;
	}

	if (strcmp(szAttribute,"bType") == 0)		// HEX
	{
		if (helper_parse_hex(szData,&m_pCurrentEntry->m_bType,0,0xFF) == false)
		{
			// Dit is geen verplicht veld...
			// ParseDataReportError(m_currentIndex,szAttribute);	
			m_pCurrentEntry->m_bType=0;
		}		
	}
	else if (strcmp(szAttribute,"szName") == 0)	// STRING
	{
		if (helper_parse(szData,sizeof(m_pCurrentEntry->m_szName)-1,&m_pCurrentEntry->m_szName[0]) == false)
		{
			// Dit is geen verplicht veld...
			// ParseDataReportError(m_currentIndex,szAttribute);	
			strcpy(m_pCurrentEntry->m_szName,"");
		}	
	}
	else if (strcmp(szAttribute,"bNodeAddress") == 0)	// HEX
	{
		if (helper_parse_hex(szData,&m_pCurrentEntry->m_bNodeAddress,0x03,0xFF) == false)
		{
			ParseDataReportError(m_currentIndex,szAttribute);	
		}		
	}
	else if (strcmp(szAttribute,"bUnitAddress") == 0)		// HEX
	{
		if (helper_parse_hex(szData,&m_pCurrentEntry->m_bUnitAddress,0x00,0xFF) == false)
		{
			ParseDataReportError(m_currentIndex,szAttribute);	
		}		
	}
	else if (strcmp(szAttribute,"bUnitType") == 0)			// HEX
	{
		if (helper_parse_hex(szData,&m_pCurrentEntry->m_bUnitType,0x00,0xFF) == false)
		{
			//Dit is geen verplicht veld...
			//ParseDataReportError(m_currentIndex,szAttribute);		

			m_pCurrentEntry->m_bUnitType=0x00;
		}	
	}
	else if (strcmp(szAttribute,"pageNR") == 0)				// DEC - UINT16_t
	{
		if (helper_parse_dec(szData,&m_pCurrentEntry->m_pageNR,0,0xFFFF)== false)
		{
			ParseDataReportError(m_currentIndex,szAttribute);
		}
	}
	else if (strcmp(szAttribute,"buttonNR") == 0)			// DEC - UINT16_t
	{
		if (helper_parse_dec(szData,&m_pCurrentEntry->m_buttonNR,0,0xFFFF)== false)
		{
			ParseDataReportError(m_currentIndex,szAttribute);
		}
	}
	else
	{
		ParseDataReportError(m_currentIndex,szAttribute);
		return FALSE;
	}
	return TRUE;
}

void CAppCfgSmapLiteClass::Initialise(void)
{
	removeAll( );
	m_currentIndex = 0;
}

int CAppCfgSmapLiteClass::GetMax(void) const
{
	return m_list.getCount( );
}

int CAppCfgSmapLiteClass::GetData(int index,BYTE *pbData)
{
	if (index < m_list.getCount( ))
	{
		CSerSmapLiteEntry* const pSerEntry = GetEntry( index );			
		if (pSerEntry)
		{
			memcpy(pbData,pSerEntry,sizeof(CSerSmapLiteEntry));
			return (sizeof(CSerSmapLiteEntry));		
		}
	}	
	pbData = 0;
	return 0;
}

BOOL CAppCfgSmapLiteClass::IsUsed(int index)
{
	if (index < m_list.getCount( ))
	{
		return TRUE;
	}
	return FALSE;
}

BOOL CAppCfgSmapLiteClass::GetEntryStrCfg(
	int index,
	char* pszTempBuffer,
	int maxBufferLength)
{
	BOOL fResult=FALSE;

	if (index<m_list.getCount())
	{		
		CSerSmapLiteEntry* const pSerEntry = GetEntry(index);			
		if (pSerEntry)
		{
			char szTempBuffer[1024];

			memset(szTempBuffer,0,sizeof(szTempBuffer));			

			// Format header.
			sprintf(szTempBuffer,_T("SMAPENTRY=%x;\n"),index);

			int idx=strlen(szTempBuffer);
			if (idx>0)
			{
				idx;

				// Format data.
				if (true==convertToCfgStrEntry(
						*pSerEntry,
						&szTempBuffer[idx],
						sizeof(szTempBuffer)-idx-1))
				{	
					if ((int)strlen(szTempBuffer)<maxBufferLength-1)
					{
						strcpy(pszTempBuffer,szTempBuffer);
						fResult=TRUE;
					}
				}			
			}
		}
	}
	return fResult;
}

void CAppCfgSmapLiteClass::dump( )
{
	const int MAX = m_list.getCount();
	for (int i=0;i<MAX;i++)
	{
		CSerSmapLiteEntry* const pSerEntry = GetEntry( i );
		if (pSerEntry)
		{
			enum { MAX_BUFFER_LENGTH = 512 };

			int length = -1;
			char szBuffer[MAX_BUFFER_LENGTH];

			if (format(*pSerEntry,szBuffer,MAX_BUFFER_LENGTH,&length))
			{
				TRACE( szBuffer );
			}
		}
	}
}

int CAppCfgSmapLiteClass::getStartIndex( )
{	
	return 0;
}
/*************************************************************/


/*************************************************************/
/****************** CAppCfgSmapLiteClass_Util ****************/
/*************************************************************/
BOOL CAppCfgSmapLiteClass_Util::createEntry(
	DUOTECNO::DATA::CSerSmapLiteEntry** ppEntry)
{
	BOOL fResult=FALSE;
	CSerSmapLiteEntry* pEntry = 0;
	
	pEntry=new CSerSmapLiteEntry( );
	if (pEntry)
	{
		clear(pEntry);
		*ppEntry=pEntry;
		fResult=TRUE;
	}
	return fResult;
}

BOOL CAppCfgSmapLiteClass_Util::findEntry(
	CAppCfgSmapLiteClass& ref,
	int pageNR,
	int buttonNR,
	DUOTECNO::DATA::CSerSmapLiteEntry** ppEntry)
{
	BOOL fResult=FALSE;
	DUOTECNO::DATA::CSerSmapLiteEntry* p=0;
	const int count = ref.GetMax();

	for(int i=0;i<count;++i)
	{
		p=ref.GetEntry(i);
		if (0!=p)
		{
			if(p->m_pageNR==pageNR &&
				p->m_buttonNR==buttonNR)
			{
				*ppEntry=p;
				fResult=TRUE;
				break;
			}
		}
	}
	return fResult;
}


BOOL CAppCfgSmapLiteClass_Util::addEntry(
	CAppCfgSmapLiteClass& ref,
	DUOTECNO::DATA::CSerSmapLiteEntry* pEntry)
{
	BOOL fResult=FALSE;

	fResult=ref.addEntry(pEntry);
	return fResult;
}

