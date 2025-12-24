// ConfigSmapPageInfoInfo.h: interface for the CConfigSmapPageInfoInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONFIGSMAPPAGEINFOINFO_H__30195D47_3F92_4F49_8CFA_B1F95FCEF265__INCLUDED_)
#define AFX_CONFIGSMAPPAGEINFOINFO_H__30195D47_3F92_4F49_8CFA_B1F95FCEF265__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//////////////////////////////////////////////

#include "configsmappageinfofile.h"

/////////////////////////////////////////////

class CConfigSmapPageInfoInfo  
{
public:
	static CConfigSmapPageInfoInfo& getInstance( );

protected:
	CConfigSmapPageInfoInfo();

public:
	virtual ~CConfigSmapPageInfoInfo();
	CString GetFullPathName(BYTE bNodeAddress);

	BOOL load(BYTE bNodeAddress);
	BOOL isLoaded( ) const;
	BYTE getNodeAddress( ) const;
	CConfigSmapPageInfoFile& getConfigFile( ) {
		return m_configFile;
	}
	void dump( );
	// Zie BUG_SOLVE_0158
	BOOL isReady() const;
	void setReady(BOOL flag);

private:
	CConfigSmapPageInfoFile m_configFile;
	BYTE m_bNodeAddress;
	BOOL m_fileLoaded;
	BOOL m_fReady;	// Zie BUG_SOLVE_0158
};

/////////////////////////////////////////////

#endif // !defined(AFX_CONFIGSMAPPAGEINFOINFO_H__30195D47_3F92_4F49_8CFA_B1F95FCEF265__INCLUDED_)
