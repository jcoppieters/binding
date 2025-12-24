#include "stdafx.h"
#include "BindingSoftware.h"
#include "TMConvenience.h"
#include "app\AppBindings\AppBindingProperty\bindingpropertyunitsens_utils.h"
#include "app\AppBindings\AppBindingProperty\bindingpropertyunit_alg.h"
#include "BindingConversionAuto2.h"
#include "app/AppNodeDbase/AppNodeDbase.h"
#include "app\appCfg\AppCfginfo\cfginfonode.h"	
#include "app/appdali/appdalibridge.h"
// -----------------------------------
#include "TMGlobals.h"
// ------------------------------------

/*********************************************************************/
#define DEBUG_VERIFY_SENS_DELEGATION		1
/*********************************************************************/
using namespace TMBindingDatabase;
using namespace TMNodeDatabase;
/*********************************************************************/

/********************************** Names ****************************/
// Sedert V16.41:
// Bij sommige Control units die extra aangemaakt werden mag de naam niet veranderen.
// - Bij de smartbox ultra krijgen de DALI control units hun naam via de DALI configuratie.
//   Deze units moeten hun naam blijven behouden tijdens het maken van een binding.

// Private helper functies...
// Smartbox ultra:
// Alle Control units die bijgemaakt werden in de smatbox ultra de vlag DEFAULT_NAME_OVERRIDDEN zetten.
static void helper_SetControlUnitsDefaultNameOverridden(CNodeDatabase* pNodeDatabase, CCanNode* pNode) {
	// Momenteel enkel voor de smartbox ultra...
	if (CNodeConfigInfo_SmartboxUltra::isType(pNode)) {
		const int lastUnitAddress = pNode->getNROfUnits();
		const int first = CNodeConfigInfo_SmartboxUltra::getUnitAddressLastInternalUnit(pNode)+1;
		if (first > lastUnitAddress) {
			return;		// Niks doen.
		}		
		int dummy;
		for (int unitAddress=first;unitAddress<lastUnitAddress;++unitAddress) {
			STRUCT_UNIT* pUnit = NULL;
			if (pNodeDatabase->FindUnit(pNode,unitAddress,&dummy,&pUnit)) {
				if (UNIT_TYPE_CONTROL == pUnit->bUnitType) {						
					pUnit->ulApplicationFlags |= DEFAULT_NAME_OVERRIDDEN;				
				}
			}
		}
	}	
}

/******************************** Text label ******************************/
// Sedert V16.42:
// Bijhouden van extra info voor de units.
// - Extra tekst label:
//	 - Smartbox ultra: Weergave zoals op de sticker.
//	 - Touchbuttons & Serinity: 1V, 2V, 4V 	
// Private helper functies...
static CString helper_GetUnitExtraLabelInfoText(CCanNode* pNode, STRUCT_UNIT* pUnit) {
	if (NULL == pNode || NULL == pUnit) {
		REQUIRE(NULL != pNode);
		REQUIRE(NULL != pUnit);
		return CString();
	}
	
	CString str;				

	// Satic labels.
	if (CNodeConfigInfo_TouchButtons::isType(pNode)) {
		str = CNodeConfigInfo_TouchButtons::getStringLabelInfo(pNode,pUnit->bUnitType,pUnit->bUnitAddress);
	}
	else if (CNodeConfigInfo_SerenityButtons::isType(pNode)) {
		str = CNodeConfigInfo_SerenityButtons::getStringLabelInfo(pNode,pUnit->bUnitType,pUnit->bUnitAddress);
	}	
	else if (CNodeConfigInfo_VelocityButtons::isType(pNode)) {
		str = CNodeConfigInfo_VelocityButtons::getStringLabelInfo(pNode,pUnit->bUnitType,pUnit->bUnitAddress);
	}	
	else if (CNodeConfigInfo_SmartboxUltra::isType(pNode)) {			
		str = CNodeConfigInfo_SmartboxUltra::getStringLabelInfo(pNode,pUnit->bUnitType,pUnit->bUnitAddress);
	}


	// Dali Bridge config.
	if (str.IsEmpty()) {
		if ((NULL != pUnit->m_pAppUnitData) && (CAppUnitDataDali::TYPE_UNIT_DATA_DALI == pUnit->m_pAppUnitData->getUnitDataType())) {
			CAppUnitDataDali* const p = dynamic_cast<CAppUnitDataDali*>(pUnit->m_pAppUnitData);
			if (NULL != p) {
				const int address = p->getDaliAddress();
				if (address >= 0) {
					const CAppUnitDataDali::DaliType_t type = p->getDaliType();
					switch(type) {
						case CAppUnitDataDali::DALI_DEVICE_CONTROLGEAR: {
							str.Format("Dali A%d",address);					  
						} break;
						case CAppUnitDataDali::DALI_DEVICE_INPUTDEVICE: {
							str.Format("Dali A%d²",address);					  
						} break;
						case CAppUnitDataDali::DALI_GROUP_CONTROLGEAR: {
							str.Format("Dali group %d",address);							 
						} break;
						case CAppUnitDataDali::DALI_SCENE: {
							str.Format("Dali scene %d",address);				 
						} break;
					}				
				}			
			}
		}		
	}
	return str;
}
/*********************************************************************/


/*********************************************************************/
// Bij een bestaande installatie.
static void TMAssignBindingFilesToRTNodes() {
	CBindingDatabaseApplication* const pAppBinding = TMGetBindingApp();
	if (0!=pAppBinding) {
		pAppBinding->AssignBindingFilesToNodes(TMGetRuntimeNodes());	
	}
}

