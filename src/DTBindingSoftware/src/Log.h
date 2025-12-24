// Log.h: interface for the CLog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOG_H__E1F106DC_547B_478A_8F69_33849DB21F3D__INCLUDED_)
#define AFX_LOG_H__E1F106DC_547B_478A_8F69_33849DB21F3D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CLogMessage;


class CLog  
{
protected:	
	CPtrArray m_Messages;
	int m_ErrorCnt;
	int m_WarningCnt;

protected:
	CLogMessage* Get(int index);
	int Get(CString &szMessage,int index,BOOL (CLogMessage::*ptr) ());
	
public:
	CLog();
	virtual ~CLog();

public:	
	void Clear();

	void AddError(CString s);
	void AddWarning(CString s);
	void AddInfo(CString s);

	BOOL IsError();
	BOOL IsWarning();

public:
	int Count();
	CString GetMessage(int index);

	int GetError(CString& szError,int index);
	int GetWarning(CString& szWarning,int index);
	int GetInfo(CString& szInfo,int index);

};

#endif // !defined(AFX_LOG_H__E1F106DC_547B_478A_8F69_33849DB21F3D__INCLUDED_)
