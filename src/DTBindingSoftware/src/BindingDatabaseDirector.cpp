// BindingDatabaseDirector.cpp: implementation of the CBindingDatabaseDirector class.
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingDatabaseDirector.h"

#include "BindingEntries.h"
#include "BindingEntry.h"
#include "BindingUnits.h"
#include "NodeDatabase.h"
#include "CanNode.h"
#include "MyWindowMessages.h"
#include "NodeDatabaseOperations.h"
#include "BindingConversion.h"
#include "BindingEntryOperations.h"
#include "bindingunitoperations.h"
#include "BindingEntriesOperations.h"
#include "BindingFilesOperations.h"
#include "BindingConversionAuto.h"
#include "App\Appbindings\AppBindingConv2Nodes\BindingConversionToNodesNormal.h"
#include "utils/app/DisplayDevice.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingDatabaseDirector::CBindingDatabaseDirector(CBindingFiles* pBindingFiles,
												   CBindingEntries* pBindingEntries,												  
												   CNodeDatabase* pNodeDatabase) :
m_pBindingFiles(pBindingFiles),
m_pBindingEntries(pBindingEntries),
m_pNodeDatabase(pNodeDatabase)
{
	ASSERT(m_pBindingFiles);
	ASSERT(m_pBindingEntries);
	ASSERT(m_pNodeDatabase);
}

CBindingDatabaseDirector::~CBindingDatabaseDirector()
{
}

// TM 20081223: Gebruik van Iterators.
// TM 20100406: extra param unitAddress.
BOOL CBindingDatabaseDirector::_Delete(BYTE bNodeAddress, int unitAddress)
{
	ASSERT(m_pBindingEntries);

	CBindingEntries::Iterator it(m_pBindingEntries->CreateIterator());
	for (it.Begin();!it.IsDone();it.Next())
	{
		CBindingEntry* const pBindingEntry = it.CurrentItem( );	
		ASSERT( pBindingEntry );

		CBindingUnits* const pBindingUnits = pBindingEntry->GetColUnit();	
		ASSERT(pBindingUnits);
				
		// TM 20060418 : Bug 
		const unsigned short usBindingNr = pBindingEntry->GetBindingNr();
		//const unsigned short nID = pBindingEntry->GetID();

		CBindingUnits::Iterator it2(pBindingUnits->CreateIterator());
		for (it2.Begin();!it2.IsDone();it2.Next())
		{
			CBindingUnit* const pBindingUnit = it2.CurrentItem();
			ASSERT( pBindingUnit );

			if (pBindingUnit->GetNodeAddress() == bNodeAddress) 
			{		
				if (unitAddress == -1 ||
					pBindingUnit->GetUnitAddress( ) == unitAddress)
				{
					const BOOL fResult = Delete(pBindingEntry);
					ASSERT(fResult);
					return fResult;
				}
			}
		}	
	}		
	return FALSE;
}

unsigned short CBindingDatabaseDirector::Delete(
	BYTE bNodeAddress, 
	BYTE bUnitAddress)
{
	unsigned short usResult = 0;

	// Alle binding entries + strings in de binding databases
	// verwijderen.
	while(_Delete(bNodeAddress, bUnitAddress) == TRUE)
	{
		usResult++;
	}

	// manuele bindings verwijderen.
	CBindingFilesOperations bindingFilesOperations(m_pBindingFiles);
	bindingFilesOperations.RemoveStrings(bNodeAddress, bUnitAddress);
	return usResult;
}

BOOL CBindingDatabaseDirector::Delete(CBindingEntry* const pBindingEntry)
{
	const unsigned short nID = pBindingEntry->GetID();
	const unsigned short usBindingNr = pBindingEntry->GetBindingNr();
	
	if (nID != 0) 
	{
		if (m_pBindingEntries->Remove(nID) == TRUE) 
		{			
			// Remove BindingNr in All BindingFile dbases
			CBindingConversionToNodesNormal BindingConvertToNodes;
			BindingConvertToNodes.DeleteBindings(usBindingNr);
		}
		else
		{
			ASSERT(0);
			return FALSE;
		}

		/**
		 * Check if removed from CBindingEntries database ...
		 */
		ASSERT(m_pBindingEntries->Get(nID,FALSE) == FALSE);
	}
	return TRUE;
}

unsigned short CBindingDatabaseDirector::Delete(BYTE bNodeAddress)
{
	int nCount = 0;	

	while(1) 
	{
		if (_Delete(bNodeAddress) == TRUE) 
		{
			nCount++;
		}
		else 
		{
			break;
		}

		ASSERT(nCount < 65536);
	}

	ASSERT( nCount >= 0 && nCount <= 0xFFFF );		// CHANGES_MVS_2008_WARNINGS
	return static_cast<unsigned short>(nCount);		// CHANGES_MVS_2008_WARNINGS
}

