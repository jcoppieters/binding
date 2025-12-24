// NodeDatabaseVerify_Handlers.h: interface for the CNodeDatabaseVerify_Handlers class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NODEDATABASEVERIFY_HANDLERS_H__D27F71FF_08B6_4F1F_A543_F441F442D87D__INCLUDED_)
#define AFX_NODEDATABASEVERIFY_HANDLERS_H__D27F71FF_08B6_4F1F_A543_F441F442D87D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "NodeDatabaseVerify.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
class CPropertyWizardPage0;
class CPropertyWizardPage7;
class CPropertyWizardPage8;
class CCanNode;
//////////////////////////////////////////////////////////////////////////////////////////////////

class CNodeDatabaseVerify_Handler0 : public CNodeDatabaseVerify_Handler
{
protected:
	CPropertyWizardPage8& m_Parent;			// CPropertyWizardPage8 !!!!!!!!!!

public:
	CNodeDatabaseVerify_Handler0(CPropertyWizardPage8& parent);
	virtual ~CNodeDatabaseVerify_Handler0();

	virtual void On_Incompatible_Nodes(int index,const CCanNode* pDocNode,const CCanNode* pRTNode);
};


//////////////////////////////////////////////////////////////////////////////////////////////////

class CNodeDatabaseVerify_Handler1 : public CNodeDatabaseVerify_Handler
{
protected:
	CPropertyWizardPage7& m_Parent;			// CPropertyWizardPage7 !!!!!!!!!!

public:
	CNodeDatabaseVerify_Handler1(CPropertyWizardPage7& parent);
	virtual ~CNodeDatabaseVerify_Handler1();


	virtual void On_Incompatible_Nodes(int index,const CCanNode* pDocNode,const CCanNode* pRTNode);
	virtual void On_Removed_Nodes(int index, const CCanNode* pDocNode);
	virtual void On_Added_Nodes(int index, const CCanNode* pRTNode);


};

//////////////////////////////////////////////////////////////////////////////////////////////////

class CNodeDatabaseVerify_Handler2 : public CNodeDatabaseVerify_Handler
{
protected:
	CPropertyWizardPage0& m_Parent;			// CPropertyWizardPage0 !!!!!!!!!!

public:
	CNodeDatabaseVerify_Handler2(CPropertyWizardPage0& parent);
	virtual ~CNodeDatabaseVerify_Handler2();

	virtual void On_Incompatible_Nodes(int index,const CCanNode* pDocNode,const CCanNode* pRTNode);
	virtual void On_Removed_Nodes(int index, const CCanNode* pDocNode);
	virtual void On_Added_Nodes(int index, const CCanNode* pRTNode);
};
//////////////////////////////////////////////////////////////////////////////////////////////////



#endif // !defined(AFX_NODEDATABASEVERIFY_HANDLERS_H__D27F71FF_08B6_4F1F_A543_F441F442D87D__INCLUDED_)
