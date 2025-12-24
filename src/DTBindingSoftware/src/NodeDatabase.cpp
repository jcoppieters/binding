// NodeDatabase.cpp: implementation of the CNodeDatabase class.
//
// TM_CHANGES_ALARM_ARITECH:
// UnitSpecs.AlarmUnit initialiseren.
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "NodeDatabase.h"

#include "App\Appbindings\appbindingproperty\BindingPropertyUnit.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
#define PMError
//////////////////////////////////////////////////////////////////////
CNodeIterator::CNodeIterator( STRUCT_POWERMATE* pDatabase) :
m_pDatabase( pDatabase ),
m_pos( 0 )
{
	Begin( );
}
CNodeIterator::~CNodeIterator()
{
}
void CNodeIterator::Next( )
{
	m_pos = m_pos->pNextNode;
}
void CNodeIterator::Begin( )
{
	m_pos = m_pDatabase->pFirstNode;
}
bool CNodeIterator::IsDone( ) const
{
	return (m_pos == 0);
}
CCanNode* CNodeIterator::CurrentItem( )
{
	ASSERT( m_pos );
	return m_pos;
}
const CCanNode* CNodeIterator::CurrentItem( ) const
{
	ASSERT( m_pos );
	return m_pos;
}
//////////////////////////////////////////////////////////////////////
CUnitIterator::CUnitIterator(CCanNode* pNode) :
m_pNode( pNode )
{
	ASSERT( pNode );
	Begin( );
}
CUnitIterator::~CUnitIterator( )
{
}
void CUnitIterator::Next( )
{
	m_pos = m_pos->pNextUnit;
}
void CUnitIterator::Begin( )
{
	m_pos = m_pNode->pFirstUnit;
}
bool CUnitIterator::IsDone( ) const
{
	return (m_pos == 0);
}
STRUCT_UNIT* CUnitIterator::CurrentItem( )
{
	ASSERT( m_pos );
	return m_pos;	
}
const STRUCT_UNIT* CUnitIterator::CurrentItem( ) const
{
	ASSERT( m_pos );
	return m_pos;
}

/////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CNodeDatabase::CNodeDatabase()
{
	InitDatabase();
}
CNodeDatabase::~CNodeDatabase()
{
}
////////////////////////////////////////////////////////////////////

/*******************************************************************
    BOOL InitPMDatabase()
    Description : Initialize Powermate database.
    Input       : 
    Output      : 
    Return      : true
*******************************************************************/

BOOL CNodeDatabase::InitDatabase()
{
	BYTE i;
    
	if ((Database.iNROfNodes >= 0) && (Database.iNROfNodes <= 255))
			ClearDatabase();

	/* Initialisation*/
    Database.iNROfNodes = 0;
    Database.pFirstNode = NULL;
     
    for (i=0;i<11;i++) Database.ulReserved[i]=0;
	return(true); 
}

	
void CNodeDatabase::ClearDatabase()
{
	 while(DeleteNodeNR(0)); // free memory...
}


//////////////////////// Node Operations ///////////////////////////

/****************************************************************************
    BOOL GetNode(int iEntryNR, STRUCT_NODE **ReturnNodePtr)
    Description : Get node element in powermate database using ID. 
    Input       : iEntryNR : ID in database 
				  (0=first element, 1 =second, .... (NR of elements-1))
    Output      : ** ReturnNodePtr : Reference to STRUCT_NODE pointer 
				  to return pointer to found NODE element.
    Return      : true or false 
****************************************************************************/

BOOL CNodeDatabase::GetNode(int iEntryNR, CCanNode /*STRUCT_NODE*/ **ReturnNodePtr)
{
	CCanNode *NodePtr;
	int i;

	if (iEntryNR >=Database.iNROfNodes) return(false);   
   
	/* Check if node address already exist ?*/
	NodePtr = Database.pFirstNode; 

	for (i=0;i<iEntryNR;i++)
	{
		NodePtr = NodePtr->pNextNode;
	}

	*ReturnNodePtr = NodePtr;

	return(true);
}


/**********************************************************************************************
    BOOL NROfNodes(int * piNROfNodes)
    Description : Get NR of node elements in database.
    Input       : 
    Output      : piNROfNodes : Reference to integer telling NR of node elements in database.
    Return      : true or false 
**********************************************************************************************/

BOOL CNodeDatabase::NROfNodes(int *piNROfNodes)
{
   *piNROfNodes = Database.iNROfNodes;
   return(true);
}



/**********************************************************************************************
    BOOL FindNode(BYTE bAddress,int *piNodeNR, STRUCT_NODE **ReturnNodePtr)
    Description : Find node element in powermate database. 
    Input       : bAddress : 8-bit logical Address of Powermate node to find
    Output      : *piNodeNR  : Reference to integer to return ID.
                  ** ReturnNodePtr : Reference to STRUCT_NODE pointer to return pointer to found NODE element.
    Return      : true or false 
**********************************************************************************************/

