// IPropertyPageNode.h: interface for the IPropertyPageNode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPROPERTYPAGENODE_H__8253F200_DD2C_4C29_8247_89A9F10B0979__INCLUDED_)
#define AFX_IPROPERTYPAGENODE_H__8253F200_DD2C_4C29_8247_89A9F10B0979__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class IPropertyPageNode : public CPropertyPage
{
public:
	
	IPropertyPageNode() {
	
	}

	IPropertyPageNode(unsigned short nID) :
	  CPropertyPage(nID) 
	{
	  
	};

	virtual ~IPropertyPageNode() {
	  
	};

public:
	virtual void UpdateRunTimeControls() = 0;
	
};

#endif // !defined(AFX_IPROPERTYPAGENODE_H__8253F200_DD2C_4C29_8247_89A9F10B0979__INCLUDED_)
