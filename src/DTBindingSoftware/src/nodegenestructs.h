
/* unit config*/
/* Caused by the lack of memory, we cannot use malloc (C) or new (C++). So we*/
/* must implement macros...   */

#ifndef NODEGENERAL
#define NODEGENERAL
    
#ifndef MAIN
#define EXTERN extern
#else
#define EXTERN 
#endif
            
#define WORKING_NORMAL   48 /* 2 sec */
#define WORKING_PASSIVE  24 /* 1 sec */
#define WORKING_BUSOFF   12 /* 1/2 sec */
#define WORKING_INIT     6  /* 1/4 sec */                        
             
/* Specific macros for feedback mechanism (switch object, dim object)...*/
#define FEEDBACK_RELOAD_VALUE 83 /* 100    5 sec. */
#define UNIT_FEEDBACKTIMER_RUNNING       0x01
#define UNIT_FEEDBACK_HW_PORT_HIGH       0x08 
#define UNIT_FEEDBACKTIMER_EXPIRED       0x04
 
EXTERN void BlinkRTC(BYTE);             
EXTERN void InitMCP2510SPIIrq(void);
EXTERN void InitAll(void);
EXTERN void InitUnits(void);            
EXTERN void ResetUnits(void);            
EXTERN void InitBindingStructs(void);
EXTERN void InitNode(void);          
EXTERN void InitPowermateLCD(void);          
EXTERN void InitialiseLCD(void);
EXTERN void EraseDataBasesLCD(void);
EXTERN void ResetLCD(void);
EXTERN void InitPowermateProcessTimer(void);

#define END_TRUE  1  
#define END_FALSE 0
//#define ERROR 1
//#define NO_ERROR 0
#define SUCCESS 0

/***************************************************************************************/
/* Node and unit structures*/

/* node types*/
#define NODE_PIN_STD_UNITS      0x01
#define NODE_PIN_ROUTER_TIN     0x02   /* Route between PL and TP.*/
#define NODE_PIN_GATEWAY		0x04   /* Protocol translation between DDL HC3 and DDL PIN*/
#define NODE_PIN_MODEM          0x08   /* Powerline modem for installation.*/
#define NODE_RESERVED           0x10  
#define NODE_LCD                0x20


// unit types
#define UNIT_TYPE_DIM					0x01	/* OUT */
#define UNIT_TYPE_SWITCH				0x02	/* OUT */
#define UNIT_TYPE_CONTROL				0x03	/* IN */
#define UNIT_TYPE_SENS					0x04	/* IN */
#define UNIT_TYPE_AUDIO					0x05	/* OUT */
#define UNIT_TYPE_LCD					0x06	/* NOT USED ANYMORE */
#define UNIT_TYPE_LCD_VIRTUAL			0x07	/* IN , OUT */
#define UNIT_TYPE_DUOSWITCH				0x08	/* OUT */
#define UNIT_TYPE_SELECTOR				0x09	/* NOT USED ANYMORE */
#define UNIT_TYPE_BOSERS				0x0A    /* IN */            
#define UNIT_TYPE_AVMATRIX				0x0B	/* OUT */
#define UNIT_TYPE_IRTX					0x0C	/* OUT */
#define UNIT_TYPE_RC5RX					0x0D	/* IN */
#define UNIT_TYPE_VIDEOMUX				0x0E	/* OUT */
#define UNIT_TYPE_PS2INTELLIMOUSE		0x0F	/* DON'T IMPLEMENT*/
#define UNIT_TYPE_SPEECHCOMM			0x10	/* NOT USED */
#define UNIT_TYPE_SPMATRIX				0x11	/* NOT USED */
#define UNIT_TYPE_ALARM					0x12	/* IN , OUT*/
#define UNIT_TYPE_ENERGYMEASURE			0x13	// IMPLEMENT_UNIT_ENERGY

#define UNIT_TYPE_AUDIO_HC4				0x85	/* Specific HC4 Audio Unit ! */


#define NAME_LENGTH            16    /* node and unit names*/
 
/***************************************************************************************/
/* Message queue (29 bytes)*/
/* Message types*/
#define BINDING_COMMAND              0x01
#define BINDING_RESPONSE             0x02
#define BINDING_EVENT                0x03
#define BINDING_BROADCAST_COMMAND    0x04
#define RUNTIME_COMMAND              0x05
#define RUNTIME_RESPONSE             0x06
#define RUNTIME_EVENT                0x07
#define RUNTIME_BROADCAST_COMMAND    0x08
#define MANAGEMENT_COMMAND           0x0C
#define MANAGEMENT_RESPONSE          0x0D
#define MANAGEMENT_EVENT             0x0E
#define MANAGEMENT_BROADCAST_COMMAND 0x0F
    
/* Macros for bUnitFlags and bNodeFlags              */
#define NIC_NETWORK_CHANNEL_DLL_RETRY_ENABLED      0x01                     
#define NIC_NETWORK_CHANNEL_EXPLICIT_RETRY_ENABLED 0x02
#define UNIT_SEND_AT_RESET                         0x04    

/* Macros specific for bNodeFlags                    */
#define NODE_MASTER_LCD                            0x08
#define NODE_LCD_TYPE_MASK                         0x30
#define NODE_LCD_TYPE_BASIC                        0x00  
#define NODE_LCD_TYPE_HC4                          0x10  
#define NODE_LCD_TYPE_HC5                          0x20  

#define NODE_LCD_MATRIX_AUDIO                      0x40  /* IR_AUDIO : bit6=0 */

// Since v0638:
// vlag die bepaalt of de gateway node een database van nodes/units heeft.
#define INCLUDE_UPDATE_DATABASE					   0x80

/* Macros specific for bUnitFlags                    */
#define EXCLUDE_FROM_BROADCAST                     0x10                   


typedef struct
{
   BYTE bUnitFlags;
   char pcUnitName[NAME_LENGTH];

} UNIT_INFO_STRUCT;
 
/* Node current state*/
#define NODESTATUS_RUNNING 0
#define NODESTATUS_BINDING_CONFIGURATION 1
#define NODESTATUS_BINDING_RESET 2
#define NODESTATUS_FACTORY_RESET 3


/* 03072001 */
#define APPL_SERIALIZE 0x01
#define APPL_UNLOCK 0x02

#define AUTO_RETRY_MODE 0x80 /* if bit 7 of bDLLAccessControl is 1*/

                                     
typedef struct
{
   /* 27012000 */ 
   BYTE bLCDMasterAddress; 
   BYTE bLogicalNodeAddress; 
               
} NODE_UNIVERSAL_STRUCT; 
   
/* macros for bNodeDebugMode*/
#define NODE_NORMAL_MODE 0x00   
#define NODE_DEBUG_MODE 0x01   
#define NODE_FACTORY_RESET 0x02   
#define NODE_FACTORY_SETTINGS 0x03   
    
EXTERN void CheckNodeObject(void);

#define NODE_MESSAGE_WAITING 0x80
/*
typedef struct
{
   BYTE bUnitType;
   
} UNIT_TYPE_STRUCT;
 */
 
typedef struct
{
   BYTE bExplicitMessageID;

} STRUCT_UNIT_STATUS; 
  
typedef struct
{
   BYTE bExplicitMessageID;
   BYTE bNodeCurrentState;                                            /* SRAM   */
   BYTE bNodeStatus; /* bit 7 : Message Waiting*/
   BYTE bNodeDebugMode;    

   /* NODE Network resources*/
   BYTE bMaxMessageLength;                               /* MACRO*/
   /* NODE Binding RESOURCES */
   BYTE bNROfReferences;                                 /* MACRO*/
   BYTE bNROfTimerOperators;                             /* MACRO*/
   BYTE bNROfBindings;                                   /* MACRO*/
   BYTE bMaxBindingConditionLength;                      /* MACRO*/
   BYTE bMaxBindingConditions;                           /* MACRO*/
   unsigned short usMaxBindingTempBuffer;                  /* MACRO*/
   /*
   UNIT_TYPE_STRUCT UnitConfig[NR_OF_UNITS]; 
   STRUCT_UNIT_STATUS UnitStatus[NR_OF_UNITS]; 
   */
} STRUCT_NODE_STATUS;
                                  
