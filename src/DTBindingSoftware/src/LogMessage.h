// LogMessage.h: interface for the CLogMessage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGMESSAGE_H__91CE3674_877A_4A84_9437_5B1E38D91E2B__INCLUDED_)
#define AFX_LOGMESSAGE_H__91CE3674_877A_4A84_9437_5B1E38D91E2B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CLogMessage  
{
	friend class CLog;

protected:
	static CLogMessage* Create();

protected:

	typedef enum 
	{
		LOGMESSAGE_TYPE_INFO =  0,
		LOGMESSAGE_TYPE_ERROR,
		LOGMESSAGE_TYPE_WARNING
		
	} ENUM_LOGMESSAGE_TYPE;

protected:

	ENUM_LOGMESSAGE_TYPE Type;
	CString szMessage;

public:
	CLogMessage();
	virtual ~CLogMessage();

	void SetInfo(CString s);
	void SetError(CString s);
	void SetWarning(CString s);

	BOOL IsTypeInfo();
	BOOL IsTypeWarning();
	BOOL IsTypeError();
};

#endif // !defined(AFX_LOGMESSAGE_H__91CE3674_877A_4A84_9437_5B1E38D91E2B__INCLUDED_)
