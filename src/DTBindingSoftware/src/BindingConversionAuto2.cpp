#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingConversionAuto2.h"

#include "BindingEntry.h"
#include "BindingEntry_Helper.h"
#include "BindingDatabaseDirector.h"

#include "NodeDatabase.h"
#include "NodeDatabase_util.h"

/********************************************** public ******************************************************/

CBindingConversionAuto2::CBindingConversionAuto2(CBindingEntries& refBindingEntries) : 
	m_refBindingEntries(refBindingEntries), m_bindingName( ) { }

CBindingConversionAuto2::CBindingConversionAuto2(CBindingEntries& refBindingEntries, const CString& bindingName) :
	m_refBindingEntries(refBindingEntries), m_bindingName(bindingName)
{
}
/******************************************* protected ******************************************************/
BOOL CBindingConversionAuto2::findUnit(CNodeDatabase& refNodeDatabase,const Unit& refUnitInfo,STRUCT_UNIT** ppUnit,BOOL updateName)
{
	CCanNode* pNode = NULL;
	int dummy = 0;
	if (!refNodeDatabase.FindNode(refUnitInfo.nodeAddress,&dummy,&pNode)) {
		return FALSE;
	}		
	STRUCT_UNIT* pUnit = NULL;
	if (!refNodeDatabase.FindUnit(pNode,refUnitInfo.unitAddress,&dummy,&pUnit)) {
		return FALSE;
	}

	if (pUnit->bUnitType != refUnitInfo.unitType) {
		return FALSE;
	}

	if (updateName) {
		if (!refUnitInfo.name.IsEmpty()) {
			memcpy(pUnit->szUnitName,refUnitInfo.name,sizeof(pUnit->szUnitName));
			pUnit->szUnitName[sizeof(pUnit->szUnitName)-1] = '\0';
		}
	}

	*ppUnit = pUnit;
	return TRUE;
}

BOOL CBindingConversionAuto2::findUnits(CNodeDatabase& refNodeDatabase,CList<const Unit*>& listInputs,CList<STRUCT_UNIT*>* pListUnits,BOOL updateName) {
	
	POSITION pos = listInputs.GetHeadPosition();
	while(pos!=NULL) {
		const Unit* pInfo = NULL;	
		if ((pInfo = (const Unit*)listInputs.GetNext(pos)) == NULL) {
			return FALSE;
		}
		STRUCT_UNIT* pUnit = NULL;
		if (!findUnit(refNodeDatabase,*pInfo,&pUnit,updateName)) {
			return FALSE;
		}			
		pListUnits->AddTail(pUnit);				
	}
	ASSERT(pListUnits->GetCount() == listInputs.GetCount());
	return TRUE;
}

int CBindingConversionAuto2::addOutputVirtual(CBindingEntry* pDest,STRUCT_UNIT* pUnit,int function,int stateOnOff) {

	if (UNIT_TYPE_LCD_VIRTUAL != pUnit->bUnitType) {
		return 0;
	}

	CBindingVirtualUnit bindingUnit(BINDING_UNIT_TYPE_LCD_VIRTUAL);
	bindingUnit.SetNodeAddress(pUnit->bNodeLogicalAddress);
	bindingUnit.SetUnitAddress(pUnit->bUnitAddress);
	bindingUnit.SetBindingUnitType(BINDING_UNIT_TYPE_LCD_VIRTUAL);
	bindingUnit.SetName(pUnit->szUnitName);						
	bindingUnit.SetInOutType(PM_INOUTTYPE_OUTPUT);
	bindingUnit.Set_Function(function);			
	bindingUnit.Set_ControlState(stateOnOff);
	return CBindingEntry_Helper::AddBindingUnit(pDest,bindingUnit);
}

int CBindingConversionAuto2::addInputVirtual(CBindingEntry* pDest,STRUCT_UNIT* pUnit,int ev,int eventData) {

	if (UNIT_TYPE_LCD_VIRTUAL != pUnit->bUnitType) {
		return 0;
	}

	CBindingVirtualUnit bindingUnit(BINDING_UNIT_TYPE_LCD_VIRTUAL);
	bindingUnit.SetNodeAddress(pUnit->bNodeLogicalAddress);
	bindingUnit.SetUnitAddress(pUnit->bUnitAddress);
	bindingUnit.SetBindingUnitType(BINDING_UNIT_TYPE_LCD_VIRTUAL);
	bindingUnit.SetName(pUnit->szUnitName);								
	bindingUnit.SetInOutType(PM_INOUTTYPE_INPUT);
	bindingUnit.Set_Event(ev);	
	bindingUnit.Set_ControlState(eventData);
	return CBindingEntry_Helper::AddBindingUnit(pDest,bindingUnit);
}

