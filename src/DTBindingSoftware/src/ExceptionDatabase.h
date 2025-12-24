// ExceptionDatabase.h: interface for the CExceptionDatabase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXCEPTIONDATABASE_H__C4154F5A_068A_4B96_946B_603BA1E7C0FA__INCLUDED_)
#define AFX_EXCEPTIONDATABASE_H__C4154F5A_068A_4B96_946B_603BA1E7C0FA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CExceptionDatabase : public CException  
{
protected:
	CString m_szErrorMessage;
public:
	CExceptionDatabase(CString szMessage);
	virtual ~CExceptionDatabase();

public:
	void Display();
};

#endif // !defined(AFX_EXCEPTIONDATABASE_H__C4154F5A_068A_4B96_946B_603BA1E7C0FA__INCLUDED_)