BOOL CNodeDatabase::FindNode(BYTE bAddress, int *piNodeNR, /*STRUCT_NODE*/ CCanNode **ReturnNodePtr)
{
	CCanNode *NodePtr;
	int i;

	if (Database.iNROfNodes == 0) return(false);   
 
	NodePtr = Database.pFirstNode; 

	for (i=0;i<Database.iNROfNodes;i++)
	{
		if (NodePtr->bLogicalAddress == bAddress) break;
			NodePtr = NodePtr->pNextNode;
	}

	if (i!=Database.iNROfNodes)
	{
		*piNodeNR = i;
		*ReturnNodePtr = NodePtr;
		return(true);
	}
	else 
		return(false);
}

/**********************************************************************************************
    BOOL FindPhysicalNode(unsigned long ulPhysicalAddress,int *piNodeNR, STRUCT_NODE **ReturnNodePtr)
    Description : Find node element in powermate database. 
    Input       : bAddress : 24-bit physical Address of Powermate node to find
    Output      : *piNodeNR  : Reference to integer to return ID.
                  ** ReturnNodePtr : Reference to STRUCT_NODE pointer to return pointer to found NODE element.
    Return      : true or false 
**********************************************************************************************/

BOOL CNodeDatabase::FindPhysicalNode(unsigned long ulPhysicalAddress, int *piNodeNR, /*STRUCT_NODE*/ CCanNode **ReturnNodePtr)
{
	/* Check if node address already exist ?*/
	CCanNode *NodePtr;
	int i;

	if (Database.iNROfNodes == 0) return(false);   
 
	/* Check if valid bAddress ?*/
	if ((ulPhysicalAddress<0x00ffffff) && (ulPhysicalAddress > 0x00000000))
	{
		NodePtr = Database.pFirstNode; 

		for (i=0;i<Database.iNROfNodes;i++)
		{
			if (NodePtr->ulPhysicalAddress == ulPhysicalAddress) break;
			NodePtr = NodePtr->pNextNode;
		}

		if (i!=Database.iNROfNodes)
		{
			*piNodeNR = i;
			*ReturnNodePtr = NodePtr;
			return(true);
		}
		else return(false);
	}
	else return(false);
}


/**********************************************************************************************
    BOOL AddNode(BYTE bAddress,int *piNodeNR, STRUCT_NODE **ReturnNodePtr)                 
    Description : Add node element to powermate database. 
    Input       : bAddress : 8-bit logical Address of Powermate node 
    Output      : *piNodeNR  : Reference to integer to return ID.
                  ** ReturnNodePtr : Reference to STRUCT_NODE pointer to return pointer to newly created NODE element.
    Return      : true or false 
**********************************************************************************************/
BOOL CNodeDatabase::AddNode(BYTE bAddress, int *piNodeNR, /*STRUCT_NODE*/ CCanNode **ReturnNodePtr)
{
	CCanNode* NodePtr = 0;			// CHANGES_MVS_2008_WARNINGS
	CCanNode* PreviousNodePtr = 0;	// CHANGES_MVS_2008_WARNINGS
	int i;
   
	/* Check if valid bAddress ? */
	if ((bAddress <NODE_ILLEGAL_ADDRESS) && (bAddress >= FIRST_VALID_LOGICAL_ADDRESS))
	{
		NodePtr = Database.pFirstNode; 

		for (i=0;i<Database.iNROfNodes;i++)
		{
			if (NodePtr->bLogicalAddress == bAddress) 
				break;

			PreviousNodePtr = NodePtr;
			NodePtr = NodePtr->pNextNode;
		}

		if (i==Database.iNROfNodes)
		{		
			if ((NodePtr = new CCanNode(bAddress)) == NULL)
			//if ((NodePtr = (CCanNode*)malloc(sizeof(CCanNode))) == NULL)
			{
	            PMError("No memory|left for|allocation,|system halted.",PM_FATAL_ERROR);
				return(false);
			}
/*         
			NodePtr->iNROfUnits = 0;
			NodePtr->pFirstUnit = NULL;
			NodePtr->pNextNode = NULL;

			NodePtr->ulReserved[0]=0;
			NodePtr->ulReserved[1]=0;
			NodePtr->ulReserved[2]=0;
			NodePtr->ulReserved[3]=0;
			NodePtr->ulReserved[4]=0;
			NodePtr->ulReserved[5]=0;		
			
			NodePtr->pvData = NULL;
			NodePtr->ulApplicationFlags = 0;
*/

			if (i==0) /* If first node in list...*/
			{
	            /* OK, we didn't find any duplicate and so we have also the new entry NR...
		           Create new node object and adjust pointers...*/
				Database.pFirstNode = NodePtr;
				// NodePtr->bLogicalAddress = bAddress;
				*ReturnNodePtr = NodePtr;
				/* Increment iNROfNodes member variable.*/
				Database.iNROfNodes++; 
				*piNodeNR = i;
			}
			else /* If following node in list...*/
			{  
				ASSERT(PreviousNodePtr != 0);	// CHANGES_MVS_2008_WARNINGS

	            PreviousNodePtr->pNextNode = NodePtr;
		        // NodePtr->bLogicalAddress = bAddress;
			    *ReturnNodePtr = NodePtr;

	            /* Increment iNROfNodes member variable.*/
				Database.iNROfNodes++; 
				*piNodeNR = i;
			}

			return(true);
		}
		else return(false);
	}
	else
		return(false);
}

