// PtrListNodes.h: interface for the CPtrListNodes class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PTRLISTNODES_H__D60DB7E4_8C08_4360_8986_22E71CFC929C__INCLUDED_)
#define AFX_PTRLISTNODES_H__D60DB7E4_8C08_4360_8986_22E71CFC929C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CCanNode;

class CPtrListNodes  
{
protected:
	CPtrList m_Nodes;

public:
	CPtrListNodes();
	virtual ~CPtrListNodes();

public:
	BOOL Add(CPtrList* pPtrList);
	BOOL Add(CCanNode* pNode);
	BOOL Remove(CCanNode* pNode);
	CCanNode* Get(BYTE index) const;
	BYTE Count() const;	
	void Clear();
};

#endif // !defined(AFX_PTRLISTNODES_H__D60DB7E4_8C08_4360_8986_22E71CFC929C__INCLUDED_)
