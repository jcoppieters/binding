// BindingConversionToNodesProperty.h: interface for the CBindingConversionToNodesProperty class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGCONVERSIONTONODESPROPERTY_H__FA6A3E9D_57C9_414D_92FD_753FBAAE5C99__INCLUDED_)
#define AFX_BINDINGCONVERSIONTONODESPROPERTY_H__FA6A3E9D_57C9_414D_92FD_753FBAAE5C99__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BindingConversionToNodes.h"

class CNodeDatabase;


// Sedert versie 0x0902:
// ProcessBindings( ) int waarde teruggeven.

class CBindingConversionToNodesProperty : public CBindingConversionToNodes  
{
public:
	CBindingConversionToNodesProperty(CBindingPropertyUnit* const pBindingUnit);
	virtual ~CBindingConversionToNodesProperty();

	// ReturnWaarde.
	// < 0 is error
	// >= 0 aantal bindings die aangemaakt werden.
	int ProcessPropertyBindings(		
		BOOL fMakeNoDefaults = FALSE,
		BOOL fShowErrorMessage = TRUE);
	BOOL AddBinding(CString s);

protected:
	BOOL ClearBindings(void);
	BOOL AddBindingsToNodes(void);
	BOOL CheckBindingFileProperties(void);

private:
	CBindingPropertyUnit* const m_pBindingUnit;
	CNodeDatabase* const m_pNodeDatabase;
};


#endif // !defined(AFX_BINDINGCONVERSIONTONODESPROPERTY_H__FA6A3E9D_57C9_414D_92FD_753FBAAE5C99__INCLUDED_)


