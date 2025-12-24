// BindingConversionAuto.h: interface for the CBindingConversionAuto class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGCONVERSIONAUTO_H__D551550F_E196_4F9F_8A9B_909DAAF9AD36__INCLUDED_)
#define AFX_BINDINGCONVERSIONAUTO_H__D551550F_E196_4F9F_8A9B_909DAAF9AD36__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CBindingUnit;
class CBindingLinks;
class CBindingOperators;
class CBindingUnits;
class CBindingAutoTemplate;

#include "BindingEntry.h"
#include "BindingEntries.h"


///////////////////////////////// AUTO BINDINGS ///////////////////////////////////
enum DimmerAutoOptions 
{
	DIMMER_AUTO_OPTION_ONOFF = 0,
	DIMMER_AUTO_OPTION_PIRON
};

enum RelaisAutoOptions 
{
	RELAIS_AUTO_OPTION_ONOFF = 0,
	RELAIS_AUTO_OPTION_PIRON
};

enum SensAutoOptions 
{
	SENS_AUTO_OPTION_HEATING = 0,
	SENS_AUTO_OPTION_COOLING
};

enum MoodAutoOptions 
{
	MOODS_AUTO_OPTION_SHORT = 0,
	MOODS_AUTO_OPTION_LONG
};

enum AudioAutoOptions
{
	AUDIO_AUTO_OPTION_SOURCE_SIMPLE = 0,
	AUDIO_AUTO_OPTION_SOURCE_INC
};
///////////////////////////////////////////////////////////////////////////////////


typedef struct
{
public:
	enum BoseType
	{
		BOSE_TYPE_LF50	= 0,
		BOSE_TYPE_LF48,
		BOSE_TYPE_V35,
		BOSE_TYPE_UNDEFINED
	};

	BoseType boseType;
	int nBoseUnits;
	int nBoseVirtualUnits;

	int nBindingUnits;
	int nUniqueBindingUnits;			// zie BUG-0183: Aantal binding units met een unieke unit addres.

	int nControlUnits;
	int nSwitchUnits;
	int nDuoSwitchUnits;
	int nIrtxUnits;
	int nSensUnits;	

	struct UNIT
	{
		// Het aantal units.
		int nrOfUnits;

		// Sedert versie 0x0C02:
		// Dit zijn units die (enkel) bedoeld zijn als inputs.
		int nrOfInputs;
	};

	UNIT dimUnit;
	UNIT virtualUnit;

	// Sedert versie 0x0C02.
	int nAudioUnitsExtended;

} CBindingConversionAutoUnitInfo;

//--------------------------------------------------------------------------------
class CBindingConversionAutoInfo
{
protected:
	CStringArray InfoMessages;

public:
	CBindingConversionAutoInfo(){ 
		InfoMessages.RemoveAll(); 
	}
	
	~CBindingConversionAutoInfo(){
		Clear();
	}
	
	void AddMessage(CString s){ 
		InfoMessages.Add(s);
	};

	void Clear(void) {
		InfoMessages.RemoveAll();
		InfoMessages.FreeExtra();
	}

public:
	void Report();
	
protected:
	BOOL GetMessage(int i,CString *s) 
	{
		if(i >= InfoMessages.GetSize()) return FALSE;
		*s = InfoMessages.GetAt(i);
		return TRUE;
	}
	int GetNrMessages(void)	{
		return InfoMessages.GetSize(); 
	};
};

//--------------------------------------------------------------------------------

class CBindingConversionAutoOptions
{
protected:
	// AutoBindings ...
	DimmerAutoOptions CurrentDimmerAutoOptions;
	RelaisAutoOptions CurrentRelaisAutoOptions;
	SensAutoOptions	CurrentSensAutoOptions;
	MoodAutoOptions CurrentMoodsAutoOptions;
	AudioAutoOptions CurrentAudioAutoOptions;

public:

