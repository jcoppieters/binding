#ifndef QUEUE_ERROR_POLICY_20070107_H
#define QUEUE_ERROR_POLICY_20070107_H



//////////////////////////////////////////////////////////////////////
#if defined(INCLUDE_DUOTECNO_RS232)
//////////////////////////////////////////////////////////////////////


/******************************************************************************/

namespace DUOTECNO {
namespace BUILDINGBLOCKS {

/******************************************************************************/

class CQueueErrorPolicy_None
{
public:
	CQueueErrorPolicy_None( );
   ~CQueueErrorPolicy_None( );

   void OnBufferFull();
};

/******************************************************************************/

class CQueueErrorPolicy_Notify
{
public:
	CQueueErrorPolicy_Notify( );
   ~CQueueErrorPolicy_Notify( );

   void OnBufferFull( );

};

/******************************************************************************/

class CQueueErrorPolicy_Fatal
{
public:
	CQueueErrorPolicy_Fatal( );
   ~CQueueErrorPolicy_Fatal( );

   void OnBufferFull( );

};

/******************************************************************************/

class CQueueErrorPolicy_Debug
{
public:
	CQueueErrorPolicy_Debug( );
   ~CQueueErrorPolicy_Debug( );

   void OnBufferFull( );

};

/******************************************************************************/

};
};

/******************************************************************************/


#endif
#endif