unsigned short CBindingDatabaseDirector::Add(CBindingEntry* const pBindingEntry,BindingStatusError::BindingError_t* errorStatus) {
	const BOOL notifyGUI = TRUE;
	const BOOL modifyNames = TRUE;
	return addBindingEntry(m_pBindingEntries, pBindingEntry, NULL, modifyNames, notifyGUI, errorStatus);
}


// sedert V16.17
unsigned short CBindingDatabaseDirector::addBindingEntry(CBindingEntries* pBindingEntries,CBindingEntry* const pBindingEntry,
														 CBindingEntry** ppNewBindingEntry,BOOL modifyNames, BOOL notifyGUI,
														 BindingStatusError::BindingError_t* errorStatus) {
	if (NULL == errorStatus) {
		ASSERT(NULL != errorStatus);
		return 0;
	}

	if ((NULL == pBindingEntry) || (NULL == pBindingEntries)) {
		ASSERT(NULL != pBindingEntry);
		ASSERT(NULL != pBindingEntries);
		*errorStatus = BindingStatusError::BINDING_EC_GENERAL;
		return 0;
	}
		
	pBindingEntry->SetSend(FALSE);
		
	CBindingConversion BindingConverter;
	if (FALSE == BindingConverter.ProcessBindingEntry(pBindingEntry)) {		
		*errorStatus = BindingStatusError::BINDING_EC_PROCESS;
		return 0;
	}
		
	ASSERT(pBindingEntry);

	if (modifyNames) {
		CBindingEntryOperations BindingEntryOperations(pBindingEntry);
		BindingEntryOperations.SetDefaultName(TRUE);
	}

	ASSERT(pBindingEntry->Verify() == TRUE);
	
	CBindingEntriesOperations BindingEntriesOperations(pBindingEntries);
	const unsigned short nId = BindingEntriesOperations.Add(pBindingEntry);											
	ASSERT(nId);
	ASSERT(pBindingEntries->Get(nId,FALSE));		
			
	CBindingEntry *const pCurrBindingEntry = pBindingEntries->Get(nId);
	if (pCurrBindingEntry == 0) {
		*errorStatus = BindingStatusError::BINDING_EC_DBASE_ENTRIES;
		return 0;
	}					

	CBindingConversionToNodesNormal BindingConvertToNodes;
	if (FALSE == BindingConvertToNodes.ProcessBindings(pCurrBindingEntry))
	{										
		pBindingEntries->Remove(nId);						// REMOVE BINDINGS FROM DATABASE ...			
		ASSERT(pBindingEntries->Get(nId,FALSE) == FALSE);	// Check if removed from CBindingEntries database ...

		if (notifyGUI) {
			const CString szMessage("Error : Binding could not be added to nodes and is removed from binding database");
			CDisplayDevice::DisplayInfoMsg(szMessage);
		}			

		*errorStatus = BindingStatusError::BINDING_EC_BINDINGFILES;
		return 0;
	}

	if (notifyGUI) {
		if (NULL != AfxGetMainWnd()){
			AfxGetMainWnd()->SendMessage(WM_APPLICATION_NOTIFY_BINDING_ADD,(WPARAM) nId);
		}
	}
	if (NULL != ppNewBindingEntry) {
		*ppNewBindingEntry = pCurrBindingEntry;
	}

	ASSERT(nId == pCurrBindingEntry->GetID());		
	return nId;	// Succeeded !!!		
}


/**
 *
 */