	CBindingConversionAutoOptions() 
	{
		CurrentDimmerAutoOptions = DIMMER_AUTO_OPTION_ONOFF;
		CurrentRelaisAutoOptions = RELAIS_AUTO_OPTION_ONOFF;
		CurrentSensAutoOptions = SENS_AUTO_OPTION_HEATING;
		CurrentMoodsAutoOptions = MOODS_AUTO_OPTION_SHORT;
		CurrentAudioAutoOptions = AUDIO_AUTO_OPTION_SOURCE_SIMPLE;
	}

	DimmerAutoOptions GetAutoDimmerOption(void)	const	{return CurrentDimmerAutoOptions;};
	RelaisAutoOptions GetAutoRelaisOption(void)	const	{return CurrentRelaisAutoOptions;};
	SensAutoOptions GetAutoSensOption(void)	const		{return CurrentSensAutoOptions; };
	MoodAutoOptions GetAutoMoodOption(void)	const		{return CurrentMoodsAutoOptions; };
	AudioAutoOptions GetAutoAudioOption(void) const		{return CurrentAudioAutoOptions; };

	void SetAutoDimmerOption(DimmerAutoOptions x)	{CurrentDimmerAutoOptions = x; };
	void SetAutoRelaisOption(RelaisAutoOptions x)	{CurrentRelaisAutoOptions = x; };
	void SetAutoSensOption(SensAutoOptions x)		{CurrentSensAutoOptions = x; };
	void SetAutoMoodOption(MoodAutoOptions x)		{CurrentMoodsAutoOptions = x; };
	void SetAutoAudioOption(AudioAutoOptions x)		{CurrentAudioAutoOptions = x; };
};

//--------------------------------------------------------------------------------

class CBindingConversionAuto  
{
	CBindingConversionAutoOptions AutoBindingOptions;
	CBindingConversionAutoInfo AutoBindingInfo;
	CBindingEntries AutoBindingEntries;

	// Bevat collectie van de units die momenteel op het werkblad staat
	CBindingUnits* m_pgBindingUnits;

protected :
	void InitNewProcess(void);
	
	// Use function pointer of type Template to invoke functions ...
	// See ProcessAutoBindings ...
	BOOL CheckTemplate_Dimmers(CBindingAutoTemplate **pReturn);
	BOOL CheckTemplate_Relais(CBindingAutoTemplate **pReturn);
	BOOL CheckTemplate_Moods(CBindingAutoTemplate **pReturn);
	BOOL CheckTemplate_Sens(CBindingAutoTemplate **pReturn);
	BOOL CheckTemplate_BoseRs(CBindingAutoTemplate **pReturn);
	BOOL CheckTemplate_DuoSwitch(CBindingAutoTemplate **pReturn);
	BOOL CheckTemplate_CondPir(CBindingAutoTemplate **pReturn);
	BOOL CheckTemplate_Audio(CBindingAutoTemplate **pReturn);
	BOOL CheckTemplate_DimmersFwd(CBindingAutoTemplate **pReturn);

private :
	BYTE bBindingFlags;
	BOOL RetrieveNrOfUnits(void);
	CBindingConversionAutoUnitInfo NrUnitInfo;

public:
	CBindingConversionAuto();
	virtual ~CBindingConversionAuto();

	// returns nr of BindingEntries made ...
	BYTE ProcessAutoBindings(CBindingUnits *pColUnits /*IN*/);
	BOOL GetBindingEntry(int index,CBindingEntry **pReturnEntry);

	CBindingConversionAutoOptions* GetAutoBindingOptions(void) { 
		return (&AutoBindingOptions); 
	}
	CBindingConversionAutoInfo* GetAutoBindingInfo(void) {
		return (&AutoBindingInfo);
	}

public:
	CBindingUnits* GetBindingUnits(void) {return m_pgBindingUnits; };
	CBindingEntries* GetAutoBindingEntries(void) {return &AutoBindingEntries; };
	CBindingConversionAutoUnitInfo* GetBindingUnitInfo(void) {return &NrUnitInfo; };
};

