//#include "BuildingBlocks\SRC\BuildingBlocksLib.h"
/******************************************************************************/
#include "stdafx.h"
#include "QueueErrorPolicy.h"



//////////////////////////////////////////////////////////////////////
#if defined(INCLUDE_DUOTECNO_RS232)
//////////////////////////////////////////////////////////////////////


/******************************************************************************/
using namespace DUOTECNO::BUILDINGBLOCKS;
/******************************************************************************/

DEFINE_THIS_FILE;

/******************************************************************************/

DUOTECNO::BUILDINGBLOCKS::CQueueErrorPolicy_None::
CQueueErrorPolicy_None()
{

}

DUOTECNO::BUILDINGBLOCKS::CQueueErrorPolicy_None::
~CQueueErrorPolicy_None( )
{

}


void DUOTECNO::BUILDINGBLOCKS::CQueueErrorPolicy_None::
OnBufferFull()
{

}


/******************************************************************************/

DUOTECNO::BUILDINGBLOCKS::CQueueErrorPolicy_Notify::
CQueueErrorPolicy_Notify()
{

}

DUOTECNO::BUILDINGBLOCKS::CQueueErrorPolicy_Notify::
~CQueueErrorPolicy_Notify( )
{

}


void DUOTECNO::BUILDINGBLOCKS::CQueueErrorPolicy_Notify::
OnBufferFull()
{
	TRACE("Error: Queue Full, Data was lost\n");
}

/******************************************************************************/

DUOTECNO::BUILDINGBLOCKS::CQueueErrorPolicy_Fatal::
CQueueErrorPolicy_Fatal()
{

}

DUOTECNO::BUILDINGBLOCKS::CQueueErrorPolicy_Fatal::
~CQueueErrorPolicy_Fatal( )
{

}


void DUOTECNO::BUILDINGBLOCKS::CQueueErrorPolicy_Fatal::
OnBufferFull()
{
	TRACE("Error: Queue Full, Data was lost\n");

	ALLEGE( 0 );
}


/******************************************************************************/

DUOTECNO::BUILDINGBLOCKS::CQueueErrorPolicy_Debug::
CQueueErrorPolicy_Debug()
{

}

DUOTECNO::BUILDINGBLOCKS::CQueueErrorPolicy_Debug::
~CQueueErrorPolicy_Debug( )
{

}


void DUOTECNO::BUILDINGBLOCKS::CQueueErrorPolicy_Debug::
OnBufferFull()
{
	TRACE("Error: Queue Full, Data was lost\n");

	ASSERT_FAILED;
}


#endif