void NodeSettings(BYTE);                                  
                                   
/******************************************************************************************************/
/* DIM UNIT*/
#define DIM_UNIT_NR_OFFSET   0
#define DIM_UNIT_NR(a)   a

/* macros for bDimStatus1 */
#define UNIT_DIM_STATE                   0x30
#define DIM_FEEDBACK_ERROR               0x40
#define DIM_UNIT_STATE_MASK 0x03                          
#define DIM_UNIT_OFF_STATE   0  /* Local state machine thread*/
#define DIM_UNIT_ON_STATE    1  /* Local state machine thread*/
#define DIM_UNIT_UP_STATE    2  /* Local state machine thread*/
#define DIM_UNIT_DOWN_STATE  3  /* Local state machine thread*/

#define PREVIOUS_DIM_OFF   0x00  /* Local state machine thread*/
#define PREVIOUS_DIM_ON    0x04  /* Local state machine thread*/
#define PREVIOUS_DIM_UP    0x08  /* Local state machine thread*/
#define PREVIOUS_DIM_DOWN  0x0c  /* Local state machine thread*/

                   

/* Local events for control unit object*/
#define DIM_OFF              0
#define DIM_ON               1  
#define DIM_UP               2
#define DIM_DOWN             3
#define DIM_FEEDBACK_OFF     4
#define DIM_FEEDBACK_ON      5
#define DIM_FEEDBACK_TIMEOUT 6
#define DIM_NO_EVENT         7

#define DIM_PIR_ON 0x10 /* 25102000 */      

/* macros for bDimStatus2                            */
#define DIM_MESSAGE_WAITING 0x80


/* Macros for bDimConfig...*/
#define FEEDBACK_USED 0x01
#define PWMTRIACDRIVE       0x00
#define CONTINUETRIACDRIVE  0x02
#define MASKTRIACDRIVE      0x06

             
typedef struct
{
   BYTE bDimStatus1; /* bit 0..1 : current state : 0=OFF, 1=ON , 2=UP, 3=DN*/
                     /* bit 2..3 : previous state : 0=OFF, 1=ON , 2=UP, 3=DN*/
                     /* bit 4..5 State machine status*/
                     /* bit 6..7 ERROR Code*/

   BYTE bDimStatus2; /* bit 0 : Feedback timer 1=running, 0=stopped */
                     /* bit 1 : Feedback 1=used, 0=unused */
                     /* bit 2 : FeedbackTimer expired  */
                     /* bit 3 : HWPortFeedback (IN)*/
                     /* bit 7 : Message waiting...*/

   BYTE bDimMax;       /* 1..99*/
   BYTE bDimMin;       /* 1..99*/
   BYTE bDimSpeedDown; /* in seconds*/
   BYTE bDimSpeedUp;   /* in seconds*/
   
   BYTE bDimCurrentVal; 
   BYTE bFeedbackTimer;
   BYTE bPWMReload;

   BYTE bDimConfig; /* for the LCD node */


} UNIT_DIM_STRUCT;
/******************************************************************************************************/


/******************************************************************************************************/
/* CONTROL UNIT*/
#define CONTROL_UNIT_NR_OFFSET  (NR_OF_DIM_UNITS+NR_OF_SWITCH_UNITS)
#define CONTROL_UNIT_NR(a)  (NR_OF_DIM_UNITS+NR_OF_SWITCH_UNITS+a)

/* Specific macros*/
#define PULSTOGGLE_RELOAD_VALUE 7 /* 8    +/- 420 ms ATmega603 */
#define CHECK_PULS_RELOAD  17 /* 20    1 sec.                */
#define CHECK_ERROR_RELOAD 1 /* 0.060 sec.                         */
                   
#define UNIT_PULSTOGGLETIMER_RUNNING     0x04
#define UNIT_CONTROLPULSTOGGLE_HIGH      0x02
#define UNIT_CONTROLTOGGLE_HIGH          0x01
#define UNIT_CONTROL_STATE               0x30

#define UNIT_HW_PORT_HIGH                0x01 
#define UNIT_HW_PORT_CHECK_HIGH          0x02 
#define UNIT_CHECK_TIMER_RUNNING         0x04 
#define UNIT_CHECK_ONE_SHOT              0x10
#define UNIT_CHECK_ERROR_CONDITION       0x20
 
#define UNIT_PULSTOGGLETIMER_EXPIRED     0x08

/* Local state machine thread*/
#define CONTROL_UNIT_OPEN_STATE   0x00
#define CONTROL_UNIT_CLOSE_STATE  0x01
#define CONTROL_UNIT_TOGGLE_STATE 0x02

/* Local events for control unit object*/
#define CONTROL_NO_EVENT    0
#define CONTROL_OFF         1
#define CONTROL_ON          2
#define CONTROL_TIMEOUT     3

#define CONTROL_TOGGLE 0
#define CONTROL_PULS 1

#define CONTROL_MESSAGE_WAITING 0x80

typedef struct
{
   BYTE bControlStatus1; /* bit 0 : Toggle status*/
                         /* bit 1 : PulsToggle status*/
                         /* bit 2 : PulsToggle timer 1=running, 0=stopped*/
   
                         /* bit 4..5 State machine status*/
                         /* bit 6..7 ERROR Code*/

   BYTE bControlStatus2; /* bit 0 : HWPortStatus (IN)*/
                         /* bit 1 : HWPortStatus (OUT) (ControlCheck)*/
                         /* bit 2 : ControlCheckLed Timer running    */
                         /* bit 3 : PulsToggleTimer expired*/
                         /* bit 4 : ControlCheckLed ONE_SHOT*/
                         /* bit 5 : ERRORCondition*/
                         /* bit 7 : Message waiting...*/
                           

   BYTE bPulsToggleTimer;
   BYTE bCheckTimer;

   BYTE bControlConfig; /* for the LCD node */
   
} UNIT_CONTROL_STRUCT;

void FindControlBindingToForward(BYTE,BYTE,BYTE);
/* 20092000 */
BYTE CheckControlObjectEvent(void); 

/******************************************************************************************************/


/******************************************************************************************************/
/* SWITCH UNIT*/
#define SWITCH_UNIT_NR_OFFSET   (NR_OF_DIM_UNITS)
#define SWITCH_UNIT_NR(a)   (NR_OF_DIM_UNITS+a)


/* macros for bSwitchStatus1*/
#define UNIT_SWITCH_STATE      0x30
#define SWITCH_FEEDBACK_ERROR  0x40
                      
#define SWITCH_UNIT_STATE_MASK 0x01                       
#define SWITCH_UNIT_OFF_STATE  0x00  /* Local state machine thread current state*/
#define SWITCH_UNIT_ON_STATE   0x01  /* Local state machine thread current state*/

#define PREVIOUS_SWITCH_OFF    0x00  /* Local state machine thread previous state*/
#define PREVIOUS_SWITCH_ON     0x02  /* Local state machine thread previous state*/

#define SWITCH_PIR_ON 0x10 /* 25102000 */

/* macros for bSwitchStatus2                      */
#define SWITCH_MESSAGE_WAITING 0x80
 
/* Local events for switch unit object state machine.*/
#define SWITCH_OFF              0
#define SWITCH_ON               1  
#define SWITCH_FEEDBACK_OFF     2
#define SWITCH_FEEDBACK_ON      3
#define SWITCH_FEEDBACK_TIMEOUT 4
#define SWITCH_NO_EVENT         5


