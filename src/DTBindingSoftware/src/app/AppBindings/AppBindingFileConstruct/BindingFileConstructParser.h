// BindingFileConstructParser.h: interface for the CBindingFileConstructParser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGFILECONSTRUCTPARSER_H__B43606D5_F534_46CD_905F_0E73A9874D3E__INCLUDED_)
#define AFX_BINDINGFILECONSTRUCTPARSER_H__B43606D5_F534_46CD_905F_0E73A9874D3E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BindingFileConstruct.h"

class CLogFileConversionConstruct;
class CBindingFileConstructParserN_H;

class CBindingFileConstructParser : public CBindingFileConstruct  
{
public:
	CBindingFileConstructParser(CStringArray* pArrayBindingStrings=NULL);
	virtual ~CBindingFileConstructParser();
	BOOL Construct(CBindingEntryInfo* pBindingEntryInfo);

protected:
	virtual BOOL IsTypeOf();
	virtual BOOL Init();
	virtual BOOL CheckPreConditions();
	virtual BOOL AddUnits();
	virtual BOOL BuildEvalString();
	virtual BOOL AddOperators();
	virtual BOOL AddLinks();
	virtual BOOL BuildNextConstruct();
	virtual BOOL AppendNextConstruct();
	virtual BOOL BuildBindingEntryInfo();
	virtual BOOL Destroy();

	BOOL AddBindingUnit(
		CBindingUnit* pNewUnit);
	BOOL FindBindingUnit(
		CBindingUnit** pFindUnit);
	BOOL AddBindingLink(
		CBindingUnit* pIn,CBindingUnit* pOut,
		BOOL fInverse = FALSE);
	int GetBindingTypeEntry(
		BOOL (CBindingFileEntryParser::*Action)() const,
		int index = 0);		
	BOOL GetBindingFlags(
		CBindingFileEntryParser* const pBindingFileEntryParser,
		BYTE *bBindingFlags);
	BOOL GetBindingNr(
		CBindingFileEntryParser* const pBindingFileEntryParser,
		unsigned short* usBindingNr);
	BOOL GetForwardUnit(
		CBindingFileEntryParser* const pBindingFileEntryParser,
		CBindingUnit* pForwardUnit);
	BOOL _BuildNextConstruct(
		CBindingFileConstructParserN_H *const pBindingFileConstructParser);
	BOOL _AppendNextConstruct(
		CBindingFileConstructParserN_H *const pBindingFileConstructParser);

protected:
	CBindingUnit* m_pForwardUnit;
	CBindingUnit* m_pReturnUnit;
	CLogFileConversionConstruct* const m_pLogFile;	
};

#endif // !defined(AFX_BINDINGFILECONSTRUCTPARSER_H__B43606D5_F534_46CD_905F_0E73A9874D3E__INCLUDED_)
