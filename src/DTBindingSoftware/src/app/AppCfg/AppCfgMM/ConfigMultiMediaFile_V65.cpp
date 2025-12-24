#include "stdafx.h"
#include "ConfigMultiMediaFile_V65.h"
#include "ConfigMultiMediaClass.h"

CConfigMultiMediaFile_V65::CConfigMultiMediaFile_V65(void) :
Base( ),
m_ppConfigBaseClass( 0 ),
m_pConfigFile( 0 )
{
	allocateMemory( );
	initData( );
}

CConfigMultiMediaFile_V65::~CConfigMultiMediaFile_V65(void)
{
	destroyMemory( );
}


CConfigExternalClass* CConfigMultiMediaFile_V65::GetClass(int type)	
{
	ASSERT( m_ppConfigBaseClass != 0 );

	if (type < MAX_CONFIG_CLASSES) 
	{
		ASSERT( m_ppConfigBaseClass[type] != 0 );

		return (m_ppConfigBaseClass[type]);
	}

	return NULL;
}

BOOL CConfigMultiMediaFile_V65::Initialise(void)
{
	initData( );
	this->Base::ClearErrors( );

	return TRUE;
}

BOOL CConfigMultiMediaFile_V65::Load(CString szFilename)
{
	if (m_pConfigFile != 0)
	{
		initData( );
		this->Base::ClearErrors( );

		if (m_pConfigFile->load((LPCTSTR) szFilename))	// CHANGES_MVS_2008_CSTRING
		{
			return (m_configErrors.Empty());			
		}	
	}
	return FALSE;
}

/////////////////////////////////////////////////////////
// private

void CConfigMultiMediaFile_V65::initData( )
{
	if (m_pConfigFile != 0)
	{
		m_pConfigFile->initialise( );
	}	
}

// Changes: TM,V14.09:
// CConfigAudioDest_V65, CConfigSpeechDevOutdoor_V65,
// CConfigAudioSource_V65: 128 ipv 32 bronnen.
void CConfigMultiMediaFile_V65::allocateMemory( )
{
	ASSERT( m_ppConfigBaseClass == 0 );

	m_ppConfigBaseClass = new CConfigExternalClass* [MAX_CONFIG_CLASSES];

	for (int i=0;i<MAX_CONFIG_CLASSES;i++) 
	{
		m_ppConfigBaseClass[i] = NULL;
	}

	m_ppConfigBaseClass[ENUM_CONFIGCLASS_AUDIOROOM] = new CConfigAudioRoom(&m_configErrors);		
	m_ppConfigBaseClass[ENUM_CONFIGCLASS_AUDIODEST] = new CConfigAudioDest_V65(&m_configErrors);
	m_ppConfigBaseClass[ENUM_CONFIGCLASS_AUDIOSOURCE] = new CConfigAudioSource_V65(&m_configErrors);	
	m_ppConfigBaseClass[ENUM_CONFIGCLASS_VIDEOMUXSRC] = new CConfigVideoMuxSource(&m_configErrors);	
	m_ppConfigBaseClass[ENUM_CONFIGCLASS_VIDEOMUXDEST] = new CConfigVideoMuxDest(&m_configErrors);	
	m_ppConfigBaseClass[ENUM_CONFIGCLASS_SPEECHDEVINDOOR] = new CConfigSpeechDevIndoor(&m_configErrors); 	
	m_ppConfigBaseClass[ENUM_CONFIGCLASS_SPEECHDEVOUTDOOR] = new CConfigSpeechDevOutdoor_V65(&m_configErrors);		
	m_ppConfigBaseClass[ENUM_CONFIGCLASS_ALARMZONE] = new CConfigAlarmZone(&m_configErrors);	
	m_ppConfigBaseClass[ENUM_CONFIGCLASS_AVMATRIX] = new CConfigAVMatrix(&m_configErrors);	
	m_ppConfigBaseClass[ENUM_CONFIGCLASS_VIDEOMUX] = new CConfigVideomux(&m_configErrors);	
	m_ppConfigBaseClass[ENUM_CONFIGCLASS_VIDEOPHONE] = new CConfigVideophone(&m_configErrors);
	
	m_pConfigFile = new CConfigExternalFile(MAX_CONFIG_CLASSES, m_ppConfigBaseClass);
}

void CConfigMultiMediaFile_V65::destroyMemory( )
{
	for (int i=0;i<MAX_CONFIG_CLASSES;i++) 
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
		m_pConfigFile = 0;
	}

	if (m_ppConfigBaseClass != 0)
	{
		delete [] m_ppConfigBaseClass;
		m_ppConfigBaseClass = 0;
	}
}