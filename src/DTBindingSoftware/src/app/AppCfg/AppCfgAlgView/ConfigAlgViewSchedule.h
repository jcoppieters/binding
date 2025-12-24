#pragma once

// De basisklasse
#include "configAlgView.h"		

/************************************************
 * Scheduling configuratie.
 ************************************************/
class CConfigAlgViewShedule : public CConfigAlgView
{
public:
	CConfigAlgViewShedule(CCanNode* pNode,IViewDisplay* pNotifyView);
	virtual ~CConfigAlgViewShedule(void);

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
	virtual BOOL IsReady(void);
	virtual BOOL IsSupportedHTTPProtocol(void);			// sedert v15.08
	virtual BOOL IsUseUploadHTTPProtocol(void);			// sedert v15.08
	virtual void SetUseUploadHTTPProtocol(BOOL flag);	// sedert v15.08
	// virtual BOOL getHttpInfoMessage(CString* pszMsg) const;			// sedert v16.01

protected:
	void init(CCanNode* pNode);							// sedert v15.08

private:
	BOOL m_useHTTPUpload;								// sedert v15.08
	// CString m_httpInfoMessage;							// sedert v16.01
	typedef CConfigAlgView Base;
};
