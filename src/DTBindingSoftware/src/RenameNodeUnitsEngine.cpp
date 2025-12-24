// RenameNodeUnitsEngine.cpp: implementation of the CRenameNodeUnitsEngine class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "RenameNodeUnitsEngine.h"

#include "NodeDatabase.h"
#include "ComApplication.h"
#include "TXNodeMessage.h"
#include "TXUnitMessage.h"

#include "utils/app/DisplayDevice.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CRenameNodeUnitsEngine::CRenameNodeUnitsEngine(IComApplication *pCom,CNodeDatabase *pDocDatabase) :
m_pComApplication( pCom ),
m_pDatabase( pDocDatabase )
{
	ASSERT(pCom);
	ASSERT(pDocDatabase);

	m_CurrentState = STATE_IDLE;
	m_CurrentMode = MODE_NONE;	
}

CRenameNodeUnitsEngine::~CRenameNodeUnitsEngine()
{
}

///////////////////////////////////////////////////////////////////////

inline BYTE CRenameNodeUnitsEngine::GetLengthToWrite(int length,int pos)
{
	BYTE bLengthToWrite;	
	
	if ((bLengthToWrite=(length-pos)) >= 5) 
		bLengthToWrite = 5;		

	return bLengthToWrite;
}

///////////////////////////////////////////////////////////////////////

void CRenameNodeUnitsEngine::FSMRenameUnit(Events bEvent)
{
	switch(m_CurrentState)
	{
		case STATE_IDLE:		
		{
			switch (bEvent)
			{				
				case EV_RENAME_START:
				{			
					m_ErrorCnt = 0;			
					
					if (m_bUnitAddress == BROADCAST_UNIT) 
					{
						m_CurrentState = STATE_IDLE;								
						
						CString szMessage("Can't rename Virtual unit Address 0xFF ...");
						CDisplayDevice::DisplayErrorMsg(szMessage);		
						
						m_pComApplication->StopTimer(TIMERID_RENAME_TIMEOUT);						

						Fire_OnFsmEnded();
					}
					else 
					{
						m_szName = m_pDatabase->GetUnitName(m_bNodeAddress,m_bUnitAddress);	
						m_bLength = FormatData(m_szName,m_pbData);
					
						if (m_bLength != 0) 
						{
							m_bPos = 0; 													
							m_bPos = WriteUnitName(m_bNodeAddress,
												   m_bUnitAddress,
												   &m_pbData[0],
												   m_bPos,
												   GetLengthToWrite(m_bLength,m_bPos));		
								
							m_CurrentState = STATE_UNITNAME_WRITE;
						
						}		
						else 
						{
							m_CurrentState = STATE_IDLE;		

							CString szMessage("Unit name Length = 0");							
							CDisplayDevice::DisplayErrorMsg(szMessage);

							m_pComApplication->StopTimer(TIMERID_RENAME_TIMEOUT);

							Fire_OnFsmEnded();
						}
					}
				}break;
			}// end switch bEvent
		}break; // case State Idle

		case STATE_UNITNAME_WRITE:
		{
			switch (bEvent)
			{
				case EV_RENAME_UNITNAME_ERROR:			
				{
					CString szMessage("EV_RENAME_UNITNAME_ERROR RECEIVED");
					CDisplayDevice::DisplayErrorMsg(szMessage);				
				
				} // Don't break

				case EV_RENAME_TIMEOUT:			
				{				
					m_ErrorCnt++;								
					m_CurrentState = STATE_IDLE;		
					DisplayMessage(m_bNodeAddress,m_bUnitAddress,FALSE);									

					m_pComApplication->StopTimer(TIMERID_RENAME_TIMEOUT);

					Fire_OnFsmEnded();

				}break;
							
				case EV_RENAME_CANCEL:
				{
					m_CurrentState = STATE_IDLE;
					
					CString szMessage("Cancel by User");
					CDisplayDevice::DisplayInfoMsg(szMessage);

					m_pComApplication->StopTimer(TIMERID_RENAME_TIMEOUT);
					Fire_OnFsmEnded();
					
				}break;
			
				case EV_RENAME_UNITNAME_ACK:
				{
					if (m_bPos<m_bLength) 
					{							
						m_bPos = WriteUnitName(m_bNodeAddress,
											 m_bUnitAddress,
											 &m_pbData[0],
											 m_bPos,
											 GetLengthToWrite(m_bLength,m_bPos));

						m_CurrentState = STATE_UNITNAME_WRITE;

					}// end if	
					else 
					{
						m_CurrentState = STATE_IDLE;	
						DisplayMessage(m_bNodeAddress,m_bUnitAddress,TRUE);

						m_pComApplication->StopTimer(TIMERID_RENAME_TIMEOUT);
						Fire_OnFsmEnded();
					}
				}break;
			}// end switch bEvent
		}// end 
	} // end switch (m_CurrentState)
}


