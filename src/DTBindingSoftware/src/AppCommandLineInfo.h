// AppCommandLineInfo.h: interface for the CAppCommandLineInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_APPCOMMANDLINEINFO_H__1DB8F405_09D0_4C0B_A8D6_43C9A08208FC__INCLUDED_)
#define AFX_APPCOMMANDLINEINFO_H__1DB8F405_09D0_4C0B_A8D6_43C9A08208FC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CAppCommandLineInfo : public CCommandLineInfo  
{
public:
	CAppCommandLineInfo();
	virtual ~CAppCommandLineInfo();

	void ParseParam(LPCTSTR lpszParam, BOOL bFlag, BOOL bLast);

protected:
	bool parseMode(const CString& strUpper);
	bool parseTcpIpMode(const CString strUpper);
	bool parseTcpRemoteCloudMode(const CString strUpper);
};

#endif // !defined(AFX_APPCOMMANDLINEINFO_H__1DB8F405_09D0_4C0B_A8D6_43C9A08208FC__INCLUDED_)