/////////////////////////////////////////////////////////////////
class CBindingAutoUnit
{
protected:
	BYTE bEvent;
	BYTE bFunction;

public:
	CBindingAutoUnit( );
	void SetEvent(BYTE x)			{bEvent = x; };
	void SetFunction(BYTE x)		{bFunction = x; };	
	BYTE GetEvent(void)				{return bEvent; };
	BYTE GetFunction(void)			{return bFunction; };
};


// BUG-0174
// Deze klasse kan gebruikt worden voor het bepalen van de toestand pijl (zwart/rood) van de binding.
class CBindingAutoConditionState : public CBindingAutoUnit {
public:
	CBindingAutoConditionState( );	

	void SetStateInverse(BOOL x)	{m_fStateInverse = x; };
	BOOL IsStateInvers(void)		{return m_fStateInverse; };
	
private:
	BOOL m_fStateInverse;
};


class CBindingAutoDimUnit : public CBindingAutoUnit
{
protected:
	BYTE bDimValue;
	BYTE bPirTime;

public:
	CBindingAutoDimUnit( );
	void SetDimValue(BYTE x)	{bDimValue = x; };
	void SetPirTime(BYTE x)		{bPirTime = x; };
	BYTE GetDimValue(void)		{return bDimValue; };
	BYTE GetPirTime(void)		{return bPirTime; };
};


class CBindingAutoSwitchUnit : public CBindingAutoUnit
{
protected:
	BYTE bPirTime;

public:
	CBindingAutoSwitchUnit( );
	void SetPirTime(BYTE x)		{bPirTime = x; };
	BYTE GetPirTime(void)		{return bPirTime; };
};


class CBindingAutoBoseUnit : public CBindingAutoUnit
{
protected:
	BYTE bSourceSelect;

public:
	CBindingAutoBoseUnit();
	void SetSource(BYTE x)		{bSourceSelect = x; };
	BYTE GetSource(void)		{return bSourceSelect; };
};


class CBindingAutoExtendedHC4 : public CBindingAutoUnit
{
protected:
	BYTE bSourceSelect;
	BYTE bDest;

public:
	CBindingAutoExtendedHC4( );
	void SetSource(BYTE x)		{bSourceSelect = x; };
	BYTE GetSource(void)		{return bSourceSelect; };

	void SetDest(BYTE x)		{bDest = x; };
	BYTE GetDest(void)			{return bDest; };
};


//-----

class CBindingAutoOperator
{
protected:
	OperatorType type;
	BOOL fUsed;

public:
	CBindingAutoOperator();
	void SetType(OperatorType x)	{type = x; };
	OperatorType GetType(void)		{return type; };		
	void SetUsed(BOOL f)			{fUsed = f; };
	BOOL IsUsed(void)				{return fUsed; };
};


////////////////////////////////////////////////////////////////
class CBindingAutoTemplate
{
public:		
	CBindingAutoTemplate(CBindingConversionAuto *pParent);
	virtual ~CBindingAutoTemplate();
	virtual int ProcessTemplate(void) = 0;

protected:
	CBindingConversionAutoUnitInfo* m_pNrUnitInfo;
	CBindingConversionAutoOptions* m_pAutoBindingOptions;
	CBindingConversionAutoInfo* m_pAutoBindingInfo;

	// Bijhouden van de properties.
	CBindingAutoConditionState m_ControlUnitProp;
	CBindingAutoUnit m_DuoSwitchUnitProp;
	CBindingAutoConditionState m_VirtualUnitProp;
	CBindingAutoDimUnit m_DimmerUnitProp;
	CBindingAutoSwitchUnit m_SwitchUnitProp;
	CBindingAutoBoseUnit m_BoseUnitProp;
	CBindingAutoUnit m_SensUnitProp;
	CBindingAutoExtendedHC4 m_AudioUnitProp;
	CBindingAutoConditionState m_CondUnitProp[2];

