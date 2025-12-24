// BindingFileEntry.h: interface for the CBindingFileEntry class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGFILEENTRY_H__09A2F36C_6A5A_4C9B_A6FC_36768BD5FA19__INCLUDED_)
#define AFX_BINDINGFILEENTRY_H__09A2F36C_6A5A_4C9B_A6FC_36768BD5FA19__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



#include "BindingFileEntryParser.h"
#include "BindingFileEntryParserB.h"
#include "BindingFileEntryParserC.h"
#include "BindingFileEntryParserG.h"
#include "BindingFileEntryParserI.h"
#include "BindingFileEntryParserCN.h"
#include "BindingFileEntryParserGN.h"
#include "BindingFileEntryParserN.h"
#include "BindingFileEntryParserNN.h"
#include "BindingFileEntryParserP.h"

class CBindingFileEntry  
{
protected:
	CString m_szBindingString;

protected:

	#define MAX_BINDINGENTRY_TYPES	9

	CBindingFileEntryParserB	m_BindingFileEntryTypeB;	// 0
	CBindingFileEntryParserC	m_BindingFileEntryTypeC;	// 1
	CBindingFileEntryParserG	m_BindingFileEntryTypeG;	// 2
	CBindingFileEntryParserI	m_BindingFileEntryTypeI;	// 3
	CBindingFileEntryParserCN	m_BindingFileEntryTypeCN;	// 4
	CBindingFileEntryParserGN	m_BindingFileEntryTypeGN;	// 5
	CBindingFileEntryParserN	m_BindingFileEntryTypeN;	// 6
	CBindingFileEntryParserNN	m_BindingFileEntryTypeNN;	// 7
	CBindingFileEntryParserP	m_BindingFileEntryTypeP;	// 8

	CBindingFileEntryParser* m_pBindingFileEntryTypes[MAX_BINDINGENTRY_TYPES];

public:
	CBindingFileEntry(CString s);
	virtual ~CBindingFileEntry();

public:
	CBindingFileEntryParser* Parse();

};


#endif // !defined(AFX_BINDINGFILEENTRY_H__09A2F36C_6A5A_4C9B_A6FC_36768BD5FA19__INCLUDED_)
