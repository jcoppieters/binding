#ifndef CONFIGMULTIMEDIASTRUCTS
#define CONFIGMULTIMEDIASTRUCTS



//--------------------------------------------------------------
#define SOURCE_NAME_LENGTH					16
#define DESTINATION_NAME_LENGTH				16
#define VIDEOMUXSOURCE_NAME_LENGTH			16
#define VIDEOMUXDESTINATION_NAME_LENGTH		16
#define	SPEECHDEV_NAME_LENGTH				16
#define ALARMZONENAME_LENGTH				16
#define DESTINATION_NAME_LENGTH				16
#define VIDEOMUXSOURCE_NAME_LENGTH			16
#define VIDEOMUXDESTINATION_NAME_LENGTH		16
#define SPEECHDEV_NAME_LENGTH				16
#define	ALARMZONENAME_LENGTH				16

#define MAX_IR_CODES						1
//--------------------------------------------------------------

// Changes: TM,V14.09: Verhogen van aantal sources/dest + speech dev outdoor units.

#define NR_OF_AVMATRIX_SYSTEMS			16		/* TM_MULTIPLE_AVMATRIX */
#define NR_OF_MATRIX_SOURCES			8
#define NR_OF_MATRIX_SOURCES_V65		128		// was 32		
#define NR_OF_MATRIX_SOURCES_V66		128
#define NR_OF_MATRIX_DESTINATIONS		64 
#define NR_OF_MATRIX_DESTINATIONS_V65	128		// nieuw
#define NR_OF_MATRIX_DESTINATIONS_V66	128
#define NR_OF_ROOM_UNITS				64 
#define NR_OF_VIDEOMUXSOURCES			16 
#define NR_OF_VIDEOMUXDESTINATIONS		16 
#define NR_OF_SPEECHDEVINDOORS			16 
#define NR_OF_SPEECHDEVOUTDOORS			5
#define NR_OF_SPEECHDEVOUTDOORS_V65		16		// nieuw
#define NR_OF_SPEECHDEVOUTDOORS_V66		16
#define GALAXYCENTRALEMAXALARMZONES		208

//--------------------------------------------------------------

#define NO_ASSIGNMENT			0xFF

#define NODE_ADR				0
#define UNIT_ADR				1

#define NODE_ADDRESS_INVALID	0xFF
#define UNIT_ADDRESS_INVALID	0xFF
#define NO_IR_CODE_ID			0xFFFF

//--------------------------------------------------------------


/********************************* VIDEOMUX CONFIGURATION STRUCTURE ********************************************/
/* macros for bMuxType */
#define MUX_DISABLED 0
#define MUX_DDL      1
#define MUX_RAS1     2
#define MUX_RAS4     3

#pragma pack(1)	/* Oppassen: #pragma pack() toevoegen aan einde */
typedef struct
{
   BYTE bMuxType; /* no mux, ras1, ras4 */
   BYTE bMuxInputs;
   BYTE bMuxOutputs;
   /* UNIT_TYPE_VIDEOMUX */
   BYTE bVideoMuxAddress[2];  /*node address (default = 0xff), unit address (default = 0xff)*/
   BYTE bReserved1;
   BYTE bReserved2;
   BYTE bReserved3;
   BYTE bReserved4;

} STRUCT_CONFIGURATION_VIDEOMUX;
#pragma pack()
/*******************************************************************************************************/

/********************************* AVMATRIX CONFIGURATION STRUCTURE ********************************************/
/* macros for bMatrixType */
#define MATRIX_DISABLED 0
#define MATRIX_XANTECH  1
#define MATRIX_DDL      2

/*********************************************************
 * TM_MULTIPLE_AVMATRIX:
 * Config structuur blijft compatible 
 * bReserved1 vervangen door bMatrixOffsetInputs
 * bReserved2 vervangen door bMatrixOffsetOutputs
 ********************************************************/
