/**********************************************************//**
 * @file	ConfigMoodClass.cpp
 * @brief	Implementatie bestand voor de mood config klassen.
 *
 * @author	Mitchell Tom
 * @date	20090427
 *
 * @since	0x0560
 *
 * @attention	
 *	MoodEntry packet moet de APDU header initialiseren.
 *************************************************************/
#include "stdafx.h"
#include "ConfigMoodClass.h"
/*************************************************************/
#include "APDU_def.h"
/*************************************************************/
using DUOTECNO::MFC_HELPER::IReportError;
/*************************************************************/
using DUOTECNO::DATA::CSerMoodHeader;
using DUOTECNO::DATA::CSerMoodEntry;
using DUOTECNO::DATA::CSerMessagePacket;
using DUOTECNO::COM::APDUHeader;
/*************************************************************/
const char* CConfigMoodClass::_szHeader = "MOOD";
/*************************************************************/

/*************************************************************/
/************************ CMoodHeader ************************/
/*************************************************************/
CMoodHeader::CMoodHeader() : 
m_list( )
{
	init( );
}
CMoodHeader::~CMoodHeader()
{
	removeAll( );
}
void CMoodHeader::removeAll( )
{
	while (!m_list.isEmpty())
	{
		CMoodEntry* const p = m_list.removeHead( );
		ASSERT( p );
		delete p;
	}
	ASSERT( m_list.getCount( ) == 0 );
}
void CMoodHeader::init( )
{
	m_header.m_count = 0;
	m_header.m_identifier = 0;
}

void CMoodHeader::add(CMoodEntry* pEntry)
{
	m_list.addTail(pEntry);
	m_header.m_count++;
}
CMoodEntry* CMoodHeader::get(int index)
{
	ASSERT( index < m_list.getCount( ) );
	return m_list.getEntry( index );
}

void CMoodHeader::dump( )
{
	char szBuffer[255];

	const int count = getCount( );

	sprintf(szBuffer,
			"MOOD=%02x,count=%d\n",
			m_header.m_identifier,
			count);

	TRACE(szBuffer);

	for (int i=0;i<count;i++)
	{
		CMoodEntry* const pEntry = get( i );
		if (pEntry)
		{
			pEntry->dump( );
		}
	}
}

