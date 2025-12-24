// ConfigSmapMenuInfo.h: interface for the CConfigSmapMenuInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONFIGSMAPMENUINFO_H__18631365_4014_47E3_B08B_1BE7039DE34D__INCLUDED_)
#define AFX_CONFIGSMAPMENUINFO_H__18631365_4014_47E3_B08B_1BE7039DE34D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//////////////////////////////////////////////

#include "configsmapmenufile.h"

/////////////////////////////////////////////

class CConfigSmapMenuInfo  
{
public:
	static CConfigSmapMenuInfo& getInstance( );

protected:
	CConfigSmapMenuFile m_configFile;
	BYTE m_bNodeAddress;
	BOOL m_fileLoaded;

protected:
	CConfigSmapMenuInfo();

public:
	virtual ~CConfigSmapMenuInfo();
	CString GetFullPathName(BYTE bNodeAddress);

	BOOL load(BYTE bNodeAddress);
	BOOL isLoaded( ) const;
	BYTE getNodeAddress( ) const;
	CConfigSmapMenuFile& getConfigFile( ) {
		return m_configFile;
	}
	void dump( );
};

/////////////////////////////////////////////


#endif // !defined(AFX_CONFIGSMAPMENUINFO_H__18631365_4014_47E3_B08B_1BE7039DE34D__INCLUDED_)
