#pragma once

/////////////////////////////////////////////////////////////////////
#define BROADCAST_UNIT	0xFF
#define BROADCAST_NODE	0x00

/////////////////////////////////////////////////////////////////////
// BROWSE RESULT CODES
#define BROWSE_USER_CANCEL					0
#define BROWSE_ERROR						1
#define BROWSE_OK							2
#define BROWSE_NORESPONSE					5

/////////////////////////////////////////////////////////////////////
// PNP RESULT CODES
#define PNP_USER_CANCEL						0
#define PNP_ERROR							1
#define PNP_OK								2
#define PNP_FACTORY_ADDRESS_FOUND			3
#define PNP_DUPLICATES_FOUND				4
#define PNP_NORESPONSE						5

//--------------------------------------------
#define EV_BROWSE_FOR_NODES					0
#define EV_BROADCAST_TIMEOUT				1
#define EV_PNP_BROWSE_PHYSICAL				0	// TM 20040707	PNP EVENTS
#define EV_PNP_TIMEOUT						1	// TM 20040707	PNP EVENTS
#define EV_BROWSE_GET_UNIT_INFO				2
// #define EV_BROWSE_ERROR					3
#define EV_UNIT_NAME_READ_OK				4
#define EV_UNIT_NAME_END					5
#define EV_UNIT_NAME_READ_ERROR				6
#define EV_NODE_NAME_READ_OK				7
#define EV_NODE_NAME_END					8
#define EV_NODE_NAME_READ_ERROR				9
#define EV_NODE_RESOURCES0					10	// Oppassen: Resource block events moeten op elkaar volgen
#define EV_NODE_RESOURCES1					11	// Oppassen: Resource block events moeten op elkaar volgen
#define EV_NODE_RESOURCES2					12	// Oppassen: Resource block events moeten op elkaar volgen
#define EV_NODE_RESOURCES3					13	// Oppassen: Resource block events moeten op elkaar volgen
#define EV_NODE_RESOURCES4					14	// Oppassen: Resource block events moeten op elkaar volgen
#define EV_BROWSE_CLOSE_FILE				15
#define EV_NODE_CONFIG						16
#define EV_BROWSE_NODEBINDING_INFO			17		
#define EV_BROWSE_USER_CANCEL				20
#define EV_USER_CANCEL						20
// TM 20040707	PNP EVENTS
#define EV_PNP_ADDRESS_ASSIGNED				22
#define EV_PNP_PHYSICAL_ADDRESS				23
#define EV_PNP_POLL_FSM						24	// V16.43

#define EV_PNP_USER_CANCEL					20
#define EV_USER_CANCEL						20

// PNP + BROWSE EVENT
#define EV_TIMEOUT							1
#define EV_NORESPONSE						30	//new

// PNP-MODES
#define PNP_MODE_AUTOMATIC					0
#define PNP_MODE_RETRIEVE_INFO				1
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
class IBrowsePnPEngine
{
public:
	// Browse FSM ...
	virtual void Fire_OnBrowseDone(void) = 0;
	virtual void Fire_OnBrowseCancel(void) = 0;
	virtual void Fire_OnBrowseError(void) = 0;
	virtual void Fire_OnBrowseTimeout(void) = 0;

	// PNP FSM ...
	virtual	void Fire_OnPNPCancel(void)	= 0;	
	virtual void Fire_OnPNPError(void)	= 0;
	virtual void Fire_OnPNPOK(void)	= 0;
	virtual void Fire_OnFactoryAddressFound(void) = 0;
	virtual void Fire_OnDuplicatesFound(void) = 0;
	virtual void Fire_OnPNPNoResponse(void)	= 0;
	// Een physical ack versturen naar een node.
	virtual BOOL sendPnpPhysicalAck(unsigned long ulPhysicalAddress) = 0;

	// GENERAL 
	virtual void StartPnPTimer(int reloadValue) = 0;
	virtual void StopPnPTimer(void) = 0;
	virtual void StartPnPAckTimer(void) = 0;
	virtual void StopPnPAckTimer(void) = 0;
	virtual void StartBrowseTimer(int reloadValue) = 0;
	virtual void StopBrowseTimer(void) = 0;
	virtual void StartPnpPollTimer(int reloadValue) = 0;		// V16.43
	virtual void StopPnpPollTimer( ) = 0;						// V16.43
};