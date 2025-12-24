// BindingFileConstructParserBC.h: interface for the CBindingFileConstructParserBC class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGFILECONSTRUCTPARSERBC_H__96A52667_8060_4DE5_9C96_0B45AB3966F0__INCLUDED_)
#define AFX_BINDINGFILECONSTRUCTPARSERBC_H__96A52667_8060_4DE5_9C96_0B45AB3966F0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "BindingFileConstructParser.h"
class CBindingFileEntryTypeC;


class CBindingFileConstructParserBC : public CBindingFileConstructParser  
{
protected:
	CBindingFileEntryParserC* m_pBindingFileEntryTypeC;	
	CStringArray m_StringArray;

protected:
	BOOL AddBindingOperator(CBindingOperator* pNewOperator);
	BOOL AddReturnUnits();
	CBindingUnit* AddForwardUnit(CBindingFileEntryParser* pBindingFileEntryParser);

public:
	CBindingFileConstructParserBC(CStringArray* pArrayBindingStrings=NULL);
	virtual ~CBindingFileConstructParserBC();

protected:	
	virtual BOOL IsTypeOf();
	virtual BOOL Init();				
	virtual BOOL CheckPreConditions();
	virtual BOOL AddUnits();
	virtual BOOL AddOperators();
	virtual BOOL AddLinks();
	virtual BOOL BuildEvalString();
	/* virtual BOOL BuildNextConstruct();	/* Absorbe */
	/* virtual BOOL AppendNextConstruct();	/* Absorbe */
	virtual BOOL BuildBindingEntryInfo();
	virtual BOOL Destroy();				

protected:
	BOOL _BuildEvalString(CBindingFileEntryParserC *pBindingFileEntryParser);
	BOOL _AddOperators(CBindingFileEntryParserC *pBindingFileEntryParser);
};

#endif // !defined(AFX_BINDINGFILECONSTRUCTPARSERBC_H__96A52667_8060_4DE5_9C96_0B45AB3966F0__INCLUDED_)
