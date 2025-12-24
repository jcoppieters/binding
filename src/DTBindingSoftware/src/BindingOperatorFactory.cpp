// BindingOperatorFactory.cpp: implementation of the CBindingOperatorFactory class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingOperatorFactory.h"

#include "BindingOperator.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingOperatorFactory::CBindingOperatorFactory()
{

}

CBindingOperatorFactory::~CBindingOperatorFactory()
{

}


//////////////////////////////////////////////////////////////////////
CBindingOperator* CBindingOperatorFactory::Clone(CBindingOperator* pBindingOperator)
{
	CBindingOperator* pNew = NULL;

	try
	{			
		switch(pBindingOperator->GetType())
		{
			case PM_OPERATOR_TIMER: 
			{
				CBindingTimerOperator* pTemp = 
					static_cast<CBindingTimerOperator*>(pBindingOperator);
				pNew = new CBindingTimerOperator(*pTemp); 

			} break;

			case PM_OPERATOR_AND :  
			case PM_OPERATOR_OR :  
			case PM_OPERATOR_XOR :
			{
				CBindingLogicalOperator* pTemp = 
					static_cast<CBindingLogicalOperator*>(pBindingOperator);
				pNew = new CBindingLogicalOperator(*pTemp);
		
			} break;
		
			default: 
			{
				ASSERT(0);
				pNew = new CBindingOperator(*pBindingOperator); 

			} break;
		}
	}
	
	catch (CMemoryException* e)
	{
		ASSERT(0);
		e->ReportError();
		e->Delete();
		return NULL;		
	}
	return pNew;
}

CBindingOperator* CBindingOperatorFactory::Create(OperatorType type)
{
	CBindingOperator* pNew = NULL;

	try
	{
		switch(type)
		{
			case PM_OPERATOR_TIMER: 
			{
				pNew = new CBindingTimerOperator(); 
			} break;
			case PM_OPERATOR_AND :  
			case PM_OPERATOR_OR :  
			case PM_OPERATOR_XOR :
			{
				pNew = new CBindingLogicalOperator(type);		
			} break;		
			default: 
			{
				pNew = NULL; 
			} break;
		}
	}
	catch (CMemoryException* e)
	{
		ASSERT(0);
		e->ReportError();
		e->Delete();
		return NULL;		
	}

	return pNew;
}

CBindingOperator* CBindingOperatorFactory::Create(const char c)
{
	CBindingOperator* pNew = NULL;

	try
	{
		switch(c)
		{
			case 'T':
			{
				pNew = new CBindingTimerOperator(); 
			} break;
			case '+':
			{
				pNew = new CBindingLogicalOperator(PM_OPERATOR_OR);
			} break;
			case '*':
			{
				pNew = new CBindingLogicalOperator(PM_OPERATOR_AND);
			} break;
			case '^':
			{
				pNew = new CBindingLogicalOperator(PM_OPERATOR_XOR);
			} break;

			default: 
			{
				pNew = NULL; 
			} break;
		}
	}
	catch (CMemoryException* e)
	{
		ASSERT(0);
		e->ReportError();
		e->Delete();
		return NULL;		
	}

	return pNew;
}