BOOL CBindingDatabaseDirector::Add(CBindingEntry* const pBindingEntry,CBindingConversionAuto* const pBindingConversionAuto,BindingStatusError::BindingError_t* errorStatus)
{
	if (NULL == errorStatus) {
		ASSERT( NULL != errorStatus);
		return FALSE;
	}
	if (NULL == pBindingEntry || NULL == pBindingConversionAuto) {
		*errorStatus = BindingStatusError::BINDING_EC_GENERAL;
		ASSERT( NULL != pBindingEntry );
		ASSERT( NULL != pBindingConversionAuto );
		return FALSE;
	}
		
	CBindingUnits* const pColUnits = pBindingEntry->GetColUnit();
	if (pColUnits == 0) {
		ASSERT( 0 );
		*errorStatus = BindingStatusError::BINDING_EC_GENERAL;
		return FALSE;
	}

	const int nNrOfAutoBindings = pBindingConversionAuto->ProcessAutoBindings(pColUnits);
	if (nNrOfAutoBindings == 0) { 
		*errorStatus = BindingStatusError::BINDING_EC_GENERAL;
		return FALSE;
	}

	for(int i=0;i<nNrOfAutoBindings;i++) {		
		CBindingEntry *pNewBindingEntry = 0;
		if (pBindingConversionAuto->GetBindingEntry(i,&pNewBindingEntry) == FALSE) {		
			CString szMessage((LPCSTR) IDS_EXCEPTION_AUTOBINDING_DBASE);
			AfxMessageBox(szMessage);
			*errorStatus = BindingStatusError::BINDING_EC_GENERAL;		// TODO ?????????????????????????
			return FALSE;			
		}

		if (pNewBindingEntry == 0) {
			ASSERT( 0 );
			*errorStatus = BindingStatusError::BINDING_EC_GENERAL;		// TODO ?????????????????????????
			return FALSE;
		}

		//CAUTION : We Must force BindingId & BindingNr back to 0
		pNewBindingEntry->SetBindingNr(0x0000);
		pNewBindingEntry->SetID(0);
	
		const unsigned short nIdNewBindingEntry = Add(pNewBindingEntry,errorStatus);
		if (nIdNewBindingEntry != 0)  {					
			if (AfxGetMainWnd() != NULL) {
				// MainFrame : update SCREEN ...
				AfxGetMainWnd()->SendMessage(WM_APPLICATION_NOTIFY_BINDING_ADD,(WPARAM) nIdNewBindingEntry);
			}
		}
	} // end for loop ...

							
	if (1)	// AUTO CONVERSION INFO
	{
		CString szMessage;
		szMessage.Format("Auto binding converter made %d bindings",nNrOfAutoBindings);
		CDisplayDevice::DisplayInfoMsg(szMessage);

		if (pBindingConversionAuto->GetAutoBindingInfo() != 0) {
			pBindingConversionAuto->GetAutoBindingInfo()->Report();
		}
	}	
	return TRUE;
}

void CBindingDatabaseDirector::Clear()
{
	m_pBindingFiles->ClearDatabase();
	m_pBindingEntries->RemoveAll();	
}

CBindingEntry* CBindingDatabaseDirector::Update(unsigned short nID)
{
	if (m_pNodeDatabase == 0) 
	{
		ASSERT(m_pNodeDatabase);
		return 0;
	}
	CBindingEntry *const pBindingEntry = m_pBindingEntries->Get(nID);
	if (pBindingEntry == 0)
	{
		ASSERT(pBindingEntry);
		return 0;
	}
				
	CNodeDatabaseOperations NodeDatabaseOperations(m_pNodeDatabase);
	CBindingUnits::Iterator it(pBindingEntry->GetColUnit()->CreateIterator());
	for (it.Begin();!it.IsDone();it.Next())
	{
		CBindingUnit* const pBindingUnit = it.CurrentItem( );
		CCanNode* pNode=0;
		STRUCT_UNIT *pUnit=0;

		ASSERT( pBindingUnit );

		if (TRUE==NodeDatabaseOperations.Find(pBindingUnit,&pNode,&pUnit))
		{		
			if (TRUE==CBindingUnitOperations::verifyTypes(pBindingUnit,pUnit))
			{
				pBindingUnit->SetName(pUnit->szUnitName);
			}
			else
			{
				// zie BUG-0153
				CString szError("Different unit types found in the node database and in the unit collection");
				AfxMessageBox(szError);
				return 0;	
			}
		}
		else 
		{
			ASSERT(0);

			CString szError("The unit could not be found in the nodedatabase");
			AfxMessageBox(szError);
			return 0;
		}						
	}
	return pBindingEntry;
}



/**
 * For all Document nodes , check if bindingfile exists ... else create ...
 */
BOOL CBindingDatabaseDirector::CreateNewFiles()
{
	CBindingFilesOperations Operations(m_pBindingFiles);
	return (Operations.CreateNewFiles( m_pNodeDatabase ));
}

BOOL CBindingDatabaseDirector::AssignBindingFiles(CNodeDatabase* const pNodeDatabase)
{
	CBindingFilesOperations Operations(m_pBindingFiles);
	return (Operations.Assign(pNodeDatabase));
}


