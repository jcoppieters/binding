// NodeDatabaseApplication.cpp: implementation of the CAppNodeDatabase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "appnodedbase.h"
#include "MyException.h"
#include "NodeDatabaseOperations.h"
#include "NodeDatabaseVerify.h"			
#include "App\Appbindings\appbindingproperty\BindingPropertyUnit.h"
#include "App\Appbindings\appbindingproperty\BindingPropertyFactory.h"
#include "app\appCfg\AppCfginfo\cfginfoproductID_def.h"	
#include "AppLogging.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
static BOOL helper_CreateBindingUnitProperties(CCanNode *pNode,STRUCT_UNIT *pUnit) {
	if (NULL == pNode || NULL == pUnit) {
		REQUIRE(NULL != pNode);
		REQUIRE(NULL != pUnit);
		return FALSE;
	}

	// Oppassen: pNewUnit mag NULL zijn.
	CBindingPropertyUnit* const pNewUnit = CBindingPropertyFactory::Create(pNode,pUnit);	
	ASSERT((pNewUnit == 0) ||
		   (pNewUnit != NULL && pNewUnit->GetNodeAddress() == pUnit->bNodeLogicalAddress && pNewUnit->GetUnitAddress() == pUnit->bUnitAddress));
	INVARIANT(NULL != pUnit);
	pUnit->m_pBindingUnit=pNewUnit;		// pBindingUnit mag NULL zijn.
	return TRUE;
}


static BOOL CreateBindingUnitProperties_whenNonAssigned(CCanNode *pNode,STRUCT_UNIT* pUnit) {
	BOOL result = FALSE;
	if ((NULL != pUnit) && (NULL == pUnit->m_pBindingUnit)) {
		result = helper_CreateBindingUnitProperties(pNode,pUnit);		
	}
	return result;
}

static BOOL helper_CopyRuntimeUnitToDocUnit(CCanNode* pDocNode, STRUCT_UNIT* pSourceRTUnit, STRUCT_UNIT* pDestDocUnit) {

	// De nieuwe runtime unit specs initialiseren voor we deze copieren...	NIET NODIG: Wordt aangeropen wanneer we een unit toevoegen.
	// CNodeDatabase::InitRuntimeUnitSpecs(pSourceRTUnit);							

	// Alle properties copieren (incl unitspecs)
	CNodeDatabase::CopyUnitObject(pDestDocUnit,pSourceRTUnit);

	// Binding unit aanmaken en koppelen aan de unit					
	CreateBindingUnitProperties_whenNonAssigned(pDocNode,pDestDocUnit);

	return TRUE;
}

// V16.43: Nieuwe functie aangemaakt (Code herwerkt.)
//
// Controle van het node type (+ product codes.)
static BOOL helper_compare_nodeType(const CCanNode& refDocNode, const CCanNode& refRuntimeNode) {	
	if (refDocNode.bNodeType == refRuntimeNode.bNodeType) {		
		return TRUE;	// Wanneer de node types dezelfde zijn => OK.
	}

	// Wanneer de node types niet gelijk zijn.
	// utzondering: Doc node=Sbox, Runtime node=Sbox Ultra.
	// Reden: Het node type van een sbox ultra is een gateway, van de gewone sbox is dit een standaard node.
	if ((PRODUCT_ID_HC_BOX == refDocNode.GetProductID()) && (PRODUCT_ID_HC_BOX_ULTRA == refRuntimeNode.GetProductID())) {
		return TRUE;	// Upgrade van een sbox naar een sbox ultra => Ok.
	}

	return TRUE;
}

// V16.43: Nieuwe functie aangemaakt (Code herwerkt.)
//
// Controle of er 1 unit (=virtuele unit met adres 0xff) aanwezig is in de database node.
//
static BOOL helper_compare_nodeHoldsVirtualUnits(CNodeDatabase& refNodeDatabase, CCanNode& refDocNode, const CCanNode& refRuntimeNode) {
	ASSERT(refDocNode.bNROfUnits > refRuntimeNode.bNROfUnits);
	if (refDocNode.bNROfUnits!=(refRuntimeNode.bNROfUnits+1)) {
		return FALSE;	// Er is altijd maar 1 virtuele unit mogelijk.
	}

	// Er is unit meer aanwezig in de doc node.
	// Conroleren of er een unit aanwezig is met adres 0xFF.
	int dummy = 0;
	STRUCT_UNIT* pDocUnit = NULL;
	const int virtualUnitAddress = UNIT_VIRTUAL_BROADCAST_ADDRESS;	// Adres 0xFF = Virtuele unit.
	return refNodeDatabase.FindUnit(&refDocNode,virtualUnitAddress,&dummy,&pDocUnit);		
}


// V16.43: Nieuwe functie aangemaakt (Code herwerkt.)
// 
// NOTE_1: Doc node die een virtuele unit heeft.
//		  - Nr of units = 5
//			-> U0 tem U3	(4 units)
//			-> UFF			(1 virtuele unit).
//			-> Last unit address = U3 => (Nr Of Units - 2)
//
// NOTE_2: Doc node die geen virtuele units heeft.
//		  - Nr of Units = 5.
//			-> U0 - U4		(5 units)
//			-> Last unit address = U4 => (NR Of Units - 1)
//
//
// Oppassen: De Runtime node bevat (na het inladen) ook de virtuele unit.
//		     zie de functie TMCopyRuntimeToDocDatabase( )
// 
static BOOL helper_compare_unitsCompactibel(CNodeDatabase& refDocNodeDatabase, CCanNode& refDocNode, 
											CNodeDatabase& refRuntimeNodeDatabase, CCanNode& refRuntimeNode,
											int& lastUnitAddress) 
{
	lastUnitAddress = 0;	// Deze bevat het laatste unit adres van de doc node (!= het virtuele unit address.)

	BOOL foundVirtualUnit = FALSE;
	const int numberOfDocumentUnits = refDocNode.bNROfUnits;
	for (int i=0;i<numberOfDocumentUnits;i++) {
		STRUCT_UNIT* pDocUnit = NULL;
		if (refDocNodeDatabase.GetUnit(&refDocNode,i,&pDocUnit) == FALSE) {
			return FALSE;	// Unit niet gevonden.
		}

		// lastUnitAddress = laatste unit address van de doc node met uitzondering van 0xff.
		if ((pDocUnit->bUnitAddress != UNIT_VIRTUAL_BROADCAST_ADDRESS) && (pDocUnit->bUnitAddress >= lastUnitAddress)) {
			lastUnitAddress = pDocUnit->bUnitAddress;
		}

		if (pDocUnit->bUnitAddress == UNIT_VIRTUAL_BROADCAST_ADDRESS) {
			foundVirtualUnit = TRUE;
		}

		// Oppassen:
		// Runtime node bevat ook een unitAddress 0xFF (wanneer deze ingeladen werd in een bestaand project.)
		int dummy = 0;
		STRUCT_UNIT* pRTUnit = NULL;
		if (FALSE == refRuntimeNodeDatabase.FindUnit(&refRuntimeNode, pDocUnit->bUnitAddress, &dummy, &pRTUnit)) {
			if (pDocUnit->bUnitAddress != UNIT_VIRTUAL_BROADCAST_ADDRESS) {
				return FALSE;
			}
			foundVirtualUnit = TRUE;		// pDocUnit->bUnitAddress = 0xFF
		}

		if (NULL != pRTUnit) {
			// Zowel de doc & runtime units werden gevonden.
			// Controleren of ze dezelfde unit types hebben.
			INVARIANT(pDocUnit->bUnitAddress == pRTUnit->bUnitAddress);
			if (pDocUnit->bUnitType != pRTUnit->bUnitType) {
				return FALSE;
			}
		}
	}
	ASSERT(lastUnitAddress<UNIT_VIRTUAL_BROADCAST_ADDRESS);
	if (foundVirtualUnit) {
		ASSERT(lastUnitAddress == (refDocNode.bNROfUnits-2));	// NOTE_1
	}
	else {
		ASSERT(lastUnitAddress == (refDocNode.bNROfUnits-1));	// NOTE_2
	}
	return TRUE;									
}



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CAppNodeDatabase* CAppNodeDatabase::GetInstance()
{
	static CAppNodeDatabase instance;
	return (&instance);
}
//////////////////////////////////////////////////////////////////////
CAppNodeDatabase::CAppNodeDatabase() : m_pRuntime(NULL), m_pDocument(NULL), m_mapUnitInfoLabels()
{
	try
	{	
		m_pRuntime = new CNodeDatabase();			
		m_pDocument = new CNodeDatabase();				
	}
	catch (CMemoryException *e)
	{
		e->ReportError();
		e->Delete();
		exit(-1);
	}
}

CAppNodeDatabase::~CAppNodeDatabase()
{
	if (m_pRuntime != NULL) {
		m_pRuntime->ClearDatabase();
		delete m_pRuntime ;
		m_pRuntime = NULL;
	}

	if (m_pDocument != NULL) {
		m_pDocument->ClearDatabase();
		delete m_pDocument;
		m_pDocument = NULL;
	}
}

/////////////////////////////////////////////////////////////////////
/**
 *	add New nodes from runtime to document node database ...
 */
