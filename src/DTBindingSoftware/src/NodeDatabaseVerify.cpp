// NodeDatabaseVerify.cpp: implementation of the CNodeDatabaseVerify class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "NodeDatabaseVerify.h"


// -----------------------------------
#include "TMGlobals.h"
using namespace TMNodeDatabase;
// ------------------------------------

//#include "nodeDatabase.h"



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNodeDatabaseVerify::CNodeDatabaseVerify(CNodeDatabaseVerify_Handler* pHandler) : 
m_pHandler(pHandler)
{

}

CNodeDatabaseVerify::~CNodeDatabaseVerify()
{

}

int CNodeDatabaseVerify::Check_Incompatible_Nodes( )
{
	int iCount = 0;
	int nDocCount;

	if (TMGetDocumentNodes()->NROfNodes(&nDocCount))
	{
		BOOL fDifferent;	
		CCanNode *pDocNode,*pRTNode;

		for (int i=0;i<nDocCount;i++) 
		{						
			// Get Document node => pDocNode
			if (TMGetDocumentNodes()->GetNode(i, &pDocNode)) 
			{				
				STRUCT_UNIT *pDocUnit,*pRTUnit;	
				int j;

				// Find Runtime node => pRTNode
				if (TMGetRuntimeNodes()->FindNode(pDocNode->bLogicalAddress, &j, &pRTNode)) 
				{				
					fDifferent = FALSE;

					// Different node Types --> fDifferent is TRUE
					if (pDocNode->bNodeType != pRTNode->bNodeType) 
					{	
						fDifferent = TRUE;		
					}
					
					if (fDifferent == FALSE)
					{					
						// If document node has more units then Runtime Node
						if (pDocNode->bNROfUnits > pRTNode->bNROfUnits) 
						{						
							// Get last unit of dument node ...
							if (TMGetDocumentNodes()->GetUnit(pDocNode, pDocNode->bNROfUnits-1, &pDocUnit))
							{
								//if lastunit of document node == virtual unit and Doc node has
								//only 1 unit more then Runtime unit --> They are not different.						
								if ((pDocNode->bNROfUnits == (pRTNode->bNROfUnits + 1)) && 
									(pDocUnit->bUnitAddress == 0xFF)) 
								{				
									// ABSORBE VIRTUAL UNITS ...
								}
								else 
								{
									fDifferent = TRUE;
								}
								
							} // if (>GetUnit(pDocNode,
						} // if (pDocNode->bNROfUnits > pRTNode->bNROfUnits) 
					}// if fDifferent == FALSE)

				
					if (fDifferent == FALSE)
					{	
						/**
						 * Doc Node en Runtime node hebben zelfde aantal units ....
						 */
							
						if (pDocNode->bNROfUnits == pRTNode->bNROfUnits) 
						{							
							/**
							 * Alle units van Doc en RT node overlopen en controleren op Unit type ...
							 */

							for (j=0;j<pDocNode->bNROfUnits;j++) 
							{									
								if (TMGetDocumentNodes()->GetUnit(pDocNode, j, &pDocUnit)) 
								{
									if (TMGetRuntimeNodes()->GetUnit(pRTNode, j, &pRTUnit)) 
									{										
										if (pDocUnit->bUnitType != pRTUnit->bUnitType) 
										{
											fDifferent = TRUE;						
										}										
									} // if (->GetUnit										
									
									ASSERT(pRTUnit);										
									
								}// if (->GetUnit
								
								ASSERT(pDocUnit);
							
							} // end for (j=0;j<pDocNode->bNROfUnits;j++) 
						
						} //if (pDocNode->bNROfUnits == pRTNode->bNROfUnits) 
						else 
						{	
							/**
							 * aantal units in doc node kleiner dan in network node (of virtuele units) ...
							 * controle op unittype van unit 0 
							 */
								
							if (TMGetDocumentNodes()->GetUnit(pDocNode, 0, &pDocUnit)) 
							{
								if (TMGetRuntimeNodes()->GetUnit(pRTNode, 0, &pRTUnit)) 
								{							
									if (pDocUnit->bUnitType != pRTUnit->bUnitType) 
									{
										fDifferent = TRUE;
									}
								}// if (->GetUnit
								
								ASSERT(pRTUnit);
							
							} // if (->GetUnit
							ASSERT(pDocUnit);
						
						}//else if (pDocNode->bNROfUnits == pRTNode->bNROfUnits) 
					
					} // if (fDifferent == FALSE)

						
					if (fDifferent == FALSE)
					{
						/**
						 * Control op alle units ...
						 */
						for (j=0;j<pDocNode->bNROfUnits;j++)
						{
							if (TMGetDocumentNodes()->GetUnit(pDocNode, j, &pDocUnit)) 
							{
								if (TMGetRuntimeNodes()->GetUnit(pRTNode, j, &pRTUnit)) 
								{							
									if (pDocUnit->bUnitType != pRTUnit->bUnitType) 
									{
										fDifferent = TRUE;
									}
								} // if (->GetUnit									
								ASSERT(pRTUnit);
							} // if (->GetUnit(pDocNode,	
							ASSERT(pDocUnit);							
						
						}// for (j=0;j<pDocNode->bNROfUnits;j++)
					}// end if fDifferent == FALSE


					if (fDifferent == TRUE) 
					{
						ASSERT(pRTNode);
						ASSERT(pDocNode);
						ASSERT(pRTNode->bLogicalAddress == pDocNode->bLogicalAddress);

						if (m_pHandler != 0)
						{
							m_pHandler->On_Incompatible_Nodes(iCount,pDocNode,pRTNode);
						}

						iCount++;						
					}

				} // //if (->FindNode

			} //if (->FindNode

		}//for (int i=0;i<nDocCount;i++)			

	}// if (->NROfNodes(&nDocCount))
	return iCount;	
}