void CRenameNodeUnitsEngine::FSMRenameNode(Events bEvent)
{	
	switch(m_CurrentState)
	{
		case STATE_IDLE:		
		{
			switch (bEvent)
			{				
				case EV_RENAME_START:
				{			
					m_ErrorCnt = 0;					
					m_bUnitAddress = 0x00; 
					m_pDatabase->NROfNodes(&m_NrOfNodes);
					m_NodeIndex = 0;
					
					int iEntryNr;
					if (!m_pDatabase->FindNode(m_bNodeAddress,&iEntryNr,&m_pNode)) 
					{
						m_CurrentState = STATE_IDLE;
						
						CString szMessage("Canceled by Fatal Error : No Nodes Found");
						CDisplayDevice::DisplayErrorMsg(szMessage);

						m_pComApplication->StopTimer(TIMERID_RENAME_TIMEOUT);
						Fire_OnFsmEnded();
					}
					else
					{														
						m_szName = m_pNode->szNodeName;	
						m_bLength = FormatData(m_szName,m_pbData);						
						
						if (m_bLength != 0) 
						{
							m_bPos = 0; 		
						
							m_bPos = WriteNodeName(m_pNode->bLogicalAddress,
												 &m_pbData[0],
												 m_bPos,
												 GetLengthToWrite(m_bLength,m_bPos));						

							m_CurrentState = STATE_NODENAME_WRITE;
							
						}// end if bLength
					}
				}break;
			}
		}break;

		case STATE_NODENAME_WRITE:
		{
			switch(bEvent)
			{
				case EV_RENAME_NODENAME_ERROR:		
				{

					CString szMessage("EV_RENAME_NODENAME_ERROR RECEIVED");
					CDisplayDevice::DisplayErrorMsg(szMessage);							
				
				} // Don't break ...

				case EV_RENAME_TIMEOUT:									
				{					
					m_ErrorCnt++;									
					m_CurrentState = STATE_IDLE;
					DisplayMessage(m_bNodeAddress,FALSE);

					m_pComApplication->StopTimer(TIMERID_RENAME_TIMEOUT);

					Fire_OnFsmEnded();
				}break;
																		
				case EV_RENAME_CANCEL:
				{					
					m_CurrentState = STATE_IDLE;																									

					CString szMessage("Cancel by User");
					CDisplayDevice::DisplayInfoMsg(szMessage);
					
					m_pComApplication->StopTimer(TIMERID_RENAME_TIMEOUT);

					Fire_OnFsmEnded();
				}break;
			
				case EV_RENAME_NODENAME_ACK:
				{
					if (m_bPos<m_bLength) 
					{	
						m_bPos = WriteNodeName(m_pNode->bLogicalAddress,
											 &m_pbData[0],
											 m_bPos,
											 GetLengthToWrite(m_bLength,m_bPos));

						m_CurrentState = STATE_NODENAME_WRITE;
					}// end if (m_bPos < bLength)
					else 
					{																		  																								
						m_CurrentState = STATE_IDLE;	
						DisplayMessage(m_pNode->bLogicalAddress,TRUE);
						m_pComApplication->StopTimer(TIMERID_RENAME_TIMEOUT);
						Fire_OnFsmEnded();
					}					
				}break;
			}// end switch bEvent
		}break;
	}// end switch
}


