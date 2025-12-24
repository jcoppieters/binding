// NodeDatabase.h: interface for the CNodeDatabase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NODEDATABASE_H__60899215_39FC_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_NODEDATABASE_H__60899215_39FC_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

///////////////////////////////////////////////////////////////////
#include "CanNode.h"

struct STRUCT_POWERMATE
{
    unsigned long ulReserved[11];
    int iNROfNodes;  
	CCanNode *pFirstNode;
};

/***************************************************************//**
 * @class	CNodeIterator
 * @brief	Iterator voor de nodes in de database
 *			Iterator voor de nodes in de database.
 *			Onveilig: Mag niet gebruikt worden om elementen toe te
 *		    voegen of te verwijderen.
 * @author	Mitchell Tom
 * @date	20081223
 ******************************************************************/
class CNodeIterator
{
protected:
	STRUCT_POWERMATE* m_pDatabase;
	CCanNode* m_pos;

public:
	CNodeIterator(STRUCT_POWERMATE* pDatabase);
	~CNodeIterator( );

	void Next( );
	void Begin( );
	bool IsDone( ) const;

	CCanNode* CurrentItem( );
	const CCanNode* CurrentItem( ) const;
};

/***************************************************************//**
 * @class	CUnitIterator
 * @brief	Iterator voor de Units van een node.
 *			Iterator voor de Units van een node.
 *			Onveilig: Mag niet gebruikt worden om elementen toe te
 *		    voegen of te verwijderen.
 * @author	Mitchell Tom
 * @date	20081224
 ******************************************************************/
class CUnitIterator
{
protected:
	CCanNode*		m_pNode;
	STRUCT_UNIT*	m_pos;

public:
	CUnitIterator(CCanNode* pNode);
	~CUnitIterator( );

	void Next( );
	void Begin( );
	bool IsDone( ) const;

	STRUCT_UNIT* CurrentItem( );
	const STRUCT_UNIT* CurrentItem( ) const;
};

///////////////////////////////////////////////////////////////////

// TODO (Als we eens veel tijd en goesting hebben).
// (static) Functies van CNodeDatabase verplaatsen naar CNodeDatabase_Util.
class CNodeDatabase  
{
private:
	STRUCT_POWERMATE Database;	

public:
	CNodeDatabase();
	virtual ~CNodeDatabase();

protected:
	BOOL NROfUnits(CCanNode* ,int *piNROfUnits);

public:	
	static BYTE CopyDatabase(CNodeDatabase *pDest,CNodeDatabase *pSource);
	static BYTE CopyUnitObject(STRUCT_UNIT *pDestUnit, STRUCT_UNIT *pSourceUnit);
	static BYTE CopyNodeObject(CCanNode *pDestNode, CCanNode *pSourceNode);

	// Oppassen: Enkel de runtime specs. bConfig niet!
	static BOOL InitRuntimeUnitSpecs(STRUCT_UNIT *pUnit);

	CString GetUnitName(BYTE bNodeAddress,BYTE bUnitAddress);
	// ONLY FOR DOCUMENT DATABASE
	BOOL ChangeUnitName(BYTE bNodeAddress,BYTE bUnitAddress,CString szName);
	BOOL SetUnitName(BYTE bNodeAddress,BYTE bUnitAddress,CString szName);
	
	CString GetNodeName(BYTE bNodeAddress);
	BOOL ChangeNodeName(BYTE bNodeAddress,CString szName);

	BOOL InitDatabase();
	void ClearDatabase();

	BOOL NROfNodes(int *piNROfNodes);
	
	/* Node Operations */
	BOOL GetNode(int iEntryNR,CCanNode **ReturnNodePtr);
	BOOL FindNode(BYTE bLogicalAddress, int *piNodeNR,CCanNode **ReturnNodePtr);
	BOOL FindPhysicalNode(unsigned long ulPhysicalAddress, int *piNodeNR,CCanNode **ReturnNodePtr);
	BOOL AddNode(BYTE bLogicalAddress, int *piNodeNR,CCanNode **ReturnNodePtr);
	BOOL AddPhysicalNode(unsigned long ulPhysicalAddress, int *piNodeNR,CCanNode **ReturnNodePtr);
	BOOL DeleteNodeAddress(BYTE bLogicalAddress);
	BOOL DeleteNodeNR(int iNodeNR);

		
	/* Unit Operations */
	BOOL GetUnit(CCanNode* ,int iEntryNR, STRUCT_UNIT **ReturnUnitSpecs);
	BOOL AddUnit(CCanNode* ,BYTE bUnitAddress,int *piUnitNR, STRUCT_UNIT **ReturnUnitSpecsPtr);    
	BOOL FindUnit(CCanNode* ,BYTE bUnitAddress,int *piUnitNR, STRUCT_UNIT **ReturnUnitPtr);
	BOOL DeleteUnitNR(CCanNode* ,int iUnitNR);
					
public:
	CNodeIterator CreateNodeIterator()		// TM 20081223
	{
		return CNodeIterator( &Database );
	}
	CUnitIterator CreateUnitIterator(CCanNode* const pNode)
	{
		return CUnitIterator( pNode );		// TM 20081224
	}
};

#endif // !defined(AFX_NODEDATABASE_H__60899215_39FC_11D8_B865_0050BAC412B1__INCLUDED_)
