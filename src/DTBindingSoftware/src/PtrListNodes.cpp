// PtrListNodes.cpp: implementation of the CPtrListNodes class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "PtrListNodes.h"
#include "CanNode.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPtrListNodes::CPtrListNodes()
{
	Clear();
}

CPtrListNodes::~CPtrListNodes()
{
	Clear();
}

BOOL CPtrListNodes::Add(CPtrList* pPtrList)
{
	m_Nodes.AddTail(pPtrList);
	return TRUE;
}

BOOL CPtrListNodes::Add(CCanNode* pNode)
{
	m_Nodes.AddTail(pNode);
	return TRUE;
}

BOOL CPtrListNodes::Remove(CCanNode* pNode)
{
	POSITION pos = m_Nodes.Find(pNode);
	if (pos != 0)
	{
		m_Nodes.RemoveAt(pos);
		return TRUE;
	}
	return FALSE;
}

CCanNode* CPtrListNodes::Get(BYTE index) const 
{
	CCanNode *pNode = NULL;

	if (m_Nodes.GetCount() != 0)
	{	
		int i = 0;

		POSITION pos = m_Nodes.GetHeadPosition();		

		while(pos!=NULL)
		{
			if ((pNode = (CCanNode*) m_Nodes.GetNext(pos)) != NULL)
			{				
				if (i == index)
				{
					return pNode;
				}
			}			
			
			i++;
		}		
	}

	return NULL;
}

BYTE CPtrListNodes::Count() const
{
	return (BYTE) (m_Nodes.GetCount());
}

void CPtrListNodes::Clear()
{
	m_Nodes.RemoveAll();
}