void CAppNodeDatabase::AddNewNodesToRTDatabase(CList<int>* pListNodes)
{
	if (0 == pListNodes) {
		ASSERT(0 != pListNodes);
		return;
	}

	if (!pListNodes->IsEmpty()) {
		pListNodes->RemoveAll();
	}

	CCanNode *pRTNode,*pDocNode;
	STRUCT_UNIT *pRTUnit,*pDocUnit;
	try {
		if ((m_pDocument == NULL) || (m_pRuntime == NULL)) {		
			CExceptionNodeDatabaseInterface *Exception = new CExceptionNodeDatabaseInterface(IDS_EXCEPTION_PTRTONULL);
			throw Exception;
		}

		// search for nodes to add from RT --> DOC database ...
		int nRTCount;
		m_pRuntime->NROfNodes(&nRTCount);
		for (int i=0;i<nRTCount;i++) {
			pRTNode = NULL;			
			int k;
			m_pRuntime->GetNode(i,&pRTNode);
			if (pRTNode == NULL) {
				CExceptionNodeDatabaseInterface *Exception = new CExceptionNodeDatabaseInterface(IDS_EXCEPTION_GETNODE);
				throw Exception;
			}

			if (m_pDocument->FindNode(pRTNode->bLogicalAddress,&k,&pDocNode) == FALSE) {			
				// node not found in Document database, Add Node to DOC-Database ... 
				if(m_pDocument->AddNode(pRTNode->bLogicalAddress,&k,&pDocNode) == FALSE) {
					CExceptionNodeDatabaseInterface *Exception = new CExceptionNodeDatabaseInterface(IDS_EXCEPTION_ADDNODE);
					throw Exception;
				}
				if (pDocNode == NULL) {
					CExceptionNodeDatabaseInterface *Exception = new CExceptionNodeDatabaseInterface(IDS_EXCEPTION_PTRTONULL);
					throw Exception;
				}
			
				// Copy node 
				m_pDocument->CopyNodeObject(pDocNode,pRTNode);				
				
				for (int j=0;j<pRTNode->iNROfUnits;j++) {
					if (m_pRuntime->GetUnit(pRTNode,j,&pRTUnit) == FALSE) {					
						CExceptionNodeDatabaseInterface *Exception = new CExceptionNodeDatabaseInterface(IDS_EXCEPTION_GETUNIT);
						throw Exception;
					}

					if (m_pDocument->AddUnit(pDocNode,j,&k,&pDocUnit)== FALSE) {					
						CExceptionNodeDatabaseInterface *Exception =  new CExceptionNodeDatabaseInterface(IDS_EXCEPTION_ADDUNIT);
						throw Exception;
					}

					if ((pDocUnit == NULL) || (pRTUnit == NULL))  {
						CExceptionNodeDatabaseInterface *Exception = new CExceptionNodeDatabaseInterface(IDS_EXCEPTION_PTRTONULL);
						throw Exception;
					}
					CNodeDatabase::CopyUnitObject(pDocUnit,pRTUnit);				
				} // end copy units + add units ...
				
				// Create Broadcast (All Room) units.
				ASSERT(0 != pDocNode);
				CreateBroadcastBindingUnits(m_pDocument,pDocNode);							
				CreateBindingUnitProperties(m_pDocument,pDocNode);

				// Node toevoegen aan de lijst...
				ASSERT(0 != pListNodes);
				pListNodes->AddTail(pDocNode->getNodeAddress());			
			}		 				
		} // for all RTNODES ...

		if (1)
		{
			int i;
			m_pDocument->NROfNodes(&i);
			if (nRTCount != i) {		
				CExceptionNodeDatabaseInterface *Exception = new CExceptionNodeDatabaseInterface(IDS_EXCEPTION_NODESNOTEQUAL);
				throw Exception;			
			}	
		}
	}
	
	catch (CExceptionNodeDatabaseInterface *Exception) {
		AfxMessageBox(Exception->GetError(),MB_ICONSTOP);		
		ASSERT(0);	
		throw;
	}
}

/**
 *	Update Document Nodedatabase with runtime properties.
 *
 *  BUG-0130
 *
 *  Sedert V16.38: Upgraden van een smartbox plus naar een Ultra.
 *  Bij de controle van incompatible nodes is het bij de smartbox plus / smartbox ultra toegelaten om een verschillende node type te hebben.
 *	Daarom wordt hier ook het nodetype gekopieerd.
 */
void CAppNodeDatabase::UpdateNodesWithRTSpecs(BOOL copyNames)
{
	CCanNode *pDocNode,*pRTNode;
	int t;
	try 
	{
		if ((m_pDocument == NULL) || (m_pRuntime == NULL)) {		
			CExceptionNodeDatabaseInterface *Exception = new CExceptionNodeDatabaseInterface(IDS_EXCEPTION_PTRTONULL);
			throw Exception;
		}
		int nDocCount;
		m_pDocument->NROfNodes(&nDocCount);	
		for (int i=0;i<nDocCount;i++) {
			if (m_pDocument->GetNode(i,&pDocNode) == FALSE) {	
				CExceptionNodeDatabaseInterface *Exception = new CExceptionNodeDatabaseInterface(IDS_EXCEPTION_GETNODE);
				throw Exception;
			}
			if (m_pRuntime->FindNode(pDocNode->bLogicalAddress,&t,&pRTNode) == FALSE) {
				CExceptionNodeDatabaseInterface *Exception =  new CExceptionNodeDatabaseInterface(IDS_EXCEPTION_FINDNODE);
				throw Exception;
			}
			if ((pDocNode == NULL) || (pRTNode == NULL)) {
				CExceptionNodeDatabaseInterface *Exception = new CExceptionNodeDatabaseInterface(IDS_EXCEPTION_PTRTONULL);
				throw Exception;
			}

			static const BOOL flagAddNewUnit_NO = FALSE;
			this->UpdateNodesWithRTSpecs(pDocNode,pRTNode,copyNames,flagAddNewUnit_NO);
		} // for all nodes ...	
	} // end try ...
	
	catch (CExceptionNodeDatabaseInterface *Exception) {
		AfxMessageBox(Exception->GetError(),MB_ICONSTOP);		
		ASSERT(0);	
		throw;
	}
}

BOOL CAppNodeDatabase::UpdateNodesWithRTSpecs(BYTE bNodeAddress, BOOL copyNames, BOOL addNewUnits) {
	if ((m_pDocument == NULL) || (m_pRuntime == NULL)) {	
		INVARIANT(NULL != m_pDocument);
		INVARIANT(NULL != m_pRuntime);
		return FALSE;
	}
	int dummy = 0;
	CCanNode *pDocNode = NULL;
	if (FALSE == m_pDocument->FindNode(bNodeAddress,&dummy,&pDocNode)) {
		return FALSE;
	}
	CCanNode *pRTNode = NULL;	
	if (FALSE == m_pRuntime->FindNode(bNodeAddress,&dummy,&pRTNode)) {
		return FALSE;
	}
	return this->UpdateNodesWithRTSpecs(pDocNode,pRTNode,copyNames,addNewUnits);
}

