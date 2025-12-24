#ifndef BINDINGUIELEMENT_H
#define BINDINGUIELEMENT_H

#include "bindingEntry.h"		/* Temp ?? */

#pragma warning( disable : 4786 )


namespace DUOTECNO {
namespace BINDINGS {
namespace UI {

/**********************************************************************************
 *
 **********************************************************************************/
class CStringEntry
{
protected:
	const BYTE m_bValue;
	const unsigned long m_ulResourceID;

public:
	CStringEntry(BYTE bValue,unsigned long ulResourceID) : 
		m_bValue(bValue), 
		m_ulResourceID(ulResourceID)
	{ };

public:
	BYTE GetValue() const {
		return m_bValue;
	}
	void Get(CString& szText) const {
		szText.LoadString(m_ulResourceID);
	}
};

/**********************************************************************************
 *
 **********************************************************************************/
class CBindingStringEntry : public CStringEntry
{
public:
	const static CBindingStringEntry Seperator;

public:
	CBindingStringEntry(BYTE bMessageCode,unsigned long ulResourceID) : 
	  CStringEntry(bMessageCode,ulResourceID) {};	
};


/**********************************************************************************
 *
 **********************************************************************************/

class CStringEntries
{
protected:
	const CStringEntry* const *m_pStringEntryArray;
	const int m_MaxEntries;

public:
	CStringEntries(const CStringEntry* const *pArray,const int Count) :
	m_pStringEntryArray(pArray), m_MaxEntries(Count)
	{ };

protected:
	BOOL Valid(int Entry) const {
		return (Entry < NrOfEntries() );
	}

public:
	int NrOfEntries() const {
		return m_MaxEntries;
	}

	const CStringEntry* const Get(int Entry) const 
	{
		if (Valid(Entry))
			return (m_pStringEntryArray[Entry]);
		return 0;
	}

	BOOL Find(BYTE bValue) const 
	{
		for (int i=0;i<NrOfEntries();i++) 
		{
			const CStringEntry* const pEntry = Get(i);
			if (pEntry)
			{
				if (bValue == pEntry->GetValue()) 
					return TRUE;
			}
		}
		return FALSE;
	}	

	BOOL Find(BYTE bValue,CString &szString) const
	{
		for (int i=0;i<NrOfEntries();i++) 
		{
			const CStringEntry* const pEntry = Get(i);
			if (pEntry)
			{
				if (bValue == pEntry->GetValue()) 
				{
					pEntry->Get(szString);
					return TRUE;
				}
			}
		}
		return FALSE;
	}
};


/**********************************************************************************
 *
 **********************************************************************************/
class CBindingStringEntries : public CStringEntries
{
protected:
	CBindingStringEntries(const CBindingStringEntry* const *pArray,const int Count) :
		CStringEntries(reinterpret_cast<const CStringEntry* const* >(pArray),Count) {};
};


/**********************************************************************************
 *
 **********************************************************************************/
typedef enum
{
	DIMMER_STRINGS_BASIC			= 0,
	DIMMER_STRINGS_EXTENDED,
	DIMMER_STRINGS_SIMPLE,
	DIMMER_STRINGS_INPUT,			// Sedert 0x0B01

	DUOSWITCH_STRINGS_BASIC,
	DUOSWITCH_STRINGS_EXTENDED,
	DUOSWITCH_STRINGS_INPUT,		// Sedert 0x0C01

	SWITCH_STRINGS_BASIC,
	SWITCH_STRINGS_EXTENDED,
	SWITCH_STRINGS_INPUT,			// Sedert 0x0C01

	SENS_STRINGS_BASIC,
	SENS_STRINGS_EXTENDED,

	SENS_STRINGS_INPUT,
	SENSUNIV_STRINGS_INPUT,			// Sedert 0x0D00

	IRTX_STRINGS_BASIC,
	IRTX_STRINGS_EXTENDED,

	BOSELF48_STRINGS_BASIC,
	BOSELF48_STRINGS_EXTENDED,
	BOSELF48_STRINGS_SIMPLE,

	BOSELF48_INPUTSTRINGS_BASIC,
	BOSELF48_INPUTSTRINGS_EXTENDED,
	BOSELF48_INPUTSTRINGS_VIRTUALS,

	AVMATRIX_STRINGS_BASIC,
	AVMATRIX_STRINGS_EXTENDED,

	AVMATRIX_INPUTSTRINGS_BASIC,		/* TM_AVMATRIX_INPUT */

	VIDEOMUX_STRINGS_BASIC,
	VIDEOMUX_STRINGS_EXTENDED,

	AUDIOHC4_STRINGS_BASIC,
	AUDIOHC4_STRINGS_SIMPLE,
	AUDIOHC4_STRINGS_EXTENDED,

	AUDIOHC4_INPUTSTRINGS_BASIC,		/* TM_AUDIOHC4_BINDING_INPUT */
	AUDIOHC4_INPUTSTRINGS_BNO_BASIC,	/* TM_AUDIOHC4_BNO */

	BOSEV35_STRINGS_BASIC,
	BOSEV35_STRINGS_EXTENDED,
	BOSEV35_STRINGS_SIMPLE,
	BOSEV35_STRINGS_VIRTUALS,

	BOSEV35_INPUTSTRINGS_BASIC,
	BOSEV35_INPUTSTRINGS_EXTENDED,
	BOSEV35_INPUTSTRINGS_VIRTUALS,

	// TM_CHANGES_BINDINGS_ALARM
	ALARM_OUTPUTSTRING,
	ALARM_INPUTSTRINGS_BASIC,
	ALARM_INPUTSTRINGS_EXTENDED,
	
	MAX,

	NOT_USED = 0xFF

}COLLECTION_KEY;

typedef enum
{
	BINDINGFLAGS_BASIC = 0,
	BINDINGFLAGS_EXTENDED,

	MAX_COLLECTION,
	KEY_NOT_USED = 0xFF
	
} COLLECTION_KEY2;



/**********************************************************************************
 *
 **********************************************************************************/
class CDimmerStringEntries : public CBindingStringEntries
{	
public:
	enum
	{
		MAX_SIMPLE  = 7,
		MAX_BASIC	= 15,
		MAX_EXTENDED = 21,			// DIMMER_CHANGES_RANGE
	};

public:
	CDimmerStringEntries(const CBindingStringEntry* const *pArray,const int Count) :
	  CBindingStringEntries(pArray,Count) { };

protected:	
	static const CBindingStringEntry Attr_OnOff;
	static const CBindingStringEntry Object_OnOff_FS;
	static const CBindingStringEntry Object_CvOff_FS;
	static const CBindingStringEntry Object_PIRON;
	static const CBindingStringEntry Object_UpDown_FS;
	static const CBindingStringEntry Object_UpDownOnOff_FS;
	static const CBindingStringEntry Object_Up;
	static const CBindingStringEntry Object_Down;
	static const CBindingStringEntry Object_Off;
	static const CBindingStringEntry Object_OnCV;
	static const CBindingStringEntry Attr_Value;
	static const CBindingStringEntry Object_OnUp;
	static const CBindingStringEntry Object_OnDown;
	static const CBindingStringEntry Object_Dec;
	static const CBindingStringEntry Object_Inc;
	static const CBindingStringEntry Attr_Range;
};


/**********************************************************************************
 *
 *********************************************************************************/
// sedert versie 0x0B01
class CDimmerInputStringEntries : public CBindingStringEntries
{	
public:
	enum
	{
		MAX_INPUTS  = 1,
	};

public:
	CDimmerInputStringEntries(const CBindingStringEntry* const *pArray,const int Count) :
	  CBindingStringEntries(pArray,Count) { };

protected:	
	static const CBindingStringEntry Event_State;
};


/**********************************************************************************
 *
 **********************************************************************************/

class CDuoswitchStringEntries : public CBindingStringEntries
{	
public:
	enum
	{
		MAX_BASIC	= 5,
		MAX_EXTENDED = 8,
	};
public:		
	CDuoswitchStringEntries(const CBindingStringEntry* const *pArray,const int Count) :
	  CBindingStringEntries(pArray,Count) { };

protected:	
	static const CBindingStringEntry Object_UpDown_FS;
	static const CBindingStringEntry Object_Stop;
	static const CBindingStringEntry Object_Up;
	static const CBindingStringEntry Object_Down;
	static const CBindingStringEntry Attr_UpStop;
	static const CBindingStringEntry Attr_DownStop;
};

/**********************************************************************************
 *
 *********************************************************************************/
// sedert versie 0x0C01
class CDuoSwitchInputStringEntries : public CBindingStringEntries
{	
public:
	enum
	{
		MAX_INPUTS  = 2,
	};

public:
	CDuoSwitchInputStringEntries(const CBindingStringEntry* const *pArray,const int Count) :
	  CBindingStringEntries(pArray,Count) { };

protected:	
	static const CBindingStringEntry Event_State_UpStop;
	static const CBindingStringEntry Event_State_DownStop;
};


/**********************************************************************************
 *
 **********************************************************************************/
class CSwitchStringEntries : public CBindingStringEntries
{	
public:
	enum
	{
		MAX_BASIC	= 5,
		MAX_EXTENDED = 7,
	};

public:		
	CSwitchStringEntries(const CBindingStringEntry* const *pArray,const int Count) :
	  CBindingStringEntries(pArray,Count) { };

protected:	
	static const CBindingStringEntry Object_OnOff_FS;
	static const CBindingStringEntry Attr_OnOff;
	static const CBindingStringEntry Object_Off;
	static const CBindingStringEntry Object_On;
	static const CBindingStringEntry Object_PIROn;
};


/**********************************************************************************
 *
 *********************************************************************************/
// sedert versie 0x0C01
class CSwitchInputStringEntries : public CBindingStringEntries
{	
public:
	enum
	{
		MAX_INPUTS  = 1,
	};

public:
	CSwitchInputStringEntries(const CBindingStringEntry* const *pArray,const int Count) :
	  CBindingStringEntries(pArray,Count) { };

protected:	
	static const CBindingStringEntry Event_State;
};

/**********************************************************************************
 *
 **********************************************************************************/
class CSensStringEntries : public CBindingStringEntries
{	
public:
	enum
	{
		MAX_BASIC	= 2,
		MAX_EXTENDED = 19,	// was 16
		MAX_INPUT = 2,
	};

public:		
	CSensStringEntries(const CBindingStringEntry* const *pArray,const int Count) :
	  CBindingStringEntries(pArray,Count) { };

protected:	
	static const CBindingStringEntry Object_OnOff_FS;
	static const CBindingStringEntry Attr_OnOff;
	static const CBindingStringEntry Object_IncDec_FS;
	static const CBindingStringEntry Object_Inc;
	static const CBindingStringEntry Object_Dec;
	static const CBindingStringEntry Attr_Preset;
	static const CBindingStringEntry Attr_Setpoint;
	static const CBindingStringEntry Attr_SampleInterval;
	static const CBindingStringEntry Attr_WorkingMode;	/* TM 20120917 */
	static const CBindingStringEntry Attr_FanSpeed;		/* TM 20120917 */
	static const CBindingStringEntry Attr_SwingAngle;	/* TM 20120917 */
	static const CBindingStringEntry Attr_SwingMode;	/* TM 20120917 */
	static const CBindingStringEntry Attr_PIDUnit;		/* TM 20220511 - zie USE_SENS_CONTROL_PID */
	static const CBindingStringEntry Attr_PIDParams;	/* TM 20220511 - zie USE_SENS_CONTROL_PID*/

