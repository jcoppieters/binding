//#include "BuildingBlocks\SRC\BuildingBlocksLib.h"
/******************************************************************************/
#include "stdafx.h"
#include "ThreadSyncPolicy.h"



//////////////////////////////////////////////////////////////////////
#if defined(INCLUDE_DUOTECNO_RS232)
//////////////////////////////////////////////////////////////////////



/******************************************************************************/

//DEFINE_THIS_FILE;

/******************************************************************************/

CThreadSyncPolicy_None::CThreadSyncPolicy_None()
{

}

CThreadSyncPolicy_None::~CThreadSyncPolicy_None()
{

}

void CThreadSyncPolicy_None::Create()
{

}
void CThreadSyncPolicy_None::Lock()
{

}
void CThreadSyncPolicy_None::Unlock()
{

}
void CThreadSyncPolicy_None::Destroy()
{

}
void CThreadSyncPolicy_None::Reset()
{

}


/******************************************************************************/

CThreadSyncPolicy_TaskScheduling::CThreadSyncPolicy_TaskScheduling()
{

}
CThreadSyncPolicy_TaskScheduling::~CThreadSyncPolicy_TaskScheduling()
{

}
void CThreadSyncPolicy_TaskScheduling::Create()
{

}
void CThreadSyncPolicy_TaskScheduling::Lock()
{
	//RTX_Disable_Task_Scheduling();
	m_CriticalSection.Enter( );
}
void CThreadSyncPolicy_TaskScheduling::Unlock()
{
	//RTX_Enable_Task_Scheduling();
	m_CriticalSection.Leave( );
}
void CThreadSyncPolicy_TaskScheduling::Destroy()
{

}
void CThreadSyncPolicy_TaskScheduling::Reset()
{

}
/******************************************************************************/

CThreadSyncPolicy_Interrupt::CThreadSyncPolicy_Interrupt()
{
	//Create();
}
CThreadSyncPolicy_Interrupt::~CThreadSyncPolicy_Interrupt()
{
	//Destroy();
}

void CThreadSyncPolicy_Interrupt::Create()
{

}
void CThreadSyncPolicy_Interrupt::Lock()
{
	//asm { CLI };
}
void CThreadSyncPolicy_Interrupt::Unlock()
{
   //asm { STI };
}
void CThreadSyncPolicy_Interrupt::Destroy()
{

}
void CThreadSyncPolicy_Interrupt::Reset()
{

}

/******************************************************************************/

CThreadSyncPolicy_Mutex::CThreadSyncPolicy_Mutex() :
m_Mutex( )
{
	Create();
}

CThreadSyncPolicy_Mutex::~CThreadSyncPolicy_Mutex()
{
	Destroy();
}

void CThreadSyncPolicy_Mutex::Create()
{

}

void CThreadSyncPolicy_Mutex::Lock()
{
  	m_Mutex.Reserve( INFINITE );	//DUOTECNO::WAIT_INFINITE );
}

void CThreadSyncPolicy_Mutex::Unlock()
{
	m_Mutex.Release( );
}

void CThreadSyncPolicy_Mutex::Destroy()
{

}
void CThreadSyncPolicy_Mutex::Reset()
{

}

/******************************************************************************/

#endif
