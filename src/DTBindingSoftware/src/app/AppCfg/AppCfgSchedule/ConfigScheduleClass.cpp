/**********************************************************//**
 * @file	ConfigScheduleClass.cpp
 * @brief	implementatie bestand voor de CConfigExternalClass klasse.
 *
 * @author	Mitchell Tom
 * @date	20091001
 *
 * @since	0x0560
 *************************************************************/
#include "stdafx.h"
#include "configScheduleClass.h"
/*************************************************************/

/*************************************************************/
using DUOTECNO::MFC_HELPER::IReportError;
/*************************************************************/
using DUOTECNO::DATA::CSerScheduleEvent;
using DUOTECNO::DATA::CSerScheduleItem;
/*************************************************************/
namespace
{
	enum ScheduleEventAction		/** Scheduling event acties */
	{
 		SCHEDULEEVENT_ACTION_NONE				= 0, /** dont change */
		SCHEDULEEVENT_ACTION_VIRTUAL_PULS   	= 1, /** dont change */
		SCHEDULEEVENT_ACTION_VIRTUAL_PTON  		= 2, /** dont change */
		SCHEDULEEVENT_ACTION_VIRTUAL_PTOFF   	= 3, /** dont change */
		SCHEDULEEVENT_ACTION_VIRTUAL_TON  		= 4, /** dont change */
		SCHEDULEEVENT_ACTION_VIRTUAL_TOFF   	= 5, /** dont change */
		SCHEDULEEVENT_ACTION_SENS_PRESET_SUN   	= 6, /** dont change */
		SCHEDULEEVENT_ACTION_SENS_PRESET_HSUN   = 7, /** dont change */
		SCHEDULEEVENT_ACTION_SENS_PRESET_MOON  	= 8, /** dont change */
		SCHEDULEEVENT_ACTION_SENS_PRESET_HMOON 	= 9, /** dont change */
		SCHEDULEEVENT_ACTION_SENS_OFF       	= 10, /** dont change */
		SCHEDULEEVENT_ACTION_CLOCK              = 11, /** dont change: klok radio.*/

		// Reserve voor nieuwe acties ...
		SCHEDULEEVENT_ACTION_MAX,                 /** Maximum action */
	};
};
/*************************************************************/

/*************************************************************/
/************** CConfigScheduleEventsClass *******************/
/*************************************************************/
const char* CConfigScheduleEventsClass::_szHeader = "SCHEDULEEVENT";
/*************************************************************/
CConfigScheduleEventsClass::CConfigScheduleEventsClass(IReportError* ptr) :
m_list( ),
m_pCurrentEntry( 0 ),
m_pErrorReport( ptr ),
m_currentIndex( 0 )
{
}
CConfigScheduleEventsClass::~CConfigScheduleEventsClass()
{
	removeAll( );
}
/*************************************************************/
void CConfigScheduleEventsClass::removeAll( )
{
	while(!m_list.isEmpty())
	{
		CSerScheduleEvent* const p = m_list.removeHead( );
		ASSERT(p);
		delete p;
	}
	ASSERT( m_list.getCount() == 0 );
}