	static const CBindingStringEntry Event_Heating;
	static const CBindingStringEntry Event_Cooling;
};

class CSensUnivStringEntries : public CBindingStringEntries
{	
public:
	enum
	{	
		MAX_INPUT = 4,
	};

public:
	CSensUnivStringEntries(const CBindingStringEntry* const *pArray,const int Count) :
	  CBindingStringEntries(pArray,Count) { };

protected:	
	static const CBindingStringEntry Event_Increment;
	static const CBindingStringEntry Event_Decrement;
	static const CBindingStringEntry Event_Increment_Puls;
	static const CBindingStringEntry Event_Decrement_Puls;
};


/**********************************************************************************
 *
 **********************************************************************************/
class CIRTXStringEntries : public CBindingStringEntries
{	
public:
	enum
	{
		MAX_BASIC	= 1,
		MAX_EXTENDED = 4,
	};

public:		
	CIRTXStringEntries(const CBindingStringEntry* const *pArray,const int Count) :
	  CBindingStringEntries(pArray,Count) { };

protected:	
	static const CBindingStringEntry Object_Puls;
	static const CBindingStringEntry Object_Start;
	static const CBindingStringEntry Object_Stop;
};

/**********************************************************************************
 *
 **********************************************************************************/
class CAudioBoseLf48StringEntries : public CBindingStringEntries
{	
public:
	enum
	{
		MAX_SIMPLE  = 7,
		MAX_BASIC	= 12,
		MAX_EXTENDED = 18,
	};

public:		
	CAudioBoseLf48StringEntries(const CBindingStringEntry* const *pArray,const int Count) :
	  CBindingStringEntries(pArray,Count) { };

protected:	
	static const CBindingStringEntry Attr_SourceOn;
	static const CBindingStringEntry Object_OnOff_FS;
	static const CBindingStringEntry Object_VolUpDown_FS;
	static const CBindingStringEntry Object_VolUp;
	static const CBindingStringEntry Object_VolDown;
	static const CBindingStringEntry Object_PressUp;
	static const CBindingStringEntry Object_PressDown;
	static const CBindingStringEntry Object_Source_Func;
	static const CBindingStringEntry Object_Dest_Func;
	static const CBindingStringEntry Object_On;
	static const CBindingStringEntry Object_Off;
	static const CBindingStringEntry Attr_Onoff;
	static const CBindingStringEntry Object_SourceInc;
	static const CBindingStringEntry Object_SourceDec;
};



/**********************************************************************************
 *
 **********************************************************************************/
class CAudioBoseLf48InputStringEntries : public CBindingStringEntries
{	
public:
	enum	
	{	
		MAX_VIRTUALS		= 5,
		MAX_BASIC			= 8,	// 5,	Since v0564 van 5 naar 8.
		MAX_EXTENDED		= 11,
	};

public:		
	CAudioBoseLf48InputStringEntries(const CBindingStringEntry* const *pArray,const int Count) :
	  CBindingStringEntries(pArray,Count) { };

protected:	
	static const CBindingStringEntry Event_DestOff;
	static const CBindingStringEntry Event_DestOn;
	static const CBindingStringEntry Event_SourceOff;
	static const CBindingStringEntry Event_SourceOn;
	static const CBindingStringEntry Event_Keypad;
	static const CBindingStringEntry Event_TPKeys;
	static const CBindingStringEntry Event_InputOff;
	static const CBindingStringEntry Event_InputOn;

};



/**********************************************************************************
 *
 **********************************************************************************/
class CAudioBoseV35StringEntries : public CBindingStringEntries
{	
public:
	enum
	{
		MAX_SIMPLE		= 7,
		MAX_BASIC		= 22,
		MAX_EXTENDED	= 27,
		MAX_VIRTUALS	= 1
	};

public:		
	CAudioBoseV35StringEntries(const CBindingStringEntry* const *pArray,const int Count) :
	  CBindingStringEntries(pArray,Count) { };

protected:	
	static const CBindingStringEntry Attr_SourceOn;
	static const CBindingStringEntry Object_OnOff_FS;
	static const CBindingStringEntry Object_VolUpDown_FS;
	static const CBindingStringEntry Object_VolUp;
	static const CBindingStringEntry Object_VolDown;
	static const CBindingStringEntry Object_PressUp;
	static const CBindingStringEntry Object_PressDown;
	static const CBindingStringEntry Object_Source_Func;
	static const CBindingStringEntry Object_Dest_Func;
	static const CBindingStringEntry Object_On;
	static const CBindingStringEntry Object_Off;
	static const CBindingStringEntry Attr_Onoff;
	static const CBindingStringEntry Object_SourceInc;
	static const CBindingStringEntry Object_SourceDec;
  	static const CBindingStringEntry Attr_Volume;
	static const CBindingStringEntry Object_Play;
	static const CBindingStringEntry Object_Stop;
	static const CBindingStringEntry Object_Pause;
	static const CBindingStringEntry Object_FF;
	static const CBindingStringEntry Object_RR;
	static const CBindingStringEntry Object_FFWD;
	static const CBindingStringEntry Object_FRWD;
	static const CBindingStringEntry Object_ExtraIR;

/*
	static const CBindingStringEntry Object_Mute;
*/

};

/**********************************************************************************
 *
 **********************************************************************************/
class CAudioBoseV35InputStringEntries : public CBindingStringEntries
{	
public:
	enum	
	{	
		MAX_VIRTUALS		= 5,
		MAX_BASIC			= 8,
		MAX_EXTENDED		= 8,
	};

public:		
	CAudioBoseV35InputStringEntries(const CBindingStringEntry* const *pArray,const int Count) :
	  CBindingStringEntries(pArray,Count) { };

protected:	
	static const CBindingStringEntry Event_DestOff;
	static const CBindingStringEntry Event_DestOn;
	static const CBindingStringEntry Event_SourceOff;
	static const CBindingStringEntry Event_SourceOn;
	static const CBindingStringEntry Event_Keypad;
	static const CBindingStringEntry Event_Extra;

};



/**********************************************************************************
 *
 **********************************************************************************/
class CAudioIRStringEntries : public CBindingStringEntries
{	
public:
	enum
	{
		MAX_SIMPLE  = 7,
		MAX_BASIC	= 12,
		MAX_EXTENDED = 18,
	};

public:		
	CAudioIRStringEntries(const CBindingStringEntry* const *pArray,const int Count) :
	  CBindingStringEntries(pArray,Count) { };

protected:	
	static const CBindingStringEntry Attr_SourceOn;
	static const CBindingStringEntry Object_OnOff_FS;
	static const CBindingStringEntry Object_VolUpDown_FS;
	static const CBindingStringEntry Object_VolUp;
	static const CBindingStringEntry Object_VolDown;
	static const CBindingStringEntry Object_PressUp;
	static const CBindingStringEntry Object_PressDown;
	static const CBindingStringEntry Object_Source_Func;
	static const CBindingStringEntry Object_Dest_Func;
	static const CBindingStringEntry Object_On;
	static const CBindingStringEntry Object_Off;
	static const CBindingStringEntry Attr_Onoff;
	static const CBindingStringEntry Object_SourceInc;
	static const CBindingStringEntry Object_SourceDec;
};


/**********************************************************************************
 *
 **********************************************************************************/
class CAVMatrixStringEntries : public CBindingStringEntries
{	
public:
	enum
	{	
		MAX_BASIC		= 7,			// was 3	- sedert versie 0x0903
		MAX_EXTENDED	= 23,			// was 19	- sedert versie 0x0903
	};

public:		
	CAVMatrixStringEntries(const CBindingStringEntry* const *pArray,const int Count) :
	  CBindingStringEntries(pArray,Count) { };

protected:	
	static const CBindingStringEntry Object_VolUp;
	static const CBindingStringEntry Object_VolDown;
	static const CBindingStringEntry Object_VolStop;
	static const CBindingStringEntry Object_VolInc;
	static const CBindingStringEntry Object_VolDec;
	static const CBindingStringEntry Attr_Volume;
	static const CBindingStringEntry Attr_Bass;
	static const CBindingStringEntry Attr_Treble;
	static const CBindingStringEntry Attr_Connection;
	static const CBindingStringEntry Object_Connect;
	static const CBindingStringEntry Object_Disconnect;
	static const CBindingStringEntry Object_Mute;
	static const CBindingStringEntry Object_DisconnectAll;
	static const CBindingStringEntry Object_Macro;
	static const CBindingStringEntry Attr_GroupSet;			// sedert versie 0x0903
	static const CBindingStringEntry Attr_GroupSlaveAdd;		// sedert versie 0x0903
	static const CBindingStringEntry Attr_GroupSlaveRemove;	// sedert versie 0x0903
};


/**********************************************************************************
 *
 **********************************************************************************/
/* TM_AVMATRIX_INPUT: Nieuwe klasse */
class CAVMatrixInputStringEntries : public CBindingStringEntries
{	
public:
	enum
	{
		MAX_BASIC	= 1			//3,	
	};

public:		
	CAVMatrixInputStringEntries(const CBindingStringEntry* const *pArray,const int Count) :
	  CBindingStringEntries(pArray,Count) { };

protected:	
	static const CBindingStringEntry Event_UnitControlPuls;	
	//static const CBindingStringEntry Event_UnitControlPulsToggle;
	//static const CBindingStringEntry Event_UnitControlToggle;
};


/**********************************************************************************
 *
 **********************************************************************************/
class CVideomuxStringEntries : public CBindingStringEntries
{	
public:
	enum
	{	
		MAX_BASIC	= 3,
		MAX_EXTENDED = 8,
	};

public:		
	CVideomuxStringEntries(const CBindingStringEntry* const *pArray,const int Count) :
	  CBindingStringEntries(pArray,Count) { };

protected:	
	static const CBindingStringEntry Attr_Connection;
	static const CBindingStringEntry Object_Connect;
	static const CBindingStringEntry Object_Disconnect;
	static const CBindingStringEntry Object_DisconnectAll;
	static const CBindingStringEntry Object_Keypress;
	static const CBindingStringEntry Object_KeypressCamera;
	static const CBindingStringEntry Object_DVMR;
};

/**********************************************************************************
 *
 **********************************************************************************/
class CAudioHC4StringEntries : public CBindingStringEntries
{	
public:
	enum
	{
		MAX_SIMPLE  = 10,
		MAX_BASIC	= 21,
		MAX_EXTENDED = 25,
	};

public:		
	CAudioHC4StringEntries(const CBindingStringEntry* const *pArray,const int Count) :
	  CBindingStringEntries(pArray,Count) { };

protected:	
	static const CBindingStringEntry Attr_SourceOn;
	static const CBindingStringEntry Object_OnOff_FS;
	static const CBindingStringEntry Object_On;
	static const CBindingStringEntry Object_Off;
	static const CBindingStringEntry Attr_Onoff;
	static const CBindingStringEntry Object_VolUpDown_FS;
	static const CBindingStringEntry Object_VolUp;
	static const CBindingStringEntry Object_VolDown;
	static const CBindingStringEntry Object_VolStop;
	static const CBindingStringEntry Attr_Volume;
	static const CBindingStringEntry Object_Play;
	static const CBindingStringEntry Object_Stop;
	static const CBindingStringEntry Object_Pause;
	static const CBindingStringEntry Object_FF;
	static const CBindingStringEntry Object_RR;
	static const CBindingStringEntry Object_FFWD;
	static const CBindingStringEntry Object_FRWD;
	static const CBindingStringEntry Object_ExtraIR;
	static const CBindingStringEntry Object_SourceInc;
	static const CBindingStringEntry Object_SourceDec;
	static const CBindingStringEntry Object_Mute;
};

/* TM_AUDIOHC4_BINDING_INPUT: Nieuwe klasse */
class CAudioHC4InputStringEntries : public CBindingStringEntries
{	
public:
	enum
	{
		MAX_BASIC	= 3,	
	};

public:		
	CAudioHC4InputStringEntries(const CBindingStringEntry* const *pArray,const int Count) :
	  CBindingStringEntries(pArray,Count) { };

protected:	
	static const CBindingStringEntry Event_UnitControlPuls;	
	static const CBindingStringEntry Event_UnitControlPulsToggle;
	static const CBindingStringEntry Event_UnitControlToggle;
};

/* TM_AUDIOHC4_BNO: Nieuwe klasse */
class CAudioHC4BnoInputStringEntries : public CBindingStringEntries
{	
public:
	enum
	{
		MAX_BASIC	= 1,	
	};

public:		
	CAudioHC4BnoInputStringEntries(const CBindingStringEntry* const *pArray,const int Count) :
	  CBindingStringEntries(pArray,Count) { };

protected:	
	static const CBindingStringEntry Event_MacroReceived;		
	static const CBindingStringEntry Event_Undefined;
};


/**********************************************************************************
 *
 **********************************************************************************/

class CBindingflagStringEntries : public CStringEntries
{
public:
	enum
	{
		MAX_BASIC		= 3,
		MAX_EXTENDED	= 5,
	};

	CBindingflagStringEntries(const CStringEntry* const *pArray,const int Count) :
	  CStringEntries(pArray,Count) { };

protected:
	static const CStringEntry NoChange;
	static const CStringEntry Pre;
	static const CStringEntry Post;
	static const CStringEntry Pre_Puls;
	static const CStringEntry Post_Puls;
};

/**************************************************************************************/

// TM_CHANGES_BINDINGS_ALARM
class CAlarmOutputStringEntries : public CBindingStringEntries
{	
public:
	enum
	{
		MAX_OUTPUTS	= 2,	
	};

public:		
	CAlarmOutputStringEntries(const CBindingStringEntry* const *pArray,const int Count) :
	  CBindingStringEntries(pArray,Count) { };

protected:	
	static const CBindingStringEntry Object_Arm;	
	static const CBindingStringEntry Object_Keypad;
};

// TM_CHANGES_BINDINGS_ALARM
class CAlarmInputStringEntries : public CBindingStringEntries
{	
public:
	enum
	{
		MAX_BASIC		= 1,	
		MAX_EXTENDED	= 3,
	};

public:		
	CAlarmInputStringEntries(const CBindingStringEntry* const *pArray,const int Count) :
	  CBindingStringEntries(pArray,Count) { };

protected:	
	static const CBindingStringEntry Event_AlarmInput;	
	static const CBindingStringEntry Event_AlarmArmed;
	static const CBindingStringEntry Event_AlarmBurglary;
};

/**************************************************************************************/

/********************* TM : C++ Template Magic ! **************************************/

template <class PARENT, const int N_SIZE, const int ID,class STRINGS = CBindingStringEntry>
	class TBindingEntries : public PARENT
{
protected:
	static const STRINGS* const m_StringEntries[N_SIZE];

public:
	TBindingEntries() : PARENT(&m_StringEntries[0], N_SIZE ) {};
	