int CBindingConversionAuto2::addInputControl(CBindingEntry* pDest,STRUCT_UNIT* pUnit,int ev,int eventData) {

	if (UNIT_TYPE_CONTROL != pUnit->bUnitType) {
		return 0;
	}

	CBindingVirtualUnit bindingUnit(BINDING_UNIT_TYPE_CONTROL);
	bindingUnit.SetNodeAddress(pUnit->bNodeLogicalAddress);
	bindingUnit.SetUnitAddress(pUnit->bUnitAddress);
	bindingUnit.SetBindingUnitType(BINDING_UNIT_TYPE_CONTROL);
	bindingUnit.SetName(pUnit->szUnitName);								
	bindingUnit.SetInOutType(PM_INOUTTYPE_INPUT);
	bindingUnit.Set_Event(ev);	
	bindingUnit.Set_ControlState(eventData);
	return CBindingEntry_Helper::AddBindingUnit(pDest,bindingUnit);
}


BOOL CBindingConversionAuto2::addBindingEntry(CBindingEntry& bindingEntry) {

	// NOTE Oppassen: 
	// bindingEntry = input arg
	// pNewBindingEntry = output arg.
	const BOOL notifyGUI = FALSE;
	const BOOL modifyNames = FALSE;
	
	BindingStatusError::BindingError_t bindingError = BindingStatusError::BINDING_SUCCESS;
	CBindingEntry* pNewBindingEntry = NULL;
	const unsigned short id = CBindingDatabaseDirector::addBindingEntry(&m_refBindingEntries,&bindingEntry,&pNewBindingEntry,
																		modifyNames, notifyGUI, &bindingError);

	if (0 == id) {	

		// todo: kunnen hier evt bindingError controleren...

		ASSERT(0 != id);
		return FALSE;
	}

	// zie NOTE.
	// Vanaf hier moeten we pNewBindingEntry gebruiken om bijkomende properties aan te passen. 
	if (NULL == pNewBindingEntry) {
		ASSERT(NULL != pNewBindingEntry);
		return FALSE;
	}

	ASSERT(id == pNewBindingEntry->GetID());

	if (!m_bindingName.IsEmpty()) {
		pNewBindingEntry->SetName(m_bindingName);
	}

	// Automatisch aangemaakt bij een nieuwe installatie.
	pNewBindingEntry->SetApplicationFlags(CBindingEntry::APPFLAG_AUTO_GENERATED);

	return TRUE;
}

void CBindingConversionAuto2::addLinkInput2Operator(CBindingEntry* pDest,int inputId,int operatorId) {
	CBindingLink bindingLink;
	bindingLink.SetInLink(inputId,PM_UNIT);
	bindingLink.SetOutLink(operatorId,PM_OPERATOR);							
	CBindingEntry_Helper::AddBindingLink(pDest,bindingLink);
}

void CBindingConversionAuto2::addLinkOperator2Output(CBindingEntry* pDest,int operatorId,int outputId) {	
	CBindingLink bindingLink;
	bindingLink.SetInLink(operatorId,PM_OPERATOR);	
	bindingLink.SetOutLink(outputId,PM_UNIT);
	CBindingEntry_Helper::AddBindingLink(pDest,bindingLink);
}

void CBindingConversionAuto2::addLinkUnits(CBindingEntry* pDest,int inputId,int outputId) {
	CBindingLink bindingLink;
	bindingLink.SetInLink(inputId,PM_UNIT);
	bindingLink.SetOutLink(outputId,PM_UNIT);				
	CBindingEntry_Helper::AddBindingLink(pDest,bindingLink);
}

/********************************************** public ******************************************************/
CBindingConversionAuto2_ConditionStatus::CBindingConversionAuto2_ConditionStatus(CBindingEntries& refBindingEntries) : 
	CBindingConversionAuto2(refBindingEntries) { }

CBindingConversionAuto2_ConditionStatus::CBindingConversionAuto2_ConditionStatus(CBindingEntries& refBindingEntries, const CString& bindingName) :
	CBindingConversionAuto2(refBindingEntries, bindingName)
{
}

