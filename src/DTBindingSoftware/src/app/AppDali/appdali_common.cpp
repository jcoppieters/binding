#include "stdafx.h"
#include "appdali_common.h"
#include "appdaliprotocol_def.h"

#include <cmath>
#include <algorithm>
/***********************************************************************************/
namespace DUOTECNO {
namespace DALI {
/************************************************************************************/

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
CAppDaliFsmErrorEntry::CAppDaliFsmErrorEntry() :
	m_errorCode(ERROR_CODE_NONE), m_shortAddress(-1), m_daliDeviceType(DALI_DEVICE_TYPE_CONTROLGEAR),
	m_serialNumber(0), m_strName(), m_strErrorMsg() { }

CAppDaliFsmErrorEntry::CAppDaliFsmErrorEntry(const CAppDaliFsmErrorEntry& r) :
	m_errorCode(r.m_errorCode), m_shortAddress(r.m_shortAddress), m_daliDeviceType(r.m_daliDeviceType),
	m_serialNumber(r.m_serialNumber), m_strName(r.m_strName), m_strErrorMsg(r.m_strErrorMsg) { }	

CAppDaliFsmErrorEntry::CAppDaliFsmErrorEntry(DaliErrorCode_t errorCode, DaliDeviceType_t deviceType, int shortAddress, const CString strDescription) :
	m_errorCode(errorCode), m_shortAddress(shortAddress), m_daliDeviceType(deviceType),
	m_strErrorMsg(strDescription), m_serialNumber(0), m_strName() { }

CAppDaliFsmErrorEntry::~CAppDaliFsmErrorEntry() {	 }

DaliErrorCode_t CAppDaliFsmErrorEntry::getErrorCode() const {
	return this->m_errorCode;
}
void CAppDaliFsmErrorEntry::setErrorCode(DaliErrorCode_t code) {
	this->m_errorCode = code;
}

int CAppDaliFsmErrorEntry::getShortAddress() const {
	return this->m_shortAddress;
}
void CAppDaliFsmErrorEntry::setShortAddress(int address) {
	this->m_shortAddress = address;
}

DaliDeviceType_t CAppDaliFsmErrorEntry::getDaliDeviceType() const {
	return this->m_daliDeviceType;
}
void CAppDaliFsmErrorEntry::setDaliDeviceType(DaliDeviceType_t type) {
	this->m_daliDeviceType = type;
}

unsigned long long CAppDaliFsmErrorEntry::getSerialNumber() const {
	return this->m_serialNumber;
}
void CAppDaliFsmErrorEntry::setSerialNumber(unsigned long long serialNumber) {
	this->m_serialNumber = serialNumber;
}

const CString& CAppDaliFsmErrorEntry::getDeviceName() const {
	return this->m_strName;
}
void CAppDaliFsmErrorEntry::setDeviceName(const CString& str) {
	this->m_strName = str;
}

const CString& CAppDaliFsmErrorEntry::getStrErrorMsg() const {
	return this->m_strErrorMsg;
}
void CAppDaliFsmErrorEntry::setStrErrorMsg(const CString& str) {
	this->m_strErrorMsg = str;
}

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
CAppDaliFsmErrorEntries::CAppDaliFsmErrorEntries() : m_listEntries() { }
CAppDaliFsmErrorEntries::~CAppDaliFsmErrorEntries() {
	this->clear();
}

void CAppDaliFsmErrorEntries::clear() {
	while (!this->m_listEntries.IsEmpty()) {
		CAppDaliFsmErrorEntry* const p = m_listEntries.RemoveHead();
		if (NULL != p) {
			delete p;
		}
	}
	ASSERT(m_listEntries.GetCount() == 0);
}

bool CAppDaliFsmErrorEntries::add(const CAppDaliFsmErrorEntry& r) {
	CAppDaliFsmErrorEntry* const pNewEntry(new CAppDaliFsmErrorEntry(r));
	if (NULL == pNewEntry) {
		ASSERT(NULL != pNewEntry);
		return false;
	}
	const POSITION pos = this->m_listEntries.AddTail(pNewEntry);
	if (NULL == pos) {
		ASSERT(NULL != pos);
		delete pNewEntry;
		return false;
	}
	return true;
}

bool CAppDaliFsmErrorEntries::get(int index, CAppDaliFsmErrorEntry** ppEntry) {
	// Check of index geldig is
	if (index < 0 || index >= m_listEntries.GetCount()) {
		ASSERT(index >= 0);
		ASSERT(index<m_listEntries.GetCount());
		return FALSE;
	}
	POSITION pos = m_listEntries.GetHeadPosition();
	for (int i = 0; i < index; ++i) {
		m_listEntries.GetNext(pos); // Ga door naar het volgende item
	}
	ASSERT(NULL != pos);
	*ppEntry = m_listEntries.GetAt(pos); // Haal het item op de huidige positie
	ENSURE(NULL != (*ppEntry));
	return true;
}

int CAppDaliFsmErrorEntries::getCount() const {
	return this->m_listEntries.GetCount();
}

// Clonen van de entries...
CAppDaliFsmErrorEntries* CAppDaliFsmErrorEntries::clone() {
	CAppDaliFsmErrorEntries* const pNewList = new CAppDaliFsmErrorEntries( );
	if (NULL == pNewList) {
		ASSERT(NULL != pNewList);
		return NULL;
	}
	POSITION pos = m_listEntries.GetHeadPosition();
	while(pos!=NULL) {
		CAppDaliFsmErrorEntry* const pEntry = (CAppDaliFsmErrorEntry*) m_listEntries.GetNext(pos);					
		if (NULL != pEntry) {
			CAppDaliFsmErrorEntry* const pNewEntry = new CAppDaliFsmErrorEntry(*pEntry);
			ASSERT(NULL != pNewEntry);
			if (NULL != pNewEntry) {
				pNewList->add(*pNewEntry);
			}
		}	
	}
	ENSURE(NULL != pNewList);
	return pNewList;
}


// Utitlity functie:
// Mogen de fout entry enkel bijvoegen wanneer deze nog niet aanwezig is voor een device address...
bool CAppDaliFsmErrorEntries::addErrorEntry(CAppDaliFsmErrorEntries* pThis, const CAppDaliFsmErrorEntry& r) {
	if (NULL == pThis) {
		REQUIRE(NULL != pThis);
		return false;
	}
	POSITION pos = pThis->m_listEntries.GetHeadPosition();
	while (pos != NULL) {
		CAppDaliFsmErrorEntry * const pEntry = (CAppDaliFsmErrorEntry*)pThis->m_listEntries.GetNext(pos);
		if (NULL != pEntry) {
			if (pEntry->getDaliDeviceType() == r.getDaliDeviceType() &&
				pEntry->getShortAddress() == r.getShortAddress() &&
				pEntry->getErrorCode() == r.getErrorCode()) {
				// Er zijn reeds identieke entries.
				return false;
			}		
		}
	}
	return pThis->add(r);
}

bool CAppDaliFsmErrorEntries::removeErrorEntry(CAppDaliFsmErrorEntries* pThis, DaliDeviceType_t deviceType, int deviceShortAddress) { // DaliErrorCode_t errorCode) {
	if (NULL == pThis) {
		return false;
	}

	bool result = false;
	POSITION pos1 = pThis->m_listEntries.GetHeadPosition();
	while (pos1 != NULL) {
		POSITION pos2 = pos1;
		CAppDaliFsmErrorEntry* pEntry = (CAppDaliFsmErrorEntry*)pThis->m_listEntries.GetNext(pos1);
		if (NULL != pEntry) {
			if ((pEntry->getShortAddress() == deviceShortAddress) && (pEntry->getDaliDeviceType() == deviceType)) {
				CAppDaliFsmErrorEntry* pEntryToRemove = pThis->m_listEntries.GetAt(pos2);			
				if (NULL != pEntryToRemove) {
					pThis->m_listEntries.RemoveAt(pos2);
					delete pEntryToRemove;
					pEntryToRemove = NULL;
				}
				result = true;
				break;
			}
		}
	}
	return result;
}
/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
CAppDaliFsmContext::CAppDaliFsmContext() : m_task(APPDALI_FSM_TASK_NONE), m_state(TASK_SEARCH_DEVICES_STATE_INIT),
	m_shortAddress(0), m_stateCounter(0), // m_supportRequestDT8ColorObject(false), 
	m_listAssignedControlGear(), m_listAssignedInputDevices(), m_listErrors() { }

CAppDaliFsmContext::~CAppDaliFsmContext() {
}

void CAppDaliFsmContext::clearStats() {
	m_listErrors.clear();
	m_listAssignedControlGear.RemoveAll();
	m_listAssignedInputDevices.RemoveAll();
}

void CAppDaliFsmContext::addAssigned(DaliDeviceType_t type, int shortAddress) {
	CList<int>* pList = NULL;
	if (DALI_DEVICE_TYPE_CONTROLGEAR == type) {
		pList = &this->m_listAssignedControlGear;
	}
	else if (DALI_DEVICE_TYPE_INPUTCONTROL == type) {
		pList = &this->m_listAssignedInputDevices;
	}
	else {
		ASSERT(0);
		return;
	}
	if (NULL == pList) {
		ASSERT(NULL != pList);
		return;
	}
	if (NULL != pList->Find(shortAddress)) {
		return;
	}
	pList->AddTail(shortAddress);
}

// Oppassen:
// Gebruiken de utility functie addErrorEntry().
// Deze zorgt ervoor dat er geen identieke error entry berichten ingevoegd worden.
void CAppDaliFsmContext::addError(const CAppDaliFsmErrorEntry& entry) {
	CAppDaliFsmErrorEntries::addErrorEntry(&this->m_listErrors, entry);
}

CAppDaliFsmErrorEntries* CAppDaliFsmContext::getCopyFsmErrors() {
	return m_listErrors.clone();
}
/***********************************************************************************/

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
CAppDaliObjectDT8ColorData::CAppDaliObjectDT8ColorData( ) : m_type(-1) { 
	memset(&this->values,-1,sizeof(this->values));
}

void CAppDaliObjectDT8ColorData::clear( ) {
	this->m_type = -1;
	memset(&this->values,-1,sizeof(this->values));
}

void CAppDaliObjectDT8ColorData::setColorType(int colorType,bool init) {
	this->m_type = colorType;
	if (init) {	
		switch(this->m_type) {
			case DALI_DT8_COLOUR_TYPE_XY_COORDINATE: {
				memset(&this->values.colorValueXY,-1,sizeof(this->values.colorValueXY));								 
			} break;
			case DALI_DT8_COLOUR_TYPE_TEMPERATURE_TC: {
				memset(&this->colorTempValues,-1,sizeof(this->colorTempValues));								 
			} break;
			case DALI_DT8_COLOUR_TYPE_PRIMARY_N: {
				memset(&this->values.colorValuePrimary,-1,sizeof(this->values.colorValuePrimary));					 
			} break;
			case DALI_DT8_COLOUR_TYPE_RGBWAF: {
				memset(&this->values.colorValueRGBWAF,-1,sizeof(this->values.colorValueRGBWAF));									  
			} break;
			case 0xff: { /* Absorbe */ } break;
			default: {
				ASSERT(0); 
			} break;
		}
	}
}

int CAppDaliObjectDT8ColorData::getColorType( ) const {
	return this->m_type;
}

// Color type Coordinate XY
void CAppDaliObjectDT8ColorData::setCoordinateX(int v) {
	this->values.colorValueXY.x = v;
}
int CAppDaliObjectDT8ColorData::getCoordinateX( ) const {
	return this->values.colorValueXY.x;
}

void CAppDaliObjectDT8ColorData::setCoordinateY(int v) {
	this->values.colorValueXY.y = v;
}
int CAppDaliObjectDT8ColorData::getCoordinateY() const {
	return this->values.colorValueXY.y;
}

// Color type Tuneable TC
void CAppDaliObjectDT8ColorData::setColorTemperature(int v) {
	this->colorTempValues.value = v;
}
int CAppDaliObjectDT8ColorData::getColorTemperature( ) const {
	return this->colorTempValues.value;
}

// Color Type RGBWAF
void CAppDaliObjectDT8ColorData::setColorDimLevelRed(int v) {
	this->values.colorValueRGBWAF.red = v;
}
int CAppDaliObjectDT8ColorData::getColorDimLevelRed( ) const {
	return this->values.colorValueRGBWAF.red;
}

void CAppDaliObjectDT8ColorData::setColorDimLevelGreen(int v) {
	this->values.colorValueRGBWAF.green = v;
}
int CAppDaliObjectDT8ColorData::getColorDimLevelGreen( ) const {
	return this->values.colorValueRGBWAF.green;
}

void CAppDaliObjectDT8ColorData::setColorDimLevelBlue(int v) {
	this->values.colorValueRGBWAF.blue = v;
}
int CAppDaliObjectDT8ColorData::getColorDimLevelBlue( ) const {
	return this->values.colorValueRGBWAF.blue;
}

void CAppDaliObjectDT8ColorData::setColorDimLevelWhite(int v) {
	this->values.colorValueRGBWAF.white = v;
}
int CAppDaliObjectDT8ColorData::getColorDimLevelWhite( ) const {
	return this->values.colorValueRGBWAF.white;
}

void CAppDaliObjectDT8ColorData::setColorDimLevelAmber(int v) {
	this->values.colorValueRGBWAF.amber = v;
}
int CAppDaliObjectDT8ColorData::getColorDimLevelAmber( ) const {
	return this->values.colorValueRGBWAF.amber;
}

void CAppDaliObjectDT8ColorData::setColorDimLevelFreeColor(int v) {
	this->values.colorValueRGBWAF.freeColor = v;
}
int CAppDaliObjectDT8ColorData::getColorDimLevelFreeColor( ) const {
	return this->values.colorValueRGBWAF.freeColor;
}

void CAppDaliObjectDT8ColorData::setColorDimLevelChannel(int idx,int v) {
	if (idx<0 || idx>=MAX_PRIMARY_CHANNELS) {
		ASSERT(0);
		return;
	} 	
	this->values.colorValuePrimary.values[idx] = v;
}
	
int CAppDaliObjectDT8ColorData::getColorDimLevelChannel(int idx) const {
	if (idx<0 || idx>=MAX_PRIMARY_CHANNELS) {
		ASSERT(0);
		return -1;
	} 	
	return this->values.colorValuePrimary.values[idx];
}

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
// Bepalen of een color type geldig is.
bool CAppDaliObjectDT8ColorData_Utils::isValidColorType(const CAppDaliObjectDT8ColorData& ref) {
	const int colorType = ref.getColorType();
	if (colorType < 0 || colorType >= 255) {
		return false;
	}
	static const int mask = (DALI_DT8_COLOUR_TYPE_XY_COORDINATE + DALI_DT8_COLOUR_TYPE_TEMPERATURE_TC + DALI_DT8_COLOUR_TYPE_PRIMARY_N + DALI_DT8_COLOUR_TYPE_RGBWAF);
	if (0 != (colorType & mask)) {
		return true;
	}
	return false;
}

// Bepalen van het type color die actief is.
bool CAppDaliObjectDT8ColorData_Utils::isTypeColorTemperature(const CAppDaliObjectDT8ColorData& ref) {
	return (DALI_DT8_COLOUR_TYPE_TEMPERATURE_TC == (ref.getColorType() & DALI_DT8_COLOUR_TYPE_TEMPERATURE_TC));
}
bool CAppDaliObjectDT8ColorData_Utils::isTypeColorCoordinateXY(const CAppDaliObjectDT8ColorData& ref) {
	return (DALI_DT8_COLOUR_TYPE_XY_COORDINATE == (ref.getColorType() & DALI_DT8_COLOUR_TYPE_XY_COORDINATE));
}
bool CAppDaliObjectDT8ColorData_Utils::isTypeColorRGBWAF(const CAppDaliObjectDT8ColorData& ref) {
	return (DALI_DT8_COLOUR_TYPE_RGBWAF == (ref.getColorType() & DALI_DT8_COLOUR_TYPE_RGBWAF));
}
bool CAppDaliObjectDT8ColorData_Utils::isTypeColorPrimaryChannelN(const CAppDaliObjectDT8ColorData& ref) {
	return (DALI_DT8_COLOUR_TYPE_PRIMARY_N == (ref.getColorType() & DALI_DT8_COLOUR_TYPE_PRIMARY_N));
}

bool CAppDaliObjectDT8ColorData_Utils::isActive(const CAppDaliObjectDT8ColorData& ref) {
	if (!isValidColorType(ref)) {
		return false;
	}
	return (isActiveTypeColorTemperature(ref) || isActiveTypeColorCoordinateXY(ref) || isActiveTypeColorRGBWAF(ref) || isActiveTypeColorPrimaryChannelN(ref));		
}

bool CAppDaliObjectDT8ColorData_Utils::isActiveTypeColorTemperature(const CAppDaliObjectDT8ColorData& ref) {
	if (!isTypeColorTemperature(ref)) {
		return false;
	}
	const int temperature = ref.getColorTemperature();
	return (((temperature<0) || (temperature>=CAppDaliObjectDT8ColorData::MASK_VALUE_SHORT)) ? false : true);
}

bool CAppDaliObjectDT8ColorData_Utils::isActiveTypeColorCoordinateXY(const CAppDaliObjectDT8ColorData& ref) {		
	if (!isTypeColorCoordinateXY(ref)) {
		return false;
	}
	const int x = ref.getCoordinateX();
	const int y = ref.getCoordinateY();
	return (((x<0) || (x>=CAppDaliObjectDT8ColorData::MASK_VALUE_SHORT) || (y<0) || (y>=CAppDaliObjectDT8ColorData::MASK_VALUE_SHORT)) ? false : true);
}

bool CAppDaliObjectDT8ColorData_Utils::isActiveTypeColorRGBWAF(const CAppDaliObjectDT8ColorData& ref) {
	if (!isTypeColorRGBWAF(ref)) {
		return false;
	}
	const int red = ref.getColorDimLevelRed();
	const int green = ref.getColorDimLevelGreen();
	const int blue = ref.getColorDimLevelBlue();

	if ((red >= 0) || (red < CAppDaliObjectDT8ColorData::MASK_VALUE_BYTE) || (green >= 0) || (green < CAppDaliObjectDT8ColorData::MASK_VALUE_BYTE) || (blue >= 0) || (blue < CAppDaliObjectDT8ColorData::MASK_VALUE_BYTE)) {
		return true;
	}

	const int white = ref.getColorDimLevelWhite();
	const int amber = ref.getColorDimLevelAmber();
	const int freeColor = ref.getColorDimLevelFreeColor();

	if ((white >= 0) || (white < CAppDaliObjectDT8ColorData::MASK_VALUE_BYTE) || (amber >= 0) || (amber < CAppDaliObjectDT8ColorData::MASK_VALUE_BYTE) || (freeColor >= 0) || (freeColor < CAppDaliObjectDT8ColorData::MASK_VALUE_BYTE)) {
		return true;
	}
	return false;
}

bool CAppDaliObjectDT8ColorData_Utils::isActiveTypeColorPrimaryChannelN(const CAppDaliObjectDT8ColorData& ref) {
	if (!isTypeColorPrimaryChannelN(ref)) {
		return false;
	}
	for (int i=0;i<CAppDaliObjectDT8ColorData::MAX_PRIMARY_CHANNELS;++i) {
		const int value = ref.getColorDimLevelChannel(i);
		if (value >= 0 || value < CAppDaliObjectDT8ColorData::MASK_VALUE_BYTE) {
			return true;
		}
	}
	return false;
}
/***********************************************************************************/

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
const CAppDaliObjectDT8ColorSpace_Utils::DaliColorSpace_RGB CAppDaliObjectDT8ColorSpace_Utils::RGB_COLOR_WHITE = { 254, 254, 254 };
const CAppDaliObjectDT8ColorSpace_Utils::DaliColorSpace_RGB CAppDaliObjectDT8ColorSpace_Utils::RGB_COLOR_AMBER = { 254, 191, 0 };

const CAppDaliObjectDT8ColorSpace_Utils::ColorTableEntry CAppDaliObjectDT8ColorSpace_Utils::m_colorTable[] = {
	{ "Red",        254,   0,   0,   0.640, 0.330 },
	{ "Green",        0, 254,   0,   0.300, 0.600 },
	{ "Blue",         0,   0, 254,   0.150, 0.060 },
	{ "White",      254, 254, 254,   0.3127, 0.3290 },
	{ "Black",        0,   0,   0,   0.3130, 0.3290 },
	{ "Yellow",     254, 254,   0,   0.419, 0.505 },
	{ "Cyan",         0, 254, 254,   0.224, 0.328 },
	{ "Magenta",    254,   0, 254,   0.320, 0.154 },
	{ "Orange",     254, 165,   0,   0.561, 0.419 },
	{ "Pink",       254, 192, 203,   0.345, 0.321 },
	{ "Light Blue", 173, 216, 230,   0.278, 0.312 },
	{ "Lime",       191, 254,   0,   0.376, 0.602 },
	{ "Purple",     128,   0, 128,   0.320, 0.160 },
	{ "Teal",         0, 128, 128,   0.200, 0.300 },
	{ "Olive",      128, 128,   0,   0.410, 0.500 },
	{ "Navy",         0,   0, 128,   0.145, 0.060 },
	{ "Maroon",     128,   0,   0,   0.639, 0.330 },
	{ "Silver",     192, 192, 192,   0.3127, 0.330 },
	{ "Gray",       128, 128, 128,   0.3126, 0.3290 },
	{ "Brown",      165,  42,  42,   0.537, 0.340 },
	{ "Gold",       254, 215,   0,   0.490, 0.469 },
	{ "Beige",      245, 245, 220,   0.348, 0.380 },
	{ "Khaki",      240, 230, 140,   0.406, 0.465 },
	{ "Coral",      254, 127,  80,   0.531, 0.370 },
	{ "Salmon",     250, 128, 114,   0.512, 0.344 },
	{ "Tomato",     254,  99,  71,   0.557, 0.338 },
	{ "Crimson",    220,  20,  60,   0.590, 0.310 },
	{ "Indigo",      75,   0, 130,   0.180, 0.080 },
	{ "Violet",     238, 130, 238,   0.370, 0.240 },
	{ "Turquoise",   64, 224, 208,   0.220, 0.340 },
	{ "Aquamarine", 127, 254, 212,   0.240, 0.350 },
	{ "Azure",       70, 130, 180,   0.210, 0.260 },
	{ "Sky Blue",   135, 206, 235,   0.270, 0.310 },
	{ "Dodger Blue", 30, 144, 254,   0.160, 0.190 },
	{ "Royal Blue",  65, 105, 225,   0.160, 0.140 },
	{ "Deep Sky Blue", 0, 191, 254,  0.180, 0.220 },
	{ "Midnight Blue", 25,  25, 112, 0.150, 0.070 },
	{ "Dark Green",   0, 100,   0,   0.280, 0.570 },
	{ "Forest Green",34, 139,  34,   0.290, 0.580 },
	{ "Lawn Green", 124, 252,   0,   0.350, 0.620 },
	{ "Spring Green",0, 254, 127,    0.250, 0.480 },
	{ "Mint Cream", 245, 254, 250,   0.310, 0.330 },
	{ "Sea Green",  46, 139,  87,    0.250, 0.440 },
	{ "Pale Green", 152, 251, 152,   0.330, 0.530 },
	{ "Chartreuse",127, 254,   0,    0.370, 0.610 },
	{ "Medium Sea Green", 60, 179, 113, 0.270, 0.500 },
	{ "Dark Khaki", 189, 183, 107,   0.400, 0.470 },
	{ "Sandy Brown",244, 164,  96,   0.480, 0.400 },
	{ "Peru",       205, 133,  63,   0.490, 0.380 },
	{ "Chocolate",  210, 105,  30,   0.530, 0.360 },
	{ "Sienna",     160,  82,  45,   0.540, 0.340 },
	{ "Rosy Brown", 188, 143, 143,   0.360, 0.360 },
	{ "Orchid",     218, 112, 214,   0.360, 0.260 },
	{ "Plum",       221, 160, 221,   0.330, 0.300 }
};
/***********************************************************************************/

int CAppDaliObjectDT8ColorSpace_Utils::getColorTableNrOfEntries() { // const {
	return sizeof(m_colorTable) / sizeof(ColorTableEntry);
}

struct CAppDaliObjectDT8ColorSpace_Utils::DaliColorSpace_XY CAppDaliObjectDT8ColorSpace_Utils::convert_rgbToXy(double red, double green, double blue) {
	// Normaliseer RGB waarden naar 0.0 - 1.0
	red = red / 255.0;
	green = green / 255.0;
	blue = blue / 255.0;