	CBindingAutoOperator m_InputOperator;
	CBindingAutoOperator m_ConditionalOperator;

	// Use function pointer to process and add bindingentry ...
	// carefully - maybe we must use static cast ???
	typedef BOOL (CBindingAutoTemplate::*Action)(CBindingEntry *p);
	int AddProcessBinding(Action f);

protected:
	CBindingEntries* m_pgBindingEntries;
	CBindingUnits *m_pgBindingUnits;

protected:
	//void ClearBindingEntry(CBindingEntry *pEntry);	

	BOOL AddOperator(OperatorType type,CBindingEntry *pBindingEntry);	
	BOOL AddUnit(CBindingUnit *pUnit,CBindingEntry *pBindingEntry);
	BOOL AddLink(CBindingLink *pLink,CBindingEntry *pBindingEntry);
	BOOL AddLinks(CBindingEntry *pEntry,int nInputs,int nOutputs);
	int CopyUnits(CBindingEntry *pEntry,BYTE bUnitType);
	int CopyUnit(CBindingEntry *pEntry,BYTE bUnitType,int index);
	void AddUnitProperties(CBindingEntry *pEntry,BYTE bUnitType,BOOL fIoType=FALSE);	
	void AddUnitProperties(CBindingEntry* pEntry,const BYTE* bUnitType,int nrOfUnitTypes);
};

//----------------------------------------------------------------
class CBindingAutoTemplate_Dimmer : public CBindingAutoTemplate
{
public:
	CBindingAutoTemplate_Dimmer(CBindingConversionAuto *pParent);
	~CBindingAutoTemplate_Dimmer();
	int ProcessTemplate(void);

protected:
	BOOL ProcessBindingShort(CBindingEntry *pEntry);
	BOOL ProcessBindingLong(CBindingEntry *pEntry);	
	BOOL ProcessBindingPIR(CBindingEntry *pEntry);

protected :
	enum DimmerAutoBindings 
	{
		AUTO_BINDING_DIM_ONOFF = 0,
		AUTO_BINDING_DIM_UPDN,
		AUTO_BINDING_DIM_PIR,
		AUTO_BINDING_DIM_MAX
	};

	CString DimmerAutoStrings[AUTO_BINDING_DIM_MAX];
	Action DimmerAutoBindingActions[AUTO_BINDING_DIM_MAX];
	
	int ProcessBinding(DimmerAutoBindings x) {
		return (AddProcessBinding(DimmerAutoBindingActions[x]));
	}
};

//-----------------------------------------------------------------
class CBindingAutoTemplate_Switch : public CBindingAutoTemplate
{
public:
	CBindingAutoTemplate_Switch(CBindingConversionAuto *pParent);
	~CBindingAutoTemplate_Switch();
	int ProcessTemplate(void);	

protected:
	BOOL ProcessBindingShort(CBindingEntry *pEntry);
	BOOL ProcessBindingPIR(CBindingEntry *pEntry);

protected:	
	enum SwitchAutoBindings 
	{
		AUTO_BINDING_SWITCH_ONOFF = 0,
		AUTO_BINDING_SWITCH_PIR,
		AUTO_BINDING_SWITCH_MAX
	};

	Action SwitchAutoBindingActions[AUTO_BINDING_SWITCH_MAX];
	CString SwitchAutoStrings[AUTO_BINDING_SWITCH_MAX];

	int ProcessBinding(SwitchAutoBindings x) {
		return (AddProcessBinding(SwitchAutoBindingActions[x]));
	}
};


//------------------------------------------------------------------
class CBindingAutoTemplate_DuoSwitch : public CBindingAutoTemplate
{
public:
	CBindingAutoTemplate_DuoSwitch(CBindingConversionAuto *pParent);
	~CBindingAutoTemplate_DuoSwitch();
	int ProcessTemplate(void);

protected:
	BOOL ProcessBindingShort(CBindingEntry *pEntry);
	BOOL ProcessBindingLong(CBindingEntry *pEntry);	

