// NodeDatabaseVerify.h: interface for the CNodeDatabaseVerify class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NODEDATABASEVERIFY_H__466501E7_5CEE_45D1_8B67_444763FE2E3A__INCLUDED_)
#define AFX_NODEDATABASEVERIFY_H__466501E7_5CEE_45D1_8B67_444763FE2E3A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



class CCanNode;
class CNodeDatabase;

///////////////////////////////////////////////////////////////////////////////////////////

class CNodeDatabaseVerify_Handler		// Basis klasse
{		
public:
	virtual void On_Incompatible_Nodes(int index, const CCanNode* pDocNode, const CCanNode* pRTNode);
	virtual void On_Removed_Nodes(int index, const CCanNode* pDocNode);
	virtual void On_Added_Nodes(int index, const CCanNode* pRTNode);

	//virtual void On_Found_Nodes(const CCanNode* pDocNode,const CCanNode* pRTNode);

};

///////////////////////////////////////////////////////////////////////////////////////////

class CNodeDatabaseVerify  
{
protected:
	CNodeDatabaseVerify_Handler* m_pHandler;

public:
	CNodeDatabaseVerify(CNodeDatabaseVerify_Handler* pHandler);
	virtual ~CNodeDatabaseVerify();

	int Check_Incompatible_Nodes( );
	int Check_Removed_Nodes( );
	int Check_Added_Nodes( );

	//int Find_Node(int bLogicalAddress);
};

///////////////////////////////////////////////////////////////////////////////////////////



#endif // !defined(AFX_NODEDATABASEVERIFY_H__466501E7_5CEE_45D1_8B67_444763FE2E3A__INCLUDED_)
