#include "stdafx.h"
#include "AppRemoteCfgApi.h"
/**************************************************************/
#define REQUIRE				ASSERT
/**************************************************************/
#define USE_TESTCASE_WRITENAMES_WRONG_VALUES		1
/**************************************************************/
namespace DUOTECNO {
/**************************************************************/


// Test functie om de fsm te testen.
// Versturen van de namen...

bool appremotecfg_test_startTaskNamesWrite(IAppRemoteCfgApi* instance) {

	const int nodeAddress = 0x45;		// Node address van de homeserver node...

	AppCfgRemoteActionList list;
	list.add(new AppCfgRemoteActionWriteName(nodeAddress, -1, "node name"));

#if(USE_TESTCASE_WRITENAMES_WRONG_VALUES ==1)
	// Een node + units die niet aanwezig zijn in het netwerk...
	list.add(new AppCfgRemoteActionWriteName(nodeAddress, 254, "unit name n/a"));
#endif 

	for (int i=0;i<32;++i) {
		char szUnitName[32];
		sprintf(szUnitName,"unit name %d",i);
		list.add(new AppCfgRemoteActionWriteName(nodeAddress, i, szUnitName));
	}

#if(USE_TESTCASE_WRITENAMES_WRONG_VALUES ==1)
	// Een node + units die niet aanwezig zijn in het netwerk...
	list.add(new AppCfgRemoteActionWriteName(0x44, -1, "node name"));
	list.add(new AppCfgRemoteActionWriteName(0x44, 0x00, "unit name"));
	list.add(new AppCfgRemoteActionWriteName(0x44, 0x01, "node name"));
	list.add(new AppCfgRemoteActionWriteName(0x44, 0x02, "node name"));
#endif

	const bool result = appremotecfg_startTaskNamesWrite(instance,list);
	return result;
}


/**************************************************************/
}; /* end namespace*/
/**************************************************************/