CBindingConversionAuto2_ConditionStatus::Result_t CBindingConversionAuto2_ConditionStatus::generate(
															 CNodeDatabase& refNodeDatabase,
															 CList<const Unit*>& listInputs, const Unit& output, 
															 int bindingFlags, OperatorType type)
{
	const int nrOfInputs = listInputs.GetSize();
	if (nrOfInputs < 2) {
		ASSERT(nrOfInputs>=2);		// min 2 inputs.
		return RESULT_EC_GENERAL;	
	}

	// De units opzoeken --> Inputs + de namen van de units aanpassen.
	CList<STRUCT_UNIT*> listUnitsInputs(nrOfInputs);
	if (!findUnits(refNodeDatabase,listInputs,&listUnitsInputs,TRUE)) {
		return RESULT_EC_NODEDATABASE;
	}

	// De Units opzoeken -> Outputs + de naam aanpassen.
	STRUCT_UNIT* pUnitOutput = NULL;
	if (!findUnit(refNodeDatabase,output,&pUnitOutput,TRUE)) {
		return RESULT_EC_NODEDATABASE;
	}

	// De bindings aanmaken...
	if (!generate_autobinding_condition(listUnitsInputs,pUnitOutput,bindingFlags,type)) {
		return RESULT_EC_BINDINGS;
	}
	return RESULT_SUCCESS;
}



// Aanmaken van een conditie binding.
// Inputs: Event 02 + 1
// Output: Short puls + state
BOOL CBindingConversionAuto2_ConditionStatus::generate_autobinding_condition(CList<STRUCT_UNIT*>& listInputs, STRUCT_UNIT* pUnitOutput, 
																   int bindingFlags, OperatorType type)
{
	const int nrOfInputs = listInputs.GetSize();
	if (nrOfInputs<=0) {
		return FALSE;
	}

	unsigned short outputId = 0;
	unsigned short idOperator = 0;
	unsigned short* m_arrInputId = new unsigned short[nrOfInputs];
	if (NULL == m_arrInputId) {
		return FALSE;
	}

	BOOL fContinue = TRUE;
	CBindingEntry bindingEntry;	
	if (fContinue) {
		int index = 0;
		POSITION pos = listInputs.GetHeadPosition();
		while(pos!=NULL) {
			STRUCT_UNIT* pUnit = NULL;		
			if ((pUnit = (STRUCT_UNIT*) listInputs.GetNext(pos)) != NULL) {
				m_arrInputId[index] = addInput(&bindingEntry, pUnit);
				if (0 == m_arrInputId[index]) {
					ASSERT(index<nrOfInputs);
					fContinue = FALSE;
					break;
				}				
				index++;
			}
		}
	}

	if (fContinue) {
		idOperator = CBindingEntry_Helper::AddBindingOperator(&bindingEntry,type);
		if (0 == idOperator) {
			fContinue = FALSE;
		}
	}

	if (fContinue) {
		outputId = addOutput(&bindingEntry,pUnitOutput);
		if (0 == outputId) {
			fContinue = FALSE;
		}
	}

	// Binding links aanmaken.
	// Eerst deze van de inputs naar de operator, daarna van de operator naar de output unit.
	if (fContinue) {
		for (int i=0;i<nrOfInputs;++i) {
			addLinkInput2Operator(&bindingEntry,m_arrInputId[i],idOperator);
		}
		addLinkOperator2Output(&bindingEntry,idOperator,outputId);
	}

	if (fContinue) {
		// Binding properties...
		bindingEntry.SetBindingReturnFlags(bindingFlags);				
	}

	if (NULL != m_arrInputId) {
		delete [] m_arrInputId;
	}

	if (fContinue) {
		return addBindingEntry(bindingEntry);
	}
	return FALSE;
}

unsigned short CBindingConversionAuto2_ConditionStatus::addInput(CBindingEntry* pDest,STRUCT_UNIT* pUnit)
{
	if (NULL == pUnit) {
		ASSERT(NULL != pUnit);
		return 0;
	}
	const BYTE bUnitType = pUnit->bUnitType;
	if (UNIT_TYPE_LCD_VIRTUAL != bUnitType) {
		ASSERT(UNIT_TYPE_LCD_VIRTUAL == bUnitType);			// Momenteel enkel virtuals !!
		return 0;	
	}
	return addInputVirtual(pDest,pUnit,EV_UNITCONTROLPULSTOGGLE,1);
}

