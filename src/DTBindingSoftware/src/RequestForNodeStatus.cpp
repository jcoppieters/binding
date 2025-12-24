// RequestForNodeStatus.cpp: implementation of the CRequestForNodeStatus class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RequestForNodeStatus.h"

#ifdef BINDING_SOFTWARE
	#include "NodeDatabase.h"
	#include "CanNode.h"
	#include "utils/app/DisplayDevice.h"
	#include "ComApplication.h"
	#ifdef _DEBUG
	#undef THIS_FILE
	static char THIS_FILE[]=__FILE__;
	#define new DEBUG_NEW
	#endif
#else
	#include "..\NodeDatabase\NodeDatabase.h"
	#include "..\NodeDatabase\CanNode.h"
	#include "..\communicationClass.h"
	#include "..\Helper\DisplayDevice.h"
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#ifdef BINDING_SOFTWARE
CRequestForNodeStatus::CRequestForNodeStatus(IComApplication *ptr,CNodeDatabase *ptr2) :
	m_pComApplication(ptr), m_pNodeDatabase(ptr2)
{
	ASSERT(NULL != this->m_pComApplication);
	ASSERT(NULL != this->m_pNodeDatabase);
	this->m_pFsmUnit = NULL;
	this->m_pCurrentNode = NULL;
	this->m_pCurrentUnit = NULL;
	this->m_mode = MODE_REQUEST_NODES_ALL;
	this->m_currentNodeAddress = -1;
	this->m_currentState = FSM_STATE_IDLE;
}
#else
CRequestForNodeStatus::CRequestForNodeStatus(CCommunicationClass *ptr,CNodeDatabase *ptr2)
{
	m_pComApplication = ptr;
	m_pNodeDatabase = ptr2;
	m_pFsmUnit = NULL;
	m_pCurrentNode = NULL;
	m_pCurrentUnit = NULL;
	this->m_mode = MODE_REQUEST_NODES_ALL;
	this->m_currentNodeAddress = -1;
	m_CurrState = FSM_STATE_IDLE;
}
#endif


CRequestForNodeStatus::~CRequestForNodeStatus() {
	this->destroyChildFsm( );
}

//-----------------------------------------------------------------------
//		Invoked by Parent : CComApplication ...	
//-----------------------------------------------------------------------
void CRequestForNodeStatus::Start(void) {
	if (this->m_currentState == FSM_STATE_DONE ) {
		this->m_currentState = FSM_STATE_IDLE;
	}

	this->m_currentNodeAddress = -1;
	this->m_mode = MODE_REQUEST_NODES_ALL;
	this->FSM(FSM_EV_OPERATION_START);
}

void CRequestForNodeStatus::Start(int nodeAddress) {	
	REQUIRE(nodeAddress >= 0x03 && nodeAddress<=0xfc);
	if (this->m_currentState == FSM_STATE_DONE ) {
		this->m_currentState = FSM_STATE_IDLE;
	}
	this->m_currentNodeAddress = nodeAddress;
	this->m_mode = MODE_REQUEST_NODES_SINGLE;
	this->FSM(FSM_EV_OPERATION_START);
}

void CRequestForNodeStatus::Cancel(void) {
	this->FSM(FSM_EV_OPERATION_CANCEL);
}

void CRequestForNodeStatus::EvSignalStatusStatusReceived(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bMethodData) {
	if ((this->m_pCurrentNode != NULL) && (this->m_pCurrentUnit != NULL) && (this->m_pFsmUnit != NULL)) {		
		if ((this->m_pCurrentNode->bLogicalAddress == bNodeAddress) && (this->m_pCurrentUnit->bUnitAddress == bUnitAddress)) {		
			if (this->m_pFsmUnit != NULL) {
				this->m_pFsmUnit->EvSignalStatusStatusReceived(bMethodData);			
			}
		}
	}
}

void CRequestForNodeStatus::EvAfterTimerExpiry(void) {
	if (this->m_pFsmUnit != NULL) {
		this->m_pFsmUnit->EvAfterTimerExpiry();
	}
}


