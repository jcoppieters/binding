// ClassFactory.cpp: implementation of the CClassFactory class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Timer.h"
#include "ClassFactory.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CClassFactory::CClassFactory()
{

}

CClassFactory::~CClassFactory()
{

}

template <class RT,class P,class Q>
BOOL Create(RT **pReturn,P* p,Q* q)
{
	try
	{
		*pReturn = new RT(p,q);
	}
	catch(CMemoryException *e)
	{
		e->ReportError();
		e->Delete;
		return FALSE;
	}
	return TRUE;
}


/*
template <class RT>
RT* CClassFactory::Create(RT* dummy)
{
	RT* pNew = NULL;

	try
	{
		pNew = new RT();
	}

	catch(CMemoryException *e)
	{
		e->ReportError();
		e->Delete;
		return NULL;
	}

	return pNew;
}

template <class RT,class P>
RT* CClassFactory::Create(RT* dummy,P* p)
{
	ASSERT(p);

	RT* pNew = NULL;

	try
	{
		pNew = new RT(p);
	}

	catch(CMemoryException *e)
	{
		e->ReportError();
		e->Delete;
		return NULL;
	}

	return pNew;
}

template <class RT,class P,class Q>
RT* CClassFactory::Create(RT* dummy,P* p,Q* q)
{
	RT* pNew = NULL;

	ASSERT(p);
	ASSERT(q);

	try
	{
		pNew = new RT(p,q);
	}

	catch(CMemoryException *e)
	{
		e->ReportError();
		e->Delete;
		return NULL;
	}

	return pNew;
}
*/