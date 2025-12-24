//*************************************************************************************
// Node type : NODE_SENS_CONTROL
// File      : nodemessage.h
// Author    : Koenraad Vanderschaeve
// Date      : 30/03/1998
// Target    : Debug   : PC P200, MSVC++
//             Runtime : Atmel 89C52 microcontroller  
//
// Changes   : KV 30/03/1998 V1.0 Initial version.
// Changes	 : TM 03/04/2012 Sens attributen bijgevoegd.
// Changes	 : TM 20/08/2012 Gateway config attributen bijgevoegd
// Changes	 : TM 17/01/2012 RoomCtrl Audio config attributen bijgevoegd.
// Changes	 : TM 15/03/2016 Sensor unit delegation.
// Changes	 : TM 21/04/2016 AVMatrix methods voor zone groep & ungroep functies.
//*************************************************************************************

// Messages to be processed by the application state machine

#ifndef INCLUDED_NODEMESS_H
#define INCLUDED_NODEMESS_H


#define    EV_UNITCONTROLTOGGLE  0x01
#define    EV_UNITCONTROLPULSTOGGLE  0x02
#define    EV_UNITCONTROLPULS  0x03
#define    EV_UNITCONTROLSTATUS  0x04
#define    EV_UNITDIMSTATUS  0x05
#define    EV_UNITSWITCHSTATUS  0x06
#define    EV_UNITSENSSTATUS  0x07
#define    EV_UNITCONFIG  0x08
#define    EV_UNITNAME  0x09
#define    EV_NODENAME  0x0a
#define    EV_BINDINGRESPONSE  0x0b
#define    EV_NODECONFIG  0x0c
#define    EV_NODERESOURCES  0x0d
#define    EV_NODESTATUS  0x0e
#define    EV_NODEPNPRESPONSE  0x0f
#define    EV_NODEBINDINGENTRY  0x10
#define    EV_MESSAGEERROR  0x11
#define    EV_INIT  0x12
#define    EV_NODEBINDINGSFILECONTROL  0x13
#define    EV_UNITSELECTOR  0x14
#define    EV_NODEEEPROMERASED  0x15
#define    EV_NODETEXTSTRING 	 0x16			/* 20100723 NEW ! */
#define    EV_UNITAUDIOSTATUS  0x17
#define    EV_NODEDLLSTATUS  0x18
#define    EV_NODELCDSTATUS  0x19
#define    EV_UNITDIRECTSET  0x1a
#define    EV_NODEDIRECTSET  0x1b
#define    EV_SLAVELCDBROWSINGDONE  0x1c
#define    EV_NODELCDMASTERSTATUS  0x1d
//#define    EV_NODEMANAGEIRCODES  0x1e
#define	   EV_DATABASEMANAGMENT  		0x1e	
#define    EV_NODEIRCODEDATABASECHANGED  0x1f
#define    EV_NODEAVIRDEVICESDATABASECHANGED  0x20
#define    EV_NODEAVIRDEVICESENDIRCODE  0x21
#define    EV_UNITRC5IRRECEIVERSTATUS  0x22

/* 14082000 speechcomm & videoswitch -- first use! */
#define    EV_NODEAVIRMATRIXSTATUS			0x23
#define	   EV_FTPFILECONTROLDATA			0x24			/* TM20100409 */
#define	   EV_FTPFILECONTROLSTATUS			0x25			/* TM20100317 */
#define    EV_UNITDUOSWITCHSTATUS  0x26
#define    EV_NODEPHYSICALADDRESS 0x27      /* 08032000 */
#define    EV_PNPLOGICALADDRESSASSIGNED 0x28      /* 08032000 */

#define    EV_LCDPMDBASEADDNODEENTRY 0x29 /* 20032000 */
#define    EV_LCDPMDBASEADDUNITENTRY 0x2A /* 20032000 */

#define    EV_LCDINFODIALOG 0x2B /* 22032000 */

#define    EV_IRCODEDATA 0x2C /* 10042000 */

#define    EV_UNITIRTXSTATUS  0x2D /* 11042000 */
#define    EV_RC5CODEMATCH 0x2E /* 11042000 */

#define    EV_IRCODEDATABASEERROR 0x2F /* 21042000 */

#define    EV_UNITSELECTORSTATUS 0x30 /* 26062000 */

/* 14082000 speechcomm & videoswitch */
#define    EV_UNITVIDEOMUXSTATUS 0x31  /* 06042001 */
#define    EV_UNITPS2INTELLIMOUSESTATUS 0x32  
#define    EV_UNITSPEECHCOMMSTATUS 0x33

#define    EV_LCDBUILDSCREENMAP 0x34 /* 13102000 */

#define    EV_EXTERNALCONFIG 0x35 /* 06042001 */
#define    EV_UNITAVMATRIXSTATUS 0x36 /* 06012001 */

// TM 03102001 Alarm messages - Events : EV_UNITALARMSTATUS
#define    EV_UNITALARMSTATUS  0x37

#define	   EV_UNITENERGYMEASUREMENTSTATUS	0x3A			// IMPLEMENT_UNIT_ENERGY

#define	   EV_NODEGATEWAYSTATUS		0x7B	/* 20060905 */
#define    EV_MEMORYDATADUMP  0x7C /* 28032000 */
#define    EV_WRITEMEMORYDATADONE  0x7D /* 28032000 */

#define    EV_NODEEEPROMREAD  0x7E
#define    EV_NODEEEPROMWRITTEN  0x7F
#define    FC_NODEMANAGEAVIRDEVICES  0x80
#define	   FC_NODETEXTSTRING		0x81		/* 20100723 */
#define    FC_NODEGETTEXTSTRING		0x82		/* 20100723 */
#define    FC_UNITREQUESTDIMSTATUS  0x83
#define    FC_UNITREQUESTCONFIG  0x84
#define    FC_UNITREQUESTSWITCHSTATUS  0x86
#define    FC_UNITREQUESTCONTROLSTATUS  0x87
#define    FC_UNITSENSSET  0x88
#define    FC_UNITREQUESTSENSSTATUS  0x89
#define    FC_NODEREQUESTCONFIG  0x8a
#define    FC_UNITSETNAME  0x8b
#define    FC_NODESETNAME  0x8c
#define    FC_UNITREQUESTNAME  0x8d
#define    FC_NODEREQUESTNAME  0x8e
#define    FC_NODEREQUESTSTATUS  0x8f
#define    FC_BROWSEFORNODES  0x90
#define    FC_BROWSEFORNEWNODES  0x91
#define    FC_UNITCONFIG  0x93
#define    FC_NODECONFIG  0x94
#define    FC_NODEREQUESTDLLSTATUS  0x95
#define    FC_NODESETMASTERLCDNODELA  0x96
#define    FC_NODEMASTERLCDSHUTDOWN  0x97
#define    FC_NODEBINDINGSFILECONTROL  0x98
#define    FC_NODEBINDINGENTRY  0x99
#define    FC_UNITRESET  0x9a
#define    FC_NODERESET  0x9b
#define    FC_NODELCDSHUTDOWN  0x9c
#define    FC_NODEGETRESOURCES  0x9d
#define    FC_SLAVELCDINITIATEBROWSING  0x9e
#define    FC_UNITAUDIOSET  0x9f
#define    FC_NODEERASEEEPROM  0xa0
#define    FC_NODELCDSET  0xa1 /* 12022001 */
#define    FC_UNITDIMSET  0xa2
#define    FC_UNITSWITCHSET  0xa3
#define    FC_UNITREQUESTLCDSTATUS  0xa4
#define    FC_UNITREQUESTAUDIOSTATUS  0xa5
#define    FC_UNITLCDVIRTUALSET  0xa6
#define    FC_UNITREQUESTLCDVIRTUALSTATUS  0xa7
#define    FC_UNITCONTROLSET  0xa8
#define    FC_NODELCDMASTERSET  0xa9
#define    FC_NODELCDMASTERTIMEDATE  0xaa
#define    FC_NODELCDMASTERREQUESTSTATUS  0xab
#define    FC_UNITFEEDBACKCONFIG  0xac

#define    FC_UNITIRTXSET  0xad
#define	   FC_FTPFILECONTROL					0xae	/* TM20100317 */
#define	   FC_REQUESTFTPFILECONTROL			0xaf	/* TM20100317 */	

/* 14082000 speechcomm & videoswitch -- first use */
#define    FC_NODEAVIRCONNECTIONMATRIXSET  0xb0
#define    FC_NODEREQUESTAVIRMATRIXSTATUS  0xb1

#define    FC_NODEAVIRDEVICESENDIRCODE  0xb2

#define    FC_UNITRC5IRRECEIVERSET  0xb3
#define    FC_UNITREQUESTRC5IRRECEIVERSTATUS  0xb4

//#define    FC_NODEMANAGEIRCODES  0xb5
#define	   FC_DATABASEMANAGEMENT	0xb5
#define    FC_UNITDUOSWITCHSET		0xb6
#define    FC_UNITREQUESTDUOSWITCHSTATUS  0xb7
#define    FC_BROWSEFORPHYSICALNODES  0xb8    /* 08032000 */
#define    FC_PNPASSIGNLOGICALADDRESS        0xb9 /* 08032000 */   

#define    FC_LCDPMDBASEADDNODEENTRY 0xBA /* 20032000 */
#define    FC_LCDPMDBASEADDUNITENTRY 0xBB /* 20032000 */

#define    FC_LCDINFODIALOG 0xBC /* 22032000 */

#define    FC_PNPPHYSADRACK  0xBD /* 24032000 */

#define    FC_GETIRCODE 0xBE /* 10042000 */
#define    FC_UNITREQUESTIRTXSTATUS  0xBF /* 11042000 */

#define    FC_CHECKRC5CODE 0xC0 /* 10042000 */

/* 14082000 speechcomm & videoswitch */
#define    FC_UNITVIDEOMUXSET 0xC1   /* 06042001 */
#define    FC_UNITREQUESTVIDEOMUXSTATUS 0xC2 /* 06042001 */
#define    FC_UNITPS2INTELLIMOUSESET 0xC3
#define    FC_UNITREQUESTPS2INTELLIMOUSESTATUS 0xC4
#define    FC_UNITSPEECHCOMMSET 0xC5
#define    FC_UNITREQUESTSPEECHCOMMSTATUS 0xC6
#define    FC_NODEMASTERLCDTAPISET 0xC7

#define    FC_LCDBUILDSCREENMAP 0xC8 /* 13102000 */