	// Gamma correctie (sRGB)
	red = (red > 0.04045) ? pow((red + 0.055) / 1.055, 2.4) : (red / 12.92);
	green = (green > 0.04045) ? pow((green + 0.055) / 1.055, 2.4) : (green / 12.92);
	blue = (blue > 0.04045) ? pow((blue + 0.055) / 1.055, 2.4) : (blue / 12.92);

	// Omzetten naar XYZ ruimte
	double X = red * 0.4124 + green * 0.3576 + blue * 0.1805;
	double Y = red * 0.2126 + green * 0.7152 + blue * 0.0722;
	double Z = red * 0.0193 + green * 0.1192 + blue * 0.9505;

	// Bereken de xy waarden
	double sum = X + Y + Z;
	DaliColorSpace_XY xy = { 0, 0 };

	if (sum != 0) {
		xy.x = X / sum;
		xy.y = Y / sum;
	}
	return xy;
}

// Clamp functie om waardes af te snijden tussen 0 en 255
uint8_t CAppDaliObjectDT8ColorSpace_Utils::clamp(double value) {
	if (value < 0.0) return 0;
    if (value > 255.0) return 255;
    return static_cast<uint8_t>(value);	
}

double CAppDaliObjectDT8ColorSpace_Utils::gammaCorrect(double channel) {
    if (channel <= 0.0031308)
        return 12.92 * channel;
    else
        return 1.055 * std::pow(channel, 1.0 / 2.4) - 0.055;
}

CAppDaliObjectDT8ColorSpace_Utils::DaliColorSpace_RGB CAppDaliObjectDT8ColorSpace_Utils::convert_xyToRGB(double x, double y, double brightness) {

	// Mask: Indien de waarden < 0
	if (x < 0 && y < 0) {
		DaliColorSpace_RGB result;
		result.r = 255;
		result.g = 255;
		result.b = 255;
		return result;
	}

	if (y == 0) y = 0.00001; // voorkomen van deling door nul

    // 1. Rekonstrueer XYZ
    double Y = brightness;
    double X = (Y / y) * x;
    double Z = (Y / y) * (1.0 - x - y);

    // 2. XYZ naar lineaire RGB
    double r = 3.2406 * X - 1.5372 * Y - 0.4986 * Z;
    double g = -0.9689 * X + 1.8758 * Y + 0.0415 * Z;
    double b = 0.0557 * X - 0.2040 * Y + 1.0570 * Z;

    // 3. Gamma correctie (zonder lambda)
    r = gammaCorrect(r);
    g = gammaCorrect(g);
    b = gammaCorrect(b);

    // 4. Schalen naar 0–255 en clampen
    DaliColorSpace_RGB result;
    result.r = clamp(r * 255.0);
    result.g = clamp(g * 255.0);
    result.b = clamp(b * 255.0);
    return result;
}


CAppDaliObjectDT8ColorSpace_Utils::DaliColorSpace_RGB CAppDaliObjectDT8ColorSpace_Utils::normalize(DaliColorSpace_RGB& r) {
	if (r.r < 0 || r.r >= 255) {
		r.r = 0;	
	}
	if (r.g < 0 || r.g >= 255) {
		r.g = 0;	
	}
	if (r.b < 0 || r.b >= 255) {
		r.b = 0;	
	}
	return r;
}


// Meng één kleurcomponent met een extra bijdrage
uint8_t CAppDaliObjectDT8ColorSpace_Utils::BlendChannel(uint8_t base, uint8_t blendValue, uint8_t target) {
    return (uint8_t)(base + ((target - base) * blendValue) / 254);
}

// RGB Kleuren aanpassen met een witte component.
// Het extra wit zorgt ervoor dat de kleuren helder worden.
// Oppassen: Waarde tussen 0 & 254, 255 is mask not used.
CAppDaliObjectDT8ColorSpace_Utils::DaliColorSpace_RGB CAppDaliObjectDT8ColorSpace_Utils::applyWhite(DaliColorSpace_RGB& r,int white) {
	if (white < 0 || white >= 255) {
		return r;
	}
	DaliColorSpace_RGB rgb = normalize(r);
	rgb.r = BlendChannel(rgb.r, white, RGB_COLOR_WHITE.r);
	rgb.g = BlendChannel(rgb.g, white, RGB_COLOR_WHITE.g);
	rgb.b = BlendChannel(rgb.b, white, RGB_COLOR_WHITE.r);
    return rgb;
}

CAppDaliObjectDT8ColorSpace_Utils::DaliColorSpace_RGB CAppDaliObjectDT8ColorSpace_Utils::applyWhiteAmber(DaliColorSpace_RGB& r,int white,int amber) {
	DaliColorSpace_RGB rgb = normalize(r);
	if (white > 0 && white <255) {
		// Eerst RGB mengen met wit
		rgb.r = BlendChannel(rgb.r, white, RGB_COLOR_WHITE.r);
		rgb.g = BlendChannel(rgb.g, white, RGB_COLOR_WHITE.g);
		rgb.b = BlendChannel(rgb.b, white, RGB_COLOR_WHITE.b);
	}
	if (amber > 0 && amber < 255) {
		// RGB mengen met amber
		rgb.r = BlendChannel(rgb.r, amber, RGB_COLOR_AMBER.r);
		rgb.g = BlendChannel(rgb.g, amber, RGB_COLOR_AMBER.g);
		rgb.b = BlendChannel(rgb.b, amber, RGB_COLOR_AMBER.b);	
	}
	return rgb;
}	

int CAppDaliObjectDT8ColorSpace_Utils::convertFromKelvin2Mirek(int value) {
	if (value >= DALI_SYSTEM_VALUE_TEMPERATURE_TC_KELVIN_MASK) {
		return DALI_SYSTEM_VALUE_TEMPERATURE_TC_MIREK_MASK;
	}
	if (value < DALI_SYSTEM_VALUE_TEMPERATURE_TC_KELVIN_MIN) {
		value = DALI_SYSTEM_VALUE_TEMPERATURE_TC_KELVIN_MIN;
	}
	if (value > DALI_SYSTEM_VALUE_TEMPERATURE_TC_KELVIN_MAX) {
		value = DALI_SYSTEM_VALUE_TEMPERATURE_TC_KELVIN_MAX;
	}
	return (_DALI_SYSTEM_ONE_MILLION / value);
}

int CAppDaliObjectDT8ColorSpace_Utils::convertFromMirek2Kelvin(int value) {
	if (value >= DALI_SYSTEM_VALUE_TEMPERATURE_TC_MIREK_MASK) {
		return DALI_SYSTEM_VALUE_TEMPERATURE_TC_KELVIN_MASK;
	}
	if (value < DALI_SYSTEM_VALUE_TEMPERATURE_TC_MIREK_MIN) {
		return DALI_SYSTEM_VALUE_TEMPERATURE_TC_KELVIN_MASK;
	}
	if (value > DALI_SYSTEM_VALUE_TEMPERATURE_TC_MIREK_MAX) {
		value = DALI_SYSTEM_VALUE_TEMPERATURE_TC_MIREK_MAX;
	}	
	return (_DALI_SYSTEM_ONE_MILLION / value);
}

double CAppDaliObjectDT8ColorSpace_Utils::convertToXYValue(int value) {
	if (value < 0 || value > 0xFFFE) {
        return -1.0; // Ongeldige input
    }
    return static_cast<double>(value) / 65534.0;
}


static int myRound(double x) {
    return static_cast<int>(x >= 0 ? x + 0.5 : x - 0.5);
}

int CAppDaliObjectDT8ColorSpace_Utils::convertFromXYValue(double value) {
	if (value < 0.0 || value > 1.0) {
        return 0xFFFF; // Ongeldige output (gebruikt als foutcode)
    }
    return static_cast<int>(myRound(value * 65534.0));
}

/***********************************************************************************/
};
};
/***********************************************************************************/