unsigned short CBindingConversionAuto2_ConditionStatus::addOutput(CBindingEntry* pDest,STRUCT_UNIT* pUnit) {

	if (NULL == pUnit) {
		ASSERT(NULL != pUnit);
		return 0;
	}
	const BYTE bUnitType = pUnit->bUnitType;
	if (UNIT_TYPE_LCD_VIRTUAL != bUnitType) {
		ASSERT(UNIT_TYPE_LCD_VIRTUAL == bUnitType);		// Momenteel enkel virtuals !!
		return 0;	
	}
	return addOutputVirtual(pDest,pUnit,VIRTUAL_OBJECT_METHOD_PULSTOGGLE,1);
}

/*****************************************************************************************************/


/********************************************** public ******************************************************/

CBindingConversionAuto2_Forward::CBindingConversionAuto2_Forward(CBindingEntries& refBindingEntries) : 
	CBindingConversionAuto2(refBindingEntries) { }

CBindingConversionAuto2_Forward::CBindingConversionAuto2_Forward(CBindingEntries& refBindingEntries, const CString& bindingName) :
	CBindingConversionAuto2(refBindingEntries, bindingName)
{

}

// Aanmaken van een forward long binding.
// Inputs: Event 01 + 1
// Output: Short puls + state
CBindingConversionAuto2_Forward::Result_t CBindingConversionAuto2_Forward::generate(CNodeDatabase& refNodeDatabase, 
																   const Unit& input, const Unit& output, 
																   int bindingFlags)
{
	// De Units opzoeken + de naam aanpassen.
	STRUCT_UNIT* pUnitInput = NULL;
	if (!findUnit(refNodeDatabase,input,&pUnitInput,TRUE)) {
		return RESULT_EC_NODEDATABASE;
	}

	// De Units opzoeken + de naam aanpassen.
	STRUCT_UNIT* pUnitOutput = NULL;
	if (!findUnit(refNodeDatabase,output,&pUnitOutput,TRUE)) {
		return RESULT_EC_NODEDATABASE;
	}

	// De binding aanmaken...
	if (!generate_autobinding_ForwardLong(pUnitInput,pUnitOutput,bindingFlags)) {
		return RESULT_EC_BINDINGS;
	}
	return RESULT_SUCCESS;
}


BOOL CBindingConversionAuto2_Forward::generate_autobinding_ForwardLong(STRUCT_UNIT* pInput, STRUCT_UNIT* pOutput, int bindingFlags) {

	CBindingEntry bindingEntry;	
	BOOL fContinue = TRUE;
	int inputID = 0;
	int outputID = 0;

	// Add input...
	if (fContinue) {
		inputID = addInput(&bindingEntry,pInput);
		if (0 == inputID) {
			fContinue = FALSE;
		}
	}
	// Add output...
	if (fContinue) {
		outputID = addOutput(&bindingEntry,pOutput);
		if (0 == inputID) {
			fContinue = FALSE;
		}
	}

	// Link.
	if (fContinue) {
		addLinkUnits(&bindingEntry,inputID,outputID);
	}
	
	if (fContinue) {
		bindingEntry.SetBindingReturnFlags(bindingFlags);				
	}

	if (fContinue) {
		return addBindingEntry(bindingEntry);
	}
	return FALSE;
}

unsigned short CBindingConversionAuto2_Forward::addInput(CBindingEntry* pDest,STRUCT_UNIT* pUnit) {
	if (NULL == pUnit) {
		ASSERT(NULL != pUnit);
		return 0;
	}
	const BYTE bUnitType = pUnit->bUnitType;
	if (UNIT_TYPE_CONTROL != bUnitType) {
		ASSERT(UNIT_TYPE_CONTROL == bUnitType);		// Momenteel enkel virtuals !!
		return 0;	
	}
	return addInputControl(pDest,pUnit,EV_UNITCONTROLTOGGLE,1);
}