/**********************************************************************************************
    BOOL AddPhysicalNode(unsigned long  ulPhysicalAddress,int *piNodeNR, STRUCT_NODE **ReturnNodePtr)
    Description : Add node element to powermate database. 
    Input       : ulPhysicalAddress : 24-bit physical Address of Powermate node 
    Output      : *piNodeNR  : Reference to integer to return ID.
                  ** ReturnNodePtr : Reference to STRUCT_NODE pointer to return pointer to newly created NODE element.
    Return      : true or false 
**********************************************************************************************/

BOOL CNodeDatabase::AddPhysicalNode(unsigned long ulPhysicalAddress, int *piNodeNR, /*STRUCT_NODE*/ CCanNode **ReturnNodePtr)
{
	/* Check if node address already exist ?*/

	CCanNode* NodePtr = 0;			// CHANGES_MVS_2008_WARNINGS
	CCanNode* PreviousNodePtr = 0;	// CHANGES_MVS_2008_WARNINGS
	int i;

	/* Check if valid bAddress ?*/
	if ((ulPhysicalAddress<0x00ffffff) && (ulPhysicalAddress > 0x00000000))
	{
		NodePtr = Database.pFirstNode; 

		for (i=0;i<Database.iNROfNodes;i++)
		{
			if (NodePtr->ulPhysicalAddress == ulPhysicalAddress) break;
			PreviousNodePtr = NodePtr;
			NodePtr = NodePtr->pNextNode;
		}

		if (i==Database.iNROfNodes)
		{
			if ((NodePtr = new CCanNode(ulPhysicalAddress)) == NULL)
			{
	            PMError("No memory|left for|allocation,|system halted.",PM_FATAL_ERROR);
				return(false);
			}
/*
			NodePtr->iNROfUnits = 0;
			NodePtr->pFirstUnit = NULL;
			NodePtr->pNextNode = NULL;         
			NodePtr->ulReserved[0]=0;
			NodePtr->ulReserved[1]=0;
			NodePtr->ulReserved[2]=0;
			NodePtr->ulReserved[3]=0;
			NodePtr->ulReserved[4]=0;
			NodePtr->ulReserved[5]=0;
			NodePtr->pvData = NULL;
			NodePtr->ulApplicationFlags = 0;
*/

			if (i==0) /* If first node in list...*/
			{
	            /* OK, we didn't find any duplicate and so we have also the new entry NR...
		           Create new node object and adjust pointers...*/
			    Database.pFirstNode = NodePtr;
	            // NodePtr->ulPhysicalAddress = ulPhysicalAddress;
            
		        *ReturnNodePtr = NodePtr;
			    /* Increment iNROfNodes member variable.*/
	            Database.iNROfNodes++; 
		        *piNodeNR = i;
			}
			else /* If following node in list...*/
			{  
				ASSERT(PreviousNodePtr != 0);	// CHANGES_MVS_2008_WARNINGS

	            PreviousNodePtr->pNextNode = NodePtr;
		        // NodePtr->ulPhysicalAddress = ulPhysicalAddress;
			    *ReturnNodePtr = NodePtr;
	            /* Increment iNROfNodes member variable.*/
		        Database.iNROfNodes++; 
			    *piNodeNR = i;
			}

			return(true);
		}
		else return(false);
	}
	else
		return(false);
}

/**********************************************************************************************
    BOOL DeleteNodeAddress(BYTE bAddress)
    Description : Delete node element from powermate database. 
    Input       : bAddress : 8-bit logical Address of Powermate node to delete.
    Output      : 
    Return      : true or false 
**********************************************************************************************/

