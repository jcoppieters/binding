#pragma once

// MyWindowMessages.h

///////////////////// WINDOW MESSAGERS ///////////////////////

#define WM_NODENAME_CHANGED					WM_USER+102
#define WM_UNITNAME_CHANGED					WM_USER+103

/*************************************************************/
#define WM_SHOW_NODE_PROPERTIES				WM_USER+106
#define WM_SHOW_UNIT_PROPERTIES			WM_USER+107

#define WM_UNIT_CLICK_SHORT_TOGGLE		WM_USER+108
#define WM_UNIT_CLICK_LONG_ON			WM_USER+109
#define WM_UNIT_CLICK_LONG_OFF			WM_USER+110

#define WM_UNIT_STATUS_CHANGED			WM_USER+111

////////////////////////////////////////////////////////////////
#define WM_SHOW_UNITS_FORM				WM_USER+112
#define WM_SHOW_NODES_FORM				WM_USER+113
#define WM_SHOW_ADDFLOW_FORM			WM_USER+114

#define WM_UPDATE_NODES_FORM			WM_USER+115

#define WM_NODE_STATUS_CHANGED			WM_USER+116

#define WM_NODE_ETHERNETNWK_STATUS		(WM_USER+117)		// wParam = physical node address, 0xfffff wanneer onbekend...

/////////////////////////////////////////////////////////////
#define WM_DISPLAY_MESSAGE				WM_USER+125
#define WM_CLEAR_MESSAGES				WM_USER+126
#define WM_DISPLAY_TRACER_MESSAGEERROR	WM_USER+127
/////////////////////////////////////////////////////////////
#define WM_SHOW_NODE_UDPLANCFG			WM_USER+130			// wParam = logicalAddress		(Sedert V16.27)

/////////////////////////////////////////////////////////////
#define WM_TREE_NODE_EXPANDED			WM_USER+140
#define WM_TREE_NODE_COLLAPSED			WM_USER+141

/////////////////////////////////////////////////////////////
#define WM_COMMUNICATION_NOTIFY			WM_USER+150



/////////////////////////////////////////////////////////////
// Browse + Pnp Status messages
//
// source : CBrowsePnpEngine 
// dest : CMainFrame	
//
/////////////////////////////////////////////////////////////



// ----------------------------------------------------------

#define WM_BROWSEPNP_BROWSE_OK			WM_USER+180
#define WM_BROWSEPNP_PNP_OK				WM_USER+181
#define WM_BROWSEPNP_ERROR				WM_USER+182
#define WM_BROWSEPNP_CANCEL				WM_USER+183
#define WM_BROWSEPNP_NORESPONSE			WM_USER+184
#define WM_BROWSEPNP_DUPLICATES			WM_USER+185
#define WM_BROWSEPNP_FACTORYADDRESS		WM_USER+186

/////////////////////////////////////////////////////////////
#define WM_SHOW_DIALOG_UNITPROPERTIES		WM_USER+190


#define WM_SHOW_DIALOG_BINDINGUNIT			WM_USER+195
#define WM_DESTROY_DIALOG_BINDINGUNIT		WM_USER+196
#define WM_SHOW_DIALOG_BINDINGTIMER			WM_USER+197

#define WM_SHOW_DIALOG_FIRMWAREUPDATE		WM_USER+198			// wParam is nodeAddress, kan ook 0xFF zijn

/////////////////////////////////////////////////////////////
#define WM_ADDFLOW_ADDUNIT				WM_USER+200
#define WM_ADDFLOW_ADDCONTROL			WM_USER+201
#define WM_ADDFLOW_DRAGGING_UNIT		WM_USER+202
#define WM_ADDFLOW_DROP_UNIT			WM_USER+203
#define WM_ADDFLOW_REMOVE_CONTROL		WM_USER+204

#define WM_BINDINGENTRY_ADD				WM_USER+205
#define WM_BINDINGENTRY_DELETE			WM_USER+206
#define WM_BINDINGENTRY_EDIT			WM_USER+207
#define WM_BINDINGENTRY_NEW				WM_USER+208

#define WM_BINDINGENTRY_AUTO			WM_USER+209

//////////////////////////////////////:
#define WM_BINDINGDATABASE_UPDATE			WM_USER+220
#define WM_BINDINGDATABASE_ADDENTRY			WM_USER+221
#define WM_BINDINGDATABASE_REMOVEENTRY		WM_USER+222
#define WM_BINDINGDATABASE_NAMECHANGED		WM_USER+223