int CNodeDatabaseVerify::Check_Removed_Nodes( )
{
	int nDocCount = 0;
	int iCount = 0;
					
	if (TMGetDocumentNodes()->NROfNodes(&nDocCount))
	{			
		for (int i=0; i<nDocCount; i++) 
		{
			CCanNode *pDocNode = 0;

			if (TMGetDocumentNodes()->GetNode(i, &pDocNode))
			{																		
				int k;				
				CCanNode *pRTNode = 0;

				if (TMGetRuntimeNodes()->FindNode(pDocNode->bLogicalAddress, 
												  &k, 
												  &pRTNode) == FALSE) 
				{				
					/* No RTNode Found for Logical address of DocNode */

					if (m_pHandler != 0)
					{
						m_pHandler->On_Removed_Nodes(iCount,pDocNode);												
						iCount++;
					}

				}//if (->FindNode

			} // if (->GetNode			

		} // for (int i=0;i<nDocCount;i++) 
	}	
	return iCount;
}



int CNodeDatabaseVerify::Check_Added_Nodes( )
{
	int iCount = 0;
	int nRTCount;

	if (TMGetRuntimeNodes()->NROfNodes(&nRTCount))
	{			
		for (int i=0;i<nRTCount;i++) 
		{			
			CCanNode *pRTNode;

			if (TMGetRuntimeNodes()->GetNode(i, &pRTNode))
			{
				CCanNode *pDocNode;
				int k;

				if (TMGetDocumentNodes()->FindNode(pRTNode->bLogicalAddress, &k, &pDocNode) 
					== FALSE) 
				{					
					if (m_pHandler != 0)
					{
						m_pHandler->On_Added_Nodes(iCount,pRTNode);												
						iCount++;
					}

				}// if (->FindNode		
				
			}// if (->GetNode		

		}// for (int i=0;i<nRTCount;i++)
	}
	return iCount;
}






//////////////////////////////////////////////////////////////////////
void CNodeDatabaseVerify_Handler::On_Incompatible_Nodes(int index,
														const CCanNode* pDocNode,
														const CCanNode* pRTNode)
{
	// Basis klasse : 
	// implementeren door afgeleidde klasse indien gewenst ...

}

//////////////////////////////////////////////////////////////////////
/*
void CNodeDatabaseVerify_Handler::On_Found_Nodes(const CCanNode* pDocNode,
												 const CCanNode* pRTNode)
{
	// Basis klasse : 
	// implementeren door afgeleidde klasse indien gewenst ...

}
*/

void CNodeDatabaseVerify_Handler::On_Removed_Nodes(int index, 
												   const CCanNode* pDocNode)
{
	// Basis klasse : 
	// implementeren door afgeleidde klasse indien gewenst ...
}

//////////////////////////////////////////////////////////////////////

void CNodeDatabaseVerify_Handler::On_Added_Nodes(int index, const CCanNode* pRTNode)
{
	// Basis klasse : 
	// implementeren door afgeleidde klasse indien gewenst ...
}

//////////////////////////////////////////////////////////////////////