BOOL CAppNodeDatabase::UpdateNodesWithRTSpecs(CCanNode* pDocNode, CCanNode* pRTNode, BOOL copyNames, BOOL addNewUnits) {	
	if (NULL == pDocNode || NULL == pRTNode) {
		ASSERT(NULL != pDocNode);
		ASSERT(NULL != pRTNode);
		return FALSE;
	}
	if (copyNames) {					
		if (strcmp(pDocNode->szNodeName, pRTNode->szNodeName) != 0) {
			strcpy(pDocNode->szNodeName, pRTNode->szNodeName);
			pDocNode->szNodeName[NODE_NAME_LENGTH-1] = '\0';
		}
	}

	BOOL result = TRUE;
	pDocNode->ulPhysicalAddress = pRTNode->ulPhysicalAddress;
	pDocNode->bNodeFlags = pRTNode->bNodeFlags;
	pDocNode->bDLLAccessControl = pRTNode->bDLLAccessControl;			
	pDocNode->bNodeType = pRTNode->bNodeType;							// TM,V16.36: Nodig voor de smartbox ultra.
						
	memcpy(pDocNode->pbResourcesBlock0, pRTNode->pbResourcesBlock0, RESOURCES_BLOCK0_LENGTH);
	memcpy(pDocNode->pbResourcesBlock1, pRTNode->pbResourcesBlock1, RESOURCES_BLOCK1_LENGTH);
	memcpy(pDocNode->pbResourcesBlock2, pRTNode->pbResourcesBlock2, RESOURCES_BLOCK2_LENGTH);		
	memcpy(pDocNode->pbResourcesBlock3, pRTNode->pbResourcesBlock3, RESOURCES_BLOCK3_LENGTH);
	memcpy(pDocNode->pbResourcesBlock4, pRTNode->pbResourcesBlock4, RESOURCES_BLOCK4_LENGTH);
	
	{
		const int NrOfUnits = pDocNode->getNROfUnits( );
		for (int i=0;i<NrOfUnits;i++) {
			STRUCT_UNIT* pDocUnit = NULL;			
			if (m_pDocument->GetUnit(pDocNode,i,&pDocUnit)) {
				STRUCT_UNIT* pRTUnit = NULL;
				if (m_pRuntime->GetUnit(pRTNode,i,&pRTUnit)) {
					if (pDocUnit->bUnitType == pRTUnit->bUnitType) {
						// Sedert V16.43: Voor alle types de unitspecs copieren
						memcpy(&pDocUnit->UnitSpecs,&pRTUnit->UnitSpecs,sizeof(UNION_UNITSPECS));		

#if(0)
						switch (pDocUnit->bUnitType) {								
							case UNIT_TYPE_AUDIO:
							case UNIT_TYPE_AUDIO_HC4: {
								// TM_AUDIOHC4_BINDING_INPUT
								// Audio Extended: 
								// bAudioConfig: Configuratie via multimedia bestand.
								// Type bepaalt welke binding unit aangemaakt wordt.
								pDocUnit->UnitSpecs.AudioUnit.bAudioConfig = pRTUnit->UnitSpecs.AudioUnit.bAudioConfig;
								pDocUnit->UnitSpecs.AudioUnit.bAudioStatus1 = pRTUnit->UnitSpecs.AudioUnit.bAudioStatus1;
								pDocUnit->UnitSpecs.AudioUnit.bAudioStatus2 = pRTUnit->UnitSpecs.AudioUnit.bAudioStatus2;
							} break;		
						}						
#endif
					}
					else {						
						ASSERT(0); // Probleem: units zijn niet van hetzelfde type ...
						result = FALSE;
					}
				}
			}
		}			
	}

	if ((TRUE == addNewUnits) && (pRTNode->getNROfUnits() > pDocNode->getNROfUnits( ))) {
		// Nieuwe units van de runtime node toevoegen aan de document node.
		const int NrOfUnits = pRTNode->getNROfUnits( );
		for (int unitAddress=pDocNode->getNROfUnits( );unitAddress<NrOfUnits;++unitAddress) {
			STRUCT_UNIT* pRTUnit = NULL;
			if (m_pRuntime->GetUnit(pRTNode,unitAddress,&pRTUnit)) {
				STRUCT_UNIT* pDocUnit = NULL;	
				int dummy = 0;
				if (m_pDocument->AddUnit(pDocNode,unitAddress,&dummy,&pDocUnit)) {
					helper_CopyRuntimeUnitToDocUnit(pDocNode, pRTUnit, pDocUnit);
				}	
				else { 
					ASSERT(0); 
					result = FALSE;
				}
			}	
			else { 
				ASSERT(0); 
				result = FALSE;
			}
		}	
	}
	return result;
}


/**
 * Update Document node with rt specs
 */
BOOL CAppNodeDatabase::UpdateNodeWithRTSpecs(BYTE bNodeAddress,BOOL* pStatusChanged)
{
	ASSERT(m_pDocument);
	ASSERT(m_pRuntime);

	int i;
	CCanNode *pDocNode;
	if (m_pDocument->FindNode(bNodeAddress,&i,&pDocNode) == TRUE)  {
		CCanNode* pRTNode;
		if (m_pRuntime->FindNode(bNodeAddress,&i,&pRTNode) == TRUE)  {	
			const int bindingNrOfEntries = pRTNode->getNodeBindingsNrEntries();
			const int bindingXORChecksum = pRTNode->getNodeBindingsXORChecksum();

#if(ENABLE_TRACE_BINDINGFILEINFO_INFO == 1)
			CString strInfoMessage;
			strInfoMessage.Format("Node 0x%02x: Updating node with runtime specs. Nr of Bindings=%d - XOR=0x%02x.",
					pDocNode->getNodeAddress(), bindingNrOfEntries,bindingXORChecksum);
			AppLogging::getInstance().addInfo(strInfoMessage);
#endif

			if (pDocNode->getNodeBindingsNrEntries() != bindingNrOfEntries) {
				pDocNode->setNodeBindingsNrEntries(bindingNrOfEntries);
				*pStatusChanged = TRUE;
			}
			
			if (pDocNode->getNodeBindingsXORChecksum() != bindingXORChecksum) {
				pDocNode->setNodeBindingsXORChecksum(bindingXORChecksum);	
				*pStatusChanged = TRUE;
			}
			
			if (pDocNode->bDLLAccessControl != pRTNode->bDLLAccessControl) {
				pDocNode->bDLLAccessControl = pRTNode->bDLLAccessControl;	
				*pStatusChanged = TRUE;
			}				
			pDocNode->CopyRuntimeStatus(pRTNode);	// Can-bus status: 'pStatusChanged' niet aanpassen.
			return TRUE;
		}
	}
	return FALSE;
}

CAppDbasePostBrowseError::Result CAppNodeDatabase::CompareNodeDatabases() const
{	
	try 
	{
		if ((m_pDocument == NULL) || (m_pRuntime == NULL)) 
		{
			CExceptionNodeDatabaseInterface *Exception = 
				new CExceptionNodeDatabaseInterface(IDS_EXCEPTION_PTRTONULL);
			throw Exception;
		}

		CAppDbasePostBrowseError::Result result = CAppDbasePostBrowseError::DBASE_EC_COMPARE_SUCCESS;

		// Nodes verwijderd...
		result = helper_CompareNodeDatabases_NodesRemoved( );
		if (CAppDbasePostBrowseError::DBASE_EC_COMPARE_SUCCESS != result) {
			return result;
		}

		// Incompatible nodes ?
		result = helper_CompareNodeDatabases_NodesIncompatible();
		if (CAppDbasePostBrowseError::DBASE_EC_COMPARE_SUCCESS != result) {
			return result;
		}
				
		// Nodes toegevoegd ?
		result = helper_CompareNodeDatabases_NodesAdded();
		if (CAppDbasePostBrowseError::DBASE_EC_COMPARE_SUCCESS != result) {
			return result;
		}
		
		// Controle van de XOR program file differences.
		result = helper_CompareNodeDatabases_ConfigDifferences();
		if (CAppDbasePostBrowseError::DBASE_EC_COMPARE_SUCCESS != result) {
			return result;
		}
			
		// Als laatste controle op de naam verschillen...
		result = helper_CompareNodeDatabases_NameDifferences( );
		if (CAppDbasePostBrowseError::DBASE_EC_COMPARE_SUCCESS != result) {
			return result;
		}				
		return result;		
	}
	
	catch (CExceptionNodeDatabaseInterface *Exception)
	{
		AfxMessageBox(Exception->GetError(),MB_ICONSTOP);		
		ASSERT(0);	
		return (CAppDbasePostBrowseError::DBASE_EC_NODES_INCOMPATIBLE);		
	}
}


// Controleren of alle bestaande units van hetzelfde type zijn.
CAppDbasePostBrowseError::Result CAppNodeDatabase::CompareNode(int nodeAddress) const {
	if ((this->m_pDocument == NULL) || (this->m_pRuntime == NULL)) {
		ASSERT(this->m_pDocument != NULL);
		ASSERT(this->m_pRuntime != NULL);
		return (CAppDbasePostBrowseError::DBASE_EC_NODES_INCOMPATIBLE);	
	}

	int dummy = 0;
	CCanNode* pDocNode = NULL;
	if (FALSE == m_pDocument->FindNode(nodeAddress,&dummy,&pDocNode)) {
		ASSERT(0);
		return (CAppDbasePostBrowseError::DBASE_EC_NODES_INCOMPATIBLE);	
	}
	ASSERT(NULL != pDocNode);
	ASSERT(pDocNode->bLogicalAddress == nodeAddress);

	CCanNode* pRTNode = NULL;
	if (FALSE == m_pRuntime->FindNode(pDocNode->bLogicalAddress,&dummy,&pRTNode))  {
		ASSERT(0);
		return (CAppDbasePostBrowseError::DBASE_EC_NODES_INCOMPATIBLE);	
	}
	ASSERT(NULL != pRTNode);
	ASSERT(pRTNode->bLogicalAddress == nodeAddress);

	INVARIANT(pDocNode->bLogicalAddress == pRTNode->bLogicalAddress);
			
	// Controle node type (en product id.)
	// Wanneer de types niet ok zijn moeten we niet verder controleren.
	// Verschillende node types zijn niet toegelaten.
	if (FALSE == helper_compare_nodeType(*pDocNode,*pRTNode)) {
		return (CAppDbasePostBrowseError::DBASE_EC_NODES_INCOMPATIBLE);	
	}	

	// Controleren op de units van de node.
	if (pDocNode->bNROfUnits > pRTNode->bNROfUnits) {
		// if Doc. node holds more units then runtime node check that it's not only the virtual unit.	
		if (FALSE == helper_compare_nodeHoldsVirtualUnits(*m_pDocument,*pDocNode,*pRTNode)) {								
			return (CAppDbasePostBrowseError::DBASE_EC_NODES_INCOMPATIBLE);	
		}			
	}
		
	if (pDocNode->bNROfUnits > 0) {		
		// Wanneer er units aanwezig zijn van verschillende types hebben we incompatible nodes.
		int dummy = 0;
		if (FALSE == helper_compare_unitsCompactibel(*m_pDocument,*pDocNode,*m_pRuntime,*pRTNode,dummy)) {
			return (CAppDbasePostBrowseError::DBASE_EC_NODES_INCOMPATIBLE);
		}					
	}						
	return CAppDbasePostBrowseError::DBASE_EC_COMPARE_SUCCESS;
}


