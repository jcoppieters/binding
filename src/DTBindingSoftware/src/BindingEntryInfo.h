// BindingEntryInfo.h: interface for the CBindingEntryInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGENTRYINFO_H__E6FBA1E8_31CF_4132_A83E_C81621D57923__INCLUDED_)
#define AFX_BINDINGENTRYINFO_H__E6FBA1E8_31CF_4132_A83E_C81621D57923__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "BindingEntry.h"
#include "BindingConstants.h"


class CBindingEntryInfo  
{
protected:
	CBindingEntry *const m_pBindingEntry;
	int m_NrOfTimers;
	int m_NrOfInputs;
	int m_NrOfOutputs;
	BOOL m_fInverseOperator;
	BYTE m_bBindingReturnFlags;
	BINDINGTYPE m_BindingType;

protected:
	BOOL FillBindingType();

public:
	CBindingEntryInfo(CBindingEntry* pBindingEntry = NULL);
	virtual ~CBindingEntryInfo();

	void Clear(void);
	BOOL Build();
	BOOL Verify();

	CBindingEntry* GetBindingEntry() 
	{
		return m_pBindingEntry;
	}

	void Set_Inverse(BOOL flag = TRUE);
	BOOL Is_Inverted(void) const;

	void Set_BindingReturnFlags(BYTE flags);	
	BYTE Get_BindingReturnFlags(void) const;	
	
	void Set_Type(BINDINGTYPE Type);		
	BINDINGTYPE Get_Type(void) const;

	void Set_NTimers(int timers);
	int Get_NTimers(void) const;
	
	void Set_NInputs(int inputs);
	int Get_NInputs(void) const;	

	void Set_NOutputs(int outputs);			
	int Get_NOutputs(void)const;			
};



#endif // !defined(AFX_BINDINGENTRYINFO_H__E6FBA1E8_31CF_4132_A83E_C81621D57923__INCLUDED_)