	int GetID() const {
		return ID;
	}
};

/***************************************************************************************/


#ifdef MAIN


/******************************************************************************************/
const CBindingStringEntry CBindingStringEntry::Seperator(0xFF,IDS_SEPERATOR);

/**************************************************************************************************************
 * CDimmerStringEntries
 **************************************************************************************************************/
const CBindingStringEntry CDimmerStringEntries::Attr_OnOff(DIMMER_OBJECT_ATTR_ONOFF,IDS_FC_DIMSET_ONOFF);
const CBindingStringEntry CDimmerStringEntries::Object_OnOff_FS(DIMMER_OBJECT_METHOD_FSCVONOFF,IDS_FC_DIMSET_ONOFFCS);
const CBindingStringEntry CDimmerStringEntries::Object_CvOff_FS(DIMMER_OBJECT_METHOD_FSCVOFF,IDS_FC_DIMSET_VALUEOFF);
const CBindingStringEntry CDimmerStringEntries::Object_PIRON(DIMMER_OBJECT_METHOD_PIRON,IDS_FC_DIMSET_PIR);
const CBindingStringEntry CDimmerStringEntries::Object_UpDown_FS(DIMMER_OBJECT_METHOD_FSUD,IDS_FC_DIMSET_UPDN);
const CBindingStringEntry CDimmerStringEntries::Object_UpDownOnOff_FS(DIMMER_OBJECT_METHOD_FSUDONOFF,IDS_FC_DIMSET_UPDNONOFF);
const CBindingStringEntry CDimmerStringEntries::Object_Up(DIMMER_OBJECT_METHOD_UP,IDS_FC_DIMSET_UP);
const CBindingStringEntry CDimmerStringEntries::Object_Down(DIMMER_OBJECT_METHOD_DN,IDS_FC_DIMSET_DOWN);
const CBindingStringEntry CDimmerStringEntries::Object_Off(DIMMER_OBJECT_METHOD_OFF,IDS_FC_DIMSET_OFF);
const CBindingStringEntry CDimmerStringEntries::Object_OnCV(DIMMER_OBJECT_METHOD_ONCV,IDS_FC_DIMSET_ON);
const CBindingStringEntry CDimmerStringEntries::Attr_Value(DIMMER_OBJECT_ATTR_VALUE,IDS_FC_DIMSET_VALUE);
const CBindingStringEntry CDimmerStringEntries::Attr_Range(DIMMER_OBJECT_ATTR_RANGE,IDS_FC_DIMSET_RANGE);		// DIMMER_CHANGES_RANGE
const CBindingStringEntry CDimmerStringEntries::Object_OnUp(DIMMER_OBJECT_METHOD_ONUP,IDS_FC_DIMSET_ONUP);
const CBindingStringEntry CDimmerStringEntries::Object_OnDown(DIMMER_OBJECT_METHOD_ONDN,IDS_FC_DIMSET_ONDN);
const CBindingStringEntry CDimmerStringEntries::Object_Dec(DIMMER_OBJECT_METHOD_DEC,IDS_FC_DIMSET_DEC);
const CBindingStringEntry CDimmerStringEntries::Object_Inc(DIMMER_OBJECT_METHOD_INC,IDS_FC_DIMSET_INC);


template < >
const CBindingStringEntry* const TBindingEntries<CDimmerStringEntries,
												 CDimmerStringEntries::MAX_EXTENDED,
												 DIMMER_STRINGS_EXTENDED>::
	m_StringEntries[CDimmerStringEntries::MAX_EXTENDED] =
{
	&Object_OnOff_FS,								//0
	&Object_CvOff_FS,								//1
	&CBindingStringEntry::Seperator,				//2
	&Object_PIRON,									//3
	&CBindingStringEntry::Seperator,				//4
	&Object_UpDown_FS,								//5
	&Object_UpDownOnOff_FS,							//6
	&Object_Up,										//7
	&Object_Down,									//8
	&CBindingStringEntry::Seperator,				//9
	&Attr_OnOff,									//10
	&Object_Off,									//11
	&Object_OnCV,									//12
	&Attr_Value,									//13
	&Attr_Range,									//14 - DIMMER_CHANGES_RANGE
	&CBindingStringEntry::Seperator,				//15
	&Object_OnUp,									//16	
	&Object_OnDown,									//17
	&CBindingStringEntry::Seperator,				//18
	&Object_Dec,									//19
	&Object_Inc										//20
};

template < >
const CBindingStringEntry* const TBindingEntries<CDimmerStringEntries,
												 CDimmerStringEntries::MAX_BASIC,
												 DIMMER_STRINGS_BASIC>::
	m_StringEntries[CDimmerStringEntries::MAX_BASIC] =
{
	&Object_OnOff_FS,								//0
	&Object_CvOff_FS,								//1
	&CBindingStringEntry::Seperator,				//2
	&Object_PIRON,									//3
	&CBindingStringEntry::Seperator,				//4
	&Object_UpDown_FS,								//5
	&Object_Up,										//6
	&Object_Down,									//7
	&CBindingStringEntry::Seperator,				//8
	&Object_Off,									//9
	&Object_OnCV,									//10
	&Attr_Value,									//11
	&CBindingStringEntry::Seperator,				//12
	&Object_Dec,									//13
	&Object_Inc										//14
};

template < >
const CBindingStringEntry* const TBindingEntries<CDimmerStringEntries,
												 CDimmerStringEntries::MAX_SIMPLE,
												 DIMMER_STRINGS_SIMPLE>::
	m_StringEntries[CDimmerStringEntries::MAX_SIMPLE] =
{
	&Object_OnOff_FS,								//0
	&CBindingStringEntry::Seperator,				//1
	&Object_UpDown_FS,								//2
	&Object_Up,										//3	
	&Object_Down,									//4
	&CBindingStringEntry::Seperator,				//5
	&Object_PIRON,									//6
};

/**************************************************************************************************************
 * CDimmerInputStringEntries
 **************************************************************************************************************/
const CBindingStringEntry CDimmerInputStringEntries::Event_State(EV_UNITCONTROLPULSTOGGLE,IDS_EVENT_STATE);

template < >
const CBindingStringEntry* const TBindingEntries<CDimmerInputStringEntries,
												 CDimmerInputStringEntries::MAX_INPUTS,
												 DIMMER_STRINGS_INPUT>::
	m_StringEntries[CDimmerInputStringEntries::MAX_INPUTS] =
{
	&Event_State,								//0
};



/**************************************************************************************************************
 * CDuoswitchStringEntries
 **************************************************************************************************************/
const CBindingStringEntry CDuoswitchStringEntries::Object_UpDown_FS(DUOSWITCH_OBJECT_METHOD_FSUPDOWN,IDS_FC_DUOSWITCH_UPDN);
const CBindingStringEntry CDuoswitchStringEntries::Object_Stop(DUOSWITCH_OBJECT_METHOD_STOP,IDS_FC_DUOSWITCH_STOP);
const CBindingStringEntry CDuoswitchStringEntries::Object_Up(DUOSWITCH_OBJECT_METHOD_UP,IDS_FC_DUOSWITCH_UP);
const CBindingStringEntry CDuoswitchStringEntries::Object_Down(DUOSWITCH_OBJECT_METHOD_DOWN,IDS_FC_DUOSWITCH_DOWN);
const CBindingStringEntry CDuoswitchStringEntries::Attr_UpStop(DUOSWITCH_OBJECT_ATTR_UPSTOP,IDS_FC_DUOSWITCH_UPSTOP);
const CBindingStringEntry CDuoswitchStringEntries::Attr_DownStop(DUOSWITCH_OBJECT_ATTR_DOWNSTOP,IDS_FC_DUOSWITCH_DNSTOP);

template < >
const CBindingStringEntry* const TBindingEntries<CDuoswitchStringEntries,
												 CDuoswitchStringEntries::MAX_EXTENDED,
												 DUOSWITCH_STRINGS_EXTENDED>::
	m_StringEntries[CDuoswitchStringEntries::MAX_EXTENDED] =
{
	&Object_UpDown_FS,								//0
	&CBindingStringEntry::Seperator,				//1
	&Object_Stop,									//2
	&Object_Up,										//3
	&Object_Down,									//4
	&CBindingStringEntry::Seperator,				//5
	&Attr_UpStop,									//6
	&Attr_DownStop,									//7
};

template < >
const CBindingStringEntry* const TBindingEntries<CDuoswitchStringEntries,
												 CDuoswitchStringEntries::MAX_BASIC,
												 DUOSWITCH_STRINGS_BASIC>::
	m_StringEntries[CDuoswitchStringEntries::MAX_BASIC] =
{
	&Object_UpDown_FS,								//0
	&CBindingStringEntry::Seperator,				//1
	&Object_Stop,									//2
	&Object_Up,										//3
	&Object_Down,									//4
};


/**************************************************************************************************************
 * CDuoSwitchInputStringEntries
 **************************************************************************************************************/
const CBindingStringEntry CDuoSwitchInputStringEntries::Event_State_UpStop(EV_UNITCONTROLPULSTOGGLE,IDS_EVENT_UPSTOP);
const CBindingStringEntry CDuoSwitchInputStringEntries::Event_State_DownStop(EV_UNITCONTROLTOGGLE,IDS_EVENT_DOWNSTOP);

template < >
const CBindingStringEntry* const TBindingEntries<CDuoSwitchInputStringEntries,
												 CDuoSwitchInputStringEntries::MAX_INPUTS,
												 DUOSWITCH_STRINGS_INPUT>::
	m_StringEntries[CDuoSwitchInputStringEntries::MAX_INPUTS] =
{
	&Event_State_UpStop,								//0
	&Event_State_DownStop								//1
};


/**************************************************************************************************************
 * CSwitchStringEntries
 **************************************************************************************************************/
const CBindingStringEntry CSwitchStringEntries::Object_OnOff_FS(SWITCH_OBJECT_METHOD_FSONOFF,IDS_FC_SWITCHSET_ONOFFFS);
const CBindingStringEntry CSwitchStringEntries::Attr_OnOff(SWITCH_OBJECT_ATTR_ONOFF,IDS_FC_SWITCHSET_ONOFF);
const CBindingStringEntry CSwitchStringEntries::Object_Off(SWITCH_OBJECT_METHOD_OFF,IDS_FC_SWITCHSET_OFF);
const CBindingStringEntry CSwitchStringEntries::Object_On(SWITCH_OBJECT_METHOD_ON,IDS_FC_SWITCHSET_ON);
const CBindingStringEntry CSwitchStringEntries::Object_PIROn(SWITCH_OBJECT_METHOD_FSCVONOFFTIMEDPIR,IDS_FC_SWITCHSET_PIR);

template < >
const CBindingStringEntry* const TBindingEntries<CSwitchStringEntries,
												 CSwitchStringEntries::MAX_EXTENDED,
												 SWITCH_STRINGS_EXTENDED>::
	m_StringEntries[CSwitchStringEntries::MAX_EXTENDED] =
{
	&Object_OnOff_FS,								//0
	&Object_Off,									//1
	&Object_On,										//2
	&CBindingStringEntry::Seperator,				//3
	&Object_PIROn,									//4
	&CBindingStringEntry::Seperator,				//5
	&Attr_OnOff,									//6
};

template < >
const CBindingStringEntry* const TBindingEntries<CSwitchStringEntries,
												 CSwitchStringEntries::MAX_BASIC,
												 SWITCH_STRINGS_BASIC>::
	m_StringEntries[CSwitchStringEntries::MAX_BASIC] =
{
	&Object_OnOff_FS,								//0
	&Object_Off,									//1
	&Object_On,										//2
	&CBindingStringEntry::Seperator,				//3
	&Object_PIROn,									//4
};

/**************************************************************************************************************
 * CSwitchInputStringEntries
 **************************************************************************************************************/
const CBindingStringEntry CSwitchInputStringEntries::Event_State(EV_UNITCONTROLPULSTOGGLE,IDS_EVENT_STATE);

template < >
const CBindingStringEntry* const TBindingEntries<CSwitchInputStringEntries,
												 CSwitchInputStringEntries::MAX_INPUTS,
												 SWITCH_STRINGS_INPUT>::
	m_StringEntries[CSwitchInputStringEntries::MAX_INPUTS] =
{
	&Event_State,								//0
};




/**************************************************************************************************************
 * CSensStringEntries
 **************************************************************************************************************/
const CBindingStringEntry CSensStringEntries::Object_OnOff_FS(SENS_OBJECT_METHOD_FSONOFF,IDS_FC_SENSSET_SET_ONOFF);
const CBindingStringEntry CSensStringEntries::Attr_OnOff(SENS_OBJECT_ATTR_CONTROL_ONOFF,IDS_FC_SENSSET_ONOFF);
const CBindingStringEntry CSensStringEntries::Object_IncDec_FS(SENS_OBJECT_METHOD_FSINC_DEC,IDS_FC_SENSSET_SETPOINTINCDEC);
const CBindingStringEntry CSensStringEntries::Object_Inc(SENS_OBJECT_METHOD_INC,IDS_FC_SENSSET_INC);
const CBindingStringEntry CSensStringEntries::Object_Dec(SENS_OBJECT_METHOD_DEC,IDS_FC_SENSSET_DEC);
const CBindingStringEntry CSensStringEntries::Attr_Preset(SENS_OBJECT_ATTR_PRESET,IDS_FC_SENSSET_PRESET);
const CBindingStringEntry CSensStringEntries::Attr_Setpoint(SENS_OBJECT_ATTR_WORKING_SET_POINT,IDS_FC_SENSSET_SETPOINT);
const CBindingStringEntry CSensStringEntries::Attr_WorkingMode(SENS_OBJECT_ATTR_WORKING_MODE,IDS_FC_SENSSET_WORKINGMODE);
const CBindingStringEntry CSensStringEntries::Attr_FanSpeed(SENS_OBJECT_ATTR_FAN_SPEED,IDS_FC_SENSSET_FANSPEED);
const CBindingStringEntry CSensStringEntries::Attr_SwingAngle(SENS_OBJECT_ATTR_SWING_ANGLE,IDS_FC_SENSSET_SWINGANGLE);
const CBindingStringEntry CSensStringEntries::Attr_SwingMode(SENS_OBJECT_ATTR_SWING_MODE,IDS_FC_SENSSET_SWINGMODE);
const CBindingStringEntry CSensStringEntries::Attr_SampleInterval(SENS_OBJECT_ATTR_SAMPLE_INTERVAL,IDS_FC_SENSSET_INTERVAL);	// Sedert versie 0x0D00
/* TM 20220511 - zie USE_SENS_CONTROL_PID */
const CBindingStringEntry CSensStringEntries::Attr_PIDUnit(SENS_OBJECT_ATTR_PIDCONTROLUNIT,IDS_FC_SENSSET_PIDUNIT);
const CBindingStringEntry CSensStringEntries::Attr_PIDParams(SENS_OBJECT_ATTR_PIDCONTROLPARAMS,IDS_FC_SENSSET_PIDPARAMS);



const CBindingStringEntry CSensStringEntries::Event_Heating(EV_HEATING,IDS_EV_HEATING);
const CBindingStringEntry CSensStringEntries::Event_Cooling(EV_COOLING,IDS_EV_COOLING);


template < >
const CBindingStringEntry* const TBindingEntries<CSensStringEntries,
												 CSensStringEntries::MAX_BASIC,
												 SENS_STRINGS_BASIC>::
	m_StringEntries[CSensStringEntries::MAX_BASIC] =
{
	&Attr_OnOff,									//0	
	&Attr_Preset,									//1
};

template < >
const CBindingStringEntry* const TBindingEntries<CSensStringEntries,
												 CSensStringEntries::MAX_EXTENDED,
												 SENS_STRINGS_EXTENDED>::
	m_StringEntries[CSensStringEntries::MAX_EXTENDED] =
{
	&Object_OnOff_FS,								//0
	&Attr_OnOff,									//1
	&CBindingStringEntry::Seperator,				//2
	&Attr_Preset,									//3
	&Attr_Setpoint,									//4
	&CBindingStringEntry::Seperator,				//5
	&Object_IncDec_FS,								//6 
	&Object_Inc,									//7
	&Object_Dec,									//8
	&CBindingStringEntry::Seperator,				//9
	&Attr_SampleInterval,							//10
	&CBindingStringEntry::Seperator,				//11
	&Attr_WorkingMode,								//12
	&Attr_FanSpeed,									//13
	&Attr_SwingAngle,								//14
	&Attr_SwingMode,								//15
	&CBindingStringEntry::Seperator,				//16
	&Attr_PIDUnit,									//17
	&Attr_PIDParams									//18
};

template < >
const CBindingStringEntry* const TBindingEntries<CSensStringEntries,
												 CSensStringEntries::MAX_INPUT,
												 SENS_STRINGS_INPUT>::
	m_StringEntries[CSensStringEntries::MAX_INPUT] =
{
	&Event_Heating,									//0
	&Event_Cooling,									//1
};

/**************************************************************************************************************
 * CSensUnivStringEntries
 **************************************************************************************************************/
const CBindingStringEntry CSensUnivStringEntries::Event_Increment(SENSUNIV_EVENT_INC,IDS_EV_SENSUNIV_INC);
const CBindingStringEntry CSensUnivStringEntries::Event_Decrement(SENSUNIV_EVENT_DEC,IDS_EV_SENSUNIV_DEC);
const CBindingStringEntry CSensUnivStringEntries::Event_Increment_Puls(SENSUNIV_EVENT_INC_PULS,IDS_EV_SENSUNIV_INC_PULS);
const CBindingStringEntry CSensUnivStringEntries::Event_Decrement_Puls(SENSUNIV_EVENT_DEC_PULS,IDS_EV_SENSUNIV_DEC_PULS);


template < >
const CBindingStringEntry* const TBindingEntries<CSensUnivStringEntries,
												 CSensUnivStringEntries::MAX_INPUT,
												 SENSUNIV_STRINGS_INPUT>::
	m_StringEntries[CSensUnivStringEntries::MAX_INPUT] =
{
	&Event_Increment,					//0
	&Event_Decrement,					//1
	&Event_Increment_Puls,				//2
	&Event_Decrement_Puls				//3
};


/**************************************************************************************************************
 * CIRTXStringEntries
 **************************************************************************************************************/
const CBindingStringEntry CIRTXStringEntries::Object_Puls(IRTX_OBJECT_METHOD_PULS_IRTX,IDS_FC_IRTXSET_PULS);
const CBindingStringEntry CIRTXStringEntries::Object_Start(IRTX_OBJECT_METHOD_START_IRTX,IDS_FC_IRTXSET_START);
const CBindingStringEntry CIRTXStringEntries::Object_Stop(IRTX_OBJECT_METHOD_STOP_IRTX,IDS_FC_IRTXSET_STOP);

template < >
const CBindingStringEntry* const TBindingEntries<CIRTXStringEntries,
												 CIRTXStringEntries::MAX_BASIC,
												 IRTX_STRINGS_BASIC>::
	m_StringEntries[CIRTXStringEntries::MAX_BASIC] =
{
	&CIRTXStringEntries::Object_Puls,				//0	
};

template < >
const CBindingStringEntry* const TBindingEntries<CIRTXStringEntries,
												 CIRTXStringEntries::MAX_EXTENDED,
												 IRTX_STRINGS_EXTENDED>::
	m_StringEntries[CIRTXStringEntries::MAX_EXTENDED] =
{
	&Object_Puls,									//0	
	&CBindingStringEntry::Seperator,				//1
	&Object_Start,									//2
	&Object_Stop									//3
};


/**************************************************************************************************************
 * CAudioBoseLf48StringEntries
 **************************************************************************************************************/
const CBindingStringEntry CAudioBoseLf48StringEntries::Attr_SourceOn(AUDIO_OBJECT_ATTR_SOURCE_ON,IDS_IRAUDIO_SOURCEON);
const CBindingStringEntry CAudioBoseLf48StringEntries::Object_OnOff_FS(AUDIO_OBJECT_METHOD_FSCSONOFF,IDS_IRAUDIO_FSCSONOFF);
const CBindingStringEntry CAudioBoseLf48StringEntries::Object_VolUpDown_FS(AUDIO_OBJECT_METHOD_FSVOLUPDOWN,IDS_IRAUDIO_FSVOLUPDN);
const CBindingStringEntry CAudioBoseLf48StringEntries::Object_VolUp(AUDIO_OBJECT_METHOD_VOLUP,IDS_IRAUDIO_VOLUP);
const CBindingStringEntry CAudioBoseLf48StringEntries::Object_VolDown(AUDIO_OBJECT_METHOD_VOLDOWN,IDS_IRAUDIO_VOLDOWN);
const CBindingStringEntry CAudioBoseLf48StringEntries::Object_PressUp(AUDIO_OBJECT_METHOD_PRESUP,IDS_IRAUDIO_PRESSUP);
const CBindingStringEntry CAudioBoseLf48StringEntries::Object_PressDown(AUDIO_OBJECT_METHOD_PRESDOWN,IDS_IRAUDIO_PRESSDN);
const CBindingStringEntry CAudioBoseLf48StringEntries::Object_Source_Func(AUDIO_OBJECT_METHOD_SOURCE_FUNCTION,IDS_IRAUDIO_SRC_FUNCTION);
const CBindingStringEntry CAudioBoseLf48StringEntries::Object_Dest_Func(AUDIO_OBJECT_METHOD_DESTINATION_FUNCTION,IDS_IRAUDIO_DEST_FUNCTION);
const CBindingStringEntry CAudioBoseLf48StringEntries::Object_On(AUDIO_OBJECT_METHOD_CSON,IDS_IRAUDIO_CSON);
const CBindingStringEntry CAudioBoseLf48StringEntries::Object_Off(AUDIO_OBJECT_METHOD_OFF,IDS_IRAUDIO_OFF);
const CBindingStringEntry CAudioBoseLf48StringEntries::Attr_Onoff(AUDIO_OBJECT_ATTR_ONOFF,IDS_IRAUDIO_ONOFF);
const CBindingStringEntry CAudioBoseLf48StringEntries::Object_SourceInc(AUDIO_OBJECT_METHOD_SOURCEINC,IDS_IRAUDIO_SOURCEINC);
const CBindingStringEntry CAudioBoseLf48StringEntries::Object_SourceDec(AUDIO_OBJECT_METHOD_SOURCEDEC,IDS_IRAUDIO_SOURCEDEC);



template < >
const CBindingStringEntry* const TBindingEntries<CAudioBoseLf48StringEntries,
												 CAudioBoseLf48StringEntries::MAX_EXTENDED,
												 BOSELF48_STRINGS_EXTENDED>::
	m_StringEntries[CAudioBoseLf48StringEntries::MAX_EXTENDED] =
{
	&Attr_SourceOn,								//0	
	&Object_OnOff_FS,							//1
	&CBindingStringEntry::Seperator,			//2
	&Object_VolUpDown_FS,						//3
	&Object_VolUp,								//4
	&Object_VolDown,							//5
	&CBindingStringEntry::Seperator,			//6
	&Object_PressUp,							//7
	&Object_PressDown,							//8
	&Object_Source_Func,						//9
	&Object_Dest_Func,							//10
	&CBindingStringEntry::Seperator,			//11
	&Object_On,									//12
	&Object_Off,								//13
	&Attr_Onoff,								//14
	&CBindingStringEntry::Seperator,			//15
	&Object_SourceInc,							//16
	&Object_SourceDec							//17
};

template < >
const CBindingStringEntry* const TBindingEntries<CAudioBoseLf48StringEntries,
												 CAudioBoseLf48StringEntries::MAX_BASIC,
												 BOSELF48_STRINGS_BASIC>::
	m_StringEntries[CAudioBoseLf48StringEntries::MAX_BASIC] =
{
	&Attr_SourceOn,								//0
	&Object_Off,								//1
	&Object_On,									//2
	&CBindingStringEntry::Seperator,			//3
	&Object_VolUpDown_FS,						//4
	&Object_VolUp,								//5
	&Object_VolDown,							//6
	&CBindingStringEntry::Seperator,			//7
	&Object_PressUp,							//8
	&Object_PressDown,							//9
	&Object_Source_Func,						//10
	&Object_Dest_Func,							//11
};

template < >
const CBindingStringEntry* const TBindingEntries<CAudioBoseLf48StringEntries,
												 CAudioBoseLf48StringEntries::MAX_SIMPLE,
												 BOSELF48_STRINGS_SIMPLE>::
	m_StringEntries[CAudioBoseLf48StringEntries::MAX_SIMPLE] =
{
	&Attr_SourceOn,								//0
	&Object_Off,								//1
	&Object_On,									//2
	&CBindingStringEntry::Seperator,			//3
	&Object_VolUpDown_FS,						//4
	&Object_VolUp,								//5
	&Object_VolDown,							//6
};



/**************************************************************************************************************
 * CAudioBoseLf48InputStringEntries
 **************************************************************************************************************/

const CBindingStringEntry CAudioBoseLf48InputStringEntries::Event_DestOff(BOSELF48_EVENT_DESTOFF,IDS_BOSELF_DESTOFF);
const CBindingStringEntry CAudioBoseLf48InputStringEntries::Event_DestOn(BOSELF48_EVENT_DESTON,IDS_BOSELF_DESTON);
const CBindingStringEntry CAudioBoseLf48InputStringEntries::Event_SourceOff(BOSELF48_EVENT_SOURCEOFF,IDS_BOSELF_SOURCEOFF);
const CBindingStringEntry CAudioBoseLf48InputStringEntries::Event_SourceOn(BOSELF48_EVENT_SOURCEON,IDS_BOSELF_SOURCEON);
const CBindingStringEntry CAudioBoseLf48InputStringEntries::Event_Keypad(BOSELF48_EVENT_KEYPAD,IDS_BOSELF_KEYPAD);
const CBindingStringEntry CAudioBoseLf48InputStringEntries::Event_TPKeys(BOSELF48_EVENT_TPKEYS,IDS_BOSELF_TPKEYS);
const CBindingStringEntry CAudioBoseLf48InputStringEntries::Event_InputOff(BOSELF48_EVENT_INPUTOFF,IDS_BOSELF_INPUTOFF);
const CBindingStringEntry CAudioBoseLf48InputStringEntries::Event_InputOn(BOSELF48_EVENT_INPUTON,IDS_BOSELF_INPUTON);


template < >
const CBindingStringEntry* const TBindingEntries<CAudioBoseLf48InputStringEntries,
												 CAudioBoseLf48InputStringEntries::MAX_EXTENDED,
												 BOSELF48_INPUTSTRINGS_EXTENDED>::
	m_StringEntries[CAudioBoseLf48InputStringEntries::MAX_EXTENDED] =
{
	&Event_DestOff,								//0	
	&Event_DestOn,								//1
	&CBindingStringEntry::Seperator,			//2
	&Event_SourceOff,							//3
	&Event_SourceOn,							//4
	&CBindingStringEntry::Seperator,			//5
	&Event_Keypad,								//6
	&Event_TPKeys,								//7
	&CBindingStringEntry::Seperator,			//8
	&Event_InputOff,							//9
	&Event_InputOn,								//10
};

template < >
const CBindingStringEntry* const TBindingEntries<CAudioBoseLf48InputStringEntries,
												 CAudioBoseLf48InputStringEntries::MAX_BASIC,
												 BOSELF48_INPUTSTRINGS_BASIC>::
	m_StringEntries[CAudioBoseLf48InputStringEntries::MAX_BASIC] =
{
	&Event_DestOff,								//0	
	&Event_DestOn,								//1	
	&CBindingStringEntry::Seperator,			//2
	&Event_Keypad,								//3
	&Event_TPKeys,								//4
	&CBindingStringEntry::Seperator,			//5		// since v0564
	&Event_SourceOff,							//6		// since v0564
	&Event_SourceOn,							//7		// since v0564
};

template < >
const CBindingStringEntry* const TBindingEntries<CAudioBoseLf48InputStringEntries,
												 CAudioBoseLf48InputStringEntries::MAX_VIRTUALS,
												 BOSELF48_INPUTSTRINGS_VIRTUALS>::
	m_StringEntries[CAudioBoseLf48InputStringEntries::MAX_VIRTUALS] =
{
	&Event_SourceOff,							//0	
	&Event_SourceOn,							//1	
	&CBindingStringEntry::Seperator,			//2
	&Event_Keypad,								//3
	&Event_TPKeys,								//4
};

/**************************************************************************************************************/

/**************************************************************************************************************
 * CAudioBoseV35StringEntries
 **************************************************************************************************************/
const CBindingStringEntry CAudioBoseV35StringEntries::Attr_SourceOn(BOSERS_OBJECT_ATTR_SOURCE_ON,IDS_IRAUDIO_SOURCEON);
const CBindingStringEntry CAudioBoseV35StringEntries::Object_OnOff_FS(BOSERS_OBJECT_METHOD_FSCSONOFF,IDS_IRAUDIO_FSCSONOFF);
const CBindingStringEntry CAudioBoseV35StringEntries::Object_VolUpDown_FS(BOSERS_OBJECT_METHOD_FSVOLUPDOWN,IDS_IRAUDIO_FSVOLUPDN);
const CBindingStringEntry CAudioBoseV35StringEntries::Object_VolUp(BOSERS_OBJECT_METHOD_VOLUP,IDS_IRAUDIO_VOLUP);
const CBindingStringEntry CAudioBoseV35StringEntries::Object_VolDown(BOSERS_OBJECT_METHOD_VOLDOWN,IDS_IRAUDIO_VOLDOWN);
const CBindingStringEntry CAudioBoseV35StringEntries::Object_PressUp(BOSERS_OBJECT_METHOD_PRESUP,IDS_IRAUDIO_PRESSUP);
const CBindingStringEntry CAudioBoseV35StringEntries::Object_PressDown(BOSERS_OBJECT_METHOD_PRESDOWN,IDS_IRAUDIO_PRESSDN);
const CBindingStringEntry CAudioBoseV35StringEntries::Object_Source_Func(BOSERS_OBJECT_METHOD_SOURCE_FUNCTION,IDS_IRAUDIO_SRC_FUNCTION);
const CBindingStringEntry CAudioBoseV35StringEntries::Object_Dest_Func(BOSERS_OBJECT_METHOD_DESTINATION_FUNCTION,IDS_IRAUDIO_DEST_FUNCTION);
const CBindingStringEntry CAudioBoseV35StringEntries::Object_On(BOSERS_OBJECT_METHOD_CSON,IDS_IRAUDIO_CSON);
const CBindingStringEntry CAudioBoseV35StringEntries::Object_Off(BOSERS_OBJECT_METHOD_OFF,IDS_IRAUDIO_OFF);
const CBindingStringEntry CAudioBoseV35StringEntries::Attr_Onoff(BOSERS_OBJECT_ATTR_ONOFF,IDS_IRAUDIO_ONOFF);
const CBindingStringEntry CAudioBoseV35StringEntries::Object_SourceInc(BOSERS_OBJECT_METHOD_SOURCEINC,IDS_IRAUDIO_SOURCEINC);
const CBindingStringEntry CAudioBoseV35StringEntries::Object_SourceDec(BOSERS_OBJECT_METHOD_SOURCEDEC,IDS_IRAUDIO_SOURCEDEC);
const CBindingStringEntry CAudioBoseV35StringEntries::Attr_Volume(BOSERS_OBJECT_ATTR_VOLUME, IDS_IRAUDIO_VOLUME);
const CBindingStringEntry CAudioBoseV35StringEntries::Object_Play(BOSERS_OBJECT_METHOD_PLAY, IDS_BOSERS_TK_PLAY);
const CBindingStringEntry CAudioBoseV35StringEntries::Object_Stop(BOSERS_OBJECT_METHOD_STOP, IDS_BOSERS_TK_STOP);
const CBindingStringEntry CAudioBoseV35StringEntries::Object_Pause(BOSERS_OBJECT_METHOD_PAUSE, IDS_BOSELF_PAUSE);
const CBindingStringEntry CAudioBoseV35StringEntries::Object_FF(BOSERS_OBJECT_METHOD_FF, IDS_BOSELF_INC);
const CBindingStringEntry CAudioBoseV35StringEntries::Object_RR(BOSERS_OBJECT_METHOD_RR, IDS_BOSELF_DEC);
const CBindingStringEntry CAudioBoseV35StringEntries::Object_FFWD(BOSERS_OBJECT_METHOD_FFWD, IDS_BOSELF_PREV);
const CBindingStringEntry CAudioBoseV35StringEntries::Object_FRWD(BOSERS_OBJECT_METHOD_FRWD, IDS_BOSELF_NEXT);
const CBindingStringEntry CAudioBoseV35StringEntries::Object_ExtraIR(BOSERS_OBJECT_METHOD_EXTRAIR, IDS_BOSELF_KEYPAD_EXTRA);


template < >
const CBindingStringEntry* const TBindingEntries<CAudioBoseV35StringEntries,
												 CAudioBoseV35StringEntries::MAX_EXTENDED,
												 BOSEV35_STRINGS_EXTENDED>::
	m_StringEntries[CAudioBoseV35StringEntries::MAX_EXTENDED] =
{
	&Attr_SourceOn,								//0	
	&Object_On,									//1
	&Object_Off,								//2
	&Attr_Onoff,								//3
	&Object_OnOff_FS,							//4
	&CBindingStringEntry::Seperator,			//5
	&Object_VolUpDown_FS,						//6
	&Object_VolUp,								//7
	&Object_VolDown,							//8
	&CBindingStringEntry::Seperator,			//9
	&Object_PressUp,							//10
	&Object_PressDown,							//11
	&Object_Source_Func,						//12
	&Object_Dest_Func,							//13
	&CBindingStringEntry::Seperator,			//14
	&Object_Play,								//15
	&Object_Stop,								//16
	&Object_Pause,								//17
	&Object_FF,									//18
	&Object_RR,									//19
	&Object_FFWD,								//20
	&Object_FRWD,								//21
	&CBindingStringEntry::Seperator,			//22
	&Object_ExtraIR,							//23
	&CBindingStringEntry::Seperator,			//24
	&Object_SourceInc,							//25
	&Object_SourceDec							//26
};

template < >
const CBindingStringEntry* const TBindingEntries<CAudioBoseV35StringEntries,
												 CAudioBoseV35StringEntries::MAX_BASIC,
												 BOSEV35_STRINGS_BASIC>::
	m_StringEntries[CAudioBoseV35StringEntries::MAX_BASIC] =
{
	&Object_Off,								//0
	&Object_On,									//1
	&Attr_SourceOn,								//2
	&CBindingStringEntry::Seperator,			//3
	&Object_VolUpDown_FS,						//4
	&Object_VolUp,								//5
	&Object_VolDown,							//6
	&CBindingStringEntry::Seperator,			//7
	&Object_PressUp,							//8
	&Object_PressDown,							//9
	&Object_Source_Func,						//10
	&Object_Dest_Func,							//11
	&CBindingStringEntry::Seperator,			//12
	&Object_Play,								//13
	&Object_Stop,								//14
	&Object_Pause,								//15
	&Object_FF,									//16
	&Object_RR,									//17
	&Object_FFWD,								//18
	&Object_FRWD,								//19
	&CBindingStringEntry::Seperator,			//20
	&Object_ExtraIR,							//21
};

template < >
const CBindingStringEntry* const TBindingEntries<CAudioBoseV35StringEntries,
												 CAudioBoseV35StringEntries::MAX_SIMPLE,
												 BOSEV35_STRINGS_SIMPLE>::
	m_StringEntries[CAudioBoseLf48StringEntries::MAX_SIMPLE] =
{	
	&Object_Off,								//0
	&Object_On,									//1
	&Attr_SourceOn,								//2
	&CBindingStringEntry::Seperator,			//3
	&Object_VolUpDown_FS,						//4
	&Object_VolUp,								//5
	&Object_VolDown,							//6
};


template < >
const CBindingStringEntry* const TBindingEntries<CAudioBoseV35StringEntries,
												 CAudioBoseV35StringEntries::MAX_VIRTUALS,
												 BOSEV35_STRINGS_VIRTUALS>::
	m_StringEntries[CAudioBoseV35StringEntries::MAX_VIRTUALS] =
{
	&Object_Off,								// 0
};


/**************************************************************************************************************
 * CAudioBoseV35InputStringEntries
 **************************************************************************************************************/

const CBindingStringEntry CAudioBoseV35InputStringEntries::Event_DestOff(BOSEV35_EVENT_DESTOFF,IDS_BOSELF_DESTOFF);
const CBindingStringEntry CAudioBoseV35InputStringEntries::Event_DestOn(BOSEV35_EVENT_DESTON,IDS_BOSELF_DESTON);
const CBindingStringEntry CAudioBoseV35InputStringEntries::Event_SourceOff(BOSEV35_EVENT_SOURCEOFF,IDS_BOSELF_SOURCEOFF);
const CBindingStringEntry CAudioBoseV35InputStringEntries::Event_SourceOn(BOSEV35_EVENT_SOURCEON,IDS_BOSELF_SOURCEON);
const CBindingStringEntry CAudioBoseV35InputStringEntries::Event_Keypad(BOSEV35_EVENT_KEYPAD,IDS_BOSELF_EVENT_SOURCE);
const CBindingStringEntry CAudioBoseV35InputStringEntries::Event_Extra(BOSEV35_EVENT_EXTRA,IDS_BOSELF_EVENT_EXTRA);


template < >
const CBindingStringEntry* const TBindingEntries<CAudioBoseV35InputStringEntries,
												 CAudioBoseV35InputStringEntries::MAX_EXTENDED,
												 BOSEV35_INPUTSTRINGS_EXTENDED>::
	m_StringEntries[CAudioBoseV35InputStringEntries::MAX_EXTENDED] =
{
	&Event_SourceOff,							//0
	&Event_SourceOn,							//1
	&CBindingStringEntry::Seperator,			//2
	&Event_DestOff,								//3	
	&Event_DestOn,								//4
	&CBindingStringEntry::Seperator,			//5
	&Event_Keypad,								//6
	&Event_Extra								//7 
};

template < >
const CBindingStringEntry* const TBindingEntries<CAudioBoseV35InputStringEntries,
												 CAudioBoseV35InputStringEntries::MAX_BASIC,
												 BOSEV35_INPUTSTRINGS_BASIC>::
	m_StringEntries[CAudioBoseV35InputStringEntries::MAX_BASIC] =
{
	&Event_SourceOff,							//0
	&Event_SourceOn,							//1
	&CBindingStringEntry::Seperator,			//2
	&Event_DestOff,								//3	
	&Event_DestOn,								//4
	&CBindingStringEntry::Seperator,			//5
	&Event_Keypad,								//6
	&Event_Extra								//7 
};

template < >
const CBindingStringEntry* const TBindingEntries<CAudioBoseV35InputStringEntries,
												 CAudioBoseV35InputStringEntries::MAX_VIRTUALS,
												 BOSEV35_INPUTSTRINGS_VIRTUALS>::
	m_StringEntries[CAudioBoseV35InputStringEntries::MAX_VIRTUALS] =
{
	&Event_SourceOff,							//0	
	&Event_SourceOn,							//1	
	&CBindingStringEntry::Seperator,			//2
	&Event_Keypad,								//3
	&Event_Extra								//4
};


/**************************************************************************************************************
 * CAVMatrixStringEntries
 **************************************************************************************************************/
const CBindingStringEntry CAVMatrixStringEntries::Object_VolUp(AVMATRIX_OBJECT_METHOD_VOLUME_UP,IDS_AVMATRIX_VOLUP);
const CBindingStringEntry CAVMatrixStringEntries::Object_VolDown(AVMATRIX_OBJECT_METHOD_VOLUME_DOWN,IDS_AVMATRIX_VOLDOWN);
const CBindingStringEntry CAVMatrixStringEntries::Object_VolStop(AVMATRIX_OBJECT_METHOD_VOLUME_STOP,IDS_AVMATRIX_VOLSTOP);
const CBindingStringEntry CAVMatrixStringEntries::Object_VolInc(AVMATRIX_OBJECT_METHOD_VOLUME_INC,IDS_AVMATRIX_VOLINC);
const CBindingStringEntry CAVMatrixStringEntries::Object_VolDec(AVMATRIX_OBJECT_METHOD_VOLUME_DEC,IDS_AVMATRIX_VOLDEC);
const CBindingStringEntry CAVMatrixStringEntries::Attr_Volume(AVMATRIX_OBJECT_ATTR_VOLUME,IDS_AVMATRIX_VOLUME);
const CBindingStringEntry CAVMatrixStringEntries::Attr_Bass(AVMATRIX_OBJECT_ATTR_BASS,IDS_AVMATRIX_BASS);
const CBindingStringEntry CAVMatrixStringEntries::Attr_Treble(AVMATRIX_OBJECT_ATTR_TREBLE,IDS_AVMATRIX_TREBLE);
const CBindingStringEntry CAVMatrixStringEntries::Attr_Connection(AVMATRIX_OBJECT_ATTR_CONNECTION,IDS_AVMATRIX_CONNECTION);
const CBindingStringEntry CAVMatrixStringEntries::Object_Connect(AVMATRIX_OBJECT_METHOD_CONNECT,IDS_AVMATRIX_CONNECT);
const CBindingStringEntry CAVMatrixStringEntries::Object_Disconnect(AVMATRIX_OBJECT_METHOD_DISCONNECT,IDS_AVMATRIX_DISCONNECT);
const CBindingStringEntry CAVMatrixStringEntries::Object_Mute(AVMATRIX_OBJECT_METHOD_MUTE_UNMUTE,IDS_AVMATRIX_MUTE);
const CBindingStringEntry CAVMatrixStringEntries::Object_DisconnectAll(AVMATRIX_OBJECT_METHOD_DISCONNECT_ALL,IDS_AVMATRIX_DISCONNECTALL);
const CBindingStringEntry CAVMatrixStringEntries::Object_Macro(AVMATRIX_OBJECT_METHOD_MACRO,IDS_AVMATRIX_MACRO);
// Sedert versie 0x0903
const CBindingStringEntry CAVMatrixStringEntries::Attr_GroupSet(AVMATRIX_OBJECT_ATTR_ZONEGROUP_SET,IDS_AVMATRIX_GROUPSET);
const CBindingStringEntry CAVMatrixStringEntries::Attr_GroupSlaveAdd(AVMATRIX_OBJECT_ATTR_ZONEGROUP_SLAVEADD,IDS_AVMATRIX_GROUPSLAVEADD);
const CBindingStringEntry CAVMatrixStringEntries::Attr_GroupSlaveRemove(AVMATRIX_OBJECT_ATTR_ZONEGROUP_SLAVEREMOVE,IDS_AVMATRIX_GROUPSLAVEREMOVE);


template < >
const CBindingStringEntry* const TBindingEntries<CAVMatrixStringEntries,
												 CAVMatrixStringEntries::MAX_EXTENDED,
												 AVMATRIX_STRINGS_EXTENDED>::
	m_StringEntries[CAVMatrixStringEntries::MAX_EXTENDED] =
{
	&Object_VolUp,									//0	
	&Object_VolDown,								//1
	&Object_VolStop,								//2
	&CBindingStringEntry::Seperator,				//3
	&Object_VolInc,									//4
	&Object_VolDec,									//5
	&CBindingStringEntry::Seperator,				//6
	&Attr_Volume,									//7
	&Attr_Bass,										//8
	&Attr_Treble,									//9
	&CBindingStringEntry::Seperator,				//10
	&Attr_Connection,								//11
	&Object_Connect,								//12
	&Object_Disconnect,								//13
	&CBindingStringEntry::Seperator,				//14
	&Object_Mute,									//15
	&CBindingStringEntry::Seperator,				//16
	&Object_DisconnectAll,							//17
	&Object_Macro,									//18
	&CBindingStringEntry::Seperator,				//19
	&Attr_GroupSet,									//20
	&Attr_GroupSlaveAdd,							//21
	&Attr_GroupSlaveRemove,							//22
};

template < >
const CBindingStringEntry* const TBindingEntries<CAVMatrixStringEntries,
												 CAVMatrixStringEntries::MAX_BASIC,
												 AVMATRIX_STRINGS_BASIC>::
	m_StringEntries[CAVMatrixStringEntries::MAX_BASIC] =
{
	&Object_Mute,									//0
	&Object_DisconnectAll,							//1
	&Object_Macro,									//2
	&CBindingStringEntry::Seperator,				//3
	&Attr_GroupSet,									//4
	&Attr_GroupSlaveAdd,							//5
	&Attr_GroupSlaveRemove,							//6
};

/**************************************************************************************************************
 * CAVMatrixInputStringEntries	TM_AVMATRIX_INPUT
 **************************************************************************************************************/
const CBindingStringEntry CAVMatrixInputStringEntries::Event_UnitControlPuls(EV_UNITCONTROLPULS,IDS_EV_CONTROLPULS);
// const CBindingStringEntry CAVMatrixInputStringEntries::Event_UnitControlPulsToggle(EV_UNITCONTROLPULSTOGGLE,IDS_EV_CONTROLPULSTOGGLE);
// const CBindingStringEntry CAVMatrixInputStringEntries::Event_UnitControlToggle(EV_UNITCONTROLTOGGLE,IDS_EV_CONTROLTOGGLE);

template < >
const CBindingStringEntry* const TBindingEntries<CAVMatrixInputStringEntries,
												 CAVMatrixInputStringEntries::MAX_BASIC,
												 AVMATRIX_INPUTSTRINGS_BASIC>::
	m_StringEntries[CAVMatrixInputStringEntries::MAX_BASIC] =
{
	&Event_UnitControlPuls,						// 0	
	// &Event_UnitControlPulsToggle,			// 1	
	// &Event_UnitControlToggle					// 2
};


/**************************************************************************************************************
 * CVideomuxStringEntries
 **************************************************************************************************************/
const CBindingStringEntry CVideomuxStringEntries::Attr_Connection(VIDEOMUX_OBJECT_ATTR_CONNECTION,IDS_VIDEOMUX_CONNECTION);
const CBindingStringEntry CVideomuxStringEntries::Object_Connect(VIDEOMUX_OBJECT_METHOD_CONNECT,IDS_VIDEOMUX_CONNECT);
const CBindingStringEntry CVideomuxStringEntries::Object_Disconnect(VIDEOMUX_OBJECT_METHOD_DISCONNECT,IDS_VIDEOMUX_DISCONNECT);
const CBindingStringEntry CVideomuxStringEntries::Object_DisconnectAll(VIDEOMUX_OBJECT_METHOD_DISCONNECT_ALL,IDS_VIDEOMUX_DISCONNECTALL);
const CBindingStringEntry CVideomuxStringEntries::Object_Keypress(VIDEOMUX_OBJECT_METHOD_KEYPRESS,IDS_VIDEOMUX_RAS_KEYPRESS);
const CBindingStringEntry CVideomuxStringEntries::Object_KeypressCamera(VIDEOMUX_OBJECT_METHOD_KEYPRESS_CAMERA,IDS_VIDEOMUX_RAS_BUTTON);
const CBindingStringEntry CVideomuxStringEntries::Object_DVMR(VIDEOMUX_OBJECT_METHOD_DVMR,IDS_VIDEOMUX_RAS_DVMR);


template < >
const CBindingStringEntry* const TBindingEntries<CVideomuxStringEntries,
												 CVideomuxStringEntries::MAX_EXTENDED,
												 VIDEOMUX_STRINGS_EXTENDED>::
	m_StringEntries[CVideomuxStringEntries::MAX_EXTENDED] =
{
	&Attr_Connection,							//0	
	&Object_Connect,							//1
	&Object_Disconnect,							//2
	&Object_DisconnectAll,						//3
	&CBindingStringEntry::Seperator,			//4
	&Object_Keypress,							//5
	&Object_KeypressCamera,						//6
	&Object_DVMR,								//7
};

template < >
const CBindingStringEntry* const TBindingEntries<CVideomuxStringEntries,
												 CVideomuxStringEntries::MAX_BASIC,
												 VIDEOMUX_STRINGS_BASIC>::
	m_StringEntries[CVideomuxStringEntries::MAX_BASIC] =
{
	&Object_Connect,							//0
	&Object_Disconnect,							//1
	&Object_DisconnectAll,						//2
};



/**************************************************************************************************************
 * CAudioHC4StringEntries
 **************************************************************************************************************/
const CBindingStringEntry CAudioHC4StringEntries::Attr_SourceOn(AUDIO_MATRIX_OBJECT_ATTR_SOURCE_ON,IDS_MATRIXAUDIO_SOURCEON);
const CBindingStringEntry CAudioHC4StringEntries::Object_OnOff_FS(AUDIO_MATRIX_OBJECT_METHOD_FSCSONOFF,IDS_MATRIXAUDIO_FSCSONOFF);
const CBindingStringEntry CAudioHC4StringEntries::Object_On(AUDIO_MATRIX_OBJECT_METHOD_CSON,IDS_MATRIXAUDIO_CSON);
const CBindingStringEntry CAudioHC4StringEntries::Object_Off(AUDIO_MATRIX_OBJECT_METHOD_OFF,IDS_MATRIXAUDIO_OFF);
const CBindingStringEntry CAudioHC4StringEntries::Attr_Onoff(AUDIO_MATRIX_OBJECT_ATTR_ONOFF,IDS_MATRIXAUDIO_ONOFF);
const CBindingStringEntry CAudioHC4StringEntries::Object_VolUpDown_FS(AUDIO_MATRIX_OBJECT_METHOD_FSVOLUPDOWN,IDS_MATRIXAUDIO_FSVOLUPDOWN);
const CBindingStringEntry CAudioHC4StringEntries::Object_VolUp(AUDIO_MATRIX_OBJECT_METHOD_VOLUP,IDS_MATRIXAUDIO_VOLUP);
const CBindingStringEntry CAudioHC4StringEntries::Object_VolDown(AUDIO_MATRIX_OBJECT_METHOD_VOLDOWN,IDS_MATRIXAUDIO_VOLDOWN);
const CBindingStringEntry CAudioHC4StringEntries::Object_VolStop(AUDIO_MATRIX_OBJECT_METHOD_VOLSTOP,IDS_MATRIXAUDIO_VOLSTOP);
const CBindingStringEntry CAudioHC4StringEntries::Attr_Volume(AUDIO_MATRIX_OBJECT_ATTR_VOLUME,IDS_MATRIXAUDIO_VOLUME);
const CBindingStringEntry CAudioHC4StringEntries::Object_Play(AUDIO_MATRIX_OBJECT_METHOD_PLAY,IDS_MATRIXAUDIO_PLAY);
const CBindingStringEntry CAudioHC4StringEntries::Object_Stop(AUDIO_MATRIX_OBJECT_METHOD_STOP,IDS_MATRIXAUDIO_STOP);
const CBindingStringEntry CAudioHC4StringEntries::Object_Pause(AUDIO_MATRIX_OBJECT_METHOD_PAUSE,IDS_MATRIXAUDIO_PAUSE);
const CBindingStringEntry CAudioHC4StringEntries::Object_FF(AUDIO_MATRIX_OBJECT_METHOD_FF,IDS_MATRIXAUDIO_FF);
const CBindingStringEntry CAudioHC4StringEntries::Object_RR(AUDIO_MATRIX_OBJECT_METHOD_RR,IDS_MATRIXAUDIO_RR);
const CBindingStringEntry CAudioHC4StringEntries::Object_FFWD(AUDIO_MATRIX_OBJECT_METHOD_FFWD,IDS_MATRIXAUDIO_FFWD);
const CBindingStringEntry CAudioHC4StringEntries::Object_FRWD(AUDIO_MATRIX_OBJECT_METHOD_FRWD,IDS_MATRIXAUDIO_FRWD);
const CBindingStringEntry CAudioHC4StringEntries::Object_ExtraIR(AUDIO_MATRIX_OBJECT_METHOD_EXTRAIR,IDS_MATRIXAUDIO_KEYPAD);
const CBindingStringEntry CAudioHC4StringEntries::Object_SourceInc(AUDIO_OBJECT_METHOD_SOURCEINC,IDS_MATRIXAUDIO_SOURCEINC);
const CBindingStringEntry CAudioHC4StringEntries::Object_SourceDec(AUDIO_OBJECT_METHOD_SOURCEDEC,IDS_MATRIXAUDIO_SOURCEDEC);
const CBindingStringEntry CAudioHC4StringEntries::Object_Mute(AUDIO_MATRIX_OBJECT_METHOD_MUTE_UNMUTE,IDS_MATRIXAUDIO_MUTE);



template < >
const CBindingStringEntry* const TBindingEntries<CAudioHC4StringEntries,
												 CAudioHC4StringEntries::MAX_EXTENDED,
												 AUDIOHC4_STRINGS_EXTENDED>::
	m_StringEntries[CAudioHC4StringEntries::MAX_EXTENDED] =
{
	&Attr_SourceOn,								//0	
	&Object_OnOff_FS,							//1
	&Object_On,									//2
	&Object_Off,								//3
	&Attr_Onoff,								//4
	&CBindingStringEntry::Seperator,			//5
	&Object_VolUpDown_FS,						//6
	&Object_VolUp,								//7
	&Object_VolDown,							//8
	&Object_VolStop,							//9
	&Attr_Volume,								//10
	&CBindingStringEntry::Seperator,			//11
	&Object_Play,								//12
	&Object_Stop,								//13
	&Object_Pause,								//14
	&Object_FF,									//15
	&Object_RR,									//16
	&Object_FFWD,								//17
	&Object_FRWD,								//18
	&Object_ExtraIR,							//19
	&CBindingStringEntry::Seperator,			//20
	&Object_SourceInc,							//21
	&Object_SourceDec,							//22
	&CBindingStringEntry::Seperator,			//23
	&Object_Mute								//24
};

template < >
const CBindingStringEntry* const TBindingEntries<CAudioHC4StringEntries,
												 CAudioHC4StringEntries::MAX_BASIC,
												 AUDIOHC4_STRINGS_BASIC>::
	m_StringEntries[CAudioHC4StringEntries::MAX_BASIC] =
{
	&Attr_SourceOn,								//0	
	&Object_OnOff_FS,							//1
	&Object_On,									//2
	&Object_Off,								//3
	&CBindingStringEntry::Seperator,			//4
	&Object_VolUpDown_FS,						//5
	&Object_VolUp,								//6
	&Object_VolDown,							//7
	&Object_VolStop,							//8
	&Attr_Volume,								//9
	&CBindingStringEntry::Seperator,			//10
	&Object_Play,								//11
	&Object_Stop,								//12
	&Object_Pause,								//13
	&Object_FF,									//14
	&Object_RR,									//15
	&Object_FFWD,								//16
	&Object_FRWD,								//17
	&Object_ExtraIR,							//18
	&CBindingStringEntry::Seperator,			//19
	&Object_Mute								//20
};

template < >
const CBindingStringEntry* const TBindingEntries<CAudioHC4StringEntries,
												 CAudioHC4StringEntries::MAX_SIMPLE,
												 AUDIOHC4_STRINGS_SIMPLE>::
	m_StringEntries[CAudioHC4StringEntries::MAX_SIMPLE] =
{
	&Attr_SourceOn,								//0	
	&Object_OnOff_FS,							//1
	&Object_On,									//2
	&Object_Off,								//3
	&CBindingStringEntry::Seperator,			//4
	&Object_VolUpDown_FS,						//5
	&Object_VolUp,								//6
	&Object_VolDown,							//7
	&Object_VolStop,							//8
	&Attr_Volume,								//9
};

/**********************************************************************************
 * TM_AUDIOHC4_BINDING_INPUT: Nieuwe strings
 **********************************************************************************/
const CBindingStringEntry CAudioHC4InputStringEntries::Event_UnitControlPuls(EV_UNITCONTROLPULS,IDS_EV_CONTROLPULS);
const CBindingStringEntry CAudioHC4InputStringEntries::Event_UnitControlPulsToggle(EV_UNITCONTROLPULSTOGGLE,IDS_EV_CONTROLPULSTOGGLE);
const CBindingStringEntry CAudioHC4InputStringEntries::Event_UnitControlToggle(EV_UNITCONTROLTOGGLE,IDS_EV_CONTROLTOGGLE);

template < >
const CBindingStringEntry* const TBindingEntries<CAudioHC4InputStringEntries,
												 CAudioHC4InputStringEntries::MAX_BASIC,
												 AUDIOHC4_INPUTSTRINGS_BASIC>::
	m_StringEntries[CAudioHC4InputStringEntries::MAX_BASIC] =
{
	&Event_UnitControlPuls,						// 0	
	&Event_UnitControlPulsToggle,				// 1	
	&Event_UnitControlToggle					// 2
};

/**********************************************************************************
 * TM_AUDIOHC4_BNO: Nieuwe strings
 **********************************************************************************/
const CBindingStringEntry CAudioHC4BnoInputStringEntries::Event_MacroReceived(EV_UNITCONTROLPULS,IDS_MATRIXAUDIO_EVENT_RX_MACRO);	/* Tricky EV_UNITCONTROLPULS gebruiken */
//const CBindingStringEntry CAudioHC4BnoInputStringEntries::Event_Undefined1(EV_UNITCONTROLPULSTOGGLE,IDS_UNDEFINED);
//const CBindingStringEntry CAudioHC4BnoInputStringEntries::Event_Undefined2(EV_UNITCONTROLTOGGLE,IDS_UNDEFINED);



template < >
const CBindingStringEntry* const TBindingEntries<CAudioHC4BnoInputStringEntries,
												 CAudioHC4BnoInputStringEntries::MAX_BASIC,
												 AUDIOHC4_INPUTSTRINGS_BNO_BASIC>::
	m_StringEntries[CAudioHC4BnoInputStringEntries::MAX_BASIC] =
{
	&Event_MacroReceived,						// 0		
};

/**********************************************************************************
 *
 **********************************************************************************/
const CStringEntry CBindingflagStringEntries::NoChange((BYTE)BINDINGFLAGS_STATUS_NOCHANGE, IDS_BINDINGFLAGS_NOCHANGE);
const CStringEntry CBindingflagStringEntries::Pre((BYTE)BINDINGFLAGS_STATUS_PRE, IDS_BINDINGFLAGS_PRE);
const CStringEntry CBindingflagStringEntries::Post((BYTE)BINDINGFLAGS_STATUS_POST, IDS_BINDINGFLAGS_POST);
const CStringEntry CBindingflagStringEntries::Pre_Puls((BYTE)BINDINGFLAGS_STATUS_PREPULS, IDS_BINDINGFLAGS_PREPULS);
const CStringEntry CBindingflagStringEntries::Post_Puls((BYTE)BINDINGFLAGS_STATUS_POSTPULS, IDS_BINDINGFLAGS_POSTPULS);


template < >
const CStringEntry* const TBindingEntries<CBindingflagStringEntries,
										  CBindingflagStringEntries::MAX_BASIC,
										  BINDINGFLAGS_BASIC,
										  CStringEntry>::
	m_StringEntries[CBindingflagStringEntries::MAX_BASIC] =
{
	&NoChange,								//0	
	&Pre,									//1
	&Post,									//2
};

template < >
const CStringEntry* const TBindingEntries<CBindingflagStringEntries,
										  CBindingflagStringEntries::MAX_EXTENDED,
										  BINDINGFLAGS_EXTENDED,
										  CStringEntry>::
	m_StringEntries[CBindingflagStringEntries::MAX_EXTENDED] =
{
	&NoChange,								//0	
	&Pre,									//1
	&Post,									//2
	&Pre_Puls,								//3
	&Post_Puls,								//4
};


/**********************************************************************************
 *
 **********************************************************************************/
// TM_CHANGES_BINDINGS_ALARM

const CBindingStringEntry CAlarmOutputStringEntries::Object_Arm(ALARM_OBJECT_METHOD_ARM,IDS_ALARM_ARM);
const CBindingStringEntry CAlarmOutputStringEntries::Object_Keypad(ALARM_OBJECT_METHOD_KEYPAD,IDS_ALARM_KEYPAD);

template < >
const CBindingStringEntry* const TBindingEntries<CAlarmOutputStringEntries,
												 CAlarmOutputStringEntries::MAX_OUTPUTS,
												 ALARM_OUTPUTSTRING>::
	m_StringEntries[CAlarmOutputStringEntries::MAX_OUTPUTS] =
{
	&Object_Arm,						// 0	
	&Object_Keypad						// 1
};

const CBindingStringEntry CAlarmInputStringEntries::Event_AlarmInput(ALARM_EVENT_ALARM_INPUT,IDS_ALARM_EVENT_ZONE);
const CBindingStringEntry CAlarmInputStringEntries::Event_AlarmArmed(ALARM_EVENT_ALARM_ARM,IDS_ALARM_EVENT_ARMED);
const CBindingStringEntry CAlarmInputStringEntries::Event_AlarmBurglary(ALARM_EVENT_ALARM_BURGLARY,IDS_ALARM_EVENT_BURGLARY);

template < >
const CBindingStringEntry* const TBindingEntries<CAlarmInputStringEntries,
												 CAlarmInputStringEntries::MAX_BASIC,
												 ALARM_INPUTSTRINGS_BASIC>::
	m_StringEntries[CAlarmInputStringEntries::MAX_BASIC] =
{
	&Event_AlarmInput,					// 0	
};

template < >
const CBindingStringEntry* const TBindingEntries<CAlarmInputStringEntries,
												 CAlarmInputStringEntries::MAX_EXTENDED,
												 ALARM_INPUTSTRINGS_EXTENDED>::
	m_StringEntries[CAlarmInputStringEntries::MAX_EXTENDED] =
{
	&Event_AlarmInput,					// 0
	&Event_AlarmArmed,					// 1
	&Event_AlarmBurglary				// 2
};

/**********************************************************************************/
#endif



/**********************************************************************************
 *
 **********************************************************************************/
class CStringEntries_Collection_alg
{
protected:
	const CStringEntries* const* m_pEntries;
	const int MAX_ELEMENTS;

public:
	CStringEntries_Collection_alg(const CStringEntries* const* pEntries,const int Max) :
	  m_pEntries(pEntries),
	  MAX_ELEMENTS(Max){};

public:
	const CStringEntries* Get(int key) const;	