/*************************************************************/
/************************ CMoodEntry *************************/
/*************************************************************/
CMoodEntry::CMoodEntry()
{
	init( );
}
CMoodEntry::~CMoodEntry()
{
}
void CMoodEntry::removeAll( )
{
}
void CMoodEntry::init( )
{
	m_entry.m_delay = 0;

	memset(&m_entry.m_packet,0,sizeof(CSerMessagePacket));

	/** @attention	APDUHeader initialiseren ... */
	m_entry.m_packet.bAPDUHeader = 
			APDUHeader::APDU_BASIC_PACKET |
   			APDUHeader::APDU_IMPLICIT_INVOKE;
}
void CMoodEntry::dump( )
{
	char szBuffer[255];
	
	sprintf(szBuffer,
		    "entry=%02x%02x%02x%02x",
			m_entry.m_packet.bRemoteAddress,
			m_entry.m_packet.bDestinationUnitNR,
			m_entry.m_packet.bMessageCode,
			m_entry.m_packet.bLength);

	TRACE( szBuffer );

	const int len = m_entry.m_packet.bLength;
	for (int i=0;i<len;i++)
	{
		sprintf(szBuffer,
				"%02x",
				m_entry.m_packet.pbData[i]);

		TRACE( szBuffer );
	}

	if (m_entry.m_delay != 0)
	{
		sprintf(szBuffer,
				";%02x;\n",
				m_entry.m_delay);

		TRACE(szBuffer);
	}
	else
	{
		TRACE(";\n");
	}
}
bool CMoodEntry::parse(const char* szData)
{
	enum State
	{
		STATE_NODEADDRESS = 0,
		STATE_UNITADDRESS,
		STATE_MESSAGECODE,
		STATE_LENGTH,
		STATE_DATA,
		STATE_DELAY,
		STATE_ERROR,
		STATE_DONE
	} currentState;

	currentState = STATE_NODEADDRESS;

	const int len = strlen(szData);
	int temp = -1;
	int pos=0;
	bool fResult = false;

	while( pos < len )
	{
		const char ch = szData[pos];
		switch (currentState)
		{
			case STATE_NODEADDRESS:
			{
				fResult = true;

				// Oppassen: Moeten "%02x" meegeven aan sscanf( ) omdat de volledige string aan
				// elkaar hangt en we telkens 2 Hex digits nodig hebben...

				if (sscanf(&szData[pos],"%02x",&temp) == 1)		// CHECKED_SSCANF_OK
				{
					ASSERT( temp >= 0 && temp <= 0xFF);

					m_entry.m_packet.bRemoteAddress = (BYTE) temp;
					pos += 2;
					currentState = STATE_UNITADDRESS;
				}
				else
				{
					currentState = STATE_ERROR;
				}
			} break;

			case STATE_UNITADDRESS:
			{
				// Oppassen: Moeten "%02x" meegeven aan sscanf( ) omdat de volledige string aan
				// elkaar hangt en we telkens 2 Hex digits nodig hebben...

				if (sscanf(&szData[pos],"%02x",&temp) == 1)		// CHECKED_SSCANF_OK
				{
					ASSERT( temp >= 0 && temp <= 0xFF);

					m_entry.m_packet.bDestinationUnitNR = (BYTE) temp;
					pos +=2;
					currentState = STATE_MESSAGECODE;
				}
				else
				{
					currentState = STATE_ERROR;
				}
			} break;

			case STATE_MESSAGECODE:
			{
				// Oppassen: Moeten "%02x" meegeven aan sscanf( ) omdat de volledige string aan
				// elkaar hangt en we telkens 2 Hex digits nodig hebben...

				if (sscanf(&szData[pos],"%02x",&temp) == 1)		// CHECKED_SSCANF_OK
				{
					ASSERT( temp >= 0 && temp <= 0xFF);

					m_entry.m_packet.bMessageCode = (BYTE) temp;
					pos +=2;
					currentState = STATE_LENGTH;
				}	
				else
				{
					currentState = STATE_ERROR;
				}
			} break;

			case STATE_LENGTH:
			{
				// Oppassen: Moeten "%02x" meegeven aan sscanf( ) omdat de volledige string aan
				// elkaar hangt en we telkens 2 Hex digits nodig hebben...

				if (sscanf(&szData[pos],"%02x",&temp) == 1)		// CHECKED_SSCANF_OK
				{
					ASSERT( temp >= 0 && temp <= 0xFF);

					m_entry.m_packet.bLength = (BYTE) temp;
					pos +=2;
					currentState = STATE_DATA;
				}		
				else
				{
					currentState = STATE_ERROR;
				}
			} break;

			case STATE_DATA:
			{
				for (int t=0;t<m_entry.m_packet.bLength;t++)
				{
					// Oppassen: Moeten "%02x" meegeven aan sscanf( ) omdat de volledige string aan
					// elkaar hangt en we telkens 2 Hex digits nodig hebben...

					if (sscanf(&szData[pos],"%02x",&temp) == 1)		// CHECKED_SSCANF_OK
					{
						ASSERT( temp >= 0 && temp <= 0xFF);

						m_entry.m_packet.pbData[t] = (BYTE) temp;	
						pos+=2;
					}
					else
					{
						currentState = STATE_ERROR;
					}
				}
				currentState = STATE_DELAY;

			} break;

			case STATE_DELAY:
			{
				if (szData[pos++] == ';')
				{		
					// Oppassen: Moeten "%02x" meegeven aan sscanf( ) omdat de volledige string aan
					// elkaar hangt en we telkens 2 Hex digits nodig hebben...

					if (sscanf(&szData[pos],"%02x",&temp) == 1)		// CHECKED_SSCANF_OK
					{
						ASSERT( temp >= 0 && temp <= 0xFF);

						m_entry.m_delay = (BYTE) temp;
						currentState = STATE_DONE;
					}
				}
				else
				{
					currentState = STATE_ERROR;	
				}
			} break;

			case STATE_DONE:
			{
				pos++;

			} break;		

			case STATE_ERROR:
			{
				pos++;
				fResult = false;

			} break;
		}	
	}
	return fResult;
}
/*************************************************************/


/*************************************************************/
/****************** CConfigMoodClass *************************/
/*************************************************************/
CConfigMoodClass::CConfigMoodClass(IReportError* ptr) :
m_list( ),
m_pCurrent( 0 ),
m_pErrorReport( ptr ),
m_currentIndex( 0 )
{
}
CConfigMoodClass::~CConfigMoodClass()
{
	removeAll( );
}
/*************************************************************/
void CConfigMoodClass::removeAll( )
{
	while (!m_list.isEmpty())
	{
		CMoodHeader* const p = m_list.removeHead( );
		ASSERT( p );
		delete p;
	}
	ASSERT( m_list.getCount( ) == 0 );
}
void CConfigMoodClass::ReportError(const char* szMessage)
{
	if (m_pErrorReport)
	{
		m_pErrorReport->reportError( szMessage );
	}
}
/*************************************************************/
BOOL CConfigMoodClass::ParseHeader(char *szAttribute,char *szData)
{
	if (strcmp(_szHeader,szAttribute) == 0) 
	{			
		if (sscanf(szData,"%x",&m_currentIndex) == 1)	// CHECKED_SSCANF_OK
		{		
			if (m_currentIndex < 256)
			{
				m_pCurrent = new CMoodHeader( );
				if (m_pCurrent)
				{
					int value = -1;
					if (sscanf(szData,"%x;",&value) != 0)		// CHANGES_MVS_2008_SSCANF
					{
						m_pCurrent->setIdentifier((short) value);
						m_list.addTail( m_pCurrent );
						return TRUE;
					}

					return FALSE;
				}
				ASSERT( m_pCurrent );
			}
			else
			{
				char szBuffer[255];
				sprintf(szBuffer,
						"Error:Mood 0x%02x:to large",
						 m_currentIndex);
				ReportError(szBuffer);	
			}
		}
		else
		{
			char szBuffer[255];
			sprintf(szBuffer,
					"Error:Mood 0x%02x",
					m_currentIndex);
			ReportError(szBuffer);
		}
		return FALSE;
	}
	return FALSE;
}