CAppDbasePostBrowseError::Result CAppNodeDatabase::helper_CompareNodeDatabases_NodesRemoved( ) const {

	CAppDbasePostBrowseError::Result result = CAppDbasePostBrowseError::DBASE_EC_COMPARE_SUCCESS;
	try 
	{
		int nDocCount = 0;
		int i;

		m_pDocument->NROfNodes(&nDocCount);

		// TM 20070927: eerst Controleren of alle nodes die in de doc nodedatabase zijn 
		// ook aanwezig zijn in de RT database anders moet 
		// een NODES NOT FOUND terug verstuurd worden.

		// 1) Check if all nodes in doc. database can be found in runtime database.
		for (i=0;i<nDocCount;i++) 
		{
			CCanNode* pDocNode = NULL;
			CCanNode* pRTNode = NULL;
			int j=0;

			if (m_pDocument->GetNode(i,&pDocNode) == FALSE) {
				CExceptionNodeDatabaseInterface *Exception = 
					new CExceptionNodeDatabaseInterface(IDS_EXCEPTION_GETNODE);
				throw Exception;
			}

			if (m_pRuntime->FindNode(pDocNode->bLogicalAddress,&j,&pRTNode) == FALSE) {
				return (CAppDbasePostBrowseError::DBASE_EC_NODES_NOT_FOUND);
			}
		}

		if (1)
		{
			// Controle op aantal nodes.
			// Indien er meer document nodes zijn dan runtime nodes moest dit al
			// voordien gedecteerd zijn. (Zie ASSERT(0)).
			int tempCount = 0;
			if (TRUE==m_pRuntime->NROfNodes(&tempCount)) {
				if (nDocCount>tempCount) {					
					ASSERT(0);	// Dit kan in principe nooit voorkomen.
					return (CAppDbasePostBrowseError::DBASE_EC_NODES_NOT_FOUND);
				}
			}
		}
	}
	catch (CExceptionNodeDatabaseInterface *Exception)
	{
		AfxMessageBox(Exception->GetError(),MB_ICONSTOP);		
		ASSERT(0);	
		result = CAppDbasePostBrowseError::DBASE_EC_NODES_INCOMPATIBLE;		
	}
	return result;
}

CAppDbasePostBrowseError::Result CAppNodeDatabase::helper_CompareNodeDatabases_NodesIncompatible( ) const
{			
	CAppDbasePostBrowseError::Result result = CAppDbasePostBrowseError::DBASE_EC_COMPARE_SUCCESS;	
	try 
	{
		int nDocCount = 0;
		m_pDocument->NROfNodes(&nDocCount);

		// Iteratie van alle nodes.
		for (int i=0;i<nDocCount;i++) 
		{			
			int dummy = 0;
			CCanNode* pDocNode = NULL;
			CCanNode* pRTNode = NULL;

			if (m_pDocument->GetNode(i,&pDocNode) == FALSE) {
				CExceptionNodeDatabaseInterface *Exception = new CExceptionNodeDatabaseInterface(IDS_EXCEPTION_GETNODE);
				throw Exception;
			}
			if (m_pRuntime->FindNode(pDocNode->bLogicalAddress,&dummy,&pRTNode) == FALSE)  {
				CExceptionNodeDatabaseInterface *Exception = new CExceptionNodeDatabaseInterface(IDS_EXCEPTION_FINDNODE);
				throw Exception;
			}
				
			ASSERT((NULL != pDocNode) && (NULL != pRTNode));
			ASSERT(pDocNode->bLogicalAddress == pRTNode->bLogicalAddress);


			// Controle node type (en product id.)
			// Wanneer de types niet ok zijn moeten we niet verder controleren.
			// Verschillende node types zijn niet toegelaten.
			if (FALSE == helper_compare_nodeType(*pDocNode,*pRTNode)) {
				return (CAppDbasePostBrowseError::DBASE_EC_NODES_INCOMPATIBLE);	
			}	
			
			// Controleren op de units van de node.
			if (pDocNode->bNROfUnits > pRTNode->bNROfUnits) {
				// if Doc. node holds more units then runtime node check that it's not
				// only a virtual binding unit created ...
				if (FALSE == helper_compare_nodeHoldsVirtualUnits(*m_pDocument,*pDocNode,*pRTNode)) {								
					return (CAppDbasePostBrowseError::DBASE_EC_NODES_INCOMPATIBLE);	
				}			
			}
			else if (pDocNode->bNROfUnits == pRTNode->bNROfUnits) {
				// if Doc Node & runtime node have same nr. of units check if all units are of same type ...
				// Wanneer er units aanwezig zijn van verschillende types hebben we incompatible nodes.
				int dummy = 0;
				if (FALSE == helper_compare_unitsCompactibel(*m_pDocument,*pDocNode,*m_pRuntime,*pRTNode,dummy)) {
					return (CAppDbasePostBrowseError::DBASE_EC_NODES_INCOMPATIBLE);
				}					
			}				
			else 
			{
				// Runtime node holds more units then doc. node ...
				ASSERT(pRTNode->bNROfUnits > pDocNode->bNROfUnits);
													
				// We check every unit of the doc. node and compare with runtime unit
				// we must skip the virtual binding unit if one is created ...				
				int docNodeLastUnitAddress = 0;	// Verwijst naar het laatste unit address van de node (exclusief de 0xFF).
				int firstUnitAddress = 0;		// Verwijst naar het eerste unit address van de runtime node.
				if (pDocNode->bNROfUnits > 0)	{
					if (FALSE == helper_compare_unitsCompactibel(*m_pDocument,*pDocNode,*m_pRuntime,*pRTNode,docNodeLastUnitAddress)) {
						return (CAppDbasePostBrowseError::DBASE_EC_NODES_INCOMPATIBLE);
					}		
					firstUnitAddress=docNodeLastUnitAddress+1;	// Eerste runtime unit address.
				}

				// Wanneer de runtime node meer units heeft dan de doc node dan gaan we nieuwe units 
				// aanmaken in de doc node en de properties copieren...
				for (int unitAddress=firstUnitAddress;unitAddress<pRTNode->bNROfUnits;unitAddress++) {	
					STRUCT_UNIT* pRTUnit = NULL;
					if (FALSE == m_pRuntime->GetUnit(pRTNode,unitAddress,&pRTUnit)) {
						ASSERT(0);
						return (CAppDbasePostBrowseError::DBASE_EC_NODES_INCOMPATIBLE);
					}				
					ASSERT(NULL != pRTUnit);
					ASSERT(unitAddress == pRTUnit->bUnitAddress);

					// Unit toevoegen aan de database node + copieren van de properties.
					int dummy = 0;		
					STRUCT_UNIT* pDocUnit = NULL;	
					if(FALSE == m_pDocument->AddUnit(pDocNode,pRTUnit->bUnitAddress,&dummy,&pDocUnit)) {							
						CExceptionNodeDatabaseInterface *Exception = new CExceptionNodeDatabaseInterface(IDS_EXCEPTION_ADDUNIT);
						throw Exception;
					}
					ASSERT(NULL != pDocUnit);
					ASSERT(pRTUnit->bUnitAddress == pDocUnit->bUnitAddress);
					
					helper_CopyRuntimeUnitToDocUnit(pDocNode, pRTUnit, pDocUnit);						
				}

				// Het aantal units moet gelijk zijn van beide nodes. (uitz wanneer er een virtuele unit aanwezig is.)
				ASSERT((pRTNode->bNROfUnits == pDocNode->bNROfUnits) || (pRTNode->bNROfUnits == pDocNode->bNROfUnits+1));
			}	
		}
	}
	catch (CExceptionNodeDatabaseInterface *Exception) {
		AfxMessageBox(Exception->GetError(),MB_ICONSTOP);		
		ASSERT(0);	
		result = CAppDbasePostBrowseError::DBASE_EC_NODES_INCOMPATIBLE;		
	}
	return result;
}

CAppDbasePostBrowseError::Result CAppNodeDatabase::helper_CompareNodeDatabases_NodesAdded( ) const {

	int nDocCount = 0;
	int nRTCount = 0;

	m_pDocument->NROfNodes(&nDocCount);
	m_pRuntime->NROfNodes(&nRTCount);

	if (nDocCount<nRTCount) 
	{
		return (CAppDbasePostBrowseError::DBASE_EC_NODES_ADDED);
	}
	return CAppDbasePostBrowseError::DBASE_EC_COMPARE_SUCCESS;
}