//////////////////////////////////////:
#define WM_BINDINGSNODE_ERASE_START			WM_USER+230
#define WM_BINDINGSNODE_ERASE_DONE			WM_USER+231
#define WM_BINDINGSNODE_ERASE_ERROR			WM_USER+232
#define WM_BINDINGSNODE_WRITE_START			WM_USER+233
#define WM_BINDINGSNODE_WRITE_DONE			WM_USER+234
#define WM_BINDINGSNODE_WRITE_ERROR			WM_USER+235
#define WM_BINDINGSNODE_READ_START			WM_USER+236
#define WM_BINDINGSNODE_READ_DONE			WM_USER+237
#define WM_BINDINGSNODE_READ_ERROR			WM_USER+238
#define WM_BINDINGSNODE_FSMWRITE_DONE		WM_USER+239
#define WM_BINDINGSNODE_FSMREAD_DONE		WM_USER+240

///////////////////////////////////////////////////////////////
#define WM_APPLICATION_FILENEW_PRE			WM_USER+250	// TM 20100511
#define WM_APPLICATION_FILENEW				WM_USER+251
#define WM_APPLICATION_FILEOPEN				WM_USER+252
#define WM_APPLICATION_FILESAVE_PRE			WM_USER+253	// TM 20100511
#define WM_APPLICATION_FILESAVE_POST		WM_USER+254	// TM 20100511

#define WM_APPLICATION_CLEARVIEWS			WM_USER+255
#define WM_APPLICATION_UPDATEVIEWS			WM_USER+256		// WParam=node address (of -1)
#define WM_APPLICATION_ONLINEIDLE			WM_USER+257		// WParam=BOOL (notifyConfigurator)

///////////////////////////////////////////////////////////
#define WM_TASK_INFO_DONE					WM_USER+270
#define WM_TASK_INFO_ERROR					WM_USER+271
#define WM_TASK_INFO_PROGRESS				WM_USER+272
#define WM_TASK_INFO_CANCEL					WM_USER+273

#define WM_DIALOG_APPLICATIONTASK_DONE		WM_USER+280
#define WM_DIALOG_APPLICATIONTASK_CANCEL	WM_USER+281
#define WM_DIALOG_APPLICATIONTASK_ERROR		WM_USER+282


///////////////////////////////////////////////////////////

#define WM_TREEVIEW_SORT_NODESBYNAME		WM_USER+300
#define WM_TREEVIEW_SORT_NODESBYPHADDRESS	WM_USER+301
#define WM_TREEVIEW_SORT_NODESBYADDRESS		WM_USER+302
#define WM_TREEVIEW_SORT_NODES				WM_USER+303
#define WM_TREEVIEW_SORT_NODESONLYNAME		WM_USER+304

#define WM_TREEVIEW_SORT_BINDINGS			WM_USER+305

#define WM_TREEVIEW_COLLAPSE				WM_USER+306
#define WM_TREEVIEW_EXPAND					WM_USER+307

#define WM_TREEVIEW_SORT_UNITS				WM_USER+308
#define WM_TREEVIEW_UNIT_BYNAME				WM_USER+309
#define WM_TREEVIEW_UNIT_BYADDRESS			WM_USER+310
#define WM_TREEVIEW_UNIT_ONLYNAME			WM_USER+311

#define WM_TREEVIEW_SORTBINDINGSONLYNAME	WM_USER+312
#define WM_TREEVIEW_SORTBINDINGSNAME		WM_USER+313
#define WM_TREEVIEW_SORTBINDINGSNUMBER		WM_USER+314

/////////////////////////////////////////////////////////////
#define WM_APPLICATION_NOTIFY_BINDING_ADD	WM_USER+350

/////////////////////////////////////////////////////////////
#define WM_APPLICATION_UPDATE_LCDSCREENMAP	WM_USER+360


// New Events to trigger FSM...
#define WM_SCREENMAP_FSM_TRIGGER_CANCEL		WM_USER+365
#define WM_SCREENMAP_FSM_TRIGGER_OK			WM_USER+366
#define WM_SCREENMAP_FSM_TRIGGER_ERROR		WM_USER+367

/////////////////////////////////////////////////////////////

#define WM_APPLICATION_MEMORYDUMP			WM_USER+370
#define WM_APPLICATION_UPLOADBITMAPS		WM_USER+380
#define WM_APPLICATION_SENDMMCONFIG			WM_USER+390
/////////////////////////////////////////////////////////////

