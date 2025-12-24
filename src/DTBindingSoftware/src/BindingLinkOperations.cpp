// BindingLinkOperations.cpp: implementation of the CBindingLinkOperations class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingLinkOperations.h"

#include "BindingLink.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingLinkOperations::CBindingLinkOperations(CBindingLink* const pBindingLink) :
m_pBindingLink( pBindingLink )
{

}

CBindingLinkOperations::~CBindingLinkOperations()
{

}

CString CBindingLinkOperations::ToString()
{
	if (m_pBindingLink == 0) 
	{
		return (CString(""));
	}

	CString szIn;
	if (m_pBindingLink->GetInLinkType() == PM_UNIT) 
	{
		szIn.Format("Unit(%d)",
			m_pBindingLink->GetInLinkID());
	}
	else 
	{
		szIn.Format("Operator(%d)",
			m_pBindingLink->GetInLinkID());	
	}

	CString szOut;
	if (m_pBindingLink->GetOutLinkType() == PM_UNIT) 
	{
		szOut.Format("Unit(%d)",
			m_pBindingLink->GetOutLinkID());
	}
	else 
	{
		szOut.Format("Operator(%d)",
			m_pBindingLink->GetOutLinkID());	
	}

	return( CString(szIn + " - " + szOut) );
}
