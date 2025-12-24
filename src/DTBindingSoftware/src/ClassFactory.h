// ClassFactory.h: interface for the CClassFactory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CLASSFACTORY_H__226E9C3D_229D_462F_92D3_525CF647D8A2__INCLUDED_)
#define AFX_CLASSFACTORY_H__226E9C3D_229D_462F_92D3_525CF647D8A2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



class CClassFactory  
{
public:
	CClassFactory();
	virtual ~CClassFactory();

	template <class RT,class P,class Q>
		BOOL Create(RT **pReturn,P* p,Q* q);

/*
	template <class RT>
		RT* Create(const RT& = RT());

	template <class RT,class P>
		RT* Create(const RT& = RT(), P* p=NULL);

	template <class RT,class P,class Q>
		RT* Create(const RT& = RT(), P* p=NULL, Q* q=NULL);
*/
};

#endif // !defined(AFX_CLASSFACTORY_H__226E9C3D_229D_462F_92D3_525CF647D8A2__INCLUDED_)
