/**********************************************************//**
 * @file	ConfigUnitIRRXClass.cpp
 * @brief	implementatie bestand voor de CConfigExternalClass klasse.
 *
 * @author	Mitchell Tom
 * @date	20090930
 *
 * @since	0x0560
 *************************************************************/
#include "stdafx.h"
#include "configIRRXClass.h"
/*************************************************************/

/*************************************************************/
using DUOTECNO::MFC_HELPER::IReportError;
/*************************************************************/
const char* CConfigIRRXClass::_szHeader = "IRCODE";
/*************************************************************/
using DUOTECNO::DATA::CSerIRRXCodeEntry;
/*************************************************************/

/*************************************************************/
/******************* CConfigIRRXClass ************************/
/*************************************************************/
CConfigIRRXClass::CConfigIRRXClass(IReportError* ptr) :
m_list( ),
m_pCurrentEntry( 0 ),
m_pErrorReport( ptr ),
m_currentIndex( 0 )
{
}
CConfigIRRXClass::~CConfigIRRXClass()
{
	removeAll( );
}
/*************************************************************/
void CConfigIRRXClass::removeAll( )
{
	while(!m_list.isEmpty())
	{
		CSerIRRXCodeEntry* const p = m_list.removeHead( );
		ASSERT(p);
		delete p;
	}
	ASSERT( m_list.getCount() == 0 );
}

CSerIRRXCodeEntry* CConfigIRRXClass::GetEntry(int entry)
{

	ASSERT( entry < m_list.getCount() );
	return m_list.getEntry(entry);
}
void CConfigIRRXClass::ReportError(const char* szMessage)
{
	if (m_pErrorReport)
	{
		m_pErrorReport->reportError( szMessage );
	}
}
/*************************************************************/
BOOL CConfigIRRXClass::ParseHeader(char *szAttribute,char *szData)
{
	if (strcmp(_szHeader,szAttribute) == 0) 
	{			
		if (sscanf(szData,"%x",&m_currentIndex) == 1)	// CHANGES_MVS_2008_SSCANF
		{		
			if (m_currentIndex < 256)
			{
				m_pCurrentEntry = new CSerIRRXCodeEntry( );
				if (m_pCurrentEntry)
				{
					m_pCurrentEntry->m_usIndex = m_currentIndex;
					m_pCurrentEntry->m_usIrCode = 0xFFFF;
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

BOOL CConfigIRRXClass::ParseData(char *szAttribute,char *szData)
{
	int value = -1;
	if (!m_pCurrentEntry) 
	{
		ASSERT(m_pCurrentEntry);
		return FALSE;
	}

	if (strcmp(szAttribute,"usCode") == 0)
	{
		if (sscanf(szData,"%x;",&value) == 1)		// CHANGES_MVS_2008_SSCANF			
		{
			m_pCurrentEntry->m_usIrCode = (unsigned short) value;
		}
		else
		{
			ASSERT( value != -1);
		}
	}
	else
	{
		CString s;
		s.Format(_T("Error:Unit 0x%02x:Unknown Attribute '%s'"),
				 m_currentIndex,
				 szAttribute);

		ReportError(s);
		return FALSE;
	}
	return TRUE;
}

void CConfigIRRXClass::Initialise(void)
{
	removeAll( );
	m_currentIndex = 0;
}
int CConfigIRRXClass::GetMax(void) const
{
	return m_list.getCount( );
}
int CConfigIRRXClass::GetData(int index,BYTE *pbData)
{
	if (index < m_list.getCount( ))
	{
		CSerIRRXCodeEntry* const pSerEntry = GetEntry( index );			
		if (pSerEntry)
		{
			memcpy(pbData,pSerEntry,sizeof(CSerIRRXCodeEntry));
			return (sizeof(CSerIRRXCodeEntry));		
		}
	}	
	pbData = 0;
	return 0;
}
BOOL CConfigIRRXClass::IsUsed(int index)
{
	if (index < m_list.getCount( ))
	{
		return TRUE;
	}
	return FALSE;
}

void CConfigIRRXClass::dump( )
{
	const int MAX = m_list.getCount();
	for (int i=0;i<MAX;i++)
	{
		CSerIRRXCodeEntry* const pSerEntry = GetEntry( i );
		if (pSerEntry)
		{
			char szBuffer[255];

			sprintf(szBuffer, 
					"CSerIRRXCode:(0x%02x,0x%04x)\n",
					pSerEntry->m_usIndex,
					pSerEntry->m_usIrCode);

			TRACE( szBuffer );
		}
	}
}

int CConfigIRRXClass::getStartIndex( )
{
	CSerIRRXCodeEntry* const pserEntry = GetEntry( 0 );
	if (pserEntry)
	{	
		return (pserEntry->m_usIndex);
	}
	return 0;
}



