// MessageError.h: interface for the CMessageError class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESSAGEERROR_H__3DAE394E_6572_4E4E_B5AF_2F1F6DB587B0__INCLUDED_)
#define AFX_MESSAGEERROR_H__3DAE394E_6572_4E4E_B5AF_2F1F6DB587B0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


struct ICommunicationFrame;
class CComApplication;

/**
 * Class CMessageError
 */
class CMessageError  
{
protected:
	const BYTE m_bErrorCode;
	const BYTE m_bNodeAddress;
	const BYTE m_bUnitAddress;

public:
	CMessageError(CComApplication* const pParent,const ICommunicationFrame* const pModemFrame);
	virtual ~CMessageError();

public:
	virtual CString ToString() const					= 0;
	virtual CString ToSmallString() const				= 0;
};

/**
 * class CMessageError_MessageError
 */
class CMessageError_MessageError : public CMessageError
{
private:
	#define MAX_MESSAGEERROR_ERROR_CODES			0x1B
	static const char* m_szErrorCodes[MAX_MESSAGEERROR_ERROR_CODES];

public:
	CMessageError_MessageError(CComApplication* const pParent,const ICommunicationFrame* const pModemFrame);
	virtual ~CMessageError_MessageError();

	// Sedert V16.30
	typedef enum {
		LOGLEVEL_UNDEFINED = 0,	
		LOGLEVEL_NORMAL = 1,
		LOGLEVEL_CONFIG = 2,
		LOGLEVEL_RUNTIME_BINDINGS = 3,
		LOGLEVEL_DEBUG = 4	
	} LogLevel_t;

public:
	virtual CString ToString() const;
	virtual CString ToSmallString() const;

	// Sedert V16.30
	LogLevel_t getLogLevel( ) const;
};


/**
 * class CMessageError_NodeBindingEntry
 */
class CMessageError_NodeBindingEntry : public CMessageError
{
private:
	#define MAX_BINDINGENTRY_ERROR_CODES			10
	static const char* m_szErrorCodes[MAX_BINDINGENTRY_ERROR_CODES];

public:
	CMessageError_NodeBindingEntry(CComApplication* const pParent, const ICommunicationFrame* const pModemFrame);
	virtual ~CMessageError_NodeBindingEntry();

public:
	virtual CString ToString() const;
	virtual CString ToSmallString() const;
};



/**
 * class CMessageError_NodeBindingFileControl
 */
class CMessageError_NodeBindingFileControl : public CMessageError
{
private:
	#define MAX_BINDINGFILE_ERROR_CODES			4
	static const char* m_szErrorCodes[MAX_BINDINGFILE_ERROR_CODES];

public:
	CMessageError_NodeBindingFileControl(CComApplication* const pParent,const ICommunicationFrame* const pModemFrame);
	virtual ~CMessageError_NodeBindingFileControl();

public:
	virtual CString ToString() const;
	virtual CString ToSmallString() const;

};

#endif // !defined(AFX_MESSAGEERROR_H__3DAE394E_6572_4E4E_B5AF_2F1F6DB587B0__INCLUDED_)
