// NodeDatabaseOperations.h: interface for the CNodeDatabaseOperations class.
//
// TM 20120404: subclassUnits( )
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NODEDATABASEOPERATIONS_H__860E91A5_C58A_4509_850A_4BED7308890D__INCLUDED_)
#define AFX_NODEDATABASEOPERATIONS_H__860E91A5_C58A_4509_850A_4BED7308890D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/********************************************************************/
class CCanNode;
class CNodeDatabase;
class CBindingUnit;
struct STRUCT_UNIT;
/********************************************************************/


/*****************************************************************//**
 * @class	CNodeDatabaseOperations
 * @brief	Node database operations.
 *
 * @author	Mitchell Tom
 * @changes	TM,v0562: ReplaceIncompatibleUnits( ). 
 ********************************************************************/
class CNodeDatabaseOperations  
{
public:
	CNodeDatabaseOperations(CNodeDatabase* const pNodeDatabase);
	virtual ~CNodeDatabaseOperations();
	CCanNode* Find(BYTE bNodeAddress);
	BOOL Find(const CBindingUnit* const pBindingUnit,CCanNode** pNode,STRUCT_UNIT **pUnit);	
	BOOL Find(BYTE bNodeAddress,BYTE bUnitAddress,STRUCT_UNIT **pUnit);
	BOOL MarkAllNamesOveridden();
	BOOL GetNROfNodesType(BYTE bNodeType,int *iNROfNodes);
	BOOL GetNROfUnitsType(BYTE bUnitType,int *iNROfUnits);
	int SubclassNodes(BYTE bNodeType,CPtrList* ptrlist);
	int SubclassNodes(CPtrList* ptrlist);
	int SubClassUnits(BYTE bUnitType,CPtrList* ptrlist);
	int SubClassUnits(CCanNode* pNode,BYTE bUnitType,CPtrList* ptrlist);	// oppassen: ptrlist wordt niet gewist !!
	BOOL CreateBroadcastBindingUnits(CCanNode *pNode);
	BOOL AddBroadcastUnit(CCanNode* pNode,STRUCT_UNIT **ReturnUnitPtr);

	/***************************************************************//**
     * @brief	Incompatible units vervangen.
	 * @pre		Zie implementatie bestand.
	 * @param	pNode: Node in de database.
	 * @param	pOldUnit: Unit in de database die moet veranderd worden.
	 * @param	pNewUnit: Src unit wellicht aanwezig in het netwerk.
	 * @since	v0562 
	 *
	 * @attention	STRUCT_UNIT::pBindingUnit wordt niet gewijzigd !!!
	 *******************************************************************/
	BOOL ReplaceIncompatibleUnits(CCanNode* pNode,STRUCT_UNIT* pOldUnit, const STRUCT_UNIT* pNewUnit);

	/***************************************************************//**
     * @brief	laatste unit verwijderen 
	 * @pre		Zie implementatie bestand.
	 * @param	pNode: Node in de database.
	 * @param	pOldUnit: Unit in de database die moet veranderd worden.
	 * @since	v0562 
	 *******************************************************************/
	BOOL RemoveTopUnit(CCanNode* pNode, STRUCT_UNIT* pOldUnit);	

protected:
	STRUCT_UNIT* Find(const CCanNode* const pNode, BYTE bUnitAddress);

private:
	CNodeDatabase* const m_pNodeDatabase;
};

#endif // !defined(AFX_NODEDATABASEOPERATIONS_H__860E91A5_C58A_4509_850A_4BED7308890D__INCLUDED_)