CAppDbasePostBrowseError::Result CAppNodeDatabase::helper_CompareNodeDatabases_ConfigDifferences( ) const {

	CAppDbasePostBrowseError::Result result = CAppDbasePostBrowseError::DBASE_EC_COMPARE_SUCCESS;
	try 
	{
		int i = 0;
		int j = 0;
		int nDocCount = 0;

		m_pDocument->NROfNodes(&nDocCount);

		for (i=0;i<nDocCount;i++) 
		{
			CCanNode* pDocNode = NULL;
			CCanNode* pRTNode = NULL;
			
			if (m_pDocument->GetNode(i,&pDocNode) == FALSE) {			
				CExceptionNodeDatabaseInterface *Exception = new CExceptionNodeDatabaseInterface(IDS_EXCEPTION_GETNODE);
				throw Exception;
			}

			if (m_pRuntime->FindNode(pDocNode->bLogicalAddress,&j,&pRTNode) == FALSE) {			
				CExceptionNodeDatabaseInterface *Exception = new CExceptionNodeDatabaseInterface(IDS_EXCEPTION_FINDNODE);
				throw Exception;
			}

			if ((pDocNode->getNodeBindingsXORChecksum() != pRTNode->getNodeBindingsXORChecksum()) || 
				(pDocNode->getNodeBindingsNrEntries() != pRTNode->getNodeBindingsNrEntries())) 
			{
				return (CAppDbasePostBrowseError::DBASE_EC_PROGRAMFILE_DIFFERENCE);
			}
		}// end for loop ..

		result = CAppDbasePostBrowseError::DBASE_EC_COMPARE_SUCCESS;
	}
	catch (CExceptionNodeDatabaseInterface *Exception)
	{
		AfxMessageBox(Exception->GetError(),MB_ICONSTOP);		
		ASSERT(0);	
		result = CAppDbasePostBrowseError::DBASE_EC_NODES_INCOMPATIBLE;		
	}
	return result;
}

// Helperfunctie voor de compare node databases...
CAppDbasePostBrowseError::Result CAppNodeDatabase::helper_CompareNodeDatabases_NameDifferences( ) const {

	CAppDbasePostBrowseError::Result result = CAppDbasePostBrowseError::DBASE_EC_COMPARE_SUCCESS;

	try 
	{
		int i = 0;
		int j = 0;
		int nDocCount = 0;

		m_pDocument->NROfNodes(&nDocCount);

		for (i=0;i<nDocCount;i++) 
		{		
			CCanNode* pDocNode = NULL;
			CCanNode* pRTNode = NULL;
			STRUCT_UNIT* pDocUnit = NULL;
			STRUCT_UNIT* pRTUnit = NULL;

			if (m_pDocument->GetNode(i,&pDocNode) == FALSE) 
			{			
				CExceptionNodeDatabaseInterface *Exception = 
					new CExceptionNodeDatabaseInterface(IDS_EXCEPTION_GETNODE);
				throw Exception;
			}

			if (m_pRuntime->FindNode(pDocNode->bLogicalAddress,&j,&pRTNode) == FALSE) 
			{			
				CExceptionNodeDatabaseInterface *Exception = 
					new CExceptionNodeDatabaseInterface(IDS_EXCEPTION_FINDNODE);
				throw Exception;
			}
		
			// now we must look for all units in network database if name is the
			// same as in de database node ...
			// There can be more Network units then there are Database units !!!!
			for (j=0;j<pRTNode->bNROfUnits;j++) 
			{		
				pRTUnit = NULL;
				
				if (m_pRuntime->GetUnit(pRTNode,j,&pRTUnit) == FALSE) 
				{						
					CExceptionNodeDatabaseInterface *Exception = 
						new CExceptionNodeDatabaseInterface(IDS_EXCEPTION_GETUNIT);
					throw Exception;
				}

				pDocUnit = NULL;

				if (m_pDocument->GetUnit(pDocNode,j,&pDocUnit)) 
				{				
					if ((pDocUnit != NULL) && (pRTUnit != NULL)) 
					{
						if (strcmp(pDocUnit->szUnitName,pRTUnit->szUnitName)!=0) 
						{
							// Sedert versie 0x0B00
							// Zie BUG-0147
							// Omdat we anders telkens naam verschillen hebben.

							if ((pDocUnit->szUnitName[0]==0)&&
								(pRTUnit->szUnitName[0]==' ')&&((pRTUnit->szUnitName[1]==0)))
							{						
								TRACE("Aborbe Name differences\r\n");
							}
							else {
								return (CAppDbasePostBrowseError::DBASE_EC_NAMES_DIFFERENCE);
							}
						}					
					}				
				}
			}
		}
		result = CAppDbasePostBrowseError::DBASE_EC_COMPARE_SUCCESS;
	}
	catch (CExceptionNodeDatabaseInterface *Exception)
	{
		AfxMessageBox(Exception->GetError(),MB_ICONSTOP);		
		ASSERT(0);	
		result = CAppDbasePostBrowseError::DBASE_EC_NODES_INCOMPATIBLE;		
	}
	return result;
}




/**
 * Copy RT Nodedatabase to document database ...
 */
void CAppNodeDatabase::CopyRuntimeToDocDatabase()
{
	ASSERT( m_pDocument );
	ASSERT( m_pRuntime );

	if (m_pDocument->CopyDatabase(m_pDocument,m_pRuntime) != 0) 
	{	
		CreateBroadcastBindingUnits(m_pDocument);		
		CreateBindingUnitProperties(m_pDocument);	

		// Na de copy alle units van de runtime datase de pointer naar pBindingUnit op NULL plaatsen.
		DeleteRuntimeNodesBindingUnitPropPtr(m_pRuntime);	
	}
	else
	{
		CString szError(LPCSTR(IDS_EXCEPTION_DBASECOPYRT));
		AfxMessageBox(szError,MB_ICONSTOP);	

		CString szTerminate(LPCSTR(IDS_EXCEPTION_CANNOTCONTINUE));
		AfxMessageBox(szTerminate,MB_ICONSTOP);
		exit(-1);
	}
}

void CAppNodeDatabase::CopyDocumentToRuntimeDatabase()
{
	if (this->m_pDocument == NULL || this->m_pRuntime == NULL) {
		INVARIANT( NULL != m_pDocument );
		INVARIANT( NULL != m_pRuntime );
		return;
	}
	
	if (m_pRuntime->CopyDatabase(m_pRuntime,m_pDocument) != 0) {
		CreateBroadcastBindingUnits(m_pDocument);		
		// Na de copy alle units van de runtime datase de pointer naar pBindingUnit op NULL plaatsen.
		DeleteRuntimeNodesBindingUnitPropPtr(m_pRuntime);	
	}
	else
	{
		CString szError(LPCSTR(IDS_EXCEPTION_DBASECOPYRT));
		AfxMessageBox(szError,MB_ICONSTOP);	
		
		CString szTerminate(LPCSTR(IDS_EXCEPTION_CANNOTCONTINUE));
		AfxMessageBox(szTerminate,MB_ICONSTOP);
		exit(-1);
	}
}

void CAppNodeDatabase::CopyDocumentNodeToRuntimeNode(int nodeAddress) {	
	REQUIRE(nodeAddress >= 0 && nodeAddress <= 0xff);

	if (this->m_pDocument == NULL || this->m_pRuntime == NULL) {
		INVARIANT( NULL != m_pDocument );
		INVARIANT( NULL != m_pRuntime );
		return;
	}
	
	int dummy = 0;
	CCanNode* pDocNode = NULL;
	if (FALSE == this->m_pDocument->FindNode(nodeAddress,&dummy,&pDocNode)) {
		return;
	}
	CCanNode* pRuntimeNode = NULL;
	if (FALSE == this->m_pRuntime->FindNode(nodeAddress,&dummy,&pRuntimeNode)) {
		return;
	}
	INVARIANT((NULL != pDocNode) && (NULL != pRuntimeNode));
	CopyDocumentNodeToRuntimeNode(pRuntimeNode,*pDocNode);
}

void CAppNodeDatabase::CopyDocumentNodeToRuntimeNode(CCanNode* pDestRTNode,CCanNode& refDocumentNode) {
	if (NULL == pDestRTNode) {
		REQUIRE(NULL != pDestRTNode);
		return;
	}
	if (this->m_pDocument == NULL || this->m_pRuntime == NULL) {
		INVARIANT( NULL != m_pDocument );
		INVARIANT( NULL != m_pRuntime );
		return;
	}

	// Wanneer de runtime node meer units bevat dan de document node dan moeten deze gewist worden...
	if (pDestRTNode->getNROfUnits() > refDocumentNode.getNROfUnits()) {	
		do {
			const int unitAddress = pDestRTNode->getNROfUnits() - 1;
			if (unitAddress >= 0) {
				if (false == this->m_pRuntime->DeleteUnitNR(pDestRTNode,unitAddress)) {
					break;
				}
			}		
		} while(pDestRTNode->getNROfUnits() >= refDocumentNode.getNROfUnits());
		ASSERT(pDestRTNode->getNROfUnits() <= refDocumentNode.getNROfUnits());
	}	

	// Copy Node Object
	this->CopyNodeObject(pDestRTNode,&refDocumentNode);		

	// Alle units copieren.
	// Wanneer er units te kort zijn dan worden deze opnieuw aangemaakt.
	const int iNrOfUnits = refDocumentNode.iNROfUnits;		
	for (int i=0;i<iNrOfUnits;i++) {
		STRUCT_UNIT* pSourceUnit = NULL;			
		if (this->m_pDocument->GetUnit(&refDocumentNode,i,&pSourceUnit)) {
			ASSERT(NULL != pSourceUnit);	
			int dummy = 0;
			STRUCT_UNIT* pDestUnit = NULL;
			if (FALSE == this->m_pRuntime->FindUnit(pDestRTNode,pSourceUnit->bUnitAddress,&dummy,&pDestUnit)) {				
				if (FALSE == this->m_pRuntime->AddUnit(pDestRTNode,pSourceUnit->bUnitAddress,&dummy,&pDestUnit)) {
					ASSERT(0);
				}
			}			
			if (NULL != pDestUnit) {
				// Copy Unit Object
				INVARIANT((NULL != pSourceUnit) && (NULL != pDestUnit));
				CNodeDatabase::CopyUnitObject(pDestUnit,pSourceUnit);
			}
		}
	}
	ASSERT(pDestRTNode->getNROfUnits() == refDocumentNode.getNROfUnits());

	// De runtime units de referentie naar pBindingUnit verwijderen...
	DeleteRuntimeNodeBindingUnitPropPtr(this->m_pRuntime, pDestRTNode);
}