	BOOL ProcessBindingShort_Input1(CBindingEntry *pEntry);
	BOOL ProcessBindingShort_Input2(CBindingEntry *pEntry);

protected:
	enum DuoSwitchAutoBindings 
	{
		AUTO_BINDING_DUOSWITCH_UPDOWN = 0,
		AUTO_BINDING_DUOSWITCH_STOP ,
		AUTO_BINDING_DUOSWITCH_UP,
		AUTO_BINDING_DUOSWITCH_DOWN,
		AUTO_BINDING_DUOSWITCH_MAX
	};

	Action DuoswitchAutoBindingActions[AUTO_BINDING_DUOSWITCH_MAX];
	CString DuoSwitchAutoStrings[AUTO_BINDING_DUOSWITCH_MAX];
	
	int ProcessBinding(DuoSwitchAutoBindings x) {
		return (AddProcessBinding(DuoswitchAutoBindingActions[x]));
	}
};

//------------------------------------------------------------------
class CBindingAutoTemplate_Moods : public CBindingAutoTemplate
{
public:
	CBindingAutoTemplate_Moods(CBindingConversionAuto *pParent);
	~CBindingAutoTemplate_Moods();
	int ProcessTemplate(void);

protected:
	BOOL ProcessBindingShort(CBindingEntry *pEntry);
	BOOL ProcessBindingLong(CBindingEntry *pEntry);
	BOOL ProcessBindingShort_virt(CBindingEntry *pEntry);
	BOOL ProcessBindingLong_virt(CBindingEntry *pEntry);

protected:
	enum MoodAutoBindings 
	{
		AUTO_BINDINGS_MOODS_CONTROL_SHORT = 0,
		AUTO_BINDINGS_MOODS_CONTROL_LONG,
		AUTO_BINDINGS_MOODS_VIRTUAL_SHORT,
		AUTO_BINDINGS_MOODS_VIRTUAL_LONG,
		AUTO_BINDINGS_MOODS_MAX
	};

	Action MoodsAutoBindingActions[AUTO_BINDINGS_MOODS_MAX];
	CString MoodsAutoStrings[AUTO_BINDINGS_MOODS_MAX];
	
	int ProcessBinding(MoodAutoBindings x) {
		return (AddProcessBinding(MoodsAutoBindingActions[x]));
	}	

private:
	int nVirtualShort,nVirtualLong;
};

//------------------------------------------------------------------
class CBindingAutoTemplate_Sens : public CBindingAutoTemplate
{
public:
	CBindingAutoTemplate_Sens(CBindingConversionAuto *pParent);
	~CBindingAutoTemplate_Sens();
	int ProcessTemplate(void);

protected:
	BOOL ProcessBindingHeating(CBindingEntry *pEntry);
	BOOL ProcessBindingCooling(CBindingEntry *pEntry);	

protected:	
	enum SensAutoBindings 
	{
		AUTO_BINDING_SENS_HEATING = 0,
		AUTO_BINDING_SENS_COOLING,
		AUTO_BINDING_SENS_MAX
	};

	Action SensAutoBindingActions[2];
	CString SensAutoStrings[AUTO_BINDING_SENS_MAX];

	int ProcessBinding(SensAutoBindings x) {
		return (AddProcessBinding(SensAutoBindingActions[x]));
	}

private:
	BYTE nSwitchHeat,nSwitchCool;
};

//------------------------------------------------------------------

