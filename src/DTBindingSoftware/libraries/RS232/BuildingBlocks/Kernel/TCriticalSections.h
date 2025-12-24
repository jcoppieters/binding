#ifndef TCRITICAL_SECTION_20071011_H
#define TCRITICAL_SECTION_20071011_H



//////////////////////////////////////////////////////////////////////
#if defined(INCLUDE_DUOTECNO_RS232)
//////////////////////////////////////////////////////////////////////


/******************************************************************************/
namespace DUOTECNO {
namespace BUILDINGBLOCKS {
/******************************************************************************/


/**
 * Usefull for Disabling interrupts + Disabling Taskscheduling ...
 */
template <class THREADSYNCPOLICY>
 	class TCriticalSection
 	{
		private:
			THREADSYNCPOLICY m_ThreadSync;

      public:
        	TCriticalSection()
         {
         	m_ThreadSync.Lock( );
         }

         ~TCriticalSection()
         {
         	m_ThreadSync.Unlock( );
         }
   };


/**
 * Usefull to be used for a mutex lock/unlock pair ...
 */
template <class THREADSYNCPOLICY,int UID>
 	class TCriticalSection2
 	{
   	public:
   		typedef THREADSYNCPOLICY Mutex;

		private:
			static THREADSYNCPOLICY m_ThreadSync;

      public:
        	TCriticalSection2()
         {
         	m_ThreadSync.Lock( );
         }

         ~TCriticalSection2()
         {
         	m_ThreadSync.Unlock( );
         }
   };




/******************************************************************************/
};
};
/******************************************************************************/



#define INIT_CS_MUTEX(id)																		\
	typedef DUOTECNO::BUILDINGBLOCKS::TCriticalSection2<								\
   	CThreadSyncPolicy_Mutex,id> Critical_Section;    								\
	namespace {																						\
		Critical_Section::Mutex Critical_Section::m_ThreadSync;						\
   }

#define INIT_CS_MUTEX2(id)                                                    \
	typedef DUOTECNO::BUILDINGBLOCKS::TCriticalSection2<								\
   	CThreadSyncPolicy_Mutex,id>Critical_Section##id; 								\
   namespace {																						\
	   Critical_Section##id::Mutex Critical_Section##id::m_ThreadSync;	  		\
   };

#define INIT_CS_DISABLE_IRQ																	\
	typedef DUOTECNO::BUILDINGBLOCKS::TCriticalSection<								\
   	CThreadSyncPolicy_Interrupt> Critical_Section;


#define INIT_CS_DISABLE_TASKS																	\
	typedef DUOTECNO::BUILDINGBLOCKS::TCriticalSection<								\
   	CThreadSyncPolicy_TaskScheduling> Critical_Section;


#define INIT_CS_NONE																				\
   typedef DUOTECNO::BUILDINGBLOCKS::TCriticalSection<								\
   	CThreadSyncPolicy_None> Critical_Section;


// ----------------------------------------------------------------------------

#define ENTER_CRITICAL_SECTION			\
	Critical_Section 				cs;

#define ENTER_CRITICAL_SECTION2(id)    \
	Critical_Section##id 		cs;

// ----------------------------------------------------------------------------







/******************************************************************************/

#endif

#endif