typedef struct
{
   
   BYTE bSwitchStatus1; /* bit 0 : current state : 0=OFF, 1=ON*/
                        /* bit 1 : previous state : 0=OFF, 1=ON*/
                        /* bit 4..5 State machine status*/
                        /* bit 6..7 ERROR Code*/

   BYTE bSwitchStatus2; /* bit 0 : Feedback timer 1=running, 0=stopped */
                        /* bit 1 : Feedback 1=used, 0=unused */
                        /* bit 2 : FeedbackTimer expired  */
                        /* bit 3 : HWPortFeedback (IN)*/
                        /* bit 7 : Message waiting...*/
   BYTE bFeedbackTimer;

   BYTE bSwitchConfig; /* for the LCD node */
   BYTE bSwitchStopTimerReloadValue;

} UNIT_SWITCH_STRUCT;
/******************************************************************************************************/

/******************************************************************************************************/
/* SENS UNIT*/
#define SENS_UNIT_NR_OFFSET (NR_OF_DIM_UNITS+NR_OF_SWITCH_UNITS+NR_OF_CONTROL_UNITS)
#define SENS_UNIT_NR(a)   (NR_OF_DIM_UNITS+NR_OF_SWITCH_UNITS+NR_OF_CONTROL_UNITS+a)

#define SENS_HEATING_ON  0
#define SENS_HEATING_OFF 1
#define SENS_COOLING_ON  2
#define SENS_COOLING_OFF 3


/* Macros for bSensStatus1...      */
#define UNIT_SENS_STATE                  0x30
 
#define STATE_SENS_STABLE                0x00   /* Local state machine state.*/
#define STATE_SENS_HIGH_CROSS            0x01   /* Local state machine state.*/
#define STATE_SENS_LOW_CROSS             0x02   /* Local state machine state.*/
/*#define STATE_SENS_HYST_ZONE             0x03      Local state machine state.*/

#define STATE_SENS_PREVIOUS_STABLE       0x00   /* Local state machine state.*/
#define STATE_SENS_PREVIOUS_HIGH_CROSS   0x04   /* Local state machine state.*/
#define STATE_SENS_PREVIOUS_LOW_CROSS    0x08   /* Local state machine state.*/
/*#define STATE_SENS_PREVIOUS_HYST_ZONE    0x0C      Local state machine state.*/

#define EV_SENS_STABLE                   0x00   /* Local state machine event.*/
#define EV_SENS_HIGH                     0x01   /* Local state machine event.*/
#define EV_SENS_LOW                      0x02   /* Local state machine event.*/
#define EV_SENS_CONTROL_OFF              0x03   /* Local state machine event.*/
#define EV_SENS_CONTROL_ON               0x04   /* Local state machine event.*/
#define EV_SENS_HYST_LOW                 0x05   /* Local state machine event.*/
#define EV_SENS_HYST_HIGH                0x06   /* Local state machine event.*/
#define EV_SENS_NO_EVENT                 0x07   /* Local state machine event. */
                              
/* Macros for bSensStatus2...*/
#define SENS_MESSAGE_WAITING 0x80
#define SENS_SAMPLE_TAKEN    0x01  /* bit 0*/
#define SENS_READY_TO_SAMPLE 0x02  /* bit 1*/
#define SENS_CONTROL_ON      0x10  /* bit 4*/

#define SENS_DRIVE_OFF 0                      
#define SENS_DRIVE_ON  1
#define SENS_DRIVE_NO_CHANGE 2

#define SENS_PRESET_MASK 0x60
#define SENS_PRESET_SUN      0x00
#define SENS_PRESET_HALFSUN  0x20
#define SENS_PRESET_MOON     0x40
#define SENS_PRESET_HALFMOON 0x60

/* Macro's for bSensConfig to check lookup table...                                                      */
#define SENS_CONFIG_TEMPERATURE    0x00
#define SENS_CONFIG_PH             0x01
#define SENS_CONFIG_LUX            0x02
#define SENS_CONFIG_AMPERE         0x03

/* TM20110319: Temperatuur waarden Max en Min veranderd */
#define TEMP_MAX	1250				/* was 510 */
#define TEMP_MIN	-400				/* was -250 */

// Standaard waarden voor beperken van de setpoint waaden.
#define SENS_DEFAULT_SP_RANGE_MIN	 	120			// 12°C
#define SENS_DEFAULT_SP_RANGE_MAX		300			// 30°C

#define SENS_TEMP_OFFSETVALUE_UNSUPPORTED	0x7FFF		/* TM20120405 */
#define SENS_TEMP_NTCVERSION_UNSUPPORTED	0xFFFF		/* TM20120405 */

typedef struct
{
   BYTE bSensStatus1; /* bit 0..1 Current state*/
                      /* bit 2..3 Previous state*/
                      /* bit 4..5 State machine status*/
                      /* bit 6..7 ERROR code*/

   BYTE bSensStatus2;  /* bit 0..2 Sample flags*/
                       /* bit 4 : Sens control ON/OFF*/
                       /* bit 5..6 = 00 sun 01 halfsun 10 moon 11 halfmoon*/
                       /* bit 7 : Message waiting...                   */
                 
   BYTE bSensInterval;     /* In seconds*/

   /* SENS_TEMPERATURE : 1/10 C*/
   /* SENS_CURRENT : 1/100 Amp*/
   /* Relative Neg to the sSensSetPointValue.*/
   BYTE bSensNegHystValue[4]; /* Unit specific data, see command/event description.*/
   /* Relative Neg to the bSensNegHystValue*/
   BYTE bSensNegHystOffset; /* Unit specific data, see command/event description.   */
   /* Relative Pos to the sSensSetPointValue.      */
   BYTE bSensPosHystValue[4]; /* Unit specific data, see command/event description.   */
   /* Relative Pos to the bSensPosHystValue*/
   BYTE bSensPosHystOffset; /* Unit specific data, see command/event description.*/

   /* SENS_TEMPERATURE : 1/10 C*/
   /* SENS_CURRENT : 1/100 Amp*/
   short sSensSetPointValue[4]; /* Unit specific data, see command/event description.   */
   
   /* SENS_TEMPERATURE : 1/10 C*/
   /* SENS_CURRENT : 1/100 Amp (=mA)*/
   short sSensCurrentValue; /* Unit specific data, see command/event description.                          */
   short sSensPreviousValue; /* Unit specific data, see command/event description.                                                     */
       
   /* SENS_TEMPERATURE : look-up table*/
   /* SENS_CURRENT : y=(dy/dx)x+offset --> x=(y-offset)/(dy/dx)*/
   unsigned short usCurrentADCValue;
   
   BYTE bPreviousLocalEvent;
   BYTE bSensConfig; /* for the LCD node */

   /* Sedert v0x0600 */
   short sMeasuredValueOffset;			// TM20120404: Niet in de LCD nodes
   unsigned short usNTCVersionInfo;		// TM20120404: Niet in de LCD en STD nodes.

      
} UNIT_SENS_STRUCT;


/******************************************************************************************************/

/******************************************************************************************************/
/*  UNIT LCD*/
#define LCD_UNIT_NR_OFFSET   (NR_OF_DIM_UNITS+NR_OF_SWITCH_UNITS+NR_OF_CONTROL_UNITS+\
                             NR_OF_SENS_UNITS+NR_OF_AUDIO_UNITS)
#define LCD_UNIT_NR(a)   (NR_OF_DIM_UNITS+NR_OF_SWITCH_UNITS+NR_OF_CONTROL_UNITS+\
                         NR_OF_SENS_UNITS+NR_OF_AUDIO_UNITS+a)
