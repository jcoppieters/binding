// BindingConversionToNodesNormal.h: interface for the CBindingConversionToNodesNormal class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGCONVERSIONTONODESNORMAL_H__9D277661_8AC3_481E_9DCA_6513AF8BA5F1__INCLUDED_)
#define AFX_BINDINGCONVERSIONTONODESNORMAL_H__9D277661_8AC3_481E_9DCA_6513AF8BA5F1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BindingConversionToNodes.h"

class CBindingEntryInfo;


// Changes versie 0x0902:
// CreateBindingStrings( ) functies.
// ProcessBindings( ) maakt gebruik van de CreateBindingStrings( )
class CBindingConversionToNodesNormal : public CBindingConversionToNodes  
{
public:
	CBindingConversionToNodesNormal();
	virtual ~CBindingConversionToNodesNormal();

	BOOL ProcessBindings(CBindingEntry *pEntry);
	BOOL DeleteBindings(unsigned short usBindingNr);

	// Aanmaken van de binding strings.
	BOOL CreateBindingStrings(
		CBindingEntry *pEntry,
		CStringArray* pStrArray);

protected:
	BOOL CreateBindingStrings(
		CBindingEntry *pEntry);
	BOOL CreateBindingStrings(
		CBindingEntry *pEntry,
		CBindingEntryInfo* pBindingEntryInfo);
	BOOL ConstructBindingStrings(CBindingEntryInfo* const pBindingEntryInfo);
	BOOL AddBindingsToNodes(
		CBindingEntry *pEntry,
		CBindingEntryInfo* const pBindingEntryInfo);
	BOOL CheckBindingFileProperties(CBindingEntryInfo* const pBindingEntryInfo);
	BOOL CheckCBindingEntry(CNodeDatabase *pDocDatabase);
};

#endif // !defined(AFX_BINDINGCONVERSIONTONODESNORMAL_H__9D277661_8AC3_481E_9DCA_6513AF8BA5F1__INCLUDED_)