// Sedert versie 0x0900.
// Bij een bestaande installatie.
//
// Zorgt ervoor dat alle binding units de juiste functionaliteiten krijgen ifv de node en 
// het product id. Dit o.a. nodig voor:
//	- Forward bindings van outputs, 
//  - Sensor unit delegetion.
//	- Ondersteuning PID.
//	- Ondersteuning van specifieke P-Bindings.
static void TMUpgradeBindingPropertyUnits() {
	bindingpropertyunitalg_upgrade(TMGetDocumentNodes());
}

static void TMUpgradeBindingPropertyUnits(CCanNode* pNode) {
	bindingpropertyunitalg_upgrade(pNode);
}

// Nieuwe units toevoegen aan de lijst 'g_listUnitsSensReadMeasuredValue'.
// Deze lijst bevat alle units die kunnen gebruikt worden om de gemeten waarde op te vragen.
// Opmerking:
// Er wordt geen controle gedaan op de gekoppelde units omdat enkel nieuwe units toegevoegd worden. (en deze nog 
// niet kunnen gekoppeld zijn).
void TMCreateUnitListRequestMeasuredValues(CCanNode& refNode) {
	const int count_prev = g_listUnitsSensReadMeasuredValue.GetCount();
	const int count = bindingPropertyUnitSens_delegationRequestMeasureValListAdd(refNode,&g_listUnitsSensReadMeasuredValue);	
	ASSERT(g_listUnitsSensReadMeasuredValue.GetCount() == (count_prev + count));			
}

// zie USE_SENSOR_PIDCONTROL	
// Nieuwe units toevoegen aan de lijst 'g_listUnitsDimPIDControl'.
// Een lijst aanmaken van alle dimmer units die kunnen gebruikt worden door de sensor unit voor PID control.
// Deze lijst wordt gebruikt voor de opbouw van de dialogen.
void TMCreateUnitDimListPIDControl(CCanNode& refNode) {
	const int count_prev = g_listUnitsDimPIDControl.GetCount();
	const int count = bindingPropertyUnitSens_pidControlDimUnitListAdd(refNode,&g_listUnitsDimPIDControl);
	ASSERT(g_listUnitsDimPIDControl.GetCount() == (count_prev + count));	
}


// Sedert V16.41:
// Bij sommige Control units die extra aangemaakt werden mag de naam niet veranderen.
// - Bij de smartbox ultra krijgen de DALI control units hun naam via de DALI configuratie.
//   Deze units moeten hun naam blijven behouden tijdens het maken van een binding.
static void TMSetControlUnitsDefaultNameOverridden() {	
	CAppNodeDatabase* const pAppNodeDatabase = CAppNodeDatabase::GetInstance();
	if (NULL == pAppNodeDatabase) {
		INVARIANT(NULL != pAppNodeDatabase);
		return;
	}	
	CNodeDatabase* const pNodeDatabase = pAppNodeDatabase->GetNodesDocument();
	if (NULL == pNodeDatabase) {
		INVARIANT(NULL != pNodeDatabase);
		return;
	}
	// Alle nodes overlopen...
	CNodeIterator it(pNodeDatabase->CreateNodeIterator());
	for (it.Begin();!it.IsDone();it.Next()) {
		CCanNode* const pNode = it.CurrentItem( );
		helper_SetControlUnitsDefaultNameOverridden(pNodeDatabase,pNode);
	}
}
	
static void TMSetControlUnitsDefaultNameOverridden(CCanNode& refNode) {	
	CAppNodeDatabase* const pAppNodeDatabase = CAppNodeDatabase::GetInstance();
	if (NULL == pAppNodeDatabase) {
		INVARIANT(NULL != pAppNodeDatabase);
		return;
	}	
	CNodeDatabase* const pNodeDatabase = pAppNodeDatabase->GetNodesDocument();
	if (NULL == pNodeDatabase) {
		INVARIANT(NULL != pNodeDatabase);
		return;
	}	
	helper_SetControlUnitsDefaultNameOverridden(pNodeDatabase,&refNode);	
}

static void TMSetDatabaseUnitExtraInfo(CAppNodeDatabase& refAppNodeDatabase,CNodeDatabase& refNodeDatabase, CCanNode& refNode) { 
	CUnitIterator it(refNodeDatabase.CreateUnitIterator(&refNode));
	for (it.Begin();!it.IsDone();it.Next()) {					
		STRUCT_UNIT* const pUnit = it.CurrentItem();
		ASSERT(NULL != pUnit);
		const CString str = helper_GetUnitExtraLabelInfoText(&refNode,pUnit);
		if (!str.IsEmpty()) {
			refAppNodeDatabase.setUnitTextLabel(refNode.getNodeAddress(),pUnit->bUnitAddress,str);
		}
		else {
			refAppNodeDatabase.removeUnitTextLabel(refNode.getNodeAddress(), pUnit->bUnitAddress);
		}
	}
}

static void TMSetDatabaseUnitExtraInfo(CCanNode& refNode) { 
	CAppNodeDatabase* const pAppNodeDatabase = CAppNodeDatabase::GetInstance();
	if (NULL == pAppNodeDatabase) {
		INVARIANT(NULL != pAppNodeDatabase);
		return;
	}	
	CNodeDatabase* const pNodeDatabase = pAppNodeDatabase->GetNodesDocument();
	if (NULL == pNodeDatabase) {
		INVARIANT(NULL != pNodeDatabase);
		return;
	}	
	TMSetDatabaseUnitExtraInfo(*pAppNodeDatabase,*pNodeDatabase,refNode);
}

