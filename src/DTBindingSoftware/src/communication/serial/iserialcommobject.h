#ifndef INCLUDED_ISERIALCOMMOBJECT_H
#define INCLUDED_ISERIALCOMMOBJECT_H
/*****************************************************************/
#ifndef INCLUDED_ICOMMUNICATIONOBJECT_H
	#include "../base/icommunicationobject.h"
#endif 
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
struct ICommunicationObject_Serial : public ICommunicationObject
{
	virtual ~ICommunicationObject_Serial();

	virtual BYTE MessageTransfer(BYTE cmd) = 0;
	virtual BOOL Send_PininstallConnect() = 0;
	virtual BOOL Send_PininstallDisconnect() = 0;	
	virtual BOOL Send_CanLoggerEnable(BOOL flag) = 0;
	virtual void ResetModem() = 0;		
	virtual void EvAfterAckTimeout(void) = 0;	
	virtual void EvAfterRetryTimeout(void) = 0;
	virtual void SetComPort(short Port) = 0;
	virtual short GetComPort(void) = 0;
	virtual void ParseRXMessage(void) = 0;
};

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
struct ISerialCommObject_Host
{
	virtual ~ISerialCommObject_Host() = 0;

	virtual void onSerialReceivedMessage(void) = 0;
	virtual void onSerialMessageSend(ICommunicationFrame* pFrame) = 0;
	virtual void onSerialReceivedAck(void) = 0;			
	virtual void onSerialReceivedNack(void) = 0;		
	virtual void onSerialReceivedModemCommand(void) = 0;
	virtual void onSerialReceivedBadChecksum(void) = 0;	
	virtual void onSerialCommBuffersFull(void) = 0;		
	virtual void onSerialNoResponse(void) = 0;		
	virtual void onSerialMaxRetries(void) = 0;	

	virtual void startTimer(unsigned int id,unsigned int value) = 0;
	virtual void stopTimer(unsigned int id) = 0;
};
/*****************************************************************/
#endif /* INCLUDED_ISERIALCOMMOBJECT_H */
/*****************************************************************/