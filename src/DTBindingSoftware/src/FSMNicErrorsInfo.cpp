// FSMNicErrorsInfo.cpp: implementation of the CFSMNicErrorsInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "FSMNicErrorsInfo.h"

#include "CanNode.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
CFSMNicErrorsInfo* CFSMNicErrorsInfo::GetInstance()
{
	static CFSMNicErrorsInfo instance;
	return (&instance);
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CFSMNicErrorsInfo::CFSMNicErrorsInfo() 
{
}

CFSMNicErrorsInfo::~CFSMNicErrorsInfo()
{
}

//////////////////////////////////////////////////////////////////////

/**
 *
 */
BOOL CFSMNicErrorsInfo::Add(CCanNode* pNode)
{
	return (m_Nodes.Add(pNode));
}

BOOL CFSMNicErrorsInfo::Remove(CCanNode* pNode)
{
	return (m_Nodes.Remove(pNode));
}

CCanNode* CFSMNicErrorsInfo::Get(BYTE index) const
{
	return (m_Nodes.Get(index));
}

BYTE CFSMNicErrorsInfo::Count() const
{
	return (m_Nodes.Count());
}

void CFSMNicErrorsInfo::Clear()
{
	m_Nodes.Clear();
}

BOOL CFSMNicErrorsInfo::Add(CPtrList* nodes)
{
	return (m_Nodes.Add(nodes));
}