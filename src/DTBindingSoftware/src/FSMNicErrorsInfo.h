// FSMNicErrorsInfo.h: interface for the CFSMNicErrorsInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FSMNICERRORSINFO_H__1157A7C2_2F7E_44DD_90CD_AA2E0294CECE__INCLUDED_)
#define AFX_FSMNICERRORSINFO_H__1157A7C2_2F7E_44DD_90CD_AA2E0294CECE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PtrListNodes.h"
class CCanNode;


class CFSMNicErrorsInfo 
{
protected:
	CPtrListNodes m_Nodes;

protected:
	CFSMNicErrorsInfo();

public:
	virtual ~CFSMNicErrorsInfo();

	static CFSMNicErrorsInfo* GetInstance();

	BOOL Add(CPtrList* nodes);
	BOOL Add(CCanNode* pNode);
	BOOL Remove(CCanNode* pNode);
	CCanNode* Get(BYTE index) const;
	BYTE Count() const;	
	void Clear();
};

#endif // !defined(AFX_FSMNICERRORSINFO_H__1157A7C2_2F7E_44DD_90CD_AA2E0294CECE__INCLUDED_)