#define    FC_EXTERNALCONFIG 0xC9 /* 06042001 */
#define    FC_UNITAVMATRIXSET 0xCA /* 06012001 */
#define    FC_UNITREQUESTAVMATRIXSTATUS 0xCB /* 06012001 */

// TM 03102001 ALarm messages FC_UNITALARMSET + FC_UNITREQUESTALARMSTATUS 
#define    FC_UNITALARMSET  0xCC
#define    FC_UNITREQUESTALARMSTATUS 0xCD
#define	   FC_UNITENERGYMEASURESET 				0xCE	// IMPLEMENT_UNIT_ENERGY
#define	   FC_UNITREQUESTENERGYMEASURESTATUS 	0xCF	// IMPLEMENT_UNIT_ENERGY

#define	   FC_NODEGATEWAYSET	0xF8			/* 20060905 */
#define	   FC_NODEGATEWAYREQUESTSTATUS 0xF9		/* 20060905 */

#define    FC_UNLOCKLCD  0xFA /* 29032000 */
#define    FC_LOCKLCD  0xFB /* 29032000 */

#define    FC_GETMEMORYDATA  0xFC /* 28032000 */
#define    FC_WRITEMEMORYDATA  0xFD /* 28032000 */

#define    FC_NODEEEPROMREAD  0xfe
#define    FC_NODEEEPROMWRITE  0xff




// ****************** Node Gateway ********************************************
/* TM 20060905 */
#define NODEGATEWAY_ATTR_STATE				0x01	
#define NODEGATEWAY_OBJECT_DATABASE_ERASE	0x02
#define NODEGATEWAY_OBJECT_NOTIFY_DATABASE	0x03		/* 0=Error,1=Done,2=start */

#define DATABASE_UPDATE_ERROR				0x00	
#define DATABASE_UPDATE_DONE				0x01
#define DATABASE_UPDATE_STARTED				0x02	



// ****************** General macros for MessageTransfer(...) ******************
#define OBJECT_MEMBER_NR 0
#define DATA_0      0
#define DATA_1      1
#define DATA_2      2
#define DATA_3      3
#define DATA_4      4
#define DATA_5      5

// ****************************** EV_MESSAGEERROR ******************************
// EV_MESSAGEERROR data
#define MESSAGE_ERROR_REFUSED_RUNNING_STATE    0x00
#define MESSAGE_ERROR_INVALID                  0x01
#define MESSAGE_ERROR_BUFFER_FULL              0x02
#define MESSAGE_ERROR_BINDING_NOT_FOUND        0x03
#define MESSAGE_ERROR_REFERENCE_NOT_FOUND      0x04
#define MESSAGE_ERROR_BINDING_TYPE_INVALID     0x05
#define MESSAGE_ERROR_MESSAGE_TRANSFER_TIMEOUT 0x06                                  
#define MESSAGE_ERROR_DLL_ACK_TIMEOUT          0x07
#define MESSAGE_ERROR_TRANCEIVER_CHIP          0x08
#define MESSAGE_ERROR_UNIT_NR_INVALID          0x09                        
#define MESSAGE_ERROR_BINDING_CONFIG_REFUSED   0x0A                        
#define MESSAGE_ERROR_WRONG_OBJECT_MEMBER      0x0B
#define MESSAGE_ERROR_INVALID_MESSAGE_CODE     0x0C 
#define MESSAGE_ERROR_WRONG_DIRECT_CALL        0x0D
#define MESSAGE_ERROR_REFUSED_WRONG_DATA       0x0E
#define MESSAGE_ERROR_REFUSED_BINDING_STATE    0x0F
#define MESSAGE_ERROR_CAN_RX_PROTOCOL_TIMEOUT  0x10
#define MESSAGE_ERROR_CAN_TX_PROTOCOL_TIMEOUT  0x11
#define MESSAGE_ERROR_ONLY_FOR_MASTER_LCD      0x12
#define MESSAGE_ERROR_NO_FREE_MTC_MAX_RETRIES  0x13
#define MESSAGE_ERROR_BINDING_SYNTAX_ERROR           0x14  /* 05092000 */
#define MESSAGE_ERROR_BINDING_MAX_CONDITIONS         0x15  /* 05092000 */
#define MESSAGE_ERROR_BINDING_MAX_TIMEROPERATORS     0x16 /* 05092000 */
#define MESSAGE_ERROR_BINDING_MAX_ENTRIES            0x17 /* 05092000 */                                   
#define MESSAGE_ERROR_BINDING_CONDITION_LENGTH       0x18 /* 05092000 */
#define MESSAGE_ERROR_BINDING_MAX_REFERENCES         0x19 /* 05092000 */
#define MESSAGE_ERROR_BINDING_BRACES_SYNTAX          0x1A /* 05092000 */
                                   
// EV_MESSAGEERROR index
#define MESSAGE_ERROR_NR                   0
#define MESSAGE_ERROR_NODESTATUS           1                    
                                 
// ****************************** EV_NODERESOURCES *****************************
// indexes
#define RESOURCE_BLOCK_NR					0  

#define RESOURCES_0							0
#define RESOURCES_0_LENGTH					5
#define RESOURCE_NR_OF_REFERENCES_LSB		1
#define RESOURCE_NR_OF_TIMER_OPERATORS		2
#define RESOURCE_MAX_PACKET_DATA_LENGTH		3
#define RESOURCE_NR_OF_UNITS				4

#define RESOURCES_1							1
#define RESOURCES_1_LENGTH					6
#define RESOURCE_NR_OF_BINDING_ENTRIES_LSB	1
#define RESOURCE_BINDING_BUFFER_LENGTH_MSB  2
#define RESOURCE_BINDING_BUFFER_LENGTH_LSB  3
#define RESOURCE_MAX_CONDITIONS_LSB         4
#define RESOURCE_MAX_CONDITION_LENGTH		5                                                      
           
#define RESOURCES_2							2
#define RESOURCES_2_LENGTH					4		// Sedert versie 0x0700, was 4
#define RESOURCE_NR_OBJECT_NAME_LENGTH		1
#define RESOURCE_NR_SOFTWARE_VERSION		2                                  
#define RESOURCE_NR_MEDIUM_TYPE				3    
#define RESOURCE_NR_EXTRA_BLOCKS			4		// Sedert versie 0x0700

// Sedert versie 0x0700: Extra resource blocks.
#define RESOURCES_3							3
#define RESOURCES_3_LENGTH					6
#define RESOURCE_PRODUCT_ID					1
#define RESOURCE_PRODUCT_FLAGS				2
#define RESOURCE_SOFTWARE_VERSION_REV		3
#define RESOURCE_PROTOCOL_VERSION			4
#define RESOURCE_PROTOCOL_VERSION_REV		5

// Sedert versie V17.00: Uitbreiding van het aantal bindings.
// Resources block 4 bevat MSB waarden ivm het aantal bindings.
#define RESOURCES_4							4
#define RESOURCES_4_LENGTH					6
#define RESOURCE_NR_OF_BINDING_ENTRIES_MSB	1
#define RESOURCE_MAX_CONDITIONS_MSB         2
#define RESOURCE_NR_OF_REFERENCES_MSB		3
// 4 = reserve.
// 5 = reserve



// ****************************** EV_UNITCONTROLxxx *****************************                                  
// ****************************** EV_BINDINGRESPONSE ****************************                                  

 
// indexes pbData 

// EV_UNITSELECTOR
// EV_CONTROLxxx 
#define BINDING_NR_MSB     0
#define BINDING_NR_LSB     1
#define CONTROL_EVENT_DATA 2 

#define SELECTOR_MEMBER_NR 2
#define SELECTOR_MSB       3
#define SELECTOR_LSB       4
#define SELECTOR_PULS   0x02 // bit 1 when EV_UNITSELECTOR member NR == DIMMER_OBJECT_USE_SELECTOR_DATA
                
// EV_dimstatus                 
#define DIM_STATUS1 0
#define DIM_STATUS2 1
#define DIM_VALUE   2
               
// EV_BINDINGRESPONSE                
#define BINDINGRESPONSE_BINDING_NR_MSB 0
#define BINDINGRESPONSE_BINDING_NR_LSB 1
#define BINDINGRESPONSE_RESULT 2
                   
// EV_switchstatus                    
#define SWITCH_STATUS1 0
#define SWITCH_STATUS2 1
                                  

// ****************************** EV_NODEBINDINGSFILECONTROL ****************************
// ****************************** FC_NODEBINDINGSFILECONTROL ****************************
// ****************************** EV_NODEBINDINGENTRY ****************************
// ****************************** FC_NODEBINDINGENTRY ****************************

#define BINDING_CONFIG_COMMMAND 0
#define BINDING_CONFIG_EVENT    0
#define BINDING_CONFIG_DATA_1   1
#define BINDING_CONFIG_NR_OF_ENTRIES 1
#define BINDING_CONFIG_ENTRY_NR 1 
#define BINDING_CONFIG_DATA_2   2
#define BINDING_CONFIG_CHECKSUM 2
#define BINDING_CONFIG_DATA_3   3
#define BINDING_CONFIG_DATA_4   4
#define BINDING_CONFIG_DATA_5   5

// EV_NODEBINDINGENTRY events...
#define HEADER_READ              0x00
#define HEADER_WRITTEN           0x01
#define DATA_READ                0x02 
#define DATA_WRITTEN             0x03
#define ENTRY_CLEARED            0x04
#define WRITE_DONE               0x05
#define WR_CANCEL                0x06
#define DONE_READ                0x07
#define WRONG_BINDING_ENTRY_NR   0x80
#define WRITE_ERROR_WRONG_STATE  0x81
#define READ_ERROR_WRONG_STATE   0x82 
#define BINDING_FILE_FULL        0x83
#define BINDING_ENTRY_ERROR      0x84
#define BINDING_FILE_CLOSED      0x85
#define BINDING_FILE_EMPTY       0x86
#define BINDING_FILE_CORRUPT     0x87
#define BINDING_CONFIG_WRONG_STATE 0x88
#define BINDING_TEMP_BUFFER_FULL 0x89

// EV_NODEBINDINGSFILECONTROL events...
#define FILE_CLOSED              0x00
#define FILE_OPENED              0x01
#define FILE_INFO                0x02
#define FILE_ERASED              0x03
#define CLOSE_ERROR              0x80 
#define OPEN_ERROR               0x81
#define SHARE_VIOLATION          0x82
#define BINDING_TEMP_BUFFER_IN_USE 0x83
 
