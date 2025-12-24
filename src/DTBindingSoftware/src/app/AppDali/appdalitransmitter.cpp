#include "stdafx.h"
#include "appdalitransmitter.h"
#include "appdaliprotocol_def.h"
#include "appdali_def.h"
#include "communication/tcpsocket/homeclient/itcpsockethomeclient.h"
/***********************************************************************************/
namespace DUOTECNO {
namespace DALI {
/***********************************************************************************/
static const int DALI_BIT_SELECTOR		= (1<<0);
static const int DALI_BIT_GROUPADDRESS	= (1<<7);
/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
CAppDaliTransmitter::CAppDaliTransmitter(ITcpSocketHomeClient* pSocket) : 
	m_pSocket(pSocket), m_clientIdentifier(0), m_messageIdentifier(0) { }

// Algemene configuratie.
bool CAppDaliTransmitter::sendControlgearBroadcastInitialiseAll(int deviceAddress) {
	BYTE pbData[2] = { 0,0 };
	ASSERT(deviceAddress == 0x00 || deviceAddress == 0xff);
	pbData[0] = 0xA5;
	pbData[1] = deviceAddress;
	return this->sendDaliMsg(16,pbData);
}

bool CAppDaliTransmitter::sendControlgearBroadcastTerminate( ) {
	static const BYTE pbData[2] = { DALI_CMD_SPECIAL_TERMINATE, 0x00};
	return this->sendDaliMsg(16,pbData);
}

bool CAppDaliTransmitter::sendControlgearBroadcastRandomize() {
	static const BYTE pbData[2] = { DALI_CMD_SPECIAL_RANDOMIZE, 0x00 };
	return this->sendDaliMsg(16, pbData);
}

bool CAppDaliTransmitter::sendControlgearBroadcastWithdraw( ) {
	static const BYTE pbData[2] = { DALI_CMD_SPECIAL_WITHDRAW, 0x00 };			// NALIJKEN....
	return this->sendDaliMsg(16, pbData);
}

bool CAppDaliTransmitter::sendControlgearProgramShortAddress(int address) {
	BYTE pbData[2] = { 0,0 };
	pbData[0] = DALI_CMD_SPECIAL_PROGRAM_SHORTADDR;
	pbData[1] =((address << 1) + 0x01);						
	return this->sendDaliMsg(16, pbData);
}

bool CAppDaliTransmitter::sendControlgearBroadcastSearchAddrL(int data) {
	BYTE pbData[2] = { 0,0 };
	pbData[0] = DALI_CMD_SPECIAL_SEARCHADDRL;
	pbData[1] = data;
	return this->sendDaliMsg(16, pbData);
}

bool CAppDaliTransmitter::sendControlgearBroadcastSearchAddrM(int data) {
	BYTE pbData[2] = { 0,0 };
	pbData[0] = DALI_CMD_SPECIAL_SEARCHADDRM;
	pbData[1] = data;
	return this->sendDaliMsg(16, pbData);
}

bool CAppDaliTransmitter::sendControlgearBroadcastSearchAddrH(int data) {
	BYTE pbData[2] = { 0,0 };
	pbData[0] = DALI_CMD_SPECIAL_SEARCHADDRH;
	pbData[1] = data;
	return this->sendDaliMsg(16, pbData);
}

bool CAppDaliTransmitter::sendControlgearBroadcastCompare() {
	static const BYTE pbData[2] = { DALI_CMD_SPECIAL_COMPARE,0 };
	return this->sendDaliMsg(16, pbData);
}

// Control gear config: Data aanwezig in DTR0 wordt gebruikt.
bool CAppDaliTransmitter::sendControlGearMaxLevel(int shortAddress) {
	return this->sendControlGearDeviceCommand(shortAddress, DALI_CMD_SET_DTR0_MAX_LEVEL);
}
bool CAppDaliTransmitter::sendControlGearMinLevel(int shortAddress) {
	return this->sendControlGearDeviceCommand(shortAddress, DALI_CMD_SET_DTR0_MIN_LEVEL);
}
bool CAppDaliTransmitter::sendControlGearSystemFailureLevel(int shortAddress) {
	return this->sendControlGearDeviceCommand(shortAddress, DALI_CMD_SET_DTR0_SYSTEM_FAILURE_LEVEL);
}
bool CAppDaliTransmitter::sendControlGearPowerOnLevel(int shortAddress) {
	return this->sendControlGearDeviceCommand(shortAddress, DALI_CMD_SET_DTR0_POWERONLEVEL);
}
bool CAppDaliTransmitter::sendControlGearFadeTime(int shortAddress) {
	return this->sendControlGearDeviceCommand(shortAddress, DALI_CMD_SET_DTR0_FADETIME);
}
bool CAppDaliTransmitter::sendControlGearFadeRate(int shortAddress) {
	return this->sendControlGearDeviceCommand(shortAddress, DALI_CMD_SET_DTR0_FADERATE);
}
bool CAppDaliTransmitter::sendControlGearExtendedFadeTime(int shortAddress) {
	return this->sendControlGearDeviceCommand(shortAddress, DALI_CMD_SET_DTR0_EXTENDED_FADETIME);
}
bool CAppDaliTransmitter::sendControlGearAddToScene(int shortAddress, int sceneNumber) {
	ASSERT(sceneNumber >= 0 && sceneNumber < 16);	
	return this->sendControlGearDeviceCommand(shortAddress, DALI_CMD_SET_SCENE + sceneNumber);
}
bool CAppDaliTransmitter::sendControlGearRemoveFromScene(int shortAddress, int sceneNumber) {
	ASSERT(sceneNumber >= 0 && sceneNumber < 16);
	return this->sendControlGearDeviceCommand(shortAddress, DALI_CMD_SET_REMOVE_FROM_SCENE + sceneNumber);
}
bool CAppDaliTransmitter::sendControlGearAddtogroup(int shortAddress, int group) {
	ASSERT(group >= 0 && group < 16);
	return this->sendControlGearDeviceCommand(shortAddress, DALI_CMD_SET_ADD_TO_GROUP + group);	
}
bool CAppDaliTransmitter::sendControlGearRemoveFromgroup(int shortAddress, int group) {
	ASSERT(group >= 0 && group < 16);
	return this->sendControlGearDeviceCommand(shortAddress, DALI_CMD_SET_REMOVE_FROM_GROUP + group);
}
bool CAppDaliTransmitter::sendControlSetShortAddress(int shortAddress) {						// DTR0 waarde.
	return this->sendControlGearDeviceCommand(shortAddress, DALI_CMD_SET_DTR0_SHORTADDRESS);
}

bool CAppDaliTransmitter::sendControlGearQuerySceneLevel(int shortAddress,int sceneNumber) {
	return this->sendControlGearDeviceCommand(shortAddress, DALI_CMD_QUERY_SCENE_LEVEL + sceneNumber);
}

bool CAppDaliTransmitter::sendQueryControlGearDeviceReadDTR0(int shortAddress) {
	return this->sendControlGearDeviceCommand(shortAddress, DALI_CMD_QUERY_CONTENT_DTR0);
}

// Algemene configuratie: Control input devices.
bool CAppDaliTransmitter::startQuiescentMode() {
	static const BYTE pbData[3] = { 0xff, 0xfe, DALI_CMD_24BIT_START_QUIESCENT_MODE };
	return this->sendDaliMsg(24, pbData);
}

bool CAppDaliTransmitter::stopQuienscentMode() {
	static const BYTE pbData[3] = { 0xff, 0xfe, DALI_CMD_24BIT_STOP_QUIESCENT_MODE };
	return this->sendDaliMsg(24, pbData);
}

bool CAppDaliTransmitter::sendControlInputDevicesBroadcastinitialiseAll(int deviceAddress) {
	BYTE pbData[3] = { 0,0,0 };
	pbData[0] = 0xC1;
	pbData[1] = 0x01;
	pbData[2] = deviceAddress;
	return this->sendDaliMsg(24, pbData);
}

bool CAppDaliTransmitter::sendControlInputDevicesBroadcastTerminate() {
	static const BYTE pbData[3] = { 0xC1, 0x00, 0x00 };
	return this->sendDaliMsg(24, pbData);
}

bool CAppDaliTransmitter::sendControlInputDevicesBroadcastRandomize() {
	static const BYTE pbData[3] = { 0xC1, 0x02, 0x00 };
	return this->sendDaliMsg(24, pbData);
}

bool CAppDaliTransmitter::sendControlInputDevicesBroadcastSearchAddrL(int data) {
	BYTE pbData[3] = { (DALI_CMD_SPECIAL_24BIT_SEARCHADDRL & 0xFF00) >> 8, 
					    DALI_CMD_SPECIAL_24BIT_SEARCHADDRL & 0xFF, 
					    0x00 };
	pbData[2] = data;
	return this->sendDaliMsg(24, pbData);
}

bool CAppDaliTransmitter::sendControlInputDevicesBroadcastSearchAddrM(int data) {
	BYTE pbData[3] = { (DALI_CMD_SPECIAL_24BIT_SEARCHADDRM & 0xFF00) >> 8, 
					    DALI_CMD_SPECIAL_24BIT_SEARCHADDRM & 0xFF, 
					    0x00 };
	pbData[2] = data;
	return this->sendDaliMsg(24, pbData);
}

bool CAppDaliTransmitter::sendControlInputDevicesBroadcastSearchAddrH(int data) {
	BYTE pbData[3] = { (DALI_CMD_SPECIAL_24BIT_SEARCHADDRH & 0xFF00) >> 8, 
					    DALI_CMD_SPECIAL_24BIT_SEARCHADDRH & 0xFF, 
					    0x00 };
	pbData[2] = data;
	return this->sendDaliMsg(24, pbData);
}

bool CAppDaliTransmitter::sendControlInputDevicesBroadcastCompare() {
	static const BYTE pbData[3] = { 0xC1, 0x03, 0x00 };
	return this->sendDaliMsg(24, pbData);
}

bool CAppDaliTransmitter::sendControlInputDevicesBroadcastWithdraw() {
	static const BYTE pbData[3] = { (DALI_CMD_SPECIAL_24BIT_WITHDRAW & 0xFF00) >> 8, 
									 DALI_CMD_SPECIAL_24BIT_WITHDRAW & 0xFF, 
									 0x00 };
	return this->sendDaliMsg(24, pbData);
}

bool CAppDaliTransmitter::sendControlInputDevicesProgramShortAddress(int shortAddress) {
	BYTE pbData[3] = { (DALI_CMD_SPECIAL_24BIT_PROGRAM_SHORTADDR & 0xFF00) >> 8, 
					    DALI_CMD_SPECIAL_24BIT_PROGRAM_SHORTADDR & 0xFF, 
					    0x00 };
	pbData[2] = shortAddress;
	return this->sendDaliMsg(24, pbData);
}

/*
bool CAppDaliTransmitter::sendControlInputDevicesProgramShortAddress(int shortAddress) {
	BYTE pbData[3] = { (DALI_CMD_SPECIAL_24BIT_PROGRAM_SHORTADDR & 0xFF00) >> 8, 
					    DALI_CMD_SPECIAL_24BIT_PROGRAM_SHORTADDR & 0xFF, 
					    0x00 };
	pbData[2] =((shortAddress << 1) + 0x01);	
	return this->sendDaliMsg(24, pbData);
}
*/

bool CAppDaliTransmitter::sendQueryControlInputDevicesShortAddress() {
	static const BYTE pbData[3] = { (DALI_CMD_SPECIAL_24BIT_QUERY_SHORTADDR & 0xFF00) >> 8, 
									 DALI_CMD_SPECIAL_24BIT_QUERY_SHORTADDR & 0xFF, 
									 0x00 };
	return this->sendDaliMsg(24, pbData);
}

// Control gear Device: Voor algemene testen/bediening.

bool CAppDaliTransmitter::sendControlGearDeviceActualLevel(int shortAddress,int value) {
	const int prioLevel = 1;
	BYTE pbData[2] = { 0,0 };
	pbData[0] = (shortAddress) << 1;			// Selector bit blijft op 0
	pbData[1] = value;
	return this->sendDaliMsg(16,pbData,prioLevel);
}

bool CAppDaliTransmitter::sendControlGearDeviceOff(int shortAddress) { 
	return this->sendControlGearDeviceCommand(shortAddress,DALI_CMD_SET_DIM_OFF);
}
bool CAppDaliTransmitter::sendControlGearRecallMaxLevel(int shortAddress) {
	return this->sendControlGearDeviceCommand(shortAddress, DALI_CMD_SET_DIM_RECALL_MAX_LEVEL);
}
bool CAppDaliTransmitter::sendControlGearRecallMinLevel(int shortAddress) {
	return this->sendControlGearDeviceCommand(shortAddress, DALI_CMD_SET_DIM_RECALL_MIN_LEVEL);
}
bool CAppDaliTransmitter::sendControlGearDimUp(int shortAddress) {
	return this->sendControlGearDeviceCommand(shortAddress, DALI_CMD_SET_DIM_UP);
}
bool CAppDaliTransmitter::sendControlGearDimDown(int shortAddress) {
	return this->sendControlGearDeviceCommand(shortAddress, DALI_CMD_SET_DIM_DOWN);
}
bool CAppDaliTransmitter::sendControlGearDimStepUp(int shortAddress) {
	return this->sendControlGearDeviceCommand(shortAddress, DALI_CMD_SET_DIM_STEP_UP);
}
bool CAppDaliTransmitter::sendControlGearDimStepDown(int shortAddress) {
	return this->sendControlGearDeviceCommand(shortAddress, DALI_CMD_SET_DIM_STEP_DOWN);
}
bool CAppDaliTransmitter::sendControlGearGoToScene(int shortAddress,int sceneNumber) {
	ASSERT(sceneNumber >= DALI_SYSTEM_CONTROLGEAR_SCENE_FIRST && sceneNumber <= DALI_SYSTEM_CONTROLGEAR_SCENE_LAST);
	return this->sendControlGearDeviceCommand(shortAddress, DALI_CMD_SET_SCENE_GOTO + sceneNumber);
}

// Control gear group config.
bool CAppDaliTransmitter::sendControlGearGroupMaxLevel(int groupAddress) {
	REQUIRE(groupAddress >= DALI_SYSTEM_CONTROLGEAR_GROUP_FIRST && groupAddress <= DALI_SYSTEM_CONTROLGEAR_GROUP_LAST);
	return this->sendControlGearGroupCommand(groupAddress,DALI_CMD_SET_DTR0_MAX_LEVEL);
}
bool CAppDaliTransmitter::sendControlGearGroupMinLevel(int groupAddress) {
	REQUIRE(groupAddress >= DALI_SYSTEM_CONTROLGEAR_GROUP_FIRST && groupAddress <= DALI_SYSTEM_CONTROLGEAR_GROUP_LAST);
	return this->sendControlGearGroupCommand(groupAddress,DALI_CMD_SET_DTR0_MIN_LEVEL);
}
bool CAppDaliTransmitter::sendControlGearGroupSystemFailureLevel(int groupAddress) {
	REQUIRE(groupAddress >= DALI_SYSTEM_CONTROLGEAR_GROUP_FIRST && groupAddress <= DALI_SYSTEM_CONTROLGEAR_GROUP_LAST);
	return this->sendControlGearGroupCommand(groupAddress,DALI_CMD_SET_DTR0_SYSTEM_FAILURE_LEVEL);
}
bool CAppDaliTransmitter::sendControlGearGroupPowerOnLevel(int groupAddress) {
	REQUIRE(groupAddress >= DALI_SYSTEM_CONTROLGEAR_GROUP_FIRST && groupAddress <= DALI_SYSTEM_CONTROLGEAR_GROUP_LAST);
	return this->sendControlGearGroupCommand(groupAddress,DALI_CMD_SET_DTR0_POWERONLEVEL);
}
bool CAppDaliTransmitter::sendControlGearGroupFadeTime(int groupAddress) {
	REQUIRE(groupAddress >= DALI_SYSTEM_CONTROLGEAR_GROUP_FIRST && groupAddress <= DALI_SYSTEM_CONTROLGEAR_GROUP_LAST);
	return this->sendControlGearGroupCommand(groupAddress,DALI_CMD_SET_DTR0_FADETIME);
}
bool CAppDaliTransmitter::sendControlGearGroupFadeRate(int groupAddress) {
	REQUIRE(groupAddress >= DALI_SYSTEM_CONTROLGEAR_GROUP_FIRST && groupAddress <= DALI_SYSTEM_CONTROLGEAR_GROUP_LAST);
	return this->sendControlGearGroupCommand(groupAddress,DALI_CMD_SET_DTR0_FADERATE);
}

// Control gear bediening: Group: Voor algemene testen/bediening.
bool CAppDaliTransmitter::sendControlGearGroupActualLevel(int groupAddress,int value) {
	const int prioLevel = 1;
	REQUIRE(groupAddress >= DALI_SYSTEM_CONTROLGEAR_GROUP_FIRST && groupAddress <= DALI_SYSTEM_CONTROLGEAR_GROUP_LAST);
	BYTE pbData[2] = { 0,0 };
	pbData[0] = (groupAddress & 0x0F) << 1;			
	pbData[0] |= DALI_BIT_GROUPADDRESS;				// Selector bit blijft op 0
	pbData[1] = value;
	return this->sendDaliMsg(16,pbData,prioLevel);
}

bool CAppDaliTransmitter::sendControlGearGroupOff(int groupAddress) {
	REQUIRE(groupAddress >= DALI_SYSTEM_CONTROLGEAR_GROUP_FIRST && groupAddress <= DALI_SYSTEM_CONTROLGEAR_GROUP_LAST);
	return this->sendControlGearGroupCommand(groupAddress,DALI_CMD_SET_DIM_OFF);
}

bool CAppDaliTransmitter::sendControlGearGroupRecallMaxLevel(int groupAddress) {
	REQUIRE(groupAddress >= DALI_SYSTEM_CONTROLGEAR_GROUP_FIRST && groupAddress <= DALI_SYSTEM_CONTROLGEAR_GROUP_LAST);
	return this->sendControlGearGroupCommand(groupAddress,DALI_CMD_SET_DIM_RECALL_MAX_LEVEL);
}

bool CAppDaliTransmitter::sendControlGearGroupRecallMinLevel(int groupAddress) {
	REQUIRE(groupAddress >= DALI_SYSTEM_CONTROLGEAR_GROUP_FIRST && groupAddress <= DALI_SYSTEM_CONTROLGEAR_GROUP_LAST);
	return this->sendControlGearGroupCommand(groupAddress,DALI_CMD_SET_DIM_RECALL_MIN_LEVEL);
}

bool CAppDaliTransmitter::sendControlGearGroupStepUp(int groupAddress) {
	REQUIRE(groupAddress >= DALI_SYSTEM_CONTROLGEAR_GROUP_FIRST && groupAddress <= DALI_SYSTEM_CONTROLGEAR_GROUP_LAST);
	return this->sendControlGearGroupCommand(groupAddress,DALI_CMD_SET_DIM_STEP_UP);
}

bool CAppDaliTransmitter::sendControlGearGroupStepDown(int groupAddress) {
	REQUIRE(groupAddress >= DALI_SYSTEM_CONTROLGEAR_GROUP_FIRST && groupAddress <= DALI_SYSTEM_CONTROLGEAR_GROUP_LAST);
	return this->sendControlGearGroupCommand(groupAddress,DALI_CMD_SET_DIM_STEP_DOWN);
}

bool CAppDaliTransmitter::sendControlGearGroupDimUp(int groupAddress) {
	REQUIRE(groupAddress >= DALI_SYSTEM_CONTROLGEAR_GROUP_FIRST && groupAddress <= DALI_SYSTEM_CONTROLGEAR_GROUP_LAST);
	return this->sendControlGearGroupCommand(groupAddress,DALI_CMD_SET_DIM_UP);
}

bool CAppDaliTransmitter::sendControlGearGroupDimDown(int groupAddress) {
	REQUIRE(groupAddress >= DALI_SYSTEM_CONTROLGEAR_GROUP_FIRST && groupAddress <= DALI_SYSTEM_CONTROLGEAR_GROUP_LAST);
	return this->sendControlGearGroupCommand(groupAddress,DALI_CMD_SET_DIM_DOWN);
}

// Controlgear 
bool CAppDaliTransmitter::sendControlGearDTR0(int value) {
	BYTE pbData[2] = { 0, 0 };
	pbData[0] = DALI_CMD_SPECIAL_DTR0;
	pbData[1] = value;
	return this->sendDaliMsg(16,pbData);
}

bool CAppDaliTransmitter::sendControlGearDTR1(int value) {
	BYTE pbData[2] = { 0, 0 };
	pbData[0] = DALI_CMD_SPECIAL_DTR1;
	pbData[1] = value;
	return this->sendDaliMsg(16,pbData);
}

bool CAppDaliTransmitter::sendControlGearDTR2(int value) {
	BYTE pbData[2] = { 0, 0 };
	pbData[0] = DALI_CMD_SPECIAL_DTR2;
	pbData[1] = value;
	return this->sendDaliMsg(16,pbData);
}

bool CAppDaliTransmitter::sendControlGearEnableDeviceType(int value) {
	BYTE pbData[2] = { 0, 0 };
	pbData[0] = DALI_CMD_SPECIAL_ENABLE_DEVICE_TYPE;
	pbData[1] = value;
	return this->sendDaliMsg(16, pbData);
}

// DT6 Speciale extended commandos.
bool CAppDaliTransmitter::sendQueryControlGearDT6DimmerCurve(int shortAddress) {
	return this->sendControlGearDeviceCommand(shortAddress, DALI_CMD_DT6_QUERY_DIMMING_CURVE);
}
bool CAppDaliTransmitter::sendControlGearDT6SelectDimmerCurve(int shortAddress) {
	return this->sendControlGearDeviceCommand(shortAddress, DALI_CMD_DT6_SELECT_DIMMING_CURVE);
}

// DT8 Speciale extended commandos.
bool CAppDaliTransmitter::sendQueryControlGearDT8ColorTypeFeatures(int shortAddress) {
	return this->sendControlGearDeviceCommand(shortAddress, DALI_CMD_DT8_QUERY_COLOUR_TYPE_FEATURES);
}
bool CAppDaliTransmitter::sendQueryControlGearDT8ColourValue(int shortAddress) {
	return this->sendControlGearDeviceCommand(shortAddress, DALI_CMD_DT8_QUERY_COLOUR_VALUE);
}

// DT8 Speciale extended commandos.
bool CAppDaliTransmitter::sendControlGearDT8SetTemporaryColorTemp(int shortAddress) {				// DTR0=lsb, DTR1=MSB
	return this->sendControlGearDeviceCommand(shortAddress, DALI_CMD_DT8_SET_TEMP_COLOUR_TEMP_TC);
}
bool CAppDaliTransmitter::sendControlGearDT8SetTemporaryColorCoordinateX(int shortAddress) {		// DTR0=lsb, DTR1=MSB
	return this->sendControlGearDeviceCommand(shortAddress, DALI_CMD_DT8_SET_TEMP_X_COORDINATE);
}
bool CAppDaliTransmitter::sendControlGearDT8SetTemporaryColorCoordinateY(int shortAddress) {		// DTR0=lsb, DTR1=MSB
	return this->sendControlGearDeviceCommand(shortAddress, DALI_CMD_DT8_SET_TEMP_Y_COORDINATE);
}
bool CAppDaliTransmitter::sendControlGearDT8SetTemporaryColorDimLevelRGB(int shortAddress) {		// DTR0=red,DTR1=green,DTR2=blue
	return this->sendControlGearDeviceCommand(shortAddress, DALI_CMD_DT8_SET_TEMP_RGB_DIMLEVEL);
}
bool CAppDaliTransmitter::sendControlGearDT8SetTemporaryColorDimLevelWAF(int shortAddress) {		// DTR0=white,DTR1=amber,DTR2=freecolor
	return this->sendControlGearDeviceCommand(shortAddress, DALI_CMD_DT8_SET_TEMP_WAF_DIMLEVEL);
}
bool CAppDaliTransmitter::sendControlGearDT8SetTemporaryColorPrimaryChannelN(int shortAddress) {	// DTR0=lsb,DTR1=msb,DTR2=channelNR
	return this->sendControlGearDeviceCommand(shortAddress, DALI_CMD_DT8_SET_TEMP_PRIMARY_N_DIMLEVEL);
}

// Control gear: Queries.	
bool CAppDaliTransmitter::sendQueryControlGearDTR0(int shortAddress) {
	return this->sendControlGearDeviceCommand(shortAddress,DALI_CMD_QUERY_CONTENT_DTR0);
}

bool CAppDaliTransmitter::sendQueryControlGearMemory(int shortAddress) {	
	return this->sendControlGearDeviceCommand(shortAddress,DALI_CMD_QUERY_READ_MEMORY_LOCATION);
}

bool CAppDaliTransmitter::sendQueryControlGearDeviceStatus(int shortAddress) { 
	return this->sendControlGearDeviceCommand(shortAddress,DALI_CMD_GET_DEVICE_STATUS);	
}

bool CAppDaliTransmitter::sendQueryControlGearBroadcastPresent( ) { 
	static const BYTE pbData[2] = { 0xff, DALI_CMD_QUERY_CONTROL_GEAR_PRESENT};
	return this->sendDaliMsg(16,pbData);
}

bool CAppDaliTransmitter::sendQueryControlGearPresent(int shortAddress) { 
	return this->sendControlGearDeviceCommand(shortAddress,DALI_CMD_QUERY_CONTROL_GEAR_PRESENT);	
}

bool CAppDaliTransmitter::sendQueryControlGearGroups_0_7(int shortAddress) {
	return this->sendControlGearDeviceCommand(shortAddress,DALI_CMD_QUERY_GROUPS_0_7);
}
bool CAppDaliTransmitter::sendQueryControlGearGroups_8_15(int shortAddress) {
	return this->sendControlGearDeviceCommand(shortAddress,DALI_CMD_QUERY_GROUPS_8_15);
}

bool CAppDaliTransmitter::sendQueryControlGearDeviceType(int shortAddress) {
	return this->sendControlGearDeviceCommand(shortAddress,DALI_CMD_QUERY_DEVICE_TYPE);
}

bool CAppDaliTransmitter::sendQueryControlGearNextDeviceType(int shortAddress) {
	return this->sendControlGearDeviceCommand(shortAddress,DALI_CMD_QUERY_NEXT_DEVICE_TYPE);
}

bool CAppDaliTransmitter::sendQueryControlGearVersionNumber(int shortAddress) {
	return this->sendControlGearDeviceCommand(shortAddress,DALI_CMD_QUERY_VERSION_NUMBER);
}

bool CAppDaliTransmitter::sendQueryControlGearOperatingMode(int shortAddress) {
	return this->sendControlGearDeviceCommand(shortAddress,DALI_CMD_QUERY_OPERATING_MODE);
}

bool CAppDaliTransmitter::sendQueryControlGearActualLevel(int shortAddress) {
	return this->sendControlGearDeviceCommand(shortAddress,DALI_CMD_QUERY_ACTUAL_LEVEL);
}

bool CAppDaliTransmitter::sendQueryControlGearMinLevel(int shortAddress) {
	return this->sendControlGearDeviceCommand(shortAddress,DALI_CMD_QUERY_MIN_LEVEL);
}

bool CAppDaliTransmitter::sendQueryControlGearMaxLevel(int shortAddress) {
	return this->sendControlGearDeviceCommand(shortAddress,DALI_CMD_QUERY_MAX_LEVEL);
}

bool CAppDaliTransmitter::sendQueryControlGearFadeTimeRate(int shortAddress) {
	return this->sendControlGearDeviceCommand(shortAddress,DALI_CMD_QUERY_FADE_TIME_FADE_RATE);
}

bool CAppDaliTransmitter::sendQueryControlGearPhysicalMinValue(int shortAddress) {
	return this->sendControlGearDeviceCommand(shortAddress,DALI_CMD_QUERY_PHYSICAL_MINIMUM);
}

bool CAppDaliTransmitter::sendQueryControlGearPowerOnLevel(int shortAddress) {
	return this->sendControlGearDeviceCommand(shortAddress,DALI_CMD_QUERY_POWER_ON_LEVEL);
}

bool CAppDaliTransmitter::sendQueryControlGearSystemFailureLevel(int shortAddress) {
	return this->sendControlGearDeviceCommand(shortAddress,DALI_CMD_QUERY_SYSTEM_FAILURE_LEVEL);
}

bool CAppDaliTransmitter::sendQueryControlGearExtendedFadeTime(int shortAddress) {
	return this->sendControlGearDeviceCommand(shortAddress,DALI_CMD_QUERY_EXTENDED_FADE_TIME);
}
bool CAppDaliTransmitter::sendQueryControlGearShortAddress() {
	static const BYTE pbData[2] = { DALI_CMD_SPECIAL_QUERY_SHORTADDR, 0x00};
	return this->sendDaliMsg(16,pbData);
/*
	return this->sendControlGearDeviceCommand(0x00, DALI_CMD_SPECIAL_QUERY_SHORTADDR);
*/
}


// input control dvices...
bool CAppDaliTransmitter::sendQueryBroadcastInputControlDeviceStatus( ) {
	return this->sendControlInputBroadcastDeviceCommand(DALI_CMD_24BIT_QUERY_DEVICE_STATUS);
}

bool CAppDaliTransmitter::sendControlInputDeviceIdentify(int shortAddress) {
	return this->sendControlInputDeviceCommand(shortAddress,DALI_CMD_24BIT_IDENTIFY_DEVICE);
}

bool CAppDaliTransmitter::sendControlInputDeviceApplicationControllerEnable(int shortAddress) {
	return this->sendControlInputDeviceCommand(shortAddress,DALI_CMD_24BIT_ENABLE_APPLICATION_CONTROLLER);
}
bool CAppDaliTransmitter::sendControlInputDeviceApplicationControllerDisable(int shortAddress) {
	return this->sendControlInputDeviceCommand(shortAddress,DALI_CMD_24BIT_DISABLE_APPLICATION_CONTROLLER);
}
bool CAppDaliTransmitter::sendControlInputDevicesSetShortAddressDTR0(int shortAddress) {
	return this->sendControlInputDeviceCommand(shortAddress, DALI_CMD_24BIT_SET_SHORT_ADDRESS);
}
bool CAppDaliTransmitter::sendControlInputDevicesSetOperatingModeDTR0(int shortAddress) {
	return this->sendControlInputDeviceCommand(shortAddress, DALI_CMD_24BIT_SET_OPERATING_MODE);
}

bool CAppDaliTransmitter::sendQueryInputControlDeviceStatus(int shortAddress) {
	return this->sendControlInputDeviceCommand(shortAddress,DALI_CMD_24BIT_QUERY_DEVICE_STATUS);
}

bool CAppDaliTransmitter::sendControlInputDevicesBroadcastWriteDTR1_DTR0(int dtr0,int dtr1) {
	BYTE pbData[3] = { 0,0,0 };	
	pbData[0] = DALI_CMD_SPECIAL_24BIT_WRITE_DTR1_DTR0;
	pbData[1] = dtr0;
	pbData[2] = dtr1;
	return this->sendDaliMsg(24,pbData);
}

bool CAppDaliTransmitter::sendControlInputDevicesBroadcastWriteDTR2_DTR1(int dtr1, int dtr2) {
	BYTE pbData[3] = { 0,0,0 };
	pbData[0] = DALI_CMD_SPECIAL_24BIT_WRITE_DTR2_DTR1;
	pbData[1] = dtr1;
	pbData[2] = dtr2;
	return this->sendDaliMsg(24, pbData);
}

bool CAppDaliTransmitter::sendControlInputDevicesBroadcastDTR0(int dtr0) {
	BYTE pbData[3] = { 0,0,0 };
	pbData[0] = (DALI_CMD_SPECIAL_24BIT_DTR0 & 0xFF00) >> 8;
	pbData[1] = (DALI_CMD_SPECIAL_24BIT_DTR0 & 0xFF);
	pbData[2] = dtr0;
	return this->sendDaliMsg(24, pbData);
}

bool CAppDaliTransmitter::sendControlInputDevicesBroadcastDTR1(int dtr1) {
	BYTE pbData[3] = { 0,0,0 };
	pbData[0] = (DALI_CMD_SPECIAL_24BIT_DTR1 & 0xFF00) >> 8;
	pbData[1] = (DALI_CMD_SPECIAL_24BIT_DTR1 & 0xFF);
	pbData[2] = dtr1;
	return this->sendDaliMsg(24, pbData);
}

bool CAppDaliTransmitter::sendControlInputDevicesBroadcastDTR2(int dtr2) {
	BYTE pbData[3] = { 0,0,0 };
	pbData[0] = (DALI_CMD_SPECIAL_24BIT_DTR2 & 0xFF00) >> 8;
	pbData[1] = (DALI_CMD_SPECIAL_24BIT_DTR2 & 0xFF);
	pbData[2] = dtr2;
	return this->sendDaliMsg(24, pbData);
}

bool CAppDaliTransmitter::sendControlInputDevicesBroadcastSetShortAddressDTR0( ) {
	const BYTE pbData[3] = { 0xff,0xfe,DALI_CMD_24BIT_SET_SHORT_ADDRESS };	
	return this->sendDaliMsg(24, pbData);
}

bool CAppDaliTransmitter::sendControlInputDevicesBroadcastRemoveFromDeviceGroups0_15() {
	const BYTE pbData[3] = { 0xff,0xfe,DALI_CMD_24BIT_REMOVE_FROM_DEVICEGROUPS_0_15 };
	return this->sendDaliMsg(24, pbData);
}

bool CAppDaliTransmitter::sendControlInputDevicesBroadcastRemoveFromDeviceGroups16_31() {
	const BYTE pbData[3] = { 0xff,0xfe,DALI_CMD_24BIT_REMOVE_FROM_DEVICEGROUPS_16_31 };
	return this->sendDaliMsg(24, pbData);
}


// Control input devices: Queries
bool CAppDaliTransmitter::sendQueryControlInputDeviceReadMemoryLocation(int shortAddress) {
	return this->sendControlInputDeviceCommand(shortAddress,DALI_CMD_24BIT_QUERY_READ_MEMORY_LOCATION);
}
bool CAppDaliTransmitter::sendQueryControlInputDeviceReadDTR0(int shortAddress) {
	return this->sendControlInputDeviceCommand(shortAddress,DALI_CMD_24BIT_QUERY_CONTENT_DTR0);
}
bool CAppDaliTransmitter::sendQueryControlInputDeviceVersionNumber(int shortAddress) {
	return this->sendControlInputDeviceCommand(shortAddress,DALI_CMD_24BIT_QUERY_VERSION_NUMBER);
}
bool CAppDaliTransmitter::sendQueryControlInputDeviceNumberOfInstances(int shortAddress){
	return this->sendControlInputDeviceCommand(shortAddress,DALI_CMD_24BIT_QUERY_NUMBER_OF_INSTANCES);
}
bool CAppDaliTransmitter::sendQueryControlInputDeviceReadDTR1(int shortAddress){
	return this->sendControlInputDeviceCommand(shortAddress,DALI_CMD_24BIT_QUERY_CONTENT_DTR1);
}
bool CAppDaliTransmitter::sendQueryControlInputDeviceReadDTR2(int shortAddress){
	return this->sendControlInputDeviceCommand(shortAddress,DALI_CMD_24BIT_QUERY_CONTENT_DTR2);
}
bool CAppDaliTransmitter::sendQueryControlInputDeviceAppControllerEnabled(int shortAddress){
	return this->sendControlInputDeviceCommand(shortAddress,DALI_CMD_24BIT_QUERY_APPLICATION_CONTROLLER_ENABLED);
}
bool CAppDaliTransmitter::sendQueryControlInputDeviceOperatingMode(int shortAddress){
	return this->sendControlInputDeviceCommand(shortAddress,DALI_CMD_24BIT_QUERY_OPERATING_MODE);
}
bool CAppDaliTransmitter::sendQueryControlInputDeviceGroups_0_7(int shortAddress){
	return this->sendControlInputDeviceCommand(shortAddress,DALI_CMD_24BIT_QUERY_DEVICEGROUPS_0_7);
}
bool CAppDaliTransmitter::sendQueryControlInputDeviceGroups_8_15(int shortAddress){
	return this->sendControlInputDeviceCommand(shortAddress,DALI_CMD_24BIT_QUERY_DEVICEGROUPS_8_15);
}
bool CAppDaliTransmitter::sendQueryControlInputDeviceGroups_16_23(int shortAddress){
	return this->sendControlInputDeviceCommand(shortAddress,DALI_CMD_24BIT_QUERY_DEVICEGROUPS_16_23);
}
bool CAppDaliTransmitter::sendQueryControlInputDeviceGroups_24_31(int shortAddress){
	return this->sendControlInputDeviceCommand(shortAddress,DALI_CMD_24BIT_QUERY_DEVICEGROUPS_24_31);
}
bool CAppDaliTransmitter::sendQueryControlInputDevicePowerCycleNotification(int shortAddress){
	return this->sendControlInputDeviceCommand(shortAddress,DALI_CMD_24BIT_QUERY_POWER_CYCLE_NOTIFICATION);
}
bool CAppDaliTransmitter::sendQueryControlInputDeviceCapabilities(int shortAddress) {
	return this->sendControlInputDeviceCommand(shortAddress,DALI_CMD_24BIT_QUERY_DEVICE_CAPABILITIES);
}
bool CAppDaliTransmitter::sendQueryControlInputDeviceExtVersionNumber(int shortAddress){
	return this->sendControlInputDeviceCommand(shortAddress,DALI_CMD_24BIT_QUERY_EXTENDED_VERSION_NUMBER);
}
bool CAppDaliTransmitter::sendQueryControlInputDeviceRandomAddressH(int shortAddress) {
	return this->sendControlInputDeviceCommand(shortAddress,DALI_CMD_24BIT_QUERY_RANDOM_ADDRESS_H);
}
bool CAppDaliTransmitter::sendQueryControlInputDeviceRandomAddressM(int shortAddress) {
	return this->sendControlInputDeviceCommand(shortAddress,DALI_CMD_24BIT_QUERY_RANDOM_ADDRESS_M);
}
bool CAppDaliTransmitter::sendQueryControlInputDeviceRandomAddressL(int shortAddress) {
	return this->sendControlInputDeviceCommand(shortAddress,DALI_CMD_24BIT_QUERY_RANDOM_ADDRESS_L);
}

// Control device instance Queries.
bool CAppDaliTransmitter::sendQueryControlInputDeviceInstanceType(int shortAddress,int instanceAddress) {
	return this->sendControlInputDeviceInstanceCommand(shortAddress, instanceAddress, DALI_CMD_24BIT_QUERY_INSTANCE_TYPE);
}
bool CAppDaliTransmitter::sendQueryControlInputDeviceInstanceResolution(int shortAddress,int instanceAddress){
	return this->sendControlInputDeviceInstanceCommand(shortAddress, instanceAddress, DALI_CMD_24BIT_QUERY_RESOLUTION);
}
bool CAppDaliTransmitter::sendQueryControlInputDeviceInstanceError(int shortAddress,int instanceAddress){
	return this->sendControlInputDeviceInstanceCommand(shortAddress, instanceAddress, DALI_CMD_24BIT_QUERY_INSTANCE_ERROR);
}
bool CAppDaliTransmitter::sendQueryControlInputDeviceInstanceStatus(int shortAddress,int instanceAddress){
	return this->sendControlInputDeviceInstanceCommand(shortAddress, instanceAddress, DALI_CMD_24BIT_QUERY_INSTANCE_STATUS);
}
bool CAppDaliTransmitter::sendQueryControlInputDeviceInstanceEventPriority(int shortAddress,int instanceAddress){
	return this->sendControlInputDeviceInstanceCommand(shortAddress, instanceAddress, DALI_CMD_24BIT_QUERY_EVENT_PRIORITY);
}
bool CAppDaliTransmitter::sendQueryControlInputDeviceInstanceEnabled(int shortAddress,int instanceAddress){
	return this->sendControlInputDeviceInstanceCommand(shortAddress, instanceAddress, DALI_CMD_24BIT_QUERY_INSTANCE_ENABLED);
}
bool CAppDaliTransmitter::sendQueryControlInputDeviceInstancePrimaryGroup(int shortAddress,int instanceAddress){
	return this->sendControlInputDeviceInstanceCommand(shortAddress, instanceAddress, DALI_CMD_24BIT_QUERY_PRIMARY_INSTANCE_GROUP);
}
bool CAppDaliTransmitter::sendQueryControlInputDeviceInstanceGroup1(int shortAddress,int instanceAddress){
	return this->sendControlInputDeviceInstanceCommand(shortAddress, instanceAddress, DALI_CMD_24BIT_QUERY_INSTANCE_GROUP_1);
}
bool CAppDaliTransmitter::sendQueryControlInputDeviceInstanceGroup2(int shortAddress,int instanceAddress){
	return this->sendControlInputDeviceInstanceCommand(shortAddress, instanceAddress, DALI_CMD_24BIT_QUERY_INSTANCE_GROUP_2);
}
bool CAppDaliTransmitter::sendQueryControlInputDeviceInstanceEventScheme(int shortAddress,int instanceAddress){
	return this->sendControlInputDeviceInstanceCommand(shortAddress, instanceAddress, DALI_CMD_24BIT_QUERY_EVENT_SCHEME);
}
bool CAppDaliTransmitter::sendQueryControlInputDeviceInstanceInputValue(int shortAddress,int instanceAddress){
	return this->sendControlInputDeviceInstanceCommand(shortAddress, instanceAddress, DALI_CMD_24BIT_QUERY_INPUT_VALUE);
}
bool CAppDaliTransmitter::sendQueryControlInputDeviceInstanceInputValueLatch(int shortAddress,int instanceAddress){
	return this->sendControlInputDeviceInstanceCommand(shortAddress, instanceAddress, DALI_CMD_24BIT_QUERY_INPUT_VALUE_LATCH);
}
bool CAppDaliTransmitter::sendQueryControlInputDeviceInstanceFeatureType(int shortAddress,int instanceAddress){
	return this->sendControlInputDeviceInstanceCommand(shortAddress, instanceAddress, DALI_CMD_24BIT_QUERY_FEATURE_TYPE);
}
bool CAppDaliTransmitter::sendQueryControlInputDeviceInstanceNextFeatureType(int shortAddress,int instanceAddress){
	return this->sendControlInputDeviceInstanceCommand(shortAddress, instanceAddress, DALI_CMD_24BIT_QUERY_NEXT_FEATURE_TYPE);
}
bool CAppDaliTransmitter::sendQueryControlInputDeviceInstanceEventFilter0_7(int shortAddress,int instanceAddress){
	return this->sendControlInputDeviceInstanceCommand(shortAddress, instanceAddress, DALI_CMD_24BIT_QUERY_EVENT_FILTER_0_7);
}
bool CAppDaliTransmitter::sendQueryControlInputDeviceInstanceEventFilter8_15(int shortAddress,int instanceAddress){
	return this->sendControlInputDeviceInstanceCommand(shortAddress, instanceAddress, DALI_CMD_24BIT_QUERY_EVENT_FILTER_8_15);
}
bool CAppDaliTransmitter::sendQueryControlInputDeviceInstanceEventFilter16_23(int shortAddress,int instanceAddress){
	return this->sendControlInputDeviceInstanceCommand(shortAddress, instanceAddress, DALI_CMD_24BIT_QUERY_EVENT_FILTER_16_23);
}
bool CAppDaliTransmitter::sendQueryControlInputDeviceInstanceConfiguration(int shortAddress,int instanceAddress){
	return this->sendControlInputDeviceInstanceCommand(shortAddress, instanceAddress, DALI_CMD_24BIT_QUERY_INSTANCE_CONFIGURATION);
}
bool CAppDaliTransmitter::sendQueryControlInputDeviceInstanceAvailableTypes(int shortAddress,int instanceAddress){
	return this->sendControlInputDeviceInstanceCommand(shortAddress, instanceAddress, DALI_CMD_24BIT_QUERY_AVAILABLE_INSTANCE_TYPES);
}

// Control device instance commandos.
bool CAppDaliTransmitter::sendControlInputDeviceInstanceEventPrioDTR0(int shortAddress,int instanceAddress) {
	return this->sendControlInputDeviceInstanceCommand(shortAddress, instanceAddress, DALI_CMD_24BIT_SET_EVENT_PRIORITY);
}
bool CAppDaliTransmitter::sendControlInputDeviceInstanceEnableInstance(int shortAddress,int instanceAddress) {
	return this->sendControlInputDeviceInstanceCommand(shortAddress, instanceAddress, DALI_CMD_24BIT_ENABLE_INSTANCE);
}
bool CAppDaliTransmitter::sendControlInputDeviceInstanceDisableInstance(int shortAddress,int instanceAddress){
	return this->sendControlInputDeviceInstanceCommand(shortAddress, instanceAddress, DALI_CMD_24BIT_DISABLE_INSTANCE);
}
bool CAppDaliTransmitter::sendControlInputDeviceInstancePrimaryGroupDTR0(int shortAddress,int instanceAddress){
	return this->sendControlInputDeviceInstanceCommand(shortAddress, instanceAddress, DALI_CMD_24BIT_SET_PRIMARY_INSTANCE_GROUP);
}
bool CAppDaliTransmitter::sendControlInputDeviceInstanceGroup1DTR0(int shortAddress,int instanceAddress){
	return this->sendControlInputDeviceInstanceCommand(shortAddress, instanceAddress, DALI_CMD_24BIT_SET_INSTANCE_GROUP1);
}
bool CAppDaliTransmitter::sendControlInputDeviceInstanceGroup2DTR0(int shortAddress,int instanceAddress){
	return this->sendControlInputDeviceInstanceCommand(shortAddress, instanceAddress, DALI_CMD_24BIT_SET_INSTANCE_GROUP2);
}
bool CAppDaliTransmitter::sendControlInputDeviceInstanceEventSchemeDTR0(int shortAddress,int instanceAddress){
	return this->sendControlInputDeviceInstanceCommand(shortAddress, instanceAddress, DALI_CMD_24BIT_SET_EVENT_SCHEME);
}
bool CAppDaliTransmitter::sendControlInputDeviceInstanceEventFilterDTR0DTR1DTR2(int shortAddress,int instanceAddress){
	return this->sendControlInputDeviceInstanceCommand(shortAddress, instanceAddress, DALI_CMD_24BIT_SET_EVENT_FILTER);
}
bool CAppDaliTransmitter::sendControlInputDeviceInstanceType(int shortAddress,int instanceAddress){
	return this->sendControlInputDeviceInstanceCommand(shortAddress, instanceAddress, DALI_CMD_24BIT_SET_INSTANCE_TYPE);
}

/***********************************************************************************/
// Occupancy device/instance commands.
bool CAppDaliTransmitter::sendQueryControlInputDeviceInstanceOccupancyReportTimerInterval(int shortAddress,int instanceAddress) {
	return this->sendControlInputDeviceInstanceCommand(shortAddress, instanceAddress, DALI_CMD_24BIT_OCCUPANCY_QUERY_REPORTTIME_TIME);
}
bool CAppDaliTransmitter::sendQueryControlInputDeviceInstanceOccupancyHoldTimerInterval(int shortAddress,int instanceAddress) {
	return this->sendControlInputDeviceInstanceCommand(shortAddress, instanceAddress, DALI_CMD_24BIT_OCCUPANCY_QUERY_HOLDTIME_TIME);
}
bool CAppDaliTransmitter::sendQueryControlInputDeviceInstanceOccupancyDeadTimerInterval(int shortAddress,int instanceAddress) {
	return this->sendControlInputDeviceInstanceCommand(shortAddress, instanceAddress, DALI_CMD_24BIT_OCCUPANCY_QUERY_DEADTIME_TIME);
}

bool CAppDaliTransmitter::sendControlInputDeviceInstanceOccupancyReportTimerInterval(int shortAddress,int instanceAddress) {			// DTR0, send twice
	return this->sendControlInputDeviceInstanceCommand(shortAddress, instanceAddress, DALI_CMD_24BIT_OCCUPANCY_SET_REPORTTIME_TIME_DTR0);
}
bool CAppDaliTransmitter::sendControlInputDeviceInstanceOccupancyHoldTimerInterval(int shortAddress,int instanceAddress) {				// DTR0, send twice
	return this->sendControlInputDeviceInstanceCommand(shortAddress, instanceAddress, DALI_CMD_24BIT_OCCUPANCY_SET_HOLDTIME_TIME_DTR0);
}
bool CAppDaliTransmitter::sendControlInputDeviceInstanceOccupancyDeadTimerInterval(int shortAddress,int instanceAddress){				// DTR0, send twice
	return this->sendControlInputDeviceInstanceCommand(shortAddress, instanceAddress, DALI_CMD_24BIT_OCCUPANCY_SET_DEADTIME_TIME_DTR0);
}

/***********************************************************************************/
// algemene functie...

// 16 Bit forward frame.
// YAAA AAAS		(S=Selector bit)	(Y=0: Device Address)
bool CAppDaliTransmitter::sendControlGearDeviceCommand(int shortAddress,int command,int prioLevel) {
	BYTE pbData[2] = { 0,0 };
	pbData[0] = (shortAddress) << 1;
	pbData[0] |= DALI_BIT_SELECTOR;
	pbData[1] = command;
	return this->sendDaliMsg(16,pbData,prioLevel);
}

// 16 Bit forward frame.
// Y00A AAAS		(S=Selector bit)	(Y=1: Group Address)
bool CAppDaliTransmitter::sendControlGearGroupCommand(int groupAddress,int command,int prioLevel) {
	BYTE pbData[2] = { 0,0 };
	pbData[0] = (groupAddress & 0x0F) << 1;
	pbData[0] |= DALI_BIT_SELECTOR;
	pbData[0] |= DALI_BIT_GROUPADDRESS;
	pbData[1] = command;
	return this->sendDaliMsg(16,pbData,prioLevel);
}


// 24 Bit forward frame naar een device short address.
bool CAppDaliTransmitter::sendControlInputDeviceCommand(int shortAddress,int command,int prioLevel) {
	BYTE pbData[3] = { 0, 0, 0 };
	pbData[0] = (shortAddress << 1);
	pbData[0] |= (1<<0);				// DALI 24bit command frame: bit 16 is 1
	pbData[1] = 0xfe;					// Device  addressing: altijd 0xfe
	pbData[2] = command;				// opcode byte.
	return this->sendDaliMsg(24,pbData,prioLevel);
}

// 24 Bit forward frame naar een device/instance address.
bool CAppDaliTransmitter::sendControlInputDeviceInstanceCommand(int shortAddress,int instanceNumber,int command,int prioLevel) {
	BYTE pbData[3] = { 0, 0, 0 };
	pbData[0] = (shortAddress << 1);
	pbData[0] |= (1<<0);				// DALI 24bit command frame: bit 16 is 1
	pbData[1] = instanceNumber;			// Instance Address
	pbData[2] = command;				// opcode byte.
	return this->sendDaliMsg(24,pbData,prioLevel);
}

// 24 Bit forward frame. Broadcast naar een device.
bool CAppDaliTransmitter::sendControlInputBroadcastDeviceCommand(int command,int prioLevel) {
	BYTE pbData[3] = { 0, 0, 0 };
	pbData[0] = 0xff;					// Broadcast	
	pbData[1] = 0xfe;					// Device  addressing.
	pbData[2] = command;				// opcode byte.
	return this->sendDaliMsg(24,pbData);
}

/***********************************************************************************/
bool CAppDaliTransmitter::sendDaliMsg(int nrOfBits, const BYTE* pbData,int prioLevel) {
	if (NULL == m_pSocket) {
		return false;
	}
	if (!m_pSocket->isConnected()) {
		return false;
	}
	this->m_messageIdentifier++;
	return m_pSocket->sendMsgDali(this->m_clientIdentifier,this->m_messageIdentifier,prioLevel,nrOfBits,pbData);
}
/***********************************************************************************/
};
};
/***********************************************************************************/