static CString getDaliBridgeCfgFileName(int nodeAddress) {
	using namespace TMGlobalSettings;

	CString szSubDir;
	const CString szPathName(TMGetFileSettings()->GetFullyQualifiedPathName());	
	szSubDir.LoadString(IDS_FILENAME_SUBDIRCONFIG);

	CString szTemp;
	szTemp.Format("%s%s\\dalibridge_%02x.json",szPathName,szSubDir,nodeAddress);	
	return szTemp;
}

static void addDaliBridgeInfo(STRUCT_UNIT* pDest,DUOTECNO::DALI::AppDaliBridgeDevice& src) {
	using namespace DUOTECNO::DALI;
	if (NULL == pDest) {
		REQUIRE(pDest);
		return;
	}
	if ((NULL != pDest->m_pAppUnitData) && (CAppUnitDataDali::TYPE_UNIT_DATA_DALI != pDest->m_pAppUnitData->getUnitDataType())) {		
		delete pDest->m_pAppUnitData;
		pDest->m_pAppUnitData = NULL;									
	}	
	if (NULL == pDest->m_pAppUnitData) {
		pDest->m_pAppUnitData = new CAppUnitDataDali( );		
	}

	INVARIANT(NULL != pDest->m_pAppUnitData);
	INVARIANT(CAppUnitDataDali::TYPE_UNIT_DATA_DALI == pDest->m_pAppUnitData->getUnitDataType());

	CAppUnitDataDali* const p = dynamic_cast<CAppUnitDataDali*>(pDest->m_pAppUnitData);
	if (NULL == p) {
		INVARIANT(NULL != p);
		return;
	}
	const AppDaliBridgeDevice::Type_t type = src.getType();
	switch(type) {
		case AppDaliBridgeDevice::TYPE_DALI_DEVICE_UNKNOWN: {
			ASSERT(0);
			return;
		} break;
		case AppDaliBridgeDevice::TYPE_DALI_DEVICE_CONTROLGEAR: {
			p->setDaliType(CAppUnitDataDali::DALI_DEVICE_CONTROLGEAR);
		} break;
		case AppDaliBridgeDevice::TYPE_DALI_DEVICE_INPUTCONTROLDEVICE: {
			p->setDaliType(CAppUnitDataDali::DALI_DEVICE_INPUTDEVICE);
		} break;
		case AppDaliBridgeDevice::TYPE_DALI_GROUP_CONTROLGEAR: {
			p->setDaliType(CAppUnitDataDali::DALI_GROUP_CONTROLGEAR);
		} break;
		case AppDaliBridgeDevice::TYPE_DALI_SCENE: {
			p->setDaliType(CAppUnitDataDali::DALI_SCENE);
		} break;
	}
	p->setDaliAddress(src.getDaliAddress());


/* TODO...
	// Binding unit applicatie vlaggen.
	if (NULL != pDest->pBindingUnit) {
		pDest->pBindingUnit->SetApp
	}
*/

}

static void removeDaliBridgeInfo(STRUCT_UNIT* pDest) { 
	if (NULL == pDest) {
		REQUIRE(pDest);
		return;
	}
	if ((NULL != pDest->m_pAppUnitData) && (CAppUnitDataDali::TYPE_UNIT_DATA_DALI == pDest->m_pAppUnitData->getUnitDataType())) {			
		delete pDest->m_pAppUnitData;
		pDest->m_pAppUnitData = NULL;
	}								
}



// Sedert V16.46:
// Dali bridge config inladen en info koppelen aan de Duotecno units.
static void _TMLoadDaliBridgeConfig(CNodeDatabase* const pNodeDatabase, CCanNode* pNode) {
	using namespace DUOTECNO::DALI;
	if (NULL == pNode || NULL == pNodeDatabase) {
		REQUIRE(NULL != pNodeDatabase);
		REQUIRE(NULL != pNode);
		return;
	}

	const int nodeAddress = pNode->getNodeAddress();

	AppDaliBridgeDevices devices;
	AppDaliJsonBridgeDevices jsonParser(nodeAddress);

	const CString szTemp = getDaliBridgeCfgFileName(nodeAddress);
	if (!jsonParser.load((LPCSTR) szTemp,&devices)) {			
		// niks doen...
	}
	
	CUnitIterator it(pNodeDatabase->CreateUnitIterator(pNode));
	for (it.Begin();!it.IsDone();it.Next()) {
		STRUCT_UNIT* const pUnit = it.CurrentItem( );
		if (NULL != pUnit) {
			AppDaliBridgeDevice* pDevice = NULL;
			if (devices.find(nodeAddress,pUnit->bUnitAddress,&pDevice)) {
				addDaliBridgeInfo(pUnit,*pDevice);
			}
			else {
				removeDaliBridgeInfo(pUnit);
			}
		}
	}
}


/*********************************************************************/


/*********************************************************************/
/****************************** PUBLIC *******************************/
/*********************************************************************/
namespace TMConvenience
{