// FC_NODEBINDINGENTRY commands...
#define READ_HEADER              0x00
#define WRITE_HEADER             0x01
#define READ_DATA                0x02 
#define WRITE_DATA               0x03
#define CLEAR_ENTRY              0x04
#define WRITE_DONE               0x05
#define CANCEL_RW                0x06

// FC_NODEBINDINGSFILECONTROL commands...
#define FILE_CLOSE               0x00
#define FILE_OPEN                0x01
#define FILE_INFO                0x02
#define FILE_ERASE               0x03

// FC_NODEEEPROM read/write
#define EEPROM_ADDRESS_MSB       0x00
#define EEPROM_ADDRESS_LSB       0x01
#define EEPROM_WRITE_DATA_BYTE   0x02
#define EEPROM_READ_DATA_BYTE    0x00 

// After parsing event in BuildLocalXXXUnitEvent(...)
// After parsing event in CheckXXXObjectEvent(...)
#define EVENT_TOGGLE       0
#define EVENT_OFF          0
#define EVENT_ON           1 
#define EVENT_PULS         2
#define EVENT_UNKNOWN      3

//************************************ VIRTUAL OBJECT ****************************************
#define VIRTUAL_OBJECT_METHOD_PULS			0x02
#define VIRTUAL_OBJECT_METHOD_PULSTOGGLE	0x03
#define VIRTUAL_OBJECT_METHOD_TOGGLE		0x04

//************************************ DIMMER OBJECT *****************************************
#define DIMMER_OBJECT_USE_SELECTOR_DATA 0x00 
#define DIMMER_OBJECT_ATTR_SPEED        0x01
#define DIMMER_OBJECT_ATTR_RANGE        0x02
#define DIMMER_OBJECT_ATTR_VALUE        0x03
#define DIMMER_OBJECT_ATTR_ONOFF        0x04
#define DIMMER_OBJECT_METHOD_UP         0x05
#define DIMMER_OBJECT_METHOD_DN         0x06
#define DIMMER_OBJECT_METHOD_FSUD       0x07 
#define DIMMER_OBJECT_METHOD_FSUDONOFF  0x08
#define DIMMER_OBJECT_METHOD_OFF        0x09
#define DIMMER_OBJECT_METHOD_ONCV       0x0A 
#define DIMMER_OBJECT_METHOD_FSCVONOFF  0x0B
#define DIMMER_OBJECT_METHOD_FSCVOFF    0x0C
#define DIMMER_OBJECT_ATTR_STATE        0x0D
#define DIMMER_OBJECT_ATTR_CONFIG       0x0E
#define DIMMER_OBJECT_METHOD_INC		0x0F
#define DIMMER_OBJECT_METHOD_DEC		0x10
#define DIMMER_OBJECT_METHOD_ONUP		0x11
#define DIMMER_OBJECT_METHOD_ONDN		0x12
#define DIMMER_OBJECT_METHOD_PIRON		0x13
#define DIMMER_OBJECT_ATTR_DMX_OFFSETADDRESS	0x14,			/* Momenteel enkel voor DMX */
#define DIMMER_OBJECT_ATTR_CFG_ADDRESS			0x15			/* Momenteel enkel voor DALI */

 
// CV : current value
// FS : f(status)
// U  : up
// D  : down 

// Bindingresult data...
//#define RESULT_DATA_LENGTH    0     
/*
#define DIM_OBJECT_MEMBER_NR  1
#define DIM_OBJECT_DATA_MSB   2
#define DIM_OBJECT_DATA_LSB   3
*/
#define DIM_OBJECT_MEMBER_NR  0
#define DIM_OBJECT_DATA_MSB   1
#define DIM_OBJECT_DATA_LSB   2


/*********************************** FC_LCDLOCK ********************/
#define LCD_TSLOCKED 0x02
#define LCD_LOCKED   0x01
#define LCD_UNLOCKED 0x00


//************************************ SWITCH OBJECT *****************************************
#define SWITCH_OBJECT_USE_SELECTOR_DATA 0x00
#define SWITCH_OBJECT_ATTR_ONOFF        0x01
#define SWITCH_OBJECT_METHOD_OFF        0x02
#define SWITCH_OBJECT_METHOD_ON         0x03
#define SWITCH_OBJECT_METHOD_FSONOFF    0x04
#define SWITCH_OBJECT_ATTR_STATE        0x05
#define SWITCH_OBJECT_ATTR_CONFIG       0x06
#define SWITCH_OBJECT_METHOD_FSCVONOFFTIMEDPIR 0x07 
#define SWITCH_OBJECT_ATTR_STOPTIMER    0x08 // KV 13082001
 
// FS : f(status)

// Bindingresult data...
//#define RESULT_DATA_LENGTH       0     
/*
#define SWITCH_OBJECT_MEMBER_NR  1
#define SWITCH_OBJECT_DATA       2
*/ 
#define SWITCH_OBJECT_MEMBER_NR  0
#define SWITCH_OBJECT_DATA       1

//************************************ DUOSWITCH OBJECT *****************************************
#define DUOSWITCH_OBJECT_USE_SELECTOR_DATA 0x00
#define DUOSWITCH_OBJECT_ATTR_UPSTOP       0x01
#define DUOSWITCH_OBJECT_ATTR_DOWNSTOP     0x02
#define DUOSWITCH_OBJECT_METHOD_STOP       0x03
#define DUOSWITCH_OBJECT_METHOD_UP         0x04
#define DUOSWITCH_OBJECT_METHOD_DOWN       0x05
#define DUOSWITCH_OBJECT_METHOD_FSUPDOWN   0x06
#define DUOSWITCH_OBJECT_ATTR_STATE        0x07
#define DUOSWITCH_OBJECT_ATTR_CONFIG       0x08
#define DUOSWITCH_OBJECT_ATTR_STOPTIMER    0x09
#define DUOSWITCH_OBJECT_ATTR_SWITCHTIMER  0x0A
 
#define DUOSWITCH_OBJECT_MEMBER_NR  0
#define DUOSWITCH_OBJECT_DATA       1

// ************************************ Control object **************************************
// set object
#define CONTROL_OBJECT_CHECK      1
#define CONTROL_OBJECT_PULS       2
#define CONTROL_OBJECT_PULSTOGGLE 3
#define CONTROL_OBJECT_TOGGLE     4
#define CONTROL_OBJECT_STATUS     5                                                             

// CONTROL_OBJECT_CHECK value...                          
#define CHECK_OFF    0
#define CHECK_ON     1 
#define CHECK_ERROR  2
#define CHECK_PULS   3


/************************************* AUDIO OBJECT ******************************************/
#define AUDIO_OBJECT_MEMBER_NR           0
#define AUDIO_OBJECT_DATA       1

#define AUDIO_OBJECT_USE_SELECTOR_DATA   0
#define AUDIO_OBJECT_ATTR_SOURCE_ON      1  /* R/W data : bSourceNR 0..5 ( 6 sources ON/OFF ) */
#define AUDIO_OBJECT_METHOD_SOURCEINC    2
#define AUDIO_OBJECT_METHOD_SOURCEDEC    3
#define AUDIO_OBJECT_METHOD_CSON         4 
#define AUDIO_OBJECT_METHOD_OFF          5
#define AUDIO_OBJECT_ATTR_ONOFF          6 /* R/W data : 0=OFF, 1=ON */ 
#define AUDIO_OBJECT_METHOD_FSCSONOFF    7 
#define AUDIO_OBJECT_METHOD_VOLUP        8
#define AUDIO_OBJECT_METHOD_VOLDOWN      9
#define AUDIO_OBJECT_METHOD_FSVOLUPDOWN 10
#define AUDIO_OBJECT_METHOD_PRESUP      11 
#define AUDIO_OBJECT_METHOD_PRESDOWN    12
#define AUDIO_OBJECT_ATTR_CONFIG        13 
#define AUDIO_OBJECT_METHOD_SOURCE_FUNCTION    14 /*  data : bFunctionNR 0..n */ 
#define AUDIO_OBJECT_METHOD_DESTINATION_FUNCTION    15 /*  data : bFunctionNR 0..n */ 
#define AUDIO_OBJECT_ATTR_STATE         16 /*  R/W data : state */ 
#define AUDIO_OBJECT_ATTR_DISC          17 /*  R data : 1 byte(bit7..4 discnr, bit3..0 tracknr) */                       
#define AUDIO_OBJECT_ATTR_TUNERFREQ     18 /*  R data : 3 bytes (byte1 per 1Mc, byte2 per 50Kc, byte 3 preset NR , FF = manual) */
/* 31102000 */
#define AUDIO_OBJECT_ATTR_VOLUME        19 /*  W/R data : 1 byte bCurrentVolume */



#define AUDIO_MATRIX_OBJECT_USE_SELECTOR_DATA   0
#define AUDIO_MATRIX_OBJECT_ATTR_SOURCE_ON      1 /* W data bDestinationNR,, R/W data : bAudioSrcNR 0..7  or 0xff (NOT_ASSIGNED) */ 
#define AUDIO_MATRIX_OBJECT_METHOD_SOURCEINC    2 /* W data bDestinationNR */ 
#define AUDIO_MATRIX_OBJECT_METHOD_SOURCEDEC    3 /* W data bDestinationNR*/ 
#define AUDIO_MATRIX_OBJECT_METHOD_CSON         4 /* W data bDestinationNR */ 
#define AUDIO_MATRIX_OBJECT_METHOD_OFF          5 /* W data bDestinationNR*/ 
#define AUDIO_MATRIX_OBJECT_ATTR_ONOFF          6 /* W data bDestinationNR, R/W data : bit0..3 = dest 0..3 on/off (1/0) */  
#define AUDIO_MATRIX_OBJECT_METHOD_FSCSONOFF    7 /* W data bDestinationNR*/ 
#define AUDIO_MATRIX_OBJECT_METHOD_VOLUP        8 /* W data bDestinationNR*/
#define AUDIO_MATRIX_OBJECT_METHOD_VOLDOWN      9 /* W data bDestinationNR*/
#define AUDIO_MATRIX_OBJECT_METHOD_VOLSTOP     10 /* W data bDestinationNR*/
#define AUDIO_MATRIX_OBJECT_METHOD_FSVOLUPDOWN 11 /* W data bDestinationNR*/
#define AUDIO_MATRIX_OBJECT_ATTR_VOLUME        12 /* W data bDestinationNR R/W bVolume*/
#define AUDIO_MATRIX_OBJECT_ATTR_CONFIG        13  
#define AUDIO_MATRIX_OBJECT_ATTR_STATE         14 /*  R/W data : state */ 
#define AUDIO_MATRIX_OBJECT_METHOD_PLAY        15 /* W data bDestinationNR*/
#define AUDIO_MATRIX_OBJECT_METHOD_STOP        16 /* W data bDestinationNR*/
#define AUDIO_MATRIX_OBJECT_METHOD_PAUSE       17 /* W data bDestinationNR*/
#define AUDIO_MATRIX_OBJECT_METHOD_FF          18 /* W data bDestinationNR*/ 
#define AUDIO_MATRIX_OBJECT_METHOD_RR          19 /* W data bDestinationNR*/ 
#define AUDIO_MATRIX_OBJECT_METHOD_FFWD        20 /* W data bDestinationNR*/ 
#define AUDIO_MATRIX_OBJECT_METHOD_FRWD        21 /* W data bDestinationNR*/ 
#define AUDIO_MATRIX_OBJECT_ATTR_DEST_STATUS   22 /* R data 5 bytes : byte0:bCurDest, byte 1..4:dest 0..3
                                                     dest 0..3 bit 7 assigned/not assigned
                                                     dest 0..3 bit 6 on/off
                                                     dest 0..3 bit 5..0 source 0..63 */