BOOL CConfigMoodClass::ParseData(char *szAttribute,char *szData)
{
	ASSERT( m_pCurrent );

	if (m_pCurrent == 0)
		return FALSE;

	if (strcmp("entry",szAttribute) == 0)
	{
		CMoodEntry* const pEntry = new CMoodEntry( );
		if (pEntry)
		{
			if (pEntry->parse(szData))
			{			
				m_pCurrent->add( pEntry );
				return TRUE;
			}
			else
			{
				char szBuffer[255];			
				sprintf(szBuffer,
						"Error:Mood 0x%02x:entry %d",
						m_pCurrent->getIdentifier(),
						m_currentIndex);

				ReportError(szBuffer);

				delete pEntry;				
				return FALSE;
			}
		}	
	}
	else
	{
		char szBuffer[255];			
		sprintf(szBuffer,
				"Error:Mood 0x%02x:Unkown attribute:'%s'",
				m_pCurrent->getIdentifier(),
				szAttribute);

		ReportError(szBuffer);	
	}
	return FALSE;
}

void CConfigMoodClass::Initialise(void)
{
	removeAll( );
	m_currentIndex = 0;
}
int CConfigMoodClass::GetMax(void) const
{
	return m_list.getCount( );
}
void CConfigMoodClass::dump( )
{
	const int max = m_list.getCount( );

	for (int i=0;i<max;i++)
	{
		CMoodHeader* const p = getMoodHeader( i );
		if (p)
		{
			p->dump( );
		}
	}
}
int CConfigMoodClass::GetData(int index,BYTE *pbData)
{
	pbData = 0;
	return 0;
}
BOOL CConfigMoodClass::IsUsed(int index)
{
	return (index < m_list.getCount( ));
}

CMoodHeader* CConfigMoodClass::getMoodHeader(int index)
{
	ASSERT (index < m_list.getCount( ));
	return (m_list.getEntry( index ));
}

/*************************************************************/
/************** CConfigMoodHeaderClass ***********************/
/*************************************************************/
CConfigMoodHeaderClass::CConfigMoodHeaderClass( ) :
m_pMoodHeader( 0 ) { }
CConfigMoodHeaderClass::~CConfigMoodHeaderClass( ) { }
/*************************************************************/
void CConfigMoodHeaderClass::set(CMoodHeader* pHeader)
{
	m_pMoodHeader = pHeader;
}
int CConfigMoodHeaderClass::GetData(int index,BYTE *pbData)
{
	ASSERT( m_pMoodHeader );

	if ((index != 0) || (m_pMoodHeader == 0))
		return 0;
			
	memcpy(pbData,m_pMoodHeader,sizeof(CMoodHeader));
	return sizeof(CMoodHeader);
}

BOOL CConfigMoodHeaderClass::IsUsed(int index)
{
	return ((index == 0) && (m_pMoodHeader != 0));
}
int CConfigMoodHeaderClass::GetMax(void) const
{
	return 1;
}
void CConfigMoodHeaderClass::dump( )
{
	if (m_pMoodHeader)
	{
		m_pMoodHeader->dump( );
	}
}
int CConfigMoodHeaderClass::getStartIndex( ) const
{
	if (m_pMoodHeader)
	{
		return m_pMoodHeader->getIdentifier();
	}
	return 0xFF;
}

/*************************************************************/
/************** CConfigMoodEntryClass ************************/
/*************************************************************/
CConfigMoodEntryClass::CConfigMoodEntryClass( ) :
m_pMoodHeader( 0 ) { }
CConfigMoodEntryClass::~CConfigMoodEntryClass( ) { }
/*************************************************************/
void CConfigMoodEntryClass::set(CMoodHeader* pHeader)
{
	m_pMoodHeader = pHeader;
}
int CConfigMoodEntryClass::GetData(int index,BYTE *pbData)
{
	ASSERT( m_pMoodHeader );

	const int max = m_pMoodHeader->getCount();
	if (index < max)
	{
		CMoodEntry* const pEntry =
			reinterpret_cast<CMoodEntry*>(m_pMoodHeader->get(index));

		if (pEntry)
		{
			memcpy(pbData,pEntry,sizeof(CMoodEntry));
			return sizeof(CMoodEntry);			
		}
		return 0;
	}
	return 0;			
}

BOOL CConfigMoodEntryClass::IsUsed(int index)
{
	ASSERT( m_pMoodHeader );
	return(index < m_pMoodHeader->getCount());
}
int CConfigMoodEntryClass::GetMax(void) const
{
	ASSERT( m_pMoodHeader );
	return m_pMoodHeader->getCount( );
}
void CConfigMoodEntryClass::dump( )
{
	if (m_pMoodHeader)
	{
		m_pMoodHeader->dump( );
	}
}

