#ifndef INCLUDED_COMMUNICATION_H
#define INCLUDED_COMMUNICATION_H
/*****************************************************************/
#ifndef INCLUDED_SERIAL_MODEMFRAME_DEF_H
	#include "serial/modemframe_def.h"				// Deze is nodig voor het afhandelen van specifieke rs232 modem commands.
#endif 
#ifndef INCLUDED_SERIAL_SERIALCOMMOBJECT_DEF_H
	#include "serial/serialcommobject_def.h"		// Deze is nodig voor windows message handling rs232 data
#endif 
#ifndef INCLUDED_ISERIALCOMMOBJECT_H
	#include "serial/iserialcommobject.h"
#endif 
#ifndef INCLUDED_TCPSOCKET_ITCPSOCKETCOMMOBJECT_H
	#include "tcpsocket/itcpsocketcommobject.h"
#endif
#ifndef INCLUDED_COMMUNICATION_SERVERINFO_H
	#include "base/communicationserverinfo.h"
#endif
#ifndef INCLUDED_TCPSOCKET_ITCPSOCKETHOMECLIENT_H
	#include "tcpsocket/homeclient/itcpsockethomeclient.h"
#endif 
#ifndef INCLUDED_TCPSOCKET_OPTIONS_H
	#include "tcpsocket/tcpsocketbase/tcpsocketoptions.h"
#endif
#ifndef INCLUDED_COMMUNICATION_SETTINGS_H
	#include "communicationsettings.h"
#endif 
/*****************************************************************/
class IComApplication;
class CCommunicationStatus;
class CCommunicationServerInfo;
class CTCPSocketOptions;				// nodig voor het aanmaken van de homeclient socket.
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/

/*****************************************************************/
class CCommunicationObject : public ISerialCommObject_Host, 
							 public ITCPCommunicationObject_Host 						  
{
public:
	CCommunicationObject(IComApplication& refParent);
	virtual ~CCommunicationObject( );

	// Opvragen van het type communicatie.
	typedef enum {
		NONE = 0, SERIAL, TCPIP
	} type_t;

	type_t getType() const; 

	// Bij een connectie met een tcpip server opvragen hoe we geconnecteerd zijn.
	ConnectionMethod_t getTcpConnectionMethod( ) const;

	// In functie van de communicatatie instellingen een implementatie aanmaken...
	ICommunicationObject* createImpl(HWND hParent);	
	void destroy();

	// Deze gebruiken om alle connecties af te sluiten.
	// Aanroepen bij afsluiten van de applicatie of wanneer er een nieuwe installatie gestart wordt.
	void closeConnections( );

	// Onderstaande nog opkuisen !!!
	typedef enum {
		RESULT_RX_MSG_NONE = -1,			// Geen bericht
		RESULT_RX_MSG_MODEMFRAME = 0,		// Modem bericht die moet geparsed worden.
		RESULT_RX_MSG_LOG = 1				// Modem log berichten (=Can-Logger)
	} ResultRxMsg_t;
	ResultRxMsg_t GetRxMsg();
	BOOL IsConnectionOpen(void);
	BYTE MessageTransfer(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bMsgCode,BYTE bLength=0,const CString& szData ='\0');
	const ICommunicationFrame* GetRxModemFrame() const;		
	void CloseConnection();

	/************************* Opvragen van de status *********************/
	CCommunicationStatus& getCommunicationStatus();
	const CCommunicationStatus& getCommunicationStatus() const;

	/************************* Serial functions ***************************/
	void ParseRxSerialData(void);
	BYTE SendSerialModemCommand(BYTE cmd);
	void ResetSerialModem();
	BOOL SendSerialModemCANLoggerOnOff(BOOL fState);

	void EvAfterAckTimeout(void);
	void EvAfterRetryTimeout(void);
	
	void setSettings(const CCommunicationSettingsUsb& settings);
	void setSettings(const CCommunicationSettingsTcp& settings);	
	short GetComPort(void);

	//////////////////////// Socket homeClient.
	// Sedert V16.36
	// Aanpassingen aan de HomeClient socket.
	// Aparte create functie met opties
	ITcpSocketHomeClient* createSocketHomeClient(int nodeAddress, const CTCPSocketOptions& refOptions); 
	
	// sedert V16.36
	// Opvragen van de instantie van de homeclient ifv het node address.
	// Deze moet eerst aangemaakt worden via de createSocketHomeClient()
	ITcpSocketHomeClient* getSocketHomeClient(int nodeAddress);	

protected:
	BOOL GetRXMsgFromQueue(void);
	CCommunicationSettings& getSettings( ) {
		return m_settings;
	}
	void destroyHomeClients();


public: // ICommunicationObject_Host
	virtual void displayMsgInfo(int socketIdentifier, const CString& msg);
	virtual void displayMsgError(int socketIdentifier, const CString& msg);

public:	// ISerialCommObject_Host
	virtual void onSerialReceivedMessage(void);
	virtual void onSerialMessageSend(ICommunicationFrame* pFrame);
	virtual void onSerialReceivedAck(void);			
	virtual void onSerialReceivedNack(void);		
	virtual void onSerialReceivedModemCommand(void);
	virtual void onSerialReceivedBadChecksum(void);	
	virtual void onSerialCommBuffersFull(void);		
	virtual void onSerialNoResponse(void);		
	virtual void onSerialMaxRetries(void);	
	virtual void startTimer(unsigned int id,unsigned int value);
	virtual void stopTimer(unsigned int id);

public:	// ITCPCommunicationObject_Host
	virtual void onTcpReceivedMessage(int socketIdentifier);
	virtual void onTcpMessageSend(int socketIdentifier, ICommunicationFrame* pFrame);
	virtual void onTcpSocketClosed(int socketIdentifier);
	// Sedert V16.26: Nodig voor de DT18-PRO.
	// Instellen van master of slave node address.
	virtual void updateServerInfo(int nodeAddress, const char* name, int numberOfUnits, int nodeFlags);

private:
	IComApplication& m_refParent;
	CCommunicationSettings m_settings;
	ICommunicationObject* m_pImpl;														// pointer naar de implementatie: m_pImplSerial of m_pImplTcp
	CCommunicationStatus* m_pCommunicationStatus;			
	ICommunicationObject_Serial* m_pImplSerial;											// Dyn alloc implementatie.
	ICommunicationObject_Tcp* m_pImplTcp;												// Dyn alloc implementatie.
	CMap<int, int, ITcpSocketHomeClient*,ITcpSocketHomeClient*> m_mapHomeClients;		// Sedert V16.35: Instanties van de homeclient worden bijgehouden dmv logisch node address

	friend struct CCommunicationObject_Utils;
};
/*****************************************************************/

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
struct CCommunicationObject_Utils
{
	static void clearServerInfo(CCommunicationObject* pThis);
	static void updateServerInfo(CCommunicationObject* pThis, const CCommunicationServerInfo& serverInfo);

	////////////////////////////////////////////////////////////////////////////
	// Functies enkel mogelijk wanneer geconnecteerd zijn met een TCP/IP server.
	////////////////////////////////////////////////////////////////////////////
	// Opvragen van de tcp/ip node server info.
	static bool sendRequestServerNodeInfo(CCommunicationObject* pThis);

	// De tcp/ip server instellen als master in het CAN-bus systeem.
	static bool sendCfgServerAsMaster(CCommunicationObject* pThis);

protected:
	static bool get(CCommunicationObject* pThis,ITcpSocketConnectFsmFunctions** ppFunctions);
};
/*****************************************************************/
#endif
/*****************************************************************/