#define AUDIO_MATRIX_OBJECT_ATTR_VOLUME_STATUS 23 /* R 4 bytes Dest0..3 bVolume */
#define AUDIO_MATRIX_OBJECT_METHOD_EXTRAIR     24	/* W data bDestinationNR* + W IRCode */ 
#define AUDIO_MATRIX_OBJECT_METHOD_MUTE_UNMUTE	25	/* W data bDestinationNR*/

// TM_AUDIOHC4_BINDING_INPUT
// Input events voor binding events.
// #define AUDIO_MATRIX_EVENT_RX_MACRO				0


// ================================================================
#define AUDIO_OBJECT_METHOD_EXTRAIR				24 
#define AUDIO_OBJECT_METHOD_MUTE_UNMUTE			25

#define BOSERS_OBJECT_USE_SELECTOR_DATA		0
#define BOSERS_OBJECT_ATTR_SOURCE_ON		1  				/* R/W data : bSourceNR 0..5 ( 6 sources ON/OFF ) */
#define BOSERS_OBJECT_METHOD_SOURCEINC		2
#define BOSERS_OBJECT_METHOD_SOURCEDEC		3
#define BOSERS_OBJECT_METHOD_CSON         4 
#define BOSERS_OBJECT_METHOD_OFF          5
#define BOSERS_OBJECT_ATTR_ONOFF          6 				/* R/W data : 0=OFF, 1=ON */ 
#define BOSERS_OBJECT_METHOD_FSCSONOFF    7 
#define BOSERS_OBJECT_METHOD_VOLUP        8
#define BOSERS_OBJECT_METHOD_VOLDOWN      9
#define BOSERS_OBJECT_METHOD_FSVOLUPDOWN 10
#define BOSERS_OBJECT_METHOD_PRESUP      11 
#define BOSERS_OBJECT_METHOD_PRESDOWN    12
#define BOSERS_OBJECT_ATTR_CONFIG        13 
#define BOSERS_OBJECT_METHOD_SOURCE_FUNCTION    	14 		/*  data : bFunctionNR 0..n */ 
#define BOSERS_OBJECT_METHOD_DESTINATION_FUNCTION   15 		/*  data : bFunctionNR 0..n */ 
#define BOSERS_OBJECT_ATTR_STATE         16 				/*  R data : bBoseRSStatus1 */                      
#define BOSERS_OBJECT_ATTR_DISC          17 				/*  R data : 1 byte(bit7..4 discnr, bit3..0 tracknr) */                       
#define BOSERS_OBJECT_ATTR_TUNERFREQ     18 				/*  R data : 3 bytes (byte1 per 1Mc, byte2 per 50Kc, byte 3 preset NR , FF = manual) */
#define BOSERS_OBJECT_ATTR_VOLUME        19 				/*  W/R data : 1 byte bCurrentVolume */
#define BOSERS_OBJECT_RESERVE0			 20		
#define BOSERS_OBJECT_RESERVE1					21
#define BOSERS_OBJECT_RESERVE2					22
#define BOSERS_OBJECT_METHOD_EXTRAIR			23			/* TM 20090707: V35 */
#define BOSERS_OBJECT_METHOD_REMOTE_EXTRAIR		24			/* TM 20050302: LF48 + V35 */
#define BOSERS_OBJECT_RESERVE3					25
#define BOSERS_OBJECT_RESERVE4					26
#define BOSERS_OBJECT_RESERVE5					27
#define BOSERS_OBJECT_RESERVE6					28
#define BOSERS_OBJECT_RESERVE7					29
#define BOSERS_OBJECT_RESERVE8					30
#define BOSERS_OBJECT_ATTR_ZONE_SOURCE_ON		31  		/* TM 20050302: LF48 + V35 */
#define BOSERS_OBJECT_METHOD_PLAY				32			/* TM 20100716 */
#define BOSERS_OBJECT_METHOD_STOP				33			/* TM 20100716 */
#define BOSERS_OBJECT_METHOD_PAUSE				34			/* TM 20100716 */
#define BOSERS_OBJECT_METHOD_FF					35			/* TM 20100716 */
#define BOSERS_OBJECT_METHOD_RR					36			/* TM 20100716 */
#define BOSERS_OBJECT_METHOD_FFWD				37			/* TM 20100716 */
#define BOSERS_OBJECT_METHOD_FRWD				38			/* TM 20100716 */


////////// BOSE LF 50 Events
#define BOSELF50_EVENT_SOURCEON				0	// All Rooms + individual Room
#define BOSELF50_EVENT_SOURCEOFF			1	// All Rooms + individual Room
#define BOSELF50_EVENT_BOSERS_DESTOFF		2	// individual Room
#define BOSELF50_EVENT_BOSERS_DESTON		3	// individual Room
#define BOSELF50_EVENT_PRESET_AUX			2	// All Rooms
#define BOSELF50_EVENT_TUNE_AUX				3	// All Rooms
#define BOSELF50_EVENT_PRESET_VIDEO1		4	// All Rooms
#define BOSELF50_EVENT_TUNE_VIDEO1			5	// All Rooms
#define BOSELF50_EVENT_PRESET_VIDEO2		6	// All Rooms
#define BOSELF50_EVENT_TUNE_VIDEO2			7	// All Rooms
#define BOSELF50_EVENT_PRESET_TAPE			8	// All Rooms
#define BOSELF50_EVENT_TUNE_TAPE			9	// All Rooms
#define BOSELF50_EVENT_TRANSPORT_AUX		10	// All Rooms
#define BOSELF50_EVENT_TRANSPORT_VIDEO1		11	// All Rooms
#define BOSELF50_EVENT_TRANSPORT_VIDEO2		12	// All Rooms
#define BOSELF50_EVENT_TRANSPORT_TAPE		13	// All Rooms


///////// BOSE LF48	Events
#define BOSELF48_EVENT_DESTOFF				0
#define BOSELF48_EVENT_DESTON				1
#define BOSELF48_EVENT_SOURCEOFF			2
#define BOSELF48_EVENT_SOURCEON				3
#define BOSELF48_EVENT_KEYPAD				4
#define BOSELF48_EVENT_TPKEYS				5
#define BOSELF48_EVENT_INPUTOFF				6
#define BOSELF48_EVENT_INPUTON				7


///////// BOSE V35 Events
#define BOSEV35_EVENT_DESTOFF				0
#define BOSEV35_EVENT_DESTON				1
#define BOSEV35_EVENT_SOURCEOFF				2
#define BOSEV35_EVENT_SOURCEON				3
#define BOSEV35_EVENT_KEYPAD				4
#define BOSEV35_EVENT_EXTRA					5


//************************************ SENSOR OBJECT *****************************************

#ifdef SKIP
	#define SENS_OBJECT_USE_SELECTOR_DATA       0x00
	#define SENS_OBJECT_ATTR_WORKING_SET_POINT  0x01
	#define SENS_OBJECT_ATTR_HYST_VALUES        0x02
	#define SENS_OBJECT_ATTR_CONTROL_ONOFF      0x03
	#define SENS_OBJECT_METHOD_FSINC_DEC        0x04
	#define SENS_OBJECT_METHOD_INC              0x05
	#define SENS_OBJECT_METHOD_DEC              0x06
	#define SENS_OBJECT_ATTR_SAMPLE_INTERVAL    0x07
	#define SENS_OBJECT_ATTR_CONFIG             0x08
	#define SENS_OBJECT_METHOD_FSONOFF          0x09
	#define SENS_OBJECT_ATTR_CURRENT_MEASURE    0x0A
	#define SENS_OBJECT_ATTR_STATE              0x0B
	#define SENS_OBJECT_ATTR_HYST_OFFSETS       0x0C
	#define SENS_OBJECT_ATTR_PRESET             0x0D
	#define SENS_OBJECT_ATTR_MEASUREDOFFSET		0x0E	/* TM20120403 */
	#define SENS_OBJECT_ATTR_NTC_VERSION		0x0F	/* TM20120403 */
	#define SENS_OBJECT_ATTR_WORKINGMODE		0x10	/* TM20120917 */ /* SensUnitDaikin::WorkingModes */
	#define SENS_OBJECT_ATTR_FANSPEED           0x11	/* TM20120917 */ /* SensUnitDaikin::FanSpeed */
	#define SENS_OBJECT_ATTR_SWINGANGLE         0x12	/* TM20120917 */ /* UShort - Degrees */
	#define SENS_OBJECT_ATTR_SWINGMODE          0x13	/* TM20120917 */ /* 0=Off, 1=on */
#endif