//-----------------------------------------------------------//
// TODO HANDLE EVENTS CANCEL ....
//-----------------------------------------------------------//
BOOL CRequestForNodeStatus::FSM(enum_Events e) {
	switch (this->m_currentState) {
		case FSM_STATE_IDLE:			
			switch(e)  {
				case FSM_EV_OPERATION_START : {										
					char szMessage[100];
					sprintf(szMessage,"Start Request Statussen");					
					CDisplayDevice::DisplayInfoMsg(szMessage);

					this->Fire_OnFsmStart();

					this->m_iNodeNr = 0;
					this->m_iUnitNr = 0;
					this->m_iMaxNodes = 0;
				
					this->destroyChildFsm( );

					if (this->m_mode == MODE_REQUEST_NODES_ALL) {
						this->m_pNodeDatabase->NROfNodes(&this->m_iMaxNodes);			
					}
					else {
						this->m_iMaxNodes = 1;
					}

					while (this->CreateUnitStatusFsm() != TRUE);				
					
					if (this->m_pFsmUnit == NULL) {
						this->m_currentState = FSM_STATE_DONE;
						this->Fire_OnFsmDone();
						return TRUE;
					}
					else {			
						this->m_pFsmUnit->EvOperationStart();
						this->m_currentState = FSM_STATE_WAIT_FOR_ACK;
					}								
				} break;
		
				case FSM_EV_OPERATION_CANCEL : {		
					if (this->m_pFsmUnit != NULL) {
						this->m_pFsmUnit->EvOperationCancel();
						return TRUE;
					}
				} break;

				default:
				case FSM_EV_SIGNAL_UNITDONE:
				case FSM_EV_SIGNAL_UNITERROR: {
					ASSERT(0);

				} break;
			}
		break;

		case FSM_STATE_WAIT_FOR_ACK:			
			switch (e) {
				case FSM_EV_SIGNAL_UNITDONE: {					
					// Get Next Unit or next node ...					
					if (this->m_iUnitNr < (this->m_pCurrentNode->bNROfUnits-1)) {
						this->m_iUnitNr++;
					}
					else  {
						if (this->m_iNodeNr < (this->m_iMaxNodes-1)) {
							this->m_iNodeNr++;
							this->m_iUnitNr = 0;
						}
						else {							
							this->m_currentState = FSM_STATE_DONE;
							this->Fire_OnFsmDone();
							return TRUE;
						}
					}

					while (this->CreateUnitStatusFsm() != TRUE);				

					if (this->m_pFsmUnit == NULL) {										
						this->m_currentState = FSM_STATE_DONE;
						this->Fire_OnFsmDone();	
						return TRUE;
					}
					else {			
						this->m_pFsmUnit->EvOperationStart();					
					}
				} break;

				case FSM_EV_SIGNAL_UNITERROR :
				{					
					// Display error Message ...
					char szMessage[100];
					sprintf(szMessage,"Node 0x%02x,Unit 0x%02x : Max retries reached,Node is skipped...",
						this->m_pCurrentNode->bLogicalAddress,this->m_pCurrentUnit->bUnitAddress);
					CDisplayDevice::DisplayErrorMsg(szMessage);

					// Get Next Node 
					if (this->m_iNodeNr < (this->m_iMaxNodes-1)) {
						this->m_iNodeNr++;
						this->m_iUnitNr = 0;
				
						while (this->CreateUnitStatusFsm() != TRUE);				

						if (this->m_pFsmUnit == NULL) {
							this->m_currentState = FSM_STATE_DONE;	
							this->Fire_OnFsmDone();		
							return TRUE;
						}
						else {			
							this->m_pFsmUnit->EvOperationStart();					
						}
					}
					else {
						this->m_currentState = FSM_STATE_DONE;	
						this->Fire_OnFsmDone();		
						return TRUE;
					}
				} break;

				case FSM_EV_OPERATION_CANCEL: {	
					if (this->m_pFsmUnit != NULL) {
						this->m_pFsmUnit->EvOperationCancel();
						return TRUE;
					}
				} break;

				default:
				case FSM_EV_OPERATION_START:	
					//Absorbe 
				break;
			}
		break;

		case FSM_STATE_DONE :
			// Dummy State ...
		break;
	}
	return TRUE;
}

