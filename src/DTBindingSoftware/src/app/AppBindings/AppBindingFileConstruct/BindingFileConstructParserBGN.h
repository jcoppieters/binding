// BindingFileConstructParserBGN.h: interface for the CBindingFileConstructParserBGN class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGFILECONSTRUCTPARSERBGN_H__E09207D5_A7A8_4A6E_9A65_A280E274D0C5__INCLUDED_)
#define AFX_BINDINGFILECONSTRUCTPARSERBGN_H__E09207D5_A7A8_4A6E_9A65_A280E274D0C5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "BindingFileConstructParserBG.h"
class CBindingFileConstructParserN_H;

class CBindingFileConstructParserBGN : public CBindingFileConstructParserBG  
{
private:
	CBindingFileEntryParserGN* m_pBindingFileEntryParserGN;
	CBindingFileConstructParserN_H *m_pBindingFileConstructN;

public:
	CBindingFileConstructParserBGN(CStringArray* pArrayBindingStrings=NULL);
	virtual ~CBindingFileConstructParserBGN();

protected:
	
	//BOOL GetBindingFlags(BYTE* bBindingFlags);
	//BOOL GetForwardUnit(CBindingUnit* pForwardUnit);

	BOOL _ConstructBindingEvaluation(CBindingUnit* pForwardUnit,
									 CBindingUnit* pReturnUnit,
	 							     CBindingUnit* pNextUnit);
	BOOL ConstructBindingInfo();
	BOOL GetNextUnit(CBindingUnit* pNextUnit);

	//CBindingUnit* AddForwardUnit();
	CBindingUnit* AddReturnUnit();

protected:
	virtual BOOL IsTypeOf();
	virtual BOOL Init();
	virtual BOOL CheckPreConditions();
	virtual BOOL AddUnits();
	/*virtual BOOL AddLinks();				/* ABSORBE */
	/*virtual BOOL BuildEvalString();		/* ABSORBE */
	virtual BOOL BuildNextConstruct();
	virtual BOOL AppendNextConstruct();
	virtual BOOL BuildBindingEntryInfo();
	virtual BOOL Destroy();

};

#endif // !defined(AFX_BINDINGFILECONSTRUCTPARSERBGN_H__E09207D5_A7A8_4A6E_9A65_A280E274D0C5__INCLUDED_)
