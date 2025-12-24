#include "stdafx.h"
#include "appdalidevices_t.h"
#include "appdali_def.h"
/***********************************************************************************/
namespace DUOTECNO {
namespace DALI {
/***********************************************************************************/

/********************************** private ****************************************/
bool appdalidevices_test_addControlGear(CAppDaliDevices* pDevices,int shortAddress) {

	CAppDaliDevice* const pNewDevice = new CAppDaliDevice(DALI_DEVICE_TYPE_CONTROLGEAR);
	if (NULL == pNewDevice) {
		return false;
	}

	pNewDevice->setShortAddress(shortAddress);
	pNewDevice->setGTIN(0x0123456789);
	pNewDevice->setSerialcode(0x9876543210);
	pNewDevice->setVersionFirmware(0x10);
	pNewDevice->setVersionHardware(0x10);


	// Properties standaard laten...


	if (!pDevices->add(pNewDevice)) {
		delete pNewDevice;
		return false;
	}
	return true;
}


bool appdalidevices_test_addControlDevice(CAppDaliDevices* pDevices,int shortAddress) {

	CAppDaliDevice* const pNewDevice = new CAppDaliDevice(DALI_DEVICE_TYPE_INPUTCONTROL);
	if (NULL == pNewDevice) {
		return false;
	}

	// Algemene properties...
	pNewDevice->setShortAddress(shortAddress);
	pNewDevice->setGTIN(0x0123456789);
	pNewDevice->setSerialcode(0x9876543210);
	pNewDevice->setVersionFirmware(0x10);
	pNewDevice->setVersionHardware(0x10);

	// Properties controlDevice....
	CAppDaliDeviceProperties_InputControlDevice* const properties = 
			dynamic_cast<CAppDaliDeviceProperties_InputControlDevice*>(&pNewDevice->getProperties());
	if (NULL == properties) {
		return false;
	}
	properties->setPowerCycleNotification(false);
	properties->setRandomAddress(0xffffff);
	properties->setAppControllerEnabled(false);
	properties->setDeviceStatus(0);
	properties->setOperatingMode(0);		/* ?? */
	properties->setVersionNumber(2);		/* Dali versie : todo bekijken in de specs */


	// Instance properties...
	enum { NR_OF_INSTANCES = 24 };	
	CAppDaliInputControlDeviceInstances& refInstances = properties->getInstances();	
	refInstances.setNumberOfInstances(NR_OF_INSTANCES);

	int instanceNumber = 0;
	for (instanceNumber = 0; instanceNumber < NR_OF_INSTANCES; ++instanceNumber) {
		CAppDaliInputControlDeviceInstance* const pInstance = new CAppDaliInputControlDeviceInstance(instanceNumber);
		if (NULL == pInstance) {
			return false;
		}
		pInstance->setInstanceActive(true);
		pInstance->setEventScheme(DALI_CONTROLDEVICE_EVENT_SCHEME_DEFAULT);				
		if (instanceNumber < 4) {
			pInstance->setInstanceType(DALI_SYSTEM_CONTROLDEVICE_INSTANCE_TYPE_PUSHBUTTON);									
			pInstance->setEventFilter(DALI_CONTROLDEVICE_PUSHBUTTON_DEFAULT_EVENTFILTER);	
		}
		else {
			pInstance->setInstanceType(DALI_SYSTEM_CONTROLDEVICE_INSTANCE_TYPE_DETECTOR_OCCUPANCY);									
			pInstance->setEventFilter(DALI_CONTROLDEVICE_DETECTOR_DEFAULT_EVENTFILTER);	
		}
		pInstance->setInstanceGroup(0,0xff);	
		pInstance->setInstanceGroup(1,0xff);	
		pInstance->setInstanceGroup(2,0xff);	
		pInstance->setEventPriority(3);	
		pInstance->setEventResolution(0);				// ?????????????
		pInstance->setStatus(0);						// ?????????????

		if (!refInstances.add(instanceNumber,pInstance)) {
			delete pInstance;			
		}
	}

	if (!pDevices->add(pNewDevice)) {
		delete pNewDevice;
		return false;
	}
	return true;		
}

/********************************** public *****************************************/
bool appdalidevices_test_buildDatabase(CAppDaliDevices* pDevices) {

	bool result = false;

	// Control devices
	appdalidevices_test_addControlDevice(pDevices,63);
	appdalidevices_test_addControlDevice(pDevices,62);
	appdalidevices_test_addControlDevice(pDevices,61);
	appdalidevices_test_addControlDevice(pDevices,60);

	// Control gear devices
	appdalidevices_test_addControlGear(pDevices,0);
	appdalidevices_test_addControlGear(pDevices,1);
	appdalidevices_test_addControlGear(pDevices,2);
	appdalidevices_test_addControlGear(pDevices,3);

	return result;
}
/***********************************************************************************/
};
};
/***********************************************************************************/