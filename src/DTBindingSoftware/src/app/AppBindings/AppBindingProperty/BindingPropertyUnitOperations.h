// BindingPropertyUnitOperations.h: interface for the CBindingPropertyUnitOperations class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGPROPERTYUNITOPERATIONS_H__717A898C_7DFF_4834_ACBB_248CCB656153__INCLUDED_)
#define AFX_BINDINGPROPERTYUNITOPERATIONS_H__717A898C_7DFF_4834_ACBB_248CCB656153__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CBindingPropertyUnit;
class CBindingUnit;


class CBindingPropertyUnitOperations  
{
protected:
	CBindingPropertyUnit* const m_pBindingPropertyUnit;

public:
	CBindingPropertyUnitOperations(CBindingPropertyUnit* const p = 0);
	virtual ~CBindingPropertyUnitOperations();

	CBindingPropertyUnit* Find(BYTE bNodeAddress,BYTE bUnitAddress);
	BOOL Update(const CBindingUnit* const pBindingUnit);
	BOOL Clear();
	BOOL Export(FILE* fp);
};

#endif // !defined(AFX_BINDINGPROPERTYUNITOPERATIONS_H__717A898C_7DFF_4834_ACBB_248CCB656153__INCLUDED_)
