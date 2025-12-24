// BindingFileConstructFormatterB_H.h: interface for the CBindingFileConstructFormatterB_H class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGFILECONSTRUCTFORMATTERB_H_H__69ADE1F2_058A_40C5_AB43_B97CCE630A2F__INCLUDED_)
#define AFX_BINDINGFILECONSTRUCTFORMATTERB_H_H__69ADE1F2_058A_40C5_AB43_B97CCE630A2F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BindingFileConstructFormatter.h"

class CBindingFileConstructFormatterB_H : public CBindingFileConstructFormatter  
{
public:
	CBindingFileConstructFormatterB_H(CBindingEntryInfo* pBindingEntryInfo,
									  CStringArray* pArrayBindingStrings,
									  CBindingUnit* pBindingUnit);									 

	virtual ~CBindingFileConstructFormatterB_H();

	BOOL Format();

protected:
	CBindingUnit* const m_pForwardUnit;
};

#endif // !defined(AFX_BINDINGFILECONSTRUCTFORMATTERB_H_H__69ADE1F2_058A_40C5_AB43_B97CCE630A2F__INCLUDED_)