unsigned short CBindingConversionAuto2_Forward::addOutput(CBindingEntry* pDest,STRUCT_UNIT* pUnit) {
	if (NULL == pUnit) {
		ASSERT(NULL != pUnit);
		return 0;
	}
	const BYTE bUnitType = pUnit->bUnitType;
	if (UNIT_TYPE_LCD_VIRTUAL != bUnitType) {
		ASSERT(UNIT_TYPE_LCD_VIRTUAL == bUnitType);		// Momenteel enkel virtuals !!
		return 0;	
	}
	return addOutputVirtual(pDest,pUnit,VIRTUAL_OBJECT_METHOD_PULSTOGGLE,1);
}
/*****************************************************************************************************/
/*****************************************************************************************************/
/*****************************************************************************************************/
int CBindingConversionAuto2_ConditionStatus_Helper::generateMasterStatusCBindings(CBindingEntries& refBindingEntries,CNodeDatabase& refNodeDatabase)
{
	int count = 0;
	if (generateBindindCondition0(refBindingEntries,refNodeDatabase)) {
		count++;
	}
	if (generateBindindCondition1(refBindingEntries,refNodeDatabase)) {
		count++;
	}
	if (generateBindindCondition2(refBindingEntries,refNodeDatabase)) {
		count++;
	}
	return count;
}

/******************************************* protected ***********************************************/

static BOOL isMasterLCDNode(CNodeDatabase& refNodeDatabase) {
	int dummy = 0;
	CCanNode* pNode = NULL;
	if (!refNodeDatabase.FindNode(0xfc,&dummy,&pNode)) {
		return FALSE;
	}
	return (NODE_LCD == pNode->bNodeType);
}


BOOL CBindingConversionAuto2_ConditionStatus_Helper::generateBindindCondition0(CBindingEntries& refBindingEntries,CNodeDatabase& refNodeDatabase) {
	
	CBindingConversionAuto2_ConditionStatus bindingConvAuto(refBindingEntries,"Status1");

	const CBindingConversionAuto2::Unit in0_lcd("Out1",0xfc, 0x00, UNIT_TYPE_LCD_VIRTUAL);
	const CBindingConversionAuto2::Unit in1_lcd("Out2",0xfc, 0x01, UNIT_TYPE_LCD_VIRTUAL);
	const CBindingConversionAuto2::Unit in0_gw("Condition",0xfc, 0x00, UNIT_TYPE_LCD_VIRTUAL);
	const CBindingConversionAuto2::Unit in1_gw("Condition",0xfc, 0x01, UNIT_TYPE_LCD_VIRTUAL);
	const CBindingConversionAuto2::Unit out("Help virtual",0xfc,0x09, UNIT_TYPE_LCD_VIRTUAL);

	CList<const CBindingConversionAuto2::Unit*> list(2);

	if (isMasterLCDNode(refNodeDatabase)) {
		list.AddTail(&in0_lcd);
		list.AddTail(&in1_lcd);
	}
	else {
		list.AddTail(&in0_gw);
		list.AddTail(&in1_gw);
	}

	if (bindingConvAuto.generate(refNodeDatabase,list,out) == CBindingConversionAuto2::RESULT_SUCCESS) {
		return TRUE;
	}	
	return FALSE;
}

BOOL CBindingConversionAuto2_ConditionStatus_Helper::generateBindindCondition1(CBindingEntries& refBindingEntries,CNodeDatabase& refNodeDatabase) {

	CBindingConversionAuto2_ConditionStatus bindingConvAuto(refBindingEntries,"Status2");

	const CBindingConversionAuto2::Unit in0("Twilight",0xfc, 0x02, UNIT_TYPE_LCD_VIRTUAL);
	const CBindingConversionAuto2::Unit in1("Dark outside",0xfc, 0x03, UNIT_TYPE_LCD_VIRTUAL);
	const CBindingConversionAuto2::Unit in2("After 22h", 0xfc, 0x04, UNIT_TYPE_LCD_VIRTUAL);
	const CBindingConversionAuto2::Unit out("Help virtual",0xfc,0x09, UNIT_TYPE_LCD_VIRTUAL);

	CList<const CBindingConversionAuto2::Unit*> list(3);
	list.AddTail(&in0);
	list.AddTail(&in1);
	list.AddTail(&in2);

	if (bindingConvAuto.generate(refNodeDatabase,list,out) == CBindingConversionAuto2::RESULT_SUCCESS) {
		return TRUE;
	}	
	return FALSE;
}

