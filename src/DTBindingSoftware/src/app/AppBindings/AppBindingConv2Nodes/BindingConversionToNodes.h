// BindingConversionToNodes.h: interface for the CBindingConversionToNodes class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGCONVERSIONTONODES_H__2DB90941_91E1_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_BINDINGCONVERSIONTONODES_H__2DB90941_91E1_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "App\Appbindings\AppbindingFileEntry\BindingFileEntryInfo.h"


class CBindingEntry;
class CBindingUnit;
class CBindingPropertyUnit;
class CNodeDatabase;
class CCanNode;

////////////////////////////////////////////////////////

class CBindingConversionToNodes  
{
private:
	CStringArray m_szBindingStrings;

protected:	
	CStringArray* GetStringArray() 
	{
		return (&m_szBindingStrings);
	}

public:
	CBindingConversionToNodes();
	virtual ~CBindingConversionToNodes();

public:
	int AddToStringArray(CString s)		
	{
		return (m_szBindingStrings.Add(s));
	}
	
	void ClearStringArray(void)	
	{
		m_szBindingStrings.RemoveAll();
	}
	
	int GetStringArraySize(void) const
	{
		return (m_szBindingStrings.GetSize());
	}
	
	CString GetStringFromArray(int i) const
	{
		return (m_szBindingStrings.GetAt(i));
	};

protected:
	BOOL CheckNrEntries(CNodeDatabase *ptr);

	// CHANGES_BINDINGS_CONDITIONLENGTH
	BOOL CheckCBindingConditionLength(CNodeDatabase *ptr);

	// CHANGES_BINDINGS_ERRORMESSAGES
	typedef enum
	{
		ERROR_MAX_BINDINGS = 0,
		ERROR_MAX_TIMER_OPERATORS,
		ERROR_MAX_C_BINDINGS,
		ERROR_MAX_REFERENCES,
		ERROR_MAX_C_LENGTH

	} Error_t;

	void notifyError(
		Error_t err,
		CCanNode* pNode,
		int neededEntries);		
};
////////////////////////////////////////////////////////
#endif // !defined(AFX_BINDINGCONVERSIONTONODES_H__2DB90941_91E1_11D8_B865_0050BAC412B1__INCLUDED_)