BOOL CRequestForNodeStatus::CreateUnitStatusFsm(void) {		
	this->destroyChildFsm();

	switch(this->m_mode) {
		case MODE_REQUEST_NODES_ALL: {
			for ( /* global*/ ;this->m_iNodeNr<this->m_iMaxNodes; this->m_iNodeNr++) {				
				if (this->m_pNodeDatabase->GetNode(this->m_iNodeNr,&this->m_pCurrentNode)) {
					for (/* global*/  ; this->m_iUnitNr < this->m_pCurrentNode->bNROfUnits; this->m_iUnitNr++) {						
						if (this->m_pNodeDatabase->GetUnit(this->m_pCurrentNode,this->m_iUnitNr,&this->m_pCurrentUnit)) {
							if (TRUE == this->CreateUnitStatusFsm(this->m_pCurrentNode->bLogicalAddress, this->m_pCurrentUnit->bUnitAddress, this->m_pCurrentUnit->bUnitType)) {
								return TRUE;
							}								
						}
					}			
					this->m_iUnitNr = 0;		// alle units done of node --> Reset units to 0 !!!
				}
			}
		} break;

		case MODE_REQUEST_NODES_SINGLE: {										
			INVARIANT(this->m_currentNodeAddress >= 0x03 && this->m_currentNodeAddress <=0xfc);				
			int dummy = 0;
			if (this->m_pNodeDatabase->FindNode(this->m_currentNodeAddress,&dummy,&this->m_pCurrentNode)) {
				for (/* global*/  ; this->m_iUnitNr < this->m_pCurrentNode->bNROfUnits; this->m_iUnitNr++) {						
					if (this->m_pNodeDatabase->GetUnit(this->m_pCurrentNode,this->m_iUnitNr,&this->m_pCurrentUnit)) {
						if (TRUE == this->CreateUnitStatusFsm(this->m_pCurrentNode->bLogicalAddress, this->m_pCurrentUnit->bUnitAddress, this->m_pCurrentUnit->bUnitType)) {
							return TRUE;
						}								
					}
				}			
				this->m_iUnitNr = 0;		// alle units done of node --> Reset units to 0 !!!
			}																
		}  break;
	}
	return TRUE;	// OPPASSEN - Must return TRUE - Else endless loop ...	
}
		

BOOL CRequestForNodeStatus::CreateUnitStatusFsm(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bUnitType) {
	switch(this->m_pCurrentUnit->bUnitType) {
		case UNIT_TYPE_DIM :
			this->m_pFsmUnit = new CFSMUnitStatusDim(this,bNodeAddress,bUnitAddress);
		return TRUE;
		case UNIT_TYPE_SWITCH :
			this->m_pFsmUnit = new CFSMUnitStatusSwitch(this,bNodeAddress,bUnitAddress);
		return TRUE;
		case UNIT_TYPE_DUOSWITCH :
			this->m_pFsmUnit = new CFSMUnitStatusDuoSwitch(this,bNodeAddress,bUnitAddress);
		return TRUE;
		case UNIT_TYPE_CONTROL :
		case UNIT_TYPE_LCD_VIRTUAL :
			this->m_pFsmUnit = new CFSMUnitStatusControl(this,bNodeAddress,bUnitAddress);
		return TRUE;
		case UNIT_TYPE_SENS:
			this->m_pFsmUnit = new CFSMUnitStatusSens(this,bNodeAddress,bUnitAddress);
		return TRUE;
		case UNIT_TYPE_AUDIO:
			this->m_pFsmUnit = new CFSMUnitStatusAudio(this,bNodeAddress,bUnitAddress);
		return TRUE;
		case UNIT_TYPE_BOSERS:
			this->m_pFsmUnit = new CFSMUnitStatusBose(this,bNodeAddress,bUnitAddress);
		return TRUE;
		case UNIT_TYPE_IRTX:
			this->m_pFsmUnit = new CFSMUnitStatusIRTX(this,bNodeAddress,bUnitAddress);
		return TRUE;					
		case UNIT_TYPE_RC5RX:
			this->m_pFsmUnit = new CFSMUnitStatusRC5RX(this,bNodeAddress,bUnitAddress);
		return TRUE;							
		case UNIT_TYPE_ALARM:
			this->m_pFsmUnit = new CFSMUnitStatusAlarm(this,bNodeAddress,bUnitAddress);
		return TRUE;
		case UNIT_TYPE_AVMATRIX:
			this->m_pFsmUnit = new CFSMUnitStatusAVMatrix(this,bNodeAddress,bUnitAddress);
		return TRUE;
		case UNIT_TYPE_AUDIO_HC4:
			this->m_pFsmUnit = new CFSMUnitStatusAudioHC4(this,bNodeAddress,bUnitAddress);
		return TRUE;
		case UNIT_TYPE_VIDEOMUX:
			this->m_pFsmUnit = new CFSMUnitStatusVideomux(this,bNodeAddress,bUnitAddress);
		return TRUE;
		case UNIT_TYPE_PS2INTELLIMOUSE:
			this->m_pFsmUnit = new CFSMUnitStatusPS2Mouse(this,bNodeAddress,bUnitAddress);
		return TRUE;
		case UNIT_TYPE_ENERGYMEASURE:	
			this->m_pFsmUnit = new CFSMUnitStatusEnergyMeasure(this,bNodeAddress,bUnitAddress);
		return TRUE;
	}
	return FALSE;
}

