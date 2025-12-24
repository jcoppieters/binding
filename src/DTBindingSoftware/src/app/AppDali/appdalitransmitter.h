#pragma once

/***********************************************************************************/
struct ITcpSocketHomeClient;

/***********************************************************************************/
namespace DUOTECNO {
namespace DALI {
/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/

class CAppDaliTransmitter {
public:
	CAppDaliTransmitter(ITcpSocketHomeClient* pSocket);

	/************************************ Control Gear devices ***********************************/
	// Algemene configuratie: Control gear.
	bool sendControlgearBroadcastInitialiseAll(int deviceAddress);			// device=0 (=all control gear), 0xff = device gear zonder short address.	
	bool sendControlgearBroadcastTerminate( );
	bool sendControlgearBroadcastRandomize( );
	bool sendControlgearBroadcastWithdraw( );
	bool sendControlgearProgramShortAddress(int address);
	bool sendControlgearBroadcastSearchAddrL(int data);
	bool sendControlgearBroadcastSearchAddrM(int data);
	bool sendControlgearBroadcastSearchAddrH(int data);
	bool sendControlgearBroadcastCompare();
	bool sendQueryControlGearBroadcastPresent( );

	// Control gear config: Data aanwezig in DTR0 wordt gebruikt.
	bool sendControlGearMaxLevel(int shortAddress);
	bool sendControlGearMinLevel(int shortAddress);
	bool sendControlGearSystemFailureLevel(int shortAddress);
	bool sendControlGearPowerOnLevel(int shortAddress);
	bool sendControlGearFadeTime(int shortAddress);
	bool sendControlGearFadeRate(int shortAddress);
	bool sendControlGearExtendedFadeTime(int shortAddress);
	bool sendControlGearAddToScene(int shortAddress,int sceneNumber);
	bool sendControlGearRemoveFromScene(int shortAddress, int sceneNumber);
	bool sendControlGearAddtogroup(int shortAddress, int group);
	bool sendControlGearRemoveFromgroup(int shortAddress, int group);
	bool sendControlSetShortAddress(int shortAddress);						// DTR0 waarde.
	bool sendControlGearDTR0(int value);
	bool sendControlGearDTR1(int value);
	bool sendControlGearDTR2(int value);
	bool sendControlGearEnableDeviceType(int value);

	// DT6 Speciale extended commandos.
	bool sendQueryControlGearDT6DimmerCurve(int shortAddress);
	bool sendControlGearDT6SelectDimmerCurve(int shortAddress);

	// DT8 Speciale extended commandos.
	bool sendQueryControlGearDT8ColorTypeFeatures(int shortAddress);
	bool sendQueryControlGearDT8ColourValue(int shortAddress);			// DTR0 bevat de selectie

	// DT8 Speciale extended commandos.
	bool sendControlGearDT8SetTemporaryColorTemp(int shortAddress);				// DTR0=lsb, DTR1=MSB
	bool sendControlGearDT8SetTemporaryColorCoordinateX(int shortAddress);		// DTR0=lsb, DTR1=MSB
	bool sendControlGearDT8SetTemporaryColorCoordinateY(int shortAddress);		// DTR0=lsb, DTR1=MSB
	bool sendControlGearDT8SetTemporaryColorDimLevelRGB(int shortAddress);		// DTR0=red,DTR1=green,DTR2=blue
	bool sendControlGearDT8SetTemporaryColorDimLevelWAF(int shortAddress);		// DTR0=white,DTR1=amber,DTR2=freecolor
	bool sendControlGearDT8SetTemporaryColorPrimaryChannelN(int shortAddress);		// DTR0=lsb,DTR1=msb,DTR2=channelNR