	// Aanmaken van een lijst van binding property units die delegation ondersteunen.
	// - inladen van project bestand.
	// - Bestaande installatie.
	// - nieuwe installatie na het browsen
	// - Bestaande installatie na het browsen/afhandelen post browse errors.
	//
	// Extra controle:
	// Nadat de lijst aangemaakt is controleren of iedere delegate 
	// connectie een buddy gekoppeld heeft.
	// Indien niet het geval dan deze binding wissen.
	// 
	void TMCreateUnitListDelegation(void)
	{
		CNodeDatabase* pNodeDatabaseDoc = TMGetDocumentNodes();
		ASSERT(0!=pNodeDatabaseDoc);

		// Wissen van de lijst
		bindingPropertyUnitSens_delegationListDestroy(&g_listUnitsDelegation);

		// Aanmaken van de lijst mbv doc node database.
		if (0!=pNodeDatabaseDoc) {
			int count = 0;
			ASSERT(g_listUnitsDelegation.GetCount()==0);
			count = bindingPropertyUnitSens_delegationListCreate(*pNodeDatabaseDoc, &g_listUnitsDelegation);
			ASSERT(count==g_listUnitsDelegation.GetCount());
			if (count>0) {
				if (FALSE== bindingPropertyUnitSens_delegationBuddyVerify(g_listUnitsDelegation)) {
					CBindingFiles* const pBindingFiles = TMGetBindingFiles( );
					ASSERT(0!=pBindingFiles);
					if(0!=pBindingFiles) {
						BOOL fNormalize =bindingPropertyUnitSens_delegationBuddyNormalize(g_listUnitsDelegation,pBindingFiles);
						ASSERT(fNormalize==TRUE);					
					}
				}
			}
		}
	}

	// sedert versie 0x0C03:
	// g_listUnitsSensReadMeasuredValue is een lijst die sensoren bevat waarvan de gemeten waarde
	// kan opgevraagd worden.
	// Een modbus sensor unit kan zulke sensor koppelen om de gemeten waarde op te vragen en 
	// door te sturen naar het Modbus systeem.
	void TMCreateUnitListRequestMeasuredValues(void)
	{
		CNodeDatabase* pNodeDatabaseDoc = TMGetDocumentNodes();
		ASSERT(0!=pNodeDatabaseDoc);
		// Wissen van de lijst
		bindingPropertyUnitSens_delegationRequestMeasureValListDestroy(&g_listUnitsSensReadMeasuredValue);

		// Aanmaken van de lijst mbv doc node database.
		if (0!=pNodeDatabaseDoc) {
			int count = 0;
			ASSERT(g_listUnitsSensReadMeasuredValue.GetCount()==0);
			count = bindingPropertyUnitSens_delegationRequestMeasureValListCreate(*pNodeDatabaseDoc,&g_listUnitsSensReadMeasuredValue);
			ASSERT(count==g_listUnitsSensReadMeasuredValue.GetCount());
			if (count>0) {
				// Controleren of alle sensoren die gekoppeld zijn ook aanwezig zijn
				// in de node database...
				// Indien een buddy niet kan gevonden worden dan de P-Bindings wissen.
				if (FALSE==bindingPropertyUnitSens_delegationRequestMeasureValVerify(pNodeDatabaseDoc)) {
					CBindingFiles* const pBindingFiles = TMGetBindingFiles( );
					ASSERT(0!=pBindingFiles);
					if(0!=pBindingFiles) {
						const int count = bindingPropertyUnitSens_delegationRequestMeasureValNormalize(pNodeDatabaseDoc,pBindingFiles);
						ASSERT(count!=0);
					}
				}
			}
		}	
	} /* TMCreateUnitListRequestMeasuredValues( ) */


	void TMSetDatabaseUnitExtraInfo( ) { 
		CAppNodeDatabase* const pAppNodeDatabase = CAppNodeDatabase::GetInstance();
		if (NULL == pAppNodeDatabase) {
			INVARIANT(NULL != pAppNodeDatabase);
			return;
		}	
		CNodeDatabase* const pNodeDatabase = pAppNodeDatabase->GetNodesDocument();
		if (NULL == pNodeDatabase) {
			INVARIANT(NULL != pNodeDatabase);
			return;
		}
		// Alle nodes overlopen...
		CNodeIterator it(pNodeDatabase->CreateNodeIterator());
		for (it.Begin();!it.IsDone();it.Next()) {
			CCanNode* const pNode = it.CurrentItem( );
			if (NULL != pNode) {
				TMSetDatabaseUnitExtraInfo(*pAppNodeDatabase,*pNodeDatabase,*pNode);
			}
		}
	}

	void TMSetDatabaseUnitExtraInfo(int nodeAddress) {
		CAppNodeDatabase* const pAppNodeDatabase = CAppNodeDatabase::GetInstance();
		if (NULL == pAppNodeDatabase) {
			INVARIANT(NULL != pAppNodeDatabase);
			return;
		}	
		CNodeDatabase* const pNodeDatabase = pAppNodeDatabase->GetNodesDocument();
		if (NULL == pNodeDatabase) {
			INVARIANT(NULL != pNodeDatabase);
			return;
		}
		int dummy = 0;
		CCanNode* pNode = NULL;
		if (FALSE == pNodeDatabase->FindNode(nodeAddress,&dummy,&pNode)) {
			ASSERT(0);
			return;
		}
		INVARIANT(pNode);
		TMSetDatabaseUnitExtraInfo(*pAppNodeDatabase,*pNodeDatabase,*pNode);
	}