#ifdef SKIP

		// TM 20081223: Gebruik van Iterators ...
		unsigned short CBindingDatabaseDirector::CompareDatabases(CStringArray* const pStringArray)
		{
			int nDif = 0;	

			CBindingFiles::Iterator it(m_pBindingFiles->CreateIterator());
			for (it.Begin();!it.IsDone();it.Next())
			{
				CBindingFile* const pBindingFile = it.CurrentItem();

				if (1)
				{
					CString szMessage;
					szMessage.Format("Check BindingFile for node : 0x%02x",
									 pBindingFile->GetNodeAddress());
					CDisplayDevice::DisplayInfoMsg(szMessage);
				}
										
				BOOL fMatch = FALSE;
				const int MAX = pBindingFile->Count();
				for (int j=0; j<MAX; j++,fMatch=FALSE) 
				{									
					const unsigned short usBindingNr = pBindingFile->GetBindingNr(j);

					if (usBindingNr != 0) 
					{
						// Check BindingEntries if match:
						// Controleren of er voor deze entry in de bindingFile een binding in de database
						// aanwezig is met hetzelfde binding NR.
						// Indien niet aanwezig wordt deze bindingstring toegevoegd aan een string array.

						CBindingEntries::Iterator bindEntry_it(m_pBindingEntries->CreateIterator());
						for (bindEntry_it.Begin( );
							 !bindEntry_it.IsDone( );
							 bindEntry_it.Next())
						{
							CBindingEntry* const pBindingEntry = bindEntry_it.CurrentItem( );
							ASSERT( pBindingEntry );			
					
							if (pBindingEntry->GetBindingNr() == usBindingNr) 
							{
								fMatch = TRUE;
								break;
							}					
						} 			
						if (fMatch == FALSE) 
						{
							nDif++;		
							
							pStringArray->Add(pBindingFile->GetString(j));					
						}						
					} // if (usBindingNr != 0) 
				}//for (int j=0;j<pBindingFile->Count();j++,fMatch=FALSE) 
			}//for (int i=0;i<m_pBindingFiles->Count();i++) 

			ASSERT( nDif >= 0 && nDif <= 0xFFFF);			// CHANGES_MVS_2008_WARNINGS
			return static_cast<unsigned short>(nDif);		// CHANGES_MVS_2008_WARNINGS
		}


#endif

// TM 20081223: Gebruik van Iterators.
BOOL CBindingDatabaseDirector::UpdateModifiedFlags(BOOL fSaveToBindingClass)
{
	int iNrNodes;
	if (m_pNodeDatabase->NROfNodes(&iNrNodes) == TRUE)
	{
		const int iNrBindingFiles = m_pBindingFiles->Count();
		
		if (iNrBindingFiles != iNrNodes) 
		{
			ASSERT(0);
			return FALSE;
		}

		CBindingFiles::Iterator it(m_pBindingFiles->CreateIterator());
		for (it.Begin(); !it.IsDone(); it.Next())
		{		
			CBindingFile* const pFile = it.CurrentItem( );
			ASSERT( pFile );
		
			CCanNode* pNode;
			int t;
			const BYTE bNodeAddress = pFile->GetNodeAddress();	

			if (m_pNodeDatabase->FindNode(bNodeAddress,&t,&pNode) == FALSE) 
			{
				ASSERT(0);
				return FALSE;
			}			
							
			if (fSaveToBindingClass == TRUE) 
			{
				// Update BindingFile data from CCanNode
				if (pNode->ulApplicationFlags & CCanNode::NODE_BINDINGFILE_MODIFIED_FLAG) 
				{
					pFile->SetModified();								
				}
				else 
				{
					pFile->ClearModified();
				}
			}
			else 
			{
				// Update CCanNode With data from BindingFile...
				if (pFile->IsModified() == TRUE)  
				{
					pNode->ulApplicationFlags |= CCanNode::NODE_BINDINGFILE_MODIFIED_FLAG;
				}
				else 
				{
					pNode->ulApplicationFlags &= CCanNode::NODE_BINDINGFILE_MODIFIED_FLAG^0xFF;
				}
			}					
		} // end for ...
		return TRUE;
	}
	return FALSE;
}

void CBindingDatabaseDirector::Load(CStringArray* pStrArrayError)
{
	ASSERT( m_pBindingFiles );

	const BOOL fResult = m_pBindingFiles->Load(pStrArrayError);
	ASSERT( fResult );
}

void CBindingDatabaseDirector::Save()
{
	ASSERT( m_pBindingFiles );

	const BOOL fResult = m_pBindingFiles->Save();
	ASSERT( fResult );
}