enum
{
	SENS_OBJECT_USE_SELECTOR_DATA       =0x00,
	SENS_OBJECT_ATTR_WORKING_SET_POINT  =0x01,
	SENS_OBJECT_ATTR_HYST_VALUES        =0x02,
	SENS_OBJECT_ATTR_CONTROL_ONOFF      =0x03,
	SENS_OBJECT_METHOD_FSINC_DEC        =0x04,
	SENS_OBJECT_METHOD_INC              =0x05,
	SENS_OBJECT_METHOD_DEC              =0x06,
	SENS_OBJECT_ATTR_SAMPLE_INTERVAL    =0x07,
	SENS_OBJECT_ATTR_CONFIG             =0x08,
	SENS_OBJECT_METHOD_FSONOFF          =0x09,
	SENS_OBJECT_ATTR_CURRENT_MEASURE    =0x0A,
	SENS_OBJECT_ATTR_STATE              =0x0B,
	SENS_OBJECT_ATTR_HYST_OFFSETS       =0x0C,
	SENS_OBJECT_ATTR_PRESET             =0x0D,
	SENS_OBJECT_ATTR_MEASUREDOFFSET	    =0x0E,
	SENS_OBJECT_ATTR_NTC_VERSION	    =0x0F,
	SENS_OBJECT_ATTR_WORKING_MODE		=0x10,
	SENS_OBJECT_ATTR_FAN_SPEED			=0x11,
	SENS_OBJECT_ATTR_SWING_ANGLE		=0x12,
	SENS_OBJECT_ATTR_SWING_MODE			=0x13,
	SENS_OBJECT_ATTR_DELEGATE			=0x14,	// sedert versie 0x0900
	SENS_OBJECT_ATTR_DELEGATOR			=0x15,	// sedert versie 0x0900
	SENS_OBJECT_ATTR_CFG_MEASURESENSOR  = 0x16,	// sedert versie 0x0C02
	SENS_OBJECT_ATTR_SP_RANGE			=0x17,	
	SENS_OBJECT_ATTR_PIDCONTROLUNIT		=0x18,	// zie USE_SENS_CONTROL_PID
	SENS_OBJECT_ATTR_PIDCONTROLPARAMS	=0x19,	// zie USE_SENS_CONTROL_PID
};
 
#define EV_HEATING							0
#define EV_COOLING							1

// FS : f(status)

// Bindingresult data...
//#define RESULT_DATA_LENGTH       0     
/*
#define SENS_OBJECT_MEMBER_NR    1
#define SENS_OBJECT_DATA_MSB     2
#define SENS_OBJECT_DATA_LSB     3
*/
#define SENS_OBJECT_MEMBER_NR    0
#define SENS_OBJECT_DATA_MSB     1
#define SENS_OBJECT_DATA_LSB     2
#define SENS_OBJECT_PRESET       3

//************************************ LCD OBJECT *****************************************
#define LCD_OBJECT_USE_SELECTOR_DATA         0x00
#define LCD_OBJECT_METHOD_PULSTOGGLE_1       0x01
#define LCD_OBJECT_METHOD_PULSTOGGLE_2       0x02
#define LCD_OBJECT_METHOD_PULSTOGGLE_3       0x03
#define LCD_OBJECT_METHOD_PULSTOGGLE_4       0x04
#define LCD_OBJECT_METHOD_PULSTOGGLE_5       0x05
#define LCD_OBJECT_METHOD_PULSTOGGLE_6       0x06
#define LCD_OBJECT_METHOD_PULSTOGGLE_7       0x07
#define LCD_OBJECT_METHOD_TOGGLE_1           0x08
#define LCD_OBJECT_METHOD_TOGGLE_2           0x09
#define LCD_OBJECT_METHOD_TOGGLE_3           0x0A
#define LCD_OBJECT_METHOD_TOGGLE_4           0x0B
#define LCD_OBJECT_METHOD_TOGGLE_5           0x0C
#define LCD_OBJECT_METHOD_TOGGLE_6           0x0D
#define LCD_OBJECT_METHOD_TOGGLE_7           0x0E
                                       
// Bindingresult data...
//#define RESULT_DATA_LENGTH       0     
//#define LCD_OBJECT_MEMBER_NR     1
#define LCD_OBJECT_MEMBER_NR     0 
#define LCD_DATA                 1


/************************************* IRTX OBJECT ******************************************/
/* 10042000 */
#define IRTX_OBJECT_MEMBER_NR           0
#define IRTX_OBJECT_DATA       1

#define IRTX_OBJECT_USE_SELECTOR_DATA   0
#define IRTX_OBJECT_METHOD_START_IRTX   1  /* W data (3bytes) = MSB-LSB IRCodeIndex  bChannel */
#define IRTX_OBJECT_METHOD_STOP_IRTX    2
#define IRTX_OBJECT_METHOD_PULS_IRTX    3  /* W data (3bytes) = MSB-LSB IRCodeIndex ! bChannel */
#define IRTX_OBJECT_ATTR_CONFIG         4 
#define IRTX_OBJECT_ATTR_STATE          5 /*  R/W data : state */ 


/************************************* RC5RX OBJECT ******************************************/
// TM 20041115
#define RC5RX_EVENT_PULS					3		//TM 200406010 was 0
#define RC5RX_EVENT_PULSTOGGLE				2 //1
#define RC5RX_EVENT_TOGGLE					1 //2

/* 10042000 */
#define RC5RX_OBJECT_MEMBER_NR           0
#define RC5RX_OBJECT_DATA                1
#define RC5RX_EVENT_DATA                 2 

#define RC5RX_OBJECT_USE_SELECTOR_DATA      0
#define RC5RX_OBJECT_METHOD_INITIATE_CODE   1  /* W data (1 byte) : RC5CodeIndex Event 0..63 */
#define RC5RX_OBJECT_ATTR_CONFIG            2 /*  R/W data : config */ 
#define RC5RX_OBJECT_ATTR_STATE             3 /*  R data : state */ 



/* 06042001 */
/************************* VIDEOMUX UNIT ************************************************/
#define VIDEOMUX_OBJECT_MEMBER_NR           0
#define VIDEOMUX_OBJECT_DATA_1              1
#define VIDEOMUX_OBJECT_DATA_2              2 

#define VIDEOMUX_OBJECT_USE_SELECTOR_DATA     0
#define VIDEOMUX_OBJECT_ATTR_CONNECTION       1 /* W BYTE bOutput, W BYTE bInput, R/W BYTE 
										           bState 0/1 */
#define VIDEOMUX_OBJECT_METHOD_CONNECT        2 /* W BYTE bOutput, W BYTE bInput */
#define VIDEOMUX_OBJECT_METHOD_DISCONNECT     3 /* W BYTE bOutput, W BYTE bInput */
#define VIDEOMUX_OBJECT_METHOD_DISCONNECT_ALL 4
#define VIDEOMUX_OBJECT_ATTR_STATE            5  /* R 2 bytes */
#define VIDEOMUX_OBJECT_ATTR_CONFIG           6  /* R/W 1 byte */ 
#define VIDEOMUX_OBJECT_ATTR_INPUTS           7  /* R 2 bytes = 16 videomux inputs */
#define VIDEOMUX_OBJECT_ATTR_OUTPUTS          8  /* R 4 bytes = max 4 videomux outputs */

/* Pan/tilt, camera selection, ... should follow here... */
#define VIDEOMUX_OBJECT_METHOD_COMMUNICATION    9   // 1 byte communication mode // TM 20/06/01
#define VIDEOMUX_OBJECT_METHOD_KEYPRESS         0x0A  // 1 byte key code // TM
#define VIDEOMUX_OBJECT_METHOD_KEYPRESS_CAMERA  0x0B  // 1 byte key code // TM 03/07/01
#define VIDEOMUX_OBJECT_METHOD_DVMR             0x0C  // 1 byte videorecorder
/*******************************************************************************************************/


/************************* AVMATRIX UNIT OBJECT ************************************************/
#define AVMATRIX_OBJECT_MEMBER_NR           0
#define AVMATRIX_OBJECT_DATA_1              1
#define AVMATRIX_OBJECT_DATA_2              2 
#define AVMATRIX_OBJECT_DATA_3              3 

#define AVMATRIX_OBJECT_USE_SELECTOR_DATA  0
#define AVMATRIX_OBJECT_ATTR_CONNECTION 1 /* W BYTE bOutput, W BYTE bInput, R/W BYTE 
						             	     bState 0/1 */
#define AVMATRIX_OBJECT_METHOD_CONNECT 2 /* W BYTE bOutput, W BYTE bInput */
#define AVMATRIX_OBJECT_METHOD_DISCONNECT 3 /* W BYTE bOutput, W BYTE bInput */
#define AVMATRIX_OBJECT_ATTR_VOLUME 4 /* W BYTE bOutput, R/W BYTE bVolume */
#define AVMATRIX_OBJECT_ATTR_BASS 5 /* W BYTE bOutput, R/W BYTE bBass */
#define AVMATRIX_OBJECT_ATTR_TREBLE 6 /* W BYTE bOutput, R/W BYTE bTreble */
#define AVMATRIX_OBJECT_METHOD_VOLUME_INC 7 /* W BYTE bOutput */
#define AVMATRIX_OBJECT_METHOD_VOLUME_DEC 8 /* W BYTE bOutput */
#define AVMATRIX_OBJECT_METHOD_DISCONNECT_ALL 9
#define AVMATRIX_OBJECT_METHOD_VOLUME_UP 10 /* W BYTE bOutput */
#define AVMATRIX_OBJECT_METHOD_VOLUME_DOWN 11 /* W BYTE bOutput */
#define AVMATRIX_OBJECT_METHOD_VOLUME_STOP 12 /* W BYTE bOutput */
#define AVMATRIX_OBJECT_ATTR_STATE 13
#define AVMATRIX_OBJECT_ATTR_CONFIG 14
#define AVMATRIX_OBJECT_ATTR_CONFIG_NR_OF_DEST_ON_UNIT  15  /* R/W Runtime Config */
#define AVMATRIX_OBJECT_ATTR_CONFIG_NR_OF_UNITS         16  /* R/W Runtime Config */
#define AVMATRIX_OBJECT_METHOD_MUTE_UNMUTE              17  /* W BYTE bOutput */
#define AVMATRIX_OBJECT_METHOD_MACRO					18
#define AVMATRIX_OBJECT_METHOD_MACRO_RECEIVED			19
#define AVMATRIX_OBJECT_ATTR_ZONEGROUP_SET			20
#define AVMATRIX_OBJECT_ATTR_ZONEGROUP_SLAVEADD		21
#define AVMATRIX_OBJECT_ATTR_ZONEGROUP_SLAVEREMOVE	22
/*******************************************************************************************************/



/************************************* PS2INTELLIMOUSE UNIT OBJECT ******************************************/
#define PS2INTELLIMOUSE_OBJECT_MEMBER_NR           0
#define PS2INTELLIMOUSE_OBJECT_DATA                1
#define PS2INTELLIMOUSE_EVENT_DATA                 2 