// ----------------------------------------------------------------------
//			Events Received From Child
// ----------------------------------------------------------------------
void CRequestForNodeStatus::StartTimer() {
	if (this->m_pComApplication != NULL) {
		this->m_pComApplication->StartTimer( TIMERID_REQUESTSTATUSSEN_FSM, REQUESTSTATUSSEN_TIMEOUT );	
	}
}

void CRequestForNodeStatus::StopTimer() {
	if (this->m_pComApplication != NULL) {	
		this->m_pComApplication->StopTimer( TIMERID_REQUESTSTATUSSEN_FSM );
	}
}

void CRequestForNodeStatus::NotifyEvUnitDone() {
	this->FSM(FSM_EV_SIGNAL_UNITDONE);
}

void CRequestForNodeStatus::NotifyEvUnitError() {
	this->FSM(FSM_EV_SIGNAL_UNITERROR);
}

void CRequestForNodeStatus::NotifyEvCancelled(void) {	
	this->m_currentState = FSM_STATE_DONE;
	this->destroyChildFsm( );
	this->Fire_OnFsmCancelled();
}

// ----------------------------------------------------------------------
//			Events Fired To Parent
// ----------------------------------------------------------------------
inline void CRequestForNodeStatus::Fire_OnFsmDone(void) {	
	const char szMessage[] = {"Request for Statussen done ..."};
	CDisplayDevice::DisplayInfoMsg((char*) szMessage);
	if (this->m_pComApplication != NULL) {	
		this->m_pComApplication->NotifyOnTaskDone(COMAPPLICATION_TASK_ID_REQUEST_FOR_STATUSSEN);
	}
}

inline void CRequestForNodeStatus::Fire_OnFsmCancelled(void) {
	const char szMessage[] = {"Request for Statussen cancelled..."};
	CDisplayDevice::DisplayInfoMsg((char*) szMessage);
	if (this->m_pComApplication != NULL) {		
		this->m_pComApplication->NotifyOnTaskCancelled(COMAPPLICATION_TASK_ID_REQUEST_FOR_STATUSSEN);
	}
}

inline void CRequestForNodeStatus::Fire_OnFsmStart(void) {
	if (this->m_pComApplication != NULL) {
		//pComApplication->NotifyEventRequestStatussenFSMStart();
	}
}

void CRequestForNodeStatus::destroyChildFsm() {
	if (this->m_pFsmUnit != NULL) {
		delete this->m_pFsmUnit;
		this->m_pFsmUnit = NULL;
	}
}