/* Macros for bLCDStatus1...      */
#define UNIT_LCD_STATE     0x30  /* 4 possible states is enough ??*/
/* States for local statemachine...        */
#define LCD_UNKNOWN_STATE 255
/* Macros for bLCDStatus2...*/
#define LCD_MESSAGE_WAITING 0x80
/******************************************************************************************************/
                             
/******************************************************************************************************/
/*  UNIT  LCD_VIRTUAL*/
#define LCD_VIRTUAL_UNIT_NR_OFFSET   (NR_OF_DIM_UNITS+NR_OF_SWITCH_UNITS+NR_OF_CONTROL_UNITS+\
                                     NR_OF_SENS_UNITS+NR_OF_AUDIO_UNITS+NR_OF_LCD_UNITS)
#define LCD_VIRTUAL_UNIT_NR(a)   (NR_OF_DIM_UNITS+NR_OF_SWITCH_UNITS+NR_OF_CONTROL_UNITS+\
                                 NR_OF_SENS_UNITS+NR_OF_AUDIO_UNITS+NR_OF_LCD_UNITS+a)
/******************************************************************************************************/

/******************************************************************************************************/
/*  UNIT AUDIO*/


#define BOSE_LIFESTYLE_SYSTEM50		0x00	
#define BOSE_LIFESTYLE_SYSTEM48		0x80	
#define BOSE_LIFESTYLE_SYSTEMV35	0x40

#define NR_BOSE_LF50_ROOMS		4
#define NR_BOSE_LF48_ROOMS		14
#define NR_BOSE_V35_ROOMS		14



#define AUDIO_UNIT_NR_OFFSET  (NR_OF_DIM_UNITS+NR_OF_SWITCH_UNITS+NR_OF_CONTROL_UNITS+\
                              NR_OF_SENS_UNITS)

#define AUDIO_UNIT_NR(a)   (NR_OF_DIM_UNITS+NR_OF_SWITCH_UNITS+NR_OF_CONTROL_UNITS+\
                           NR_OF_SENS_UNITS+a)


#ifdef IR_AUDIO
/* macros for bDimStatus1 */
#define UNIT_AUDIO_STATE      0x30
#define AUDIO_UNIT_STATE_MASK 0x03                          
#define AUDIO_UNIT_PREVIOUS_STATE_MASK 0x0C                          

#define AUDIO_UNIT_OFF_STATE      0x00  /* Local state machine thread*/
#define AUDIO_UNIT_ON_STATE       0x01  /* Local state machine thread*/
#define AUDIO_UNIT_VOLUP_STATE    0x02  /* Local state machine thread*/
#define AUDIO_UNIT_VOLDOWN_STATE  0x03  /* Local state machine thread*/

#define PREVIOUS_AUDIO_OFF        0x00  /* Local state machine thread*/
#define PREVIOUS_AUDIO_ON         0x04  /* Local state machine thread*/
#define PREVIOUS_AUDIO_VOLUP      0x08  /* Local state machine thread*/
#define PREVIOUS_AUDIO_VOLDOWN    0x0C  /* Local state machine thread*/

/* Local events for control unit object*/
#define AUDIO_OFF              0
#define AUDIO_ON               1  
#define AUDIO_SOURCE_SELECT    2
#define AUDIO_VOLUME_UP        3
#define AUDIO_VOLUME_DOWN      4
#define AUDIO_NO_EVENT         5

/* Macros for bAudioStatus2...*/
#define AUDIO_MESSAGE_WAITING 0x80
#define AUDIO_UNIT_ON 0x40
#define AUDIO_SOURCESELECT_MASK 0x0F

#endif /* IR_AUDIO */

/******************************** MATRIX AUDIO *************************************/
#ifdef MATRIX_AUDIO

/* macros for bCurrentState & bPreviousState */
#define AUDIO_UNIT_STATE_MASK 0x03                          

#define AUDIO_UNIT_OFF_STATE      0x00  /* Local state machine thread*/
#define AUDIO_UNIT_ON_STATE       0x01  /* Local state machine thread*/
#define AUDIO_UNIT_VOLUP_STATE    0x02  /* Local state machine thread*/
#define AUDIO_UNIT_VOLDOWN_STATE  0x03  /* Local state machine thread*/

#define AUDIO_UNIT_DESTINATIONSONOFFMASK 0x0f

/* Local events for control unit object*/
#define AUDIO_OFF              0
#define AUDIO_ON               1  
#define AUDIO_SOURCE_SELECT    2
#define AUDIO_VOLUME_UP        3
#define AUDIO_VOLUME_DOWN      4
#define AUDIO_NO_EVENT         5

/* Macros for bAudioStatus2...*/
#define AUDIO_MESSAGE_WAITING 0x80
#endif /********************************* MATRIX AUDIO ************************************/



/******************************************************************************************************/

/* LCD objects in unitvlcd.h ...*/
                                                         
/******************************************************************************************************/
/*  UNIT DUOSWITCH*/

#define DUOSWITCH_UNIT_NR_OFFSET   (NR_OF_DIM_UNITS+NR_OF_SWITCH_UNITS+NR_OF_CONTROL_UNITS+\
                                      NR_OF_SENS_UNITS+NR_OF_AUDIO_UNITS+NR_OF_LCD_UNITS+\
                                      NR_OF_LCD_VIRTUAL_UNITS)

#define DUOSWITCH_UNIT_NR(a)  (NR_OF_DIM_UNITS+NR_OF_SWITCH_UNITS+NR_OF_CONTROL_UNITS+\
                                 NR_OF_SENS_UNITS+NR_OF_AUDIO_UNITS+NR_OF_LCD_UNITS+\
                                 NR_OF_LCD_VIRTUAL_UNITS+a)
 
/* Macros for bDuoSwitchStatus2...*/
#define DUOSWITCH_MESSAGE_WAITING 0x80
#define DUOSWITCH_STOPTIMER_EXPIRED   0x01
#define DUOSWITCH_SWITCHTIMER_EXPIRED 0x02
#define DUOSWITCH_STOPTIMER_RUNNING   0x04
#define DUOSWITCH_SWITCHTIMER_RUNNING 0x08

/* Macros for bDuoSwitchStatus1... */
#define DUOSWITCH_UNIT_STATE_MASK      0x07                       
#define DUOSWITCH_UNIT_STOP_STATE      0x00
#define DUOSWITCH_UNIT_WAIT_UP_STATE   0x01 
#define DUOSWITCH_UNIT_WAIT_DOWN_STATE 0x02 
#define DUOSWITCH_UNIT_UP_STATE        0x03 
#define DUOSWITCH_UNIT_DOWN_STATE      0x04
#define DUOSWITCH_UNIT_PREVIOUS_STATE_MASK      0x38                        
#define DUOSWITCH_UNIT_STOP_PREVIOUS_STATE      0x00
#define DUOSWITCH_UNIT_WAIT_UP_PREVIOUS_STATE   0x08 
#define DUOSWITCH_UNIT_WAIT_DOWN_PREVIOUS_STATE 0x10 
#define DUOSWITCH_UNIT_UP_PREVIOUS_STATE        0x18 
#define DUOSWITCH_UNIT_DOWN_PREVIOUS_STATE      0x20
 
#define DUOSWITCH_UNIT_MANUAL_STOP 0x40 
 
/* Local events for switch unit object state machine.*/
#define EV_DUOSWITCH_NO_EVENT          0
#define EV_DUOSWITCH_STOP              1
#define EV_DUOSWITCH_UP                2  
#define EV_DUOSWITCH_DOWN              3
#define EV_DUOSWITCH_SWITCHTIMEOUT     4
#define EV_DUOSWITCH_STOPTIMEOUT       5
 
