// LogFile.h: interface for the CLogFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGFILE_H__D0BF9621_F05E_4457_97A9_23D9BB23D6A7__INCLUDED_)
#define AFX_LOGFILE_H__D0BF9621_F05E_4457_97A9_23D9BB23D6A7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CLogFileConversionFromNodes;
class CLogFileConversionFromNodesErrors;
class CLogFileConversionConstruct;

class CLogFile  
{
public:
	static CLogFileConversionFromNodes* GetConversionFromNodes();
	static CLogFileConversionFromNodesErrors* GetConversionFromNodesErrors();
	static CLogFileConversionConstruct* GetConversionConstruct();

protected:
	FILE* m_fp;

	CString m_szStartTime;
	CString szPathName;

public:
	CLogFile();
	virtual ~CLogFile();

protected:
	BOOL CreateFile(CString s);
	BOOL CloseFile();
	BOOL AddHeader();
	BOOL AddSeperator();
	BOOL AddSpace();
	BOOL Write(CString s);

protected:
	virtual BOOL _Start() {
		return FALSE;
	}
	virtual BOOL _Stop() {
		return FALSE;
	}

public:
	BOOL Start();
	BOOL Stop();

	BOOL Add(CString s);
};

#endif // !defined(AFX_LOGFILE_H__D0BF9621_F05E_4457_97A9_23D9BB23D6A7__INCLUDED_)
