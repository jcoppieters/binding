// BindingFileConstructParserP.h: interface for the CBindingFileConstructParserP class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGFILECONSTRUCTPARSERP_H__49DFA728_C67C_44C9_BD52_6AE202700718__INCLUDED_)
#define AFX_BINDINGFILECONSTRUCTPARSERP_H__49DFA728_C67C_44C9_BD52_6AE202700718__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "BindingFileConstruct.h"
class CBindingUnits;

/**
 *
 */
class CBindingFileConstructParserP : public CBindingFileConstruct  
{
protected:
	CBindingUnits* const m_pBindingUnits;

public:
	CBindingFileConstructParserP(CStringArray* pArrayBindingStrings=NULL);
	virtual ~CBindingFileConstructParserP();

public:
	BOOL Construct();

	const CBindingUnits* GetBindingUnits() const {
		return m_pBindingUnits;
	}

};

#endif // !defined(AFX_BINDINGFILECONSTRUCTPARSERP_H__49DFA728_C67C_44C9_BD52_6AE202700718__INCLUDED_)