void CRenameNodeUnitsEngine::FSMRenameNodeUnits(Events bEvent)
{	
	switch(m_CurrentState)
	{
		case STATE_IDLE:		
		{			
			switch (bEvent)
			{
				case EV_RENAME_CANCEL:
				{				
					m_CurrentState = STATE_IDLE;

					CString szMessage("Cancel by User");
					CDisplayDevice::DisplayInfoMsg(szMessage);

					m_pComApplication->StopTimer(TIMERID_RENAME_TIMEOUT);
					
					Fire_OnFsmEnded();

				}break;

				case EV_RENAME_START:
				{				
					m_ErrorCnt = 0;					
					m_bUnitAddress = 0x00; 
					m_pDatabase->NROfNodes(&m_NrOfNodes);
					m_NodeIndex = 0;
					
					if (m_pDatabase->GetNode(m_NodeIndex,&m_pNode) == FALSE) 
					{
						m_CurrentState = STATE_IDLE;					

						CString szMessage("Canceled by Fatal Error : No Nodes Found");
						CDisplayDevice::DisplayErrorMsg(szMessage);

						m_pComApplication->StopTimer(TIMERID_RENAME_TIMEOUT);

						Fire_OnFsmEnded();
					}
					else
					{														
						m_szName = m_pDatabase->GetNodeName(m_pNode->bLogicalAddress);
						m_bLength = FormatData(m_szName,m_pbData);						
					
						if (m_bLength != 0) 
						{
							m_bPos = 0; 								
							m_bPos = WriteNodeName(m_pNode->bLogicalAddress,
											      &m_pbData[0],
												  m_bPos,
												  GetLengthToWrite(m_bLength,m_bPos));						

							m_CurrentState = STATE_NODENAME_WRITE;
							
						}// end if bLength
					}
				}break;
			}// end switch
		}break;

		case STATE_NODENAME_WRITE:
		{
			switch(bEvent)
			{
				case EV_RENAME_NODENAME_ERROR:									
				{
					CString szMessage("EV_RENAME_NODENAME_ERROR RECEIVED");
					CDisplayDevice::DisplayErrorMsg(szMessage);
				
				}// Don't break ...

				case EV_RENAME_TIMEOUT:				
				{					
					m_ErrorCnt++;				
					DisplayMessage(m_pNode->bLogicalAddress,FALSE);
					if (++m_NodeIndex >= m_NrOfNodes) 
					{											
						m_CurrentState = STATE_IDLE;

						m_pComApplication->StopTimer(TIMERID_RENAME_TIMEOUT);

						Fire_OnFsmEnded();//DisplayDone();											
					}						
					else 
					{							
						if (!m_pDatabase->GetNode(m_NodeIndex,&m_pNode)) 
						{								
							m_CurrentState = STATE_IDLE;						
							
							CString szMessage("Canceled by Fatal Error : No Nodes Found");
							CDisplayDevice::DisplayErrorMsg(szMessage);

							m_pComApplication->StopTimer(TIMERID_RENAME_TIMEOUT);
							Fire_OnFsmEnded();
						}
						else 
						{
							m_szName = m_pDatabase->GetNodeName(m_pNode->bLogicalAddress);
							m_bLength = FormatData(m_szName,m_pbData);

							if (m_bLength != 0) 
							{
								m_bPos = 0; 									
								m_bPos = WriteNodeName(m_pNode->bLogicalAddress,&m_pbData[0],
													 m_bPos,
													 GetLengthToWrite(m_bLength,m_bPos));						

								m_CurrentState = STATE_NODENAME_WRITE;	
								
							}// end if bLength								
						}
					}																
				}break;
			
				case EV_RENAME_CANCEL:
				{					
					m_CurrentState = STATE_IDLE;			
					
					CString szMessage("Cancel by User");
					CDisplayDevice::DisplayInfoMsg(szMessage);

					m_pComApplication->StopTimer(TIMERID_RENAME_TIMEOUT);
					Fire_OnFsmEnded();	
					
				}break;
			
				case EV_RENAME_NODENAME_ACK:
				{
					if (m_bPos<m_bLength) 
					{								
						m_bPos = WriteNodeName(m_pNode->bLogicalAddress,
											   &m_pbData[0],
											   m_bPos,
											   GetLengthToWrite(m_bLength,m_bPos));

						m_CurrentState = STATE_NODENAME_WRITE;

					}// end if (m_bPos < bLength)
					else 
					{																		  					
						DisplayMessage(m_pNode->bLogicalAddress,TRUE);
						if (m_pNode->bNROfUnits != 0) 
						{
							STRUCT_UNIT *pUnit =0;
							m_bUnitAddress = 0;
							m_pDatabase->GetUnit(m_pNode,m_bUnitAddress,&pUnit);
							
							if (pUnit->bUnitAddress != 0xFF) 
							{
								m_szName = m_pDatabase->GetUnitName(m_pNode->bLogicalAddress,m_bUnitAddress);
								m_bLength = FormatData(m_szName,m_pbData);
								
								if (m_bLength != 0) 
								{
									m_bPos = 0; 																		
									
									m_bPos = WriteUnitName(m_pNode->bLogicalAddress,
														 m_bUnitAddress,
														 &m_pbData[0],
														 m_bPos,
														 GetLengthToWrite(m_bLength,m_bPos));

									m_CurrentState = STATE_UNITNAME_WRITE;								
								}										
							}
							else 
							{		// What todo --> Go To Next Node ??
								if (++m_NodeIndex >= m_NrOfNodes) 
								{							
									m_CurrentState = STATE_IDLE;
									
									CString szMessage;
									szMessage.Format("All Names send : %02d errors occured",m_ErrorCnt);
									CDisplayDevice::DisplayInfoMsg(szMessage);

									m_pComApplication->StopTimer(TIMERID_RENAME_TIMEOUT);
									Fire_OnFsmEnded();
								}						
								else 
								{					
									if (m_pDatabase->GetNode(m_NodeIndex,&m_pNode) == FALSE)
									{			
										m_CurrentState = STATE_IDLE;
										
										CString szMessage("Fatal error - No nodes Found");									

										m_pComApplication->StopTimer(TIMERID_RENAME_TIMEOUT);
										Fire_OnFsmEnded();
									}
									else 
									{
										m_szName = m_pDatabase->GetNodeName(m_pNode->bLogicalAddress);
										m_bLength = FormatData(m_szName,m_pbData);
									
										if (m_bLength != 0) 
										{
											m_bPos = 0; 												
											
											m_bPos = WriteNodeName(m_pNode->bLogicalAddress,
																 &m_pbData[0],
																 m_bPos,
																 GetLengthToWrite(m_bLength,m_bPos));						

											m_CurrentState = STATE_NODENAME_WRITE;
										
										}// end if bLength								
									}
								}
							}
						}
/*
						else 
						{																			
							m_CurrentState = STATE_IDLE;	
							szMessage.Format("All Names send : %02d errors occured",m_ErrorCnt);
							CDisplayDevice::DisplayInfoMsg(szMessage);

							pCommApplication->StopTimer(TIMERID_RENAME_TIMEOUT);

							Fire_OnFsmEnded();
						}
*/
						else
						{
							// What todo --> Go To Next Node ??
							if (++m_NodeIndex >= m_NrOfNodes) 
							{							
								m_CurrentState = STATE_IDLE;
								
								CString szMessage;
								szMessage.Format("All Names send : %02d errors occured",m_ErrorCnt);
								CDisplayDevice::DisplayInfoMsg(szMessage);

								m_pComApplication->StopTimer(TIMERID_RENAME_TIMEOUT);
								
								Fire_OnFsmEnded();
							}						
							else 
							{					
								if (m_pDatabase->GetNode(m_NodeIndex,&m_pNode) == FALSE)
								{			
									m_CurrentState = STATE_IDLE;
									
									CString szMessage("Fatal error - No nodes Found");									

									m_pComApplication->StopTimer(TIMERID_RENAME_TIMEOUT);
									Fire_OnFsmEnded();
								}
								else 
								{
									m_szName = m_pDatabase->GetNodeName(m_pNode->bLogicalAddress);
									m_bLength = FormatData(m_szName,m_pbData);
									
									if (m_bLength != 0) 
									{
										m_bPos = 0; 		
									
										m_bPos = WriteNodeName(m_pNode->bLogicalAddress,
															 &m_pbData[0],
															 m_bPos,
															 GetLengthToWrite(m_bLength,m_bPos));						

										m_CurrentState = STATE_NODENAME_WRITE;
										
									}// end if bLength								
								}
							}
						}
					}					
				}break;
			}
		}break;
			
		case STATE_UNITNAME_WRITE:
		{
			switch (bEvent)
			{			
				case EV_RENAME_UNITNAME_ERROR:		
				{
					CString szMessage("EV_RENAME_UNITNAME_ERROR RECEIVED");
					CDisplayDevice::DisplayErrorMsg(szMessage);

				} // Don't break ...

				case EV_RENAME_TIMEOUT:
				{				
					m_ErrorCnt++;
					DisplayMessage(m_pNode->bLogicalAddress,FALSE);
				
					// DISCARD UNIT - MORE UNITS TO SEND ??
					// else NEXT NODE TO SEND ??
					if (++m_bUnitAddress < m_pNode->bNROfUnits) 
					{
						STRUCT_UNIT *pUnit = 0;
						m_pDatabase->GetUnit(m_pNode,m_bUnitAddress,&pUnit);

						if (pUnit->bUnitAddress != BROADCAST_UNIT) 
						{
							m_szName = m_pDatabase->GetUnitName(m_pNode->bLogicalAddress,m_bUnitAddress);
							m_bLength = FormatData(m_szName,m_pbData);
							if (m_bLength != 0) 
							{
								m_bPos = 0; 							
								
								m_bPos = WriteUnitName(m_pNode->bLogicalAddress,
													 m_bUnitAddress,&m_pbData[0],
													 m_bPos,
													 GetLengthToWrite(m_bLength,m_bPos));		

								m_CurrentState = STATE_UNITNAME_WRITE;								
							}																
						}
						else 
						{
							if (++m_NodeIndex >= m_NrOfNodes) 
							{							
								m_CurrentState = STATE_IDLE;

								CString szMessage;
								szMessage.Format("All Names send : %02d errors occured",m_ErrorCnt);
								CDisplayDevice::DisplayInfoMsg(szMessage);

								m_pComApplication->StopTimer(TIMERID_RENAME_TIMEOUT);
								Fire_OnFsmEnded();
							}						
							else 
							{
								if (!m_pDatabase->GetNode(m_NodeIndex,&m_pNode)) 
								{			
									m_CurrentState = STATE_IDLE;

									CString szMessage("Fatal error - No nodes Found");	
									CDisplayDevice::DisplayErrorMsg(szMessage);

									m_pComApplication->StopTimer(TIMERID_RENAME_TIMEOUT);
									Fire_OnFsmEnded();
								}
								else 
								{
									m_szName = m_pDatabase->GetNodeName(m_pNode->bLogicalAddress);
									m_bLength = FormatData(m_szName,m_pbData);			

									if (m_bLength != 0) 
									{
										m_bPos = 0; 											
										
										m_bPos = WriteNodeName(m_pNode->bLogicalAddress,
																 &m_pbData[0],
																 m_bPos,
																 GetLengthToWrite(m_bLength,m_bPos));						

										m_CurrentState = STATE_NODENAME_WRITE;
										
									}// end if bLength								
								}
							}						
						}
					}
					else
					{
						// NO GOTO NEXT NODE
						if (++m_NodeIndex >= m_NrOfNodes) 
						{							
							m_CurrentState = STATE_IDLE;
							CString szMessage;
							szMessage.Format(_T("All Names send : %02d errors occured"),m_ErrorCnt);
							CDisplayDevice::DisplayInfoMsg(szMessage);

							m_pComApplication->StopTimer(TIMERID_RENAME_TIMEOUT);

							Fire_OnFsmEnded();
						}						
						else 
						{
							if (!m_pDatabase->GetNode(m_NodeIndex,&m_pNode)) 
							{			
								m_CurrentState = STATE_IDLE;

								CString szMessage("Fatal error - No nodes Found");									
								CDisplayDevice::DisplayErrorMsg(szMessage);

								m_pComApplication->StopTimer(TIMERID_RENAME_TIMEOUT);
								Fire_OnFsmEnded();
							}
							else 
							{
								m_szName = m_pDatabase->GetNodeName(m_pNode->bLogicalAddress);
								m_bLength = FormatData(m_szName,m_pbData);			
								
								if (m_bLength != 0) 
								{
									m_bPos = 0; 		
																																												
									m_bPos = WriteNodeName(m_pNode->bLogicalAddress,
														 &m_pbData[0],
														 m_bPos,
														 GetLengthToWrite(m_bLength,m_bPos));						

									m_CurrentState = STATE_NODENAME_WRITE;									

								}// end if bLength								
							}
						}
					}
				}break;
			
				case EV_RENAME_CANCEL:
				{
					m_CurrentState = STATE_IDLE;	
					
					CString szMessage("Cancel by User");
					CDisplayDevice::DisplayInfoMsg(szMessage);

					m_pComApplication->StopTimer(TIMERID_RENAME_TIMEOUT);
					Fire_OnFsmEnded();					

				}break;
			
				case EV_RENAME_UNITNAME_ACK:
				{
					if (m_bPos<m_bLength) 
					{ 

						m_bPos = WriteUnitName(m_pNode->bLogicalAddress,
											 m_bUnitAddress,
											 &m_pbData[0],
											 m_bPos,
											 GetLengthToWrite(m_bLength,m_bPos));

						m_CurrentState = STATE_UNITNAME_WRITE;

					}// end if	
					else
					{						
						DisplayMessage(m_pNode->bLogicalAddress,m_bUnitAddress,TRUE);

						// MORE UNITS TO SEND ??
						if (++m_bUnitAddress < m_pNode->bNROfUnits) 
						{
							STRUCT_UNIT *pUnit = 0;
							m_pDatabase->GetUnit(m_pNode,m_bUnitAddress,&pUnit);
							if (pUnit->bUnitAddress != BROADCAST_UNIT) 
							{								
								m_szName = m_pDatabase->GetUnitName(m_pNode->bLogicalAddress,m_bUnitAddress);
								m_bLength = FormatData(m_szName,m_pbData);
								if (m_bLength != 0) 
								{
									m_bPos = 0; 
																								
									m_bPos = WriteUnitName(m_pNode->bLogicalAddress,
														 m_bUnitAddress,
														 &m_pbData[0],
														 m_bPos,
														 GetLengthToWrite(m_bLength,m_bPos));		

									m_CurrentState = STATE_UNITNAME_WRITE;								
								}																
							}
							else
							{
								if (++m_NodeIndex >= m_NrOfNodes) 
								{							
									m_CurrentState = STATE_IDLE;	

									CString szMessage;
									szMessage.Format(_T("All Names send : %02d errors occured"),m_ErrorCnt);
									CDisplayDevice::DisplayInfoMsg(szMessage);

									m_pComApplication->StopTimer(TIMERID_RENAME_TIMEOUT);
									Fire_OnFsmEnded();
								}						
								else 
								{							
									if (!m_pDatabase->GetNode(m_NodeIndex,&m_pNode)) 
									{	
										m_CurrentState = STATE_IDLE;

										CString szMessage("Canceled by Fatal Error : No Nodes Found");																	
										CDisplayDevice::DisplayErrorMsg(szMessage);

										m_pComApplication->StopTimer(TIMERID_RENAME_TIMEOUT);
										Fire_OnFsmEnded();
									}
									else 
									{
										m_szName = m_pDatabase->GetNodeName(m_pNode->bLogicalAddress);
										m_bLength = FormatData(m_szName,m_pbData);				
										
										if (m_bLength != 0) 
										{
											m_bPos = 0; 		
																																														
											m_bPos = WriteNodeName(m_pNode->bLogicalAddress,
																 &m_pbData[0],
																 m_bPos,
																 GetLengthToWrite(m_bLength,m_bPos));						

											m_CurrentState = STATE_NODENAME_WRITE;											

										}// end if bLength								
									}
								}
							}
						}
						else 
						{
							// NO GOTO NEXT NODE
							if (++m_NodeIndex >= m_NrOfNodes) 
							{							
								m_CurrentState = STATE_IDLE;	
								
								CString szMessage;
								szMessage.Format(_T("All Names send : %02d errors occured"),m_ErrorCnt);
								CDisplayDevice::DisplayInfoMsg(szMessage);

								m_pComApplication->StopTimer(TIMERID_RENAME_TIMEOUT);
								Fire_OnFsmEnded();
							}						
							else 
							{							
								if (!m_pDatabase->GetNode(m_NodeIndex,&m_pNode)) 
								{	
									m_CurrentState = STATE_IDLE;

									CString szMessage("Canceled by Fatal Error : No Nodes Found");																	
									CDisplayDevice::DisplayErrorMsg(szMessage);

									m_pComApplication->StopTimer(TIMERID_RENAME_TIMEOUT);
									Fire_OnFsmEnded();
								}
								else 
								{
									m_szName = m_pDatabase->GetNodeName(m_pNode->bLogicalAddress);
									m_bLength = FormatData(m_szName,m_pbData);				

									if (m_bLength != 0) 
									{
										m_bPos = 0; 		
									
										m_bPos = WriteNodeName(m_pNode->bLogicalAddress,
															 &m_pbData[0],
															 m_bPos,
															 GetLengthToWrite(m_bLength,m_bPos));						

										m_CurrentState = STATE_NODENAME_WRITE;
										
									}// end if bLength								
								}
							}
						}
					}					
				}break;			
			}
		}break;		
		
		default:

		break;
	}
}

