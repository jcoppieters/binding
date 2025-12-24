#pragma once

// De basisklasse
#include "configAlgView.h"		

/************************************************
 * Multi media configuratie.
 ************************************************/
class CConfigAlgViewMM : public CConfigAlgView
{
public:
	CConfigAlgViewMM(CCanNode* pNode,IViewDisplay* pNotifyView);
	virtual ~CConfigAlgViewMM(void);

	virtual BOOL Load(void);
	virtual BOOL Upload(void);	
	virtual BOOL UploadToAll(void);
	virtual int GetViewID(void) const;
	virtual BOOL IsSupported(void);
	virtual BOOL IsFileFound(void);
	virtual BOOL IsLoadSupported(void);
	virtual BOOL IsUploadCanSupported(void);
	virtual BOOL IsDownloadCanSupported(void);
	virtual BOOL IsUploadCanAllSupported(void);
	virtual BOOL IsUseConfigurator(void);
	virtual const char* getName(void);
	virtual BOOL getFullPathName(int idx,CString* pszFullPathName);
	virtual BOOL IsUseFTPProtocol(void);
	virtual BOOL IsReady(void);

private:
	typedef CConfigAlgView Base;
};
