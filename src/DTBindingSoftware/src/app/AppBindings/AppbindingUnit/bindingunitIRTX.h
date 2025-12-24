#pragma once

#include "bindingunit.h"



class CBindingIRTXUnit : public CBindingUnit
{
protected:
	virtual void Init(void);		//TM 20040701

public:
	CBindingIRTXUnit(BYTE bBindingUnitType); 
	CBindingIRTXUnit(const CBindingUnit& pBindingUnit); // Oppassen: Dit is geen COPY Constructor !!!!!
	/********************************************************************************** 
	 * Oppassen: Copy constructor wordt impliciet aangeroepen.
	 **********************************************************************************/
	virtual ~CBindingIRTXUnit(); 
	virtual BOOL ConvertToBindingData(void);
	virtual BOOL ConvertFromBindingData(void);
	virtual CString ConvertFuntionToString(void);
	virtual BOOL UpgradeIoType();

protected:
	BYTE bFunctionData;
	unsigned short usIRCode;

public:
	void Set_Function(BYTE bData) {bFunctionData = bData;};
	void Set_IRCode(unsigned short usCode) {usIRCode = usCode; };

	BYTE Get_Function(void) const {
		return bFunctionData; 
	};
	unsigned short Get_IRCode(void) const {
		return usIRCode; 
	};

private:
	typedef CBindingUnit Base;
};