	// Sedert V16.00: USE_SENSOR_PIDCONTROL	
	// Een lijst aanmaken van alle dimmer units die kunnen gebruikt worden door de sensor unit voor PID control.
	// Deze lijst wordt gebruikt voor de opbouw van de dialogen.
	// Controleren of alle units die gekoppeld werden als PID control unit nog aanwezig zijn.
	// Indien er units gekoppeld werden die niet meer beschikbaar zijn de P-Binding verwijderen.
	void TMCreateUnitDimListPIDControl(void) {
		CNodeDatabase* pNodeDatabaseDoc = TMGetDocumentNodes();
		ASSERT(0!=pNodeDatabaseDoc);		
		g_listUnitsDimPIDControl.RemoveAll();		// Wissen van de lijst
		if (0!=pNodeDatabaseDoc) {
			int count = 0;
			ASSERT(g_listUnitsDimPIDControl.GetCount()==0);
			count = bindingPropertyUnitSens_pidControlDimUnitListAdd(*pNodeDatabaseDoc,&g_listUnitsDimPIDControl);
			ASSERT(count==g_listUnitsDimPIDControl.GetCount());
			if (count>0) {
				if (FALSE == bindingPropertyUnitSens_pidControlDimUnitsVerify(pNodeDatabaseDoc)) {
					CBindingFiles* const pBindingFiles = TMGetBindingFiles( );					
					if(0!=pBindingFiles) {
						const int count = bindingPropertyUnitSens_pidControlDimUnitsNormalize(pNodeDatabaseDoc,pBindingFiles);
						ASSERT(count!=0);
					}
				}			
			}
		}
	}

	// Wordt aangeroepen bij de start van een nieuwe applicatie en afsluiten.
	// Wissen van de binding & node databases.
	// Wissen van de lijst met sensor units die delegation ondersteunen.
	void TMInitialiseDatabases() {
		CAppNodeDatabase* const pAppNodeDatabase = CAppNodeDatabase::GetInstance();
		CBindingDatabaseApplication* const pAppBinding = TMGetBindingApp();		
		if (0!=pAppBinding) {
			pAppBinding->Clear();		
		}

		if (0!=pAppNodeDatabase) {
			pAppNodeDatabase->GetNodesDocument()->InitDatabase();	
			pAppNodeDatabase->GetNodesRuntime()->InitDatabase();
			pAppNodeDatabase->clearDatabaseExtraUnitInfo( );
		}

		bindingPropertyUnitSens_delegationListDestroy(&g_listUnitsDelegation);
		ASSERT(g_listUnitsDelegation.GetCount()==0);

		bindingPropertyUnitSens_delegationRequestMeasureValListDestroy(&g_listUnitsSensReadMeasuredValue);		
		ASSERT(g_listUnitsSensReadMeasuredValue.GetCount()==0);
	}

	// Opslaan 
	void TMSaveDatabases()
	{
		CBindingDatabaseApplication* const pAppBinding = TMGetBindingApp();
		if (0!=pAppBinding) {
			pAppBinding->Save();
		}
	}

	// inladen.
	void TMLoadDatabases(CStringArray* pBindingFileErrors)
	{
		CAppNodeDatabase* const pAppNodeDatabase = CAppNodeDatabase::GetInstance();
		CBindingDatabaseApplication* const pAppBinding = TMGetBindingApp();
		CNodeDatabase* pNodeDatabaseDoc = NULL;
		CNodeDatabase* pNodeDatabaseRT = NULL;

		ASSERT(0!=pAppNodeDatabase);
		if (0!=pAppNodeDatabase) {
			pAppNodeDatabase->CopyDocumentToRuntimeDatabase();				
			pNodeDatabaseDoc=pAppNodeDatabase->GetNodesDocument();
			pNodeDatabaseRT=pAppNodeDatabase->GetNodesRuntime();			
		}

		ASSERT(NULL!=pNodeDatabaseDoc);
		ASSERT(NULL!=pNodeDatabaseRT);
		ASSERT(NULL!=pAppBinding);

		if (NULL!=pAppBinding) {				
			pAppBinding->CreateBindingFiles();
			pAppBinding->Load(pBindingFileErrors);		
			if (NULL!=pNodeDatabaseDoc) {
				pAppBinding->AssignBindingFilesToNodes(pNodeDatabaseDoc);
			}
			if (NULL!=pNodeDatabaseRT) {
				pAppBinding->AssignBindingFilesToNodes(pNodeDatabaseRT);		
			}
			pAppBinding->UpdateData(TRUE);
		}
	}

	// Nieuwe binding bestanden aanmaken bij een NIEUW project.
	// Normale en engineering mode.
	void TMCreateNewBindingFiles()
	{
		CBindingDatabaseApplication* const pAppBinding = TMGetBindingApp();
		
		TMGetBindingFiles()->ClearDatabase();

		if (0!=pAppBinding)
		{	
			CNodeDatabase* pNodeDatabaseDoc = TMGetDocumentNodes();
			CNodeDatabase* pNodeDatabaseRT = TMGetRuntimeNodes();

			ASSERT(0!=pNodeDatabaseDoc);
			ASSERT(0!=pNodeDatabaseRT);

			pAppBinding->CreateBindingFiles();	
			pAppBinding->AssignBindingFilesToNodes(pNodeDatabaseDoc);
			pAppBinding->AssignBindingFilesToNodes(pNodeDatabaseRT);
		}
	}


	// Deze functie wordt aangeroepen wanneer we online op een nieuwe installatie gebrowsed hebben.
	// Alle Nodes + units (unitspecs) worden gecopieerd van de runtime naar de node database.
	void TMCopyRuntimeToDocDatabase() {
		TMGetNodeDatabaseApp()->CopyRuntimeToDocDatabase();
	}

	// Bij een bestaande installatie:
	// Er werden nodes toegevoegd.
	void TMAddNewNodes() {

		CList<int> listNodes;	// Bevat een lijst met node addressen van de nodes die toegevoegd werden.
		TMGetNodeDatabaseApp()->AddNewNodesToRTDatabase(&listNodes);		

		CBindingDatabaseApplication* const pAppBinding = TMGetBindingApp();		
		if (0==pAppBinding) { 
			ASSERT(0!=pAppBinding);
			return;
		}			

		pAppBinding->CreateBindingFiles();
		pAppBinding->AssignBindingFilesToNodes(TMGetDocumentNodes());

#if(USE_DIMMER_PBINDINGS_MINVALUE == 1)
		if (!listNodes.IsEmpty()) {
			bindingpropertyunitalg_createExtraPBindings(TMGetDocumentNodes(),listNodes);
		}
#endif 
	}


