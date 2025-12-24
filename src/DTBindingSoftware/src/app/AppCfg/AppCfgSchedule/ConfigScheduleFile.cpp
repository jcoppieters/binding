#include "stdafx.h"
#include "configSchedulefile.h"
//////////////////////////////////////////////////////////////////////
#include "ConfigScheduleClass.h"
//////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
using DUOTECNO::MFC_HELPER::CArrayErrors;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CConfigScheduleFile::CConfigScheduleFile() :
m_ppConfigBaseClass( 0 ),
m_pConfigFile( 0 ),
m_configErrors( )
{	
	m_ppConfigBaseClass = new CConfigExternalClass* [MAX_CLASSES];

	for (int i=0;i<MAX_CLASSES;i++) 
	{
		m_ppConfigBaseClass[i] = NULL;
	}

	m_ppConfigBaseClass[ConfigScheduleClasses::ENUM_CONFIGCLASS_SCHEDULE_ITEMS] = 
		new CConfigScheduleItemsClass(&m_configErrors);		
	m_ppConfigBaseClass[ConfigScheduleClasses::ENUM_CONFIGCLASS_SCHEDULE_EVENTS] = 
		new CConfigScheduleEventsClass(&m_configErrors);	
	
	m_pConfigFile = new CConfigExternalFile(MAX_CLASSES, m_ppConfigBaseClass);

	Initialise();
}

CConfigScheduleFile::~CConfigScheduleFile()
{
	for (int i=0;i<MAX_CLASSES;i++) 
	{
		if (m_ppConfigBaseClass[i] != 0)
		{
			delete m_ppConfigBaseClass[i];
			m_ppConfigBaseClass[i] = 0;
		}
	}

	if (m_pConfigFile != 0)
	{
		delete m_pConfigFile;
	}

	m_configErrors.removeAll( );

	delete [] m_ppConfigBaseClass;
}

///////////////////////////////////////////////////////////////////////
CConfigExternalClass* 
CConfigScheduleFile::GetClass(int type)	
{
	if (type < MAX_CLASSES) 
	{
		return (m_ppConfigBaseClass[type]);
	}
	return NULL;
}

CString CConfigScheduleFile::GetErrors(int i)
{
	const char* szString = 0;
	if (m_configErrors.GetErrors(i,&szString))
	{
		return szString;
	}	
	return "";
}
///////////////////////////////////////////////////////////////////////
BOOL CConfigScheduleFile::Initialise(void)
{
	if (m_pConfigFile != 0)
		m_pConfigFile->initialise( );

	m_configErrors.removeAll( );
	return TRUE;
}

BOOL CConfigScheduleFile::Load(CString szFilename)
{
	if (m_pConfigFile != 0)
	{
		Initialise( );

		if (m_pConfigFile->load((LPCTSTR) szFilename))	// CHANGES_MVS_2008_CSTRING
		{
			return (m_configErrors.Empty());			
		}	
	}
	return FALSE;
}