	const CStringEntries* Get(int extended, 
							  int basic, 
							  BYTE bMethodData) const;

	const CStringEntries* Get(int extended, 
							  int basic, 
							  int simple,
							  BYTE bMethodData) const;
};


/************************************************************************************
 *
 ************************************************************************************/
class CBindingStringEntries_Collection
{
protected:

	CStringEntries_Collection_alg Algoritme;


	#define TEMPLATE_STRINGENTRY_DEF(X,Y,Z,T)			typedef TBindingEntries<X,X::Y,Z,T>	

	TEMPLATE_STRINGENTRY_DEF(CBindingflagStringEntries,
							 MAX_BASIC,
							 BINDINGFLAGS_BASIC,
							 CStringEntry)				CBindingFlagsStringEntry_Basic;

	TEMPLATE_STRINGENTRY_DEF(CBindingflagStringEntries,
							 MAX_EXTENDED,
							 BINDINGFLAGS_EXTENDED,
							 CStringEntry)				CBindingFlagsStringEntry_Extended;


protected:
	CBindingStringEntries_Collection();

public:
	static const CBindingStringEntries_Collection* Instance();

	const CStringEntries* Get(COLLECTION_KEY2 key) const 
	{	
		return (Algoritme.Get(key));	
	}

	const CStringEntries* Get(COLLECTION_KEY2 extended, 
									 COLLECTION_KEY2 basic, 
									 BYTE bMethodData) const
	{
		return (Algoritme.Get(extended,basic,bMethodData));
	}

