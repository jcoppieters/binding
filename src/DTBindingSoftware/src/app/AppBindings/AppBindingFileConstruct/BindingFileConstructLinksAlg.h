// BindingFileConstructLinksAlg.h: interface for the CBindingFileConstructLinksAlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGFILECONSTRUCTLINKSALG_H__A8150C05_A654_48DF_8D0F_A67C3B08EBDD__INCLUDED_)
#define AFX_BINDINGFILECONSTRUCTLINKSALG_H__A8150C05_A654_48DF_8D0F_A67C3B08EBDD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CBindingEntry;
class CBindingLinks;
class CBindingOperators;
class CBindingUnits;
class CBindingOperator;
class CBindingUnit;

class CBindingFileConstructLinksAlg  
{
protected:
	CBindingEntry* const m_pBindingEntry;
	CBindingLinks* const m_pBindingLinks;
	CBindingOperators* const m_pBindingOperators;
	CBindingUnits* const m_pBindingUnits;

	CStringArray m_StringArray;

public:
	CBindingFileConstructLinksAlg(CBindingEntry* pBindingEntry);
	virtual ~CBindingFileConstructLinksAlg();

protected:
	BOOL ConstructInputs(CString s);
	BOOL ConstructOutputs(CString s);

	//CBindingOperator* GetOperator(int index,CString s);
	//CBindingUnit* GetUnit(int index,CString s);

public:
	BOOL Build();
};

#endif // !defined(AFX_BINDINGFILECONSTRUCTLINKSALG_H__A8150C05_A654_48DF_8D0F_A67C3B08EBDD__INCLUDED_)
