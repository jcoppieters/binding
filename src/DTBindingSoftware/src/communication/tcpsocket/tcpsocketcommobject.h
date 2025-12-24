#ifndef INCLUDED_TCPSOCKET_TCPSOCKETCOMMOBJECT_H
#define INCLUDED_TCPSOCKET_TCPSOCKETCOMMOBJECT_H
/*****************************************************************/
#ifndef INCLUDED_TCPSOCKET_CMN_H
	#include "tcpsocketbase/tcpsocketcmn.h"
#endif 
#ifndef INCLUDED_TCPSOCKET_TCPSOCKETCOMMFRAME_H
	#include "tcpsocketbase/tcpsocketcommframe.h"
#endif
#ifndef INCLUDED_TCPSOCKET_ITCPSOCKETCOMMOBJECT_H
	#include "itcpsocketcommobject.h"
#endif
#ifndef INCLUDED_TCPSOCKETBASE_TCPSOCKETWIN_H
	#include "tcpsocketbase/tcpsocketwin.h"
#endif 
#ifndef INCLUDED_ICOMMUNICATIONFRAME_H
	#include "../base/icommunicationframe.h"
#endif 
#ifndef INCLUDED_TCPSOCKET_BASE_H
	#include "tcpsocketbase/tcpsocketbase.h"
#endif 
/*****************************************************************/
#define USE_CHANGES_CHECK_DUPLICATES		0
/*****************************************************************/
class CSocketMsgObserver_Impl;
class CTCPSocketWinHost_Impl;
class CTCPSocketCommFrame;
class CTCPSocketMsgQueue;
class CTCPSocketWin;
class CTCPSocketCommObjectHomeClient;
/*****************************************************************/
/*****************************************************************
 * Principe connectie maken.
 * 1) Socket connect.
 * 1.1) Wachten op socketconnected event.
 * 2) Request token.
 * 2.1) Wachten op ontvangen token.
 * 3) Login versturen.
 * 3.1) Wachten op connect status ok.
 *****************************************************************/

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
class CTCPSocketCommObject : public CTCPSocketBase, public ICommunicationObject_Tcp 
{
public:
	CTCPSocketCommObject(ITCPCommunicationObject_Host& refHost);
	virtual ~CTCPSocketCommObject( );

	//ICommunicationObject interface
	virtual void Init(HWND hParent = 0);
	virtual BOOL IsConnectionOpen(void);
	virtual BOOL OpenConnection();
	virtual BOOL CloseConnection();
	virtual BYTE MessageTransfer(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bMsgCode,
								 BYTE bLength=0,const CString& szData ='\0');
	virtual const ICommunicationFrame* GetRxModemFrame() const;
	virtual BOOL GetRXMsgFromQueue(void);


	virtual int GetIdentifier() const {
		return this->CTCPSocketBase::getSocketIdentifier();
	}
	virtual BOOL setSettings(const CTCPSocketCommSettings& settings);
	virtual ITcpSocketConnectFsmFunctions* getTcpSocketConnectFsmFunctions();

protected:
	enum { MAX_RX_FRAMES = 100 };

	// Deze wordt aangeroepen wanneer er een nieuwe CAN frame ontvangen wordt.
	// Moeten de frame in een queue stoppen en een event naar de applicatie geven...
	// Return waarde: TRUE bericht toegevoegd aan de queue, FALSE bericht niet toegevoegd.
	BOOL onRxNewFrame(CTCPSocketCommFrame* pFrame);

	CTCPSocketMessageAPI& getTxMsg() {
		return this->getSocketMessageAPI();
	}

#if(USE_CHANGES_CHECK_DUPLICATES == 1)
	BOOL checkDuplicate(CTCPSocketCommFrame* frame, unsigned long long timeStamp);
#endif 

private:
	BOOL addRxCanMsgFrame(CTCPSocketCommFrame* frame);
	BOOL getRxCanMsgFrame(CTCPSocketCommFrame* frame);

private:	
	CSocketMsgObserver_Impl* m_pObserver;
	CTCPSocketCommFrame* m_pCurrentRxFrame;
	CTCPSocketMsgQueue* m_rxFramesCanMessages;
	CTCPSocketCommFrame m_currentTxFrame;	

#if(USE_CHANGES_CHECK_DUPLICATES == 1)
	CTCPSocketCommFrame m_previousRxFrame;
	unsigned long long m_prevRxTimestamp;
#endif

	friend class CSocketMsgObserver_Impl;
	friend class CTCPSocketCommObjectHomeClient;
};
/*****************************************************************/

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
class CSocketMsgObserver_Impl : public ISocketMsgObserver
{
public:
	CSocketMsgObserver_Impl(CTCPSocketCommObject& parent);
	virtual ~CSocketMsgObserver_Impl();

	virtual BOOL onStatusConnectRequestKey(unsigned short protocolVersion,
		unsigned short softwareVersion,int tokenNumber,
		const char* szClientKey);
	virtual BOOL onStatusConnectPassword(BOOL stateConnected);
	virtual BOOL onStatusHeartbeat();	
	virtual BOOL onStatusCanRxMsgSubscribe(BOOL status);
	virtual BOOL onRecievedCanRxMsg(int nodeAddress, int unitAddress,
		int messageCode, int length, const BYTE* pbData);
	virtual BOOL onRecievedServerNodeInfo(int nodeAddress,unsigned long ulPhysicalAddress,const char* name,
		int numberOfUnits,int nodeType,int nodeFlags);
	// Sedert V16.26: Master functies.
	virtual BOOL onReceivedServerNodeStatusMaster(BOOL masterNode);
	virtual BOOL onReceivedServerNodeStatusMasterSupported(BOOL masterSupported);
	virtual BOOL onReceivedServerNodeMasterSet(BOOL success);

	// HomeClient functies.
	virtual BOOL onRecievedAppTaskStatus(TcpSocketHomeClient_Tasks::Task_t task, TcpSocketHomeClient_Tasks::Status_t status) {
		return FALSE;
	}
	virtual BOOL onRecievedAppTaskCancel(TcpSocketHomeClient_Tasks::Status_t status) {
		return FALSE;
	}
	virtual BOOL onRecievedAppTaskInfo(const CString& msg) {
		return FALSE;
	}
	virtual BOOL onRecievedAppTaskState(TcpSocketHomeClient_Tasks::State_t state) {
		return FALSE;
	}
	virtual BOOL onRecievedNodedatabaseState(TcpSocketHomeClient_NodeDbase::Status_t status) {
		return FALSE;
	}
	// Integratie DALI in TCP/IP protocol.
	virtual BOOL onStatusDALIRxMsgSubscribe(BOOL status) {
		return FALSE;
	}
	virtual BOOL onStatusDALIResponse(int clientIdentifier, BYTE msgIdentifier, BYTE type, BYTE responseData) {
		return FALSE;
	}
	virtual BOOL onRxDALIMessage(BYTE lengthBits, uint32_t recievedFrame) {
		return FALSE;
	}

	// Ontvangen bericht van de cloud server bridge.
	virtual BOOL onRecievedTunnelLoginResponse(BOOL status);

private:
	CTCPSocketCommObject& m_parent;
};

/*****************************************************************/
#endif 
/*****************************************************************/