CSerScheduleEvent* CConfigScheduleEventsClass::GetEntry(int entry)
{
	ASSERT( entry < m_list.getCount() );
	return m_list.getEntry(entry);
}
void CConfigScheduleEventsClass::ReportError(const char* szMessage)
{
	if (m_pErrorReport)
	{
		m_pErrorReport->reportError( szMessage );
	}
}
/*************************************************************/
BOOL CConfigScheduleEventsClass::ParseHeader(char *szAttribute,char *szData)
{
	if (strcmp(_szHeader,szAttribute) == 0) 
	{			
		if (sscanf(szData,"%x",&m_currentIndex) == 1)			// CHECKED_SSCANF_OK
		{								
			m_pCurrentEntry = new CSerScheduleEvent( );
			if (m_pCurrentEntry)
			{
				m_pCurrentEntry->bWeekNr = 0;
				m_pCurrentEntry->bDayNr = 0;
				m_pCurrentEntry->bHour = 0;
				m_pCurrentEntry->bMinute = 0;
				m_pCurrentEntry->bNodeAddress = 0xFF;
				m_pCurrentEntry->bUnitAddress = 0xFF;
				m_pCurrentEntry->bAction = SCHEDULEEVENT_ACTION_NONE;					
				m_list.addTail( m_pCurrentEntry );
				return TRUE;
			}
			ASSERT( m_pCurrentEntry );						
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

BOOL CConfigScheduleEventsClass::ParseData(char *szAttribute,char *szData)
{
	int value = -1;

	if (!m_pCurrentEntry) 
	{
		ASSERT(m_pCurrentEntry);
		return FALSE;
	}
	if (strcmp(szAttribute,"bWeekNr") == 0)
	{
		sscanf(szData,"%x;",&value);							// CHECKED_SSCANF_OK
		m_pCurrentEntry->bWeekNr = (BYTE) (value & 0xFF);
	}
	else if (strcmp(szAttribute,"bDayNr") == 0)
	{
		sscanf(szData,"%x;",&value);							// CHECKED_SSCANF_OK
		m_pCurrentEntry->bDayNr = (BYTE) (value & 0xFF);
	}
	else if (strcmp(szAttribute,"bHour") == 0)
	{
		sscanf(szData,"%x;",&value);							// CHECKED_SSCANF_OK
		m_pCurrentEntry->bHour = (BYTE) (value & 0xFF);
	}
	else if (strcmp(szAttribute,"bMinute") == 0)
	{
		sscanf(szData,"%x;",&value);							// CHECKED_SSCANF_OK
		m_pCurrentEntry->bMinute = (BYTE) (value & 0xFF);
	}
	else if (strcmp(szAttribute,"bNodeAddress") == 0)
	{
		sscanf(szData,"%x;",&value);							// CHECKED_SSCANF_OK
		m_pCurrentEntry->bNodeAddress = (BYTE) (value & 0xFF);
	}
	else if (strcmp(szAttribute,"bUnitAddress") == 0)
	{
		sscanf(szData,"%x;",&value);							// CHECKED_SSCANF_OK
		m_pCurrentEntry->bUnitAddress = (BYTE) (value & 0xFF);
	}
	else if (strcmp(szAttribute,"bAction") == 0)
	{
		sscanf(szData,"%x;",&value);							// CHECKED_SSCANF_OK
		m_pCurrentEntry->bAction = (BYTE) (value & 0xFF);
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

void CConfigScheduleEventsClass::Initialise(void)
{
	removeAll( );
	m_currentIndex = 0;
}
int CConfigScheduleEventsClass::GetMax(void) const
{
	return m_list.getCount( );
}
int CConfigScheduleEventsClass::GetData(int index,BYTE *pbData)
{
	if (index < m_list.getCount( ))
	{
		CSerScheduleEvent* const pSerEntry = GetEntry( index );			
		if (pSerEntry)
		{
			memcpy(pbData,pSerEntry,sizeof(CSerScheduleEvent));
			return (sizeof(CSerScheduleEvent));		
		}
	}	
	pbData = 0;
	return 0;
}
BOOL CConfigScheduleEventsClass::IsUsed(int index)
{
	if (index < m_list.getCount( ))
	{
		return TRUE;
	}
	return FALSE;
}

void CConfigScheduleEventsClass::dump( )
{
	const int MAX = m_list.getCount();
	for (int i=0;i<MAX;i++)
	{
		CSerScheduleEvent* const pSerEntry = GetEntry( i );
		if (pSerEntry)
		{
			char szBuffer[512];

			sprintf(szBuffer,
					"SCHEDULEEEVENT=0x%02x;"
					"bWeekNr=0x%02x;"
					"bDayNr=0x%02x;"
					"bHour=0x%02x;"
					"bMinute=0x%02x;"
					"bNodeAddress=0x%02x;"
					"bUnitAddress=0x%02x;"
					"bAction=0x%02x;\n",
					i,
					pSerEntry->bWeekNr,
					pSerEntry->bDayNr,
					pSerEntry->bHour,
					pSerEntry->bMinute,
					pSerEntry->bNodeAddress,
					pSerEntry->bUnitAddress,
					pSerEntry->bAction);

			TRACE( szBuffer );
		}
	}
}
/*************************************************************/

/*************************************************************/
/************** CConfigScheduleItemsClass ********************/
/*************************************************************/
const char* CConfigScheduleItemsClass::_szHeader = "SCHEDULEITEM";
/*************************************************************/
CConfigScheduleItemsClass::CConfigScheduleItemsClass(IReportError* ptr) :
m_list( ),
m_pCurrentEntry( 0 ),
m_pErrorReport( ptr ),
m_currentIndex( 0 )
{
}
CConfigScheduleItemsClass::~CConfigScheduleItemsClass()
{
	removeAll( );
}
/*************************************************************/
void CConfigScheduleItemsClass::removeAll( )
{
	while(!m_list.isEmpty())
	{
		CSerScheduleItem* const p = m_list.removeHead( );
		ASSERT(p);
		delete p;
	}
	ASSERT( m_list.getCount() == 0 );
}

CSerScheduleItem* CConfigScheduleItemsClass::GetEntry(int entry)
{
	ASSERT( entry < m_list.getCount() );
	return m_list.getEntry(entry);
}
void CConfigScheduleItemsClass::ReportError(const char* szMessage)
{
	if (m_pErrorReport)
	{
		m_pErrorReport->reportError( szMessage );
	}
}
/*************************************************************/
BOOL CConfigScheduleItemsClass::ParseHeader(char *szAttribute,char *szData)
{
	if (strcmp(_szHeader,szAttribute) == 0) 
	{			
		if (sscanf(szData,"%x",&m_currentIndex) == 1)		// CHECKED_SSCANF_OK 
		{								
			m_pCurrentEntry = new CSerScheduleItem( );
			if (m_pCurrentEntry)
			{
				m_pCurrentEntry->bNodeAddress = 0xFF;
				m_pCurrentEntry->bUnitAddress = 0xFF;		
				m_pCurrentEntry->bStateOnOff = 0;
				m_pCurrentEntry->bCurrentState = 0xFF;
				m_pCurrentEntry->bPreviousState = 0xFF;

				m_list.addTail( m_pCurrentEntry );
				return TRUE;
			}
			ASSERT( m_pCurrentEntry );						
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

BOOL CConfigScheduleItemsClass::ParseData(char *szAttribute,char *szData)
{
	int value = -1;

	if (!m_pCurrentEntry) 
	{
		ASSERT(m_pCurrentEntry);
		return FALSE;
	}
	
	if (strcmp(szAttribute,"bNodeAddress") == 0)
	{
		sscanf(szData,"%x;",&value);				// CHECKED_SSCANF_OK
		m_pCurrentEntry->bNodeAddress = (BYTE) (value & 0xFF);
	}
	else if (strcmp(szAttribute,"bUnitAddress") == 0)
	{
		sscanf(szData,"%x;",&value);				// CHECKED_SSCANF_OK
		m_pCurrentEntry->bUnitAddress = (BYTE) (value & 0xFF);
	}
	else if (strcmp(szAttribute,"bState") == 0)
	{
		sscanf(szData,"%x;",&value);				// CHECKED_SSCANF_OK
		m_pCurrentEntry->bStateOnOff = (BYTE) (value & 0xFF);
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

void CConfigScheduleItemsClass::Initialise(void)
{
	removeAll( );
	m_currentIndex = 0;
}
int CConfigScheduleItemsClass::GetMax(void) const
{
	return m_list.getCount( );
}
int CConfigScheduleItemsClass::GetData(int index,BYTE *pbData)
{
	if (index < m_list.getCount( ))
	{
		CSerScheduleItem* const pSerEntry = GetEntry( index );			
		if (pSerEntry)
		{
			memcpy(pbData,pSerEntry,sizeof(CSerScheduleItem));
			return (sizeof(CSerScheduleItem));		
		}
	}	
	pbData = 0;
	return 0;
}
BOOL CConfigScheduleItemsClass::IsUsed(int index)
{
	if (index < m_list.getCount( ))
	{
		return TRUE;
	}
	return FALSE;
}

void CConfigScheduleItemsClass::dump( )
{
	const int MAX = m_list.getCount();
	for (int i=0;i<MAX;i++)
	{
		CSerScheduleItem* const pSerEntry = GetEntry( i );
		if (pSerEntry)
		{
			char szBuffer[256];

			sprintf(szBuffer,
					"SCHEDULEITEM=0x%02x;"
					"bNodeAddress=0x%02x;"
					"bUnitAddress=0x%02x;"
					"bState=0x%02x;\n",
					i,
					pSerEntry->bNodeAddress,
					pSerEntry->bUnitAddress,
					pSerEntry->bStateOnOff);

			TRACE( szBuffer );
		}
	}
}




#ifdef SKIP

		/*************************************************************/
		/************** CConfigScheduleItemsClass ********************/
		/*************************************************************/
		CConfigScheduleHeaderClass::CConfigScheduleEventsHeaderClass( )
		{
			m_SerScheduleHeader.bType = 0xFF;
			m_SerSchedule
		}

		/***********************************************************//**
		 * @class	CConfigScheduleHeaderClass
		 * @brief	Deze klasse wordt gebruikt door de FSM.
		 * 
		 * @note	deze klasse wordt niet gebruikt om de config file
		 *			in te lezen.
		 *
		 * @author	Mitchell Tom
		 * @date	20091001
		 **************************************************************/
		class CConfigScheduleHeaderClass : public CConfigExternalClass
		{
		protected:
			CSerScheduleHeader m_SerScheduleHeader;

		public:
			CConfigScheduleEventsHeaderClass( );
			~CConfigScheduleEventsHeaderClass( );

			void setNrOfEntries(unsigned short usNrOfEntries);
			void setType(BYTE bType);

		public:
			virtual BOOL ParseHeader(char *szAttribute,char *szData) {
				return FALSE;
			}
			virtual BOOL ParseData(char *szAttribute,char *szData) {
				return FALSE;
			}
			virtual void Initialise(void) {
			}	
		public:
			virtual int GetData(int index,BYTE *pbData);
			virtual BOOL IsUsed(int index);
			virtual int GetMax(void) const;
			virtual void dump( );
			int getStartIndex( ) const;
		};

#endif