typedef struct
{
   BYTE bDuoSwitchStatus1; /* bit 0..2 : current state 
                            bit 3..5 : previous state
                            bit 6..7 ERROR Code */

   BYTE bDuoSwitchStatus2; /* bit 0..3 timer flags (stop & switch timer) 
                            bit 7 : Message waiting...*/

   unsigned short usStopTimer;     /* To transit ftom the UP/DOWN state go to the STOP state after some time !!*/
   BYTE bSwitchTimer;   /*To prevent the two relays from being on together !!*/


   BYTE bDuoSwitchConfig;
   BYTE bDuoSwitchStopTimerReloadValue;
   BYTE bDuoSwitchSwitchTimerReloadValue;                              

} UNIT_DUOSWITCH_STRUCT;
/******************************************************************************************************/


/******************************************************************************************************/
/*  UNIT SELECTOR*/
#define SELECTOR_UNIT_NR_OFFSET   (NR_OF_DIM_UNITS+NR_OF_SWITCH_UNITS+NR_OF_CONTROL_UNITS+\
                                  NR_OF_SENS_UNITS+NR_OF_AUDIO_UNITS+NR_OF_LCD_UNITS+\
                                  NR_OF_LCD_VIRTUAL_UNITS+NR_OF_DUOSWITCH_UNITS)
#define SELECTOR_UNIT_NR(a)   (NR_OF_DIM_UNITS+NR_OF_SWITCH_UNITS+NR_OF_CONTROL_UNITS+\
                              NR_OF_SENS_UNITS+NR_OF_AUDIO_UNITS+NR_OF_LCD_UNITS+\
                              NR_OF_LCD_VIRTUAL_UNITS+NR_OF_DUOSWITCH_UNITS+a)
/******************************************************************************************************/


/******************************************************************************************************/
/*  UNIT BOSERS*/

#define BOSERS_UNIT_NR_OFFSET   (NR_OF_DIM_UNITS+NR_OF_SWITCH_UNITS+NR_OF_CONTROL_UNITS+\
                                  NR_OF_SENS_UNITS+NR_OF_AUDIO_UNITS+NR_OF_LCD_UNITS+\
                                  NR_OF_LCD_VIRTUAL_UNITS+NR_OF_DUOSWITCH_UNITS+NR_OF_SELECTOR_UNITS)

#define BOSERS_UNIT_NR(a)   (NR_OF_DIM_UNITS+NR_OF_SWITCH_UNITS+NR_OF_CONTROL_UNITS+\
                              NR_OF_SENS_UNITS+NR_OF_AUDIO_UNITS+NR_OF_LCD_UNITS+\
                              NR_OF_LCD_VIRTUAL_UNITS+NR_OF_DUOSWITCH_UNITS+NR_OF_SELECTOR_UNITS+a)

/******************************************************************************************************/


/* 14082000 speechcomm & videoswitch */
/****************************************************************************************************/
#define VIDEOSWITCHIN_UNIT_NR_OFFSET   (NR_OF_DIM_UNITS+NR_OF_SWITCH_UNITS+NR_OF_CONTROL_UNITS+\
                               NR_OF_SENS_UNITS+NR_OF_AUDIO_UNITS+NR_OF_LCD_UNITS+\
                               NR_OF_LCD_VIRTUAL_UNITS+NR_OF_DUOSWITCH_UNITS+NR_OF_SELECTOR_UNITS+\
                               NR_OF_BOSERS_UNITS+NR_OF_AVIR_UNITS+NR_OF_IRTX_UNITS+NR_OF_RC5RX_UNITS)

                         
#define VIDEOSWITCHIN_UNIT_NR(a)   (NR_OF_DIM_UNITS+NR_OF_SWITCH_UNITS+NR_OF_CONTROL_UNITS+\
                              NR_OF_SENS_UNITS+NR_OF_AUDIO_UNITS+NR_OF_LCD_UNITS+\
                              NR_OF_LCD_VIRTUAL_UNITS+NR_OF_DUOSWITCH_UNITS+NR_OF_SELECTOR_UNITS+\
                              NR_OF_BOSERS_UNITS+NR_OF_AVIR_UNITS+NR_OF_IRTX_UNITS+NR_OF_RC5RX_UNITS+a)



/* macros for bVideoSwitchInStatus1*/
#define VIDEOSWITCHIN_UNIT_STATE_MASK 0x01
#define VIDEOSWITCHIN_UNIT_PREVIOUS_STATE_MASK 0x02
#define VIDEOSWITCHIN_UNIT_STATE_NR_OF_BITS 1
#define VIDEOSWITCHIN_UNIT_OFF_STATE  0x00  /* Local state machine thread current state*/
#define VIDEOSWITCHIN_UNIT_ON_STATE   0x01  /* Local state machine thread current state*/
#define PREVIOUS_VIDEOSWITCHIN_OFF    0x00  /* Local state machine thread previous state*/
#define PREVIOUS_VIDEOSWITCHIN_ON     0x02  /* Local state machine thread previous state*/
 
/* macros for bVideoSwitchInStatus2*/
#define VIDEOSWITCHIN_MESSAGE_WAITING 0x80

/* Local events for switch unit object state machine.*/
#define VIDEOSWITCHIN_OFF              0
#define VIDEOSWITCHIN_ON               1  
#define VIDEOSWITCHIN_NO_EVENT         2


/* macros for bVideoSwitchInConfig */
#define VIDEOSWITCHIN_POWER_ALWAYS_ON 0x01 

typedef struct
{
   BYTE bVideoSwitchInConfig; /* bit 0 : power coltrolled(0)/always on(1) (default, controlled) */

   BYTE bVideoSwitchInStatus1; /* bit 0 : current state : 0=OFF, 1=ON*/
                               /* bit 1 : previous state : 0=OFF, 1=ON*/
        

   BYTE bVideoSwitchInStatus2;
                      /* bit 7 : Message waiting...*/
} UNIT_VIDEOSWITCHIN_STRUCT;

/******************************************************************************************************/


/* 14082000 speechcomm & videoswitch */
/* VIDEOSWITCHOUT UNIT  */
#define VIDEOSWITCHOUT_UNIT_NR_OFFSET   (NR_OF_DIM_UNITS+NR_OF_SWITCH_UNITS+NR_OF_CONTROL_UNITS+\
                               NR_OF_SENS_UNITS+NR_OF_AUDIO_UNITS+NR_OF_LCD_UNITS+\
                               NR_OF_LCD_VIRTUAL_UNITS+NR_OF_DUOSWITCH_UNITS+NR_OF_SELECTOR_UNITS+\
                               NR_OF_BOSERS_UNITS+NR_OF_AVIR_UNITS+NR_OF_IRTX_UNITS+NR_OF_RC5RX_UNITS+\
                               NR_OF_VIDEOSWITCHIN_UNITS)

                         
#define VIDEOSWITCHOUT_UNIT_NR(a)   (NR_OF_DIM_UNITS+NR_OF_SWITCH_UNITS+NR_OF_CONTROL_UNITS+\
                              NR_OF_SENS_UNITS+NR_OF_AUDIO_UNITS+NR_OF_LCD_UNITS+\
                              NR_OF_LCD_VIRTUAL_UNITS+NR_OF_DUOSWITCH_UNITS+NR_OF_SELECTOR_UNITS+\
                              NR_OF_BOSERS_UNITS+NR_OF_AVIR_UNITS+NR_OF_IRTX_UNITS+NR_OF_RC5RX_UNITS+\
                              NR_OF_VIDEOSWITCHIN_UNITS+a)

/* macros for bVideoSwitchInStatus1*/
#define VIDEOSWITCHOUT_UNIT_STATE_MASK 0x01
#define VIDEOSWITCHOUT_UNIT_PREVIOUS_STATE_MASK 0x02
#define VIDEOSWITCHOUT_UNIT_STATE_NR_OF_BITS 1
#define VIDEOSWITCHOUT_UNIT_OFF_STATE  0x00  /* Local state machine thread current state*/
#define VIDEOSWITCHOUT_UNIT_ON_STATE   0x10  /* Local state machine thread current state*/
#define PREVIOUS_VIDEOSWITCHOUT_OFF    0x00  /* Local state machine thread previous state*/
#define PREVIOUS_VIDEOSWITCHOUT_ON     0x20  /* Local state machine thread previous state*/
 
