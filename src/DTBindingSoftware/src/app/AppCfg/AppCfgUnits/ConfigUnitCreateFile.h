#ifndef DUOTECNO_CONFIG_UNITCREATEFILE_H
#define DUOTECNO_CONFIG_UNITCREATEFILE_H
///////////////////////////////////////////////////////////////////////
struct CConfigExternalClass;
///////////////////////////////////////////////////////////////////////
#include "app\appcfg\appcfgbase\ConfigExternalFile.h"
#include "utils/mfc/ArrayErrors.h"
///////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
// Sedert V16.40:
// Oplossing naam verschillen bij het aanmaken van units.
struct ICConfigUnitCreateDataValidator {

	virtual ~ICConfigUnitCreateDataValidator( ) = 0;

	// Zoeken naar een unit naam in node database.
	// Wanneer aanwezig return waarde TRUE, Anders FALSE.
	virtual BOOL getName(int nodeAddress, int unitAddress, CString* pStringName) = 0;
};


///////////////////////////////////////////////////////////////////////
struct ConfigUnitCreateClasses
{
	enum 
	{
		ENUM_CONFIGCLASS_UNIT = 0,

		MAX_CONFIG_CLASSES,	
		ENUM_CONFIGCLASS_NOASSIGNMENT = 0xFF
	};
};
///////////////////////////////////////////////////////////////////////
class CConfigUnitCreateFile 
{
public:
	CConfigUnitCreateFile();
	virtual ~CConfigUnitCreateFile();

	BOOL Load(CString szFilename,int nodeAddress, ICConfigUnitCreateDataValidator* pValidator);
	BOOL Initialise(void);
	int GetNrErrors(void) {
		return m_configErrors.GetNrErrors();
	}
	CString GetErrors(int i);
	CConfigExternalClass* GetClass(int type); 	
	CConfigExternalClass** GetClasses( ) {
		return m_ppConfigBaseClass;
	}

protected:
	// Sedert V16.40.
	BOOL NormaliseNames(int nodeAddress, ICConfigUnitCreateDataValidator* pValidator);

private:
	enum { 
		MAX_CLASSES = ConfigUnitCreateClasses::MAX_CONFIG_CLASSES 
	};

	DUOTECNO::MFC_HELPER::CArrayErrors m_configErrors;
	CConfigExternalFile* m_pConfigFile;
	CConfigExternalClass **m_ppConfigBaseClass;	
};
////////////////////////////////////////////////////////////////////////
#endif 
////////////////////////////////////////////////////////////////////////
