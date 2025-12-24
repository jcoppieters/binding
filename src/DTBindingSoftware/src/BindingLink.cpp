// BindingLink.cpp: implementation of the CBindingLink class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingLink.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// zie USE_IMPLEMENTATION_SOLVE_BUG_0157
CBindingLink::CBindingLink() :
	m_nID(0),
	m_fInverse(FALSE),
	m_ulApplicationFlags(0)
{
	m_InLink.m_Type=UNDEFINED;
	m_InLink.m_nID=0; 

	m_OutLink.m_Type=UNDEFINED;
	m_OutLink.m_nID=0; 
}

#if(0)
CBindingLink::CBindingLink()
{
	SetApplicationFlags(0);
}
#endif

CBindingLink::~CBindingLink()
{
}
////////////////////////////////////////////////////////////////////////
void CBindingLink::SetID(unsigned short ID)
{
	m_nID = ID;
}

unsigned short CBindingLink::GetID(void) const
{
	return m_nID;
}
////////////////////////////////////////////////////////////////////////
void CBindingLink::SetInLink(unsigned short ID,ControlType type)
{
	m_InLink.m_nID = ID;
	m_InLink.m_Type = type;
}

void CBindingLink::SetOutLink(unsigned short ID,ControlType type)
{
	m_OutLink.m_nID = ID;
	m_OutLink.m_Type = type;
}

unsigned short CBindingLink::GetInLinkID(void) const
{
	return (m_InLink.m_nID);
}

ControlType CBindingLink::GetInLinkType(void) const
{
	return (m_InLink.m_Type);
}

unsigned short CBindingLink::GetOutLinkID(void) const
{
	return (m_OutLink.m_nID);
}

ControlType CBindingLink::GetOutLinkType(void) const
{
	return (m_OutLink.m_Type);
}
	
void CBindingLink::SetInverse(BOOL flag)
{
	ASSERT(flag==TRUE || flag==FALSE);					// USE_IMPLEMENTATION_SOLVE_BUG_0157
	m_fInverse = (flag == TRUE ? TRUE : FALSE);			// USE_IMPLEMENTATION_SOLVE_BUG_0157
}

BOOL CBindingLink::isInverted(void) const
{
	return (m_fInverse);
}
