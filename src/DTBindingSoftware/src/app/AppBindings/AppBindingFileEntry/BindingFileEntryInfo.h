// BindingFileEntryInfo.h: interface for the CBindingFileEntryInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGFILEENTRYINFO_H__F5520AEC_BA44_4036_A536_8BB6B8202DA2__INCLUDED_)
#define AFX_BINDINGFILEENTRYINFO_H__F5520AEC_BA44_4036_A536_8BB6B8202DA2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BindingConstants.h"

class CBindingFileEntryInfo  
{
public:
	CBindingFileEntryInfo();
	virtual ~CBindingFileEntryInfo();

	void Clear(void)
	{
		m_NrOfTimers = 0; 
		m_NrOfInputs = 0; 
		m_NrOfOutputs = 0;
		m_fInverseOperator = FALSE;
		m_bBindingFlags = 0;
		m_BindingType = TYPE_UNDEFINED_BINDING;	
	}

	void IncNTimers(void)
	{
		m_NrOfTimers++;
	};

	void IncNInputs(void)
	{
		m_NrOfInputs++;
	};

	void IncNOutputs(void)
	{
		m_NrOfOutputs++;
	};

	void Set_Inverse(BOOL flag = TRUE)
	{
		m_fInverseOperator = flag; 
	};

	void Set_BindingFlags(BYTE flags)	
	{
		m_bBindingFlags = flags; 
	};

	void Set_Type(BINDINGTYPE Type)		
	{
		m_BindingType = Type; 
	};

	void Set_NTimers(int timers)
	{
		m_NrOfTimers = timers;
	};

	void Set_NInputs(int inputs)			
	{
		m_NrOfInputs = inputs;
	};

	void Set_NOutputs(int outputs)			
	{
		m_NrOfOutputs = outputs;
	};

	int Get_NTimers(void) const
	{
		return m_NrOfTimers;
	};

	int Get_NInputs(void) const
	{
		return m_NrOfInputs;
	};

	int Get_NOutputs(void) const			
	{
		return m_NrOfOutputs;
	};

	BYTE Get_BindingFlags(void) const		
	{
		return m_bBindingFlags;
	};

	BOOL Is_Inverted(void) const
	{
		return m_fInverseOperator; 
	};

	BINDINGTYPE Get_Type(void) const	
	{
		return m_BindingType; 
	};

private:	
	int m_NrOfTimers;
	int m_NrOfInputs;
	int m_NrOfOutputs;
	BOOL m_fInverseOperator;
	BYTE m_bBindingFlags;
	BINDINGTYPE m_BindingType;
};

#endif // !defined(AFX_BINDINGFILEENTRYINFO_H__F5520AEC_BA44_4036_A536_8BB6B8202DA2__INCLUDED_)
