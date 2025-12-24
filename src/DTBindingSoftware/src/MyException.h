// MyException.h: interface for the CMyException class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYEXCEPTION_H__856C00A1_EB21_4697_BD96_F408318A88F1__INCLUDED_)
#define AFX_MYEXCEPTION_H__856C00A1_EB21_4697_BD96_F408318A88F1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMyException  
{
public:
	CMyException() {};
	CMyException(UINT nResourceID);
	virtual ~CMyException();

	CString GetError(void)	{return szErrorMsg;};

protected:
	CString strClassError;
	CString szErrorMsg;

};

//////////////////////////////////////////////////////////////////
class CExceptionBindingInterface : public CMyException
{
public:
	CExceptionBindingInterface(UINT nResourceID);
	virtual ~CExceptionBindingInterface();
};

///////////////////////////////////////////////////////////////////
class CExceptionNodeDatabaseInterface : public CMyException
{
public:
	CExceptionNodeDatabaseInterface(UINT nResourceID);
	virtual ~CExceptionNodeDatabaseInterface();
};

////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_MYEXCEPTION_H__856C00A1_EB21_4697_BD96_F408318A88F1__INCLUDED_)
