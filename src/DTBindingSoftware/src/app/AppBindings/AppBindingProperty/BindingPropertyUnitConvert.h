// BindingPropertyUnitConvert.h: interface for the CBindingPropertyUnitConvert class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGPROPERTYUNITCONVERT_H__05D84909_0FFF_4CB6_B420_06F7430A6E4D__INCLUDED_)
#define AFX_BINDINGPROPERTYUNITCONVERT_H__05D84909_0FFF_4CB6_B420_06F7430A6E4D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CBindingUnit;
class CBindingPropertyUnit;
class CLogFileConversionFromNodes;
/**************************************************************************************/

/**************************************************************************************/
/**************************************************************************************/
/**************************************************************************************/
class CBindingPropertyUnitConvert  
{
protected:
	const CBindingUnit* const m_pBindingUnit;
	CBindingPropertyUnit* const m_pBindingPropertyUnit;
	CLogFileConversionFromNodes* const m_pLogFile;

public:
	CBindingPropertyUnitConvert(const CBindingUnit* const pBindingUnit,
								CBindingPropertyUnit* const pBindingPropertyUnit);

	virtual ~CBindingPropertyUnitConvert();

public:
	/* Factory method ... */
	static CBindingPropertyUnitConvert* Create(const CBindingUnit* const pBindingUnit,
											   CBindingPropertyUnit* const pBindingPropertyUnit);

protected:
	virtual BOOL _Update();

public:
	BOOL Update();
};
/**************************************************************************************/

/**************************************************************************************/
/**************************************************************************************/
/**************************************************************************************/
class CBindingPropertyUnitConvertDim : public CBindingPropertyUnitConvert
{
public:
	CBindingPropertyUnitConvertDim(const CBindingUnit* const pBindingUnit,
								   CBindingPropertyUnit* const pBindingPropertyUnit) :
		CBindingPropertyUnitConvert(pBindingUnit,pBindingPropertyUnit)
		{ };
										
	virtual ~CBindingPropertyUnitConvertDim() {};

protected:
	virtual BOOL _Update();
};
/**************************************************************************************/

/**************************************************************************************/
/**************************************************************************************/
/**************************************************************************************/
class CBindingPropertyUnitConvertSwitch : public CBindingPropertyUnitConvert
{
public:
	CBindingPropertyUnitConvertSwitch(const CBindingUnit* const pBindingUnit,
								   CBindingPropertyUnit* const pBindingPropertyUnit) :
		CBindingPropertyUnitConvert(pBindingUnit,pBindingPropertyUnit)
		{ };
										
	virtual ~CBindingPropertyUnitConvertSwitch() {};

protected:
	virtual BOOL _Update();
};
/**************************************************************************************/

/**************************************************************************************/
/**************************************************************************************/
/**************************************************************************************/
class CBindingPropertyUnitConvertDuoswitch : public CBindingPropertyUnitConvert
{
public:
	CBindingPropertyUnitConvertDuoswitch(const CBindingUnit* const pBindingUnit,
								   CBindingPropertyUnit* const pBindingPropertyUnit) :
		CBindingPropertyUnitConvert(pBindingUnit,pBindingPropertyUnit)
		{ };
										
	virtual ~CBindingPropertyUnitConvertDuoswitch() {};

protected:
	virtual BOOL _Update();
};
/**************************************************************************************/

/**************************************************************************************/
/**************************************************************************************/
/**************************************************************************************/
class CBindingPropertyUnitConvertSens : public CBindingPropertyUnitConvert
{
public:
	CBindingPropertyUnitConvertSens(const CBindingUnit* const pBindingUnit,
								   CBindingPropertyUnit* const pBindingPropertyUnit) :
		CBindingPropertyUnitConvert(pBindingUnit,pBindingPropertyUnit)
		{ };
										
	virtual ~CBindingPropertyUnitConvertSens() {};

protected:
	virtual BOOL _Update();
};
/**************************************************************************************/

/**************************************************************************************/
/**************************************************************************************/
/**************************************************************************************/
class CBindingPropertyUnitConvertAlarm : public CBindingPropertyUnitConvert
{
public:
	CBindingPropertyUnitConvertAlarm(const CBindingUnit* const pBindingUnit,
								   CBindingPropertyUnit* const pBindingPropertyUnit) :
		CBindingPropertyUnitConvert(pBindingUnit,pBindingPropertyUnit)
		{ };
										
	virtual ~CBindingPropertyUnitConvertAlarm() {};

protected:
	virtual BOOL _Update();
};
/**************************************************************************************/

/**************************************************************************************/
/**************************************************************************************/
/**************************************************************************************/
class CBindingPropertyUnitConvertAVMatrix : public CBindingPropertyUnitConvert
{
public:
	CBindingPropertyUnitConvertAVMatrix(const CBindingUnit* const pBindingUnit,
								   CBindingPropertyUnit* const pBindingPropertyUnit) :
		CBindingPropertyUnitConvert(pBindingUnit,pBindingPropertyUnit) { };										
	virtual ~CBindingPropertyUnitConvertAVMatrix() {};

protected:
	virtual BOOL _Update();
};
/**************************************************************************************/




#endif // !defined(AFX_BINDINGPROPERTYUNITCONVERT_H__05D84909_0FFF_4CB6_B420_06F7430A6E4D__INCLUDED_)