/**********************************************************************
 * Sedert V16.42: m_mapUnitInfoLabels
 * Bijhouden van extra unit info in een map.	
 **********************************************************************/
void CAppNodeDatabase::clearDatabaseExtraUnitInfo( ) {
	m_mapUnitInfoLabels.RemoveAll();
}

CString CAppNodeDatabase::getUnitTextLabel(int nodeAddress, int unitAddress) {
	CString str;
	const int key = ((nodeAddress << 8) + unitAddress);
	if (FALSE == m_mapUnitInfoLabels.Lookup(key,str)) {
		return CString();
	}
	return str;
}

void CAppNodeDatabase::setUnitTextLabel(int nodeAddress, int unitAddress, const CString& str) {
	const int key = ((nodeAddress << 8) + unitAddress);
	m_mapUnitInfoLabels.SetAt(key,str);
}

void CAppNodeDatabase::removeUnitTextLabel(int nodeAddress, int unitAddress) {
	const int key = ((nodeAddress << 8) + unitAddress);
	m_mapUnitInfoLabels.RemoveKey(key);
}
/************************************************************************/


/************************************************************************/
/************************************************************************/
/************************************************************************/
// Controle in de runtime node database of er een master aanwezig is.
BOOL CAppNodeDatabase::runtimeNodeDatabase_isMasterPresent( ) {
	BOOL result = FALSE;
	CNodeIterator it(this->GetNodesRuntime()->CreateNodeIterator());
	for (it.Begin();!it.IsDone();it.Next()) {
		CCanNode* const pNode = it.CurrentItem( );
		if (NULL != pNode) {
			if (NODE_LCD_MASTER_ADDRESS == pNode->getNodeAddress()) {
				result = TRUE;
				break;
			}		
		}
	}
	return result;
}

static BOOL findNodesWithFactoryAddresses(CNodeDatabase& refNodeDatabase,CList<int>& list,int nodeType) {
	BOOL result = FALSE;	
	CNodeIterator it(refNodeDatabase.CreateNodeIterator());
	for (it.Begin();!it.IsDone();it.Next()) {
		CCanNode* const pNode = it.CurrentItem( );
		if ((NULL != pNode) && (NODE_FACTORY_ADDRESS == pNode->getNodeAddress())) {
			if (pNode->getNodeType() == nodeType) {
				list.AddTail(pNode->getPhysicalAddress());
				result = TRUE;
			}
		}
	}
	return result;
}

BOOL CAppNodeDatabase::runtimeNodeDatabase_foundTouchscreenNodesWithFactoryAddress(CList<int>& list) {
	CNodeDatabase* const pDatabase = this->GetNodesRuntime();
	if (NULL == pDatabase) {
		INVARIANT(NULL != pDatabase);
		return FALSE;
	}
	list.RemoveAll();
	return findNodesWithFactoryAddresses(*pDatabase,list,NODE_LCD);
}


#if(0)
// Verwijderen uit de lijst dmv een min en max waarde.
static void removeElementsOutOfRange(CList<int>& myList, int minValue, int maxValue) {
    POSITION pos = myList.GetHeadPosition();
    POSITION current;
    while (pos != NULL) {
        current = pos;  // Bewaar de huidige positie (ZORGT ERVOOR DAT DE POS NIET CORRUPT KOMT!!)
        int value = myList.GetNext(pos); // Haal de waarde op en ga naar de volgende
        // Verwijder als het buiten het bereik valt
        if (value < minValue || value > maxValue) {
            myList.RemoveAt(current);
        }
    }
}
#endif 


BOOL CAppNodeDatabase::runtimeNodeDatabase_foundSmartboxUltraNodesWithFactoryAddresses(CList<int>& list) {

	enum {
		// Reeks1: adressen: 0x0B0000 - 0x0BFFFF
		PHYSICAL_NODEADDRESS_SBOX_START = 0x0B0000,
		PHYSICAL_NODEADDRESS_SBOX_LAST  = 0x0BFFFF,

		// Reeks2: adressen: 0x1B0000 - 0x1BFFFF
		PHYSICAL_NODEADDRESS_SBOX_START2 = 0x1B0000,
		PHYSICAL_NODEADDRESS_SBOX_LAST2  = 0x1BFFFF,
	};

	CNodeDatabase* const pDatabase = this->GetNodesRuntime();
	if (NULL == pDatabase) {
		INVARIANT(NULL != pDatabase);
		return FALSE;
	}
	CList<int> listAddresses;
	if (FALSE == findNodesWithFactoryAddresses(*pDatabase,listAddresses,NODE_PIN_GATEWAY)) {
		return FALSE;	
	}

	list.RemoveAll();

	// Oppassen:
	// Controle van de fysische adressen.
	// Mogen enkel de adressen overhouden die in de range van de physical addressen van de smartbox ultra liggen.
	POSITION pos = listAddresses.GetHeadPosition();
    while (pos != NULL) {        
        const int value = listAddresses.GetNext(pos); 
		if ((value >= PHYSICAL_NODEADDRESS_SBOX_START && value<=PHYSICAL_NODEADDRESS_SBOX_LAST) ||
			(value >= PHYSICAL_NODEADDRESS_SBOX_START2 && value<=PHYSICAL_NODEADDRESS_SBOX_LAST2)) {
			list.AddTail(value);
		}
	}
	return (list.IsEmpty() ? FALSE : TRUE);
}


/************************************************************************/
BOOL CAppNodeDatabase::possibleToUpgradeIncompatibleUnits(BYTE bNodeAddress)
{
	BOOL result = FALSE;
	CCanNode* pNodeDoc = 0;
	CCanNode* pNodeNetwork = 0;
	int k = 0;
	if (GetNodesRuntime()->FindNode(bNodeAddress,&k,&pNodeNetwork)) {
		if (GetNodesDocument()->FindNode(bNodeAddress,&k,&pNodeDoc)) {
			// Beide nodes moeten LCD type zijn.
			// De Network node moet minstens evenveel units hebben als de de database node.
			//
			// Sedert V15.03:
			// Master TCP/IP vervangen door een touchscreen.
			// De Network node moet minstens evenveel units hebben als de de database node.
#if(USE_UPGRADE_TCPIP_MASTER==1)
			if (((pNodeNetwork->bNodeType == NODE_LCD) && (pNodeDoc->bNodeType == NODE_LCD) && 
				(pNodeNetwork->bNROfUnits >= pNodeDoc->bNROfUnits)) ||
				((pNodeNetwork->bNodeType == NODE_LCD) && (pNodeDoc->bNodeType == NODE_PIN_GATEWAY) && 
				 (pNodeDoc->bNodeFlags & NODE_MASTER_LCD) && (pNodeNetwork->bNROfUnits >= pNodeDoc->bNROfUnits)))
#else
			if ((pNodeNetwork->bNodeType == NODE_LCD) &&(pNodeDoc->bNodeType == NODE_LCD) && (pNodeNetwork->bNROfUnits >= pNodeDoc->bNROfUnits))	
#endif
			{
				result = TRUE;		// !!!
				const int NrOfUnits = pNodeDoc->getNROfUnits( );
				for (int i=0;i<NrOfUnits;i++)
				{			
					// Als de unittypes verschillend zijn van de Doc node en RT node.
					// en het unit type != UNIT_TYPE_SENS kan de LCD/TFT niet compatible
					// gemaakt worden.
					STRUCT_UNIT* pDocUnit = 0;
					if (GetNodesDocument()->GetUnit(pNodeDoc,i,&pDocUnit)) {
						if (pDocUnit->bUnitType != UNIT_TYPE_SENS) {
							STRUCT_UNIT* pRtUnit = 0;							
							if (GetNodesRuntime()->GetUnit(pNodeNetwork,i,&pRtUnit)) {
								if (pDocUnit->bUnitType != pRtUnit->bUnitType) {							
									result = FALSE;
									break;								
								}
							}
						}
					}						
				}			
			}
		}
	}
	return result;
}