	// Sedert v15.06
	void TMCreateExtraPBindings( ) {
#if(USE_DIMMER_PBINDINGS_MINVALUE == 1)
		CList<int> listNodes;
		CNodeDatabase* const pNodeDatabase = TMGetDocumentNodes();
		if (0 == pNodeDatabase) {
			ASSERT(0 != pNodeDatabase);
			return;
		}
		int count=0;
		pNodeDatabase->NROfNodes(&count);
		ASSERT(count>0);
		for (int i=0;i<count;++i) {
			CCanNode* pNode = 0;
			if (pNodeDatabase->GetNode(i,&pNode)) {
				ASSERT(0 != pNode);
				listNodes.AddTail(pNode->getNodeAddress());	
			}
		}		
		ASSERT(count == listNodes.GetCount());
		if (!listNodes.IsEmpty()) {
			bindingpropertyunitalg_createExtraPBindings(pNodeDatabase,listNodes);
		}
		else {
			ASSERT(!listNodes.IsEmpty());
		}
#endif 
	}


	// Sedert V16.17.
	// Aanmaken van bindings bij een nieuwe installatie.
	// - Master C-Bindings met status conditie bindings...
	// - Twilight module forward bindings naar de master.
	void TMCreateExtraBindings() {
	
		CBindingEntries* const pBindingEntries = TMGetBindingEntries();
		CNodeDatabase* const pNodeDatabase = TMGetDocumentNodes();		
		int count = 0;

		count = CBindingConversionAuto2_ConditionStatus_Helper::generateMasterStatusCBindings(*pBindingEntries,*pNodeDatabase);
		if (count>0) {
	
		}
		count = CBindingConversionAuto2_Forward_Helper::generateTwilightForwardBindings(*pBindingEntries,*pNodeDatabase);
		if (count>0) {
	
		}
	}



	// Changes versie 0x0900.
	// - functie bindingPropertyUnitSens_checkBindingsNodeRemoved( ) toegevoegd zodat de 
	//   buddy's van de property sensor unit verwijderd worden.
	//	 Oppassen: Deze functie maakt gebruik van de nodedatabase dus moet aangeroepen
	//	 worden voordat de node verwijderd wordt uit de nodedatabase
	//
	// Changes versie 0x0C03
	// - functie bindingPropertyUnitSens_checkBindingsNodeRemoved( )  uitgebreid zodat ook
	//   de bindings verwijderd worden voor de modbus sensor unit die een sensor gekoppeld
	//   heeft van de node die verwijderd wordt.
	// - functie bindingPropertyUnitSens_delegationVerify( ) toegevoegd.
	//   De verify() werd vroeger hier uitgevoerd. Nu verplaatst en bijkomende verify
	//   voor de modbus sensor unit
	//
	// Changes versie 0x1000
	// - functie bindingPropertyUnitSens_checkBindingsNodeRemoved() uitgebreid zodat ook
	//   alle P-bindings gecontroleerd worden voor de PID control unit.

	unsigned short TMDeleteNodefromDatabases(BYTE bNodeAddress)
	{
		unsigned short usRetVal = 0;		
		CNodeDatabase* const pNodeDatabase = TMGetDocumentNodes();
		CBindingDatabaseApplication* const pAppBinding = TMGetBindingApp();

		if ((0!=pNodeDatabase)&&(0!=pAppBinding)) {		
			CBindingFiles* const pBindingFiles=TMGetBindingFiles();

			if (0!=pBindingFiles) {			
				// Zie opmerking.
				bindingPropertyUnitSens_checkBindingsNodeRemoved(pNodeDatabase,pBindingFiles,bNodeAddress);
			} 

			if (pNodeDatabase->DeleteNodeAddress(bNodeAddress)) {				
				if (0!=pBindingFiles) {
					pBindingFiles->Delete(bNodeAddress);
				}
				usRetVal=pAppBinding->Delete(bNodeAddress);
			}

#if (DEBUG_VERIFY_SENS_DELEGATION==1)
			// Controleren of alle units die gekoppeld zijn via delegation zich in de nodedatabase bevinden.
			bindingPropertyUnitSens_delegationVerify(pNodeDatabase);
#endif
		}
		return usRetVal;

	} /* unsigned short TMDeleteNodefromDatabases(BYTE bNodeAddress) */


