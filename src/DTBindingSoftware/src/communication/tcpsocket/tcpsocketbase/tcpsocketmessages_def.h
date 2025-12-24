#ifndef INCLUDED_TCPSOCKET_TCPSOCKETMESSAGES_DEF_H
#define INCLUDED_TCPSOCKET_TCPSOCKETMESSAGES_DEF_H
/*****************************************************************/

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
struct Connect_pr2
{
	enum
	{
		EV_CLIENTCONNECTSTATUS	= 67, 	// pr1 + pr2
		FC_CLIENTCONNECTSET		= 214	// pr1 + pr2
	};

	enum
	{
		OBJECT_DISCONNECT       = 0,	// pr2 : client + server.
		OBJECT_RESERVED			= 1,	// reserve: gebruikt bij pr1
		OBJECT_PASSWORD_SECURE  = 2,	// Don't use: gebruikt bij pr1
		OBJECT_PASSWORD			= 3, 	// pr2: client: max 16 char, server response
		OBJECT_KEY = 4,					// ebkel pr2
		OBJECT_PASSWORD_SECURE_V2 = 5,	// enkel pr2
		MAX_ATTRIBUTES
	};
};
/*****************************************************************/

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
struct ProtocolMsg {
	enum {
		EV_PROTOCOLMSG = 122,				// 0x7a
		FC_PROTOCOLMSG = 245 				// 0xf5
	};

	enum Object {
		OBJECT_CANBUS_MSG 				= 0x01,
		ATTR_SUBSCRIBE_CANBUS_MSG 		= 0x02,
		ATTR_SUBSCRIBE_DALIBUS = 0x03,
		OBJECT_DALIBUS_MSG = 0x04,
		OBJECT_DALIBUS_RESPONSE = 0x05,
		MAX_ATTRIBUTES				 	// Altijd als laatste
	};
};
/*****************************************************************/

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
struct Heartbeat
{
	enum
	{
		EV_HEARTBEATSTATUS_OLD				= 70,	// pr1 + pr2	(deprecated)
		EV_HEARTBEATSTATUS               	= 72,	// pr1 + pr2
		FC_HEARTBEATREQUESTSTATUS			= 215,	// pr1 + pr2
	};

	enum
	{
		OBJECT_POLL_OLD          			= 0,	// pr1 + pr2 (deprecated)
		OBJECT_POLL  		               	= 1,	// pr1 + pr2
		MAX_ATTRIBUTES
	};
};

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
struct NodeManagement
{
	enum
	{
		EV_NODEMANAGEMENTINFO 		= 74, 	// pr1 + pr2
		FC_NODEMANAGEMENTSET       	= 219,	// pr1 + pr2
		FC_REQUESTNODEMANAGEMENT	= 220	// pr1 + pr2
	};

	enum Object
	{
		ATTR_MASTER_SUPPORTED      	= 0,	// pr1 + pr2
     	OBJECT_SET_MASTER          	= 1,	// pr1 + pr2
		OBJECT_SET_MASTER_OK       	= 2,	// pr1 + pr2
		OBJECT_SET_MASTER_EC       	= 3,	// pr1 + pr2
		ATTR_MASTER				 	= 4,	// pr1 + pr2
		ATTR_NODEINFO				= 5,	// pr1 + pr2
		ATTR_VERSION_SOFTWARE      	= 6,	// pr1 + pr2
		ATTR_VERSION_PROTOCOL     	= 7,	// pr1 + pr2
		MAX_ATTRIBUTES
	};
};

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
struct ApplicationTask
{
	enum
	{
		FC_APPLICATIONTASKSET = 212,	// pr1 en pr2 ???
		FC_APPLICATIONTASKREQUEST = 213,	// pr1 en pr2 ???
		EV_APPLICATIONTASKSTATUS = 66,		// pr1 en pr2 ???
	};

	enum Object
	{
		OBJECT_BROWSE = 0,	// NIET VERANDEREN !!
		OBJECT_BROWSECLEAR = 1,	// NIET VERANDEREN !!
		OBJECT_PNP = 2,	// NIET VERANDEREN !!
		OBJECT_UPDATEDBASE = 3,	// NIET VERANDEREN !!
		OBJECT_RESETALL = 4,  	// NIET VERANDEREN !!
		OBJECT_FACTORY_RESETALL = 5,	// NIET VERANDEREN !!
		ATTR_STATE = 6,	// NIET VERANDEREN !!
		OBJECT_CANCEL = 7,	// NIET VERANDEREN !!
		OBJECT_INFO = 8,	// NIET VERANDEREN !!
		MAX_ATTRIBUTES                          // laatste - NIET VERANDEREN !!
	};
};

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
struct NodeDabase
{
	enum
	{
		EV_NODEDATABASEINFO 		= 64, 		// pr1 + pr2
		FC_REQUESTNODEDATABASE		= 209,	  	// pr1 + pr2
		FC_NODEDATABASESET      	= 221		// pr1 + pr2
	};

	enum Object 
	{
		ATTR_INFO				= 0,			// pr1
		ATTR_NODEINFO			= 1,			// pr1
		ATTR_UNITINFO			= 2,			// pr1
		ATTR_UNITSTATUS			= 3,			// pr1 + pr2
		ATTR_NODESTATUS			= 4,			// pr1
		ATTR_STATE				= 5,			// pr1 + pr2
		OBJECT_ERASE         	= 6,			// pr1 + pr2
		MAX_ATTRIBUTES
	};
};


/*****************************************************************/
#endif
/*****************************************************************/