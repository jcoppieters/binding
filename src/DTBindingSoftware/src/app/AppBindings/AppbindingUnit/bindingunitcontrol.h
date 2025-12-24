#pragma once

#include "bindingunit.h"

class CBindingControlUnit : public CBindingUnit
{
protected:
	virtual void Init(void);		//TM 20040701

public:
	CBindingControlUnit(BYTE bBindingUnitType);
	CBindingControlUnit(const CBindingUnit& pBindingUnit); // Oppassen: Dit is geen COPY Constructor !!!!!
	/********************************************************************************** 
	 * Oppassen: Copy constructor wordt impliciet aangeroepen.
	 **********************************************************************************/
	virtual ~CBindingControlUnit(); 
	virtual BOOL ConvertToBindingData(void);
	virtual BOOL ConvertFromBindingData(void);
	virtual CString ConvertFuntionToString(void);
	virtual BOOL UpgradeIoType();

protected:
	BYTE bControlEvent;
	BYTE bControlState;

public:
	void Set_Event(BYTE bEvent) { bControlEvent = bEvent; };
	void Set_ControlState(BYTE bState) {bControlState = bState; };

	BYTE Get_Event(void) const { 
		return bControlEvent; 
	};
	BYTE Get_ControlState(void) const {
		return bControlState; 
	};

private:
	typedef CBindingUnit Base;
};