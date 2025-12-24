// Mutex.h: interface for the CMutex class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MUTEX_H__E573FA5A_4A80_457E_A694_1454FFA3F04F__INCLUDED_)
#define AFX_MUTEX_H__E573FA5A_4A80_457E_A694_1454FFA3F04F__INCLUDED_

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

/********************************************************
 * A mutex kan gebruikt worden in een win32 omgeving
 * om een task exclusieve toegang te geven tot een gedeelde
 * resource.
 * 
 * Een mutex kan ook gebruikt worden voor communicatie tussen 
 * verschillende processen...
 *
 * Toestand van de mutex:
 *  - Signaled : Geen enkele thread heeft de mutex in gebruik.
 *  - Non signaled : De mutex is in gebruik door een thread.
 * 
 * 
 *******************************************************/

// om een mutex klasse aan te maken voor communicatie tussen
// verschillende processen kunnen we van deze klasse afleiden ...

class CMutex  
{
protected:
	void create( const char* name);

public:
	CMutex();
	CMutex(const char* szName);
	virtual ~CMutex();

	BOOL Reserve(long timeout);
    void Release();

protected:
    HANDLE m_hMutex;
};



/*******************************************************/
};
};
};
/*******************************************************/



#endif




#endif // !defined(AFX_MUTEX_H__E573FA5A_4A80_457E_A694_1454FFA3F04F__INCLUDED_)
