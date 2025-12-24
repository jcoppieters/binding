#pragma once

// De basisklasse
#include "configAlgView.h"		

/************************************************
 * Unit create configuratie.
 ************************************************/
class CConfigAlgViewUnits : public CConfigAlgView
{
public:
	CConfigAlgViewUnits(CCanNode* pNode,IViewDisplay* pNotifyView);
	virtual ~CConfigAlgViewUnits(void);

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


	static BOOL IsFileFound(int nodeAddress);

private:
	typedef CConfigAlgView Base;
};
