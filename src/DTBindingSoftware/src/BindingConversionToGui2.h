// BindingConversionToGui2.h: interface for the CBindingConversionToGui2 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGCONVERSIONTOGUI2_H__49C1DCF4_C20D_4C83_AFE6_957BCF02C257__INCLUDED_)
#define AFX_BINDINGCONVERSIONTOGUI2_H__49C1DCF4_C20D_4C83_AFE6_957BCF02C257__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CBindingEntry;
#include "BindingConvertHelper.h"

class CBindingConversionToGui2  
{
protected:
	CStringArray m_StringArray;
	CBindingEntry* const m_pBindingEntry;
	CBindingConvertHelper Convert;

	float m_xCurrentPos;
	float m_yCurrentPos;

protected:
	BOOL BuildArrayLinkInfo();

	BOOL BindingInputUnits();
	BOOL BindingOperators();
	BOOL BindingOutputUnits();

public:
	CBindingConversionToGui2(CBindingEntry* pBindingEntry);
	virtual ~CBindingConversionToGui2();

public:
	//BOOL ProcessBindingEntry(CBindingEntry *pEntry);
	BOOL Process();
};

#endif // !defined(AFX_BINDINGCONVERSIONTOGUI2_H__49C1DCF4_C20D_4C83_AFE6_957BCF02C257__INCLUDED_)
