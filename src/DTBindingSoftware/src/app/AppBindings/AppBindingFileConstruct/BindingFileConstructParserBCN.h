// BindingFileConstructParserBCN.h: interface for the CBindingFileConstructParserBCN class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGFILECONSTRUCTPARSERBCN_H__CDD81093_79B1_453E_BF05_7DA68A7245AB__INCLUDED_)
#define AFX_BINDINGFILECONSTRUCTPARSERBCN_H__CDD81093_79B1_453E_BF05_7DA68A7245AB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "BindingFileConstructParserBC.h"
class CBindingFileEntryParserCN;
class CBindingFileConstructParserN_H;


class CBindingFileConstructParserBCN : public CBindingFileConstructParserBC  
{
private:
	CBindingFileEntryParserCN* m_pBindingFileEntryParserCN;
	CBindingFileConstructParserN_H *m_pBindingFileConstructN;

public:
	CBindingFileConstructParserBCN(CStringArray* pArrayBindingStrings=NULL);
	virtual ~CBindingFileConstructParserBCN();

protected:
	virtual BOOL IsTypeOf();
	virtual BOOL Init();
	virtual BOOL CheckPreConditions();
	virtual BOOL AddUnits();
	virtual BOOL AddOperators();
	//virtual BOOL AddLinks();				/* ABSORBE */
	virtual BOOL BuildEvalString();			/* ABSORBE */
	virtual BOOL BuildNextConstruct();
	virtual BOOL AppendNextConstruct();
	virtual BOOL BuildBindingEntryInfo();
	virtual BOOL Destroy();
};

#endif // !defined(AFX_BINDINGFILECONSTRUCTPARSERBCN_H__CDD81093_79B1_453E_BF05_7DA68A7245AB__INCLUDED_)