BOOL CAppNodeDatabase::possibleToMakeDbaseNodesCompatible( ) {

	// NOTE: Als er 1 probleem tussen zit dan is de return waarde FALSE !!!
	class CNodeDatabaseVerify_HandlerImpl : public CNodeDatabaseVerify_Handler
	{
	public:
		CNodeDatabaseVerify_HandlerImpl(CAppNodeDatabase* parent) : m_pParent(parent), m_possibleToMakeCompatible(TRUE) { }

		virtual void On_Incompatible_Nodes(int index, const CCanNode* pDocNode, const CCanNode* pRTNode) {			
			if (!m_pParent->possibleToUpgradeIncompatibleUnits(pDocNode->getNodeAddress())) {
				m_possibleToMakeCompatible = FALSE;		// zie NOTE
			}		
		}
	public:	
		CAppNodeDatabase* m_pParent;
		BOOL m_possibleToMakeCompatible;		// TRUE wanneer alle problemen kunnen opgelost worden.
	};

	CNodeDatabaseVerify_HandlerImpl handler(this);
	CNodeDatabaseVerify Algoritme(&handler);

	BOOL result = FALSE;
	const int nCount = Algoritme.Check_Incompatible_Nodes( );
	if (nCount > 0) {
		result = handler.m_possibleToMakeCompatible;
	}
	return result;
}


// Mogelijkheid om een probleem op te lossen via Replace nodes.
// - Als alle oude nodes (pc-database) ook aanwezig zijn in het netwerk dan kunnen er geen vervangen worden.
BOOL CAppNodeDatabase::possibleToReplaceNodes( ) 
{
	{	
		// Als alle oude nodes (pc-database) ook aanwezig zijn in het netwerk dan kunnen er geen vervangen worden.
		// Doen telkens een vergelijking met het physical node address.
		CCanNode* pNodeDoc = 0;
		int nrOfNodes = 0;
		int counterNodesFound = 0;
		GetNodesDocument()->NROfNodes(&nrOfNodes);
		for (int i=0;i<nrOfNodes;++i) {			
			if (GetNodesDocument()->GetNode(i,&pNodeDoc)) {
				CCanNode* pNetworkNode = NULL;
				int dummy;

				// Controle doen op het logisch address...
				if (GetNodesRuntime()->FindNode(pNodeDoc->getNodeAddress(),&dummy,&pNetworkNode)) {
					if (pNodeDoc->getPhysicalAddress() == pNetworkNode->getPhysicalAddress()) {
						ASSERT(0xFE != pNetworkNode->getNodeAddress());		// Kan niet voorkomen - zie FindNode()
						++counterNodesFound;
					}
				}
				else {
					// Controle doen op het physical node address.
					// indien de node gevonden wordt met een factory init address dan is het dezelfde node en 
					// moeten we geen replace doen (maar een restore of manual assign).
					if (GetNodesRuntime()->FindPhysicalNode(pNodeDoc->getPhysicalAddress(),&dummy,&pNetworkNode)) {
						if (0xFE == pNetworkNode->getNodeAddress()) {
							ASSERT(pNodeDoc->getPhysicalAddress() == pNetworkNode->getPhysicalAddress());	// dezelfde node.
							++counterNodesFound;						
						}					
					}				
				}
			}		
		}
		if (nrOfNodes == counterNodesFound) {
			return FALSE;
		}
	}	
	return TRUE;
}


// Indien er nodes aanwezig zijn in de runtime database die een ander adres hebben in
// de document database... (Mogelijk een 0xFE adres in het netwerk.)
BOOL CAppNodeDatabase::possibleToResolveAddressFromDbase( ) {
	
	int nrOfNodes = 0;
	GetNodesRuntime()->NROfNodes(&nrOfNodes);
	for (int i=0;i<nrOfNodes;++i) {	
		int dummy = 0;
		CCanNode* pNetworkNode = NULL;
		if (GetNodesRuntime()->GetNode(i,&pNetworkNode)) {
			CCanNode* pDocNode = NULL;
			if (GetNodesDocument()->FindPhysicalNode(pNetworkNode->getPhysicalAddress(),&dummy,&pDocNode)) {
				if (pDocNode->getNodeAddress() != pNetworkNode->getNodeAddress()) {
					return TRUE;
				}
			}		
		}
	}
	return FALSE;
}



//	RESULT_UNKNOWN = 0,
//	RESULT_DATABASE_RECENT,
//	RESULT_NETWORK_RECENT
CAppNodeDatabase::ProgramFileDiffResult_t CAppNodeDatabase::findTypeProgramFileDifference(CString& dateDatabase, CString& dateNetwork) {	
	CCanNode* pDocNode = NULL;
	CCanNode* pRtNode = NULL;
	int dummy = 0xFF;

	if (!GetNodesDocument()->FindNode(0xfc, &dummy, &pDocNode)) {
		return RESULT_UNKNOWN;
	}
	if (!GetNodesRuntime()->FindNode(0xfc, &dummy, &pRtNode)) {
		return RESULT_UNKNOWN;
	}
	if (NULL == pDocNode || NULL == pRtNode) {
		return RESULT_UNKNOWN;
	}
	const char* const nameDocNode = pDocNode->getNodeName();
	const char* const nameRtNode = pRtNode->getNodeName();
	if (NULL == nameDocNode || NULL == nameRtNode) {
		return RESULT_UNKNOWN;
	}

	int db_year = 0;
	int db_month = 0;
	int db_day = 0;
	int nwk_year = 0;
	int nwk_month = 0;
	int nwk_day = 0;

	if (3 != sscanf(nameDocNode, "%04d%02d%02d", &db_year, &db_month, &db_day)) {
		return RESULT_UNKNOWN;
	}
	if (3 != sscanf(nameRtNode, "%04d%02d%02d", &nwk_year, &nwk_month, &nwk_day)) {
		return RESULT_UNKNOWN;
	}
	
	if (db_day < 1 || db_day > 31 || nwk_day < 1 || nwk_day > 31) {
		return RESULT_UNKNOWN;
	}
	if (db_month < 1 || db_month > 12 || nwk_month < 1 || nwk_month > 12) {
		return RESULT_UNKNOWN;
	}
	if (db_year < 2000 || db_year > 3000 || nwk_year < 2000 || nwk_year > 3000) {
		return RESULT_UNKNOWN;	
	}

	ProgramFileDiffResult_t result = RESULT_UNKNOWN;
	const int db_date = db_day + (db_month * 100) + (db_year * 10000);
	const int nwk_date = nwk_day + (nwk_month * 100) + (nwk_year * 10000);
	if (db_date == nwk_date) {
		result = RESULT_UNKNOWN;
	}
	else if (db_date > nwk_date) {
		result = RESULT_DATABASE_RECENT;
	}
	else if (db_date < nwk_date) {
		result = RESULT_NETWORK_RECENT;
	}	
	dateDatabase.Format("%02d/%02d/%04d", db_day, db_month, db_year);
	dateNetwork.Format("%02d/%02d/%04d", nwk_day, nwk_month, nwk_year);
	return result;
}

void CAppNodeDatabase::copyNamesFromNetwork( ) {
	int nNodes;	
	if (GetNodesRuntime()->NROfNodes(&nNodes)){			
		CCanNode *pDocNode,*pRTNode;
		for (int i=0;i<nNodes;i++) {
			if (GetNodesRuntime()->GetNode(i,&pRTNode)) {
				int j;
				if (GetNodesDocument()->FindNode(pRTNode->bLogicalAddress,&j,&pDocNode)) {
					copyNamesFromNetwork(pDocNode,pRTNode);						
				} 
				ASSERT(pDocNode);
			} 
			ASSERT(pRTNode);
		} 
	}
}

void CAppNodeDatabase::copyNamesFromNetwork(CCanNode *pDocNode,CCanNode *pRTNode) {
	if ((pDocNode != NULL) && (pRTNode!=NULL)) {
		STRUCT_UNIT *pRTUnit,*pDocUnit;
		for (int i=0;i<pRTNode->bNROfUnits;i++) {
			if (GetNodesRuntime()->GetUnit(pRTNode, i, &pRTUnit)) {
				if (GetNodesDocument()->GetUnit(pDocNode, i, &pDocUnit)) {										
					if ((NULL != pRTUnit) && (NULL != pDocUnit)) {
						strcpy(pDocUnit->szUnitName,pRTUnit->szUnitName);
					}
				} 
				ASSERT(pDocUnit);
			}
			ASSERT(pRTUnit);
		}
	}
}