	// Control gear bediening: Device.
	bool sendControlGearDeviceActualLevel(int shortAddress,int value);
	bool sendControlGearDeviceOff(int shortAddress);
	bool sendControlGearRecallMaxLevel(int shortAddress);
	bool sendControlGearRecallMinLevel(int shortAddress);
	bool sendControlGearDimStepUp(int shortAddress);
	bool sendControlGearDimStepDown(int shortAddress);
	bool sendControlGearDimUp(int shortAddress);
	bool sendControlGearDimDown(int shortAddress);
	bool sendControlGearGoToScene(int shortAddress,int sceneNumber);

	// Control gear group config.
	bool sendControlGearGroupMaxLevel(int groupAddress);
	bool sendControlGearGroupMinLevel(int groupAddress);
	bool sendControlGearGroupSystemFailureLevel(int groupAddress);
	bool sendControlGearGroupPowerOnLevel(int groupAddress);
	bool sendControlGearGroupFadeTime(int groupAddress);
	bool sendControlGearGroupFadeRate(int groupAddress);

	// Control gear bediening: Group.
	bool sendControlGearGroupActualLevel(int groupAddress,int value);
	bool sendControlGearGroupOff(int groupAddress);
	bool sendControlGearGroupRecallMaxLevel(int groupAddress);
	bool sendControlGearGroupRecallMinLevel(int groupAddress);
	bool sendControlGearGroupStepUp(int groupAddress);
	bool sendControlGearGroupStepDown(int groupAddress);
	bool sendControlGearGroupDimUp(int groupAddress);
	bool sendControlGearGroupDimDown(int groupAddress);

	// Control gear: Queries.	
	bool sendControlGearQuerySceneLevel(int shortAddress,int sceneNumber);
	bool sendQueryControlGearDeviceReadDTR0(int shortAddress);
//	bool sendQueryControlGearDeviceReadMemoryLocation(int shortAddress);
	bool sendQueryControlGearDTR0(int shortAddress);
	bool sendQueryControlGearMemory(int shortAddress);	
	bool sendQueryControlGearDeviceStatus(int shortAddress);	
	bool sendQueryControlGearPresent(int shortAddress);	
	bool sendQueryControlGearGroups_0_7(int shortAddress);	
	bool sendQueryControlGearGroups_8_15(int shortAddress);	
	bool sendQueryControlGearDeviceType(int shortAddress);
	bool sendQueryControlGearNextDeviceType(int shortAddress);
	bool sendQueryControlGearVersionNumber(int shortAddress);
	bool sendQueryControlGearOperatingMode(int shortAddress);
	bool sendQueryControlGearActualLevel(int shortAddress);
	bool sendQueryControlGearMinLevel(int shortAddress);
	bool sendQueryControlGearMaxLevel(int shortAddress);
	bool sendQueryControlGearFadeTimeRate(int shortAddress);
	bool sendQueryControlGearPhysicalMinValue(int shortAddress);
	bool sendQueryControlGearPowerOnLevel(int shortAddress);
	bool sendQueryControlGearSystemFailureLevel(int shortAddress);
	bool sendQueryControlGearExtendedFadeTime(int shortAddress);
	bool sendQueryControlGearShortAddress();

	/************************************ Control Input devices ***********************************/
	// Algemene configuratie: Control input devices.
	bool startQuiescentMode();
	bool stopQuienscentMode();
	bool sendControlInputDevicesBroadcastinitialiseAll(int deviceAddress);	// 0xff gebruiken bij het zoeken naar input devices. 0x7F gebruiken bij het toekennen van nieuwe devices.
	bool sendControlInputDevicesBroadcastTerminate();
	bool sendControlInputDevicesBroadcastRandomize();
	bool sendControlInputDevicesBroadcastSearchAddrL(int data);
	bool sendControlInputDevicesBroadcastSearchAddrM(int data);
	bool sendControlInputDevicesBroadcastSearchAddrH(int data);
	bool sendControlInputDevicesBroadcastCompare();
	bool sendControlInputDevicesBroadcastWithdraw();
	bool sendControlInputDevicesProgramShortAddress(int shortAddress);
	bool sendQueryControlInputDevicesShortAddress();
	bool sendQueryBroadcastInputControlDeviceStatus( );

