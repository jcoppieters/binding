#pragma once

#include "bindingunit.h"


class CBindingVirtualUnit : public CBindingUnit
{
protected:
	virtual void Init(void);		//TM 20040701

public:
	CBindingVirtualUnit(BYTE bBindingUnitType); 
	CBindingVirtualUnit(const CBindingUnit& pBindingUnit); // Oppassen: Dit is geen COPY Constructor !!!!!
	/********************************************************************************** 
	 * Oppassen: Copy constructor wordt impliciet aangeroepen.
	 **********************************************************************************/
	virtual ~CBindingVirtualUnit(); 
	virtual BOOL ConvertToBindingData(void);
	virtual BOOL ConvertFromBindingData(void);
	virtual CString ConvertFuntionToString(void);
	virtual BOOL UpgradeIoType();

protected:
	BYTE bControlEvent;
	BYTE bFunctionData;
	BYTE bControlState;

public:
	void Set_Function(BYTE bData) {bFunctionData = bData;};
	void Set_Event(BYTE bEvent) { bControlEvent = bEvent; };
	void Set_ControlState(BYTE bState) {bControlState = bState; };

	BYTE Get_Function(void) const {
		return bFunctionData; 
	};
	BYTE Get_Event(void) const { 
		return bControlEvent; 
	};
	BYTE Get_ControlState(void) const {
		return bControlState; 
	};

private:
	typedef CBindingUnit Base;
};