BYTE CRenameNodeUnitsEngine::WriteNodeName(BYTE bNodeAddress,BYTE *pbData,BYTE bPos,BYTE bLengthToWrite)
{
	BYTE szConvertedData[15];
	GetConvertedNameData(bLengthToWrite,&pbData[bPos],&szConvertedData[0]);
	CTXNodeMessage::getInstance()->SetNodeName(bNodeAddress,
											   bPos,
											   bLengthToWrite,
											   &szConvertedData[0]);
	
	m_pComApplication->StartTimer(TIMERID_RENAME_TIMEOUT,RENAME_ACK_TIMEOUT);
	bPos += bLengthToWrite;
	return bPos;
}

BYTE CRenameNodeUnitsEngine::WriteUnitName(BYTE bNodeAddress,BYTE bUnitAddress,BYTE *pbData,BYTE bPos,BYTE bLengthToWrite)
{
	ASSERT(bUnitAddress != BROADCAST_UNIT);
	if (bUnitAddress == BROADCAST_UNIT) return 0;

	BYTE szConvertedData[15];
	GetConvertedNameData(bLengthToWrite,&pbData[bPos],&szConvertedData[0]);									
	CTXUnitMessage *pTxUnitMessage = new CTXUnitMessage;
	if (NULL != pTxUnitMessage) {
		pTxUnitMessage->SetUnitName(bNodeAddress,
									bUnitAddress,
									bPos,
									bLengthToWrite,
									&szConvertedData[0]);
		delete pTxUnitMessage;
	}
	m_pComApplication->StartTimer(TIMERID_RENAME_TIMEOUT,RENAME_ACK_TIMEOUT);	
	bPos += bLengthToWrite;
	return bPos;
}
///////////////////////////////////////////////////////////////////////////////////////