/* macros for bVideoSwitchInStatus2*/
#define VIDEOSWITCHOUT_MESSAGE_WAITING 0x80

/* Local events for switch unit object state machine.*/
#define VIDEOSWITCHOUT_OFF              0
#define VIDEOSWITCHOUT_ON               1  
#define VIDEOSWITCHOUT_NO_EVENT         2

/* macros for bVideoSwitchOutConfig */
#define VIDEOSWITCHOUT_POWER_ALWAYS_ON 0x01 


typedef struct
{
   BYTE bVideoSwitchOutConfig; /* bit 0 : power coltrolled(0)/always on(1) (default, controlled) */

   BYTE bVideoSwitchOutStatus1;
                      /* bit 4 State machine status*/
                      /* bit 5 Previous State*/
   BYTE bVideoSwitchOutStatus2;
                      /* bit 7 : Message waiting...*/
} UNIT_VIDEOSWITCHOUT_STRUCT;
                
/******************************************************************************************************/

/* 14082000 speechcomm & videoswitch */
/* SPEECHCOMM UNIT  */
#define SPEECHCOMM_UNIT_NR_OFFSET   (NR_OF_DIM_UNITS+NR_OF_SWITCH_UNITS+NR_OF_CONTROL_UNITS+\
                               NR_OF_SENS_UNITS+NR_OF_AUDIO_UNITS+NR_OF_LCD_UNITS+\
                               NR_OF_LCD_VIRTUAL_UNITS+NR_OF_DUOSWITCH_UNITS+NR_OF_SELECTOR_UNITS+\
                               NR_OF_BOSERS_UNITS+NR_OF_AVIR_UNITS+NR_OF_IRTX_UNITS+NR_OF_RC5RX_UNITS+\
                               NR_OF_VIDEOSWITCHIN_UNITS+NR_OF_VIDEOSWITCHOUT_UNITS)

                         
#define SPEECHCOMM_UNIT_NR(a)   (NR_OF_DIM_UNITS+NR_OF_SWITCH_UNITS+NR_OF_CONTROL_UNITS+\
                              NR_OF_SENS_UNITS+NR_OF_AUDIO_UNITS+NR_OF_LCD_UNITS+\
                              NR_OF_LCD_VIRTUAL_UNITS+NR_OF_DUOSWITCH_UNITS+NR_OF_SELECTOR_UNITS+\
                              NR_OF_BOSERS_UNITS+NR_OF_AVIR_UNITS+NR_OF_IRTX_UNITS+NR_OF_RC5RX_UNITS+\
                              NR_OF_VIDEOSWITCHIN_UNITS+NR_OF_VIDEOSWITCHOUT_UNITS+a)

/* macros for bUpStream and bDownStream */
#define SPEECHCOMM_NO_STREAM 255
/* macros for bUpStream and bRingType */
#define SPEECHCOMM_NO_RING 255

/* macros for bSpeechCommStatus1 */
#define SPEECHCOMM_FLAGS_MASK 0x07  /* bit 2..0 */
#define SPEECHCOMM_CALLREQUEST_RING 0x01
#define SPEECHCOMM_UPSTREAM_CONNECTED 0x02
#define SPEECHCOMM_DOWNSTREAM_CONNECTED 0x04

/* states */
#define SPEECHCOMM_STATE_MASK 0x30  /* bit 5..4 */
#define SPEECHCOMM_PREVIOUS_STATE_MASK 0xC0  /* bit 7..6 */
#define SPEECHCOMM_STATE_NR_OF_BITS 2

#define STATE_SPEECHCOMM_IDLE    0x00
#define STATE_SPEECHCOMM_PULS    0x10
#define STATE_SPEECHCOMM_TOGGLE  0x20

#define STATE_PREVIOUS_SPEECHCOMM_IDLE    0x00
#define STATE_PREVIOUS_SPEECHCOMM_PULS    0x40
#define STATE_PREVIOUS_SPEECHCOMM_TOGGLE  0x80

/* events */
#define SPEECHCOMM_NO_EVENT 0
#define SPEECHCOMM_CONTROL_ON 1
#define SPEECHCOMM_CONTROL_OFF 2
#define SPEECHCOMM_PULSTOGGLE_TIMEOUT 3

#define SPEECHCOMM_CONTROL_TOGGLE 0
#define SPEECHCOMM_CONTROL_PULS 1


/* macros for bSpeechCommStatus2 */
#define SPEECHCOMM_MESSAGE_WAITING 0x80
/* speechcomm toggle timer */
#define SPEECHCOMM_PULSTOGGLE_TIMER_EXPIRED 0x02
#define SPEECHCOMM_PULSTOGGLE_TIMER_RUNNING 0x01
#define SPEECHCOMM_PULSTOGGLE_TIMER_RELOAD_VALUE 7

#define SPEECHCOMM_HW_INPUT_BIT 0x04

/* macros for bSpeechCommConfig */
#define SPEECHCOMM_TYPE_MASK       0x03 
#define SPEECHCOMM_INDOOR          0x00
#define SPEECHCOMM_OUTDOOR         0x01
#define SPEECHCOMM_RINGER_DISABLED 0x04
#define SPEECHCOMM_ACCEPT_DISABLED 0x08


typedef struct
{

   BYTE bSpeechCommConfig; /* bit 1..0 : indoor unit(0x00 default), outdoor unit(0x01) */
                           /* bit 2 : ringer enabled(0 default)/disabled(1)*/
                           /* bit 3 : accept enabled(0 default)/disabled(1)*/

   BYTE bSpeechCommStatus1; /* bit 0 : call request(outdoor) / incoming call, ring(indoor)*/
                            /* bit 1 : upstream connected(1)/disconnected(0)*/
                            /* bit 2 : downstream connected(1)/disconnected(0)*/
                            
                            /* bit 4..5 Current */
                            /* bit 6..7 Previous state*/
                            
   BYTE bSpeechCommStatus2;  /* bit 0 : speechcomm toggle timer running*/
                             /* bit 1 : speechcomm toggle timer expired*/
                             /* bit 2 : control HW bit*/
                             /* bit 7 : Message waiting...*/
   
   BYTE bSpeechCommPulsToggleTimer;
   
   BYTE bUpStreamID;    /* 255 = SPEECHCOMM_NO_STREAM */
   BYTE bDownStreamID;  /* 255 = SPEECHCOMM_NO_STREAM */
   BYTE bRingType;    /* 255 = SPEECHCOMM_NO_RING */
   BYTE bVideoSwitchLinkUnitNR; /* 255 = UNIT_BROADCAST_ADDRESS, means no link ! */
   BYTE bVolume;
      
} UNIT_SPEECHCOMM_STRUCT;



////////////////////////////////// TEMP ???????? /////////////////////////////////////////



//****************************************************************************************************
//  UNIT AUDIO
#define AUDIO_UNIT_NR_OFFSET  (NR_OF_DIM_UNITS+NR_OF_SWITCH_UNITS+NR_OF_CONTROL_UNITS+\
                              NR_OF_SENS_UNITS)

#define AUDIO_UNIT_NR(a)   (NR_OF_DIM_UNITS+NR_OF_SWITCH_UNITS+NR_OF_CONTROL_UNITS+\
                           NR_OF_SENS_UNITS+a)

/* macros for bDimStatus1 */
#define UNIT_AUDIO_STATE      0x30
#define AUDIO_UNIT_STATE_MASK 0x03                          
#define AUDIO_UNIT_PREVIOUS_STATE_MASK 0x0C                          

