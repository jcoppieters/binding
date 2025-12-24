// ConfigSmapSmapInfoClass.cpp: implementation of the CConfigSmapSmapInfoClass class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ConfigSmapSmapInfoClass.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CSerSmapSmapInfo::CSerSmapSmapInfo( ) :
m_data( )
{
	init( );
}
CSerSmapSmapInfo::~CSerSmapSmapInfo( )
{
}

// public

int CSerSmapSmapInfo::getData(BYTE* pbData)
{
	pbData[0] = m_data.m_screenmapID;
	pbData[1] = m_data.m_startPageNR;
	return 2;
}

void CSerSmapSmapInfo::dump( )
{
	char szBuffer[255];
	sprintf(szBuffer, 
			"SCREENMAP=%d;\nStartPage=%d;\n",
			m_data.m_screenmapID,
			m_data.m_startPageNR);

	TRACE( szBuffer );
}


BOOL CSerSmapSmapInfo::save(FILE* fs)
{
	BOOL result = FALSE;

	if (fs)
	{	
		char szBuffer[255];
		sprintf(szBuffer, 
			"SCREENMAP=%d;\nStartPage=%d;\n",
			m_data.m_screenmapID,
			m_data.m_startPageNR);

		fputs(szBuffer,fs);			// CHANGES_0X0A01_USE_FPUTS
		result = TRUE;
	}

	return result;
}

// protected

void CSerSmapSmapInfo::init( )
{
	m_data.m_screenmapID = 0xFF;
	m_data.m_startPageNR = 0;
}



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const char* const CConfigSmapSmapInfoClass::_szHeader = "SCREENMAP";

CConfigSmapSmapInfoClass::CConfigSmapSmapInfoClass(
	DUOTECNO::MFC_HELPER::IReportError *ptr) :
	m_pErrorReport( ptr ),
	m_list( ),
	m_currScreenmapID( -1 )
{

}

CConfigSmapSmapInfoClass::~CConfigSmapSmapInfoClass()
{
	removeAll( );
}


// public

CSerSmapSmapInfo* CConfigSmapSmapInfoClass::getEntry(int index)
{
	ASSERT (index < m_list.getCount( ));
	return (m_list.getEntry( index ));
}

BOOL CConfigSmapSmapInfoClass::ParseHeader(char *szAttribute,char *szData)
{
	if (strcmp(_szHeader,szAttribute) == 0) 
	{			
		m_currScreenmapID = -1;

		int tempValue = 0;
		if (sscanf(szData,"%d",&tempValue) == 1)		// CHECKED_SSCANF_OK
		{				
			if (tempValue < 256)
			{
				m_currScreenmapID = tempValue;
				
				return TRUE;						
			}
			else
			{
				char szBuffer[255];
				sprintf(szBuffer,
						"Error:Screenmap id %d:to large",
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

BOOL CConfigSmapSmapInfoClass::ParseData(char *szAttribute,char *szData)
{
	ASSERT( m_currScreenmapID != -1 );

	if (m_currScreenmapID == -1)
	{
		return FALSE;
	}

	static const char* const STARTPAGE = "StartPage";

	if (strcmp(STARTPAGE,szAttribute) == 0)
	{
		int value = 0;
		if (sscanf(szData,"%d",&value) == 1)				// CHECKED_SSCANF_OK
		{
			CSerSmapSmapInfo* const pNew = new CSerSmapSmapInfo( );
			if (pNew)
			{
				pNew->setScreenmapID( m_currScreenmapID );
				pNew->setStartPageNR( value );

				if (addEntry(pNew))
				{
					return TRUE;
				}
				else
				{
					ASSERT( 0 );
					delete pNew;
				}
			}
		}
		else
		{
			char szBuffer[255];			

			sprintf(szBuffer,
					"Error:Screenmap id %d , attribute '%s'",
					m_currScreenmapID,
					STARTPAGE);

			ReportError(szBuffer);
		
			return FALSE;
		}		
	}
	return FALSE;
}

void CConfigSmapSmapInfoClass::Initialise(void)
{
	removeAll( );
	m_currScreenmapID = -1;	
}

int CConfigSmapSmapInfoClass::GetMax(void) const
{
	return m_list.getCount( );
}

void CConfigSmapSmapInfoClass::dump( )
{
	const int max = m_list.getCount( );

	for (int i=0;i<max;i++)
	{
		CSerSmapSmapInfo* const p = getEntry( i );
		if (p)
		{
			p->dump( );
		}
	}
}

int CConfigSmapSmapInfoClass::GetData(int index,BYTE *pbData)
{
	int result = 0;

	CSerSmapSmapInfo* const p = getEntry(index);
	if (p)
	{
		result = p->getData( pbData );
	}

	return result;
}

BOOL CConfigSmapSmapInfoClass::IsUsed(int index)
{
	return index < m_list.getCount( );
}

BOOL CConfigSmapSmapInfoClass::addEntry(CSerSmapSmapInfo* p)
{
	ASSERT( p );

	m_list.addTail(p);
	return TRUE;
}


// protected

void CConfigSmapSmapInfoClass::removeAll( )
{
	while (!m_list.isEmpty())
	{
		CSerSmapSmapInfo* const p = m_list.removeHead( );
		ASSERT( p );
		delete p;
	}

	ASSERT( m_list.getCount( ) == 0 );
}

void CConfigSmapSmapInfoClass::ReportError(const char* szMessage)
{
	if (m_pErrorReport)
	{
		m_pErrorReport->reportError( szMessage );
	}
}

BOOL CConfigSmapSmapInfoClass::save(FILE* fs)
{
	BOOL result = FALSE;

	if (fs)
	{
		const int max = m_list.getCount( );

		for (int i=0;i<max;i++)
		{
			CSerSmapSmapInfo* const p = getEntry( i );
			if (p)
			{
				p->save(fs);
			}
		}	
		result = TRUE;
	}

	return result;
}


/***************************************************/
