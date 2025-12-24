// BindingLink.h: interface for the CBindingLink class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGLINK_H__C9953FA6_765C_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_BINDINGLINK_H__C9953FA6_765C_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BindingConstants.h"

////////////////////////////////////////////////////////

typedef struct
{	
	ControlType m_Type;				/* UNIT / OPERATOR */
	unsigned short m_nID;			/* Id f(Type) */

}BINDING_CONNECTION;

////////////////////////////////////////////////////////

class CBindingLink  
{
public:
	CBindingLink();
	virtual ~CBindingLink();

private:
	/**
	 * Overwrite default : declarations with no defintion !!!
	 */
	//CBindingLink(const CBindingLink& rhs );
	//CBindingLink& operator=( const CBindingLink& rhs);

private:	
	unsigned short m_nID;
	BOOL m_fInverse;
	BINDING_CONNECTION m_InLink;
	BINDING_CONNECTION m_OutLink;
	unsigned long m_ulApplicationFlags;

public:
	void SetID(unsigned short);
	unsigned short GetID(void) const;
	
	void SetInLink(unsigned short ID,ControlType type);	
	void SetOutLink(unsigned short ID,ControlType type);

	unsigned short GetInLinkID(void) const;
	ControlType GetInLinkType(void) const;

	unsigned short GetOutLinkID(void) const;
	ControlType GetOutLinkType(void) const;
	
	void SetInverse(BOOL flag);
	BOOL isInverted(void) const;

	void SetApplicationFlags(unsigned long x) {
		m_ulApplicationFlags = x; 
	};	
	unsigned long GetApplicationFlags(void)	const {
		return m_ulApplicationFlags; 
	};
};

/////////////////////////////////////////////////////////


#endif // !defined(AFX_BINDINGLINK_H__C9953FA6_765C_11D8_B865_0050BAC412B1__INCLUDED_)