BOOL CNodeDatabase::DeleteNodeAddress(BYTE bAddress)
{
	CCanNode *NodePtr = 0;
	CCanNode* PreviousNodePtr = 0;		// CHANGES_MVS_2008_WARNINGS
	int i;

	/* Check if valid bAddress ?*/
	if ((bAddress <NODE_ILLEGAL_ADDRESS) && (bAddress >= FIRST_VALID_LOGICAL_ADDRESS))
	{
		if (Database.iNROfNodes == 0) return(false);
      
		NodePtr = Database.pFirstNode; 

		for (i=0;i<Database.iNROfNodes;i++)
		{
			if (NodePtr->bLogicalAddress == bAddress) break;
			PreviousNodePtr = NodePtr;
			NodePtr = NodePtr->pNextNode;
		}

		if (i==Database.iNROfNodes) return(false);

		if (i==0) /* If first node in list...*/
		{
			Database.pFirstNode = NodePtr->pNextNode;
		}
		else /* If following node in list...*/
		{  
			ASSERT(PreviousNodePtr != 0);	// CHANGES_MVS_2008_WARNINGS

			PreviousNodePtr->pNextNode = NodePtr->pNextNode;
		}

		/* free all unit elements attached to the node */
		while(DeleteUnitNR(NodePtr,0));
      
/*		
		if (NodePtr->pvData != NULL) 
			delete NodePtr->pvData;	//free(NodePtr->pvData);		
		NodePtr->pvData = NULL;      
		//free(NodePtr);
*/
		delete NodePtr;
      
		/* Decrement iNROfNodes member variable.*/
		Database.iNROfNodes--; 

		return(true);
	}
	else return(false);
}


/**********************************************************************************************
    BOOL DeleteNodeNR(int iNodeNR)
    Description : Delete node element from powermate database. 
    Input       : iNodeNR : ID in database.
                  TIP : To delete all nodes, Delete ID 0 until false returns! 
    Output      : 
    Return      : true or false 
**********************************************************************************************/
BOOL CNodeDatabase::DeleteNodeNR(int iNodeNR)
{
	CCanNode* NodePtr = 0;				// CHANGES_MVS_2008_WARNINGS
	CCanNode* PreviousNodePtr = 0;		// CHANGES_MVS_2008_WARNINGS
	int i;	

	if (Database.iNROfNodes == 0) return(false);
   
	NodePtr = Database.pFirstNode; 

	if (iNodeNR<Database.iNROfNodes)
	{
		if (iNodeNR==0) /* If first node in list...*/
		{
			Database.pFirstNode = NodePtr->pNextNode;
		}
		else /* If following node in list...*/
		{  
			for (i=0;i<iNodeNR;i++)
			{
	            PreviousNodePtr = NodePtr;
		        NodePtr = NodePtr->pNextNode;
			}

			ASSERT(PreviousNodePtr != 0);	// CHANGES_MVS_2008_WARNINGS
			PreviousNodePtr->pNextNode = NodePtr->pNextNode;
		}

		while(DeleteUnitNR(NodePtr,0));
/*		
		if (NodePtr->pvData != NULL) 
			delete NodePtr->pvData; //free(NodePtr->pvData);
		NodePtr->pvData = NULL;
      
		//free(NodePtr);
*/

		delete NodePtr;
      
		/* Increment iNROfNodes member variable.*/
		Database.iNROfNodes--; 

		return(true);
	}
	else return(false);
}


//////////////////////// Unit Operations ///////////////////////////



/**********************************************************************************************
    BOOL GetUnit(STRUCT_NODE *pNode,int iEntryNR, STRUCT_UNIT **ReturnUnitPtr)
    Description : Get Unit element in powermate database's NODE using element ID (0 .. (NROfElements-1)). 
    Input       : *pNode : Reference to STRUCT_NODE
                  iEntryNR :  Entry in list (0 .. (NROfElements-1)). 
    Output      : ** ReturnUnitPtr : Reference to STRUCT_UNIT pointer to return pointer to found UNIT element.
    Return      : true or false 
**********************************************************************************************/

BOOL CNodeDatabase::GetUnit(/*STRUCT_NODE*/ CCanNode *pNode,int iEntryNR, STRUCT_UNIT **ReturnUnitSpecs)
{
	STRUCT_UNIT *UnitSpecsPtr;
	int i;

	if (iEntryNR >= pNode->iNROfUnits) return(false);   
 
	UnitSpecsPtr = pNode->pFirstUnit; 

	for (i=0;i<iEntryNR;i++)
	{
		UnitSpecsPtr = UnitSpecsPtr->pNextUnit;
	}	

	*ReturnUnitSpecs = UnitSpecsPtr;
	return(true);
}


/**********************************************************************************************
    BOOL AddUnit(STRUCT_NODE* pNode,BYTE bUnitAddress, int * piUnitNR, STRUCT_UNIT **ReturnUnitPtr)
    Description : Add Unit element to powermate database's NODE. 
    Input       : *pNode : Reference to STRUCT_NODE
                  bUnitAddress :  Address of Powermate Unit 
    Output      : *piUnitNR  : Reference to integer to return ID.
                  ** ReturnUnitPtr : Reference to STRUCT_UNIT pointer to return pointer to newly created UNIT element.
    Return      : true or false 
**********************************************************************************************/

