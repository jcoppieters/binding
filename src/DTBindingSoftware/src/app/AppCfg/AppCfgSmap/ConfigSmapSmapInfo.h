// ConfigSmapSmapInfo.h: interface for the CConfigSmapSmapInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONFIGSMAPSMAPINFO_H__07D971F4_642D_408A_A1EB_421897FDBBD9__INCLUDED_)
#define AFX_CONFIGSMAPSMAPINFO_H__07D971F4_642D_408A_A1EB_421897FDBBD9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//////////////////////////////////////////////

#include "configsmapsmapinfofile.h"

/////////////////////////////////////////////

class CConfigSmapSmapInfo  
{
public:
	static CConfigSmapSmapInfo& getInstance( );

protected:
	CConfigSmapSmapInfoFile m_configFile;
	BYTE m_bNodeAddress;
	BOOL m_fileLoaded;

protected:
	CConfigSmapSmapInfo();

public:
	virtual ~CConfigSmapSmapInfo();
	CString GetFullPathName(BYTE bNodeAddress);

	BOOL load(BYTE bNodeAddress);
	BOOL save(BYTE bNodeAddress);
	BOOL isLoaded( ) const;
	BYTE getNodeAddress( ) const;
	CConfigSmapSmapInfoFile& getConfigFile( ) {
		return m_configFile;
	}
	void dump( );
};

/////////////////////////////////////////////


#endif // !defined(AFX_CONFIGSMAPSMAPINFO_H__07D971F4_642D_408A_A1EB_421897FDBBD9__INCLUDED_)
