// BindingLinkFactory.h: interface for the CBindingLinkFactory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGLINKFACTORY_H__D8243655_1754_4FAF_A595_FD600335E14A__INCLUDED_)
#define AFX_BINDINGLINKFACTORY_H__D8243655_1754_4FAF_A595_FD600335E14A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CBindingLink;

class CBindingLinkFactory  
{
public:
	CBindingLinkFactory();
	virtual ~CBindingLinkFactory();

public:
	static CBindingLink* Create();
	static CBindingLink* Clone(const CBindingLink* pBindingLink);
};

#endif // !defined(AFX_BINDINGLINKFACTORY_H__D8243655_1754_4FAF_A595_FD600335E14A__INCLUDED_)
