#ifndef INCLUDED_BINDINGCONVERSIONAUTO2_H
#define INCLUDED_BINDINGCONVERSIONAUTO2_H
/*****************************************************************************************************/
#include "cannode.h"				// STRUCT_UNIT
#include "BindingConstants.h"		// enums
#include "BindingEntry.h"
#include "CanNode.h"
/*****************************************************************************************************/
class CBindingEntries;
class CNodeDatabase;
/*****************************************************************************************************/

/*****************************************************************************************************/
/*****************************************************************************************************/
/*****************************************************************************************************/
class CBindingConversionAuto2
{
public:
	CBindingConversionAuto2(CBindingEntries& refBindingEntries);
	CBindingConversionAuto2(CBindingEntries& refBindingEntries,const CString& bindingName);

		// Bepaalt de unit...
	class Unit {
		public:
			Unit(const CString& strName,int n,int u,int t) : name(strName), nodeAddress(n), unitAddress(u), unitType(t) { }
			const CString name;
			const int nodeAddress;
			const int unitAddress;
			const int unitType;
	};

	typedef enum {
		RESULT_SUCCESS = 0,
		RESULT_EC_GENERAL = -1,				// algemeen - params
		RESULT_EC_NODEDATABASE = -2,		// Units niet gevonden.
		RESULT_EC_BINDINGS = -3				// Bindings konden niet aangemaakt worden.
	
	} Result_t;


protected:
	// Helper - zoeken naar de units...
	BOOL findUnit(CNodeDatabase& refNodeDatabase,const Unit& refUnitInfo,STRUCT_UNIT** ppUnit,BOOL updateName);
	BOOL findUnits(CNodeDatabase& refNodeDatabase,CList<const Unit*>& listInputs,CList<STRUCT_UNIT*>* pListUnits,BOOL updateName);

	// Helper - toevoegen van binding units....
	int addOutputVirtual(CBindingEntry* pDest,STRUCT_UNIT* pUnit,int function,int stateOnOff);
	int addInputVirtual(CBindingEntry* pDest,STRUCT_UNIT* pUnit,int ev,int eventData);
	int addInputControl(CBindingEntry* pDest,STRUCT_UNIT* pUnit,int ev,int eventData);

	// Links aanmaken
	void addLinkInput2Operator(CBindingEntry* pDest,int inputId,int operatorId);
	void addLinkOperator2Output(CBindingEntry* pDest,int operatorId,int outputId);
	void addLinkUnits(CBindingEntry* pDest,int inputId,int outputId);

	// Binding entry aanmaken.
	BOOL addBindingEntry(CBindingEntry& bindingEntry);

protected:
	// private:
	CBindingEntries& m_refBindingEntries;
	CString m_bindingName;
};
/*****************************************************************************************************/


/*****************************************************************************************************/
/*****************************************************************************************************/
/*****************************************************************************************************/
// Sedert V16.16
class CBindingConversionAuto2_ConditionStatus : public CBindingConversionAuto2
{
public:
	CBindingConversionAuto2_ConditionStatus(CBindingEntries& refBindingEntries);
	CBindingConversionAuto2_ConditionStatus(CBindingEntries& refBindingEntries,const CString& bindingName);


	// Aanmaken van een conditie binding.
	// Inputs: Event 02 + 1
	// Output: Short puls + state
	Result_t generate(CNodeDatabase& refNodeDatabase, 
					  CList<const Unit*>& inputs, const Unit& outputs, 
					  int bindingFlags = BINDINGFLAGS_STATUS_PRE,
				  	  OperatorType type = PM_OPERATOR_AND);
	
protected:
	BOOL generate_autobinding_condition(CList<STRUCT_UNIT*>& listInputs, STRUCT_UNIT* pOutput, 
										int bindingFlags, OperatorType type = PM_OPERATOR_AND);

	
	unsigned short addInput(CBindingEntry* pDest,STRUCT_UNIT* pUnit);
	unsigned short addOutput(CBindingEntry* pDest,STRUCT_UNIT* pUnit);
};
/*****************************************************************************************************/

/*****************************************************************************************************/
/*****************************************************************************************************/
/*****************************************************************************************************/
class CBindingConversionAuto2_Forward : public CBindingConversionAuto2
{
public:
	CBindingConversionAuto2_Forward(CBindingEntries& refBindingEntries);
	CBindingConversionAuto2_Forward(CBindingEntries& refBindingEntries,const CString& bindingName);

	// Aanmaken van een forward long binding.
	// Inputs: Event 01 + 1
	// Output: Short pulstoggle + state
	Result_t generate(CNodeDatabase& refNodeDatabase, 
					  const Unit& input, const Unit& output, 
					  int bindingFlags = BINDINGFLAGS_STATUS_POST);

protected:
	BOOL generate_autobinding_ForwardLong(STRUCT_UNIT* pInput, STRUCT_UNIT* pOutput, int bindingFlags);