#pragma pack(1)	/* Oppassen: #pragma pack() toevoegen aan einde */
typedef struct
{
   BYTE bMatrixType;			/* no Matrix, Xantech */
   BYTE bMatrixInputs;
   BYTE bMatrixOutputs; 
   BYTE bAVMatrixAddress[2];
   BYTE bMatrixOffsetInputs;	/* TM_MULTIPLE_AVMATRIX: Vervangt bReserved1 */
   BYTE bMatrixOffsetOutputs;	/* TM_MULTIPLE_AVMATRIX: Vervangt bReserved2 */
   BYTE bReserved3;
   BYTE bReserved4;

} STRUCT_CONFIGURATION_AVMATRIX;
#pragma pack()
/*******************************************************************************************************/

/********************************* AUDIO MATRIX SOURCE STRUCTURE ********************************************/
/* macros for bConfig */
#define SRC_CONFIG_MASK       0x03
#define SRC_CONFIG_AUDIO      0x00
#define SRC_CONFIG_AUDIOVIDEO 0x01
#define SRC_CONFIG_VIDEO      0x02
#define SRC_CONFIG_MUXOUTPUT  0x03

#define SRC_RGB_VIDEO_INPUT      0x04  /* or CV */
#define SRC_SVHS_VIDEO_INPUT     0x08  /* or Digital */
#define SRC_DIG_AUDIO_INPUT      0x10  /* or Analog audio */

/* macros for bSourceStatus */
#define SOURCESTATUS_ON 0x01

/* 19032001 */
/* macros for bType */
#define SRC_DEFAULT 0
#define SRC_PC 1

#define EXTRAIRCODES 25

#pragma pack(1)	/* Oppassen: #pragma pack() toevoegen aan einde */
typedef struct
{
   char szSourceName[SOURCE_NAME_LENGTH];

   unsigned short usOn[MAX_IR_CODES];
   unsigned short usOff[MAX_IR_CODES];
   unsigned short usPlay[MAX_IR_CODES];
   unsigned short usStop[MAX_IR_CODES];
   unsigned short usPause[MAX_IR_CODES];
   unsigned short usRR[MAX_IR_CODES];
   unsigned short usFF[MAX_IR_CODES];
   unsigned short usFrwd[MAX_IR_CODES];
   unsigned short usFfwd[MAX_IR_CODES];

   unsigned short usExtraIR[EXTRAIRCODES][MAX_IR_CODES];

   BYTE bPlayControlAddress[2];  /*node address (default = 0xff), unit address (default = 0xff)*/
   /* UNIT_TYPE_IRTX */ 
   BYTE bIRTXAddress[2];  /*node address (default = 0xff), unit address (default = 0xff)*/
   BYTE bConfig; /*
                    bit 1..0 = 00:Audio, 01:Audio/Video, 10:Video, 11:Mux output
                    Bit 2 = RGB video input on destination 
                    Bit 3 = SVHS video on destination 
                    Bit 4 = Digital audio on destination
                 */

   BYTE bSourceStatus; /* bit 0 source ON/OFF */
   BYTE bAVMatrixInput;

   BYTE bType;

   BYTE bReserved1;
   BYTE bReserved2;
   BYTE bReserved3;

} STRUCT_MATRIX_AUDIO_UNIT_SOURCE;
#pragma pack()
/*******************************************************************************************************/


/********************************* AUDIO MATRIX DESTINATION STRUCTURE ********************************************/
/* macros for bConfig */
#define DEST_USE_STARTUP_VOL 0x01

#define DEST_CONFIG_MASK       0x06
#define DEST_CONFIG_AUDIOVIDEO 0x00
#define DEST_CONFIG_AUDIO      0x02
#define DEST_CONFIG_VIDEO      0x04

/* 19032001 */
/* macros for bType */
#define DEST_DEFAULT   0
#define DEST_LCD_TFT   1
#define DEST_MUX_INPUT 2

/* macros for bState */
#define OVERRIDE_BY_VIDEOPHONE 0x01

