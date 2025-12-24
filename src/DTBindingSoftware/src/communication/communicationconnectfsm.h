#ifndef INCLUDED_COMMUNICATION_CONNECTFSM_H
#define INCLUDED_COMMUNICATION_CONNECTFSM_H
/*****************************************************************/
#ifndef INCLUDED_COMMUNICATION_ICONNECTFSM_H
	#include "base/icommunicationconnectfsm.h"
#endif 
#ifndef INCLUDED_COMMUNICATION_SERVERINFO_H
	#include "base/communicationserverinfo.h"
#endif
/*****************************************************************/
class IComApplication; 
class CCommunicationObject;
/*****************************************************************/

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
class CCommunicationConnectFsm : public ICCommunicationConnectFsm_Host
{
public:
	CCommunicationConnectFsm(IComApplication *pCom);			
	virtual ~CCommunicationConnectFsm();	

	BOOL startConnect(HWND hParent,CCommunicationObject *pCommObject);	

	// Sedert V16.14: Toelaten om te connecteren op de touchscreen 
	// via tcp/ip configuratie.
	void setEnableCfgTouchscreen(bool flag) {
		m_allowConnectTouchscreen = flag;
	}

	// Opvragen van de server info wanneer de connectie os is.
	const CCommunicationServerInfo& getServerInfo() {
		return m_serverInfo;
	}

public: // Serial functions.
	void EvSignalAckReceived();
	void EvAfterTimeout();
	void EvSignalMessageReceived();
	void EvSignalModemStatus();
	void EvSignalModemAddress();
	void EvSignalSoftwareVersion( );

public: // ICCommunicationConnectFsm_Host
	void displayMsgError(const CString& msg);
	void displayMsgInfo(const CString& msg);
	void startTimer(unsigned int timerID,unsigned int value);
	void stopTimer(unsigned int timerID);
	void notifyFsmDone( );
	void notifyFsmError( );
	bool checkFirmwareVersion(int version);
	bool checkNodeProperties(int nodeAddress, unsigned long ulPhysicalAddress, int nodeType, int nodeFlags);

private:
	ICCommunicationConnectFsm* m_pImpl;				// de implemententie van de fsm.
	IComApplication* m_pCommApplication;			// de parent.
	CCommunicationObject* m_pCommunicationObject;	// Communication object --> Nodig voor het updaten van de server info.
	CCommunicationServerInfo m_serverInfo;			// deze moet ingevuld worden door de Fsm implementatie.
	bool m_allowConnectTouchscreen;					// Sedert V16.14
};
/*****************************************************************/
#endif
/*****************************************************************/