	const CStringEntries* Get(COLLECTION_KEY2 extended, 
									 COLLECTION_KEY2 basic, 
									 COLLECTION_KEY2 simple,
									 BYTE bMethodData) const
	{
		return (Algoritme.Get(extended,basic,simple,bMethodData));
	}

protected:
	const CStringEntries*					m_Entries[MAX_COLLECTION];

	CBindingFlagsStringEntry_Basic			m_BindingFlagsBasic;
	CBindingFlagsStringEntry_Extended		m_BindingFlagsExtended;
};




/**********************************************************************************
 *
 **********************************************************************************/
class CBindingUnitStringEntries_Collection
{
protected:
	CStringEntries_Collection_alg Algoritme;

	#define TEMPLATE_DEF(X,Y,Z)	typedef TBindingEntries<X,X::Y,Z>
	
	TEMPLATE_DEF(CDimmerStringEntries,
				 MAX_BASIC,
				 DIMMER_STRINGS_BASIC)			CDimmerStringEntries_Basic;
	TEMPLATE_DEF(CDimmerStringEntries,
				 MAX_EXTENDED,
				 DIMMER_STRINGS_EXTENDED)		CDimmerStringEntries_Extended;
	TEMPLATE_DEF(CDimmerStringEntries,
				 MAX_SIMPLE,
				 DIMMER_STRINGS_SIMPLE)			CDimmerStringEntries_Simple;


