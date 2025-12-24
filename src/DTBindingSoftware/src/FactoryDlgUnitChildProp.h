// FactoryDlgUnitChildProp.h: interface for the CFactoryDlgUnitChildProp class.
//
// TM_CHANGES_ALARM_ARITECH:
// Deze klasse aangemaakt om dialogen aan te maken ifv het binding unit type
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FACTORYDLGUNITCHILDPROP_H__1B72E8AB_94CC_4D94_AAB7_6E1B33171A96__INCLUDED_)
#define AFX_FACTORYDLGUNITCHILDPROP_H__1B72E8AB_94CC_4D94_AAB7_6E1B33171A96__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CDlgAlarmChildProp;
class CBindingPropertyUnit;
class CDlgAvMatrixChildProp;

// TODO:
// Eventueel deze klasse uitbreiden voor het aanmakan van alle property dialogen.
//
// CDlgBoseChildProp
// CDlgSensPropChild
// CDlgDuoSwitchPropChild
// CDlgSwitchPropChild
// CDlgDimmerChildProp

struct CFactoryDlgUnitChildProp  
{
	static CDlgAlarmChildProp* createDlgAlarm(const CBindingPropertyUnit* pBindingUnit);

	// Deze kan NULL teruggeven wanneer het niet nodig is om er ene te tonen...
	static CDlgAvMatrixChildProp* createDlgAVMatrix(const CBindingPropertyUnit* pBindingUnit);
};

#endif // !defined(AFX_FACTORYDLGUNITCHILDPROP_H__1B72E8AB_94CC_4D94_AAB7_6E1B33171A96__INCLUDED_)
