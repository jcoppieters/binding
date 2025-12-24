/**********************************************************//**
 * @file	ConfigExternalClass.cpp
 * @brief	implementatie bestand voor de CConfigExternalClass klasse.
 *
 * @author	Mitchell Tom
 * @date	20090423
 *
 * @since	0x0560
 *************************************************************/
#include "stdafx.h"
#include "ConfigUnitCreateClass.h"
#include "app\appcfg\appcfgbase\configExternalhelper.h"
/*************************************************************/
using DUOTECNO::MFC_HELPER::IReportError;
using DUOTECNO::DATA::CSerUnit;
/*************************************************************/
const char* CConfigUnitCreateClass::_szHeader = "UNIT";
/*************************************************************/

/*************************************************************/
CConfigUnitCreateClass::CConfigUnitCreateClass(IReportError* ptr) :
m_list( ),
m_pCurrentUnit( 0 ),
m_pErrorReport( ptr ),
m_currentIndex( 0 )
{
}
CConfigUnitCreateClass::~CConfigUnitCreateClass()
{
	removeAll( );
}
/*************************************************************/
void CConfigUnitCreateClass::removeAll( )
{
	while(!m_list.isEmpty())
	{
		CSerUnit* const p = m_list.removeHead( );
		ASSERT(p);
		delete p;
	}
	ASSERT( m_list.getCount() == 0 );
}

CSerUnit* CConfigUnitCreateClass::GetEntry(int entry)
{
	ASSERT( entry < m_list.getCount() );
	return m_list.getEntry(entry);
}
void CConfigUnitCreateClass::ReportError(const char* szMessage)
{
	if (m_pErrorReport)
	{
		m_pErrorReport->reportError( szMessage );
	}
}
/*************************************************************/
BOOL CConfigUnitCreateClass::ParseHeader(char *szAttribute,char *szData)
{
	if (strcmp(_szHeader,szAttribute) == 0) 
	{			
		if (sscanf(szData,"%x",&m_currentIndex) == 1)		// CHECKED_SSCANF_OK
		{			
			if (m_currentIndex < 256)
			{
				m_pCurrentUnit = new CSerUnit( );
				if (m_pCurrentUnit)
				{
					m_pCurrentUnit->m_bNodeLogicalAddress = 0xFF;

					ASSERT(m_currentIndex >= 0 && m_currentIndex <= 0xFF);				// CHANGES_MVS_2008_WARNINGS			
					m_pCurrentUnit->m_bUnitAddress = static_cast<BYTE>(m_currentIndex);	// CHANGES_MVS_2008_WARNINGS

					m_pCurrentUnit->m_bUnitType = 0xFF;
					m_pCurrentUnit->m_bUnitFlags = 0x00;
					
					// CHANGES_PRINTF_PARAM_TYPOS
					sprintf(m_pCurrentUnit->m_szName,"U%02X",m_currentIndex);

					m_list.addTail( m_pCurrentUnit );

					return TRUE;
				}
				ASSERT( m_pCurrentUnit );
			}	
			else
			{
				char szBuffer[255];
				sprintf(szBuffer,"Error:Unit 0x%02x:to large",m_currentIndex);
				ReportError(szBuffer);	
			}
		}
		else
		{
			char szBuffer[255];
			sprintf(szBuffer,"Error:Unit 0x%02x",m_currentIndex);
			ReportError(szBuffer);
		}
		return FALSE;
	}
	return FALSE;
}