	void TMUpgradeIncompatibleNodes(BYTE bNodeAddress)
	{		
		CAppNodeDatabase* const pAppNodeDatabase = CAppNodeDatabase::GetInstance();
		CCanNode* pNodeDoc = 0;
		CCanNode* pNodeNetwork = 0;

		int j = 0;
		if (pAppNodeDatabase->GetNodesDocument()->FindNode(bNodeAddress,&j,&pNodeDoc) &&
			pAppNodeDatabase->GetNodesRuntime()->FindNode(bNodeAddress,&j,&pNodeNetwork))
		{			
			// Sedert V15.03:
			// Upgraden van een TCP/IP als master naar een touchscreen.
			// Moeten enkel het node type aanpassen. De andere node properties en resources 
			// worden upgedate in de UpdateNodesWithRTSpecs()
#if(USE_UPGRADE_TCPIP_MASTER == 1)
			ASSERT(0 != pNodeDoc);
			ASSERT(0 != pNodeNetwork);
			if (pNodeDoc->bNodeType == NODE_PIN_GATEWAY && pNodeDoc->bNodeFlags & NODE_MASTER_LCD &&
				pNodeNetwork->bNodeType == NODE_LCD && pNodeNetwork->bNodeFlags & NODE_MASTER_LCD) 
			{
				pNodeDoc->bNodeType = pNodeNetwork->bNodeType;
			}
#endif 
			const int MAX_UNITS = 255;
			//pUnits is een array van 255 points naar STRUCT_UNIT
			STRUCT_UNIT* pUnits[MAX_UNITS];	
			unsigned short count = pAppNodeDatabase->findIncompatibleUnits(pNodeDoc,pNodeNetwork,&pUnits[0],MAX_UNITS);
			if (count > 0) {
				CBindingDatabaseApplication* const pAppBinding = TMGetBindingApp();
				for (int i=0;i<count;i++) {
					if (pUnits[i] != 0) {
						pAppBinding->Delete(pUnits[i]->bNodeLogicalAddress,pUnits[i]->bUnitAddress);					
					}
				}
				pAppNodeDatabase->replaceIncompatibleNodes(pNodeDoc,pNodeNetwork);
			}		
		}
	}
	
	
	void TMUpdateDocumentNodeDatabase(BOOL copyNames) {
		TMAssignBindingFilesToRTNodes();			
		TMGetNodeDatabaseApp()->UpdateNodesWithRTSpecs(copyNames);		// BUG-0130
		TMUpgradeBindingUnitTypes();									// TM20110708: TM_AUDIOHC4_BINDING_INPUT
		TMUpgradeBindingPropertyUnits();		
		TMCreateUnitListDelegation();

		// Nieuwe units toevoegen aan de lijst 'g_listUnitsSensReadMeasuredValue'.
		// Deze lijst bevat alle units die kunnen gebruikt worden om de gemeten waarde op te vragen.
		TMCreateUnitListRequestMeasuredValues();	


		// Sedert V16.00: USE_SENSOR_PIDCONTROL	
		// Een lijst aanmaken van alle dimmer units die kunnen gebruikt worden door de sensor unit voor PID control.
		// Controleren of alle units die gekoppeld werden als PID control unit nog aanwezig zijn.
		// Indien er units gekoppeld werden die niet meer beschikbaar zijn de P-Binding verwijderen.
		TMCreateUnitDimListPIDControl();

		// Sedert V16.41:
		// De extra (DALI) control input units moeten hun naam behouden wanneer deze gebruikt worden 
		// in een binding.
		// Alle Control units die extra aangemaakt werden in de smartbox ultra wordt de DEFAULT_NAME_OVERRIDDEN 
		// vlag gezet.
		TMSetControlUnitsDefaultNameOverridden( );

		// Sedert V16.42:
		// Extra info voor de units.
		// Momenteel wordt deze enkel gebruikt om een extra tekstlabel weer te geven bij de units.
		TMSetDatabaseUnitExtraInfo( );
	}


	// Sedert V16.43: Browse single node.
	//
	// Deze functie moet aangeroepen worden na een 'Browse for single node'.
	// Zorgt ervoor dat alle nieuwe units op de juiste manier toegevoegd worden in het systeem +
	// dat deze units kunnen gebruikt worden in de bindings.
	// Taken:
	//	- Document node uitbreiden met de units van de runtime node + unitspecs copieren.
	//  - Upgraden binding unit types.
	//	- Bepalen van de mogelijkheden van de binding unit ifv de node en product code.
	//  - Extra:
	//		- Kunnen de nieuwe sensor gebuikt worden voor delegation / modbus opvragen van de 
	//		  gemeten waarde.
	//		- Kunnen de nieuwe units aangestuurd worden via een PID sensor unit.
	//		- Moeten we de namen van de nieuwe units blijven en niet overschreven worden wanneer er 
	//		  een binding aangemaakt wordt (= Control input units van DALI).
	//		- Mogelijkheid om een extra tekst label aan te maken voor de nieuwe units.
	void TMUpdateDocumentNode(BYTE bNodeAddress) {
		CAppNodeDatabase* const pAppNodeDatabase = CAppNodeDatabase::GetInstance();
		if (NULL == pAppNodeDatabase) {
			INVARIANT(NULL != pAppNodeDatabase);
			return;
		}	
	
		// Document node updaten met de specs van de runtime node.
		// Indien er nieuwe units bijgemaakt werden de units van de doc node uitbreiden.
		// De Binding unit wordt gekoppeld aan pUnit.
		const BOOL copyNames = TRUE;
		const BOOL addNewUnits = TRUE;
		if (FALSE == pAppNodeDatabase->UpdateNodesWithRTSpecs(bNodeAddress, copyNames, addNewUnits)) {
			ASSERT(0);
		}
	
		// Bij een bestaande installatie.
		// Binding Unit Types upgraden ifv het unitType en bConfig. (Momenteel wordt enkel Audio Extended ondersteund.)
		// Moet in principe niet aangeroepen worden omdat de er voor de nieuwe units geen bindings aangemaakt werden.
		// Doet wel een controle op de bestaande nodes in de database...	
		TMUpgradeBindingUnitTypes(bNodeAddress);

		// De node opzoeken in de node database.
		// Nodig voor onderstaande functies.
		int dummy = 0;
		CCanNode* pNode = NULL;
		if (FALSE == pAppNodeDatabase->GetNodesDocument()->FindNode(bNodeAddress,&dummy,&pNode)) {
			ASSERT(0);
			return;	
		}

		// Zorgt ervoor dat alle binding units de juiste functionaliteiten krijgen ifv de node en 
		// het product id. Dit o.a. nodig voor:
		//	- Forward bindings van outputs, 
		//  - Sensor unit delegetion.
		//	- Ondersteuning PID.
		//	- Ondersteuning van specifieke P-Bindings.	
		INVARIANT(NULL != pNode);
		TMUpgradeBindingPropertyUnits(pNode);
				
		// Lijst updaten van de units die kunnen gebruikt worden voor delegation.
		// Wanneer een buddy niet gevonden wordt dan wordt de koppeling/binding gewist.
		//
		// TODO LOW-PRIO: Onderstaande functie uitwerken enkel op deze node.
		//				  Momenteel wordt deze functie uitgevoerd op de volledige database.		
		TMCreateUnitListDelegation( );			

		// Nieuwe units toevoegen aan de lijst 'g_listUnitsSensReadMeasuredValue'.
		// Deze lijst bevat alle units die kunnen gebruikt worden om de gemeten waarde op te vragen.
		INVARIANT(NULL != pNode);
		TMCreateUnitListRequestMeasuredValues(*pNode);

		// Nieuwe units toevoegen aan de lijst van alle dimmer units die kunnen gebruikt worden door de sensor unit voor PID control.
		// Deze lijst wordt gebruikt voor de opbouw van de dialogen.
		INVARIANT(NULL != pNode);
		TMCreateUnitDimListPIDControl(*pNode);

		// Bij sommige Control units die extra aangemaakt werden mag de naam niet veranderen.
		// - Bij de smartbox ultra krijgen de DALI control units hun naam via de DALI configuratie.
		//   Deze units moeten hun naam blijven behouden tijdens het maken van een binding.
		INVARIANT(NULL != pNode);
		TMSetControlUnitsDefaultNameOverridden(*pNode);

		// Sedert V16.46: Inladen van de DALi bridge config.
		// Mogelijk zijn er nieuwe units bijgekomen die gekoppeld werden via de DALI bridge config.
		TMLoadDaliBridgeConfig(bNodeAddress);

		// Bijhouden van extra info voor de units.
		// - Extra tekst label:
		//	 - Smartbox ultra: Weergave zoals op de sticker.
		INVARIANT(NULL != pNode);
		TMSetDatabaseUnitExtraInfo(*pNode);
	}