	// Control input device.
	bool sendControlInputDeviceIdentify(int shortAddress);
	bool sendControlInputDeviceApplicationControllerEnable(int shortAddress);
	bool sendControlInputDeviceApplicationControllerDisable(int shortAddress);
	bool sendControlInputDevicesSetShortAddressDTR0(int shortAddress);
	bool sendControlInputDevicesSetOperatingModeDTR0(int shortAddress);

	// Control input devices: Queries
	bool sendQueryInputControlDeviceStatus(int shortAddress);
	bool sendQueryControlInputDeviceReadMemoryLocation(int shortAddress);
	bool sendQueryControlInputDeviceReadDTR0(int shortAddress);
	bool sendQueryControlInputDeviceVersionNumber(int shortAddress);
	bool sendQueryControlInputDeviceNumberOfInstances(int shortAddress);
	bool sendQueryControlInputDeviceReadDTR1(int shortAddress);
	bool sendQueryControlInputDeviceReadDTR2(int shortAddress);
	bool sendQueryControlInputDeviceAppControllerEnabled(int shortAddress);
	bool sendQueryControlInputDeviceOperatingMode(int shortAddress);
	bool sendQueryControlInputDeviceGroups_0_7(int shortAddress);
	bool sendQueryControlInputDeviceGroups_8_15(int shortAddress);
	bool sendQueryControlInputDeviceGroups_16_23(int shortAddress);
	bool sendQueryControlInputDeviceGroups_24_31(int shortAddress);
	bool sendQueryControlInputDevicePowerCycleNotification(int shortAddress);
	bool sendQueryControlInputDeviceCapabilities(int shortAddress);	
	bool sendQueryControlInputDeviceExtVersionNumber(int shortAddress);
	bool sendQueryControlInputDeviceRandomAddressH(int shortAddress);
	bool sendQueryControlInputDeviceRandomAddressM(int shortAddress);
	bool sendQueryControlInputDeviceRandomAddressL(int shortAddress);


	// Control input devices: uitlezen van de memory map...
	bool sendControlInputDevicesBroadcastWriteDTR1_DTR0(int dtr0,int dtr1);			
	bool sendControlInputDevicesBroadcastWriteDTR2_DTR1(int dtr1, int dtr2);
	bool sendControlInputDevicesBroadcastDTR0(int dtr0);
	bool sendControlInputDevicesBroadcastDTR1(int dtr1);
	bool sendControlInputDevicesBroadcastDTR2(int dtr2);
	bool sendControlInputDevicesBroadcastSetShortAddressDTR0( );
	bool sendControlInputDevicesBroadcastRemoveFromDeviceGroups0_15();
	bool sendControlInputDevicesBroadcastRemoveFromDeviceGroups16_31();

	// TODO_DALI_TRANSMITTER: Control devices.
	// bool sendQueryControlInputDeviceMissingShortAddress(int shortAddress);		TODO: DALI_CMD_24BIT_QUERY_MISSING_SHORT_ADDRESS
		// MOETEN DIT COMMANDO IN Broadcast mode versturen	- TODO NAKIJKEN IN DE LOGGER...