class CBindingAutoTemplate_Bose : public CBindingAutoTemplate
{
public:
	CBindingAutoTemplate_Bose(CBindingConversionAuto *pParent);
	~CBindingAutoTemplate_Bose();
	int ProcessTemplate(void);

protected:
	BOOL ProcessBindingShort(CBindingEntry *pEntry);
	BOOL ProcessBindingLong(CBindingEntry *pEntry);	
	BOOL ProcessBindingShort_Input1(CBindingEntry *pEntry);
	BOOL ProcessBindingShort_Input2(CBindingEntry *pEntry);
	BOOL ProcessBindingLong_Input1(CBindingEntry *pEntry);
	BOOL ProcessBindingLong_Input2(CBindingEntry *pEntry);
	BOOL ProcessBindingLong_InvInput1(CBindingEntry *pEntry);
	BOOL ProcessBindingLong_InvInput2(CBindingEntry *pEntry);
	BOOL ProcessBindingShort_Inv(CBindingEntry *pEntry);
	BYTE getBoseBindingUnitType(void);

protected:
	enum BoseAutoBindings 
	{
		AUTO_BINDING_BOSE_SOURCE		= 0,
		AUTO_BINDING_BOSE_VOLUPDN		= 1,
		AUTO_BINDING_BOSE_SOURCE_FM		= 2,
		AUTO_BINDING_BOSE_SOURCE_CD		= 3,
		AUTO_BINDING_BOSE_VOLUP			= 4,
		AUTO_BINDING_BOSE_VOLDN			= 5,
		AUTO_BINDING_BOSE_VOLUP_STOP	= 6,
		AUTO_BINDING_BOSE_VOLDN_STOP	= 7,
		AUTO_BINDING_BOSE_DESTOFF		= 8,
		AUTO_BINDING_BOSE_MAX
	};

	Action BoseAutoBindingActions[AUTO_BINDING_BOSE_MAX];
	CString BoseAutoStrings[AUTO_BINDING_BOSE_MAX];
	
	int ProcessBinding(BoseAutoBindings x) 
	{
		return (AddProcessBinding(BoseAutoBindingActions[x]));
	}
};

//------------------------------------------------------------------
class CBindingAutoTemplate_CondPIR : public CBindingAutoTemplate
{
public:
	CBindingAutoTemplate_CondPIR(CBindingConversionAuto *pParent);
	~CBindingAutoTemplate_CondPIR();
	int ProcessTemplate(void);

protected:
	BOOL CheckAddress(BYTE bNodeAddress,BYTE bUnitAddress);

	BOOL ProcessBindingPIR(CBindingEntry *pEntry,BYTE bDimValue,BYTE bPirTime,
						   BOOL fInv,BOOL fInv2);	// used by DK-Buiten & klaar

	BOOL AddConditinalUnitProperties(CBindingEntry *pEntry);
	BOOL AddLinks(CBindingEntry *pEntry,int nInputs,int nOutputs,int nConditions);

//---

	BOOL ProcessBindingPIR_DkBuiten(CBindingEntry *pEntry);
	BOOL ProcessBindingPIR_Klaar(CBindingEntry *pEntry);
	BOOL ProcessBindingPIR_Na22h(CBindingEntry *pEntry);
	BOOL ProcessBindingPIR_Voor22h(CBindingEntry *pEntry);
	BOOL ProcessBindingPIR_Dk99(CBindingEntry *pEntry); // TM 20041115

protected:
	enum ConditionalAutoBindings 
	{
		AUTO_BINDING_COND_DONKERBUITEN = 0,
		AUTO_BINDING_COND_KLAAR,
		AUTO_BINDING_COND_NA22H,
		AUTO_BINDING_COND_VOOR22H,
		AUTO_BINDING_COND_DONKER99,			// TM 20041115
		AUTO_BINDING_COND_MAX
	};

	Action ConditionalBindingActions[AUTO_BINDING_COND_MAX];
	CString ConditionalAutoStrings[AUTO_BINDING_COND_MAX];

	int ProcessBinding(ConditionalAutoBindings x) {
		return (AddProcessBinding(ConditionalBindingActions[x]));
	}
};

