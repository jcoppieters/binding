// BindingOperator.cpp: implementation of the CBindingOperator class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingOperator.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CBindingOperator::CBindingOperator()
{
	SetPosition(1000,1000);
	SetApplicationFlags(0);
}
CBindingOperator::~CBindingOperator()
{
}
///////////////////////////////////////////////////////////////////////
unsigned short CBindingOperator::GetID(void) const
{
	return (nID);
}

void CBindingOperator::SetID(unsigned short id)
{
	nID = id;
}

OperatorType CBindingOperator::GetType(void) const
{
	return(Type);
}

void CBindingOperator::SetType(OperatorType opType)
{
	Type = opType;
}

void CBindingOperator::SetPosition(float x,float y)
{
	xPos = x;
	yPos = y;
}

void CBindingOperator::GetPosition(float *x,float *y) const
{
	*x = xPos;
	*y = yPos;
}

BOOL CBindingOperator::IsEqual(const CBindingOperator* const pBindingOperator)
{
	if (Type != pBindingOperator->Type) 
	{
		return FALSE;
	}

	return TRUE;
}

/****************************************************************************************
 * OperatorType CBindingOperator::Convert(const char cOperator)
 *
 * Param1: const Char c = [In] 
 *
 * '*' => PM_OPERATOR_AND
 * '+' => PM_OPERATOR_OR
 * '^' => PM_OPERATOR_XOR
 * 'T' => PM_OPERATOR_TIMER
 *
 ****************************************************************************************/
OperatorType CBindingOperator::Convert(const char cOperator)
{
	switch(cOperator) 
	{
		case '*':
		{
			return (PM_OPERATOR_AND);

		} break;

		case '+':
		{ 
			return (PM_OPERATOR_OR); 		

		} break;
		
		case '^': 
		{
			return (PM_OPERATOR_XOR);

		}break;
		
		case 'T': 
		{
			return (PM_OPERATOR_TIMER);

		}break;
	}

	return (PM_OPERATOR_UNDEFINED);
}

///////////////////////////////////////////////////////////////////////
CBindingTimerOperator::CBindingTimerOperator()
{
	SetType(PM_OPERATOR_TIMER); 
	SetTimerType(PM_TIMER_TE);
	SetTimeValue(15);
}

void CBindingTimerOperator::SetTimerType(TimerType type)
{
	Timer = type;
}

TimerType CBindingTimerOperator::GetTimerType(void) const
{
	return (Timer);
}

void CBindingTimerOperator::SetTimeValue(BYTE bVal)
{
	bValue = bVal;
}

BYTE CBindingTimerOperator::GetTimeValue(void) const
{
	return(bValue);
}

TimerType CBindingTimerOperator::Convert(const char cTimerType)
{
	switch(cTimerType)
	{
		case 'e':
		case 'E':
		{
			return PM_TIMER_TE;

		} break;

		case 'i':
		case 'I':
		{
			return PM_TIMER_TI;

		} break;

		case 'o':
		case 'O':
		{
			return PM_TIMER_TO;

		} break;

		case 'f':
		case 'F':
		{
			return PM_TIMER_TF;

		} break; 

		case 'd':
		case 'D':
		{
			return PM_TIMER_TD;

		} break;

		case 'r':
		case 'R':
		{
			return PM_TIMER_TR;

		} break;
	}

	return PM_TIMER_NONE;
}

BOOL CBindingTimerOperator::IsEqual(const CBindingOperator* const pBindingOperator)
{	
	if (this->CBindingOperator::IsEqual(pBindingOperator) == FALSE)		// CHANGES_MVS_2008
	{
		return FALSE;
	}

	if (Timer != static_cast<const CBindingTimerOperator*>(pBindingOperator)->Timer)
	{
		return FALSE;
	}

	if (bValue != static_cast<const CBindingTimerOperator*>(pBindingOperator)->bValue)
	{
		return FALSE;
	}

	return TRUE;
}
