#pragma once

// De basisklasse
#include "configAlgView.h"		


/************************************************
 * DALi Configuratie voor de smartbox ultra - Sedert V16.33
 ************************************************/
class CConfigAlgViewDALI : public CConfigAlgView
{
public:
	CConfigAlgViewDALI(CCanNode* pNode,IViewDisplay* pNotifyView);
	virtual ~CConfigAlgViewDALI(void);

	virtual BOOL Load(void);
	virtual BOOL Upload(void);	
	virtual BOOL UploadToAll(void);
	virtual int GetViewID(void) const;
	virtual BOOL IsSupported(void);
	virtual BOOL IsLoadSupported(void);
	virtual BOOL IsFileFound(void);
	virtual BOOL IsUploadCanSupported(void);
	virtual BOOL IsDownloadCanSupported(void);
	virtual BOOL IsUploadCanAllSupported(void);
	virtual BOOL IsUseConfigurator(void);	
	virtual const char* getName(void);
	virtual BOOL getFullPathName(int idx,CString* pszFullPathName);
	virtual BOOL IsUseFTPProtocol(void);
	virtual BOOL openConfiguration(BYTE bNodeAddress);

private:
	typedef CConfigAlgView Base;
};
