// BindingFileConstruction.h: interface for the CBindingFileConstruction class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGFILECONSTRUCTION_H__BB4FEA7D_6817_4A21_9DDB_274F90C0927D__INCLUDED_)
#define AFX_BINDINGFILECONSTRUCTION_H__BB4FEA7D_6817_4A21_9DDB_274F90C0927D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "BindingEntry.h"
#include "BindingEntryInfo.h"

#include "BindingFileConstructParserBC.h"
#include "BindingFileConstructParserBCN.h"
#include "BindingFileConstructParserBG.h"
#include "BindingFileConstructParserBGN.h"
#include "BindingFileConstructParserI.h"


class CBindingFileConstruction  
{
protected:
	CStringArray* const m_pStringArray;
	CBindingEntry m_BindingEntry;
	CBindingEntryInfo m_BindingEntryInfo;

protected:
	enum
	{
		MAX_BINDINGCONSTRUCTION_TYPES	= 5
	};

	CBindingFileConstructParserBC	m_BindingFileConstructionBC;
	CBindingFileConstructParserBCN	m_BindingFileConstructionBCN;
	CBindingFileConstructParserBG	m_BindingFileConstructionBG;
	CBindingFileConstructParserBGN	m_BindingFileConstructionBGN;
	CBindingFileConstructParserI	m_BindingFileConstructionI;

	CBindingFileConstructParser* m_pBindingConstructions[MAX_BINDINGCONSTRUCTION_TYPES];

public:
	CBindingFileConstruction(CStringArray* pStrArray);
	virtual ~CBindingFileConstruction();

	BOOL Construct();
	CBindingEntryInfo* GetBindingEntryInfo() 
	{
		return &m_BindingEntryInfo;
	}
};

#endif // !defined(AFX_BINDINGFILECONSTRUCTION_H__BB4FEA7D_6817_4A21_9DDB_274F90C0927D__INCLUDED_)
