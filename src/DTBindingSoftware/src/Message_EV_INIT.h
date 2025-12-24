// Message_EV_INIT.h: interface for the CMessage_EV_INIT class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESSAGE_EV_INIT_H__3F4A355C_2685_4F06_9D30_97CACD386BB8__INCLUDED_)
#define AFX_MESSAGE_EV_INIT_H__3F4A355C_2685_4F06_9D30_97CACD386BB8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct ICommunicationFrame;
class CComApplication;


class CMessage_EV_INIT  
{
protected:
	#define MAX_RESET_MODES			0x06
	static const char* m_szResetMode[MAX_RESET_MODES];

protected:
	const BYTE m_bResetCode;
	const BYTE m_bNodeAddress;
	const BYTE m_bUnitAddress;

public:
	CMessage_EV_INIT(CComApplication* const pParent, const ICommunicationFrame* const pModemFrame);
	virtual ~CMessage_EV_INIT();

public:
	CString ToString() const;
	CString ToSmallString() const;

};

#endif // !defined(AFX_MESSAGE_EV_INIT_H__3F4A355C_2685_4F06_9D30_97CACD386BB8__INCLUDED_)