BOOL CNodeDatabase::AddUnit(/*STRUCT_NODE* */ CCanNode *pNode,BYTE bUnitAddress, int * piUnitNR, STRUCT_UNIT **ReturnUnitPtr)
{
	STRUCT_UNIT* UnitSpecsPtr = 0;
	STRUCT_UNIT* PreviousUnitSpecsPtr = 0;		// CHANGES_MVS_2008_WARNINGS
	int i;

	/* Check if valid bAddress ?*/
	if ((bUnitAddress<0xff) && (bUnitAddress >= 0x00))
	{
		/* Check if node address already exist ?*/

		UnitSpecsPtr = pNode->pFirstUnit; 

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
	            PMError("No memory|left for|allocation,|system halted.",PM_FATAL_ERROR);
				return(false);
			}					

			// TM,V16.43: Alles op 0 zetten.
			memset(UnitSpecsPtr,0,sizeof(STRUCT_UNIT));

			UnitSpecsPtr->bUnitAddress = bUnitAddress;

			/* KV_NEW 28121999 */
			// UnitSpecsPtr->pvData=NULL;
			// TM New 11052004
			// UnitSpecsPtr->pBindingUnit = NULL;
			// UnitSpecsPtr->pNextUnit = NULL;
			// UnitSpecsPtr->ulApplicationFlags = 0;

			/* Increment iNROfNodes member variable.*/
			pNode->iNROfUnits++; 
			/* KV to track bNROfUnits if created locally ! */
			if (pNode->bNROfUnits < pNode->iNROfUnits) 
				pNode->bNROfUnits = pNode->iNROfUnits;
         
			*ReturnUnitPtr = UnitSpecsPtr;
			*piUnitNR = i;

			if (i==0) /* If first node in list...*/
			{
	            /* OK, we didn't find any duplicate and so we have also the new entry NR...*/
		        /* Create new node object and adjust pointers...*/
				pNode->pFirstUnit = UnitSpecsPtr;
			}
			else /* If following node in list...*/
			{  
				ASSERT(PreviousUnitSpecsPtr != 0);	// CHANGES_MVS_2008_WARNINGS

	            PreviousUnitSpecsPtr->pNextUnit = UnitSpecsPtr;
			}

	        return(true);
		}
		else return(false);
	}
	else
		return(false);
}


/**********************************************************************************************
    BOOL NROfUnits(STRUCT_NODE *pNode,int * piNROfUnits)
    Description : Find Unit element in powermate database's NODE. 
    Input       : *pNode : Reference to STRUCT_NODE
    Output      : *piNROfUnits  : Reference to integer to return NR of elements.
    Return      : true or false 
**********************************************************************************************/

BOOL CNodeDatabase::NROfUnits(/*STRUCT_NODE*/ CCanNode *pNode,int *piNROfUnits)
{
	*piNROfUnits = pNode->iNROfUnits;
	return(true);

}

/**********************************************************************************************
    BOOL FindUnit(STRUCT_NODE *pNode,BYTE bUnitAddress, int * piUnitNR, STRUCT_UNIT **ReturnUnitPtr)
    Description : Find Unit element in powermate database's NODE. 
    Input       : *pNode : Reference to STRUCT_NODE
                  bUnitAddress :  Address of Powermate Unit 
    Output      : *piUnitNR  : Reference to integer to return ID.
                  ** ReturnUnitPtr : Reference to STRUCT_UNIT pointer to return pointer to found UNIT element.
    Return      : true or false 
**********************************************************************************************/

BOOL CNodeDatabase::FindUnit(/*STRUCT_NODE*/ CCanNode *pNode,BYTE bUnitAddress,int *piUnitNR, STRUCT_UNIT **ReturnUnitPtr)
{
	STRUCT_UNIT *UnitSpecsPtr;
	int i;

	if (pNode->iNROfUnits == 0) return(false);   
 
	UnitSpecsPtr = pNode->pFirstUnit; 

	for (i=0;i<pNode->iNROfUnits;i++)
	{
		if (UnitSpecsPtr->bUnitAddress == bUnitAddress) break;
		UnitSpecsPtr = UnitSpecsPtr->pNextUnit;
	}

	if (i!=pNode->iNROfUnits)
	{
		*piUnitNR = i;
		*ReturnUnitPtr = UnitSpecsPtr;
		return(true);
	}
	else return(false);
}