#pragma pack(1)	/* Oppassen: #pragma pack() toevoegen aan einde */
typedef struct
{

   char szDestinationName[DESTINATION_NAME_LENGTH];

   unsigned short usVideoOn[MAX_IR_CODES];
   unsigned short usVideoOff[MAX_IR_CODES];
   unsigned short usAudioOn[MAX_IR_CODES];
   unsigned short usAudioOff[MAX_IR_CODES];
   /* react on bit2 setting in source */
   unsigned short usVideoCVInput[MAX_IR_CODES]; 
   unsigned short usVideoRGBInput[MAX_IR_CODES];
   /* react on bit3 setting in source */
   unsigned short usVideoSVHSInput[MAX_IR_CODES];
   unsigned short usVideoDigitalInput[MAX_IR_CODES];
   /* react on bit4 setting in source */
   unsigned short usAudioAnalogInput[MAX_IR_CODES];
   unsigned short usAudioDigitalInput[MAX_IR_CODES];

   /* UNIT_TYPE_IRTX */ 
   BYTE bIRTXAddress[2];  /*node address (default = 0xff), unit address (default = 0xff)*/
   /* Audio on/off. Can be a UNIT_TYPE_SWITCH or UNIT_TYPE_CONTROL */ 
   BYTE bAudioOnOffAddress[2];  /*node address (default = 0xff), unit address (default = 0xff)*/
   /* Video display on/off. Can be a UNIT_TYPE_SWITCH or UNIT_TYPE_CONTROL */ 
   BYTE bVideoOnOffAddress[2];  /*node address (default = 0xff), unit address (default = 0xff)*/

   BYTE bMaxVolume; /* 0..40 */   
   BYTE bStartVolume; /* 0..40 */   
   BYTE bVolume; /* default 0 */
   BYTE bTreble; /* default 0 */   
   BYTE bBass; /* default 0 */   
    
   BYTE bConfig; /* bit0 : use startup volume */

   BYTE bState; /* bit0 : Override by videophone */ 

   BYTE bSourceSelection; /* bSourceNR 0..MAX_AUDIO_UNIT_SOURCES */
   BYTE bPreviousSourceSelection; /* bSourceNR 0..MAX_AUDIO_UNIT_SOURCES */

   BYTE bAVMatrixOutput;

   BYTE bVideoMuxSrc; /* index for STRUCT_VIDEOMUXSOURCE array */

   BYTE bType; /* default, lcdtft, videomuxinput */ 
   BYTE bReserved1;
   BYTE bReserved2;
   BYTE bReserved3;


} STRUCT_MATRIX_AUDIO_UNIT_DESTINATION;
#pragma pack()
/*******************************************************************************************************/


/********************************* VIDEO MUX SOURCE STRUCTURE ********************************************/
/* macros for bConfig */
#define MUX_CAMERA_OUTPUT  0
#define MUX_MOZAIEK_OUTPUT 1

/* 28032001 Videophone & camera surveillance configuration structures. */

#pragma pack(1)		/* Oppassen: #pragma pack() toevoegen aan einde */
typedef struct
{
   char szVideoMuxSourceName[VIDEOMUXSOURCE_NAME_LENGTH];
   BYTE bVideoMuxInput;
   BYTE bConfig; 

   BYTE bType;  /* idem as destinations */
   BYTE bReserved1;
   BYTE bReserved2;
   BYTE bReserved3;

} STRUCT_VIDEOMUXSOURCE;
#pragma pack()
/*******************************************************************************************************/


/********************************* VIDEO MUX DESTINATION STRUCTURE ********************************************/
/* macros for bType */
#define VIDEOMUXDEST_DEFAULT   0
#define VIDEOMUXDEST_LCD_TFT   1
#define VIDEOMUXDEST_MTX_INPUT 2