#define WM_MYBUTTON_CLICKED					WM_USER+410




// TM 20050824 : New messages to give events to local Application FSM
#define WM_EV_APPLICATION_TASK_START		WM_USER+500
#define WM_EV_APPLICATION_TASK_DONE			WM_USER+501
#define WM_EV_APPLICATION_TASK_ERROR		WM_USER+502

// ...


/////////////////////////////////////////////////////////
#define WM_GENERICTASK_FSM_DONE				WM_USER+600
#define WM_GENERICTASK_FSM_ERROR			WM_USER+601
#define WM_GENERICTASK_FSM_CANCELLED		WM_USER+602
////////////////////////////////////////////////////////



/**
 *
 */
#define WM_COMMUNICATION_MESSAGE_RECEIVED					WM_USER+1000
#define WM_COMMUNICATION_MESSAGE_SEND						WM_USER+1001
#define WM_COMMUNICATION_COMMAND_RECEIVED					WM_USER+1002
#define WM_COMMUNICATION_ERROR								WM_USER+1004
// #define WM_COMMUNICATION_EVMESSAGEERROR				    WM_USER+1005

/**
 *
 */
#define WM_COMMUNICATION_MODEM_STATUS						WM_USER+1010
#define WM_COMMUNICATION_CONNECTION_LOST					WM_USER+1011	// Sedert V16.07: Verbinding verbroken.
#define WM_COMMUNICATION_MODEMLOGGERSTATUS					WM_USER+1012


/**
 * TM20120830
 */
#define WM_PROPERTYSHEET_NODES_ENTER	/*wParam=LA*/		(WM_USER+1020)
#define WM_PROPERTYSHEET_NODES_EXIT	    /*wParam=LA*/       (WM_USER+1021)


/**
 * Sedert versie 0x0C05
 */
enum
{
	USERMESSAGE_UPDATE_DATABASE_NEEDED = 0,
};

#define WM_APPLICATION_USERMESSAGE								(WM_USER+1022)	/* Param = type bericht */
#define WM_APPLICATION_NODE_SHOW_SERVICES 						(WM_USER+1023)	/* WParam = ptr naar een node */


// Sedert V16.40:
// Wanneer de DALI-bus configuratie gewijzigd wordt dan worden de wijzigingen opgeslagen in 
// het Json configuratie bestand.
// Als de configurator geopend is dan wordt er een windows IPC bericht verstuurd om te melden 
// dat het json export bestand gewijzigd werd.
#define WM_APPLICATION_CONFIGURATOR_DALI_RELOAD					(WM_USER+1024)	/* wParam = Node address */


// Sedert V16.40:
// Openen van de DALI subbus configuratie via de PropertyPageNodeConfig dialoog.
// zie CConfigAlgViewDALI
#define WM_APPLICATION_OPEN_DALISUBBUS_CFG						(WM_USER+1025)	/* wParam = Node address */

// Sedert V16.41
// Vragen om een individuele node te browsen.
#define WM_APPLICATION_BROWSE_SINGLE_NODE						(WM_USER+1026)	/* wParam = Node address, LPARAM = bool vragen om te browsen. */

// Sedert V16.43
// Na het versturen van de unit configuratie vragen om ook te browen op de module om de units toe te voegen.
#define WM_APPLICATION_UNITCONFIG_DONE							(WM_USER+1027)	/* wParam = Node address */
#define WM_APPLICATION_DALIBRIDGECONFIG_DONE					(WM_USER+1028)	/* wParam = Node address */

// Sedert V16.44:
// Wanneer er een logisch node address toegekend werd dit doorgeven aan de applicatie.
// Wordt verstuurd door CCommApplication naar CMainFrame.
#define WM_APPLICATION_NOTIFY_ASSIGNED_LA						(WM_USER+1029)	/* wParam = Node address */


// sedert V16.47: zie CHANGES_UPLOAD_UNITS
// Dit bericht wordt verstuurd vanuit de problem solve wizard en het startscherm nadat er een logisch adres toegekend werd
// aan een module met 0xFE adres
// De applicatie bepaalt via dit bericht welke taken er moeten uitgevoerd worden.
#define WM_NOTIFY_PROBLEMSOLVED_RESTORED_NODEADDRESS			(WM_USER+1030)	/* wParam = Node Address, lParam = fysisch adres */