//------------------------------------------------------------------

class CBindingAutoTemplate_AudioExt : public CBindingAutoTemplate
{
public:
	CBindingAutoTemplate_AudioExt(CBindingConversionAuto *pParent);
	~CBindingAutoTemplate_AudioExt();
	int ProcessTemplate(void);

protected:
	BOOL ProcessBindingShort_Input1(CBindingEntry *pEntry);		// Source
	BOOL ProcessBindingShort_Input2(CBindingEntry *pEntry);		// Off
	BOOL ProcessBindingLong_Input1(CBindingEntry *pEntry);		// Vol up
	BOOL ProcessBindingLong_Input2(CBindingEntry *pEntry);		// Vol down
	BOOL ProcessBindingLong_InvInput1(CBindingEntry *pEntry);	// Vol Stop
	BOOL ProcessBindingLong_InvInput2(CBindingEntry *pEntry);	// Vol Stop
	BOOL ProcessBindingLong_Input1_PTON(CBindingEntry *pEntry);		// Source
	BOOL ProcessBindingLong_Input1_PTOFF(CBindingEntry *pEntry);	// Source
	BOOL ProcessBindingLong_Input2_SRCINC(CBindingEntry *pEntry);	// Source Inc

protected:
	enum AudioAutoBindings 
	{
		AUTO_BINDING_AUDIO_SOURCE			= 0,
		AUTO_BINDING_AUDIO_VOLUP			= 1,
		AUTO_BINDING_AUDIO_VOLDOWN			= 2,
		AUTO_BINDING_AUDIO_VOLSTOP1			= 3,
		AUTO_BINDING_AUDIO_VOLSTOP2			= 4,
		AUTO_BINDING_AUDIO_DESTOFF			= 5,
		AUTO_BINDING_AUDIO_SOURCE_PTON		= 6,
		AUTO_BINDING_AUDIO_DESTOFF_PTOFF	= 7,
		AUTO_BINDING_AUDIO_SOURCE_INC		= 8,
		AUTO_BINDING_AUDIO_MAX
	};

	Action m_AutoBindingAudioActions[AUTO_BINDING_AUDIO_MAX];
	CString m_AutoAutoStrings[AUTO_BINDING_AUDIO_MAX];
	
	int ProcessBinding(AudioAutoBindings x) 
	{
		REQUIRE(x<AUTO_BINDING_AUDIO_MAX);
		return (AddProcessBinding(m_AutoBindingAudioActions[x]));
	}
};


class CBindingAutoTemplate_DimmerFWD : public CBindingAutoTemplate
{
public:
	CBindingAutoTemplate_DimmerFWD(CBindingConversionAuto *pParent);
	~CBindingAutoTemplate_DimmerFWD();
	int ProcessTemplate(void);

protected:
	BOOL ProcessBinding_Input_Single(CBindingEntry *pEntry);
	BOOL ProcessBinding_Input_Multiple(CBindingEntry *pEntry);

protected:
	enum DimmerFwdAutoBindings 
	{
		AUTO_BINDING_DIMMERFWD_SINGLE	= 0,
		AUTO_BINDING_DIMMERFWD_MULTIPLE	= 1,
		AUTO_BINDING_DIMMERFWD_MAX
	};

	Action m_dimmerFwdAutoBindingActions[AUTO_BINDING_DIMMERFWD_MAX];
	CString m_dimmerFwdAutoStrings[AUTO_BINDING_DIMMERFWD_MAX];
	
	int ProcessBinding(DimmerFwdAutoBindings x) 
	{
		return (AddProcessBinding(m_dimmerFwdAutoBindingActions[x]));
	}
};

/********************************************************************************************/
// Sedert V16.17

/********************************************************************************************/


#endif // !defined(AFX_BINDINGCONVERSIONAUTO_H__D551550F_E196_4F9F_8A9B_909DAAF9AD36__INCLUDED_)
