#ifndef QUEUE_SIMPLE_CIRC_20071102_H
#define QUEUE_SIMPLE_CIRC_20071102_H


//////////////////////////////////////////////////////////////////////
#if defined(INCLUDE_DUOTECNO_RS232)
//////////////////////////////////////////////////////////////////////

/******************************************************************************/
#include "BuildingBlocks_Include.h"
/******************************************************************************/

template
<
	int RX_QUEUE_SIZE,
	class CRITICAL_SECTION = DUOTECNO::BUILDINGBLOCKS::
   		TCriticalSection<DUOTECNO::BUILDINGBLOCKS::DefaultThreadSyncPolicy>,
	class QUEUEERRORPOLICY = DUOTECNO::BUILDINGBLOCKS::CQueueErrorPolicy_Notify
>
class TQueueSimpleCirc
{
protected:
	BYTE rxQueue[RX_QUEUE_SIZE];
	int rxQueueRead;
	int rxQueueWrite;

public:
  	TQueueSimpleCirc() : rxQueueRead(-1), rxQueueWrite(0) {};
   ~TQueueSimpleCirc() { };

   void Add(BYTE far* pbBuffer, int Length)
   {
   		CRITICAL_SECTION cs;

   		for(int i=0; i<Length; ++i)
		{
   			if( rxQueueWrite != rxQueueRead )  // Queue not reached maximum
      		{
      			rxQueue[rxQueueWrite] = ( (BYTE far *)pbBuffer )[i];

				// Was queue empty
   				if( rxQueueRead == -1 ) rxQueueRead = rxQueueWrite;

         		// Next place in queue
				rxQueueWrite = ( rxQueueWrite + 1 ) % RX_QUEUE_SIZE;
   			}
      		else
			{
				QUEUEERRORPOLICY ecPolicy;
				ecPolicy.OnBufferFull( );

				break;	/* TM 20080104 */
			}
   		}
   };

   int get(BYTE far* pbBuffer, int MaxLength)
   {
   		unsigned int cnt = 0;

     	CRITICAL_SECTION cs;
      
   		if (rxQueueRead != -1)
		{
      		for (int i=0;i<MaxLength;i++)
			{
         		pbBuffer[cnt++] = rxQueue[rxQueueRead];

         		rxQueueRead = ( rxQueueRead + 1 ) % RX_QUEUE_SIZE;

			  	if( rxQueueRead == rxQueueWrite )  // Was queue empty
           		{
			    	rxQueueRead = -1;
             		break;
           		}
			}
   		}
		return cnt;
	};

   unsigned int spaceFree() const 
   {
		unsigned int retVal = 0;

		CRITICAL_SECTION cs;		
		
		if (rxQueueWrite != rxQueueRead)
		{
			if (rxQueueRead == -1)
				retVal = RX_QUEUE_SIZE;
			else if (rxQueueWrite > rxQueueRead)
				retVal = (RX_QUEUE_SIZE - (rxQueueWrite - rxQueueRead));
			else
				retVal = (rxQueueRead - rxQueueWrite);	
		}

		return retVal;
   }

   unsigned int count() const 
   {
		unsigned int retVal = RX_QUEUE_SIZE;

		CRITICAL_SECTION cs;		
		
		if (rxQueueWrite != rxQueueRead)
		{
			if (rxQueueRead == -1)
				retVal = 0;
			else if (rxQueueWrite > rxQueueRead)
				retVal = rxQueueWrite - rxQueueRead;
			else
				retVal = (RX_QUEUE_SIZE - (rxQueueRead - rxQueueWrite));

		}
		
		return retVal;
   }
};
/******************************************************************************/
#endif
#endif