	// Sedert versie 0x0B01:
	TEMPLATE_DEF(CDimmerInputStringEntries,
				 MAX_INPUTS,
				 DIMMER_STRINGS_INPUT)			CDimmerInputStringEntries;

	TEMPLATE_DEF(CDuoswitchStringEntries,
				 MAX_BASIC,
				 DUOSWITCH_STRINGS_BASIC)		CDuoswitchStringEntries_Basic;
	TEMPLATE_DEF(CDuoswitchStringEntries,
				 MAX_EXTENDED,
				 DUOSWITCH_STRINGS_EXTENDED)	CDuoswitchStringEntries_Extended;

	// Sedert versie 0x0C01:
    TEMPLATE_DEF(CDuoSwitchInputStringEntries,
				 MAX_INPUTS,
				 DUOSWITCH_STRINGS_INPUT)		CDuoSwitchInputStringEntries;

	TEMPLATE_DEF(CSwitchStringEntries,
				 MAX_BASIC,
				 SWITCH_STRINGS_BASIC)			CSwitchStringEntries_Basic;
	TEMPLATE_DEF(CSwitchStringEntries,
				 MAX_EXTENDED,
				 SWITCH_STRINGS_EXTENDED)		CSwitchStringEntries_Extended;	

	// Sedert versie 0x0C01:
    TEMPLATE_DEF(CSwitchInputStringEntries,
				 MAX_INPUTS,
				 SWITCH_STRINGS_INPUT)			CSwitchInputStringEntries;