#define PS2INTELLIMOUSE_OBJECT_USE_SELECTOR_DATA      0x00
#define PS2INTELLIMOUSE_OBJECT_ATTR_POS               0x01 /* R/W 4 bytes XMSB XLSB YMSB YLSB */ 
#define PS2INTELLIMOUSE_OBJECT_METHOD_LCLICK          0x02 
#define PS2INTELLIMOUSE_OBJECT_METHOD_LDOUBLECLICK    0x03 
#define PS2INTELLIMOUSE_OBJECT_METHOD_RCLICK          0x04 
#define PS2INTELLIMOUSE_OBJECT_METHOD_LBUTTONDOWN     0x05 
#define PS2INTELLIMOUSE_OBJECT_METHOD_RBUTTONDOWN     0x06 
#define PS2INTELLIMOUSE_OBJECT_METHOD_BUTTONRELEASE   0x07 
#define PS2INTELLIMOUSE_OBJECT_ATTR_SCREENSIZE        0x08 /* R/W 4 bytes XMSB XLSB YMSB YLSB */ 
#define PS2INTELLIMOUSE_OBJECT_ATTR_STATE             0x09  /*  R data : state */ 
#define PS2INTELLIMOUSE_OBJECT_ATTR_CONFIG            0x0A  /*  R/W data : config */ 
#define PS2INTELLIMOUSE_OBJECT_METHOD_INITPOSITION    0x0B
 
/* 06042001 */
/************************************* SPEECHCOMM UNIT OBJECT ******************************************/
#define SPEECHCOMM_OBJECT_MEMBER_NR           0
#define SPEECHCOMM_OBJECT_DATA_1              1
#define SPEECHCOMM_OBJECT_DATA_2              2 

#define SPEECHCOMM_OBJECT_USE_SELECTOR_DATA   0
#define SPEECHCOMM_OBJECT_METHOD_VOLUME_UP    1
#define SPEECHCOMM_OBJECT_METHOD_VOLUME_DOWN  2
#define SPEECHCOMM_OBJECT_METHOD_CALLREQUEST  3
#define SPEECHCOMM_OBJECT_METHOD_CONNECT      4 
#define SPEECHCOMM_OBJECT_METHOD_DISCONNECT   5
#define SPEECHCOMM_OBJECT_METHOD_DISCARD      6
#define SPEECHCOMM_OBJECT_METHOD_CALLINITIATE 7
#define SPEECHCOMM_OBJECT_METHOD_HANGUP       8
#define SPEECHCOMM_OBJECT_METHOD_ACCEPT       9
#define SPEECHCOMM_OBJECT_ATTR_CONFIG        10 /*  R/W data : config */ 
#define SPEECHCOMM_OBJECT_ATTR_STATE         11 /*  R data : state */ 
#define SPEECHCOMM_OBJECT_ATTR_REGISTER      12 /*  R/W 2 bytes : MSB, LSB */
#define SPEECHCOMM_OBJECT_ATTR_VOLUME        13 /* NOV R/W 1 byte, 128=0db */ 
/* 17042001 */
#define SPEECHCOMM_OBJECT_METHOD_REQUESTVIDEOSWITCH  14 /* W 1 byte : bVideoMuxSrc */ 



/* 14082000 speechcomm & videoswitch */              
/************************************* AVIR NODE MATRIX OBJECT ******************************************/
/* 
   FC_NODEAVIRCONNECTIONMATRIXSET
   Response EV_NodeDirectSet with ERROR or SUCCESS 
   If error, bErrorNR can be asked for with FC_NODEREQUESTAVIRMATRIXSTATUS NODE_AVIR_MATRIX_ERROR 

   FC_NODEREQUESTAVIRMATRIXSTATUS
   EV_NODEAVIRMATRIXSTATUS 
*/

#define NODE_AVIR_MATRIX_OBJECT_MEMBER_NR           0
#define NODE_AVIR_MATRIX_OBJECT_DATA                1
#define NODE_AVIR_MATRIX_EVENT_DATA                 2 

#define NODE_AVIR_MATRIX_METHOD_ROUTE 1 /* R/W 4 bytes,  bInputNR   bOutputMask2 bOutputMask1 bOutputMask0 */
#define NODE_AVIR_MATRIX_METHOD_OFF   2 /* W 1 byte bOutputNR  */
#define NODE_AVIR_MATRIX_ATTR_STATE   3 /* R */
#define NODE_AVIR_MATRIX_ERROR        4 /* R 1 byte eg. invalid matrix connection */



// 09112001 ALARM UNIT
/************************************** ALARM UNIT OBJECT ********************************************/
#define ALARM_OBJECT_MEMBER_NR           0
#define ALARM_OBJECT_DATA_1              1
#define ALARM_OBJECT_DATA_2              2 
#define ALARM_OBJECT_DATA_3              3 
#define ALARM_OBJECT_DATA_4              4 
#define ALARM_OBJECT_DATA_5              5

#define ALARM_OBJECT_USE_SELECTOR_DATA      0
#define ALARM_OBJECT_METHOD_KEYPAD          1   /* W BYTE LogicalAddress, W BYTE KeyCode*/
#define ALARM_OBJECT_METHOD_ARM             2   /* W BYTE bGroupNr, W BYTE Part/Full set*/
#define ALARM_OBJECT_ATTR_INFO              3   /*  R 1Byte  */
#define ALARM_OBJECT_ATTR_GROUPSTATUS       4   /*  R 4Bytes */
#define ALARM_OBJECT_ATTR_ALARMZONE         5   /*  R 2Bytes  */
#define ALARM_OBJECT_ATTR_OPENZONE          6   /*  R 1Byte  */
#define ALARM_OBJECT_ATTR_PREALARM_COUNTER  7   /*  R 1 Byte */
#define ALARM_OBJECT_ATTR_REMOTECODE        8   /*  R/W data : config */ 
#define ALARM_OBJECT_ATTR_CONFIG            9   /*  R/W data : config */ 
#define ALARM_OBJECT_METHOD_LOGON           10  
#define ALARM_OBJECT_ATTR_STATE             11   /*  R 2Bytes */ 

// INFO/ERROR MESSAGES
#define ALARM_PINCODE_OK              0x00
#define ALARM_WRONG_PINCODE           0x01
#define ALARM_PINCODE_TO_LONG         0x02
#define ALARM_WRONG_SEQUENCE_LENGHT   0x03
#define ALARM_NEW_PINCODE             0x04
#define ALARM_PINCODE_REJECTED        0x05
#define ALARM_SEQUENCE_OK             0x06
#define ALARM_ACCESS_DENIED           0x07
#define ALARM_NO_PINCODES             0x08
#define ALARM_REMOTECODE_WRITE_ERROR  0x09
#define ALARM_REMOTECODE_WRITE_OK     0x0A
#define ALARM_BURGLARY_STRING         0x0B
#define ALARM_ZONE_BUFFERS_FULL       0x0C
#define ALARM_BURGLARY_SEC_BEEP       0x0D
#define ALARM_NO_MORE_ZONES           0x0E
#define ALARM_REMOTECODE_WRONG        0x0F
// TM : 03102001 Alarm 


// TM_CHANGES_BINDINGS_ALARM: specifiek events voor bindings
#define ALARM_EVENT_ALARM_INPUT			0x00
#define ALARM_EVENT_ALARM_ARM			0x01
#define ALARM_EVENT_ALARM_BURGLARY		0x02


/* 06042001 */
/************************************* LCD MASTER NODE TAPI ******************************************/
/*
FC_NodeMasterLCDTAPISet 
*/
#define NODE_LCDMASTER_TAPI_OBJECT_MEMBER_NR           0
#define NODE_LCDMASTER_TAPI_OBJECT_DATA                1
#define NODE_LCDMASTER_TAPI_EVENT_DATA                 2 

#define NODE_LCDMASTER_TAPI_METHOD_CALL    1 /* W */
#define NODE_LCDMASTER_TAPI_METHOD_HANGUP  2 /* W */
#define NODE_LCDMASTER_TAPI_METHOD_ACCEPT  3 /* W */
#define NODE_LCDMASTER_TAPI_METHOD_DOOR    4 /* W */
#define NODE_LCDMASTER_TAPI_METHOD_DISCARD 5 /* W */
/* 27042001 */
#define NODE_LCDMASTER_TAPI_METHOD_REQUESTVIDEOSWITCH 6 /* W, 2 byte bVideoMuxSrc, bState 1/0 */
#define NODE_LCDMASTER_TAPI_METHOD_CALLEND 7 /* W */
#define NODE_LCDMASTER_TAPI_METHOD_CALLSTART 8 /* W */
#define NODE_LCDMASTER_TAPI_METHOD_CALLBUSY 9 /* W */
#define NODE_LCDMASTER_TAPI_METHOD_CALLDISCARD 10 /* W */


//*********************************** EV_UNITDIRECTSET ************************************
#define OBJECT_DIRECT_SET_OK 0
#define OBJECT_DIRECT_SET_ERROR 1


//********************************* FC_UNITSETNAME,FC_NODESETNAME *************************
//********************************* FC_UNITREQUESTNAME, FC_NODEREQUESTNAME **************** 
#define OBJECT_NAME_WRITE_OK    0
#define OBJECT_NAME_READ_OK     1
#define OBJECT_NAME_WRITE_ERROR 2
#define OBJECT_NAME_READ_ERROR  3

//******************************** EV_Init ************************************************

#define NODERESET 0x04
#define UNIT_SOFTWARE_RESET 0x05


#define SCREENMAP_INIT_SCREENMAP 0 /* FC_LCDBUILDSCREENMAP:		3 bytes, byte0:bScreenMapID (light, switch,mood,...), byte1:MSB usNROfEntries, byte2:LSB usNROfEntries */
#define SCREENMAP_INIT_OK 0			/* EV_LCDBUILDSCREENMAP */
#define SCREENMAP_INIT_ERROR 1		/* EV_LCDBUILDSCREENMAP */

#define SCREENMAP_ENTRY 1			/* FC_LCDBUILDSCREENMAP:	5 bytes, byte0:bScreenMapID (light, switch,mood,...), byte1/2:page/button, byte3:bNodeLogicalAddress, byte4:bUnitNR */
#define SCREENMAP_ENTRY_OK 2		/* EV_LCDBUILDSCREENMAP */
#define SCREENMAP_ENTRY_ERROR 3		/* EV_LCDBUILDSCREENMAP */
#define SCREENMAP_ENTRY_UNIT_NOT_EXISTING 4	/* EV_LCDBUILDSCREENMAP */
#define SCREENMAP_ENTRY_NODE_NOT_EXISTING 5	/* EV_LCDBUILDSCREENMAP */

#define SCREENMAP_DONE_SMAP	6			/* FC_LCDBUILDSCREENMAP:  Voor de nieuwe touchscreen. */
#define SCREENMAP_DONE_SMAP_OK	7		/* EV_LCDBUILDSCREENMAP - Voor de nieuwe touchscreen */ 
#define SCREENMAP_DONE_SMAP_ERROR 8		/* EV_LCDBUILDSCREENMAP - Voor de nieuwe touchscreen */	



