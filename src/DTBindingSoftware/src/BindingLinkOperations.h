// BindingLinkOperations.h: interface for the CBindingLinkOperations class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGLINKOPERATIONS_H__375B5590_BA8E_4D90_B6AA_165EE110F7B6__INCLUDED_)
#define AFX_BINDINGLINKOPERATIONS_H__375B5590_BA8E_4D90_B6AA_165EE110F7B6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CBindingLink;

class CBindingLinkOperations  
{
protected:
	CBindingLink* const m_pBindingLink;

public:
	CBindingLinkOperations(CBindingLink* const pBindingLink);
	virtual ~CBindingLinkOperations();
	CString ToString();
};

#endif // !defined(AFX_BINDINGLINKOPERATIONS_H__375B5590_BA8E_4D90_B6AA_165EE110F7B6__INCLUDED_)
