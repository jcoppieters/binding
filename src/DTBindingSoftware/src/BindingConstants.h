#ifndef BINDINGCONSTANTS
#define BINDINGCONSTANTS


// unit types: 
// Oppassen mogen de waarden niet veranderen.
// Mogen wel nieuwe toevoegen aan het einde van de lijst.
#define BINDING_UNIT_TYPE_DIM					0x01	/* OUT */
#define BINDING_UNIT_TYPE_SWITCH				0x02	/* OUT */
#define BINDING_UNIT_TYPE_CONTROL				0x03	/* IN */
#define BINDING_UNIT_TYPE_SENS					0x04	/* IN */
#define BINDING_UNIT_TYPE_AUDIO					0x05	/* OUT */
#define BINDING_UNIT_TYPE_LCD					0x06	/* NOT USED ANYMORE */
#define BINDING_UNIT_TYPE_LCD_VIRTUAL			0x07	/* IN , OUT */
#define BINDING_UNIT_TYPE_DUOSWITCH				0x08	/* OUT */
#define BINDING_UNIT_TYPE_SELECTOR				0x09	/* NOT USED ANYMORE */
#define BINDING_UNIT_TYPE_BOSERS				0x0A    /* IN */            
#define BINDING_UNIT_TYPE_AVMATRIX				0x0B	/* OUT */
#define BINDING_UNIT_TYPE_IRTX					0x0C	/* OUT */
#define BINDING_UNIT_TYPE_RC5RX					0x0D	/* IN */
#define BINDING_UNIT_TYPE_VIDEOMUX				0x0E	/* OUT */
#define BINDING_UNIT_TYPE_PS2INTELLIMOUSE		0x0F	/* NOT USED ANYMORE*/
#define BINDING_UNIT_TYPE_SPEECHCOMM			0x10	/* NOT USED */
#define BINDING_UNIT_TYPE_SPMATRIX				0x11	/* NOT USED */
#define BINDING_UNIT_TYPE_ALARM_GALAXY			0x12	/* IN */
#define BINDING_UNIT_TYPE_AUDIO_HC4				0x85	/* Specific HC4 Audio Unit ! */
#define BINDING_UNIT_TYPE_BOSELF48				0x90	/* Specific Binding Bose LF48 unit */
#define BINDING_UNIT_TYPE_BOSEV35				0x91	/* Specific Binding Bose V35 unit */
#define BINDING_UNIT_TYPE_ALARM_ARITECH			0x93	/* IN : TM_CHANGES_ALARM_ARITECH */
#define BINDING_UNIT_TYPE_AUDIO_HC4_BNO			0x94	/* Specific Bno HC4 Audio Unit */
#define BINDING_UNIT_TYPE_AVMATRIX_INPUT		0x95	/* TM 20110711: TM_AVMATRIX_INPUT */ 
#define BINDING_UNIT_TYPE_ALARM_ANB				0x96	/* TM_CHANGES_ALARM_ANB */
#define BINDING_UNIT_TYPE_ALARM_TEXECOM			0x97	/* TM_CHANGES_ALARM_TEXECOM */
#define BINDING_UNIT_TYPE_ALARM_INTEGRA			0x98	/* TM_CHANGES_ALARM_INTEGRA */
#define BINDING_UNIT_TYPE_DIM_FWD				0x99	// Sedert versie 0x0B01 - Forward binding voor 1/10V ShuitOff
#define BINDING_UNIT_TYPE_SWITCH_FWD			0xA0	// Sedert versie 0x0C01 - Forward binding
#define BINDING_UNIT_TYPE_DUOSWITCH_FWD			0xA1	// Sedert versie 0x0C01 - Forward binding
#define BINDING_UNIT_TYPE_SENSUNIV				0xA2

/*******************************************************/
#define TOGGLE		2

enum ControlType
{
	PM_UNIT = 0, 
	PM_OPERATOR, 
	PM_TIMEROPERATOR,					/* Only used several times !! */	
	UNDEFINED = 0xFF
};

enum LinkType
{
	PM_INLINKS = 0, 
	PM_OUTLINKS , 
	PM_ALLLINKS 
};

enum OperatorType
{
	PM_OPERATOR_AND=0,
	PM_OPERATOR_OR,
	PM_OPERATOR_XOR,
	PM_OPERATOR_TIMER,
	PM_OPERATOR_UNDEFINED = 0xFF
};

#define TIMER_OFFSET_SECONDS 0x00
#define TIMER_OFFSET_MINUTES 0x3B
#define TIMER_OFFSET_HOURS   0x77

enum TimerType 
{
	PM_TIMER_TE = 0,
	PM_TIMER_TI,
	PM_TIMER_TO,
	PM_TIMER_TF,
	PM_TIMER_TD,
	PM_TIMER_TR,
	PM_TIMER_NONE = 0xFF
};


enum inouttype
{
	PM_INOUTTYPE_INPUT		= 0, 
	PM_INOUTTYPE_OUTPUT, 
	PM_INOUTTYPE_UNDEFINED	= 0xFF
};


enum BINDINGTYPE 
{
	TYPE_I_BINDING = 0, 
	TYPE_P_BINDING,
	TYPE_BG_BINDING, 
	TYPE_BGN_BINDING,
	TYPE_BC_BINDING, 
	TYPE_BCN_BINDING,
	TYPE_UNDEFINED_BINDING = 0xFF
};

#endif
