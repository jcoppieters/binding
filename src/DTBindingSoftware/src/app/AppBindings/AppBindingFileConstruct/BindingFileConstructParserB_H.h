// BindingFileConstructParserB_H.h: interface for the CBindingFileConstructParserB_H class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGFILECONSTRUCTPARSERB_H_H__44F6D954_6E2E_42FB_8F32_A91338812475__INCLUDED_)
#define AFX_BINDINGFILECONSTRUCTPARSERB_H_H__44F6D954_6E2E_42FB_8F32_A91338812475__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BindingFileConstructParser.h"

class CBindingFileConstructParserB_H : public CBindingFileConstructParser  
{
protected:
	CBindingUnits m_BindingUnits;
	CBindingUnit* m_pForwardUnit;

public:
	CBindingFileConstructParserB_H(CStringArray* pStringArray);
	virtual ~CBindingFileConstructParserB_H();

public:
	BOOL GetReturnUnit(CBindingUnit* pReturnUnit);
	BOOL Construct();

public:
	const CBindingUnits* GetReturnUnits() const;
	const CBindingUnit* GetForwardUnit() const;
};

#endif // !defined(AFX_BINDINGFILECONSTRUCTPARSERB_H_H__44F6D954_6E2E_42FB_8F32_A91338812475__INCLUDED_)