BOOL CConfigUnitCreateClass::ParseData(char *szAttribute,char *szData)
{
	int value = -1;						// CHANGES_MVS_2008_SSCANF

	if (m_pCurrentUnit == NULL) 
	{
		ASSERT(0);
		return FALSE;
	}
	if (strcmp(szAttribute,"bNodeAddress") == 0)
	{
		sscanf(szData,"%x;",&value);				// CHANGES_MVS_2008_SSCANF	
		ASSERT(value >= 0 && value <= 0xFF);		// CHANGES_MVS_2008_SSCANF	
		if (value>=0 && value<=0xFF) {
			m_pCurrentUnit->m_bNodeLogicalAddress = (BYTE)value; 
		}
	}
	else if (strcmp(szAttribute,"bType") == 0)
	{
		sscanf(szData,"%x;",&value);				// CHANGES_MVS_2008_SSCANF	
		ASSERT(value >= 0 && value <= 0xFF);		// CHANGES_MVS_2008_SSCANF	
		if (value>=0 && value<=0xFF) {
			m_pCurrentUnit->m_bUnitType = (BYTE)value;
		}
	}
	else if (strcmp(szAttribute,"bFlags") == 0)
	{
		sscanf(szData,"%x;",&value);				// CHANGES_MVS_2008_SSCANF	
		ASSERT(value >= 0 && value <= 0xFF);		// CHANGES_MVS_2008_SSCANF	
		if (value>=0 && value<=0xFF) {
			m_pCurrentUnit->m_bUnitFlags = (BYTE)value;
		}
	}
	else if (strcmp(szAttribute,"szName") == 0)
	{
		// BUG-0101 en BUG-0105
		// BUG-0159 en (BUG-0160): strncpy gebruiken ipv sprintf( ).
		// Mogelijk probleem wanneer er een '%' in de string aanwezig is.
		enum { MAX_STRING_LENGTH = 16 };
		char szTempName[ MAX_STRING_LENGTH ];
		
		memset(szTempName,'\0',sizeof(szTempName));
		if (CConfigExternalHelper::parseString(szData,szTempName,MAX_STRING_LENGTH-1))
		{	
			CConfigExternalHelper::stripEndOfString(szTempName,MAX_STRING_LENGTH-1,';');		
			memset(m_pCurrentUnit->m_szName,'\0',sizeof(m_pCurrentUnit->m_szName));
			strncpy(m_pCurrentUnit->m_szName,szTempName,sizeof(m_pCurrentUnit->m_szName));			
		}
		else { ASSERT( 0 ); }
	}
	else
	{
		CString s;
		s.Format(_T("Error:Unit 0x%02x:Unknown Attribute '%s'"),m_currentIndex,szAttribute);
		ReportError(s);
		return FALSE;
	}
	return TRUE;
}

void CConfigUnitCreateClass::Initialise(void)
{
	removeAll( );
	m_currentIndex = 0;
}
int CConfigUnitCreateClass::GetMax(void) const
{
	return m_list.getCount( );
}
int CConfigUnitCreateClass::GetData(int index,BYTE *pbData)
{
	if (index < m_list.getCount( ))
	{
		CSerUnit* const pSerUnit = GetEntry( index );			
		if (pSerUnit)
		{
			memcpy(pbData,pSerUnit,sizeof(CSerUnit));
			return (sizeof(CSerUnit));		
		}
	}	
	pbData = 0;
	return 0;
}
BOOL CConfigUnitCreateClass::IsUsed(int index)
{
	if (index < m_list.getCount( ))
	{
		return TRUE;
	}
	return FALSE;
}

void CConfigUnitCreateClass::dump( )
{
	const int MAX = m_list.getCount();
	for (int i=0;i<MAX;i++)
	{
		CSerUnit* const pserUnit = GetEntry( i );
		if (pserUnit)
		{
			char szBuffer[255];

			sprintf(szBuffer, 
					"CSerUnit:Name=%s,(0x%02x,0x%02x),Type=%02x,Flags=%02x\n",
					 pserUnit->m_szName,
					 pserUnit->m_bNodeLogicalAddress,
					 pserUnit->m_bUnitAddress,
					 pserUnit->m_bUnitType,
					 pserUnit->m_bUnitFlags);

			TRACE( szBuffer );
		}
	}
}

int CConfigUnitCreateClass::getStartIndex( )
{
	CSerUnit* const pserUnit = GetEntry( 0 );
	if (pserUnit)
	{	
		return (pserUnit->m_bUnitAddress);
	}
	return 0;
}



