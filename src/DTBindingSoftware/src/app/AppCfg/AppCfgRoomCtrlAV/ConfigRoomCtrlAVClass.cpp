#include "stdafx.h"
#include "configRoomCtrlAVClass.h"

#include "app\appcfg\appcfgbase\ConfigExternalHelper.h"

/*************************************************************/
using DUOTECNO::MFC_HELPER::IReportError;
/*************************************************************/
const char* const CConfigRoomCtrlAVDest::_szHeader = "DEST";
const char* const CConfigRoomCtrlAVSource::_szHeader = "SOURCE";
const char* const CConfigRoomCtrlTunerPreset::_szHeader = "TUNERPRESET";
const char* const CConfigRoomCtrlAVFunction::_szHeader = "FUNCTION";
/*************************************************************/
using DUOTECNO::DATA::CSerRoomCtrlAVDest;
using DUOTECNO::DATA::CSerRoomCtrlAVSource;
using DUOTECNO::DATA::CSerRoomCtrlAVTunerPreset;
using DUOTECNO::DATA::CSerRoomCtrlAVFunction;
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
																		// <-- memset
	if (CConfigExternalHelper::parseString(szData,
										   szTempName,
										   maxStringLength))			// <-- MAX_STRING_LENGTH-1 ???
	{	
		CConfigExternalHelper::stripEndOfString(szTempName,
												maxStringLength,		// <-- MAX_STRING_LENGTH-1 ???
												';');
		
		const int stringLength = strlen(szTempName);
		if (stringLength < maxStringLength)
		{
			sprintf(pszDestination, szTempName);			// strcpy
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

// CHANGES_BUG_PARSEVALUES
// Er mogen maar 2 waarden geparsed worden ipv nr...
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

	if (*nr==2)
	{
		if (sscanf(s,"%x;%x;",&values[0],&values[1])==2)
		{
			for (i=0;i<2;i++)
			{
				pDest[i]=values[i];
			}	
			return true;
		}
	}
	return false;
}
/*************************************************************/

/*************************************************************/
/******************** CSerRoomCtrlAVDest *********************/
/*************************************************************/
static void clear(CSerRoomCtrlAVDest* pEntry)
{
	using DUOTECNO::DATA::AUDIOCFG_DESTNAME_MAX;

	int i;

	for (i=0;i<AUDIOCFG_DESTNAME_MAX;i++)
	{
		pEntry->m_szName[i] = 0;
	}
	pEntry->m_bDestNR = 0xFF;
	pEntry->m_bMaxVolume = 0;
	pEntry->m_bConfig = 0;
	pEntry->m_bType = 0;
	pEntry->m_bAudioUnit[0] = 0xff;
	pEntry->m_bAudioUnit[1] = 0xff;
};

static bool format(
	const CSerRoomCtrlAVDest& refEntry,
	char* szBuffer,
	int maxLength,
	int *length)
{
	char szTempBuffer[512];

	sprintf(szTempBuffer,
		    "szName=%s;\n"
			"bAddress=0x%02x;0x%02x;\n"
			"bDestNR=0x%02x;\n"
			"bMaxVolume=0x%02x;\n"
			"bType=0x%02x;\n"
			"bConfig=0x%02x;\n",
			refEntry.m_szName,
			refEntry.m_bAudioUnit[0],refEntry.m_bAudioUnit[1],
			refEntry.m_bDestNR,
			refEntry.m_bMaxVolume,
			refEntry.m_bType,
			refEntry.m_bConfig);

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
/******************* CSerRoomCtrlAVSource ********************/
/*************************************************************/
static void clear(CSerRoomCtrlAVSource* pEntry)
{
	using DUOTECNO::DATA::AUDIOCFG_SOURCENAME_MAX;
	using DUOTECNO::DATA::AUDIOCFG_SOURCE_MAX_FUNC;

	int i;	

	for (i=0;i<AUDIOCFG_SOURCENAME_MAX;i++)
	{
		pEntry->m_szName[i] = 0;
	}
	for (i=0;i<AUDIOCFG_SOURCE_MAX_FUNC;i++)
	{
		pEntry->m_bFunctionIdx[i] = 0xFF;
	}

	pEntry->m_bSourceNR = 0xFF;
	pEntry->m_bConfig = 0;
};

static bool format(
	const CSerRoomCtrlAVSource& refEntry,
	char* szBuffer,
	int maxLength,
	int *length)
{
	using DUOTECNO::DATA::AUDIOCFG_SOURCE_MAX_FUNC;

	char szTempBuffer[2*1024];		// Buffer van 2K

	sprintf(szTempBuffer,
			"szName=%s;\n"
			"bSourceNR=0x%02x;\n"
			"bConfig=0x%02x;\n",
			refEntry.m_szName,
			refEntry.m_bSourceNR,
			refEntry.m_bConfig);

	for (int i=0;i<AUDIOCFG_SOURCE_MAX_FUNC;i++)
	{
		if (refEntry.m_bFunctionIdx[i] != 0xFF)
		{
			char szTempEntry[64];

			sprintf(szTempEntry,
					"bFunction_%02d=0x%02x;\n",
					i,
					refEntry.m_bFunctionIdx[i]);

			strcat(szTempBuffer,szTempEntry);
		}
	}

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
/***************** CSerRoomCtrlAVTunerPreset *****************/
/*************************************************************/
static void clear(CSerRoomCtrlAVTunerPreset* pEntry)
{
	using DUOTECNO::DATA::AUDIOCFG_TUNERPRESETNAME_MAX;

	int i;	
	
	for (i=0;i<AUDIOCFG_TUNERPRESETNAME_MAX;i++)
	{
		pEntry->m_szName[i] = 0;
	}
	pEntry->m_ulTunerFrequency = 0;
	pEntry->m_bTunerPreset = 0xFF;
	pEntry->m_bSourceNr = 0xFF;
	pEntry->m_bConfig = 0;
};

static bool format(
	const CSerRoomCtrlAVTunerPreset& refEntry,
	char* szBuffer,
	int maxLength,
	int *length)
{
	char szTempBuffer[512];		

	// CHANGES_PRINTF_PARAM_TYPOS
	
	sprintf(szTempBuffer,
			"szName=%s;\n"
			"bTunerPreset=0x%02x;\n"
			"bSourceNR=0x%02x;\n"
			"bConfig=0x%02x;\n"
			"ulTunerFreq=0x%06x;\n",
			refEntry.m_szName,
			refEntry.m_bTunerPreset,
			refEntry.m_bSourceNr,
			refEntry.m_bConfig,
			refEntry.m_ulTunerFrequency);

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
/****************** CSerRoomCtrlAVFunction *******************/
/*************************************************************/
static void clear(CSerRoomCtrlAVFunction* pEntry)
{
	using DUOTECNO::DATA::AUDIOCFG_FUNCTIONNAME_MAX;

	int i;	

	for (i=0;i<AUDIOCFG_FUNCTIONNAME_MAX;i++)
	{
		pEntry->m_szName[i] = 0x00;
	}
	pEntry->m_bFunction = 0xFF;	
};

static bool format(
	const CSerRoomCtrlAVFunction& refEntry,
	char* szBuffer,
	int maxLength,
	int *length)
{
	char szTempBuffer[512];	

	sprintf(szTempBuffer,
			"szName=%s;\n"
			"bFunction=0x%02x;\n",
			refEntry.m_szName,
			refEntry.m_bFunction);

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
/********************CConfigRoomCtrlAVDest *******************/
/*************************************************************/
CConfigRoomCtrlAVDest::CConfigRoomCtrlAVDest(
	DUOTECNO::MFC_HELPER::IReportError* ptr) :
	m_list( ),
	m_pCurrentEntry( 0 ),
	m_pErrorReport( ptr ),
	m_currentIndex( 0 )
{

}

CConfigRoomCtrlAVDest::~CConfigRoomCtrlAVDest(void)
{
	removeAll( );
}
/*************************************************************/

void CConfigRoomCtrlAVDest::removeAll( )
{
	while(!m_list.isEmpty())
	{
		CSerRoomCtrlAVDest* const p = m_list.removeHead( );
		ASSERT(p);
		delete p;
	}
	ASSERT( m_list.getCount() == 0 );
}

CSerRoomCtrlAVDest* CConfigRoomCtrlAVDest::GetEntry(int entry)
{
	ASSERT( entry < m_list.getCount() );
	return m_list.getEntry(entry);
}

void CConfigRoomCtrlAVDest::ReportError(const char* szMessage)
{
	if (m_pErrorReport)
	{
		m_pErrorReport->reportError( szMessage );
	}
}

void CConfigRoomCtrlAVDest::ParseDataReportError(
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

BOOL CConfigRoomCtrlAVDest::ParseHeader(
	char *szAttribute,
	char *szData)
{
	if (strcmp(_szHeader,szAttribute) == 0) 
	{	
		if (sscanf(szData,"%x",&m_currentIndex) == 1)	
		{	
			if (m_currentIndex < 0xFFFF)
			{
				m_pCurrentEntry = new CSerRoomCtrlAVDest( );
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

BOOL CConfigRoomCtrlAVDest::ParseData(char *szAttribute,char *szData)
{
	using DUOTECNO::DATA::AUDIOCFG_DESTNAME_MAX;

	int value = -1;

	if (!m_pCurrentEntry) 
	{
		ASSERT(m_pCurrentEntry);
		return FALSE;
	}

	if (strcmp(szAttribute,"szName") == 0)
	{
		if (helper_parse(szData,AUDIOCFG_DESTNAME_MAX-1,&m_pCurrentEntry->m_szName[0]) == false)
		{
			ParseDataReportError(m_currentIndex,szAttribute);		
		}	
	}
	else if (strcmp(szAttribute,"bAddress") == 0)
	{
		int cnt = -1;
		unsigned short usValues[2];

		if (helper_Parse2(szData,&usValues[0],0x03,0xFF,&cnt) == false)
		{			
			ParseDataReportError(m_currentIndex,szAttribute);		
		}		
		else
		{
			// Moeten 2 parameters parsen: Node & Unit Address.
			if (cnt == 2)
			{
				m_pCurrentEntry->m_bAudioUnit[0] = (BYTE) usValues[0];
				m_pCurrentEntry->m_bAudioUnit[1] = (BYTE) usValues[1];
			}
			else
			{
				ParseDataReportError(m_currentIndex,szAttribute);	
			}
		}
	}
	else if (strcmp(szAttribute,"bDestNR") == 0)
	{
		if (helper_Parse(szData,&m_pCurrentEntry->m_bDestNR,0,0xFF) == false)
		{
			ParseDataReportError(m_currentIndex,szAttribute);	
		}		
	}
	else if (strcmp(szAttribute,"bMaxVolume") == 0)
	{
		if (helper_Parse(szData,&m_pCurrentEntry->m_bMaxVolume,0,0xFF) == false)
		{
			ParseDataReportError(m_currentIndex,szAttribute);	
		}		
	}
	else if (strcmp(szAttribute,"bType") == 0)
	{
		if (helper_Parse(szData,&m_pCurrentEntry->m_bType,0,0xFF) == false)
		{
			ParseDataReportError(m_currentIndex,szAttribute);	
		}		
	}
	else if (strcmp(szAttribute,"bConfig") == 0)
	{
		if (helper_Parse(szData,&m_pCurrentEntry->m_bConfig,0,0xFF) == false)
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
	
void CConfigRoomCtrlAVDest::Initialise(void)
{
	removeAll( );
	m_currentIndex = 0;
}

int CConfigRoomCtrlAVDest::GetMax(void) const
{
	return m_list.getCount( );
}

int CConfigRoomCtrlAVDest::GetData(int index,BYTE *pbData)
{
	if (index < m_list.getCount( ))
	{
		CSerRoomCtrlAVDest* const pSerEntry = GetEntry( index );			
		if (pSerEntry)
		{
			memcpy(pbData,pSerEntry,sizeof(CSerRoomCtrlAVDest));
			return (sizeof(CSerRoomCtrlAVDest));		
		}
	}	
	pbData = 0;
	return 0;
}

BOOL CConfigRoomCtrlAVDest::IsUsed(int index)
{
	if (index < m_list.getCount( ))
	{
		return TRUE;
	}
	return FALSE;
}

void CConfigRoomCtrlAVDest::dump( )
{
	const int MAX = m_list.getCount();
	for (int i=0;i<MAX;i++)
	{
		CSerRoomCtrlAVDest* const pSerEntry = GetEntry( i );
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

int CConfigRoomCtrlAVDest::getStartIndex( )
{	
	return 0;
}
/*************************************************************/

/*************************************************************/
/*******************CConfigRoomCtrlAVSource ******************/
/*************************************************************/
CConfigRoomCtrlAVSource::CConfigRoomCtrlAVSource(
	DUOTECNO::MFC_HELPER::IReportError* ptr) :
	m_list( ),
	m_pCurrentEntry( 0 ),
	m_pErrorReport( ptr ),
	m_currentIndex( 0 ),
	m_currentFunctionIdx(0)
{

}

CConfigRoomCtrlAVSource::~CConfigRoomCtrlAVSource(void)
{
	removeAll( );
}
/*************************************************************/

void CConfigRoomCtrlAVSource::removeAll( )
{
	while(!m_list.isEmpty())
	{
		CSerRoomCtrlAVSource* const p = m_list.removeHead( );
		ASSERT(p);
		delete p;
	}
	ASSERT( m_list.getCount() == 0 );
}

CSerRoomCtrlAVSource* CConfigRoomCtrlAVSource::GetEntry(int entry)
{
	ASSERT( entry < m_list.getCount() );
	return m_list.getEntry(entry);
}

void CConfigRoomCtrlAVSource::ReportError(const char* szMessage)
{
	if (m_pErrorReport)
	{
		m_pErrorReport->reportError( szMessage );
	}
}

void CConfigRoomCtrlAVSource::ParseDataReportError(
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

BOOL CConfigRoomCtrlAVSource::ParseHeader(
	char *szAttribute,
	char *szData)
{
	if (strcmp(_szHeader,szAttribute) == 0) 
	{	
		if (sscanf(szData,"%x",&m_currentIndex) == 1)	
		{	
			if (m_currentIndex < 0xFFFF)
			{				
				m_pCurrentEntry = new CSerRoomCtrlAVSource( );
				if (m_pCurrentEntry)
				{
					m_currentFunctionIdx = 0;		// !!!!
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

BOOL CConfigRoomCtrlAVSource::ParseData(char *szAttribute,char *szData)
{
	using DUOTECNO::DATA::AUDIOCFG_SOURCENAME_MAX;
	using DUOTECNO::DATA::AUDIOCFG_SOURCE_MAX_FUNC;

	int value = -1;

	if (!m_pCurrentEntry) 
	{
		ASSERT(m_pCurrentEntry);
		return FALSE;
	}

	if (strcmp(szAttribute,"szName") == 0)
	{
		if (helper_parse(szData,AUDIOCFG_SOURCENAME_MAX-1,&m_pCurrentEntry->m_szName[0]) == false)
		{
			ParseDataReportError(m_currentIndex,szAttribute);		
		}	
	}
	else if (strcmp(szAttribute,"bSource") == 0)
	{
		if (helper_Parse(szData,&m_pCurrentEntry->m_bSourceNR,0,0xFF) == false)
		{
			ParseDataReportError(m_currentIndex,szAttribute);	
		}		
	}
	else if (strcmp(szAttribute,"bConfig") == 0)
	{
		if (helper_Parse(szData,&m_pCurrentEntry->m_bConfig,0,0xFF) == false)
		{
			ParseDataReportError(m_currentIndex,szAttribute);	
		}		
	}
	else
	{
		BOOL fError = FALSE;

		if (m_currentFunctionIdx < AUDIOCFG_SOURCE_MAX_FUNC)
		{
			char szTempBuffer[64];

			sprintf(szTempBuffer,"bFunction_%02d",m_currentFunctionIdx);

			if (strcmp(szAttribute,szTempBuffer) == 0)
			{
				if (helper_Parse(szData,&m_pCurrentEntry->m_bFunctionIdx[m_currentFunctionIdx],0,0xFF) == false)
				{
					ParseDataReportError(m_currentIndex,szAttribute);	
				}	

				m_currentFunctionIdx++;
			}
			else
			{
				fError = TRUE;	
			}
		}
		else
		{
			fError = TRUE;
		}

		if (fError == TRUE)
		{
			ParseDataReportError(m_currentIndex,szAttribute);
			return FALSE;
		}
	}	
	return TRUE;
}
	
void CConfigRoomCtrlAVSource::Initialise(void)
{
	removeAll( );
	m_currentIndex = 0;
}

int CConfigRoomCtrlAVSource::GetMax(void) const
{
	return m_list.getCount( );
}

int CConfigRoomCtrlAVSource::GetData(int index,BYTE *pbData)
{
	if (index < m_list.getCount( ))
	{
		CSerRoomCtrlAVSource* const pSerEntry = GetEntry( index );			
		if (pSerEntry)
		{
			memcpy(pbData,pSerEntry,sizeof(CSerRoomCtrlAVSource));
			return (sizeof(CSerRoomCtrlAVSource));		
		}
	}	
	pbData = 0;
	return 0;
}

BOOL CConfigRoomCtrlAVSource::IsUsed(int index)
{
	if (index < m_list.getCount( ))
	{
		return TRUE;
	}
	return FALSE;
}

void CConfigRoomCtrlAVSource::dump( )
{
	const int MAX = m_list.getCount();
	for (int i=0;i<MAX;i++)
	{
		CSerRoomCtrlAVSource* const pSerEntry = GetEntry( i );
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

int CConfigRoomCtrlAVSource::getStartIndex( )
{	
	return 0;
}
/*************************************************************/


/*************************************************************/
/******************CConfigRoomCtrlTunerPreset ****************/
/*************************************************************/
CConfigRoomCtrlTunerPreset::CConfigRoomCtrlTunerPreset(
	DUOTECNO::MFC_HELPER::IReportError* ptr) :
	m_list( ),
	m_pCurrentEntry( 0 ),
	m_pErrorReport( ptr ),
	m_currentIndex( 0 )
{

}

CConfigRoomCtrlTunerPreset::~CConfigRoomCtrlTunerPreset(void)
{
	removeAll( );
}
/*************************************************************/

void CConfigRoomCtrlTunerPreset::removeAll( )
{
	while(!m_list.isEmpty())
	{
		CSerRoomCtrlAVTunerPreset* const p = m_list.removeHead( );
		ASSERT(p);
		delete p;
	}
	ASSERT( m_list.getCount() == 0 );
}

CSerRoomCtrlAVTunerPreset* CConfigRoomCtrlTunerPreset::GetEntry(int entry)
{
	ASSERT( entry < m_list.getCount() );
	return m_list.getEntry(entry);
}

void CConfigRoomCtrlTunerPreset::ReportError(const char* szMessage)
{
	if (m_pErrorReport)
	{
		m_pErrorReport->reportError( szMessage );
	}
}

void CConfigRoomCtrlTunerPreset::ParseDataReportError(
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

BOOL CConfigRoomCtrlTunerPreset::ParseHeader(
	char *szAttribute,
	char *szData)
{
	if (strcmp(_szHeader,szAttribute) == 0) 
	{	
		if (sscanf(szData,"%x",&m_currentIndex) == 1)	
		{	
			if (m_currentIndex < 0xFFFF)
			{
				m_pCurrentEntry = new CSerRoomCtrlAVTunerPreset( );
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

BOOL CConfigRoomCtrlTunerPreset::ParseData(char *szAttribute,char *szData)
{
	using DUOTECNO::DATA::AUDIOCFG_DESTNAME_MAX;

	int value = -1;

	if (!m_pCurrentEntry) 
	{
		ASSERT(m_pCurrentEntry);
		return FALSE;
	}

	if (strcmp(szAttribute,"szName") == 0)
	{
		if (helper_parse(szData,AUDIOCFG_DESTNAME_MAX-1,&m_pCurrentEntry->m_szName[0]) == false)
		{
			ParseDataReportError(m_currentIndex,szAttribute);		
		}	
	}
	else if (strcmp(szAttribute,"bTunerPreset") == 0)
	{
		if (helper_Parse(szData,&m_pCurrentEntry->m_bTunerPreset,0,0xFF) == false)
		{
			ParseDataReportError(m_currentIndex,szAttribute);	
		}		
	}
	else if (strcmp(szAttribute,"bSourceNR") == 0)
	{
		if (helper_Parse(szData,&m_pCurrentEntry->m_bSourceNr,0,0xFF) == false)
		{
			ParseDataReportError(m_currentIndex,szAttribute);	
		}		
	}
	else if (strcmp(szAttribute,"bConfig") == 0)
	{
		if (helper_Parse(szData,&m_pCurrentEntry->m_bConfig,0,0xFF) == false)
		{
			ParseDataReportError(m_currentIndex,szAttribute);	
		}		
	}
	else if (strcmp(szAttribute,"ulTunerFreq") == 0)
	{
		if (helper_Parse(szData,&m_pCurrentEntry->m_ulTunerFrequency,0,0xFFFFFFul) == false)
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
	
void CConfigRoomCtrlTunerPreset::Initialise(void)
{
	removeAll( );
	m_currentIndex = 0;
}

int CConfigRoomCtrlTunerPreset::GetMax(void) const
{
	return m_list.getCount( );
}

int CConfigRoomCtrlTunerPreset::GetData(int index,BYTE *pbData)
{
	if (index < m_list.getCount( ))
	{
		CSerRoomCtrlAVTunerPreset* const pSerEntry = GetEntry( index );			
		if (pSerEntry)
		{
			memcpy(pbData,pSerEntry,sizeof(CSerRoomCtrlAVTunerPreset));
			return (sizeof(CSerRoomCtrlAVTunerPreset));		
		}
	}	
	pbData = 0;
	return 0;
}

BOOL CConfigRoomCtrlTunerPreset::IsUsed(int index)
{
	if (index < m_list.getCount( ))
	{
		return TRUE;
	}
	return FALSE;
}

void CConfigRoomCtrlTunerPreset::dump( )
{
	const int MAX = m_list.getCount();
	for (int i=0;i<MAX;i++)
	{
		CSerRoomCtrlAVTunerPreset* const pSerEntry = GetEntry( i );
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

int CConfigRoomCtrlTunerPreset::getStartIndex( )
{	
	return 0;
}
/*************************************************************/

/*************************************************************/
/******************CConfigRoomCtrlAVFunction *****************/
/*************************************************************/
CConfigRoomCtrlAVFunction::CConfigRoomCtrlAVFunction(
	DUOTECNO::MFC_HELPER::IReportError* ptr) :
	m_list( ),
	m_pCurrentEntry( 0 ),
	m_pErrorReport( ptr ),
	m_currentIndex( 0 )
{

}

CConfigRoomCtrlAVFunction::~CConfigRoomCtrlAVFunction(void)
{
	removeAll( );
}
/*************************************************************/

void CConfigRoomCtrlAVFunction::removeAll( )
{
	while(!m_list.isEmpty())
	{
		CSerRoomCtrlAVFunction* const p = m_list.removeHead( );
		ASSERT(p);
		delete p;
	}
	ASSERT( m_list.getCount() == 0 );
}

CSerRoomCtrlAVFunction* CConfigRoomCtrlAVFunction::GetEntry(int entry)
{
	ASSERT( entry < m_list.getCount() );
	return m_list.getEntry(entry);
}

void CConfigRoomCtrlAVFunction::ReportError(const char* szMessage)
{
	if (m_pErrorReport)
	{
		m_pErrorReport->reportError( szMessage );
	}
}

void CConfigRoomCtrlAVFunction::ParseDataReportError(
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

BOOL CConfigRoomCtrlAVFunction::ParseHeader(
	char *szAttribute,
	char *szData)
{
	if (strcmp(_szHeader,szAttribute) == 0) 
	{	
		if (sscanf(szData,"%x",&m_currentIndex) == 1)	
		{	
			if (m_currentIndex < 0xFFFF)
			{
				m_pCurrentEntry = new CSerRoomCtrlAVFunction( );
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

BOOL CConfigRoomCtrlAVFunction::ParseData(char *szAttribute,char *szData)
{
	using DUOTECNO::DATA::AUDIOCFG_DESTNAME_MAX;

	int value = -1;

	if (!m_pCurrentEntry) 
	{
		ASSERT(m_pCurrentEntry);
		return FALSE;
	}

	if (strcmp(szAttribute,"szName") == 0)
	{
		if (helper_parse(szData,AUDIOCFG_DESTNAME_MAX-1,&m_pCurrentEntry->m_szName[0]) == false)
		{
			ParseDataReportError(m_currentIndex,szAttribute);		
		}	
	}
	else if (strcmp(szAttribute,"bFunction") == 0)
	{
		if (helper_Parse(szData,&m_pCurrentEntry->m_bFunction,0,0xFF) == false)
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
	
void CConfigRoomCtrlAVFunction::Initialise(void)
{
	removeAll( );
	m_currentIndex = 0;
}

int CConfigRoomCtrlAVFunction::GetMax(void) const
{
	return m_list.getCount( );
}

int CConfigRoomCtrlAVFunction::GetData(int index,BYTE *pbData)
{
	if (index < m_list.getCount( ))
	{
		CSerRoomCtrlAVFunction* const pSerEntry = GetEntry( index );			
		if (pSerEntry)
		{
			memcpy(pbData,pSerEntry,sizeof(CSerRoomCtrlAVFunction));
			return (sizeof(CSerRoomCtrlAVFunction));		
		}
	}	
	pbData = 0;
	return 0;
}

BOOL CConfigRoomCtrlAVFunction::IsUsed(int index)
{
	if (index < m_list.getCount( ))
	{
		return TRUE;
	}
	return FALSE;
}

void CConfigRoomCtrlAVFunction::dump( )
{
	const int MAX = m_list.getCount();
	for (int i=0;i<MAX;i++)
	{
		CSerRoomCtrlAVFunction* const pSerEntry = GetEntry( i );
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

int CConfigRoomCtrlAVFunction::getStartIndex( )
{	
	return 0;
}
/*************************************************************/

/*********************** END OF FILE *************************/