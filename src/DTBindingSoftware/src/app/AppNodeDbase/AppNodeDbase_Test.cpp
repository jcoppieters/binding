#include "stdafx.h"
#include "BindingSoftware.h"
#include "appnodedbase_Test.h"

// Test functie om een aantal nodes toe te voegen aan de node database...
/*****************************************************************/

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
AppNodeDbaseTest::AppNodeDbaseTest(CNodeDatabase* pNodeDatabase) : 
	m_pNodeDatabase(pNodeDatabase) { }

AppNodeDbaseTest::~AppNodeDbaseTest( ) { }

/**************************** public *****************************/
bool AppNodeDbaseTest::testAddNewNodes(int numberOfNodes,int newNodeAddress) 
{
	int nodeAddress = 0;
	if (!getNextAvailableAddress(&nodeAddress)) {
		ASSERT(0);
		return false;
	}

	// Dimmer nodes aanmaken.
	int physicalAddress = 0x054879;
	for (int i = 0; i < numberOfNodes; ++i) {
		CCanNode* pNode = NULL;
		int dummy = 0;
		if (!m_pNodeDatabase->AddNode(nodeAddress,&dummy,&pNode)) {
			return false;
		}
		if (!configNode(physicalAddress,NODE_STD_DIMMER,pNode)) {
			return false;		
		}		

		if (newNodeAddress > 0) {
			pNode->bLogicalAddress = newNodeAddress;
		}
		physicalAddress++;
		nodeAddress++;
	}
	return true;
}

	// Test functie om een node te verwijderen uit de nodedatbase
bool AppNodeDbaseTest::testRemoveNode(int nodeAddress) {

	if (m_pNodeDatabase->DeleteNodeAddress(nodeAddress)) {
		return true;
	}
	return false;
}

bool AppNodeDbaseTest::testChangeNodeUnit(int nodeAddress,int unitAddress,int unitType) {
	
	CCanNode* pNode = NULL;
	int dummy = 0;
	if (!m_pNodeDatabase->FindNode(nodeAddress,&dummy,&pNode)) {
		return false;
	}

	pNode->setPhysicalAddress(pNode->getPhysicalAddress() + 1);		// Physical address wijzigen...

	STRUCT_UNIT* pUnit = NULL;
	if (!m_pNodeDatabase->FindUnit(pNode,unitAddress,&dummy,&pUnit)) {
		return false;
	}

	pUnit->bUnitType = unitType;
	CNodeDatabase::InitRuntimeUnitSpecs(pUnit);
	return true;
}

bool AppNodeDbaseTest::testSimulateNoMasterFoundPossibleAssignSboxUltra( ) {

	static int physicalNodeAddres = 0x0B0000;

	m_pNodeDatabase->DeleteNodeAddress(NODE_LCD_MASTER_ADDRESS);

	int dummy=0;
	CCanNode* pNode = NULL;
	if (FALSE == m_pNodeDatabase->AddPhysicalNode(physicalNodeAddres,&dummy,&pNode)) {
		return false;
	}

	pNode->setNodeAddress(NODE_FACTORY_ADDRESS);
	pNode->bNodeType = NODE_PIN_GATEWAY;
	physicalNodeAddres++;
	return true;
}

/*************************** protected ***************************/

// Node Address opvragen - Neemt gewoon het laatste beschikbare adres.
// Neemt geen addressen van tussen de nodes.
bool AppNodeDbaseTest::getNextAvailableAddress(int* nodeAddress) {

	int count = 0;
	int tempNodeAddress = 0;
	m_pNodeDatabase->NROfNodes(&count);
	for (int i=0;i<count;++i) {
		CCanNode* pNode = NULL;
		if (m_pNodeDatabase->GetNode(i,&pNode)) {
			if (pNode->getNodeAddress()>tempNodeAddress && pNode->getNodeAddress() < 0xFC) {
				tempNodeAddress = pNode->getNodeAddress()+1;
			}
		}	
	}
	if (tempNodeAddress>=0x03 && tempNodeAddress<0xFD) {
		*nodeAddress = tempNodeAddress;
		return true;
	}
	return false;
}

bool AppNodeDbaseTest::configNode(int physicalAddress, NodeCreateType_t type, CCanNode* pNode) 
{
	pNode->setPhysicalAddress(physicalAddress);
	pNode->bNodeFlags = 0;
	pNode->bNROfUnits = 0;

	pNode->setNodeBindingsXORChecksum(0);
	pNode->setNodeBindingsNrEntries(0);	

	char nodeName[16];
	sprintf(nodeName,"A%06X",physicalAddress);
	strcpy(pNode->szNodeName, nodeName);

	switch(type) {
		case NODE_STD_DIMMER: {

			pNode->bNodeType = NODE_PIN_STD_UNITS;

			int dummy = 0;
			for (int i=0;i<6;++i) {
				STRUCT_UNIT* pUnit = NULL;
				if (!m_pNodeDatabase->AddUnit(pNode,i,&dummy,&pUnit)) {				
					return false;
				}

				char unitName[16];
				sprintf(unitName,"LU%02X%02X|A%06X",pNode->getNodeAddress(),i,physicalAddress);
				strcpy(pUnit->szUnitName, unitName);

				pUnit->bUnitType = UNIT_TYPE_DIM;	
				pUnit->bNodeLogicalAddress = pNode->getNodeAddress();

				CNodeDatabase::InitRuntimeUnitSpecs(pUnit);
			}
		} return true;	
	}
	return false;
}

/*****************************************************************/