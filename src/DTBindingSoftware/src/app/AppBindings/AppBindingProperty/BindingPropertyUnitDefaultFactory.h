// BindingPropertyUnitDefaultFactory.h: interface for the CBindingPropertyUnitDefaultFactory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGPROPERTYUNITDEFAULTFACTORY_H__ED06F87F_5781_4201_9BA3_256EAD799DC1__INCLUDED_)
#define AFX_BINDINGPROPERTYUNITDEFAULTFACTORY_H__ED06F87F_5781_4201_9BA3_256EAD799DC1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CBindingPropertyUnitDefault;

class CBindingPropertyUnitDefaultFactory	// Singleton 
{
public:
	static CBindingPropertyUnitDefaultFactory* Instance();

	struct CUnitInfo
	{
		BYTE bNodeAddress;
		BYTE bUnitAddress;
		BYTE bBindingUnitType;
	};

protected:
	CBindingPropertyUnitDefaultFactory();

public:
	virtual ~CBindingPropertyUnitDefaultFactory();

	const CBindingPropertyUnitDefault* Create(BYTE bUnitType) const;
	const CBindingPropertyUnitDefault* Create(BYTE bUnitType,const CUnitInfo& refUnitInfo) const;
};



#endif // !defined(AFX_BINDINGPROPERTYUNITDEFAULTFACTORY_H__ED06F87F_5781_4201_9BA3_256EAD799DC1__INCLUDED_)