//////////////////////////////////////////////////////////////////////////
#define AUDIO_CONFIG_SET_SOURCE					0x01  
#define AUDIO_CONFIG_SET_DESTINATION			0x02  
#define AUDIO_CONFIG_SET_ROOM					0x03  
#define AUDIO_CONFIG_SET_SOURCE_ERROR			0x04
#define AUDIO_CONFIG_SET_DESTINATION_ERROR		0x05
#define AUDIO_CONFIG_SET_ROOM_ERROR				0x06
#define AUDIO_CONFIG_SET_SOURCE_OK				0x07
#define AUDIO_CONFIG_SET_DESTINATION_OK			0x08
#define AUDIO_CONFIG_SET_ROOM_OK				0x09  
#define EXTERNAL_CONFIG_INITIALIZE				0x0A  
#define EXTERNAL_CONFIG_INITIALIZE_ERROR		0x0B  
#define EXTERNAL_CONFIG_INITIALIZE_OK			0x0C  
#define VIDEOMUX_CONFIG_SET						0x0D
#define AVMATRIX_CONFIG_SET						0x0E
#define VIDEOPHONE_CONFIG_SET					0x0F
#define VIDEOMUXSOURCE_CONFIG_SET				0x10
#define VIDEOMUXDESTINATION_CONFIG_SET			0x11
#define SPEECHDEVINDOOR_CONFIG_SET				0x12
#define SPEECHDEVOUTDOOR_CONFIG_SET				0x13
#define VIDEOMUX_CONFIG_SET_ERROR				0x14
#define AVMATRIX_CONFIG_SET_ERROR				0x15
#define VIDEOPHONE_CONFIG_SET_ERROR				0x16
#define VIDEOMUXSOURCE_CONFIG_SET_ERROR			0x17
#define VIDEOMUXDESTINATION_CONFIG_SET_ERROR	0x18
#define SPEECHDEVINDOOR_CONFIG_SET_ERROR		0x19 /*27042001*/
#define SPEECHDEVOUTDOOR_CONFIG_SET_ERROR		0x1A /*27042001*/
#define VIDEOMUX_CONFIG_SET_OK					0x1B
#define AVMATRIX_CONFIG_SET_OK					0x1C
#define VIDEOPHONE_CONFIG_SET_OK				0x1D
#define VIDEOMUXSOURCE_CONFIG_SET_OK			0x1E
#define VIDEOMUXDESTINATION_CONFIG_SET_OK		0x1F
#define SPEECHDEVINDOOR_CONFIG_SET_OK			0x20 /*27042001*/
#define SPEECHDEVOUTDOOR_CONFIG_SET_OK			0x21 /*27042001*/
#define ALARMZONE_CONFIG_SET					0x22
#define ALARMZONE_CONFIG_SET_OK					0x23
#define ALARMZONE_CONFIG_SET_ERROR				0x24

#define EXTERNAL_CONFIG_DONE					0x25 /* since v65.20 */
#define EXTERNAL_CONFIG_DONE_OK					0x26 /* since v65.20 */
#define EXTERNAL_CONFIG_DONE_ERROR				0x27 /* since v65.20 */
#define UNIT_CONFIG_INITIALIZE					0x28 /* since v65.20 */
#define	UNIT_CONFIG_INITIALIZE_OK				0x29 /* since v65.20 */
#define UNIT_CONFIG_INITIALIZE_ERROR			0x2a /* since v65.20 */
#define UNIT_CONFIG_SET							0x2b /* since v65.20 */
#define UNIT_CONFIG_SET_OK						0x2c /* since v65.20 */
#define UNIT_CONFIG_SET_ERROR					0x2d /* since v65.20 */
#define UNIT_CONFIG_DONE						0x2e /* since v65.20 */
#define UNIT_CONFIG_DONE_OK						0x2f /* since v65.20 */
#define UNIT_CONFIG_DONE_ERROR					0x30 /* since v65.20 */
#define MOOD_CONFIG_INITIALIZE					0x31 /* since v65.20 */
#define MOOD_CONFIG_INITIALIZE_OK				0x32 /* since v65.20 */
#define MOOD_CONFIG_INITIALIZE_ERROR			0x33 /* since v65.20 */
#define MOODHEADER_CONFIG_SET					0x34 /* since v65.20 */
#define MOODHEADER_CONFIG_SET_OK				0x35 /* since v65.20 */
#define MOODHEADER_CONFIG_SET_ERROR				0x36 /* since v65.20 */
#define MOODENTRY_CONFIG_SET					0x37 /* since v65.20 */
#define MOODENTRY_CONFIG_SET_OK					0x38 /* since v65.20 */
#define MOODENTRY_CONFIG_SET_ERROR				0x39 /* since v65.20 */
#define MOOD_CONFIG_DONE						0x3a /* since v65.20 */
#define MOOD_CONFIG_DONE_OK						0x3b /* since v65.20 */
#define MOOD_CONFIG_DONE_ERROR					0x3c /* since v65.20 */
#define IRRX_CONFIG_INITIALIZE					0x3d /* since v65.20 */
#define IRRX_CONFIG_INITIALIZE_OK				0x3e /* since v65.20 */
#define IRRX_CONFIG_INITIALIZE_ERROR			0x3f /* since v65.20 */
#define IRRX_CONFIG_SET							0x40 /* since v65.20 */
#define IRRX_CONFIG_SET_OK						0x41 /* since v65.20 */
#define IRRX_CONFIG_SET_ERROR					0x42 /* since v65.20 */
#define IRRX_CONFIG_DONE						0x43 /* since v65.20 */
#define IRRX_CONFIG_DONE_OK						0x44 /* since v65.20 */
#define IRRX_CONFIG_DONE_ERROR					0x45 /* since v65.20 */
#define SCHEDULE_CONFIG_INITIALIZE				0x46 /* since v65.20 */
#define SCHEDULE_CONFIG_INITIALIZE_OK			0x47 /* since v65.20 */
#define SCHEDULE_CONFIG_INITIALIZE_ERROR		0x48 /* since v65.20 */
#define SCHEDULEITEM_CONFIG_SET					0x49 /* since v65.20 */
#define SCHEDULEITEM_CONFIG_SET_OK				0x4a /* since v65.20 */
#define SCHEDULEITEM_CONFIG_SET_ERROR			0x4b /* since v65.20 */
#define SCHEDULEEVENT_CONFIG_SET				0x4c /* since v65.20 */
#define SCHEDULEEVENT_CONFIG_SET_OK				0x4d /* since v65.20 */
#define SCHEDULEEVENT_CONFIG_SET_ERROR			0x4e /* since v65.20 */
#define SCHEDULE_CONFIG_DONE					0x4f /* since v65.20 */
#define SCHEDULE_CONFIG_DONE_OK					0x50 /* since v65.20 */
#define SCHEDULE_CONFIG_DONE_ERROR				0x51 /* since v65.20 */

