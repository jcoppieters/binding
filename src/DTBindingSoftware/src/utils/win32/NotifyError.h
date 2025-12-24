// NotifyError.h: interface for the CNotifyError class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NOTIFYERROR_H__FAA97381_DAC5_40F7_BA6A_931C158B8183__INCLUDED_)
#define AFX_NOTIFYERROR_H__FAA97381_DAC5_40F7_BA6A_931C158B8183__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <afxtempl.h>


/********************************************************************************/

class CNotifyError  
{
public:
	CNotifyError( );
	virtual ~CNotifyError();

	virtual void operator()(int error) = 0;
	virtual void operator()(CString szError) = 0;
};

/********************************************************************************/

class CNotifyError_None : public CNotifyError
{
public:
	CNotifyError_None( );
	virtual ~CNotifyError_None();

	virtual void operator()(int error);
	virtual void operator()(CString szError);
};


/********************************************************************************/

class CNotifyError_Simple : public CNotifyError
{
public:
	CNotifyError_Simple( );
	virtual ~CNotifyError_Simple();

	virtual void operator()(int error);
	virtual void operator()(CString szError);
};


/********************************************************************************/

class CNotifyError_FileOpener : public CNotifyError_Simple
{
protected:
	CMap<int,int,CString,CString> m_ErrorMessages;

protected:
	typedef CNotifyError_Simple Base;

public:
	CNotifyError_FileOpener( );
	virtual ~CNotifyError_FileOpener();

	virtual void operator()(int error);
};

/********************************************************************************/

class CNotifyError_PdfOpener : public CNotifyError_FileOpener
{
protected:
	typedef CNotifyError_FileOpener Base;

public:
	CNotifyError_PdfOpener( );
	virtual ~CNotifyError_PdfOpener();

	//virtual void operator()(int error);
};

/********************************************************************************/


#endif // !defined(AFX_NOTIFYERROR_H__FAA97381_DAC5_40F7_BA6A_931C158B8183__INCLUDED_)
