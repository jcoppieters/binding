#pragma once

class CBindingUnit;

// Sedert versie 0x0C02

struct CCfgInfoBindingUnit
{
public:
	// Opvragen of een virtual unit als een Control kan gezien worden.
	// Dit is het geval bij: 
	//	smarttouch Oled - De eerste 4 virtuals.
	//  room-e Oled: De eerste 2 virtuals.
	static BOOL getSupportedAsInput(
		const CBindingUnit* pBindingUnit);
};