BOOL CBindingConversionAuto2_ConditionStatus_Helper::generateBindindCondition2(CBindingEntries& refBindingEntries,CNodeDatabase& refNodeDatabase) {
	
	CBindingConversionAuto2_ConditionStatus bindingConvAuto(refBindingEntries,"Status3");

	const CBindingConversionAuto2::Unit in0("Condition 1",0xfc, 0x05, UNIT_TYPE_LCD_VIRTUAL);
	const CBindingConversionAuto2::Unit in1("Condition 2",0xfc, 0x06, UNIT_TYPE_LCD_VIRTUAL);
	const CBindingConversionAuto2::Unit in2("Condition 3", 0xfc, 0x07, UNIT_TYPE_LCD_VIRTUAL);
	const CBindingConversionAuto2::Unit in3("Condition 4", 0xfc, 0x08, UNIT_TYPE_LCD_VIRTUAL);

	const CBindingConversionAuto2::Unit out("Help virtual",0xfc,0x09, UNIT_TYPE_LCD_VIRTUAL);

	CList<const CBindingConversionAuto2::Unit*> list(4);
	list.AddTail(&in0);
	list.AddTail(&in1);
	list.AddTail(&in2);
	list.AddTail(&in3);

	if (bindingConvAuto.generate(refNodeDatabase,list,out) == CBindingConversionAuto2::RESULT_SUCCESS) {
		return TRUE;
	}	
	return FALSE;
}
/*****************************************************************************************************/

// Aanmaken van forward bindings naar de conditie inputs van de master.
int CBindingConversionAuto2_Forward_Helper::generateTwilightForwardBindings(CBindingEntries& refBindingEntries,CNodeDatabase& refNodeDatabase)
{
	// Zoeken naar het node address van een twilight module.
	int nodeAddress = findNodeTwilight(refNodeDatabase);
	if (nodeAddress < 0) {
		return 0;
	}

	int count = 0;
	if (generateTwilightBinding0(refBindingEntries,refNodeDatabase,nodeAddress)) {
		count++;
	}
	if (generateTwilightBinding1(refBindingEntries,refNodeDatabase,nodeAddress)) {
		count++;
	}
	return count;
}


// Zoeken naar een twilight module.
// Indien er geen gevonden wordt -> return waarde -1.
int CBindingConversionAuto2_Forward_Helper::findNodeTwilight(CNodeDatabase& refNodeDatabase)
{	
	CList<int> listNodeAddresses;
	if (CNodeDatabase_Util::findNodeTwilight(refNodeDatabase,&listNodeAddresses) <= 0) {
		return -1;
	}
	ASSERT(listNodeAddresses.GetCount()>=1);
	const int retVal = listNodeAddresses.GetHead();				// Node address van de eerste.
	return retVal;
}

/*
	{"Active          "},		0x08
	{"Sun             "},		0x09
	{"Sun|Cloudy      "},		0x0A
    {"Cloudy          "},		0x0B
    {"Cloudy|Twilight "},		0x0C
    {"Twilight        "},		0x0D
    {"Twilight|Dark   "},		0x0E
    {"Dark            "}		0x0F
*/
BOOL CBindingConversionAuto2_Forward_Helper::generateTwilightBinding0(CBindingEntries& refBindingEntries,CNodeDatabase& refNodeDatabase,int nodeAddress) {

	CBindingConversionAuto2_Forward bindingConvAuto(refBindingEntries,"Twilight");
	const CBindingConversionAuto2::Unit in("Twilight",nodeAddress, 0x0D, UNIT_TYPE_CONTROL);	// Input twilight
	const CBindingConversionAuto2::Unit out("Twilight",0xfc, 0x02, UNIT_TYPE_LCD_VIRTUAL);		// Output master

	if (bindingConvAuto.generate(refNodeDatabase,in,out) == CBindingConversionAuto2::RESULT_SUCCESS) {
		return TRUE;
	}	
	return FALSE;
}

BOOL CBindingConversionAuto2_Forward_Helper::generateTwilightBinding1(CBindingEntries& refBindingEntries,CNodeDatabase& refNodeDatabase,int nodeAddress) {

	CBindingConversionAuto2_Forward bindingConvAuto(refBindingEntries,"Dark");
	const CBindingConversionAuto2::Unit in("Dark",nodeAddress, 0x0F, UNIT_TYPE_CONTROL);		// Input twilight
	const CBindingConversionAuto2::Unit out("Dark outside",0xfc, 0x03, UNIT_TYPE_LCD_VIRTUAL);	// Output master

	if (bindingConvAuto.generate(refNodeDatabase,in,out) == CBindingConversionAuto2::RESULT_SUCCESS) {
		return TRUE;
	}	
	return FALSE;
}