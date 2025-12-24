#ifndef INCLUDED_DTTHREADWIN32_H
#define INCLUDED_DTTHREADWIN32_H
/**************************************************************/
#ifndef __THREADBASE_H__
	#include "threadbase.h"
#endif 
/**************************************************************/
namespace DUOTECNO {
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
class CThreadEventWin
{
public:
	CThreadEventWin(BOOL manualReset = TRUE, BOOL initialState = FALSE);
	virtual ~CThreadEventWin();
	
	void Set();
	void Reset();
	BOOL Wait(long dwTimeout);

protected:
	void create(BOOL manualReset, BOOL initialState);

private:
	HANDLE m_hEvent;
};
/**************************************************************/


/**************************************************************/
/**************************************************************/
/**************************************************************/
class CThreadMutexWin
{
public:
	CThreadMutexWin();
	CThreadMutexWin(const char* szName);
	virtual ~CThreadMutexWin();

	BOOL Reserve(long timeout);
    void Release();

protected:
	void create( const char* name);

private:
    HANDLE m_hMutex;
};
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
// Deze werd nog nergens gebruikt ??
class CThreadCSWin 
{
public:
	CThreadCSWin();
	virtual ~CThreadCSWin();

	//BOOL TryEnter( );
	void Enter( );
	void Leave( );

private:
	CRITICAL_SECTION m_hCriticalSection;
};
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
class CThreadWin : public CThreadBase
{
public:
	CThreadWin();
	virtual ~CThreadWin();

	virtual DWORD Start( void* arg = NULL);
	void Kill(void);
	BOOL Running( );

protected:
	virtual DWORD Run(LPVOID arg) = 0;		// Newly Created Thread
	virtual void ThreadCtor();
	virtual void ThreadDtor();
	BOOL requestThreadStop( );

private:
	enum { TIMEOUT_MSEC = 1000 };
	HANDLE m_ThreadStop;
	CThreadEventWin m_threadRunning;
};
/**************************************************************/

/**************************************************************/
}; /* namespace DUOTECNO */
/**************************************************************/
#endif 
/**************************************************************/