#pragma pack(1)		/* Oppassen: #pragma pack() toevoegen aan einde */
typedef struct
{
   char szVideoMuxDestinationName[VIDEOMUXDESTINATION_NAME_LENGTH];

   BYTE bVideoMuxOutput;

   BYTE bConfig; /* 0=Camoutput, 1=mozaiek*/
   
   BYTE bState; /* bit0 : Override by videophone */ 

   /* Video display on/off. Can be a UNIT_TYPE_SWITCH or UNIT_TYPE_CONTROL */
   BYTE bVideoOnOffAddress[2];  /*node address (default = 0xff), unit address (default = 0xff)*/

   BYTE bAVMatrixSrc; /* index for STRUCT_MATRIX_AUDIO_UNIT_SOURCE array */

   BYTE bType; /* default, lcdtft, AV matrix input */

   BYTE bSourceSelection; /* bSourceNR 0..NR_OF_VIDEOMUXSOURCES */
   BYTE bPreviousSourceSelection; /* bSourceNR 0..NR_OF_VIDEOMUXSOURCES */
   
   BYTE bReserved0;
   BYTE bReserved1;
   BYTE bReserved2;
   BYTE bReserved3;

} STRUCT_VIDEOMUXDESTINATION;
#pragma pack()
/*******************************************************************************************************/


/* 27042001 */
/********************************* SPEECHCOMM UNIT CONFIGURATION STRUCTURE ********************************************/
/* This configuration should be done with pindings ove rthe Powermate protocolstack. However,
   for simplicity, we use a simple structure that we download to the LCD master and map to speechcomm
   units after browsing. */
/* macros for bConfig */
#define MAX_MATRIX_DESTINATIONS 4

#pragma pack(1)		/* Oppassen: #pragma pack() toevoegen aan einde */
typedef struct
{
   char szName[SPEECHDEV_NAME_LENGTH];

   /* UNIT_TYPE_SPEECHCOMM */
   BYTE bLCDMapAddress;  /*node address (default = 0xff)*/
   BYTE bDest[MAX_MATRIX_DESTINATIONS]; /* if indoor */ /* can be mux or mtx destination */
   BYTE bAVMatrixDest;
   /* Video display on/off. Must be UNIT_TYPE_SWITCH*/
   BYTE bSwitchAddress[2]; /*node address (default = 0xff), unit address (default = 0xff)*/
   BYTE bConfig;
   BYTE bReserved1;
   BYTE bReserved2;
   BYTE bReserved3;

} STRUCT_SPEECHDEVINDOOR;
#pragma pack()
/*******************************************************************************************************/

/* 27042001 */
/********************************* SPEECHCOMM UNIT CONFIGURATION STRUCTURE ********************************************/
/* This configuration should be done with pindings ove rthe Powermate protocolstack. However,
   for simplicity, we use a simple structure that we download to the LCD master and map to speechcomm
   units after browsing. */
/* macros for bConfig */

#pragma pack(1)		/* Oppassen: #pragma pack() toevoegen aan einde */
typedef struct
{
   char szName[SPEECHDEV_NAME_LENGTH];

   BYTE bVideoMuxSrc; /* if outdoor */
   /* UNIT_TYPE_CONTROL UNIT_TYPE_LCDVIRTUAL, must be explicitely binded */ 
   BYTE bRingerAddress[2];  /*node address (default = 0xff), unit address (default = 0xff)*/
   /* UNIT_TYPE_CONTROL UNIT_TYPE_LCDVIRTUAL, must be explicitely binded */ 
   BYTE bDoorOpenerAddress[2];  /*node address (default = 0xff), unit address (default = 0xff)*/
   /* UNIT_TYPE_CONTROL UNIT_TYPE_LCDVIRTUAL, must be explicitely binded */ 
   BYTE bCallerAddress[2];  /*node address (default = 0xff), unit address (default = 0xff)*/
   /* Video display on/off. Must be UNIT_TYPE_SWITCH*/
   BYTE bSwitchAddress[2]; /*node address (default = 0xff), unit address (default = 0xff)*/
   BYTE bAVMatrixSrc; /* 17052001 */
   BYTE bFunctionOutput[2];		// bReserved1 + bReserved2
   BYTE bRefVideoPhone;			// bReserved3
/*
   BYTE bReserved1;
   BYTE bReserved2;
   BYTE bReserved3;
*/

} STRUCT_SPEECHDEVOUTDOOR;
#pragma pack()
/*******************************************************************************************************/

