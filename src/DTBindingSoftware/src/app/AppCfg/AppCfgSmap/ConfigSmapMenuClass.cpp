// ConfigSmapMenuClass.cpp: implementation of the CConfigSmapMenuClass class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ConfigSmapMenuClass.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSerSmapMenu::CSerSmapMenu() :
m_data( )
{
	init( );
}

CSerSmapMenu::~CSerSmapMenu()
{

}

int CSerSmapMenu::getData(BYTE* pbData)
{
	pbData[0] = m_data.m_position;
	pbData[1] = m_data.m_function;				
	pbData[2] = m_data.m_state;	

	return 3;
}

void CSerSmapMenu::dump( )
{
	char szBuffer[255];
	int length;

	if (getString(szBuffer, 255, &length))
	{
		TRACE( szBuffer );
	}
}

BOOL CSerSmapMenu::save(FILE* fs)
{
	char szBuffer[255];
	int length;

	if (fs)
	{
		if (getString(szBuffer, 255, &length))
		{
			fputs(szBuffer,fs);		// CHANGES_0X0A01_USE_FPUTS
		}

		return TRUE;
	}
	return FALSE;
}

// protected 

void CSerSmapMenu::init( )
{
	m_data.m_position = 0xFF;
	m_data.m_function = 0xFF;		
	m_data.m_state = 0;
}

BOOL CSerSmapMenu::getString(
	char* pbData,
	int maxLength,
	int *length)
{
	char szBuffer[255];
	
	sprintf(szBuffer, 
			"ICON=%d;\nfunction=%d;state=%d\n",
			m_data.m_position,
			m_data.m_function,
			m_data.m_state);

	*length = strlen(szBuffer) + 1;
	if (*length < maxLength)
	{
		memcpy(pbData, szBuffer, *length);
		return TRUE;
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
const char* const CConfigSmapMenuClass::_szHeader = "ICON";

CConfigSmapMenuClass::CConfigSmapMenuClass(
	DUOTECNO::MFC_HELPER::IReportError *ptr) :
	m_pErrorReport( ptr ),
	m_list( ),
	m_pCurrent( 0 )
{

}

CConfigSmapMenuClass::~CConfigSmapMenuClass()
{
	removeAll( );
}


// public

CSerSmapMenu* CConfigSmapMenuClass::getEntry(int index)
{
	ASSERT (index < m_list.getCount( ));
	return (m_list.getEntry( index ));
}

BOOL CConfigSmapMenuClass::ParseHeader(char *szAttribute,char *szData)
{
	if (strcmp(_szHeader,szAttribute) == 0) 
	{				
		m_pCurrent = 0;

		int tempValue = 0;
		if (sscanf(szData,"%d",&tempValue) == 1)		// CHECKED_SSCANF_OK
		{				
			if ((tempValue >= 0) && (tempValue < 256))
			{
				m_pCurrent = new CSerSmapMenu( );
				if (m_pCurrent)
				{
					ASSERT(tempValue >= 0 && tempValue <= 0xFF);			// CHANGES_MVS_2008_WARNINGS
					m_pCurrent->setPosition(static_cast<BYTE>(tempValue));	// CHANGES_MVS_2008_WARNINGS
				}

				if (addEntry(m_pCurrent))
				{
					return TRUE;
				}
				else
				{
					ASSERT( 0 );

					delete m_pCurrent;
					m_pCurrent = 0;
				}			
				return TRUE;						
			}
			else
			{
				char szBuffer[255];
				sprintf(szBuffer,
						"Error:Icon id %d:to large",
						 tempValue);

				ReportError(szBuffer);	
			}
		}
		else
		{
			char szBuffer[255];
			sprintf(szBuffer,"Error:Screenmap field could not be parsed");
			ReportError(szBuffer);
		}
		return FALSE;
	}

	return FALSE;	
}

BOOL CConfigSmapMenuClass::ParseData(char *szAttribute,char *szData)
{
	ASSERT( m_pCurrent != 0);

	if (m_pCurrent == 0)
	{
		return FALSE;
	}

	static const char* const FUNCTION = "Function";

	if (strcmp(FUNCTION,szAttribute) == 0)
	{
		int value = 0;
		if (sscanf(szData,"%d",&value) == 1)		// CHECKED_SSCANF_OK
		{
			ASSERT( value >= 0 && value <= 0xFF);				// CHANGES_MVS_2008_WARNINGS
			m_pCurrent->setFunction(static_cast<BYTE>(value));	// CHANGES_MVS_2008_WARNINGS
		}
		else
		{
			m_pCurrent->setFunction( 255 );		
		}		

		return TRUE;
	}
	return FALSE;
}

void CConfigSmapMenuClass::Initialise(void)
{
	removeAll( );
	m_pCurrent = 0;
}

int CConfigSmapMenuClass::GetMax(void) const
{
	return m_list.getCount( );
}

void CConfigSmapMenuClass::dump( )
{
	const int max = m_list.getCount( );

	for (int i=0;i<max;i++)
	{
		CSerSmapMenu* const p = getEntry( i );
		if (p)
		{
			p->dump( );
		}
	}
}

int CConfigSmapMenuClass::GetData(int index,BYTE *pbData)
{
	int result = 0;

	CSerSmapMenu* const p = getEntry(index);
	if (p)
	{
		result = p->getData( pbData );
	}

	return result;
}

BOOL CConfigSmapMenuClass::IsUsed(int index)
{
	return index < m_list.getCount( );
}


// protected

void CConfigSmapMenuClass::removeAll( )
{
	while (!m_list.isEmpty())
	{
		CSerSmapMenu* const p = m_list.removeHead( );
		ASSERT( p );
		delete p;
	}

	ASSERT( m_list.getCount( ) == 0 );
}

void CConfigSmapMenuClass::ReportError(const char* szMessage)
{
	if (m_pErrorReport)
	{
		m_pErrorReport->reportError( szMessage );
	}
}

BOOL CConfigSmapMenuClass::addEntry(CSerSmapMenu* p)
{
	ASSERT( p );

	m_list.addTail(p);
	return TRUE;
}

BOOL CConfigSmapMenuClass::save(FILE* fs)
{
	BOOL result = FALSE;

	if (fs)
	{
		const int max = m_list.getCount( );

		for (int i=0;i<max;i++)
		{
			CSerSmapMenu* const p = getEntry( i );
			if (p)
			{
				p->save(fs);
			}
		}	
		result = TRUE;
	}

	return result;
}

////////////////////////////////////////////////////////////////////////