#define SMAP_CONFIG_INITIALIZE 					0x52 /* since v0571 */
#define SMAP_CONFIG_INITIALIZE_OK 				0x53 /* since v0571 */
#define SMAP_CONFIG_INITIALIZE_ERROR 			0x54 /* since v0571 */
#define SMAP_CONFIG_PAGEINFO_HEADER				0x55 /* since v0571 */
#define SMAP_CONFIG_PAGEINFO_HEADER_OK 			0x56 /* since v0571 */
#define SMAP_CONFIG_PAGEINFO_HEADER_ERROR 		0x57 /* since v0571 */
#define SMAP_CONFIG_PAGEINFO_DATA				0x58 /* since v0571 */
#define SMAP_CONFIG_PAGEINFO_DATA_OK 			0x59 /* since v0571 */
#define SMAP_CONFIG_PAGEINFO_DATA_ERROR 		0x5A /* since v0571 */
#define SMAP_CONFIG_DONE 						0x5B /* since v0571 */
#define SMAP_CONFIG_DONE_OK 					0x5C /* since v0571 */
#define SMAP_CONFIG_DONE_ERROR					0x5D /* since v0571 */
#define SMAP_MENU_CONFIG_INITIALIZE 			0x5E /* since v0571 */
#define SMAP_MENU_CONFIG_INITIALIZE_OK 			0x5F /* since v0571 */
#define SMAP_MENU_CONFIG_INITIALIZE_ERROR 		0x60 /* since v0571 */
#define SMAP_MENU_CONFIG_SET 					0x61 /* since v0571 */
#define SMAP_MENU_CONFIG_SET_OK 				0x62 /* since v0571 */
#define SMAP_MENU_CONFIG_SET_ERROR 				0x63 /* since v0571 */
#define SMAP_MENU_CONFIG_DONE 					0x64 /* since v0571 */
#define SMAP_MENU_CONFIG_DONE_OK 				0x65 /* since v0571 */
#define SMAP_MENU_CONFIG_DONE_ERROR 			0x66 /* since v0571 */
#define SMAP_CONFIG_AVROOM_INITIALIZE			0x67 /* since v0610 */
#define SMAP_CONFIG_AVROOM_INITIALIZE_OK		0x68 /* since v0610 */
#define SMAP_CONFIG_AVROOM_INITIALIZE_ERROR		0x69 /* since v0610 */
#define SMAP_CONFIG_AVROOM_SET					0x6A /* since v0610 */
#define SMAP_CONFIG_AVROOM_SET_OK				0x6B /* since v0610 */
#define SMAP_CONFIG_AVROOM_SET_ERROR			0x6C /* since v0610 */
#define SMAP_CONFIG_AVROOM_DONE					0x6D /* since v0610 */
#define SMAP_CONFIG_AVROOM_DONE_OK				0x6E /* since v0610 */
#define SMAP_CONFIG_AVROOM_DONE_ERROR			0x6F /* since v0610 */
#define GATEWAY_CONFIG_UNIV_INITIALIZE      	0x70 /* since v0620 */
#define GATEWAY_CONFIG_UNIV_INITIALIZE_OK    	0x71 /* since v0620 */
#define GATEWAY_CONFIG_UNIV_INITIALIZE_ERROR 	0x72 /* since v0620 */
#define GATEWAY_CONFIG_UNIV_SET              	0x73 /* since v0620 */
#define GATEWAY_CONFIG_UNIV_SET_OK           	0x74 /* since v0620 */
#define GATEWAY_CONFIG_UNIV_SET_ERROR        	0x75 /* since v0620 */
#define GATEWAY_CONFIG_UNIV_DONE            	0x76 /* since v0620 */
#define GATEWAY_CONFIG_UNIV_DONE_OK          	0x77 /* since v0620 */
#define GATEWAY_CONFIG_UNIV_DONE_ERROR       	0x78 /* since v0620 */
// Reserve 0x79 tem 0x7F
#define ROOMCTRL_AUDIOCFG_INITIALISE			0x80 /* since v0625 */
#define ROOMCTRL_AUDIOCFG_INITIALISE_OK			0x81 /* since v0625 */
#define ROOMCTRL_AUDIOCFG_INITIALISE_ERROR		0x82 /* since v0625 */
#define ROOMCTRL_AUDIOCFG_DEST_SET				0x83 /* since v0625 */
#define ROOMCTRL_AUDIOCFG_DEST_SET_OK			0x84 /* since v0625 */
#define ROOMCTRL_AUDIOCFG_DEST_SET_ERROR		0x85 /* since v0625 */
#define ROOMCTRL_AUDIOCFG_SRC_SET				0x86 /* since v0625 */
#define ROOMCTRL_AUDIOCFG_SRC_SET_OK			0x87 /* since v0625 */
#define ROOMCTRL_AUDIOCFG_SRC_SET_ERROR			0x88 /* since v0625 */
#define ROOMCTRL_AUDIOCFG_TNRPRESET_SET			0x89 /* since v0625 */
#define ROOMCTRL_AUDIOCFG_TNRPRESET_SET_OK		0x8a /* since v0625 */
#define ROOMCTRL_AUDIOCFG_TNRPRESET_SET_ERROR	0x8b /* since v0625 */
#define ROOMCTRL_AUDIOCFG_SRCFUNC_SET			0x8c /* since v0625 */
#define ROOMCTRL_AUDIOCFG_SRCFUNC_SET_OK		0x8d /* since v0625 */
#define ROOMCTRL_AUDIOCFG_SRCFUNC_SET_ERROR		0x8e /* since v0625 */
#define ROOMCTRL_AUDIOCFG_DONE					0x8f /* since v0625 */
#define ROOMCTRL_AUDIOCFG_DONE_OK				0x90 /* since v0625 */
#define ROOMCTRL_AUDIOCFG_DONE_ERROR			0x91 /* since v0625 */
// Reserve 0x92 tem 0x9A
#define BOSEST_CONFIG_INITIALISE				0xA0 /* since v0802 */
#define BOSEST_CONFIG_INITIALISE_OK				0xA1 /* since v0802 */
#define BOSEST_CONFIG_INITIALISE_ERROR			0xA2 /* since v0802 */
#define BOSEST_CONFIG_RENDER					0xA3 /* since v0802 */
#define BOSEST_CONFIG_RENDER_OK					0xA4 /* since v0802 */
#define BOSEST_CONFIG_RENDER_ERROR				0xA5 /* since v0802 */
#define BOSEST_CONFIG_DONE						0xA6 /* since v0802 */
#define BOSEST_CONFIG_DONE_OK					0xA7 /* since v0802 */
#define BOSEST_CONFIG_DONE_ERROR				0xA8 /* since v0802 */
// Reserve 0xA8 tem 0xAF
#define LEDCFG_INIT								0xB0 /* since v0804 */
#define LEDCFG_INIT_OK							0xB1 /* since v0804 */
#define LEDCFG_INIT_ERROR						0xB2 /* since v0804 */
#define LEDCFG_DATA								0xB3 /* since v0804 */
#define LEDCFG_DATA_OK							0xB4 /* since v0804 */
#define LEDCFG_DATA_ERROR						0xB5 /* since v0804 */
#define LEDCFG_DATA_LED							0xB6 /* since v0804 */
#define LEDCFG_DATA_LED_OK						0xB7 /* since v0804 */
#define LEDCFG_DATA_LED_ERROR					0xB8 /* since v0804 */
#define LEDCFG_DONE								0xB9 /* since v0804 */
#define LEDCFG_DONE_OK							0xBA /* since v0804 */
#define LEDCFG_DONE_ERROR						0xBB /* since v0804 */

#define SMAPLITE_CONFIG_INITIALISE				0xC0 /* since 0x0C05 */
#define SMAPLITE_CONFIG_INITIALISE_OK			0xC1 /* since 0x0C05 */
#define SMAPLITE_CONFIG_INITIALISE_ERROR		0xC2 /* since 0x0C05 */
#define SMAPLITE_CONFIG_ENTRY					0xC3 /* since 0x0C05 */
#define SMAPLITE_CONFIG_ENTRY_OK				0xC4 /* since 0x0C05 */
#define SMAPLITE_CONFIG_ENTRY_ERROR				0xC5 /* since 0x0C05 */
#define SMAPLITE_CONFIG_DONE					0xC6 /* since 0x0C05 */
#define SMAPLITE_CONFIG_DONE_OK					0xC7 /* since 0x0C05 */
#define	SMAPLITE_CONFIG_DONE_ERROR				0xC8 /* since 0x0C05 */

#define EXTERNALCONFIG_NOT_IMPLEMENTED			0xFF /* since v0571 */

//////////////////////////////////////////////////////////////////////////

// Objecten
#define FTP_OBJECT_OPEN_WRITE					0x00 /** TM20100317 */
#define	FTP_OBJECT_OPEN_READ					0x01 /** TM20100317 */
#define FTP_OBJECT_APPEND_DATA					0x02 /** TM20100325 */
#define FTP_OBJECT_CLOSE						0x03 /** TM20100325 */
#define FTP_OBJECT_INFO_FILESIZE				0x04 /** TM20100401 */
#define FTP_OBJECT_ABORT						0x05 /** TM20100407 */
#define FTP_OBJECT_READ_DATA					0x06 /** TM20100412 */

// Error codes.
#define	FTP_OBJECT_EC_WRONG_STATE				0x80 /** TM20100331 */
#define FTP_OBJECT_EC_WRITE_DATA  				0x81 /** TM20100331 */
#define FTP_OBJECT_EC_READ_DATA					0x82 /** TM20100331 */
#define FTP_OBJECT_EC_OPEN_FILE					0x83 /** TM20100331 */
#define FTP_OBJECT_EC_CLOSE_FILE				0x84 /** TM20100331 */
#define FTP_OBJECT_EC_GENERAL					0x85 /** TM20100331 */
#define FTP_OBJECT_EC_NOT_IMPLEMENTED			0x86 /** TM20100402 */
#define FTP_OBJECT_EC_GET_INFO					0x87 /** TM20100413 */
//////////////////////////////////////////////////////////////////////////

// Objecten
#define DATABASE_OBJECT_ERASE		 	 		0x00 /** TM 20100419 */
#define DATABASE_OBJECT_LOAD		  	  		0x01 /** TM 20100419 */
#define DATABASE_OBJECT_SAVE			  		0x02 /** TM 20100419 */
#define DATABASE_OBJECT_BACKUP		  			0x03 /** TM 20100419 */
#define DATABASE_ERROR_NOT_IMPLEMENTED  		0x80 /** TM 20100419 */
#define DATABASE_ERROR_SERIALIZATION_BUSY		0x81 /** TM 20191028 */

// Database identifier.
#define	DATABASE_ID_IRTXCODES           		0x00 /** TM 20100419 */
#define DATABASE_ID_SOURCES_SONOS				0x01 /** TM 20150218 */
#define DATABASE_ID_AVSETTINGS					0x02 /** TM 20181108 */
 
//////////////////////////////////////////////////////////////////////////

/*
// methods : zie IMPLEMENT_UNIT_ENERGY
#define ENERGYMEASUREMENT_OBJECT_USE_SELECTOR_DATA		0x00
#define ENERGYMEASUREMENT_ATTR_STATE					0x01
#define ENERGYMEASUREMENT_ATTR_CONFIG					0x02
#define ENERGYMEASUREMENT_ATTR_MEASURESTATUS			0x03	//
#define ENERGYMEASUREMENT_ATTR_ONOFF					0x04
#define ENERGYMEASUREMENT_ATTR_SUPPORTED				0x05
#define ENERGYMEASUREMENT_ATTR_ENERGY_ACTIVE			0x06
#define ENERGYMEASUREMENT_ATTR_POWER_ACTIVE				0x07
#define ENERGYMEASUREMENT_ATTR_CURRENT_RMS				0x08
#define ENERGYMEASUREMENT_ATTR_VOLTAGE_RMS				0x09
#define ENERGYMEASUREMENT_ATTR_ENERGY_ACTIVE_3PHASE		0x0A
#define ENERGYMEASUREMENT_ATTR_POWER_ACTIVE_3PHASE		0x0B
#define ENERGYMEASUREMENT_ATTR_CURRENT_RMS_3PHASE		0x0C
#define ENERGYMEASUREMENT_ATTR_VOLTAGE_RMS_3PHASE		0x0D
#define ENERGYMEASUREMENT_OBJECT_SAMPLE_REQUEST			0x0E
#define ENERGYMEASUREMENT_ATTR_SAMPLE_INTERVAL			0x0F


// extra : zie IMPLEMENT_UNIT_ENERGY
#define ENERGYMEASUREMENT_MEASURE_DONE					0x00
#define ENERGYMEASUREMENT_MEASURE_BUSY					0x01
*/

// methods : zie IMPLEMENT_UNIT_ENERGY
#define EM_OBJECT_USE_SELECTOR_DATA  	0x00
#define EM_ATTR_STATE					0x01
#define EM_ATTR_CONFIG					0x02
#define EM_ATTR_MEASURESTATUS			0x03	// verstuurd als broadcast
#define EM_ATTR_ONOFF					0x04
#define EM_ATTR_SUPPORTED				0x05
#define EM_OBJECT_SAMPLE_REQUEST		0x06
#define EM_ATTR_SAMPLE_INTERVAL			0x07
#define EM_ATTR_ENERGY_ACTIVE_DAY		0x08
#define EM_ATTR_ENERGY_ACTIVE_NIGHT		0x09
#define EM_ATTR_POWER_ACTIVE			0x0A
#define EM_ATTR_CURRENT_RMS				0x0B
#define EM_ATTR_VOLTAGE_RMS				0x0C

// extra : zie IMPLEMENT_UNIT_ENERGY
#define EM_MEASURE_DONE					0x00
#define EM_MEASURE_BUSY					0x01

//////////////////////////////////////////////////////////////////////////
#define MAX_LENGTH			200
#define BROADCAST_UNIT		0xFF
#define BROADCAST_NODE		0x00


#endif /* INCLUDED_NODEMESS_H */