/* 27042001 */
/********************************* VIDEOPHONE - MUX CONFIGURATION STRUCTURE ********************************************/

#pragma pack(1)		/* Oppassen: #pragma pack() toevoegen aan einde */
typedef struct
{
   BYTE bCallTimeout;  /* default 120 seconds */
   /* in minutes */
   BYTE bConnectTimeout; /* default 5 minutes */
   /* UNIT_TYPE_SPEECHMTX */
   BYTE bSpeechMtxAddress[2];  /*node address (default = 0xff), unit address (default = 0xff)*/
   
   BYTE bVideoMuxDest;
   BYTE bDropVolume;

   BYTE bReserved0;
   BYTE bReserved1;
   BYTE bReserved2;
   BYTE bReserved3;

} STRUCT_VIDEOPHONE_CONFIG;
#pragma pack()
/*******************************************************************************************************/


/********************************* AUDIOROOM UNIT STRUCTURE ********************************************/
#define MAX_AUDIO_UNIT_DESTINATIONS 4
#define MAX_AUDIO_UNIT_SOURCES 8

/* macros for bDestOnOff */
#define AUDIOUNITDEST_ON 1

#define AUDIO_UNIT_STATE_MASK 0x03                          

/* states for bCurrentState & bPreviousState */
#define AUDIO_UNIT_OFF_STATE      0x00  /* Local state machine thread*/
#define AUDIO_UNIT_ON_STATE       0x01  /* Local state machine thread*/
#define AUDIO_UNIT_VOLUP_STATE    0x02  /* Local state machine thread*/
#define AUDIO_UNIT_VOLDOWN_STATE  0x03  /* Local state machine thread*/

/* Local events for control unit object*/
#define AUDIO_OFF              0
#define AUDIO_ON               1  
#define AUDIO_SOURCE_SELECT    2
#define AUDIO_VOLUME_UP        3
#define AUDIO_VOLUME_DOWN      4
#define AUDIO_NO_EVENT         5

#pragma pack(1)		/* Oppassen: #pragma pack() toevoegen aan einde */
typedef struct
{
   BYTE bAudioConfig;

   BYTE bSrc[MAX_AUDIO_UNIT_SOURCES];
   BYTE bDest[MAX_AUDIO_UNIT_DESTINATIONS];
   BYTE bDestLink[MAX_AUDIO_UNIT_DESTINATIONS];

   BYTE bReserved0;
   BYTE bReserved1;
   BYTE bReserved2;
   BYTE bReserved3;

   BYTE bAudioStatus1; /* bit 0..3 : destinations ON/OFF  */
                       /* bit 4..7 status 0 = running, 1 = error*/
   BYTE bAudioStatus2; /* bit 7 : Message waiting...                   */

   BYTE bCurrentState; /* bit 1..0 dest 0
					      bit 3..2 dest 1
						  bit 5..4 dest 2
						  bit 7..6 dest 3 */

   BYTE bPreviousState; /* bit 1..0 dest 0
					      bit 3..2 dest 1
						  bit 5..4 dest 2
						  bit 7..6 dest 3 */

   BYTE bDestCurSel;
   BYTE bDestSrcSel[MAX_AUDIO_UNIT_DESTINATIONS];
   BYTE bDestVolume[MAX_AUDIO_UNIT_DESTINATIONS];

   BYTE bIRReceiverAddress[2]; /* 30112001 */

} UNIT_AUDIOROOM_STRUCT;
#pragma pack()

#pragma pack(1)			/* Oppassen: #pragma pack() toevoegen aan einde */
typedef struct
{
   char szName[ALARMZONENAME_LENGTH];
   BYTE bStatus;

} STRUCT_ALARMZONE_CONFIG;
#pragma pack()


#endif
