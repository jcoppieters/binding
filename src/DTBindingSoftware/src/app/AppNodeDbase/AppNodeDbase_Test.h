#pragma once
/*****************************************************************/
#include "NodeDatabase.h"
/*****************************************************************/

// Test functies voor het testen van de Problem solve wizard.
struct AppNodeDbaseTest
{
public:
	AppNodeDbaseTest(CNodeDatabase* pNodeDatabase);
	~AppNodeDbaseTest( );

	// Test functie om een aantal nodes toe te voegen aan de node database...
	bool testAddNewNodes(int numberOfNodes,int nodeAddress = -1);

	// Test functie om een node te verwijderen uit de nodedatbase
	bool testRemoveNode(int nodeAddress);

	// Test functie om een node incompatibel te maken.
	// Een unit van de node krijgt een ander unit type.
	bool testChangeNodeUnit(int nodeAddress,int unitAddress,int unitType);

	// Test functie om te simuleren dat er geen master aanwezig is in de database.
	// Er wordt een node aangemaakt van het type smartbox ultra (dmv het fysisch adres).
	bool testSimulateNoMasterFoundPossibleAssignSboxUltra( );


protected:
	// Opvragen van het volgende node address.
	bool getNextAvailableAddress(int* nodeAddress);

	// Een node aanmaken.
	typedef enum {
		NODE_STD_DIMMER = 0,		// Node met 6 dimmer units...
	} NodeCreateType_t;

	bool configNode(int physicalAddress, NodeCreateType_t type,CCanNode* ppNode);

private:
	 CNodeDatabase* m_pNodeDatabase;
};
/*****************************************************************/