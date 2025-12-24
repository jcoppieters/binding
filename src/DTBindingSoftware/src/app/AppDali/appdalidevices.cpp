#include "stdafx.h"
#include "appdalidevices.h"
#include "appdaliprotocol_def.h"
#include "appdali_common.h"
#include "appdali_def.h"
#include "utils/mfc/mfcCollections.h"
/***********************************************************************************/
namespace DUOTECNO {
namespace DALI {
/***********************************************************************************/
// private helper functie
static bool format(char* pszDest,int maxLength,unsigned long long value) {
	ASSERT(NULL != pszDest);
	char strTemp[32];
	sprintf(strTemp,"%llu",value);
	if ((int)strlen(strTemp)>maxLength) {
		return false;
	}
	strcpy(pszDest,strTemp);
	return true;
}

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
bool CAppDaliDevices_Utils::addDeviceToGroup(CAppDaliDevices* pDatabase, DaliDeviceType_t type, int shortAddress,int groupAddress) {	
	if (DALI_DEVICE_TYPE_CONTROLGEAR != type) {
		REQUIRE(DALI_DEVICE_TYPE_CONTROLGEAR == type);
		return false;
	}	
	CAppDaliDevice* pDevice = NULL;
	if (false == pDatabase->find(type,shortAddress,&pDevice)) {
		return false;
	}
	return CAppDaliDevice_Utils::addDeviceToGroup(*pDevice,groupAddress);
}

bool CAppDaliDevices_Utils::removeDeviceFromGroup(CAppDaliDevices* pDatabase,DaliDeviceType_t type, int shortAddress,int groupAddress) {
	if (DALI_DEVICE_TYPE_CONTROLGEAR != type) {
		REQUIRE(DALI_DEVICE_TYPE_CONTROLGEAR == type);
		return false;
	}
	CAppDaliDevice* pDevice = NULL;
	if (false == pDatabase->find(type,shortAddress,&pDevice)) {
		return false;
	}
	return CAppDaliDevice_Utils::removeDeviceFromGroup(*pDevice,groupAddress);
}

bool CAppDaliDevices_Utils::isDeviceGroupMember(CAppDaliDevices* pDatabase,DaliDeviceType_t type, int shortAddress, int groupAddress) {
	if (DALI_DEVICE_TYPE_CONTROLGEAR != type) {
		REQUIRE(DALI_DEVICE_TYPE_CONTROLGEAR == type);
		return false;
	}
	CAppDaliDevice* pDevice = NULL;
	if (false == pDatabase->find(type,shortAddress,&pDevice)) {
		return false;
	}
	return CAppDaliDevice_Utils::isDeviceGroupMember(*pDevice,groupAddress);
}

bool CAppDaliDevices_Utils::setDeviceSceneInfo(CAppDaliDevices* pDatabase,DaliDeviceType_t type, int shortAddress,int sceneAddress, uint8_t data) {
	if (DALI_DEVICE_TYPE_CONTROLGEAR != type) {
		REQUIRE(DALI_DEVICE_TYPE_CONTROLGEAR == type);
		return false;
	}
	CAppDaliDevice* pDevice = NULL;
	if (false == pDatabase->find(type,shortAddress,&pDevice)) {
		return false;
	}
	return CAppDaliDevice_Utils::setDeviceSceneInfo(*pDevice,sceneAddress,data);
}

bool CAppDaliDevices_Utils::setAllDevicesSceneInfo(CAppDaliDevices* pDatabase, int sceneAddress, uint8_t intensity,
	const CAppDaliObjectDT8ColorData& refColorData) {
	if (NULL == pDatabase) {
		REQUIRE(NULL != pDatabase);
		return false;
	}
	bool result = false;
	const int count = pDatabase->getCount();
	for (int i = 0; i < count; ++i) {
		CAppDaliDevice* pDevice = NULL;
		if (pDatabase->get(i, &pDevice)) {
			ASSERT(NULL != pDevice);
			if (CAppDaliDevice_Utils::setDeviceSceneInfo(pDevice, sceneAddress, intensity, refColorData)) {
				result = true;	// Vanaf wanneer er 1 aangepast werd.
			}
		}
	}
	return result;	
}


// Namen van de devices en van de device input instances copieren naar de dest database...
bool CAppDaliDevices_Utils::mergeNames(CAppDaliDevices* pDest,CAppDaliDevices& src) {
	if (NULL == pDest) {
		REQUIRE(NULL != pDest);
		return false;
	}

	bool result = false;
	if (CAppDaliDevices_Utils::mergeNames(pDest,src,DALI_DEVICE_TYPE_CONTROLGEAR)) {
		result = true;
	}
	if (CAppDaliDevices_Utils::mergeNames(pDest,src,DALI_DEVICE_TYPE_INPUTCONTROL)) {
		result = true;
	}
	return result;
}

bool CAppDaliDevices_Utils::mergeNames(CAppDaliDevices* pDest,CAppDaliDevices& src,DaliDeviceType_t type) {
	bool result = false;
	for (int address=DALI_SYSTEM_DEVICE_SHORTADDRESS_FIRST;address<=DALI_SYSTEM_DEVICE_SHORTADDRESS_LAST;++address) {
		CAppDaliDevice* pSourceDevice = NULL;
		if (src.find(type,address,&pSourceDevice)) {
			CAppDaliDevice* pDestDevice = NULL;
			ASSERT(NULL != pSourceDevice);
			if (pDest->find(type,address,&pDestDevice)) {
				ASSERT(NULL != pDestDevice);
				if (CAppDaliDevice_Utils::mergeNames(pDestDevice,*pSourceDevice)) {
					result = true;
				}
			}
		}
	}
	return result;
}
/***********************************************************************************/

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
bool CAppDaliDevice_Utils::mergeNames(CAppDaliDevice* pDest,CAppDaliDevice& src) {
	if (NULL == pDest) {
		REQUIRE(NULL != pDest);
		return false;
	}
	const DaliDeviceType_t type = src.getType();
	if (pDest->getType() != type) {
		REQUIRE(0);
		return false;
	}

	// Copy device name.
	pDest->setName(src.getName());
	
	// Only for input devices: copy alle device input instances.
	if (DALI_DEVICE_TYPE_CONTROLGEAR == type) {
		return true;
	}
	ASSERT(DALI_DEVICE_TYPE_INPUTCONTROL == type);
	return (CAppDaliDevice_Utils::mergeNamesInputDeviceInstances(pDest,src));
}

// Om te bepalen of een device het DALI-2 protocol ondersteund zijn er geen queries in het
// protocol die deze info kunnen opvragen.
// Dit kan enkel bepaald worden of een device geantwoord heeft op nieuwe queries die enkel aanwezig
// zijn in het DALI-2 protocol.
//
// Control input device: Altijd DALI V2
// Control gear device: 
//	- Wanneer de extended fade time ondersteund wordt. (Wanneer geen antwoord/niet ondersteund dan is deze -1)
//	- TODO: Nog andere?
bool CAppDaliDevice_Utils::supportDALIVersion2(CAppDaliDevice& refDevice) {	
	const DaliDeviceType_t type = refDevice.getType();
	if (DALI_DEVICE_TYPE_INPUTCONTROL == type) {
		return true;
	}
	CAppDaliDeviceProperties_ControlGear* const properties = dynamic_cast<CAppDaliDeviceProperties_ControlGear*>(&refDevice.getProperties());
	if (NULL == properties) {
		return false;
	}
	if (properties->getExtendedFadeTime() >= 0) {
		return true;
	}
	return false;
}

bool CAppDaliDevice_Utils::isControlGearDeviceTypeDT8(CAppDaliDevice& refDevice) {
	const DaliDeviceType_t type = refDevice.getType();
	if (DALI_DEVICE_TYPE_INPUTCONTROL == type) {
		return false;
	}
	CAppDaliDeviceProperties_ControlGear* const properties = dynamic_cast<CAppDaliDeviceProperties_ControlGear*>(&refDevice.getProperties());
	if (NULL == properties) {
		return false;
	}
	if (8 == properties->getDeviceType()) {
		return true;
	}
	const CList<int>& listDeviceTypes = properties->getDeviceTypes();
	if (NULL != listDeviceTypes.Find(8)) {
		return true;
	}
	return false;

}

bool CAppDaliDevice_Utils::mergeNamesInputDeviceInstances(CAppDaliDevice* pDest,CAppDaliDevice& src) {
	if (NULL == pDest) {
		REQUIRE(NULL != pDest);
		return false;
	}
	const DaliDeviceType_t type = src.getType();
	if ((pDest->getType() != type) || (DALI_DEVICE_TYPE_INPUTCONTROL != type))  {
		REQUIRE(0);
		return false;
	}
	CAppDaliDeviceProperties_InputControlDevice* const destProperties = dynamic_cast<CAppDaliDeviceProperties_InputControlDevice*>(&pDest->getProperties());
	CAppDaliDeviceProperties_InputControlDevice* const srcProperties = dynamic_cast<CAppDaliDeviceProperties_InputControlDevice*>(&src.getProperties());
	if (NULL == destProperties || NULL == srcProperties) {
		INVARIANT(NULL != destProperties);
		INVARIANT(NULL != srcProperties);
		return false;
	}

	bool result = false;
	for (int instanceNumber=0;instanceNumber<32;++instanceNumber) {
		CAppDaliInputControlDeviceInstance* pSrcDeviceInstance = NULL;
		if (srcProperties->getInstances().find(instanceNumber,&pSrcDeviceInstance)) {
			ASSERT(NULL != pSrcDeviceInstance);
			CAppDaliInputControlDeviceInstance* pDestDeviceInstance = NULL;
			if (destProperties->getInstances().find(instanceNumber,&pDestDeviceInstance)) {
				ASSERT(NULL != pDestDeviceInstance);
				pDestDeviceInstance->setName(pSrcDeviceInstance->getName());
				result = true;
			}
		}	
	}
	return result;
}



bool CAppDaliDevice_Utils::addDeviceToGroup(CAppDaliDevice& refDevice,int groupAddress) {
	if (groupAddress < 0 || groupAddress > 15) {
		ASSERT(0);
		return false;
	}
	CAppDaliDeviceProperties_ControlGear* properties = NULL;
	if (false == getControlGearProperties(refDevice,&properties)) {
		return false;
	}
	ASSERT(NULL != properties);
	int groupInfo = properties->getGroupInfo();
	groupInfo |= (1 << groupAddress);
	if (groupInfo == properties->getGroupInfo()) {
		return false;
	}	
	properties->setGroupInfo(groupInfo);
	return true;
}

bool CAppDaliDevice_Utils::removeDeviceFromGroup(CAppDaliDevice& refDevice,int groupAddress) {
	if (groupAddress < 0 || groupAddress > 15) {
		ASSERT(0);
		return false;
	}
	CAppDaliDeviceProperties_ControlGear* properties = NULL;
	if (false == getControlGearProperties(refDevice,&properties)) {
		return false;
	}
	ASSERT(NULL != properties);
	int groupInfo = properties->getGroupInfo();
	groupInfo &= ~(1 << groupAddress);
	if (groupInfo == properties->getGroupInfo()) {
		return false;
	}	
	properties->setGroupInfo(groupInfo);
	return true;
}

bool CAppDaliDevice_Utils::isDeviceGroupMember(CAppDaliDevice& refDevice,int groupAddress) {
	if (groupAddress < 0 || groupAddress > 15) {
		ASSERT(0);
		return false;
	}
	CAppDaliDeviceProperties_ControlGear* properties = NULL;
	if (false == getControlGearProperties(refDevice,&properties)) {
		return false;
	}
	ASSERT(NULL != properties);
	const int mask = (1 << groupAddress);
	const int groupInfo = properties->getGroupInfo();	
	if (mask == (groupInfo & mask)) {
		return true;
	}
	return false;
}

bool CAppDaliDevice_Utils::setDeviceSceneInfo(CAppDaliDevice& refDevice,int sceneAddress,uint8_t data) {
	if (sceneAddress < 0 || sceneAddress > 15) {
		ASSERT(0);
		return false;
	}
	CAppDaliDeviceProperties_ControlGear* properties = NULL;
	if (false == getControlGearProperties(refDevice,&properties)) {
		return false;
	}
	ASSERT(NULL != properties);
	int sceneInfo = properties->getSceneLevel(sceneAddress);
	if (sceneInfo == data) {
		return false;
	}
	properties->setSceneLevel(sceneAddress,data);
	return true;
}

bool CAppDaliDevice_Utils::getControlGearProperties(CAppDaliDevice& refDevice,CAppDaliDeviceProperties_ControlGear** ppProperties) {
	if (DALI_DEVICE_TYPE_CONTROLGEAR != refDevice.getType()) {
		return false;
	}
	CAppDaliDeviceProperties_ControlGear* const properties = dynamic_cast<CAppDaliDeviceProperties_ControlGear*>(&refDevice.getProperties());
	if (NULL == properties) {
		return false;
	}
	*ppProperties = properties;
	return true;
}

void CAppDaliDevice_Utils::setDefaults(CAppDaliInputControlDeviceInstance* pDest) {
	if (NULL == pDest) {
		ASSERT(NULL != pDest);
		return;	
	}
	bool instanceEnable = true;
	const int type = pDest->getInstanceType();
	if (DALI_SYSTEM_CONTROLDEVICE_INSTANCE_TYPE_PUSHBUTTON == type) {
		pDest->setEventFilter(DALI_CONTROLDEVICE_PUSHBUTTON_DEFAULT_EVENTFILTER);
	}
	else if (DALI_SYSTEM_CONTROLDEVICE_INSTANCE_TYPE_DETECTOR_OCCUPANCY == type) {
		pDest->setEventFilter(DALI_CONTROLDEVICE_DETECTOR_DEFAULT_EVENTFILTER);
	}
	else {
		instanceEnable = false;
	}
	pDest->setEventScheme(DALI_CONTROLDEVICE_EVENT_SCHEME_DEFAULT);
	pDest->setInstanceActive(instanceEnable);
}

void CAppDaliDevice_Utils::setDefaults(CAppDaliDeviceProperties_ControlGear* pDest) {
	if (NULL == pDest) {
		ASSERT(NULL != pDest);
		return;	
	}

	// Minimum level.
	int physicalMinimum = pDest->getPhysicalMinValue();
	if (physicalMinimum < 0) {
		physicalMinimum = 1;
	}

	pDest->setLevelMin(physicalMinimum);
	pDest->setLevelMax(0xFE);
	pDest->setLevelSystemFailure(0xFE);
	pDest->setLevelPowerOn(0xFE);
	pDest->setFadeTime(pDest, 0);
	pDest->setFadeRate(pDest, 7);
}

void CAppDaliDevice_Utils::setScenesDefaults(CAppDaliDeviceProperties_ControlGear* pDest) {
	if (NULL == pDest) {
		ASSERT(NULL != pDest);
		return;	
	}

	// Intensity op mask plaatsen.
	for (int i=DALI_SYSTEM_CONTROLGEAR_SCENE_FIRST;i<=DALI_SYSTEM_CONTROLGEAR_SCENE_LAST;++i) {
		pDest->setSceneLevel(i,0xFF);
	}

	// DT8 colortypes op mask zetten indien aanwezig.
	CAppDaliDeviceProperties_ControlGear::clearDT8SceneColorData(pDest);
}

bool CAppDaliDevice_Utils::setDeviceSceneInfo(CAppDaliDevice* pDevice, int sceneAddress, uint8_t intensity, const CAppDaliObjectDT8ColorData& refColorData) {
	if (NULL == pDevice) {
		REQUIRE(NULL != pDevice);
		return false;
	}
	CAppDaliDeviceProperties_ControlGear* properties = NULL;
	if (!getControlGearProperties(*pDevice, &properties)) {
		return false;
	}	
	// properties
	ASSERT(NULL != properties);
	properties->setSceneLevel(sceneAddress, intensity);

	// scene kleur info: Enkel wanneer het kleur type ondersteund wordt.
	if (false == CAppDaliDeviceProperties_ControlGear::isDT8ColorObjectSupported(*properties, refColorData.getColorType())) {
		return true;
	}
	if (!properties->setDT8SceneColorInfo(sceneAddress, refColorData)) {
		return false;
	}	
	TRACE("Updated device A%d scene.\r\n",pDevice->getShortAddress());
	return true;
}
/***********************************************************************************/

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
CAppDaliDevices::CAppDaliDevices( ) : m_list( ) { }
CAppDaliDevices::~CAppDaliDevices( ) { 
	this->erase();
}

void CAppDaliDevices::erase( ) {
	while (!m_list.IsEmpty()) {
		CAppDaliDevice* const p = m_list.RemoveHead( );
		ASSERT( p );
		delete p;
	}
	ASSERT(m_list.GetCount( )==0 );
}

bool CAppDaliDevices::find(DaliDeviceType_t type, int shortAddress, CAppDaliDevice **ppDevice) const {
	POSITION pos = m_list.GetHeadPosition();
	while(pos!=NULL)  {
		CAppDaliDevice* pEntry = 0;
		if ((pEntry=m_list.GetNext(pos)) != NULL) {	
			if ((shortAddress == pEntry->getShortAddress()) && (type == pEntry->getType())) {
				if (0 != ppDevice) {
					*ppDevice = pEntry;
				}
				return true;
			}			
		}
	}
	return false;
}

bool CAppDaliDevices::add(CAppDaliDevice* pNewDevice) {
	if (0 == pNewDevice) {
		return false;
	}
	// ASSERT(false == find(pNewDevice->getMACAddress(),0));
	m_list.AddTail(pNewDevice);
	return true;
}

int CAppDaliDevices::getCount() const {
	return m_list.GetCount();
}

bool CAppDaliDevices::get(int index,CAppDaliDevice **ppDevice) const {
	if (0 == ppDevice) {
		return false;
	}
	if (index>=m_list.GetCount()) {
		return false;
	}
	const POSITION pos = m_list.FindIndex( index );
	if (pos == NULL)
	{
		return false;
	}
	*ppDevice = m_list.GetAt(pos);
	return true;
}

bool CAppDaliDevices::remove(DaliDeviceType_t type, int shortAddress) {
	int index = 0;
	int indexInList = -1;

	{
		POSITION pos = m_list.GetHeadPosition();
		while(pos!=NULL)  {		
			CAppDaliDevice* pEntry = NULL;
			if ((pEntry=m_list.GetNext(pos)) != NULL) {
				if ((shortAddress == pEntry->getShortAddress()) && (type == pEntry->getType())) {
					indexInList = index;	
					break;
				}
				index++;
			}
		}
	}

	if (indexInList >=0) {
		ASSERT(indexInList < m_list.GetCount());
		const POSITION tempPos = m_list.FindIndex(indexInList);
		if (NULL != tempPos) {
			CAppDaliDevice* const p = m_list.GetAt(tempPos);
			if (NULL != p) {
				ASSERT((shortAddress == p->getShortAddress()) && (type == p->getType()));
				delete p;
			}		
			m_list.RemoveAt(tempPos);
			return true;
		}
	}
	return false;
}

CAppDaliDevices* CAppDaliDevices::clone( ) {
	CAppDaliDevices* const pNewInstance = new CAppDaliDevices();
	if (NULL == pNewInstance) {
		ASSERT(NULL != pNewInstance);
		return pNewInstance;
	}
	bool cleanup = false;
	const int count = this->getCount();
	for (int i=0;i<count;++i) {
		CAppDaliDevice* pDevice = NULL;
		if (this->get(i,&pDevice)) {
			ASSERT(NULL != pDevice);
			CAppDaliDevice* const pNewDevice = pDevice->clone();
			if (NULL == pNewDevice) {
				cleanup = true;
				break;
			}
			if (!pNewInstance->add(pNewDevice)) {
				ASSERT(0);
				cleanup=true;
				break;
			}
		}
	}
	if (cleanup) {
		delete pNewInstance;
		return NULL;
	}
	return pNewInstance;
}

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
CAppDaliDevice::CAppDaliDevice(DaliDeviceType_t type) : 
	m_type(type), m_strName(), m_properties(NULL), m_shortAddress(-1), m_GTIN(0), m_serialCode(0),
	m_versionFirmware(0), m_versionHardware(0), m_errorStatus(ERROR_STATUS_NONE), m_errorCode(ERROR_CODE_NONE)
{ 
	if (DALI_DEVICE_TYPE_CONTROLGEAR == type) {
		m_properties =  new CAppDaliDeviceProperties_ControlGear( );		
	}
	else if (DALI_DEVICE_TYPE_INPUTCONTROL == type) {
		m_properties =  new CAppDaliDeviceProperties_InputControlDevice( );		
	}	
	ASSERT(NULL != m_properties);
}
CAppDaliDevice::~CAppDaliDevice( ) { 
	if (NULL != m_properties) {
		delete this->m_properties;
		this->m_properties = NULL;
	}
}

DaliDeviceType_t CAppDaliDevice::getType() const {
	return this->m_type;
}

void CAppDaliDevice::setName(const CString& str) {
	this->m_strName = str;
}
const CString& CAppDaliDevice::getName( ) const {
	return this->m_strName;
}

int CAppDaliDevice::getShortAddress( ) const {
	return this->m_shortAddress;
}
void CAppDaliDevice::setShortAddress(int v) {
	ASSERT(v>=DALI_SYSTEM_DEVICE_SHORTADDRESS_FIRST && v<=DALI_SYSTEM_DEVICE_SHORTADDRESS_LAST);
	this->m_shortAddress = v;
}

unsigned long long CAppDaliDevice::getGTIN( ) const {
	return this->m_GTIN;
}

void CAppDaliDevice::setGTIN(unsigned long long v) {
	this->m_GTIN = v;
}

unsigned long long CAppDaliDevice::getSerialcode( ) const {
	return this->m_serialCode;
}

void CAppDaliDevice::setSerialcode(unsigned long long v) {
	this->m_serialCode = v;
}

int CAppDaliDevice::getVersionFirmware( ) const {
	return this->m_versionFirmware;
}
void CAppDaliDevice::setVersionFirmware(int v) {
	this->m_versionFirmware = v;
}

int CAppDaliDevice::getVersionHardware( ) const {
	return this->m_versionHardware;
}

void CAppDaliDevice::setVersionHardware(int v) {
	this->m_versionHardware = v;
}

void CAppDaliDevice::setErrorStatus(DaliDeviceErrorStatus_t status) {
	this->m_errorStatus = status;
}
DaliDeviceErrorStatus_t CAppDaliDevice::getErrorStatus( ) const {
	return this->m_errorStatus;
}

void CAppDaliDevice::setErrorCode(DaliErrorCode_t errorCode) {
	this->m_errorCode = errorCode;
}

DaliErrorCode_t CAppDaliDevice::getErrorCode( ) const {
	return this->m_errorCode;
}

// Opvragen van de device input/output properties...
CAppDaliDeviceProperties& CAppDaliDevice::getProperties() {
	ASSERT(NULL != this->m_properties);
	return *this->m_properties;
}

const CAppDaliDeviceProperties& CAppDaliDevice::getProperties() const {
	ASSERT(NULL != this->m_properties);
	return *this->m_properties;
}

// Utilities...
bool CAppDaliDevice::getStrGTIN(const CAppDaliDevice& ref,char* pszDest,int maxLength) {	
	return format(pszDest,maxLength,ref.m_GTIN);
}

bool CAppDaliDevice::getStrSerialCode(const CAppDaliDevice& ref,char* pszDest,int maxLength) {
	return format(pszDest,maxLength,ref.m_serialCode);
}

CAppDaliDevice* CAppDaliDevice::clone( ) {
	CAppDaliDevice* const pNewInstance = new CAppDaliDevice(this->m_type);
	if (NULL == pNewInstance) {
		ASSERT(NULL != pNewInstance);
		return NULL;
	}

	if (DALI_DEVICE_TYPE_CONTROLGEAR == this->m_type) {	
		CAppDaliDeviceProperties_ControlGear* const p = dynamic_cast<CAppDaliDeviceProperties_ControlGear*>(&this->getProperties());
		if (NULL == p) {
			ASSERT(0);
			delete pNewInstance;
			return NULL;
		}
		pNewInstance->m_properties = p->clone();
		if (NULL == pNewInstance->m_properties) {
			ASSERT(0);
			delete pNewInstance;
			return NULL;
		}
	}
	else if (DALI_DEVICE_TYPE_INPUTCONTROL) {
		CAppDaliDeviceProperties_InputControlDevice* const p = dynamic_cast<CAppDaliDeviceProperties_InputControlDevice*>(&this->getProperties());
		if (NULL == p) {
			ASSERT(0);
			delete pNewInstance;
			return NULL;
		}
		pNewInstance->m_properties = p->clone();
		if (NULL == pNewInstance->m_properties) {
			ASSERT(0);
			delete pNewInstance;
			return NULL;
		}
	}
	else {
		ASSERT(0);
		delete pNewInstance;
		return NULL;
	}

	ASSERT(NULL != pNewInstance);
	pNewInstance->m_strName = this->m_strName;
	pNewInstance->m_shortAddress = this->m_shortAddress;
	pNewInstance->m_GTIN = this->m_GTIN;
	pNewInstance->m_serialCode = this->m_serialCode;
	pNewInstance->m_versionFirmware = this->m_versionFirmware;			
	pNewInstance->m_versionHardware = this->m_versionHardware;
	pNewInstance->m_errorStatus = this->m_errorStatus;
	pNewInstance->m_errorCode = this->m_errorCode;
	return pNewInstance;
}

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
CAppDaliDeviceProperties::CAppDaliDeviceProperties( ) : m_deviceStatus(-1),						
	m_operatingMode(-1), m_versionNumber(1) {
}
CAppDaliDeviceProperties::~CAppDaliDeviceProperties( ) { }

int CAppDaliDeviceProperties::getDeviceStatus( ) {
	return this->m_deviceStatus;
}
void CAppDaliDeviceProperties::setDeviceStatus(int v) {
	// REQUIRE(v>=0 && v<=255);
	this->m_deviceStatus = v;
}

int CAppDaliDeviceProperties::getOperatingmode( ) {
	return this->m_operatingMode;
}
void CAppDaliDeviceProperties::setOperatingMode(int v) {
	REQUIRE(v>=0 && v<=255);
	this->m_operatingMode = v;
}

int CAppDaliDeviceProperties::getVersionNumber( ) {
	return this->m_versionNumber;
}
void CAppDaliDeviceProperties::setVersionNumber(int v) {
	// REQUIRE(v>=0 && v<=255);
	this->m_versionNumber = v;
}

bool CAppDaliDeviceProperties::copy(CAppDaliDeviceProperties* pDest,const CAppDaliDeviceProperties& src) {
	if (NULL == pDest) {
		ASSERT(NULL != pDest);
		return false;
	}
	pDest->m_deviceStatus = src.m_deviceStatus;
	pDest->m_operatingMode = src.m_operatingMode;
	pDest->m_versionNumber = src.m_versionNumber;	
	return true;
}
/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
CAppDaliDeviceProperties_ControlGear::CAppDaliDeviceProperties_ControlGear( ) : m_bitfieldGroupInfo(0), 
	m_deviceType(-1), m_levelMin(-1), m_levelMax(-1), m_fadeTimeRate(-1), m_physicalMinValue(-1), 
	m_levelPowerOn(-1), m_levelSystemFailure(-1), m_extendedFadeTime(-1), m_actualLevel(0), m_dimmingCurve(-1), 
	m_colorFeatures(-1), m_deviceTypes(), m_mapColorScenes() {
	for (int i = 0; i < MAX_NR_SCENES; ++i) {
		this->m_sceneInfo[i] = 0xff;
	}
}

CAppDaliDeviceProperties_ControlGear::~CAppDaliDeviceProperties_ControlGear( ) { 
	this->eraseColorDT8SceneInfo(&this->m_mapColorScenes);
}

void CAppDaliDeviceProperties_ControlGear::setGroupInfo(uint16_t groupInfo) {
	this->m_bitfieldGroupInfo = groupInfo;
}
unsigned short CAppDaliDeviceProperties_ControlGear::getGroupInfo( ) const {
	return this->m_bitfieldGroupInfo;
}

int CAppDaliDeviceProperties_ControlGear::getDeviceType( ) const {
	return this->m_deviceType;
}
void CAppDaliDeviceProperties_ControlGear::setDeviceType(int v) {
	REQUIRE((v==-1) || (v>=0 && v<=255));
	this->m_deviceType = v;
}

int CAppDaliDeviceProperties_ControlGear::getLevelMin( ) const {
	return this->m_levelMin;
}
void CAppDaliDeviceProperties_ControlGear::setLevelMin(int v) {
	REQUIRE((v==-1) || (v>=0 && v<=255));
	this->m_levelMin = v;
}

int CAppDaliDeviceProperties_ControlGear::getLevelMax( ) const {	
	return this->m_levelMax;
}
void CAppDaliDeviceProperties_ControlGear::setLevelMax(int v) {
	REQUIRE((v==-1) || (v>=0 && v<=255));
	this->m_levelMax = v;
}

int CAppDaliDeviceProperties_ControlGear::getFadeTimeRate( ) const {
	return this->m_fadeTimeRate;
}
void CAppDaliDeviceProperties_ControlGear::setFadeTimeRate(int v) {
	REQUIRE((v==-1) || (v>=0 && v<=255));
	this->m_fadeTimeRate = v;
}

void CAppDaliDeviceProperties_ControlGear::setFadeTime(CAppDaliDeviceProperties_ControlGear* p,int v) {
	REQUIRE(v>=0 && v<=15);
	if (p->m_fadeTimeRate < 0) {
		p->m_fadeTimeRate = 0;
	}
	p->m_fadeTimeRate &= 0x0F;
	p->m_fadeTimeRate |= ((v & 0x0F) << 4);
}

int CAppDaliDeviceProperties_ControlGear::getFadeTime(const CAppDaliDeviceProperties_ControlGear& r) {
	if (r.m_fadeTimeRate < 0) {
		return -1;
	}
	return ((r.m_fadeTimeRate & 0xF0) >> 4);
}


void CAppDaliDeviceProperties_ControlGear::setFadeRate(CAppDaliDeviceProperties_ControlGear* p,int v) {
	REQUIRE(v>=0 && v<=15);
	if (p->m_fadeTimeRate < 0) {
		p->m_fadeTimeRate = 0;
	}
	p->m_fadeTimeRate &= 0xF0;
	p->m_fadeTimeRate |= (v & 0x0F);
}


int CAppDaliDeviceProperties_ControlGear::getFadeRate(const CAppDaliDeviceProperties_ControlGear& r) {
	if (r.m_fadeTimeRate < 0) {
		return -1;
	}
	return (r.m_fadeTimeRate & 0x0F);
}

int CAppDaliDeviceProperties_ControlGear::getPhysicalMinValue( ) const {
	return this->m_physicalMinValue;
}
void CAppDaliDeviceProperties_ControlGear::setPhysicalMinValue(int v) {
	REQUIRE((v==-1) || (v>=0 && v<=255));
	this->m_physicalMinValue = v;
}

int CAppDaliDeviceProperties_ControlGear::getLevelPowerOn( ) const {
	return this->m_levelPowerOn;
}
void CAppDaliDeviceProperties_ControlGear::setLevelPowerOn(int v) {
	REQUIRE((v==-1) || (v>=0 && v<=255));
	this->m_levelPowerOn = v;
}

int CAppDaliDeviceProperties_ControlGear::getLevelSystemFailure( ) const {
	return this->m_levelSystemFailure;
}
void CAppDaliDeviceProperties_ControlGear::setLevelSystemFailure(int v) {
	REQUIRE((v==-1) || (v>=0 && v<=255));
	this->m_levelSystemFailure = v;
}

int CAppDaliDeviceProperties_ControlGear::getExtendedFadeTime( ) const {
	return this->m_extendedFadeTime;
}
void CAppDaliDeviceProperties_ControlGear::setExtendedFadeTime(int v) {
	REQUIRE((v==-1) || (v>=0 && v<=255));
	this->m_extendedFadeTime = v;	
}

// m_extendedFadeTime

void CAppDaliDeviceProperties_ControlGear::setSceneLevel(int sceneNumber, int value) {
	REQUIRE(sceneNumber >= 0 && sceneNumber < MAX_NR_SCENES);
	REQUIRE(value >= 0 && value <= 255);
	if ((sceneNumber < 0) || (sceneNumber >= MAX_NR_SCENES)) {
		return;
	}
	this->m_sceneInfo[sceneNumber] = value;
}

int CAppDaliDeviceProperties_ControlGear::getSceneLevel(int sceneNumber) const {
	REQUIRE(sceneNumber >= 0 && sceneNumber < MAX_NR_SCENES);
	if (sceneNumber < 0 || sceneNumber >= MAX_NR_SCENES) {
		return 0xff;
	}
	return this->m_sceneInfo[sceneNumber];
}

void CAppDaliDeviceProperties_ControlGear::setActualLevel(int actualLevel) {
	// REQUIRE(actualLevel >= 0 && actualLevel <= 255);
	this->m_actualLevel = actualLevel;
}
int CAppDaliDeviceProperties_ControlGear::getActualLevel() const {
	return this->m_actualLevel;
}

void CAppDaliDeviceProperties_ControlGear::setDimmingCurve(int val) {
	this->m_dimmingCurve = val;
}
int CAppDaliDeviceProperties_ControlGear::getDimmingCurve() const {
	return this->m_dimmingCurve;
}

const CList<int>& CAppDaliDeviceProperties_ControlGear::getDeviceTypes() const {
	return this->m_deviceTypes;
}
void CAppDaliDeviceProperties_ControlGear::setDeviceTypes(CList<int>& list) {
	this->m_deviceTypes.RemoveAll( );

	POSITION pos = list.GetHeadPosition();
	while(pos!=NULL) {	
		this->m_deviceTypes.AddTail(list.GetNext(pos));				
	}
}

void CAppDaliDeviceProperties_ControlGear::setColorFeatures(int v) {
	this->m_colorFeatures = v;
}
int CAppDaliDeviceProperties_ControlGear::getColorFeatures( ) const {
	return this->m_colorFeatures;
}

// Utilities.
bool CAppDaliDeviceProperties_ControlGear::isSceneActive(const CAppDaliDeviceProperties_ControlGear& r, int sceneNumber) {
	if ((sceneNumber < 0) || (sceneNumber >= MAX_NR_SCENES)) {
		REQUIRE((sceneNumber>=0) && (sceneNumber<MAX_NR_SCENES));
		return false;
	}
	if ((r.m_sceneInfo[sceneNumber] >=0) && (r.m_sceneInfo[sceneNumber] < 0xFF)) {
		return true;
	}
	CAppDaliObjectDT8ColorData* pSceneColorData = NULL;
	if (FALSE == r.m_mapColorScenes.Lookup(sceneNumber,pSceneColorData)) {
		return false;
	}
	ASSERT(NULL != pSceneColorData);
	return (CAppDaliObjectDT8ColorData_Utils::isActive(*pSceneColorData));
}

bool CAppDaliDeviceProperties_ControlGear::isDT8ColorObjectSupported(const CAppDaliDeviceProperties_ControlGear& r) {
	return ((r.m_colorFeatures > 0) && (r.m_colorFeatures < 0xFF));
}


bool CAppDaliDeviceProperties_ControlGear::isDT8ColorObjectSupported(const CAppDaliDeviceProperties_ControlGear& r, int sceneColorType) {
	if (sceneColorType < 0 || sceneColorType >= 0xff) {
		return false;
	}
	if (r.m_colorFeatures < 0 || r.m_colorFeatures >= 0xff) {
		return false;
	}
	switch (sceneColorType) {
		case DALI_DT8_COLOUR_TYPE_XY_COORDINATE: {
			if (r.m_colorFeatures & DALI_DT8_COLOR_FEATURES_XY_COORDINATE) {
				return true;
			}
		} break;
		case DALI_DT8_COLOUR_TYPE_TEMPERATURE_TC: {
			if (r.m_colorFeatures & DALI_DT8_COLOR_FEATURES_COLOR_TEMPERATURE) {
				return true;
			}
		} break;
		case DALI_DT8_COLOUR_TYPE_PRIMARY_N: {
			const int nrOfChannels = getColorFeaturesNrOfPrimaryNChannels(r);
			if (nrOfChannels > 0) {
				return true;
			}				
		} break;
		case DALI_DT8_COLOUR_TYPE_RGBWAF: {
			const int nrOfChannels = getColorFeaturesNrOfRGBWAFChannels(r);
			if (nrOfChannels > 0) {
				return true;
			}
		} break;
		default: {
			ASSERT(0);
		} break;
	}
	return false;
}

// Opvragen aantal channels.
int CAppDaliDeviceProperties_ControlGear::getColorFeaturesNrOfRGBWAFChannels(const CAppDaliDeviceProperties_ControlGear& r) {
	if (r.m_colorFeatures < 0 || r.m_colorFeatures == 0xff) {
		return 0;
	}
	return ((r.m_colorFeatures & DALI_DT8_COLOR_FEATURES_MASK_NUMBER_OF_RGBWAF_CHANNELS) >> 5);
}

int CAppDaliDeviceProperties_ControlGear::getColorFeaturesNrOfPrimaryNChannels(const CAppDaliDeviceProperties_ControlGear& r) {
	if (r.m_colorFeatures < 0 || r.m_colorFeatures == 0xff) {
		return 0;
	}
	return ((r.m_colorFeatures & DALI_DT8_COLOR_FEATURES_MASK_NUMBER_OF_PRIMARY_CHANNELS) >> 2);
}

bool CAppDaliDeviceProperties_ControlGear::getColorFeaturesSupportColorTemperatureTC(const CAppDaliDeviceProperties_ControlGear& r) {
	if (r.m_colorFeatures < 0 || r.m_colorFeatures == 0xff) {
		return 0;
	}
	return (DALI_DT8_COLOR_FEATURES_COLOR_TEMPERATURE == (r.m_colorFeatures & DALI_DT8_COLOR_FEATURES_COLOR_TEMPERATURE));
}

bool CAppDaliDeviceProperties_ControlGear::getColorFeaturesSupportColorCoordinatesXY(const CAppDaliDeviceProperties_ControlGear& r) {
	if (r.m_colorFeatures < 0 || r.m_colorFeatures == 0xff) {
		return 0;
	}
	return (DALI_DT8_COLOR_FEATURES_XY_COORDINATE == (r.m_colorFeatures & DALI_DT8_COLOR_FEATURES_XY_COORDINATE));
}

// Oppassen:
// DT8 objecten waneer er een colortype aanwezig is dan moeten de waarden op MASK geplaatst worden en doorgestuurd
// worden met dit color type.
void CAppDaliDeviceProperties_ControlGear::clearDT8SceneColorData(CAppDaliDeviceProperties_ControlGear* pDest) {
	if (NULL == pDest) {
		REQUIRE(NULL != pDest);
		return;
	}	
	for (int sceneNumber=DALI_SYSTEM_CONTROLGEAR_SCENE_FIRST;sceneNumber<=DALI_SYSTEM_CONTROLGEAR_SCENE_LAST;++sceneNumber) {
		clearDT8SceneColorData(pDest,sceneNumber);		
	}
}

void CAppDaliDeviceProperties_ControlGear::clearDT8SceneColorData(CAppDaliDeviceProperties_ControlGear* pDest,int sceneNumber) {
	if (NULL == pDest) {
		REQUIRE(NULL != pDest);
		return;
	}	
	if (!isDT8ColorObjectSupported(*pDest)) {
		return;
	}
	CAppDaliObjectDT8ColorData* pObject = NULL;
	if (pDest->m_mapColorScenes.Lookup(sceneNumber,pObject)) {
		const int colorType = pObject->getColorType();
		if ((-1 != colorType) || (0xFF != colorType)) {
			static const bool initColorType = true;
			pObject->setColorType(colorType,initColorType);
		}
	}
}

// Volledige map.
void CAppDaliDeviceProperties_ControlGear::setDT8SceneColorInfo(const CMapDT8ColorScenesInfo& src) {
	copyColorDT8SceneInfo(&this->m_mapColorScenes, src);
}

const CAppDaliDeviceProperties_ControlGear::CMapDT8ColorScenesInfo& CAppDaliDeviceProperties_ControlGear::getT8SceneColorInfo() const {
	return this->m_mapColorScenes;
}

// Individueel item.
bool CAppDaliDeviceProperties_ControlGear::setDT8SceneColorInfo(int sceneNumber, const CAppDaliObjectDT8ColorData& src) {
	if ((sceneNumber < DALI_SYSTEM_CONTROLGEAR_SCENE_FIRST) || (sceneNumber > DALI_SYSTEM_CONTROLGEAR_SCENE_LAST)) {
		REQUIRE(sceneNumber>=DALI_SYSTEM_CONTROLGEAR_SCENE_FIRST && sceneNumber<=DALI_SYSTEM_CONTROLGEAR_SCENE_LAST);
		return false;
	}

	CAppDaliObjectDT8ColorData* pObject = NULL;
	if (this->m_mapColorScenes.Lookup(sceneNumber,pObject)) {
		this->m_mapColorScenes.RemoveKey(sceneNumber);
		delete pObject;
		pObject=NULL;		
	}

	ASSERT(NULL == pObject);
	pObject = new CAppDaliObjectDT8ColorData(src);
	if (NULL == pObject) {
		ASSERT(NULL != pObject);
		return false;
	}
	this->m_mapColorScenes.SetAt(sceneNumber, pObject);
	return true;
}

const CAppDaliObjectDT8ColorData& CAppDaliDeviceProperties_ControlGear::getDT8SceneColorInfo(int sceneNumber) const {
	static const CAppDaliObjectDT8ColorData emptyObject;
	if ((sceneNumber < DALI_SYSTEM_CONTROLGEAR_SCENE_FIRST) || (sceneNumber > DALI_SYSTEM_CONTROLGEAR_SCENE_LAST)) {
		REQUIRE(sceneNumber >= DALI_SYSTEM_CONTROLGEAR_SCENE_FIRST && sceneNumber <= DALI_SYSTEM_CONTROLGEAR_SCENE_LAST);
		return emptyObject;
	}
	CAppDaliObjectDT8ColorData* pObject = NULL;
	if (FALSE == this->m_mapColorScenes.Lookup(sceneNumber, pObject)) {
		return emptyObject;
	}
	ASSERT(NULL != pObject);
	return *pObject;
}


// bool getDT8SceneColorInfo(int sceneNumber, CAppDaliObjectDT8ColorData** ppRefItem);	


bool CAppDaliDeviceProperties_ControlGear::isGroupMember(CAppDaliDeviceProperties_ControlGear& refDevice,int groupNumber) {
	if (groupNumber<0 || groupNumber>=16) {
		REQUIRE(groupNumber>=0 && groupNumber<16);
		return false;
	}
	const uint16_t mask = (1 << groupNumber);
	return (mask == (refDevice.m_bitfieldGroupInfo & mask));
}

bool CAppDaliDeviceProperties_ControlGear::isSupported(CAppDaliDeviceProperties_ControlGear& refDevice,int deviceType) {
	if (refDevice.m_deviceType == deviceType) {
		return true;
	}
	POSITION pos = refDevice.m_deviceTypes.GetHeadPosition();
	while(pos!=NULL) {	
		if (deviceType == refDevice.m_deviceTypes.GetNext(pos)) {			
			return true;
		}
	}
	return false;
}

CAppDaliDeviceProperties_ControlGear* CAppDaliDeviceProperties_ControlGear::clone( ) {
	CAppDaliDeviceProperties_ControlGear* const pNewInstance = new CAppDaliDeviceProperties_ControlGear();
	if (NULL == pNewInstance) {
		ASSERT(NULL != pNewInstance);
		return NULL;
	}	
	CAppDaliDeviceProperties_ControlGear::copy(pNewInstance,*this);		
	return pNewInstance;
}

bool CAppDaliDeviceProperties_ControlGear::copy(CAppDaliDeviceProperties_ControlGear* pDest,const CAppDaliDeviceProperties_ControlGear& src,uint8_t flags) {
	if (NULL == pDest) {
		REQUIRE(NULL != pDest);
		return false;
	}

	if (flags & FLAG_COPY_PROPERTIES) {
		// Basisklasse	
		CAppDaliDeviceProperties::copy(pDest,src);

		// Eigen properties.
		pDest->m_bitfieldGroupInfo = src.m_bitfieldGroupInfo;
		pDest->m_deviceType = src.m_deviceType;					
		pDest->m_levelMin = src.m_levelMin;							
		pDest->m_levelMax = src.m_levelMax;					
		pDest->m_fadeTimeRate = src.m_fadeTimeRate;					
		pDest->m_physicalMinValue = src.m_physicalMinValue;					
		pDest->m_levelPowerOn = src.m_levelPowerOn;						
		pDest->m_levelSystemFailure = src.m_levelSystemFailure;				
		pDest->m_extendedFadeTime = src.m_extendedFadeTime;					
		pDest->m_actualLevel = src.m_actualLevel;
		pDest->m_dimmingCurve = src.m_dimmingCurve;
		pDest->m_colorFeatures = src.m_colorFeatures;

		pDest->m_deviceTypes.RemoveAll();
		POSITION pos = src.m_deviceTypes.GetHeadPosition();
		while(pos!=NULL) {	
			pDest->m_deviceTypes.AddTail(src.m_deviceTypes.GetNext(pos));				
		}
	}

	if (flags & FLAG_COPY_SCENES) {
		for (int i=0;i<MAX_NR_SCENES;++i) {
			pDest->m_sceneInfo[i] = src.m_sceneInfo[i];
		}	
		copyColorDT8SceneInfo(&pDest->m_mapColorScenes,src.m_mapColorScenes);
	}
	return true;
}

void CAppDaliDeviceProperties_ControlGear::copyColorDT8SceneInfo(CMapDT8ColorScenesInfo* pDest,const CMapDT8ColorScenesInfo& src) {

	eraseColorDT8SceneInfo(pDest);

	POSITION pos = src.GetStartPosition();  
    while (pos != NULL) {
		int key = 0;   
		CAppDaliObjectDT8ColorData* pSourceObject = NULL;

        // Haal de sleutel en waarde op
        src.GetNextAssoc(pos, key, pSourceObject);
		ASSERT(NULL != pSourceObject);

		// Nieuw object aanmaken en toevoegen aan de doelmap
		if (NULL != pSourceObject) {
			CAppDaliObjectDT8ColorData* pDestColorObject = new CAppDaliObjectDT8ColorData(*pSourceObject);
			ASSERT(NULL != pDestColorObject);			
			pDest->SetAt(key, pDestColorObject);
		}
    }
	ENSURE(pDest->GetSize() == src.GetSize());
}

void CAppDaliDeviceProperties_ControlGear::eraseColorDT8SceneInfo(CMapDT8ColorScenesInfo* pDest) {
	if (NULL == pDest) {
		REQUIRE(NULL != pDest);
		return;
	}
	DUOTECNO::MFC_HELPER::DeallocCMapValues(*pDest);

/*
	POSITION pos = pDest->GetStartPosition();
	int keyValue;	
	while (pos != NULL) {
		CAppDaliObjectDT8ColorData* pData = NULL;
		pDest->GetNextAssoc(pos, keyValue, pData);
		pDest->RemoveKey(keyValue);
		if (NULL != pData) {
			delete pData;
		}
	}
	ENSURE(pDest->GetSize() == 0);
*/
}
/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
CAppDaliDeviceProperties_InputControlDevice::CAppDaliDeviceProperties_InputControlDevice( ) : 
	m_pInstances(new CAppDaliInputControlDeviceInstances()), m_bitfieldGroupInfo(0),
	m_appControllerEnabled(false), m_powerCycleNotification(false), m_deviceCapabilities(-1),
	m_randomAddress(0) {
	ASSERT(NULL != this->m_pInstances);
}

CAppDaliDeviceProperties_InputControlDevice::CAppDaliDeviceProperties_InputControlDevice(CAppDaliInputControlDeviceInstances* pInstances) :
	m_pInstances(pInstances), m_bitfieldGroupInfo(0),
	m_appControllerEnabled(false), m_powerCycleNotification(false), m_deviceCapabilities(-1),
	m_randomAddress(0) {
	ASSERT(NULL != this->m_pInstances);
}

CAppDaliDeviceProperties_InputControlDevice::~CAppDaliDeviceProperties_InputControlDevice( ) { 
	if (NULL != this->m_pInstances) {
		delete this->m_pInstances;
	}
}
/***********************************************************************************/
// Bitfield group info: 32 groepen.
void CAppDaliDeviceProperties_InputControlDevice::setGroupInfo(uint32_t groupInfo) {
	this->m_bitfieldGroupInfo = groupInfo;
}
uint32_t CAppDaliDeviceProperties_InputControlDevice::getGroupInfo( ) const {
	return this->m_bitfieldGroupInfo;
}

bool CAppDaliDeviceProperties_InputControlDevice::getAppControllerEnabled( ) const {
	return this->m_appControllerEnabled;
}
void CAppDaliDeviceProperties_InputControlDevice::setAppControllerEnabled(bool v) {
	this->m_appControllerEnabled = v;
}

bool CAppDaliDeviceProperties_InputControlDevice::getPowerCycleNotification() const {
	return this->m_powerCycleNotification;
}
void CAppDaliDeviceProperties_InputControlDevice::setPowerCycleNotification(bool v) {
	this->m_powerCycleNotification = v;
}

int CAppDaliDeviceProperties_InputControlDevice::getDeviceCapabilities( ) const {
	return this->m_deviceCapabilities;
}
void CAppDaliDeviceProperties_InputControlDevice::setDeviceCapabilities(int v) {
	// REQUIRE(v>=0 && v<=255);
	this->m_deviceCapabilities = v;
}

uint32_t CAppDaliDeviceProperties_InputControlDevice::getRandomAddress() const {
	return this->m_randomAddress;
}
void CAppDaliDeviceProperties_InputControlDevice::setRandomAddress(uint32_t v) {
	this->m_randomAddress = v;
}

CAppDaliInputControlDeviceInstances& CAppDaliDeviceProperties_InputControlDevice::getInstances() {
	ASSERT(NULL != this->m_pInstances);
	return *this->m_pInstances;
}
const CAppDaliInputControlDeviceInstances& CAppDaliDeviceProperties_InputControlDevice::getInstances() const {
	ASSERT(NULL != this->m_pInstances);
	return *this->m_pInstances;
}

bool CAppDaliDeviceProperties_InputControlDevice::isGroupMember(CAppDaliDeviceProperties_InputControlDevice& refDevice,int groupNumber) {
	if (groupNumber<0 || groupNumber>=32) {
		REQUIRE(groupNumber>=0 && groupNumber<32);
		return false;
	}
	const uint32_t mask = (1 << groupNumber);
	return (mask == (refDevice.m_bitfieldGroupInfo & mask));
}

CAppDaliDeviceProperties_InputControlDevice* CAppDaliDeviceProperties_InputControlDevice::clone( ) {
	if (NULL == this->m_pInstances) {
		ASSERT(NULL != this->m_pInstances);
		return NULL;
	}
	CAppDaliInputControlDeviceInstances* const pInstances = this->m_pInstances->clone();
	if (NULL == pInstances) {
		ASSERT(NULL != pInstances);
		return NULL;
	}

	CAppDaliDeviceProperties_InputControlDevice* const pNewInstance = new CAppDaliDeviceProperties_InputControlDevice(pInstances);
	if (NULL == pNewInstance) {
		ASSERT(NULL != pNewInstance);
		delete pInstances;
		return NULL;
	}

	// Basisklasse
	CAppDaliDeviceProperties::copy(pNewInstance,*this);

	// properties...
	pNewInstance->m_bitfieldGroupInfo = this->m_bitfieldGroupInfo;
	pNewInstance->m_appControllerEnabled = this->m_appControllerEnabled;
	pNewInstance->m_powerCycleNotification = this->m_powerCycleNotification;
	pNewInstance->m_deviceCapabilities = this->m_deviceCapabilities;
	pNewInstance->m_randomAddress = this->m_randomAddress;				
	return pNewInstance;
}

bool CAppDaliDeviceProperties_InputControlDevice::copy(CAppDaliDeviceProperties_InputControlDevice* pDest,const CAppDaliDeviceProperties_InputControlDevice& src) {
	if (NULL == pDest) {
		REQUIRE(NULL != pDest);
		return false;
	}

	// Basisklasse
	CAppDaliDeviceProperties::copy(pDest,src);

	// properties...
	pDest->m_bitfieldGroupInfo = src.m_bitfieldGroupInfo;
	pDest->m_appControllerEnabled = src.m_appControllerEnabled;
	pDest->m_powerCycleNotification = src.m_powerCycleNotification;
	pDest->m_deviceCapabilities = src.m_deviceCapabilities;
	pDest->m_randomAddress = src.m_randomAddress;	

	// Copy van de instances...
	return CAppDaliInputControlDeviceInstances::copy(&pDest->getInstances(),src.getInstances());
}
/***********************************************************************************/

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
CAppDaliInputControlDeviceInstances::CAppDaliInputControlDeviceInstances( ) : m_numberOfInstance(0), m_map( ) {  }

CAppDaliInputControlDeviceInstances::~CAppDaliInputControlDeviceInstances( ) { 	
	this->erase( );
}
int CAppDaliInputControlDeviceInstances::getNumberOfInstances() const {
	return this->m_numberOfInstance;
}
void CAppDaliInputControlDeviceInstances::setNumberOfInstances(int v) {
	this->m_numberOfInstance = v;
}

// het aantal instances in de map.
int CAppDaliInputControlDeviceInstances::getCountNumberOfInstances() const {
	return this->m_map.GetCount();
}

void CAppDaliInputControlDeviceInstances::erase( ) {
	POSITION pos = this->m_map.GetStartPosition();
	int keyValue;	
	while (pos != NULL) {
		CAppDaliInputControlDeviceInstance* pDevice = NULL;
		this->m_map.GetNextAssoc(pos, keyValue, pDevice);
		this->m_map.RemoveKey(keyValue);
		if (NULL != pDevice) {
			delete pDevice;
		}
	}
	ASSERT(this->m_map.GetSize() == 0);
}
/*	
bool CAppDaliInputControlDeviceInstances::find(int instanceNumber,CAppDaliInputControlDeviceInstance** pp) {
	if ((instanceNumber < FIRST_INSTANCE_NUMBER) || (instanceNumber > LAST_INSTANCE_NUMBER)) {
		REQUIRE(0);
		return false;
	}
	CAppDaliInputControlDeviceInstance* pDevice = NULL;
	if (FALSE == this->m_map.Lookup(instanceNumber,pDevice)) {
		return false;	// Niet aanwezig in de map.
	}
	if (NULL == pDevice) {
		return FALSE;
	}
	*pp = pDevice;
	return true;
}
*/
bool CAppDaliInputControlDeviceInstances::find(int instanceNumber,CAppDaliInputControlDeviceInstance** pp) const {
	if ((instanceNumber < FIRST_INSTANCE_NUMBER) || (instanceNumber > LAST_INSTANCE_NUMBER)) {
		REQUIRE(0);
		return false;
	}
	CAppDaliInputControlDeviceInstance* pDevice = NULL;
	if (FALSE == this->m_map.Lookup(instanceNumber,pDevice)) {
		return false;	// Niet aanwezig in de map.
	}
	if (NULL == pDevice) {
		return FALSE;
	}
	*pp = pDevice;
	return true;
}
	
bool CAppDaliInputControlDeviceInstances::add(int instanceNumber,CAppDaliInputControlDeviceInstance* p) {
	if ((instanceNumber < FIRST_INSTANCE_NUMBER) || (instanceNumber > LAST_INSTANCE_NUMBER)) {
		REQUIRE(0);
		return false;
	}
	CAppDaliInputControlDeviceInstance* pOldDevice = NULL;
	if (this->m_map.Lookup(instanceNumber,pOldDevice)) {
		if (NULL != pOldDevice) {
			return false;	// reeds aanwezig in de map.
		}
	}
	this->m_map.SetAt(instanceNumber,p);	
	return true;
}

CAppDaliInputControlDeviceInstances* CAppDaliInputControlDeviceInstances::clone( ) {
	CAppDaliInputControlDeviceInstances* const pNewInstance = new CAppDaliInputControlDeviceInstances( );
	if (NULL == pNewInstance) {
		ASSERT(NULL != pNewInstance);
		return NULL;
	}

	pNewInstance->m_numberOfInstance = this->m_numberOfInstance;

	bool cleanup = false;
	for (int instanceNumber=FIRST_INSTANCE_NUMBER;instanceNumber<=LAST_INSTANCE_NUMBER;++instanceNumber) {
		CAppDaliInputControlDeviceInstance* pDevice = NULL;
		if (this->find(instanceNumber,&pDevice)) {
			ASSERT(NULL != pDevice);
			CAppDaliInputControlDeviceInstance* const pNew = pDevice->clone();
			ASSERT(NULL!=pNew);
			if (NULL != pNew) {
				if (!pNewInstance->add(instanceNumber,pNew)) {
					ASSERT(0);
					cleanup = true;
					delete pNew;
				}
			}
		}
	}
	if (cleanup) {
		delete pNewInstance;
		return NULL;
	}
	return pNewInstance;
}

bool CAppDaliInputControlDeviceInstances::copy(CAppDaliInputControlDeviceInstances* pDest,const CAppDaliInputControlDeviceInstances& src) {
	if (NULL == pDest) {
		REQUIRE(NULL != pDest);
		return false;
	}
	if (pDest->getNumberOfInstances() != src.getNumberOfInstances()) {
		ASSERT(pDest->getNumberOfInstances() == src.getNumberOfInstances());
		return false;
	}

	bool result = true;
	for (int instanceNumber=FIRST_INSTANCE_NUMBER;instanceNumber<=LAST_INSTANCE_NUMBER;++instanceNumber) {
		CAppDaliInputControlDeviceInstance* pSrcDeviceInstance = NULL;		
		if (src.find(instanceNumber,&pSrcDeviceInstance)) {
			CAppDaliInputControlDeviceInstance* pDestDeviceInstance = NULL;
			if (pDest->find(instanceNumber,&pDestDeviceInstance)) {
				CAppDaliInputControlDeviceInstance::copy(pDestDeviceInstance,*pSrcDeviceInstance);			
			} else { result = false; }
		}
		else { result = false; }
	}
	return result;
}

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
CAppDaliInputControlDeviceInstance::CAppDaliInputControlDeviceInstance(int instanceNumber) :
	m_instanceNumber(instanceNumber), m_strName(), m_instanceActive(true), m_instanceType(-1),
	m_eventScheme(0), m_eventFilter(-1), m_eventPriority(-1), m_instanceEventResolution(-1), m_instanceStatus(-1) {
	ASSERT(instanceNumber>=0 && instanceNumber<32);
	for (int i=0;i<NR_OF_INSTANCE_GROUPS;i++) {
		m_instanceGroup[i] = -1;
	}	
} 
CAppDaliInputControlDeviceInstance::~CAppDaliInputControlDeviceInstance() { }

/*
void CAppDaliInputControlDeviceInstance::setInstanceNumber(int number) {
	this->m_instanceNumber = number;
}
*/

int CAppDaliInputControlDeviceInstance::getInstanceNumber() const {
	return this->m_instanceNumber;
}

bool CAppDaliInputControlDeviceInstance::getInstanceActive( ) const {
	return this->m_instanceActive;
}
void CAppDaliInputControlDeviceInstance::setInstanceActive(bool v) {
	this->m_instanceActive = v;
}

int CAppDaliInputControlDeviceInstance::getInstanceType( ) const {
	return this->m_instanceType;
}
void CAppDaliInputControlDeviceInstance::setInstanceType(int v) {
	this->m_instanceType = v;
}

int CAppDaliInputControlDeviceInstance::getEventScheme( ) const {
	return this->m_eventScheme;
}
void CAppDaliInputControlDeviceInstance::setEventScheme(int v) {	
	if ((v < 0) || (v > 4)) {
		ASSERT((v >= 0 && v <= 4) || (v == -1));
		return;
	}
	this->m_eventScheme = v;
}

int CAppDaliInputControlDeviceInstance::getEventFilter( ) const {
	return this->m_eventFilter;
}
void CAppDaliInputControlDeviceInstance::setEventFilter(int v) {
	this->m_eventFilter = v;
}

int CAppDaliInputControlDeviceInstance::getInstanceGroup(int idx) const {
	if (idx < 0 || idx >=NR_OF_INSTANCE_GROUPS) {
		ASSERT(idx>=0 && idx<NR_OF_INSTANCE_GROUPS);
		return -1;
	}
	return this->m_instanceGroup[idx];
}

void CAppDaliInputControlDeviceInstance::setInstanceGroup(int idx,int v) {
	if (idx < 0 || idx >=NR_OF_INSTANCE_GROUPS) {
		ASSERT(idx>=0 && idx<NR_OF_INSTANCE_GROUPS);
		return;
	}
	this->m_instanceGroup[idx] = v;
}

int CAppDaliInputControlDeviceInstance::getEventPriority( ) const {
	return this->m_eventPriority;
}
void CAppDaliInputControlDeviceInstance::setEventPriority(int v) {
	this->m_eventPriority = v;
}


int CAppDaliInputControlDeviceInstance::getEventResolution()  const {
	return this->m_instanceEventResolution;
}
void CAppDaliInputControlDeviceInstance::setEventResolution(int v) {
	this->m_instanceEventResolution = v;
}

int CAppDaliInputControlDeviceInstance::getStatus() const {
	return this->m_instanceStatus;
}
void CAppDaliInputControlDeviceInstance::setStatus(int v) {
	this->m_instanceStatus = v;
}

const CString& CAppDaliInputControlDeviceInstance::getName( ) const {
	return this->m_strName;
}

void CAppDaliInputControlDeviceInstance::setName(const CString& strName) {
	this->m_strName = strName;
}

CAppDaliInputControlDeviceInstance* CAppDaliInputControlDeviceInstance::clone( ) {
	CAppDaliInputControlDeviceInstance* const pNewInstance = new CAppDaliInputControlDeviceInstance(this->m_instanceNumber);
	if (NULL == pNewInstance) {
		ASSERT(NULL != pNewInstance);
		return NULL;	
	}
	CAppDaliInputControlDeviceInstance::copy(pNewInstance,*this);
	return pNewInstance;
}

bool CAppDaliInputControlDeviceInstance::copy(CAppDaliInputControlDeviceInstance* pDest,const CAppDaliInputControlDeviceInstance& src) {
	if (NULL == pDest) {
		REQUIRE(NULL != pDest);
		return false;
	}
	pDest->m_strName = src.m_strName;
	pDest->m_instanceActive = src.m_instanceActive;
	pDest->m_instanceType = src.m_instanceType;									
	pDest->m_eventScheme = src.m_eventScheme;										
	pDest->m_eventFilter = src.m_eventFilter;											
	pDest->m_eventPriority = src.m_eventPriority;
	pDest->m_instanceEventResolution = src.m_instanceEventResolution;
	pDest->m_instanceStatus = src.m_instanceStatus;									
	for (int i=0;i<NR_OF_INSTANCE_GROUPS;++i) {
		pDest->m_instanceGroup[i] = src.m_instanceGroup[i];
	}
	return true;
}
/***********************************************************************************/


/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
// Sedert V16.32: Namen van de groepen / sferen.
CAppDaliCollectionNames::CAppDaliCollectionNames(int maxItems) :
	m_maxItems(maxItems), m_mapNames() { }
	
CAppDaliCollectionNames::~CAppDaliCollectionNames( ) {	
}
	

void CAppDaliCollectionNames::setName(int identifier,const CString& str) {
	if (identifier >= m_maxItems) {
		return;
	}
	m_mapNames.SetAt(identifier,str);
}

const CString CAppDaliCollectionNames::getName(int identifier) const {
	CString str;
	if (FALSE == m_mapNames.Lookup(identifier,str)) {
		return CString();
	}
	return str;
}

bool CAppDaliCollectionNames::copy(CMap<int,int,CString,CString>* dest) {
	using namespace DUOTECNO::MFC_HELPER;
	if (NULL == dest) {
		REQUIRE(NULL != dest);
		return false;
	}
	
	// Copieren van de map.
	CopyCMap(this->m_mapNames, *dest);	
	return true;
}

/***********************************************************************************/
};
};
/***********************************************************************************/