void CBindingDatabaseDirector::UpgradeBindingUnitTypes(int nodeAddress) {
	ASSERT( m_pBindingEntries );
	ASSERT( m_pNodeDatabase );
	CBindingEntries::Iterator it(m_pBindingEntries->CreateIterator());
	for (it.Begin();!it.IsDone();it.Next()) {
		CBindingEntry* const pBindingEntry = it.CurrentItem( );	
		ASSERT( pBindingEntry );
		if (NULL != pBindingEntry) {
			CBindingUnits* const pBindingUnits = pBindingEntry->GetColUnit();	
			ASSERT(pBindingUnits);
			if (NULL != pBindingUnits) {
				CBindingUnits::Iterator it2(pBindingUnits->CreateIterator());
				for (it2.Begin();!it2.IsDone();it2.Next()) {
					CBindingUnit* const pBindingUnit = it2.CurrentItem();
					ASSERT( pBindingUnit );
					if ((NULL != pBindingUnit) && ((nodeAddress < 0) || (nodeAddress == pBindingUnit->GetNodeAddress()))) {				
						this->UpgradeBindingUnitTypes_Audio(pBindingUnit);
						this->UpgradeBindingUnitTypes_Dali(pBindingUnit);
					}
				}
			}
		}
	}
}

void CBindingDatabaseDirector::UpgradeBindingUnitTypes_Audio(CBindingUnit* const pBindingUnit) {
	const BYTE bBindingUnitType = pBindingUnit->GetBindingUnitType( );
	if ((bBindingUnitType == BINDING_UNIT_TYPE_AUDIO_HC4) || (bBindingUnitType == BINDING_UNIT_TYPE_AUDIO_HC4_BNO)) {
		CCanNode* pNode = 0;				
		int iEntryNR = 0;
		if (m_pNodeDatabase->FindNode(pBindingUnit->GetNodeAddress( ),&iEntryNR,&pNode)) {
			STRUCT_UNIT* pUnit = 0;
			if (m_pNodeDatabase->FindUnit(pNode, pBindingUnit->GetUnitAddress( ),&iEntryNR, &pUnit)) {
				// Enkel voor de audio units.						
				if ((pUnit->bUnitType == UNIT_TYPE_AUDIO_HC4) && (bBindingUnitType == BINDING_UNIT_TYPE_AUDIO_HC4 ||
																  bBindingUnitType == BINDING_UNIT_TYPE_AUDIO_HC4_BNO))
				{
					const BYTE bConfig = pUnit->UnitSpecs.AudioUnit.bAudioConfig;
					if ((bConfig & AUDIO_TYPE_CONFIG_MASK) == AUDIO_TYPE_CONFIG_BNO) {						
						if (bBindingUnitType != BINDING_UNIT_TYPE_AUDIO_HC4_BNO) {
							pBindingUnit->SetBindingUnitType( BINDING_UNIT_TYPE_AUDIO_HC4_BNO );
						}
					}
					else  {						
						if (bBindingUnitType != BINDING_UNIT_TYPE_AUDIO_HC4) {
							pBindingUnit->SetBindingUnitType( BINDING_UNIT_TYPE_AUDIO_HC4 );
						}
					}						
				}				
			}			
		}		
	} 
}

void CBindingDatabaseDirector::UpgradeBindingUnitTypes_Dali(CBindingUnit* const pBindingUnit) {
	const BYTE bBindingUnitType = pBindingUnit->GetBindingUnitType( );
	if ((BINDING_UNIT_TYPE_DIM == bBindingUnitType) || (BINDING_UNIT_TYPE_DIM_FWD == bBindingUnitType) || 
		(BINDING_UNIT_TYPE_CONTROL == bBindingUnitType) ||
		(BINDING_UNIT_TYPE_LCD_VIRTUAL == bBindingUnitType)) {
		CCanNode* pNode = 0;				
		int iEntryNR = 0;
		if (m_pNodeDatabase->FindNode(pBindingUnit->GetNodeAddress( ),&iEntryNR,&pNode)) {
			STRUCT_UNIT* pUnit = 0;
			if (m_pNodeDatabase->FindUnit(pNode, pBindingUnit->GetUnitAddress( ),&iEntryNR, &pUnit)) {
				unsigned long ulApplicationFlags = pBindingUnit->GetApplicationFlags();						
				if ((NULL != pUnit->m_pAppUnitData) && (CAppUnitDataDali::TYPE_UNIT_DATA_DALI == pUnit->m_pAppUnitData->getUnitDataType())) {				
					ulApplicationFlags = CBindingUnit::APPFLAG_DALI_BINDING_UNIT;
				}
				else {
					ulApplicationFlags &= ~CBindingUnit::APPFLAG_DALI_BINDING_UNIT;
				}					
				pBindingUnit->SetApplicationFlags(ulApplicationFlags);
			}
		}
	}
}