// Changes Versie 0x0B00: BUG-0147
// Array pbData wissen.
// Indien de lengte 0 is dan als eerste char een Spatie door sturen.
BYTE CRenameNodeUnitsEngine::FormatData(CString szName,BYTE *pbData)
{
	int length=-1;
	int i;

	memset(pbData,0,UNIT_NAME_LENGTH*sizeof(BYTE));

	if (!(szName.IsEmpty()))
	{
		length = szName.GetLength();
	}

	if (length<=0)				// sedert versie 0x0B00
	{
		pbData[0] = ' ';		// Spatie.
		length=1;
	}
	else if (length<=(UNIT_NAME_LENGTH-1)) 
	{
		for (i=0;i<length;i++) 
		{
			pbData[i] = szName.GetAt(i);						
		}

		pbData[length]=0;
		length++;
	}
	else 
	{
		length = UNIT_NAME_LENGTH-1;
		
		for (i=0;i<length;i++) 
		{
			pbData[i] = szName.GetAt(i);		
		}

		pbData[length]=0;
		length= UNIT_NAME_LENGTH;
	}

	ASSERT((length>=0)&&(length<=UNIT_NAME_LENGTH));
	return (BYTE) length;
}


void CRenameNodeUnitsEngine::GetConvertedNameData(BYTE bLength,const BYTE *pbData,BYTE *szConvertedData)
{
	TRACE1("\nAscii Data : %s",pbData);
	
	for (int i=0;i<bLength;i++) 
	{														
		if (((pbData[i]/16) >= 0) && ((pbData[i]/16) <= 9)) 
			szConvertedData[i*2] = (pbData[i]/16) + 48;
		else if (((pbData[i]/16) >= 10) && ((pbData[i]/16) <= 15)) 
			szConvertedData[i*2] = (pbData[i]/16) + 55;

		if (((pbData[i]%16) >= 0) && ((pbData[i]%16) <= 9)) 
			szConvertedData[i*2+1] = (pbData[i]%16) + 48;
		else if (((pbData[i]%16) >= 10) && ((pbData[i]%16) <= 15)) 
			szConvertedData[i*2+1] = (pbData[i]%16) + 55;													
	}
	szConvertedData[bLength*2]='\0';		
	TRACE1("\nConverted Data : %s",szConvertedData);
}