/**********************************************************************************************
    BOOL DeleteUnitNR(STRUCT_NODE *pNode,int iUnitNR)
    Description : Delete Unit element from powermate database's NODE. 
    Input       : *pNode : Reference to STRUCT_NODE
                  iUnitNR :  ID of Powermate Unit in list.
                             TIP : use delete ID 0 until false returns to delete all elements. 
    Output      : 
    Return      : true or false 
**********************************************************************************************/

BOOL CNodeDatabase::DeleteUnitNR(/*STRUCT_NODE*/ CCanNode *pNode,int iUnitNR)
{
	/* Check if UnitSpecs address already exist ?*/
	STRUCT_UNIT* UnitSpecsPtr = 0;				// CHANGES_MVS_2008_WARNINGS
	STRUCT_UNIT* PreviousUnitSpecsPtr = 0;		// CHANGES_MVS_2008_WARNINGS
	BYTE i;

	if (pNode->iNROfUnits == 0) return(false);
   
	UnitSpecsPtr = pNode->pFirstUnit; 

	if (iUnitNR<pNode->iNROfUnits)
	{
		if (iUnitNR==0) /* If first UnitSpecs in list...*/
		{
			pNode->pFirstUnit = UnitSpecsPtr->pNextUnit;
		}
		else /* If following UnitSpecs in list...*/
		{  
			for (i=0;i<iUnitNR;i++)
			{
	            PreviousUnitSpecsPtr = UnitSpecsPtr;
		        UnitSpecsPtr = UnitSpecsPtr->pNextUnit;
			}
			ASSERT(PreviousUnitSpecsPtr != 0);	// CHANGES_MVS_2008_WARNINGS
	        PreviousUnitSpecsPtr->pNextUnit = UnitSpecsPtr->pNextUnit;
		}
 
		if (NULL != UnitSpecsPtr->m_pAppUnitData) {
			delete UnitSpecsPtr->m_pAppUnitData;
			UnitSpecsPtr->m_pAppUnitData = NULL;
		}
		if (UnitSpecsPtr->m_pBindingUnit != NULL) {
			delete UnitSpecsPtr->m_pBindingUnit;
			UnitSpecsPtr->m_pBindingUnit = NULL;
		}

		free(UnitSpecsPtr);

		/* Decrement iNROfUnits member variable.*/
		pNode->iNROfUnits--; 
		/* KV to track bNROfUnits if created locally ! */
		if (pNode->bNROfUnits > pNode->iNROfUnits) 
			pNode->bNROfUnits = pNode->iNROfUnits;

		return(true);
	}
	else return(false);
}

/////////////////////////////////////////////////////////////////////////////////////
BYTE CNodeDatabase::CopyNodeObject(CCanNode *pDestNode, CCanNode *pSourceNode)
{
	if (NULL == pDestNode) {
		REQUIRE(pDestNode != NULL);
		return FALSE;
	}
	if (NULL == pSourceNode) {
		REQUIRE(pSourceNode != NULL);
		return FALSE;
	}
	
	pDestNode->bNodeType = pSourceNode->bNodeType;
	pDestNode->bNROfUnits = pSourceNode->bNROfUnits; 
	pDestNode->bNodeFlags = pSourceNode->bNodeFlags;
	pDestNode->bDLLAccessControl = pSourceNode->bDLLAccessControl;
	pDestNode->ulPhysicalAddress = pSourceNode->ulPhysicalAddress;
	pDestNode->bLogicalAddress = pSourceNode->bLogicalAddress;							
	
	//NEW  BindingFile Info ...
	pDestNode->SetBindingFilePtr(pSourceNode->GetBindingFilePtr());	
	pDestNode->setNodeBindingsNrEntries(pSourceNode->getNodeBindingsNrEntries());
	pDestNode->setNodeBindingsXORChecksum(pSourceNode->getNodeBindingsXORChecksum());

	strcpy(pDestNode->szNodeName,pSourceNode->szNodeName);
		
	memcpy(pDestNode->pbResourcesBlock0,
		   pSourceNode->pbResourcesBlock0,
		   RESOURCES_BLOCK0_LENGTH);

	memcpy(pDestNode->pbResourcesBlock1,
		   pSourceNode->pbResourcesBlock1,
		   RESOURCES_BLOCK1_LENGTH);

	memcpy(pDestNode->pbResourcesBlock2,
		   pSourceNode->pbResourcesBlock2,
		   RESOURCES_BLOCK2_LENGTH);

	// Sedert versie 0x0700.
	memcpy(pDestNode->pbResourcesBlock3,
		   pSourceNode->pbResourcesBlock3,
		   RESOURCES_BLOCK3_LENGTH);

	memcpy(pDestNode->pbResourcesBlock4,
		   pSourceNode->pbResourcesBlock4,
		   RESOURCES_BLOCK4_LENGTH);

	pDestNode->ulApplicationFlags = pSourceNode->ulApplicationFlags;

	/* TM 20060908 */
	pDestNode->AllocateRuntimeStatus();
	pDestNode->CopyRuntimeStatus(pSourceNode);

	return TRUE;
}

