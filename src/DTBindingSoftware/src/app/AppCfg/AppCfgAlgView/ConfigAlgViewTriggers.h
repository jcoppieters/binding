#pragma once

// De basisklasse
#include "configAlgView.h"		

// CHANGES_AV_UNIVERSAL

/************************************************
 * Triggers & acties configuratie.
 ************************************************/
class CConfigAlgViewTriggers : public CConfigAlgView
{
public:
	CConfigAlgViewTriggers(CCanNode* pNode,IViewDisplay* pNotifyView);
	virtual ~CConfigAlgViewTriggers(void);

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

private:
	typedef CConfigAlgView Base;
};
