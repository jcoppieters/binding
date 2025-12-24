#ifndef INCLUDED_COMMUNICATION_ICONNECTFSM_H
#define INCLUDED_COMMUNICATION_ICONNECTFSM_H
/*****************************************************************/
struct ICCommunicationConnectFsm 
{
	virtual ~ICCommunicationConnectFsm() = 0;

	virtual void EvOperationConnect( ) = 0;
	virtual void EvAfterTimeout() = 0;
};

struct ICCommunicationConnectFsm_Host {
	virtual ~ICCommunicationConnectFsm_Host() = 0;	
	virtual void displayMsgError(const CString& msg) = 0;
	virtual void displayMsgInfo(const CString& msg) = 0;
	virtual void startTimer(unsigned int timerID,unsigned int value) = 0;
	virtual void stopTimer(unsigned int timerID) = 0;
	virtual void notifyFsmDone( ) = 0;
	virtual void notifyFsmError( ) = 0;
	virtual bool checkFirmwareVersion(int version) = 0;		
	virtual bool checkNodeProperties(int nodeAddress, unsigned long ulPhysicalAddress, int nodeType, int nodeFlags) = 0;
};
/*****************************************************************/
#endif
/*****************************************************************/