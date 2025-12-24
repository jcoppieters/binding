#pragma once

#include "bindingunit.h"

class CBindingRC5RXUnit : public CBindingUnit
{
protected:
	virtual void Init(void);		//TM 20040701

public:
	CBindingRC5RXUnit(BYTE bBindingUnitType);
	CBindingRC5RXUnit(const CBindingUnit& pBindingUnit);	// Oppassen: Dit is geen COPY Constructor !!!!!
	/********************************************************************************** 
	 * Oppassen: Copy constructor wordt impliciet aangeroepen.
	 **********************************************************************************/
	virtual ~CBindingRC5RXUnit(); 
	virtual BOOL ConvertToBindingData(void);
	virtual BOOL ConvertFromBindingData(void);
	virtual CString ConvertFuntionToString(void);
	virtual BOOL UpgradeIoType();

protected:
	BYTE bRC5RXEvent;
	BYTE bIRCode;
	BYTE bControlState;

public:
	void Set_Event(BYTE bEvent)		{ bRC5RXEvent = bEvent;		};
	void Set_IRCode(BYTE bCode)		{ bIRCode = bCode;			};
	void Set_State(BYTE bState)		{ bControlState = bState;	};

	BYTE Get_Event(void) const { 
		return bRC5RXEvent;		
	};
	BYTE Get_IRCode(void) const { 
		return bIRCode;			
	};
	BYTE Get_State(void) const { 
		return bControlState;		
	};

private:
	typedef CBindingUnit Base;
};