BYTE CNodeDatabase::CopyUnitObject(STRUCT_UNIT *pDestUnit, STRUCT_UNIT *pSourceUnit) {
	if (NULL == pDestUnit) {
		ASSERT(pDestUnit != NULL);
		return FALSE;
	}
	if (NULL == pSourceUnit) {
		ASSERT(pSourceUnit != NULL);
		return FALSE;
	}	
	pDestUnit->bNodeLogicalAddress = pSourceUnit->bNodeLogicalAddress;
	pDestUnit->bUnitAddress = pSourceUnit->bUnitAddress;
	pDestUnit->bUnitFlags =	pSourceUnit->bUnitFlags;
	pDestUnit->bUnitType = pSourceUnit->bUnitType;
	strcpy(pDestUnit->szUnitName,pSourceUnit->szUnitName);
	memcpy(&pDestUnit->UnitSpecs,&pSourceUnit->UnitSpecs, sizeof(UNION_UNITSPECS));			
	pDestUnit->UnitStatus = pSourceUnit->UnitStatus;	
	pDestUnit->m_pBindingUnit = pSourceUnit->m_pBindingUnit;				// Copy van de pointer (geen nieuwe geheugen allocatie.)
	pDestUnit->m_pAppUnitData = pSourceUnit->m_pAppUnitData;				// Copy van de pointer (geen nieuwe geheugen allocatie.)
	pDestUnit->ulApplicationFlags = pSourceUnit->ulApplicationFlags;
	return TRUE;
}

BYTE CNodeDatabase::CopyDatabase(CNodeDatabase *pDest, CNodeDatabase *pSource)
{
	if (NULL == pDest) {
		ASSERT(pDest != NULL);	
		return FALSE;
	}
	if (NULL == pSource) {
		ASSERT(pSource != NULL);
		return FALSE;
	}

	int iNrOfNodes,iNrOfUnits,iEntryNr;
	int i,j;
	
	pDest->InitDatabase();

	if (!(pSource->NROfNodes(&iNrOfNodes)))
		AfxMessageBox("Error - Database Error",MB_ICONSTOP);

	if (iNrOfNodes == NULL) 
		return NULL;

	for (i=0;i<iNrOfNodes;i++) {
		CCanNode* pSourceNode = NULL;
		CCanNode* pDestNode = NULL;

		pSource->GetNode(i,&pSourceNode);
		ASSERT(NULL != pSourceNode);

		pDest->AddNode(pSourceNode->bLogicalAddress, &iEntryNr, &pDestNode);
		ASSERT(NULL != pDestNode);

		// Copy Node Object
		CopyNodeObject(pDestNode,pSourceNode);		

		// Alle units copieren.
		iNrOfUnits = pSourceNode->iNROfUnits;		
		for (j=0;j<iNrOfUnits;j++) {
			STRUCT_UNIT* pSourceUnit = NULL;
			STRUCT_UNIT* pDestUnit = NULL;

			pSource->GetUnit(pSourceNode,j,&pSourceUnit);			
			ASSERT(NULL != pSourceUnit);

			pDest->AddUnit(pDestNode,j,&iEntryNr,&pDestUnit);
			ASSERT(NULL != pDestUnit);

			// Copy Unit Object
			INVARIANT((NULL != pSourceUnit) && (NULL != pDestUnit));
			CNodeDatabase::CopyUnitObject(pDestUnit,pSourceUnit);
		}
	}

	return (iNrOfNodes);
}
//////////////////////////////////////////////////////////////////////////
BOOL CNodeDatabase::SetUnitName(BYTE bNodeAddress,BYTE bUnitAddress,CString szName)
{
	CCanNode *pNode;
	STRUCT_UNIT *pUnit;
	int iEntryNr;

	if (!FindNode(bNodeAddress,&iEntryNr,&pNode))
		return FALSE;

	ASSERT(pNode != NULL);

	if (!FindUnit(pNode,bUnitAddress,&iEntryNr,&pUnit))
		return FALSE;

	if (szName.GetLength() >= UNIT_NAME_LENGTH)
		szName.SetAt(UNIT_NAME_LENGTH-1,'\0');

	if (!(pUnit->ulApplicationFlags & DEFAULT_NAME_OVERRIDDEN))
	{
		strcpy(pUnit->szUnitName,(LPCTSTR) szName);		// CHANGES_MVS_2008_CSTRING
	}

	return TRUE;
}

