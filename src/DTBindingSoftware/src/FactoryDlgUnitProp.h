// FactoryDlgUnitProp.h: interface for the CFactoryDlgUnitProp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FACTORYDLGUNITPROP_H__2ACBE128_62AE_4430_843B_1564969C9817__INCLUDED_)
#define AFX_FACTORYDLGUNITPROP_H__2ACBE128_62AE_4430_843B_1564969C9817__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDlgUnitProp;

class CCanNode;
struct STRUCT_UNIT;


struct CFactoryDlgUnitProp  
{
	static CDlgUnitProp* Create(const CCanNode* pNode,STRUCT_UNIT* pUnit);
};

#endif // !defined(AFX_FACTORYDLGUNITPROP_H__2ACBE128_62AE_4430_843B_1564969C9817__INCLUDED_)
