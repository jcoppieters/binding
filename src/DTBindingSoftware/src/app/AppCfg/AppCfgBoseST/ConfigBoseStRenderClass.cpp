#include "stdafx.h"
#include "configBoseSTRenderClass.h"

#include "app\appcfg\appcfgbase\ConfigExternalHelper.h"

/*************************************************************/
using DUOTECNO::MFC_HELPER::IReportError;
/*************************************************************/
const char* const CConfigBoseSTCfgRender::_szHeader = "MEDIARENDER";
/*************************************************************/
using DUOTECNO::DATA::CSerBoseSTCfgRender;
/*************************************************************/

/*************************************************************/
/*********************** Helper Functies *********************/
/*************************************************************/

static bool helper_parse(
	const char* szData,
	int maxStringLength,
	char* pszDestination)
{
	enum { MAX_STRING_LENGTH = 64 };
	char szTempName[ MAX_STRING_LENGTH ];
		
	if (CConfigExternalHelper::parseString(szData,
										   szTempName,
										   MAX_STRING_LENGTH-1))
	{	
		CConfigExternalHelper::stripEndOfString(szTempName,
												MAX_STRING_LENGTH-1,
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

static bool helper_Parse(
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

// Oppassen bij porteren van de code.
// Functie is enkel mogelijk omdat een integer een 32 bit waarde is (zoals een unsigned long).
static bool helper_Parse(	
	const char* s,
	unsigned long* pDest,
	unsigned long min,
	unsigned long max)
{
	unsigned int value = 0;

	if (sscanf(s,"%x;",&value) == 1)		
	{
		if (value >= min && value <= max)
		{
			*pDest = (unsigned long) value;
			return true;
		}		
	}	
	return false;
}

#ifdef SKIP

	static bool helper_Parse2(
		 const char* s,
		 unsigned short* pDest,
		 int min,
		 int max,	
		 int* nr)
	{
		int i;
		int values[2];
		bool result = false;
		const int len = strlen(s);

		*nr = 0;

		for (i=0;i<len;i++) 
		{
			if (s[i]==';') (*nr)++;
		}

		if (sscanf(s,"%x;%x;",&values[0],&values[1]) >= 1)
		{
			for (i=0;i<(*nr);i++)
			{
				pDest[i]=values[i];
			}	
			return true;
		}
		return false;
	}

#endif

/*************************************************************/

/*************************************************************/
/******************** CSerBoseSTCfgRender *********************/
/*************************************************************/
static void clear(CSerBoseSTCfgRender* pEntry)
{	
	memset(pEntry,0,sizeof(CSerBoseSTCfgRender));
};

static bool format(
	const CSerBoseSTCfgRender& refEntry,
	char* szBuffer,
	int maxLength,
	int *length)
{
	char szTempBuffer[512];

	sprintf(szTempBuffer,
		    "szName=%s;\n"
			"pbMACAddress=%02X:%02X:%02X:%02X:%02X:%02X;\n"
			"bDestNR=0x%x;\n"
			"ipAddress=0x%x\n",
			refEntry.szName,
			refEntry.pbMACAddress[0],refEntry.pbMACAddress[1],refEntry.pbMACAddress[2],
			refEntry.pbMACAddress[3],refEntry.pbMACAddress[4],refEntry.pbMACAddress[5],
			refEntry.bDestNR,
			refEntry.ipAddress);

	*length = strlen(szTempBuffer);
	if (*length < maxLength)
	{
		strcpy(szBuffer,szTempBuffer);
		return true;
	}
	return false;
}

/*************************************************************/


/*************************************************************/
/*******************CConfigBoseSTCfgRender *******************/
/*************************************************************/
CConfigBoseSTCfgRender::CConfigBoseSTCfgRender(
	DUOTECNO::MFC_HELPER::IReportError* ptr) :
	m_list( ),
	m_pCurrentEntry( 0 ),
	m_pErrorReport( ptr ),
	m_currentIndex( 0 )
{

}

CConfigBoseSTCfgRender::~CConfigBoseSTCfgRender(void)
{
	removeAll( );
}
/*************************************************************/

void CConfigBoseSTCfgRender::removeAll( )
{
	while(!m_list.isEmpty())
	{
		CSerBoseSTCfgRender* const p = m_list.removeHead( );
		ASSERT(p);
		delete p;
	}
	ASSERT( m_list.getCount() == 0 );
}

CSerBoseSTCfgRender* CConfigBoseSTCfgRender::GetEntry(int entry)
{
	ASSERT( entry < m_list.getCount() );
	return m_list.getEntry(entry);
}

void CConfigBoseSTCfgRender::ReportError(const char* szMessage)
{
	if (m_pErrorReport)
	{
		m_pErrorReport->reportError( szMessage );
	}
}

void CConfigBoseSTCfgRender::ParseDataReportError(
	int currentIndex,
	const char* szAttribute)
{
	CString s;
	s.Format(_T("Error:Index 0x%02x - Attribute '%s' Wrong value"),
			 currentIndex,
			 szAttribute);
	ReportError(s);	
}
/*************************************************************/

BOOL CConfigBoseSTCfgRender::ParseHeader(
	char *szAttribute,
	char *szData)
{
	if (strcmp(_szHeader,szAttribute) == 0) 
	{	
		if (sscanf(szData,"%x",&m_currentIndex) == 1)	
		{	
			if (m_currentIndex < 0xFFFF)
			{
				m_pCurrentEntry = new CSerBoseSTCfgRender( );
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


// TM_CHANGES_MAKE_UPPER
// Gaat de attribute converteren naar een temp upper string...
BOOL CConfigBoseSTCfgRender::ParseData(char *szAttribute,char *szData)
{
	using DUOTECNO::DATA::SERCLASS_BOSESTCFG_NAME_LENGTH;

	int value = -1;

	if (!m_pCurrentEntry) 
	{
		ASSERT(m_pCurrentEntry);
		return FALSE;
	}

	CString szAttributeUpper(szAttribute);
	szAttributeUpper.MakeUpper( );

	if (strcmp(szAttributeUpper,"SZNAME") == 0)
	{
		if (helper_parse(szData,SERCLASS_BOSESTCFG_NAME_LENGTH-1,&m_pCurrentEntry->szName[0]) == false)
		{
			ParseDataReportError(m_currentIndex,szAttribute);		
		}	
	}
	else if (strcmp(szAttributeUpper,"PBMACADDRESS") == 0)
	{
		int values[6];

		if (6==sscanf(szData,
			  	   "%02X:%02X:%02X:%02X:%02X:%02X;",
				   &values[0],
				   &values[1],
				   &values[2],
				   &values[3],
				   &values[4],
				   &values[5]))		
		{
			int i;

			for(i=0;i<6;++i)
			{
				if ((values[i]<0)||(values[i]>0xff))
				{
					break;
				}				
				m_pCurrentEntry->pbMACAddress[i]=(BYTE)values[i];
			}

			if (i!=6)
			{
				ParseDataReportError(m_currentIndex,szAttribute);		
			}
		}
		else
		{
			ParseDataReportError(m_currentIndex,szAttribute);	
		}
	}
	else if (strcmp(szAttributeUpper,"BDESTNR") == 0)
	{
		if (helper_Parse(szData,&m_pCurrentEntry->bDestNR,0,0xFF) == false)
		{
			ParseDataReportError(m_currentIndex,szAttribute);	
		}		
	}
	// ipAddress wordt normaal niet geparsed...
	else if (strcmp(szAttributeUpper,"IPADDRESS")==0)
	{
		// TODO --> Is DIT OK --> Is een uint32_t waarde. 
		// een "%X" is de scanf specifier voor een unsigned long.
		// een "%x" is voor een integer

		int value=0;
		if (sscanf(szData,"%X",&value)==1)
		{
			m_pCurrentEntry->ipAddress=(unsigned long) value;
		}
		else
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
	
void CConfigBoseSTCfgRender::Initialise(void)
{
	removeAll( );
	m_currentIndex = 0;
}

int CConfigBoseSTCfgRender::GetMax(void) const
{
	return m_list.getCount( );
}

int CConfigBoseSTCfgRender::GetData(int index,BYTE *pbData)
{
	if (index < m_list.getCount( ))
	{
		CSerBoseSTCfgRender* const pSerEntry = GetEntry( index );			
		if (pSerEntry)
		{
			memcpy(pbData,pSerEntry,sizeof(CSerBoseSTCfgRender));
			return (sizeof(CSerBoseSTCfgRender));		
		}
	}	
	pbData = 0;
	return 0;
}

BOOL CConfigBoseSTCfgRender::IsUsed(int index)
{
	if (index < m_list.getCount( ))
	{
		return TRUE;
	}
	return FALSE;
}

void CConfigBoseSTCfgRender::dump( )
{
	const int MAX = m_list.getCount();
	for (int i=0;i<MAX;i++)
	{
		CSerBoseSTCfgRender* const pSerEntry = GetEntry( i );
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

int CConfigBoseSTCfgRender::getStartIndex( )
{	
	return 0;
}
/*************************************************************/


/*********************** END OF FILE *************************/