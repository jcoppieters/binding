// ConfigMultiMediaFile.h: interface for the CConfigMultiMediaFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONFIGMULTIMEDIAFILE_H__9D430F75_B982_42E2_BA4C_399D6ECD40BB__INCLUDED_)
#define AFX_CONFIGMULTIMEDIAFILE_H__9D430F75_B982_42E2_BA4C_399D6ECD40BB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


///////////////////////////////////////////////////////////////////////
struct CConfigExternalClass;
///////////////////////////////////////////////////////////////////////
#include "app\appcfg\appcfgbase\ConfigExternalFile.h"
#include "utils/mfc/ArrayErrors.h"
///////////////////////////////////////////////////////////////////////
typedef enum 
{
	ENUM_CONFIGCLASS_AUDIOROOM = 0,
	ENUM_CONFIGCLASS_AUDIODEST,
	ENUM_CONFIGCLASS_AUDIOSOURCE,
	ENUM_CONFIGCLASS_VIDEOMUXSRC,
	ENUM_CONFIGCLASS_VIDEOMUXDEST,
	ENUM_CONFIGCLASS_SPEECHDEVINDOOR,
	ENUM_CONFIGCLASS_SPEECHDEVOUTDOOR,
	ENUM_CONFIGCLASS_ALARMZONE,
	ENUM_CONFIGCLASS_AVMATRIX,
	ENUM_CONFIGCLASS_VIDEOMUX,
	ENUM_CONFIGCLASS_VIDEOPHONE,

	MAX_CONFIG_CLASSES,
	
	ENUM_CONFIGCLASS_NOASSIGNMENT = 0xFF

}ENUM_MULTIMEDIA_CONFIGCLASS;

///////////////////////////////////////////////////////////////////////

class CConfigMultiMediaFile 
{
protected:
	DUOTECNO::MFC_HELPER::CArrayErrors m_configErrors;

public:
	CConfigMultiMediaFile();
	virtual ~CConfigMultiMediaFile();

	virtual BOOL Load(CString szFilename)						= 0;
	virtual BOOL Initialise(void)								= 0;
	virtual CConfigExternalClass* GetClass(int type)			= 0;
	DUOTECNO::MFC_HELPER::CArrayErrors* getErrors( );

protected:
	void ClearErrors( );
};

inline DUOTECNO::MFC_HELPER::CArrayErrors* CConfigMultiMediaFile::getErrors( )
{
	return &m_configErrors;
}

////////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_CONFIGMULTIMEDIAFILE_H__9D430F75_B982_42E2_BA4C_399D6ECD40BB__INCLUDED_)