///////////////////////////////////////////////////////////////////
inline void CRenameNodeUnitsEngine::Fire_OnFsmEnded(void) 
{
	//m_pComApplication->NotifyRenameFSMEnded();
	if (m_pComApplication != 0)
	{
		m_pComApplication->NotifyOnTaskDone(COMAPPLICATION_TASK_ID_RENAME_ENGINE);
	}
}

void CRenameNodeUnitsEngine::DisplayMessage(BYTE bNodeAddress,BOOL fResult)
{
	CString szMessage;
	CString szResult;
	
	if (fResult)  szResult.Format("OK !");
	else szResult.Format("Failure!");	

	szMessage.Format(_T("Node 0x%02x ,Name '%s' - Written "),bNodeAddress,m_szName);
	szMessage += szResult;
	CDisplayDevice::DisplayInfoMsg(szMessage);
}

void CRenameNodeUnitsEngine::DisplayMessage(BYTE bNodeAddress,BYTE bUnitAddress,BOOL fResult)
{
	CString szMessage;
	CString szResult;	

	if (fResult) 
	{
		szResult.Format("OK !");
	}
	else 
	{
		szResult.Format("Failure!");
	}
	
	szMessage.Format(_T("Node 0x%02x,Unit 0x%02x ,Name '%s' - Written %s"),
					 bNodeAddress,
					 bUnitAddress,
					 m_szName,
					 (LPCTSTR) szResult);		// CHANGES_MVS_2008_CSTRING

	CDisplayDevice::DisplayInfoMsg(szMessage);
}