	unsigned short addInput(CBindingEntry* pDest,STRUCT_UNIT* pUnit);
	unsigned short addOutput(CBindingEntry* pDest,STRUCT_UNIT* pUnit);

};
/*****************************************************************************************************/


/*****************************************************************************************************/
/*
 * - generateMasterStatusCBindings( )
 *		--> generateBindindCondition0( ) + generateBindindCondition1() + generateBindindCondition2()
 *
 * Aanmaken van status helper bindings in de master. 
 *
 *****************************************************************************************************
 * - generateBindindCondition0
 *	 R0: out1				(0xFC,0x00)		Event Short puls + state 1
 *	 R1: out2				(0xFC,0x01)		Event Short puls + state 1
 *	 R2: Help virtual		(0xFC,0x09)		Short pulstoggle+state on
 *	 R0 * R1 => R2							PRE
 *****************************************************************************************************
 * - generateBindindCondition1
 *	 R0: Twilight outside	(0xFC,0x02)		Event Short puls + state 1
 *	 R1: Dark outside       (0xFC,0x03)		Event Short puls + state 1
 *	 R2: After 22h          (0xFC,0x04)		Event Short puls + state 1
 *	 R3: Help virtual		(0xFC,0x09)		Short pulstoggle+state on
 *	 R0 * R1 * R2 => R3						PRE
 *****************************************************************************************************
 *   R0: Condition 1        (0xFC,0x05)		Event Short puls + state 1
 *	 R1: Condition 2        (0xFC,0x06)		Event Short puls + state 1
 *	 R2: Condition 3        (0xFC,0x07)		Event Short puls + state 1
 *	 R3: Condition 4	    (0xFC,0x08)		Event Short puls + state 1
 *	 R4: Help virtual		(0xFC,0x09)		Short pulstoggle+state on
 *	 R0 * R1 * R2 * R3 => R4				PRE
 *****************************************************************************************************/
struct CBindingConversionAuto2_ConditionStatus_Helper
{
public:
	// Aanmaken van de status C-Binding 
	static int generateMasterStatusCBindings(CBindingEntries& refBindingEntries,CNodeDatabase& refNodeDatabase);

private:
	// Helper functies voor aanmaken van de status C-Bindings in de master.
	static BOOL generateBindindCondition0(CBindingEntries& refBindingEntries,CNodeDatabase& refNodeDatabase);
	static BOOL generateBindindCondition1(CBindingEntries& refBindingEntries,CNodeDatabase& refNodeDatabase);
	static BOOL generateBindindCondition2(CBindingEntries& refBindingEntries,CNodeDatabase& refNodeDatabase);
};
/*****************************************************************************************************/

/*****************************************************************************************************/
/*****************************************************************************************************/
/*****************************************************************************************************/

/*****************************************************************************************************/
/*
 * - generateTwilightForwardBindings( )
 *		--> generateTwilightBinding0( ) + generateTwilightBinding1()
 *
 * Aanmaken van forward bindings van de twilight module naar de condities van de master.
 *
 *****************************************************************************************************
 * - generateTwilightBinding0
 *	 R0: Twilight		(node,0x0D)				Event long
 *	 R1: Twilight		(0xfc,0x02)	Output.		Short pulstoggle+state on
 *	 R0 --> R1									POST
 *****************************************************************************************************
 * - generateTwilightBinding1
 *	 R0: Dark			(node,0x0F)				Event long
 *	 R1: Dark outside	(0xfc,0x03)	Output.		Short pulstoggle+state on 
 *	 R0 --> R1									POST
 *****************************************************************************************************/
struct CBindingConversionAuto2_Forward_Helper 
{
public:
	// Aanmaken van forward bindings naar de conditie inputs van de master.
	static int generateTwilightForwardBindings(CBindingEntries& refBindingEntries,CNodeDatabase& refNodeDatabase);

private:
	// Helper functies voor forward bindings naar de conditie inputs van de master.
	static int findNodeTwilight(CNodeDatabase& refNodeDatabase);		// zoeken naar een twilight module.
	static BOOL generateTwilightBinding0(CBindingEntries& refBindingEntries,CNodeDatabase& refNodeDatabase,int nodeAddress);
	static BOOL generateTwilightBinding1(CBindingEntries& refBindingEntries,CNodeDatabase& refNodeDatabase,int nodeAddress);
};
/*****************************************************************************************************/

#endif /* INCLUDED_BINDINGCONVERSIONAUTO2_H */