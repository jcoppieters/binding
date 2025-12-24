// Event.h: interface for the CEvent class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EVENT_H__1CF77695_34F4_4548_8A27_8F3B0D71F2E2__INCLUDED_)
#define AFX_EVENT_H__1CF77695_34F4_4548_8A27_8F3B0D71F2E2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000




//////////////////////////////////////////////////////////////////////
#if defined(INCLUDE_DUOTECNO_RS232)
//////////////////////////////////////////////////////////////////////




/*******************************************************/

namespace DUOTECNO {
namespace KERNEL {
namespace WIN_32 {

/*******************************************************/



class CEvent  
{
protected:
	HANDLE m_hEvent;

public:
	CEvent();
	virtual ~CEvent();

	void Set();
	void Reset();
	BOOL Wait(long dwTimeout);

protected:
	void create( );

};


/*******************************************************/
};
};
};
/*******************************************************/


#endif


#endif // !defined(AFX_EVENT_H__1CF77695_34F4_4548_8A27_8F3B0D71F2E2__INCLUDED_)
