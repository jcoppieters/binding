#pragma once

class CDlgBindingUnitProp;
class CBindingUnit;

struct CFactoryDlgBindingUnit
{
	static CDlgBindingUnitProp* Create(
		const CBindingUnit *pBindingUnit);
};