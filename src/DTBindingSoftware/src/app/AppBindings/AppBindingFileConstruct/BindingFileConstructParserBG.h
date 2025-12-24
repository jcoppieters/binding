// BindingFileConstructParserBG.h: interface for the CBindingFileConstructParserBG class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGFILECONSTRUCTPARSERBG_H__DC323891_2D53_4A78_BDF5_1B91E96FCD29__INCLUDED_)
#define AFX_BINDINGFILECONSTRUCTPARSERBG_H__DC323891_2D53_4A78_BDF5_1B91E96FCD29__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "BindingFileConstructParser.h"
class CBindingFileEntryParserG;


class CBindingFileConstructParserBG : public CBindingFileConstructParser  
{
private:
	CBindingFileEntryParserG* m_pBindingFileEntryParserG;

public:
	CBindingFileConstructParserBG(CStringArray* pArrayBindingStrings=NULL);
	virtual ~CBindingFileConstructParserBG();
	
public:
	//CBindingUnit* AddForwardUnit();

	CBindingUnit* AddForwardUnit(CBindingFileEntryParser* pBindingFileEntryParser);
	CBindingUnit* AddReturnUnit();

protected:
	//BOOL GetForwardUnit(CBindingUnit* pForwardUnit);
	//BOOL GetBindingNr(unsigned short* usBindingNr);
	//BOOL GetBindingFlags(BYTE* bBindingFlags);

	BOOL ConstructBindingEvaluation(CBindingUnit* pForwardUnit,
									CBindingUnit* pReturnUnit);
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

#endif // !defined(AFX_BINDINGFILECONSTRUCTPARSERBG_H__DC323891_2D53_4A78_BDF5_1B91E96FCD29__INCLUDED_)