	TEMPLATE_DEF(CSensStringEntries,
				 MAX_BASIC,
				 SENS_STRINGS_BASIC)			CSensStringEntries_Basic;
	TEMPLATE_DEF(CSensStringEntries,
				 MAX_EXTENDED,
				 SENS_STRINGS_EXTENDED)			CSensStringEntries_Extended;
	TEMPLATE_DEF(CSensStringEntries,
				 MAX_INPUT,
				 SENS_STRINGS_INPUT)			CSensStringEntries_Input;

	TEMPLATE_DEF(CSensUnivStringEntries,
				 MAX_INPUT,
				 SENSUNIV_STRINGS_INPUT)		CSensUnivStringEntries_Input;

	TEMPLATE_DEF(CIRTXStringEntries,
				 MAX_BASIC,
				 IRTX_STRINGS_BASIC)			CIRTxStringEntries_Basic;
	TEMPLATE_DEF(CIRTXStringEntries,
				 MAX_EXTENDED,
				 IRTX_STRINGS_EXTENDED)			CIRTxStringEntries_Extended;

	TEMPLATE_DEF(CAudioBoseLf48StringEntries,
				 MAX_EXTENDED,
				 BOSELF48_STRINGS_EXTENDED)		CBoseLf48StringEntries_Extended;
	TEMPLATE_DEF(CAudioBoseLf48StringEntries,
				 MAX_BASIC,
				 BOSELF48_STRINGS_BASIC)		CBoseLf48StringEntries_Basic;
	TEMPLATE_DEF(CAudioBoseLf48StringEntries,
				 MAX_SIMPLE,
				 BOSELF48_STRINGS_SIMPLE)		CBoseLf48StringEntries_Simple;

