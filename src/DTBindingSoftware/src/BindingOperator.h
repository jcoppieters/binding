// BindingOperator.h: interface for the CBindingOperator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGOPERATOR_H__C9953FA4_765C_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_BINDINGOPERATOR_H__C9953FA4_765C_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "BindingConstants.h"

//////////////////////////////////////////////////////////////////////
class CBindingOperator  
{
public:
	CBindingOperator();
	virtual ~CBindingOperator();

protected:
	unsigned short nID;
	OperatorType Type;
	float xPos,yPos;
	unsigned long ulApplicationFlags;

public:
	static OperatorType Convert(const char cOperator);

public:
	OperatorType GetType() const;
	void SetType(OperatorType);

	void SetID(unsigned short id);
	unsigned short GetID(void) const;

	void SetPosition(float x,float y);
	void GetPosition(float *x,float *y) const;

	// Todo virtual --> must be inherited !!!!
	//virtual BOOL Copy(CBindingOperator *pSource);

	void SetApplicationFlags(unsigned long x) {
		ulApplicationFlags = x; 
	};
	unsigned long GetApplicationFlags(void)	const {
		return ulApplicationFlags; 
	};

	virtual BOOL IsEqual(const CBindingOperator* const pBindingOperator);
};

///////////////////////////////////////////////////////////////////////

class CBindingTimerOperator : public CBindingOperator
{
public:	
	CBindingTimerOperator();

protected:
	TimerType Timer;
	BYTE bValue;

public:
	void SetTimerType(TimerType type);
	TimerType GetTimerType(void) const;

	void SetTimeValue(BYTE bVal);
	BYTE GetTimeValue(void) const;

	static TimerType Convert(const char cTimerType);

	virtual BOOL IsEqual(const CBindingOperator* const pBindingOperator);
};

///////////////////////////////////////////////////////////////////////
class CBindingLogicalOperator : public CBindingOperator
{
public:	
	CBindingLogicalOperator(OperatorType type) 
	{
		SetType(type);
	};
};

///////////////////////////////////////////////////////////////////////


#endif // !defined(AFX_BINDINGOPERATOR_H__C9953FA4_765C_11D8_B865_0050BAC412B1__INCLUDED_)
