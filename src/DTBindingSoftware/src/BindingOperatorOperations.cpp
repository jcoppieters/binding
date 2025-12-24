// BindingOperatorOperations.cpp: implementation of the CBindingOperatorOperations class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingOperatorOperations.h"

#include "BindingOperator.h"

#include "App\Appbindings\appbindingproperty\BindingPropertyStrings.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingOperatorOperations::CBindingOperatorOperations(CBindingOperator* const pBindingOperator) :
m_pBindingOperator( pBindingOperator )
{
	ASSERT( m_pBindingOperator );
}

CBindingOperatorOperations::~CBindingOperatorOperations()
{

}

CString CBindingOperatorOperations::GetFunction()
{
	if (m_pBindingOperator == 0) {
		return (CString(""));
	}

	switch(m_pBindingOperator->GetType())
	{
		case PM_OPERATOR_AND:
			return (CString("AND"));
		break;

		case PM_OPERATOR_OR:
			return (CString("OR"));
		break;

		case PM_OPERATOR_XOR:
			return (CString("XOR"));
		break;

		case PM_OPERATOR_TIMER:
		{			
			CBindingTimerOperator* const pBindingTimerOperator = 
				static_cast<CBindingTimerOperator*>(m_pBindingOperator);

			if (pBindingTimerOperator->GetTimerType() < MAX_PM_TIMERS) 
			{
				CString strType((LPCSTR) TimerStrings[pBindingTimerOperator->GetTimerType()]);	
				CString strValue;

				const BYTE bValue = pBindingTimerOperator->GetTimeValue();
	
				if (bValue <=59) 
				{
					strValue.Format("%02d sec.",bValue);
				}
				else 
				{
					strValue.Format("%02d min.",bValue-59);
				}

				return(CString(strType + "-" + strValue));
			}

		} break;
	}
	return (CString(""));
}