	TEMPLATE_DEF(CAudioBoseLf48InputStringEntries,
				 MAX_BASIC,
				 BOSELF48_INPUTSTRINGS_BASIC)		CBoseLf48InputStringEntries_Basic;
	TEMPLATE_DEF(CAudioBoseLf48InputStringEntries,
				 MAX_EXTENDED,
				 BOSELF48_INPUTSTRINGS_EXTENDED)	CBoseLf48InputStringEntries_Extended;
	TEMPLATE_DEF(CAudioBoseLf48InputStringEntries,
				 MAX_VIRTUALS,
				 BOSELF48_INPUTSTRINGS_VIRTUALS)	CBoseLf48InputStringEntries_Virtuals;

	TEMPLATE_DEF(CAVMatrixStringEntries,
				 MAX_EXTENDED,
				 AVMATRIX_STRINGS_EXTENDED)			CAVMatrixStringEntries_Extended;
	TEMPLATE_DEF(CAVMatrixStringEntries,
				 MAX_BASIC,
				 AVMATRIX_STRINGS_BASIC)			CAVMatrixStringEntries_Basic;

	/* TM_AVMATRIX_INPUT */
	TEMPLATE_DEF(CAVMatrixInputStringEntries,
				 MAX_BASIC,
				 AVMATRIX_INPUTSTRINGS_BASIC)		CAVMatrixInputStringEntries_Basic;	

	TEMPLATE_DEF(CVideomuxStringEntries,
				 MAX_EXTENDED,
				 VIDEOMUX_STRINGS_EXTENDED)			CVideomuxStringEntries_Extended;
	TEMPLATE_DEF(CVideomuxStringEntries,
				 MAX_BASIC,
				 VIDEOMUX_STRINGS_BASIC)			CVideomuxStringEntries_Basic;

	TEMPLATE_DEF(CAudioHC4StringEntries,
				 MAX_EXTENDED,
				 AUDIOHC4_STRINGS_EXTENDED)		CAudioHC4StringEntries_Extended;
	TEMPLATE_DEF(CAudioHC4StringEntries,
				 MAX_BASIC,
				 AUDIOHC4_STRINGS_BASIC)		CAudioHC4StringEntries_Basic;
	TEMPLATE_DEF(CAudioHC4StringEntries,
				 MAX_SIMPLE,
				 AUDIOHC4_STRINGS_SIMPLE)		CAudioHC4StringEntries_Simple;

	/* TM_AUDIOHC4_BINDING_INPUT */
	TEMPLATE_DEF(CAudioHC4InputStringEntries,
				 MAX_BASIC,
				 AUDIOHC4_INPUTSTRINGS_BASIC)	CAudioHC4InputStringEntries_Basic;

	/* TM_AUDIOHC4_BNO */
	TEMPLATE_DEF(CAudioHC4BnoInputStringEntries,
				 MAX_BASIC,
				 AUDIOHC4_INPUTSTRINGS_BNO_BASIC)	CAudioHC4BnoInputStringEntries_Basic;

	TEMPLATE_DEF(CAudioBoseV35StringEntries,
				 MAX_EXTENDED,
				 BOSEV35_STRINGS_EXTENDED)		CBoseV35StringEntries_Extended;
	TEMPLATE_DEF(CAudioBoseV35StringEntries,
				 MAX_BASIC,
				 BOSEV35_STRINGS_BASIC)			CBoseV35StringEntries_Basic;
	TEMPLATE_DEF(CAudioBoseV35StringEntries,
				 MAX_SIMPLE,
				 BOSEV35_STRINGS_SIMPLE)		CBoseV35StringEntries_Simple;
	TEMPLATE_DEF(CAudioBoseV35StringEntries,
				 MAX_VIRTUALS,
				 BOSEV35_STRINGS_VIRTUALS)		CBoseV35StringEntries_Virtuals;

	TEMPLATE_DEF(CAudioBoseV35InputStringEntries,
				 MAX_BASIC,
				 BOSEV35_INPUTSTRINGS_BASIC)	CBoseV35InputStringEntries_Basic;
	TEMPLATE_DEF(CAudioBoseV35InputStringEntries,
				 MAX_EXTENDED,
				 BOSEV35_INPUTSTRINGS_EXTENDED)	CBoseV35InputStringEntries_Extended;
	TEMPLATE_DEF(CAudioBoseV35InputStringEntries,
				 MAX_VIRTUALS,
				 BOSEV35_INPUTSTRINGS_VIRTUALS)	CBoseV35InputStringEntries_Virtuals;

	// TM_CHANGES_BINDINGS_ALARM
	TEMPLATE_DEF(CAlarmOutputStringEntries,
				 MAX_OUTPUTS,
				 ALARM_OUTPUTSTRING)			CAlarmOutputStringEntries;
	TEMPLATE_DEF(CAlarmInputStringEntries,
				 MAX_BASIC,
				 ALARM_INPUTSTRINGS_BASIC)		CAlarmInputStringEntries_Basic;
	TEMPLATE_DEF(CAlarmInputStringEntries,
				 MAX_EXTENDED,
				 ALARM_INPUTSTRINGS_EXTENDED)	CAlarmInputStringEntries_Extended;


protected:
	const CBindingStringEntries*					m_Entries[MAX];

	const CDimmerStringEntries_Extended				m_DimmerExtended;
	const CDimmerStringEntries_Basic				m_DimmerBasic;	
	const CDimmerStringEntries_Simple				m_DimmerSimple;
	const CDimmerInputStringEntries					m_DimmerInput;		// Sedert versie 0x0B01
	const CDuoswitchStringEntries_Extended			m_DuoswitchExtended;
	const CDuoswitchStringEntries_Basic				m_DuoswitchBasic;
	const CSwitchStringEntries_Extended				m_SwitchExtended;
	const CDuoSwitchInputStringEntries				m_DuoSwitchInput;	// Sedert versie 0x0C01
	const CSwitchStringEntries_Basic				m_SwitchBasic;
	const CSwitchInputStringEntries					m_SwitchInput;		// Sedert versie 0x0C01
	const CSensStringEntries_Basic					m_SensBasic;
	const CSensStringEntries_Extended				m_SensExtended;
	const CSensStringEntries_Input					m_SensInput;
	const CSensUnivStringEntries_Input				m_SensUnivInput;	// Sedert versie 0x0D00
	const CIRTxStringEntries_Basic					m_IRTXBasic;
	const CIRTxStringEntries_Extended				m_IRTXExtended;
	const CBoseLf48StringEntries_Extended			m_BoseLf48Extended;
	const CBoseLf48StringEntries_Basic				m_BoseLf48Basic;
	const CBoseLf48StringEntries_Simple				m_BoseLf48Simple;
	const CBoseLf48InputStringEntries_Basic			m_BoseLf48InputBasic;
	const CBoseLf48InputStringEntries_Extended		m_BoseLf48InputExtended;
	const CBoseLf48InputStringEntries_Virtuals		m_BoseLf48InputVirtuals;
	const CAVMatrixStringEntries_Extended			m_AvMatrixExtended;
	const CAVMatrixStringEntries_Basic				m_AvMatrixBasic;
	const CAVMatrixInputStringEntries_Basic			m_AvMatrixInputBasic;		/* TM_AVMATRIX_INPUT */
	const CVideomuxStringEntries_Extended			m_VideomuxExtended;
	const CVideomuxStringEntries_Basic				m_VideomuxBasic;
	const CAudioHC4StringEntries_Basic				m_AudioHC4Basic;
	const CAudioHC4StringEntries_Simple				m_AudioHC4Simple;
	const CAudioHC4StringEntries_Extended			m_AudioHC4Extended;
	const CAudioHC4InputStringEntries_Basic			m_AudioHC4InputBasic;		/* TM_AUDIOHC4_BINDING_INPUT */
	const CAudioHC4BnoInputStringEntries_Basic		m_AudioHC4BnoInputBasic;	/* TM_AUDIOHC4_BNO */
	const CBoseV35StringEntries_Extended			m_BoseV35Extended;
	const CBoseV35StringEntries_Basic				m_BoseV35Basic;
	const CBoseV35StringEntries_Simple				m_BoseV35Simple;
	const CBoseV35StringEntries_Virtuals			m_BoseV35Virtuals;
	const CBoseV35InputStringEntries_Basic			m_BoseV35InputBasic;
	const CBoseV35InputStringEntries_Extended		m_BoseV35InputExtended;
	const CBoseV35InputStringEntries_Virtuals		m_BoseV35InputVirtuals;

	// TM_CHANGES_BINDINGS_ALARM
	const CAlarmOutputStringEntries					m_AlarmOutputs;
	const CAlarmInputStringEntries_Basic			m_AlarmInputBasic;
	const CAlarmInputStringEntries_Extended			m_AlarmInputExtended;

protected:
	CBindingUnitStringEntries_Collection();

public:
	static CBindingUnitStringEntries_Collection* Instance();

	const CBindingStringEntries* Get(COLLECTION_KEY key) const;	
	const CBindingStringEntries* Get(COLLECTION_KEY extended, 
									 COLLECTION_KEY basic, 
									 BYTE bMethodData) const;
	const CBindingStringEntries* Get(COLLECTION_KEY extended, 
									 COLLECTION_KEY basic, 
									 COLLECTION_KEY simple,
									 BYTE bMethodData) const;
};


/**********************************************************************************
 *
 **********************************************************************************/
class CBindingStringEntries_Helper
{
protected:
	const CStringEntries* const m_pStringEntries;

public:
	CBindingStringEntries_Helper(const CStringEntries* const pEntries) : 
	  m_pStringEntries(pEntries) { };
	
	void Insert(CComboBox& comboBox);
	BYTE GetCurrSelData(CComboBox& comboBox);
	void SetCurrSel(CComboBox& comboBox,BYTE bMethodData);
};


/**********************************************************************************
 *
 **********************************************************************************/

class CBindingFlagControl_Helper
{
protected:
	const CStringEntries* const m_pStringEntries;

public:
	CBindingFlagControl_Helper(const CStringEntries* const pEntries) : 
	  m_pStringEntries(pEntries) { };

	void Update(CEdit& editBox,BYTE bBindingFlags);
	void Adjust(BYTE& bBindingFlags,int iStep);

};


};
};
}; // namespace DUOTECNO


#endif




