// BindingFileConstructParserI.h: interface for the CBindingFileConstructParserI class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGFILECONSTRUCTPARSERI_H__F62A5D90_CFF9_4C60_B18B_A21711BB757C__INCLUDED_)
#define AFX_BINDINGFILECONSTRUCTPARSERI_H__F62A5D90_CFF9_4C60_B18B_A21711BB757C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BindingFileConstructParser.h"

class CBindingFileConstructParserI : public CBindingFileConstructParser  
{
protected:
	CBindingFileEntryParserI* m_pBindingFileEntryParserI;
	BOOL m_fInverse;

protected:	
	BOOL GetReturnUnit(CBindingUnit* pReturnUnit);
	BOOL ConstructBindingEvaluation(CBindingUnit* pForwardUnit,
									CBindingUnit* pReturnUnit);
	
	CBindingUnit* AddReturnUnit();
	CBindingUnit* AddForwardUnit();

public:
	CBindingFileConstructParserI(CStringArray* pArrayBindingStrings=NULL);
	virtual ~CBindingFileConstructParserI();

protected:
	virtual BOOL IsTypeOf();
	virtual BOOL Init();					/* Absorbe */
	virtual BOOL CheckPreConditions();
	virtual BOOL AddUnits();
	virtual BOOL AddLinks();
	virtual BOOL BuildEvalString();
	/* virtual BOOL BuildNextConstruct();	/* Absorbe */
	/* virtual BOOL AppendNextConstruct();	/* Absorbe */
	virtual BOOL BuildBindingEntryInfo();
	virtual BOOL Destroy();					/* Absorbe */
};

#endif // !defined(AFX_BINDINGFILECONSTRUCTPARSERI_H__F62A5D90_CFF9_4C60_B18B_A21711BB757C__INCLUDED_)