//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////

void CRenameNodeUnitsEngine::FSMSendEvents(Events bEvent)
{
	switch (m_CurrentMode) 
	{
		case MODE_SEND_ALLNAMES:	
			FSMRenameNodeUnits(bEvent);		
		break;
		
		case MODE_SEND_NODENAME:	
			FSMRenameNode(bEvent);			
		break;
		
		case MODE_SEND_UNITNAME:	
			FSMRenameUnit(bEvent);			
		break;
		
		default:					
			ASSERT(0);						
		break;
	}
}

void CRenameNodeUnitsEngine::SendUnitName(const BYTE bNodeLogAddress,const BYTE bUnitLogAddress)
{
	m_bNodeAddress = bNodeLogAddress;
	m_bUnitAddress = bUnitLogAddress;

	if (m_bUnitAddress != BROADCAST_UNIT) 
	{
		m_CurrentMode = MODE_SEND_UNITNAME;
		FSMSendEvents(EV_RENAME_START);
	}
}

void CRenameNodeUnitsEngine::SendNodeName(const BYTE bNodeLogAddress) 
{
	m_bNodeAddress = bNodeLogAddress;

	m_CurrentMode = MODE_SEND_NODENAME;
	FSMSendEvents(EV_RENAME_START);
}

void CRenameNodeUnitsEngine::SendAllNames(void) 
{
	m_CurrentMode = MODE_SEND_ALLNAMES;
	FSMSendEvents(EV_RENAME_START);
}