	void TMRestoreRuntimeNode(BYTE bNodeAddress) {	
		CAppNodeDatabase* const pAppNodeDatabase = CAppNodeDatabase::GetInstance();
		if (NULL == pAppNodeDatabase) {
			INVARIANT(NULL != pAppNodeDatabase);
			return;
		}	
		pAppNodeDatabase->CopyDocumentNodeToRuntimeNode(bNodeAddress);
	}

	// Sedert V16.46:
	// Dali bridge config inladen en info koppelen aan de Duotecno units.
	void TMLoadDaliBridgeConfig( ) {		
		CAppNodeDatabase* const pAppNodeDatabase = CAppNodeDatabase::GetInstance();
		if (NULL == pAppNodeDatabase) {
			INVARIANT(NULL != pAppNodeDatabase);
			return;
		}	
		CNodeDatabase* const pNodeDatabase = pAppNodeDatabase->GetNodesDocument();
		if (NULL == pNodeDatabase) {
			INVARIANT(NULL != pNodeDatabase);
			return;
		}

		// Alle nodes overlopen...
		CNodeIterator it(pNodeDatabase->CreateNodeIterator());
		for (it.Begin();!it.IsDone();it.Next()) {
			CCanNode* const pNode = it.CurrentItem( );
			if (NULL != pNode) {
				if (CNodeConfigInfo_SmartboxUltra::isType(pNode)) {					
					_TMLoadDaliBridgeConfig(pNodeDatabase, pNode);
				}
			}
		}
	}

	void TMLoadDaliBridgeConfig(int nodeAddress) {
		CAppNodeDatabase* const pAppNodeDatabase = CAppNodeDatabase::GetInstance();
		if (NULL == pAppNodeDatabase) {
			INVARIANT(NULL != pAppNodeDatabase);
			return;
		}	
		CNodeDatabase* const pNodeDatabase = pAppNodeDatabase->GetNodesDocument();
		if (NULL == pNodeDatabase) {
			INVARIANT(NULL != pNodeDatabase);
			return;
		}
		CCanNode* pNode = NULL;
		int dummy = 0;
		if (FALSE == pNodeDatabase->FindNode(nodeAddress,&dummy,&pNode)) {
			ASSERT(0);
			return;
		}
		INVARIANT(NULL != pNode);
		_TMLoadDaliBridgeConfig(pNodeDatabase, pNode);
	}

	// Bij een bestaande installatie.
	// Binding Unit Types upgraden ifv het unitType en bConfig & DALI units
	//
	// Alle binding units van de bestaande binding entries worden overlopen. 
	// Er wordt nagekeken in de node database aan welke unit type deze gekoppeld zijn.
	// Ifv het unit type + configvlaggen wordt het type van de binding unit ge-upgrade zodat er 
	// meer mogelijkheden zijn.
	void TMUpgradeBindingUnitTypes(int nodeAddress) {
		CBindingDatabaseApplication* const pAppBinding = TMGetBindingApp();
		ASSERT(0!=pAppBinding);
		if (0!=pAppBinding) {	
			pAppBinding->UpgradeBindingUnitTypes(nodeAddress);	
		}
	}


}; /* end namespace */