	// Control device instance Queries.
	bool sendQueryControlInputDeviceInstanceType(int shortAddress,int instanceAddress);
	bool sendQueryControlInputDeviceInstanceResolution(int shortAddress,int instanceAddress);
	bool sendQueryControlInputDeviceInstanceError(int shortAddress,int instanceAddress);
	bool sendQueryControlInputDeviceInstanceStatus(int shortAddress,int instanceAddress);
	bool sendQueryControlInputDeviceInstanceEventPriority(int shortAddress,int instanceAddress);
	bool sendQueryControlInputDeviceInstanceEnabled(int shortAddress,int instanceAddress);
	bool sendQueryControlInputDeviceInstancePrimaryGroup(int shortAddress,int instanceAddress);
	bool sendQueryControlInputDeviceInstanceGroup1(int shortAddress,int instanceAddress);
	bool sendQueryControlInputDeviceInstanceGroup2(int shortAddress,int instanceAddress);
	bool sendQueryControlInputDeviceInstanceEventScheme(int shortAddress,int instanceAddress);
	bool sendQueryControlInputDeviceInstanceInputValue(int shortAddress,int instanceAddress);
	bool sendQueryControlInputDeviceInstanceInputValueLatch(int shortAddress,int instanceAddress);
	bool sendQueryControlInputDeviceInstanceFeatureType(int shortAddress,int instanceAddress);
	bool sendQueryControlInputDeviceInstanceNextFeatureType(int shortAddress,int instanceAddress);
	bool sendQueryControlInputDeviceInstanceEventFilter0_7(int shortAddress,int instanceAddress);
	bool sendQueryControlInputDeviceInstanceEventFilter8_15(int shortAddress,int instanceAddress);
	bool sendQueryControlInputDeviceInstanceEventFilter16_23(int shortAddress,int instanceAddress);
	bool sendQueryControlInputDeviceInstanceConfiguration(int shortAddress,int instanceAddress);
	bool sendQueryControlInputDeviceInstanceAvailableTypes(int shortAddress,int instanceAddress);

	// Control device instance commandos.
	bool sendControlInputDeviceInstanceEventPrioDTR0(int shortAddress,int instanceAddress);
	bool sendControlInputDeviceInstanceEnableInstance(int shortAddress,int instanceAddress);
	bool sendControlInputDeviceInstanceDisableInstance(int shortAddress,int instanceAddress);
	bool sendControlInputDeviceInstancePrimaryGroupDTR0(int shortAddress,int instanceAddress);
	bool sendControlInputDeviceInstanceGroup1DTR0(int shortAddress,int instanceAddress);
	bool sendControlInputDeviceInstanceGroup2DTR0(int shortAddress,int instanceAddress);
	bool sendControlInputDeviceInstanceEventSchemeDTR0(int shortAddress,int instanceAddress);
	bool sendControlInputDeviceInstanceEventFilterDTR0DTR1DTR2(int shortAddress,int instanceAddress);
	bool sendControlInputDeviceInstanceType(int shortAddress,int instanceAddress);

	// Occupancy device/instance commands.
	bool sendQueryControlInputDeviceInstanceOccupancyReportTimerInterval(int shortAddress,int instanceAddress);
	bool sendQueryControlInputDeviceInstanceOccupancyHoldTimerInterval(int shortAddress,int instanceAddress);
	bool sendQueryControlInputDeviceInstanceOccupancyDeadTimerInterval(int shortAddress,int instanceAddress);									
	bool sendControlInputDeviceInstanceOccupancyReportTimerInterval(int shortAddress,int instanceAddress);		// DTR0, send twice
	bool sendControlInputDeviceInstanceOccupancyHoldTimerInterval(int shortAddress,int instanceAddress);		// DTR0, send twice
	bool sendControlInputDeviceInstanceOccupancyDeadTimerInterval(int shortAddress,int instanceAddress);		// DTR0, send twice

protected:
	bool sendDaliMsg(int nrOfBits, const BYTE* pbData,int prioLevel = 1);
	
	// algemene functie: Control gear
	bool sendControlGearDeviceCommand(int shortAddress,int command,int prioLevel = 1);
	bool sendControlGearGroupCommand(int groupAddress,int command,int prioLevel = 1);
	

	// algemene functie: Control input device
	bool sendControlInputDeviceCommand(int shortAddress,int command,int prioLevel = 1);
	bool sendControlInputDeviceInstanceCommand(int shortAddress,int instanceNumber,int command,int prioLevel = 1);			
	bool sendControlInputBroadcastDeviceCommand(int command,int prioLevel = 1);

private:
	ITcpSocketHomeClient* m_pSocket;
	unsigned short m_clientIdentifier;
	BYTE m_messageIdentifier;
};
/***********************************************************************************/
};
};
/***********************************************************************************/