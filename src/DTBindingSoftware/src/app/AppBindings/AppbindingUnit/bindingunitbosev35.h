#pragma once

#include "bindingunit.h"
#include "bindingunitaudiobasic_def.h"
#include "bindingunitbosev35_def.h"



/*********************************************************************//**
 * @class	CBindingBoseV35Unit_Helper en CIteratorBindingFunctions
 * @detail	iteratie over de binding tabel.
 *			wordt gebruikt door de GUI om de bron en functie te verhogen.
 *************************************************************************/
class CBindingBoseV35Unit_Helper
{
protected:
	enum 
	{
		// De Source Binding functies is niet de volledige tabel.
		MAX_FUNCTIONS = BOSEV35_SOURCE_FUNCTION_SHUFFLE + 1
	};

public:
	class CIteratorBindingFunctions
	{		
	public:
		void incSource( );
		void decSource( );
		void incFunction( );
		void decFunction( );	
		void beginFunction( );

		CString getString( );
		int getSource( ) const { return m_source; };
		int getFunction( )const {return m_function; };

	protected:
		CIteratorBindingFunctions(int source,int function) :
			 m_function(function),
			 m_source(source) { }
		~CIteratorBindingFunctions( ) {	}
	
	private:
		int m_source;
		int m_function;

		friend class CBindingBoseV35Unit_Helper;
	};

public:

	static bool create_Iterator(int source,int function,CIteratorBindingFunctions** pIterator);
	static void delete_Iterator(CIteratorBindingFunctions* pIterator);

#ifdef _DEBUG
	static int testDriver( );
#endif

	friend class CIteratorBindingFunctions;
};
 //////////////

class CBindingBoseV35Unit : public CBindingUnit
{
private:
	static const BYTE bindingFlagTabel[ BOSEV35_MAX_BINDINGFLAG_FUNCTIONS ][ BOSEV35_MAX_SOURCES ];

public:
	CBindingBoseV35Unit(BYTE bBindingUnitType);
	CBindingBoseV35Unit(const CBindingUnit& pBindingUnit); // Oppassen: Dit is geen COPY Constructor !!!!!

	/********************************************************************************** 
	 * Oppassen: Copy constructor wordt impliciet aangeroepen.
	 **********************************************************************************/

	virtual ~CBindingBoseV35Unit(); 
	virtual BOOL ConvertToBindingData(void);
	virtual BOOL ConvertFromBindingData(void);
	virtual CString ConvertFuntionToString(void);
	virtual BOOL UpgradeIoType();

	void Set_Event(BYTE bEvent)				{bBoseLfEvent = bEvent;};
	void Set_Function(BYTE bData);		//	{bFunctionData = bData;};
	void Set_Source(BYTE bSource)			{bSourceSelection = bSource;};
	void Set_State(BYTE bState)				{bDestState = bState; };
	void Set_SrcFunction(BYTE bFunction)	{bSrcFunction = bFunction; };
	void Set_DestFunction(BYTE bFunction)	{bDestFunction =  bFunction; };
	void Set_Button(BYTE bButton)			{bSelKeyButton = bButton; };
	void Set_TKFunction(BYTE bFunction)		{bTKFunction = bFunction; };
	void Set_ControlState(BOOL flag)		{fControlState = flag; };
	void Set_ControlEvent(BYTE bEvent)		{bControlEvent = bEvent; };
	void Set_FunctionExtraIR(BYTE func)		{bFunctionExtraIR = func; };
	void Set_Volume(BYTE vol)				{bVolume = vol; };
	void Set_DestFunctionExtraIR(BYTE func) {bDestFunctionExtraIR = func; }



	BYTE Get_Function(void)	const {
		return bFunctionData; 
	};
	BYTE Get_Event(void) const {
		return bBoseLfEvent; 
	};
	BYTE Get_Source(void) const {
		return bSourceSelection;
	};
	BYTE Get_State(void) const {
		return bDestState; 
	};
	BYTE Get_SrcFunction(void) const {
		return bSrcFunction; 
	};
	BYTE Get_DestFunction(void)	const {
		return bDestFunction; 
	};
	BYTE Get_Button(void) const {
		return bSelKeyButton; 
	};
	BYTE Get_TKFunction(void) const	{
		return bTKFunction; 
	};
	BOOL Get_ControlState(void) const {
		return fControlState; 
	};
	BYTE Get_ControlEvent(void) const {
		return bControlEvent; 
	};
	BYTE Get_FunctionExtraIR(void) const {
		return bFunctionExtraIR;
	}
	BYTE Get_Volume(void) const {
		return bVolume;
	}
	BYTE Get_DestFunctionExtraIR( ) const {
		return bDestFunctionExtraIR;
	}
	
	static CString Get_strSource(int src);
	static CString Get_strFunctionExtraIR(int func);
	static CString Get_strFunctionTK(int func);

protected:
	BOOL fControlState;		// Input: Bool ...
	BYTE bFunctionData;		
	BYTE bBoseLfEvent;
	BYTE bControlEvent;		// Input: E01 - E03
	BYTE bSelSource;		// Input		
	BYTE bSelKeyButton;		// Input: 0 - 9 + left,right,up,down,enter
	BYTE bTKFunction;		// Input: 0 - 6
	BYTE bSourceSelection;	// Output: 0 - 8
	BYTE bDestState;		// Output: On/Off
	BYTE bSrcFunction;		// Output: f0-f1
	BYTE bDestFunction;		// Output: f0-f1
	BYTE bFunctionExtraIR;	// Output 
	BYTE bVolume;			// Output
	BYTE bDestFunctionExtraIR;	// Input event


	virtual void Init(void);	
	BYTE ConvertFromBindingTable(BYTE Src,BYTE func);
	BYTE GetSourceFromBindingTable(BYTE flags);
	BYTE GetFunctionFromBindingTable(BYTE flags);

private:
	friend class CBindingBoseV35Unit_Helper;
	friend class CBindingBoseV35Unit_Helper::CIteratorBindingFunctions;

	typedef CBindingUnit Base;
};