BOOL CAppNodeDatabase::replaceIncompatibleNodes(CCanNode* pNodeDoc, CCanNode* pNodeNetwork)
{
	ASSERT( pNodeDoc && pNodeNetwork );
	ASSERT( pNodeDoc->bLogicalAddress == pNodeNetwork->bLogicalAddress);

	BOOL result = FALSE;
	const int nrOfNetworkUnits = pNodeNetwork->getNROfUnits( );
	const int nrOfDocumentUnits = pNodeDoc->getNROfUnits( );	
	if (nrOfDocumentUnits > nrOfNetworkUnits)
	{		
		// Als het aantal units van de document node hoger is dan deze 
		// van de network node moeten de document units verwijderd worden.

		// Todo Moeten nog rekening houden met de virtual units. !!!!
		for (int i=nrOfDocumentUnits;i>=nrOfNetworkUnits;i--) {
			const int unitIndex = i-1;
			GetNodesDocument()->DeleteUnitNR(pNodeDoc,unitIndex);	
		}
	}

	for (int i=0;i<nrOfNetworkUnits;i++)
	{
		STRUCT_UNIT* pNetworkUnit = 0;
		if (GetNodesRuntime()->GetUnit(pNodeNetwork,i,&pNetworkUnit)) {
			STRUCT_UNIT* pDocUnit = 0;
			if (GetNodesDocument()->GetUnit(pNodeDoc,i,&pDocUnit)) {
				if (pNetworkUnit->bUnitType != pDocUnit->bUnitType)
				{									
					// Oppassen:
					// De doc unit wordt gewijzigd naar de network unit, 
					// De Referentie naar de binding unit moet afzonderlijk
					// gewijzigd worden.
					// CBindingPropertyFactory::destroy( pDocUnit ) en
					// CreateBindingUnitProperties( pDocUnit ).

					CNodeDatabaseOperations operations(GetNodesDocument());
					result = operations.ReplaceIncompatibleUnits(pNodeDoc,pDocUnit,pNetworkUnit);
					ASSERT( result == TRUE );
					if (result == TRUE) {
						CBindingPropertyFactory::destroy( pDocUnit );
						// Binding unit aanmaken en koppelen aan de unit
						CreateBindingUnitProperties_whenNonAssigned(pNodeDoc,pDocUnit);
					}				
				}
			}
			else
			{
				// De networknode heeft meer units dan de document node.
				// Moeten de units bijvoegen aan de docNode.

				int k = 0;
				if (GetNodesDocument()->AddUnit(pNodeDoc,i,&k,&pDocUnit)) {		
					// Alle properties copieren 
					CNodeDatabase::CopyUnitObject(pDocUnit,pNetworkUnit);				
					// Binding unit aanmaken en koppelen aan de unit
					CreateBindingUnitProperties_whenNonAssigned(pNodeDoc,pDocUnit);
				} 																					
			}
		}
	}
	if (result) {
		CreateBroadcastBindingUnits(GetNodesDocument(),pNodeDoc);	
	}
	return result;
}

int CAppNodeDatabase::findIncompatibleUnits(
	CCanNode* pNodeDoc,
	CCanNode* pNodeNetwork,
	STRUCT_UNIT** pArrUnits,
	int maxLength)
{
	int result = 0;

	const int nrOfNetworkUnits = pNodeNetwork->getNROfUnits( );
	const int nrOfDocumentUnits = pNodeDoc->getNROfUnits( );

	for (int i=0;i<nrOfDocumentUnits;i++)
	{
		STRUCT_UNIT* pDocUnit = 0;
		STRUCT_UNIT* pNetworkUnit = 0;

		GetNodesDocument()->GetUnit(pNodeDoc,i,&pDocUnit);
		GetNodesRuntime()->GetUnit(pNodeNetwork,i,&pNetworkUnit);

		if (pNetworkUnit == 0 ||
			pDocUnit->bUnitType != pNetworkUnit->bUnitType)
		{
			if (result < maxLength-1)
			{
				pArrUnits[result] = pDocUnit;
				result++;
			}
		}						
	}
	return result;
}

/********************************** protected ******************************************/
BOOL CAppNodeDatabase::CreateBindingUnitProperties(CNodeDatabase *pNodes)
{	
	int iNrOfNodes;
	if (pNodes->NROfNodes(&iNrOfNodes) == TRUE) {
		if (iNrOfNodes != 0)  {
			CCanNode *pNode;
			for (int i=0;i<iNrOfNodes;i++) {
				if (pNodes->GetNode(i,&pNode)) {
					CreateBindingUnitProperties(pNodes,pNode);	
				}
			}
			return TRUE;
		}
	}
	return FALSE;
}


BOOL CAppNodeDatabase::CreateBindingUnitProperties(CNodeDatabase *pNodes,CCanNode *pNode) {
	STRUCT_UNIT *pUnit = NULL;
	const int iNrOfUnits = pNode->iNROfUnits;	
	for (int j=0;j<iNrOfUnits;j++) {
		if (pNodes->GetUnit(pNode,j,&pUnit)) {		
			if (!helper_CreateBindingUnitProperties(pNode,pUnit)) {
				ASSERT(0);
			}
		}
		else { ASSERT(0); } 
	}
	return TRUE;
}

BOOL CAppNodeDatabase::DeleteRuntimeNodesBindingUnitPropPtr(CNodeDatabase* pNodes) {

	if (NULL == pNodes){
		REQUIRE(NULL != pNodes);
		return FALSE;
	}

	int iNrOfNodes = 0;
	if (pNodes->NROfNodes(&iNrOfNodes) == FALSE) {
		ASSERT(0);
		return FALSE;
	}
	
	if (iNrOfNodes != 0) {			
		for (int i=0;i<iNrOfNodes;i++) {
			CCanNode *pNode = NULL;	
			if (pNodes->GetNode(i,&pNode)) {
				INVARIANT(NULL != pNode);
				DeleteRuntimeNodeBindingUnitPropPtr(pNodes, pNode);				
			}
		}
	}
	return TRUE;
}

BOOL CAppNodeDatabase::DeleteRuntimeNodeBindingUnitPropPtr(CNodeDatabase *pNodes, CCanNode* pNode) {	
	if ((NULL == pNode) || (NULL == pNodes)){
		REQUIRE(NULL != pNode);
		REQUIRE(NULL != pNodes);
		return FALSE;
	}

	const int iNrOfUnits = pNode->iNROfUnits;		
	for (int j=0;j<iNrOfUnits;j++) {
		STRUCT_UNIT *pUnit = NULL;
		if (pNodes->GetUnit(pNode,j,&pUnit)) {									
			INVARIANT(NULL != pUnit);
			pUnit->m_pBindingUnit = NULL;							
		} else { ASSERT(0); }
	}
	return TRUE;
}

BOOL CAppNodeDatabase::CreateBroadcastBindingUnits(CNodeDatabase *pNodes) {	
	int iNrOfNodes;
	BOOL fSuccess = FALSE;	
	if (pNodes->NROfNodes(&iNrOfNodes)) {
		if (iNrOfNodes != 0)  {
			CCanNode *pNode;
			for (int i=0;i<iNrOfNodes;i++) {
				if (pNodes->GetNode(i,&pNode)) {		
					if (CreateBroadcastBindingUnits(pNodes,pNode) == TRUE) {
						fSuccess = TRUE;
					}
				}
			}
		}				
	}	
	return fSuccess;
}

BOOL CAppNodeDatabase::CreateBroadcastBindingUnits(CNodeDatabase *pNodes,CCanNode *pNode)
{	
	BYTE bLf48BoseRooms = 0;
	BYTE bNrOfBoseRooms = 0;
	BYTE bV35BoseRooms = 0;

	const int iNrOfUnits = pNode->iNROfUnits;
	for (int j=0;j<iNrOfUnits;j++) {
		STRUCT_UNIT *pUnit = NULL;
		if (pNodes->GetUnit(pNode,j,&pUnit)) {
			switch(pUnit->bUnitType) {
				case UNIT_TYPE_BOSERS: {
					const BYTE bConfig = pUnit->UnitSpecs.AudioUnit.bAudioConfig;
					if (BOSE_LIFESTYLE_SYSTEM50 == bConfig) {
						bNrOfBoseRooms++;
					}
					else if (BOSE_LIFESTYLE_SYSTEMV35 == bConfig) {
						bV35BoseRooms++;
					}
					else  {
						bLf48BoseRooms++;
					}
				} break;
			}
		}
		ASSERT(NULL != pUnit);
	}		
	
	if ((bNrOfBoseRooms == NR_BOSE_LF50_ROOMS) || (bV35BoseRooms == NR_BOSE_V35_ROOMS) ||(bLf48BoseRooms == NR_BOSE_LF48_ROOMS)) {
		STRUCT_UNIT *pNewUnit = NULL;	
		if (CAppNodeDatabase::AddBroadcastUnit(pNode,&pNewUnit) == TRUE) {			
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
			else if (bV35BoseRooms == NR_BOSE_V35_ROOMS) {
				pNewUnit->UnitSpecs.AudioUnit.bAudioConfig = BOSE_LIFESTYLE_SYSTEMV35;	
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

BOOL CAppNodeDatabase::AddBroadcastUnit(CCanNode* pNode,STRUCT_UNIT **ReturnUnitPtr)
{
	STRUCT_UNIT *PreviousUnitSpecsPtr = 0;		// CHANGES_MVS_2008_WARNINGS	
	const BYTE bUnitAddress = UNIT_VIRTUAL_BROADCAST_ADDRESS;
	int i;

	STRUCT_UNIT *UnitSpecsPtr = pNode->pFirstUnit; 

	for (i=0;i<pNode->iNROfUnits;i++) {
		if (UnitSpecsPtr->bUnitAddress == bUnitAddress) break;
		PreviousUnitSpecsPtr = UnitSpecsPtr;
		UnitSpecsPtr = UnitSpecsPtr->pNextUnit;
	}
	if (i==pNode->iNROfUnits) {
		if ((UnitSpecsPtr = (STRUCT_UNIT*)malloc(sizeof(STRUCT_UNIT))) == NULL) {          
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

		if (i==0)  {	          
			pNode->pFirstUnit = UnitSpecsPtr;
		}
		else {  
			ASSERT(NULL != PreviousUnitSpecsPtr);		
	        PreviousUnitSpecsPtr->pNextUnit = UnitSpecsPtr;
		}
	    return(true);	
	}
	return (false);
}




