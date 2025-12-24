#include "stdafx.h"
#include "ConfigGatewayUClass.h"

/*************************************************************/
using DUOTECNO::MFC_HELPER::IReportError;
/*************************************************************/
const char* CConfigGatewayUClass::_szHeader = "GATEWAYCFG";
/*************************************************************/
using DUOTECNO::DATA::CSerGatewayUnivCfgEntry;
/*************************************************************/

static void clear(CSerGatewayUnivCfgEntry* pEntry)
{
	if (pEntry)
	{
  		pEntry->bNodeAddress = 0xFF;
		pEntry->bUnitAddress = 0xFF;
		pEntry->bUnitType = 0xFF;
		pEntry->bFunction = 0xFF;
		pEntry->bSystemInfo = 0xFF;

		for (int i=0;i<4;i++)
		{
      		pEntry->systemAddress[i] = 0xFFFF;
			pEntry->systemAction[i] = 0xFFFF;
			pEntry->systemFunction[i] = 0xFFFF;
			pEntry->systemParams[i] = 0xFFFF;
		}
	}
}

static bool format(
	const CSerGatewayUnivCfgEntry& refEntry,
	char* szBuffer,
	int maxLength,
	int *length)
{
	// CHANGES_PRINTF_PARAM_TYPOS
	char szTempBuffer[512];

	sprintf(szTempBuffer,
			"bDTNodeAddress=0x%02x;"
			"bDTUnitAddress=0x%02x;"
			"bDTUnitType=0x%02x;"
			"bDTFunction=0x%02x;"
			"System=0x%02;"
			"SystemAddress=0x%04x;0x%04x;0x%04x;0x%04x;"
			"SystemAction=0x%04x;0x%04x;0x%04x;0x%04x;"
			"SystemFunction=0x%04x;0x%04x;0x%04x;0x%04x;"
			"SystemParams=0x%04x;0x%04x;0x%04x;0x%04x;",
			refEntry.bNodeAddress,
			refEntry.bUnitAddress,
			refEntry.bUnitType,
			refEntry.bFunction,
			refEntry.bSystemInfo,
			refEntry.systemAddress[0],refEntry.systemAddress[1],refEntry.systemAddress[2],refEntry.systemAddress[3],
			refEntry.systemAction[0],refEntry.systemAction[1],refEntry.systemAction[2],refEntry.systemAction[3],
			refEntry.systemFunction[0],refEntry.systemFunction[1],refEntry.systemFunction[2],refEntry.systemFunction[3],
			refEntry.systemParams[0],refEntry.systemParams[1],refEntry.systemParams[2],refEntry.systemParams[3]); 

	*length = strlen(szTempBuffer);

	if (*length < maxLength)
	{
		strcpy(szBuffer,szTempBuffer);
		return true;
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

static bool helper_parse4(
	 const char* s,
	 unsigned short* pDest,
	 int min,
	 int max,	
	 int* nr)
{
	int i;
	int values[4];
	bool result = false;
	const int len = strlen(s);

	*nr = 0;

	for (i=0;i<len;i++) 
	{
		if (s[i]==';') (*nr)++;
	}

	if (sscanf(s,"%x;%x;%x;%x;",&values[0],&values[1],&values[2],&values[3]) >= 1)
	{
		for (i=0;i<(*nr);i++)
		{
			pDest[i]=values[i];
		}	
		return true;
	}
	return false;
}

/*************************************************************/

CConfigGatewayUClass::CConfigGatewayUClass(IReportError* ptr) :
m_list( ),
m_pCurrentEntry( 0 ),
m_pErrorReport( ptr ),
m_currentIndex( 0 ) //,
// m_currentState(STATE_IDLE)
{
}

CConfigGatewayUClass::~CConfigGatewayUClass(void)
{
	removeAll( );
}

/*************************************************************/

void CConfigGatewayUClass::removeAll( )
{
	while(!m_list.isEmpty())
	{
		CSerGatewayUnivCfgEntry* const p = m_list.removeHead( );
		ASSERT(p);
		delete p;
	}
	ASSERT( m_list.getCount() == 0 );
}

CSerGatewayUnivCfgEntry* CConfigGatewayUClass::GetEntry(int entry)
{
	ASSERT( entry < m_list.getCount() );
	return m_list.getEntry(entry);
}

void CConfigGatewayUClass::ReportError(const char* szMessage)
{
	if (m_pErrorReport)
	{
		m_pErrorReport->reportError( szMessage );
	}
}

/*************************************************************/

BOOL CConfigGatewayUClass::ParseHeader(char *szAttribute,char *szData)
{
	if (strcmp(_szHeader,szAttribute) == 0) 
	{	
		if (sscanf(szData,"%x",&m_currentIndex) == 1)	
		{		
			// m_currentState = STATE_PARSE_CFG;

			if (m_currentIndex < 0xFFFF)
			{
				m_pCurrentEntry = new CSerGatewayUnivCfgEntry( );
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
/*
	else
	{
		STATE_PARSE_CFG = STATE_PARSE_RESERVED;
	}
*/
	return FALSE;
}

BOOL CConfigGatewayUClass::ParseData(char *szAttribute,char *szData)
{
	int value = -1;

	if (!m_pCurrentEntry) 
	{
		ASSERT(m_pCurrentEntry);
		return FALSE;
	}

	if (strcmp(szAttribute,"DTNodeAddress") == 0)
	{
		if (helper_Parse(szData,&m_pCurrentEntry->bNodeAddress,0x03,0xFF) == false)
		{
			CString s;
			s.Format(_T("Error:Index 0x%02x - Attribute '%s' Wrong value"),
					m_currentIndex,
					szAttribute);
			ReportError(s);
		}		
	}
	else if (strcmp(szAttribute,"DTUnitAddress") == 0)
	{
		if (helper_Parse(szData,&m_pCurrentEntry->bUnitAddress,0,0xFF) == false)
		{
			CString s;
			s.Format(_T("Error:Index 0x%02x - Attribute '%s' Wrong value"),
					m_currentIndex,
					szAttribute);
			ReportError(s);
		}		
	}
	else if (strcmp(szAttribute,"DTUnitType") == 0)
	{
		if (helper_Parse(szData,&m_pCurrentEntry->bUnitType,0,0xFF) == false)
		{
			CString s;
			s.Format(_T("Error:Index 0x%02x - Attribute '%s' Wrong value"),
					m_currentIndex,
					szAttribute);
			ReportError(s);
		}			
	}
	else if (strcmp(szAttribute,"DTFunction") == 0)
	{
		if (helper_Parse(szData,&m_pCurrentEntry->bFunction,0,0xFF) == false)
		{
			CString s;
			s.Format(_T("Error:Index 0x%02x - Attribute '%s' Wrong value"),
					m_currentIndex,
					szAttribute);
			ReportError(s);
		}		
	}
	else if (strcmp(szAttribute,"System") == 0)
	{
		if (helper_Parse(szData,&m_pCurrentEntry->bSystemInfo,0,0xFF) == false)
		{
			CString s;
			s.Format(_T("Error:Index 0x%02x - Attribute '%s' Wrong value"),
					m_currentIndex,
					szAttribute);
			ReportError(s);
		}		
	}
	else if (strcmp(szAttribute,"SystemAddress")  == 0)
	{
		int i;

		if (helper_parse4(szData,&m_pCurrentEntry->systemAddress[0],0,0xffff,&i) == false)
		{
			CString s;
			s.Format(_T("Error:Index 0x%02x - Attribute '%s' Wrong value"),
					m_currentIndex,
					szAttribute);
			ReportError(s);
		}	 
	}
	else if (strcmp(szAttribute,"SystemAction")  == 0)
	{
		int i;

		if (helper_parse4(szData,&m_pCurrentEntry->systemAction[0],0,0xffff,&i) == false)
		{
			CString s;
			s.Format(_T("Error:Index 0x%02x - Attribute '%s' Wrong value"),
					m_currentIndex,
					szAttribute);
			ReportError(s);
		}			
	}
	else if (strcmp(szAttribute,"SystemFunction")  == 0)
	{
		int i;

		if (helper_parse4(szData,&m_pCurrentEntry->systemFunction[0],0,0xffff,&i) == false)
		{
			CString s;
			s.Format(_T("Error:Index 0x%02x - Attribute '%s' Wrong value"),
					m_currentIndex,
					szAttribute);
			ReportError(s);
		}			
	}
	else if (strcmp(szAttribute,"SystemParams")  == 0)
	{
		int i;

		if (helper_parse4(szData,&m_pCurrentEntry->systemParams[0],0,0xffff,&i) == false)
		{
			CString s;
			s.Format(_T("Error:Index 0x%02x - Attribute '%s' Wrong value"),
					m_currentIndex,
					szAttribute);
			ReportError(s);
		}				
	}
	else
	{
		CString s;
		s.Format(_T("Error:Index 0x%02x:Unknown Attribute '%s'"),
				 m_currentIndex,
				 szAttribute);

		ReportError(s);
		return FALSE;
	}
	return TRUE;
}

void CConfigGatewayUClass::Initialise(void)
{
	removeAll( );
	m_currentIndex = 0;
}

int CConfigGatewayUClass::GetMax(void) const
{
	return m_list.getCount( );
}

int CConfigGatewayUClass::GetData(int index,BYTE *pbData)
{
	if (index < m_list.getCount( ))
	{
		CSerGatewayUnivCfgEntry* const pSerEntry = GetEntry( index );			
		if (pSerEntry)
		{
			memcpy(pbData,pSerEntry,sizeof(CSerGatewayUnivCfgEntry));
			return (sizeof(CSerGatewayUnivCfgEntry));		
		}
	}	
	pbData = 0;
	return 0;
}

BOOL CConfigGatewayUClass::IsUsed(int index)
{
	if (index < m_list.getCount( ))
	{
		return TRUE;
	}
	return FALSE;
}

void CConfigGatewayUClass::dump( )
{
	const int MAX = m_list.getCount();
	for (int i=0;i<MAX;i++)
	{
		CSerGatewayUnivCfgEntry* const pSerEntry = GetEntry( i );
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

int CConfigGatewayUClass::getStartIndex( )
{
	/*
	CSerIRRXCodeEntry* const pserEntry = GetEntry( 0 );
	if (pserEntry)
	{	
		return (pserEntry->m_usIndex);
	}
	*/
	return 0;
}

/////////////////////////////////////////////////////////////////////////
const char* CConfigGatewayUSystemClass::_szHeader = "GATEWAYSYSTEM";
/////////////////////////////////////////////////////////////////////////
CConfigGatewayUSystemClass::CConfigGatewayUSystemClass(
	DUOTECNO::MFC_HELPER::IReportError* ptr) 
{ 
};

CConfigGatewayUSystemClass::~CConfigGatewayUSystemClass(void) 
{ 
}

BOOL CConfigGatewayUSystemClass::ParseHeader(char *szAttribute,char *szData)
{
	if (strcmp(_szHeader,szAttribute) == 0) 
	{	
		return TRUE;
	}
	return FALSE;
}