// TODO SET FLAG !!!!!!!!!!!
BOOL CNodeDatabase::ChangeUnitName(BYTE bNodeAddress,BYTE bUnitAddress,CString szName)
{
	CCanNode *pNode;
	STRUCT_UNIT *pUnit;
	int iEntryNr;

	if (!FindNode(bNodeAddress,&iEntryNr,&pNode))
		return FALSE;

	ASSERT(pNode != NULL);

	if (szName.GetLength() >= UNIT_NAME_LENGTH)
		szName.SetAt(UNIT_NAME_LENGTH-1,'\0');

	if (!FindUnit(pNode,bUnitAddress,&iEntryNr,&pUnit))
		return FALSE;

	strcpy(pUnit->szUnitName,(LPCTSTR) szName);	// CHANGES_MVS_2008_CSTRING
	pUnit->ulApplicationFlags |= DEFAULT_NAME_OVERRIDDEN;

	return TRUE;
}

CString CNodeDatabase::GetUnitName(BYTE bNodeAddress,BYTE bUnitAddress)
{
	CCanNode *pNode = NULL;
	STRUCT_UNIT *pUnit = NULL;
	int iEntryNr = 0;
	CString szName;

	if (!FindNode(bNodeAddress,&iEntryNr,&pNode))
		return szName;

	ASSERT(pNode != NULL);

	if (!FindUnit(pNode,bUnitAddress,&iEntryNr,&pUnit))
		return szName;
	
	szName.Format("%s",pUnit->szUnitName);
	
	return szName;
}

BOOL CNodeDatabase::ChangeNodeName(BYTE bNodeAddress,CString szName)
{
	CCanNode *pNode;
	int iEntryNr;

	if (!FindNode(bNodeAddress,&iEntryNr,&pNode))
		return FALSE;

	if (szName.GetLength() >= NODE_NAME_LENGTH)
		szName.SetAt(NODE_NAME_LENGTH-1,'\0');

	ASSERT(pNode != NULL);

	strcpy(pNode->szNodeName,(LPCTSTR) szName);	// CHANGES_MVS_2008_CSTRING
	return TRUE;
}

CString CNodeDatabase::GetNodeName(BYTE bNodeAddress)
{
	CCanNode *pNode;
	int iEntryNr;
	CString szName;

	if (!FindNode(bNodeAddress,&iEntryNr,&pNode))
		return szName;;

	ASSERT(pNode != NULL);

	szName.Format("%s",pNode->szNodeName);

	return szName;
}


BOOL CNodeDatabase::InitRuntimeUnitSpecs(STRUCT_UNIT *pUnit)
{
	switch(pUnit->bUnitType)
	{
		case UNIT_TYPE_CONTROL:
		case UNIT_TYPE_LCD_VIRTUAL:
		{
			pUnit->UnitSpecs.ControlUnit.bControlStatus1 &= (UNIT_CONTROLPULSTOGGLE_HIGH^0xff);
		} break;

		case UNIT_TYPE_SWITCH:
		{
			pUnit->UnitSpecs.RelayUnit.bSwitchStatus1=SWITCH_UNIT_OFF_STATE;
		} break;

		case UNIT_TYPE_DIM:
		{
			pUnit->UnitSpecs.DimmerUnit.bDimStatus1=DIM_UNIT_OFF_STATE;
		} break;

		case UNIT_TYPE_DUOSWITCH:
		{
			pUnit->UnitSpecs.DuoSwitchUnit.bDuoSwitchStatus1=DUOSWITCH_UNIT_STOP_STATE;
		} break;

		case UNIT_TYPE_SENS:
		{
			pUnit->UnitSpecs.SensorUnit.bSensStatus1=STATE_SENS_STABLE;
			pUnit->UnitSpecs.SensorUnit.bSensStatus2 &= (SENS_CONTROL_ON^0xff);		
			pUnit->UnitSpecs.SensorUnit.sMeasuredValueOffset = SENS_TEMP_OFFSETVALUE_UNSUPPORTED;	// TM20120404.
			pUnit->UnitSpecs.SensorUnit.usNTCVersionInfo = SENS_TEMP_NTCVERSION_UNSUPPORTED;		// TM20120404.
		} break;

		case UNIT_TYPE_AUDIO:
		case UNIT_TYPE_BOSERS:
		case UNIT_TYPE_AUDIO_HC4:
		{
			pUnit->UnitSpecs.AudioUnit.bAudioStatus1 = 0x00; 
			pUnit->UnitSpecs.AudioUnit.bAudioStatus2 = 0x00;
		} break;

		case UNIT_TYPE_ALARM:	// TM_CHANGES_ALARM_ARITECH:
		{
			// Oppassen: De bConfig wordt niet aangepast.
			pUnit->UnitSpecs.AlarmUnit.bAlarmStatus1 = 0x00;
			pUnit->UnitSpecs.AlarmUnit.bAlarmStatus2 = 0x00;
		} break;
	}
	return TRUE;
}

