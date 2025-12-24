// NodeDatabaseOperations.cpp: implementation of the CNodeDatabaseOperations class.
//
// Changes: TM20120404: SubClassUnits( )
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "NodeDatabaseOperations.h"

#include "NodeDatabase.h"
#include "BindingUnits.h"
#include "CanNode.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNodeDatabaseOperations::CNodeDatabaseOperations(CNodeDatabase* const pNodeDatabase) :
m_pNodeDatabase( pNodeDatabase )
{
	ASSERT( m_pNodeDatabase );
}

CNodeDatabaseOperations::~CNodeDatabaseOperations()
{
}

CCanNode* CNodeDatabaseOperations::Find(BYTE bNodeAddress)
{
	ASSERT( m_pNodeDatabase );

	int iEntryNr;
	CCanNode* pNode = 0;
	m_pNodeDatabase->FindNode(bNodeAddress,&iEntryNr,&pNode);
	return pNode;
}

STRUCT_UNIT* CNodeDatabaseOperations::Find(const CCanNode* const pNode, BYTE bUnitAddress)
{
	ASSERT( m_pNodeDatabase );

	int iEntryNr;
	STRUCT_UNIT* pUnit = 0;
	m_pNodeDatabase->FindUnit(const_cast<CCanNode*>(pNode),bUnitAddress,&iEntryNr,&pUnit);
	return pUnit;
}