#define AUDIO_UNIT_OFF_STATE      0x00  /* Local state machine thread*/
#define AUDIO_UNIT_ON_STATE       0x01  /* Local state machine thread*/
#define AUDIO_UNIT_VOLUP_STATE    0x02  /* Local state machine thread*/
#define AUDIO_UNIT_VOLDOWN_STATE  0x03  /* Local state machine thread*/

#define PREVIOUS_AUDIO_OFF        0x00  /* Local state machine thread*/
#define PREVIOUS_AUDIO_ON         0x04  /* Local state machine thread*/
#define PREVIOUS_AUDIO_VOLUP      0x08  /* Local state machine thread*/
#define PREVIOUS_AUDIO_VOLDOWN    0x0C  /* Local state machine thread*/

/* Local events for control unit object*/
#define AUDIO_OFF              0
#define AUDIO_ON               1  
#define AUDIO_SOURCE_SELECT    2
#define AUDIO_VOLUME_UP        3
#define AUDIO_VOLUME_DOWN      4
#define AUDIO_NO_EVENT         5

/* Macros for bAudioStatus2...*/
#define AUDIO_MESSAGE_WAITING 0x80
#define AUDIO_UNIT_ON 0x40
#define AUDIO_SOURCESELECT_MASK 0x0F

/* HC4 audio structure */
#define MAX_AUDIO_UNIT_SOURCES 8
#define MAX_AUDIO_UNIT_DESTINATIONS 4

#define AUDIO_TYPE_CONFIG_DEFAULT      0x00	/* TM_AUDIOHC4_BNO */
#define AUDIO_TYPE_CONFIG_BNO		   0x01	/* TM_AUDIOHC4_BNO */
#define AUDIO_TYPE_CONFIG_MASK         0x07	/* TM_AUDIOHC4_BNO */


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

} UNIT_AUDIO_STRUCT;

//****************************************************************************************************
// RC5RX UNIT  

#define RC5RX_UNIT_NR_OFFSET   (NR_OF_DIM_UNITS+NR_OF_SWITCH_UNITS+NR_OF_CONTROL_UNITS+\
                               NR_OF_SENS_UNITS+NR_OF_AUDIO_UNITS+NR_OF_LCD_UNITS+\
                               NR_OF_LCD_VIRTUAL_UNITS+NR_OF_DUOSWITCH_UNITS+NR_OF_SELECTOR_UNITS+\
                               NR_OF_BOSERS_UNITS+NR_OF_AVIR_UNITS+NR_OF_IRTX_UNITS)

                         
#define RC5RX_UNIT_NR(a)   (NR_OF_DIM_UNITS+NR_OF_SWITCH_UNITS+NR_OF_CONTROL_UNITS+\
                              NR_OF_SENS_UNITS+NR_OF_AUDIO_UNITS+NR_OF_LCD_UNITS+\
                              NR_OF_LCD_VIRTUAL_UNITS+NR_OF_DUOSWITCH_UNITS+NR_OF_SELECTOR_UNITS+\
                              NR_OF_BOSERS_UNITS+NR_OF_AVIR_UNITS+NR_OF_IRTX_UNITS+a)
// Specific macros
#define RC5RX_PULSTOGGLE_RELOAD_VALUE 7 // 8 // +/- 420 ms ATmega603 
#define CHECK_PULS_RELOAD  17 // 20 // 1 sec.                
#define CHECK_ERROR_RELOAD 1 // 0.060 sec.                         
                   
#define UNIT_PULSTOGGLETIMER_RUNNING     0x04
#define UNIT_RC5RXPULSTOGGLE_HIGH      0x02
#define UNIT_RC5RXTOGGLE_HIGH          0x01
#define UNIT_RC5RX_STATE               0x30

#define UNIT_HW_PORT_HIGH                0x01 
#define UNIT_HW_PORT_CHECK_HIGH          0x02 
#define UNIT_CHECK_TIMER_RUNNING         0x04 
#define UNIT_CHECK_ONE_SHOT              0x10
#define UNIT_CHECK_ERROR_CONDITION       0x20
 
#define UNIT_PULSTOGGLETIMER_EXPIRED     0x08

// Local state machine thread
#define RC5RX_UNIT_IDLE_STATE   0x00

// Local events for control unit object
#define RC5RX_NO_EVENT    0
#define RC5RX_EVENT         1

#define RC5RX_TOGGLE 0
#define RC5RX_PULS 1

#define RC5RX_MESSAGE_WAITING 0x80

#define MAX_RC5_COMMANDS 64 // divideable by 8 !!! per 8 bits


#define NORMAL_RC5_IR 0       // default !!
#define SONY_IR       1

typedef struct
{
   BYTE bRC5RxStatus1; // bit 0 : Toggle status
                      // bit 4..5 State machine status
                      // bit 6..7 ERROR Code

   BYTE bRC5RxStatus2; // bit 0 : HWPortStatus (IN)
                      // bit 1 : HWPortStatus (OUT) (RC5RxCheck)
                      // bit 2 : RC5RxCheckLed Timer running    
                      // bit 3 : PulsToggleTimer expired
                      // bit 4 : RC5RxCheckLed ONE_SHOT
                      // bit 5 : ERRORCondition
                      // bit 7 : Message waiting...
   
   BYTE bRC5CommandStatus[MAX_RC5_COMMANDS/8]; // 64 bits 
                           
   BYTE bPulsToggleTimer;

   BYTE bRC5RxConfig;

} UNIT_RC5RX_STRUCT;


//****************************************************************************************************

// TM_CHANGES_ALARM_ARITECH : Alarm unit info toegevoegd.
/*********************************************** ALARM UNIT *******************************************/
#define ALARM_UNIT_NR_OFFSET    (NR_OF_DIM_UNITS+NR_OF_SWITCH_UNITS+NR_OF_CONTROL_UNITS+\
                               NR_OF_SENS_UNITS+NR_OF_AUDIO_UNITS+NR_OF_LCD_UNITS+\
                               NR_OF_LCD_VIRTUAL_UNITS+NR_OF_DUOSWITCH_UNITS+NR_OF_SELECTOR_UNITS+\
                               NR_OF_BOSERS_UNITS+NR_OF_AVIR_UNITS+NR_OF_IRTX_UNITS+NR_OF_RC5_UNITS)

#define ALARM_UNIT_NR(a)   (NR_OF_DIM_UNITS+NR_OF_SWITCH_UNITS+NR_OF_CONTROL_UNITS+\
                              NR_OF_SENS_UNITS+NR_OF_AUDIO_UNITS+NR_OF_LCD_UNITS+\
                              NR_OF_LCD_VIRTUAL_UNITS+NR_OF_DUOSWITCH_UNITS+NR_OF_SELECTOR_UNITS+\
                              NR_OF_BOSERS_UNITS+NR_OF_AVIR_UNITS+NR_OF_IRTX_UNITS+NR_OF_RC5_UNITS+a)


/* MACRO's -> bAlarmState = STATUS FLAGS*/
#define ALARM_MASK_ALARMSTATE 0xC0  /* BIT 6 & 7*/
#define ALARM_NOT_SET       0x3F
#define ALARM_PART_SET      0x40
#define ALARM_FULL_SET      0x80


#define NR_OF_GROUPS 4
#define GROUP_1 0
#define GROUP_2 1
#define GROUP_3 2
#define GROUP_4 3


#define ALARM_REMOTECODE_BUFFER_MAX 			8
#define ALARM_DEFAULT_PINCODE   				9999

