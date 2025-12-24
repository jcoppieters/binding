#ifndef THREADSYNCPOLICY_20071206_H
#define THREADSYNCPOLICY_20071206_H


//////////////////////////////////////////////////////////////////////
#if defined(INCLUDE_DUOTECNO_RS232)
//////////////////////////////////////////////////////////////////////



/******************************************************************************/
class CThreadSyncPolicy_None
{
public:
	CThreadSyncPolicy_None();
   ~CThreadSyncPolicy_None();

public:
  	void Lock();
   void Unlock();
   void Reset();

protected:
	void Create();
   void Destroy();
};

/******************************************************************************/
class CThreadSyncPolicy_TaskScheduling
{
protected:
	DUOTECNO::KERNEL::WIN_32::CCriticalSectionObj m_CriticalSection;

public:
	CThreadSyncPolicy_TaskScheduling();
   ~CThreadSyncPolicy_TaskScheduling();

public:
  	void Lock();
   void Unlock();
   void Reset();

protected:
	void Create();
   void Destroy();
};


/******************************************************************************/

class CThreadSyncPolicy_Interrupt
{
public:
	CThreadSyncPolicy_Interrupt();
   ~CThreadSyncPolicy_Interrupt();

public:
  	void Lock();
   void Unlock();
   void Reset();

protected:
	void Create();
   void Destroy();
};

/******************************************************************************/

class CThreadSyncPolicy_Mutex
{
protected:
	DUOTECNO::KERNEL::WIN_32::CMutex m_Mutex;

public:
	CThreadSyncPolicy_Mutex();
   ~CThreadSyncPolicy_Mutex();

public:
  	void Lock();
   void Unlock();
   void Reset();

protected:
	void Create();
   void Destroy();
};

/******************************************************************************/


#endif
#endif