BOOL CNodeDatabaseOperations::Find(
	const CBindingUnit* const pBindingUnit,
	CCanNode **pNodeReturn,
    STRUCT_UNIT **pUnitReturn)
{
	if (pBindingUnit == 0) 
	{
		ASSERT( pBindingUnit );
		return FALSE;
	}

	if ((pUnitReturn==0)||(pNodeReturn==0))
	{
		ASSERT( pUnitReturn );
		ASSERT( pNodeReturn );
		return FALSE;
	}

	CCanNode* const pNode = Find(pBindingUnit->GetNodeAddress());
	if (pNode != 0)
	{
		STRUCT_UNIT* const pUnit = Find(pNode, pBindingUnit->GetUnitAddress());
		if (pUnit != 0)
		{
			*pUnitReturn = pUnit;
			*pNodeReturn = pNode;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CNodeDatabaseOperations::Find(BYTE bNodeAddress,BYTE bUnitAddress,STRUCT_UNIT **pUnit)
{
	CCanNode* const pNode = Find( bNodeAddress );
	if (pNode != 0)
	{
		*pUnit = Find( pNode, bUnitAddress );
		if (*pUnit != 0)
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CNodeDatabaseOperations::MarkAllNamesOveridden()
{
	ASSERT(m_pNodeDatabase);

	int nMaxNodes = 0;
	m_pNodeDatabase->NROfNodes(&nMaxNodes);

	for (int i=0;i<nMaxNodes;i++)
	{
		CCanNode* pNode = 0;
		if (m_pNodeDatabase->GetNode(i,&pNode))
		{
			for (int j=0;j<pNode->bNROfUnits;j++)	
			{
				STRUCT_UNIT* pUnit = 0;
				if (m_pNodeDatabase->GetUnit(pNode,j,&pUnit))
				{
					pUnit->ulApplicationFlags |= DEFAULT_NAME_OVERRIDDEN;
				}
			}
		}
	}
	return TRUE;
}

BOOL CNodeDatabaseOperations::CreateBroadcastBindingUnits(CCanNode *pNode) {
	STRUCT_UNIT *pUnit;	
	BYTE bLf48BoseRooms = 0;
	BYTE bNrOfBoseRooms = 0;
	const int iNrOfUnits = pNode->iNROfUnits;
	for (int j=0;j<iNrOfUnits;j++) {
		pUnit = NULL;
		if (m_pNodeDatabase->GetUnit(pNode,j,&pUnit)) {
			switch(pUnit->bUnitType) {
				case UNIT_TYPE_BOSERS: {
					if (pUnit->UnitSpecs.AudioUnit.bAudioConfig == BOSE_LIFESTYLE_SYSTEM50) {
						bNrOfBoseRooms++;
					}
					else {
						bLf48BoseRooms++;
					}
				} break;
			}
		}
		ASSERT(pUnit);
	}		
	
	if ((bNrOfBoseRooms == NR_BOSE_LF50_ROOMS) || (bLf48BoseRooms == NR_BOSE_LF48_ROOMS)) {
		// Create Virtual "Bose All Rooms Unit"
		STRUCT_UNIT *pNewUnit = NULL;		
		if (AddBroadcastUnit(pNode,&pNewUnit) == TRUE) {			
			pNewUnit->bNodeLogicalAddress = pNode->bLogicalAddress;
			pNewUnit->bUnitAddress = UNIT_VIRTUAL_BROADCAST_ADDRESS;
			pNewUnit->bUnitType = UNIT_TYPE_BOSERS;
			pNewUnit->bUnitFlags = 0;
			pNewUnit->ulApplicationFlags = 0;
			strcpy(pNewUnit->szUnitName,"BOSE|ALL ROOMS");
			pNewUnit->m_pBindingUnit = NULL;
			pNewUnit->m_pAppUnitData = NULL;
			if (bNrOfBoseRooms == NR_BOSE_LF50_ROOMS) {
				pNewUnit->UnitSpecs.AudioUnit.bAudioConfig = BOSE_LIFESTYLE_SYSTEM50;
			}
			else  {
				pNewUnit->UnitSpecs.AudioUnit.bAudioConfig = BOSE_LIFESTYLE_SYSTEM48;
			}
			return TRUE;
		}
		else {
			return FALSE;
		}
	}		
	return FALSE;
}

BOOL CNodeDatabaseOperations::AddBroadcastUnit(CCanNode* pNode,STRUCT_UNIT **ReturnUnitPtr)
{
	STRUCT_UNIT *PreviousUnitSpecsPtr = 0;	// CHANGES_MVS_2008_WARNINGS
	BYTE bUnitAddress = 0xFF;
	int i;

	STRUCT_UNIT *UnitSpecsPtr = pNode->pFirstUnit; 

	for (i=0;i<pNode->iNROfUnits;i++)
	{
		if (UnitSpecsPtr->bUnitAddress == bUnitAddress) break;

		PreviousUnitSpecsPtr = UnitSpecsPtr;
		UnitSpecsPtr = UnitSpecsPtr->pNextUnit;
	}

	if (i==pNode->iNROfUnits)
	{
		if ((UnitSpecsPtr = (STRUCT_UNIT*)malloc(sizeof(STRUCT_UNIT))) == NULL)
		{          
			return(false);
		}
         
		UnitSpecsPtr->bUnitAddress = bUnitAddress;	
#if(0)
		UnitSpecsPtr->pvData=NULL;
#endif 
		UnitSpecsPtr->m_pAppUnitData = NULL;
		UnitSpecsPtr->pNextUnit = NULL;
		UnitSpecsPtr->ulApplicationFlags = 0;
	
		pNode->iNROfUnits++; 
	
		if (pNode->bNROfUnits < pNode->iNROfUnits) 
			pNode->bNROfUnits = pNode->iNROfUnits;
         
		*ReturnUnitPtr = UnitSpecsPtr;

		if (i==0) 
		{	          
			pNode->pFirstUnit = UnitSpecsPtr;
		}
		else 
		{  
			ASSERT(PreviousUnitSpecsPtr);		// CHANGES_MVS_2008_WARNINGS
	        PreviousUnitSpecsPtr->pNextUnit = UnitSpecsPtr;
		}

	    return(true);	
	}
	return (false);
}

BOOL CNodeDatabaseOperations::GetNROfNodesType(BYTE bNodeType,int *iNROfNodes)
{
	int j;

	*iNROfNodes = 0;
	
	if (m_pNodeDatabase->NROfNodes(&j))
	{
		CCanNode *pNode;

		for (int i=0;i<j;i++)
		{
			if (m_pNodeDatabase->GetNode(i,&pNode))
			{
				if (pNode->bNodeType == bNodeType)
				{
					(*iNROfNodes)++;
				}
			}
			else
			{
				return FALSE;
			}
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CNodeDatabaseOperations::GetNROfUnitsType(BYTE bUnitType,int *iNROfUnits)
{
	int iCnt = 0;

	if (iNROfUnits != NULL) 
	{
		int iNodes;

		if (m_pNodeDatabase->NROfNodes(&iNodes))
		{	
			STRUCT_UNIT *pUnit;
			CCanNode *pNode;

			for (int i=0;i<iNodes;i++) 
			{
				if (m_pNodeDatabase->GetNode(i,&pNode)) 
				{
					const int iUnits = pNode->bNROfUnits;
			
					for (int j=0;j<iUnits;j++) 
					{
						if (m_pNodeDatabase->GetUnit(pNode,j,&pUnit)) 
						{
							if (pUnit->bUnitType == bUnitType) 
							{
								iCnt++;
							}
						}
					}
				}
			}
			*iNROfUnits = iCnt;
			return TRUE;
		}
		*iNROfUnits = 0;
	}
	return FALSE;
}

int CNodeDatabaseOperations::SubClassUnits(
	BYTE bUnitType,
	CPtrList* ptrlist)
{
	int nCount = 0;

	if (ptrlist != NULL)
	{
		ptrlist->RemoveAll();

		CNodeIterator it(m_pNodeDatabase->CreateNodeIterator());
		for (it.Begin();!it.IsDone();it.Next())
		{
			CCanNode* const pNode = it.CurrentItem( );
			nCount += this->SubClassUnits(pNode,
										  bUnitType,
										  ptrlist);
		}
	}

	return nCount;
}

int CNodeDatabaseOperations::SubClassUnits(
	CCanNode* pNode,
	BYTE bUnitType,
	CPtrList* ptrlist)
{
	int nCount = 0;

	if (ptrlist != NULL)
	{
		// Oppasen: ptrList wordt niet gewist...

		if (pNode)
		{
			CUnitIterator it(m_pNodeDatabase->CreateUnitIterator(pNode));
			for (it.Begin();!it.IsDone();it.Next())
			{
				STRUCT_UNIT* const pUnit = it.CurrentItem();
				if (pUnit->bUnitType == bUnitType)
				{
					ptrlist->AddTail( pUnit );
					nCount++;
				}
			}		
		}
	}
	return nCount;
}

int CNodeDatabaseOperations::SubclassNodes(BYTE bNodeType,CPtrList* ptrlist)
{
	int nCount = 0;

	if (ptrlist != NULL)
	{
		ptrlist->RemoveAll();

		int nNodes;
		if (m_pNodeDatabase->NROfNodes(&nNodes))
		{
			CCanNode *pNode;

			for (int i=0;i<nNodes;i++)
			{
				if (m_pNodeDatabase->GetNode(i,&pNode))
				{
					if (pNode->bNodeType == bNodeType)
					{
						ptrlist->AddTail(pNode);
						nCount++;
					}
				}
			}		
		}	
	}
	return nCount;
}


int CNodeDatabaseOperations::SubclassNodes(CPtrList* ptrlist)
{
	int nCount = 0;

	if (ptrlist != NULL)
	{
		ptrlist->RemoveAll();

		int nNodes;
		if (m_pNodeDatabase->NROfNodes(&nNodes))
		{
			CCanNode *pNode;

			for (int i=0;i<nNodes;i++)
			{
				if (m_pNodeDatabase->GetNode(i,&pNode))
				{				
					ptrlist->AddTail(pNode);
					nCount++;				
				}
			}		
		}	
	}
	return nCount;
}

// Since v0562
BOOL CNodeDatabaseOperations::ReplaceIncompatibleUnits(CCanNode* pNode,STRUCT_UNIT* pOldUnit, const STRUCT_UNIT* pNewUnit)
{
	ASSERT( pNode && pOldUnit && pNewUnit );
	ASSERT( pOldUnit->bUnitAddress == pNewUnit->bUnitAddress );	// Moeten hetzelfde unit adres hebben.
	ASSERT( pOldUnit->bNodeLogicalAddress == pNewUnit->bNodeLogicalAddress); // Moeten hetzelfde node adres hebben.
	ASSERT( pOldUnit->bUnitType != pNewUnit->bUnitType );	// Moeten incompatible zijn.
	//ASSERT( pOldUnit->pBindingUnit == 0);					// Binding property unit moet reeds verwijderd zijn.

	if (pOldUnit->bNodeLogicalAddress == pNode->bLogicalAddress)		
	{
		pOldUnit->bNodeLogicalAddress = pNewUnit->bNodeLogicalAddress;
		pOldUnit->bUnitAddress = pNewUnit->bUnitAddress;
		pOldUnit->bUnitFlags =	pNewUnit->bUnitFlags;
		pOldUnit->bUnitType = pNewUnit->bUnitType;
		strcpy(pOldUnit->szUnitName,pNewUnit->szUnitName);

		CNodeDatabase::InitRuntimeUnitSpecs( pOldUnit );

		// De unitspecs moeten reeds gealloceerd zijn !!
		memcpy(&pOldUnit->UnitSpecs, &pNewUnit->UnitSpecs, sizeof(UNION_UNITSPECS));				
		pOldUnit->UnitStatus = pNewUnit->UnitStatus;

		//pOldUnit->pBindingUnit = 0;
		pOldUnit->ulApplicationFlags = 0;	
		return TRUE;
	}
	return FALSE;
}
BOOL CNodeDatabaseOperations::RemoveTopUnit(CCanNode* pNode,  STRUCT_UNIT* pOldUnit)
{
	ASSERT( pNode && pOldUnit);	
	ASSERT( pOldUnit->bNodeLogicalAddress == pNode->bLogicalAddress); // Moeten hetzelfde node adres hebben.
	ASSERT( pOldUnit->m_pBindingUnit == NULL);					// Binding property unit moet reeds verwijderd zijn.

	if ((pOldUnit->m_pBindingUnit == NULL) && (pOldUnit->bUnitAddress == pNode->bNROfUnits-1)) {
		if (m_pNodeDatabase->DeleteUnitNR(pNode, pOldUnit->bUnitAddress)) {
			return TRUE;
		}	
	}
	return FALSE;
}