#define ALARM_TYPE_CONFIG_GALAXY60				0x00
#define ALARM_TYPE_CONFIG_GALAXY500				0x01
#define ALARM_TYPE_CONFIG_ARITECH				0x02    /* TM 20110427 */
#define ALARM_TYPE_CONFIG_ANB_BIBUS				0x03    /* TM_CHANGES_ALARM_ANB */
#define ALARM_TYPE_CONFIG_TEXECOM_PREMIER48 	0x04	/* TM_CHANGES_ALARM_TEXECOM */
#define ALARM_TYPE_CONFIG_TEXECOM_PREMIER88 	0x05 	/* TM_CHANGES_ALARM_TEXECOM */
#define ALARM_TYPE_CONFIG_TEXECOM_PREMIER168 	0x06 	/* TM_CHANGES_ALARM_TEXECOM */
//#define ALARM_TYPE_CONFIG_TEXECOM_PREMIER640 	0x07	/* TM_CHANGES_ALARM_TEXECOM */
#define ALARM_TYPE_CONFIG_INTEGRA				0x08	/* TM_CHANGES_ALARM_INTEGRA */
#define ALARM_TYPE_CONFIG_MASK          		0x0F    // TM_CHANGES_ALARM_TEXECOM: aangepast naar 0x0F ipv 0x07

#define NR_OF_PINS              		4
#define PINCODE_FREE            		0
#define PINCODE_USED            		1
#define PINCODE_GROUP_1         		0x01
#define PINCODE_GROUP_2         		0x02
#define PINCODE_GROUP_3         		0x04
#define PINCODE_GROUP_4         		0x08
#define PINCODE_ALL_GROUPS      		0x0F

// TM20130329:
// Maximaal aantal inputs is voor alle implementaties beperkt tot 128.
// Reden: Enkel binding vlaggen 0x00 tem 0x7F kunnen gebruikt worden.
#define ALARM_GALAXY_MAX_INPUTS			128		/* TM 20110509: Enkel voor binding software (TM_CHANGES_ALARM_ARITECH)*/
#define ALARM_ARITECH_MAX_INPUTS		64		/* TM 20110509: Enkel voor binding software (TM_CHANGES_ALARM_ARITECH)*/
#define ALARM_ANB_MAX_INPUTS			64		/* TM 20120713: Enkel voor binding software (TM_CHANGES_ALARM_ANB)*/
#define ALARM_TEXECOM_MAX_INPUTS		128		/* TM 20130328: Enkel voor binding software (TM_CHANGES_ALARM_TEXECOM)*/
#define ALARM_INTEGRA_MAX_INPUTS		128		/* TM 20150320: Enkel voor binding software (TM_CHANGES_ALARM_INTEGRA) */

typedef struct
{
  BYTE bState;
  BYTE bGroupNr;  
  unsigned short usPinCode;

} STRUCT_PINCODES;

typedef struct
{
   BYTE bAlarmStatus1;    /* bit 0 : Ask Alarm info for all groups  (response : UNSET/SET/PART SET )*/
                          /* bit 1 : Ask alarm state info for all groups (response : NORMAL/ALARM/RESET REQUIRED)*/
                          /* bit 2 : Ask alarm info -> zone states XZS1 (Check for open zones)*/
                          /* bit 3 : Re-connect to galaxy interface !! */
                          /* bit 4 : Send Positive Ack Flag !*/
                          /* bit 5 : Ask zone states XZS1*/
                          
   BYTE bAlarmStatus2;    /* bit 0 : Help flag -> PRE Alarm !*/
                          /* bit 1 : Help Flag -> Burglary beep !*/
                          /* bit 4 : RX timer expired*/
                          /* bit 5 : RX timer running*/
                          /* bit 7 : Message waiting...*/
     
   BYTE bAlarmState[NR_OF_GROUPS];   
   BYTE bAlarmConfig;	  /* TM,v64.08: Nodig voor bepalen van het type alarm centrale */
      
} UNIT_ALARM_STRUCT;


// TM_AVMATRIX_INPUT : AVMatrix unit info toegevoegd.
/*********************************************** ALARM UNIT *******************************************/
#define AVMATRIX_UNIT_NR_OFFSET    (NR_OF_DIM_UNITS+NR_OF_SWITCH_UNITS+NR_OF_CONTROL_UNITS+\
                               NR_OF_SENS_UNITS+NR_OF_AUDIO_UNITS+NR_OF_LCD_UNITS+\
                               NR_OF_LCD_VIRTUAL_UNITS+NR_OF_DUOSWITCH_UNITS+NR_OF_SELECTOR_UNITS+\
                               NR_OF_BOSERS_UNITS+NR_OF_AVIR_UNITS+NR_OF_IRTX_UNITS+NR_OF_RC5_UNITS+NR_OF_ALARM_UNITS)

#define AVMATRIX_UNIT_NR(a)   (NR_OF_DIM_UNITS+NR_OF_SWITCH_UNITS+NR_OF_CONTROL_UNITS+\
                              NR_OF_SENS_UNITS+NR_OF_AUDIO_UNITS+NR_OF_LCD_UNITS+\
                              NR_OF_LCD_VIRTUAL_UNITS+NR_OF_DUOSWITCH_UNITS+NR_OF_SELECTOR_UNITS+\
                              NR_OF_BOSERS_UNITS+NR_OF_AVIR_UNITS+NR_OF_IRTX_UNITS+NR_OF_RC5_UNITS+NR_OF_ALARM_UNITS+a)

#define AVMATRIX_TYPE_CONFIG_DEFAULT      	0x00		/* TM_AVMATRIX_INPUT */
#define AVMATRIX_TYPE_CONFIG_RESERVED		0x01		/* TM_AVMATRIX_INPUT */
#define AVMATRIX_TYPE_CONFIG_INPUT	  		0x02		/* TM_AVMATRIX_INPUT */
#define AVMATRIX_TYPE_CONFIG_MASK         	0x07		/* TM_AVMATRIX_INPUT */

typedef struct
{       
   BYTE bAVMatrixConfig;
   
   BYTE bAVMatrixStatus1;          
   BYTE bAVMatrixStatus2; /* bit 4 : repeat timer expired
                             bit 5 : repeat timer running
                             bit 7 : Message waiting... */

   
} UNIT_AVMATRIX_STRUCT; 


/******************************************************************************************************/
/* NEW UNITS...*/

/* New units shoul follow here...*/

/******************************************************************************************************/

#define NO_EVENTS 0
#define NODE_EVENT 0x01
#define OPERATOR_TIMER_EVENT 0x10
              
               
typedef struct
{
   BYTE bEventFlags;
   
} STRUCT_EVENT;


/***************************************************************************************/

/***************************************************************************************/
/* NIC receive buffer*/


#define NIC_PACKET_HEADER_SIZE 9

#define UNIT_BROADCAST_NR 255 /* Message for node object...*/
/* 08032000 */  
#define UNIT_ILLEGAL_ADDRESS 255                                                  
#define NODE_ILLEGAL_ADDRESS 255                                                  
#define NODE_FACTORY_ADDRESS 254
#define NODE_CONFIGURATION_ADDRESS 253 /* modem */
#define NODE_LCD_MASTER_ADDRESS 252
#define NODE_DEBUG_ADDRESS 251  
#define BROADCAST_ALL_ADDRESS 0
#define BROADCAST_LIM_ADDRESS 1
#define BROADCAST_PNP_ADDRESS 2
#define FIRST_VALID_LOGICAL_ADDRESS 3
#define LAST_VALID_LOGICAL_ADDRESS 250
/***************************************************************************************/

/***************************************************************************************/
/* Internal dataram structure*/
  
/***************************************************************************************/


#ifdef SKIP							/* SKIP CODE */
/* EEPROM structure*/

typedef struct
{                                          
   NODE_UNIVERSAL_STRUCT NodeConfiguration; 
   BINDINGS_FILE_STRUCT BindingsFile;
   TIMER_STRUCT_CONFIG timer;                         /* Timer structures*/

} STRUCT_NON_VOLATILE_MEMORY;

#endif

/***************************************************************************************/

#endif



