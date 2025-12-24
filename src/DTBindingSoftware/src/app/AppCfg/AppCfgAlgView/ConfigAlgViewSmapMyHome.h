#pragma once

// De basisklasse
#include "configAlgView.h"		

/************************************************
 * Smap MyHome configuratie.
 ************************************************/
class CConfigAlgViewSmapMyHome : public CConfigAlgView
{
public:
	CConfigAlgViewSmapMyHome(CCanNode* pNode,IViewDisplay* pNotifyView);
	virtual ~CConfigAlgViewSmapMyHome(void);

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
	virtual BOOL IsReady(void);	// zie BUG_SOLVE_0158
	virtual BOOL getInfoMessages(CStringArray* messages);	// Sedert V16.01: CHANGES_SMAP_ONLY_FOR_APP

private:
	BOOL m_useOnlyDuotecnoProApp;		// Sedert V16.01: CHANGES_SMAP_ONLY_FOR_APP
	typedef CConfigAlgView Base;
};
