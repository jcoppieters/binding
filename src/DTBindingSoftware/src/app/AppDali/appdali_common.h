#pragma once
#include "appdali_fwd.h"
/***********************************************************************************/
namespace DUOTECNO {
namespace DALI {
/************************************************************************************/

// Oppassen: Fuctie pointers volgens de index van de toestand.
	typedef enum {

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Search devices FSM states - Begin.
		TASK_SEARCH_DEVICES_STATE_INIT = 0,
		TASK_COMMISSIONING_STATE_INIT = TASK_SEARCH_DEVICES_STATE_INIT,
		TASK_SEARCH_DEVICES_STATE_SEARCH_DEVICES,											    // Initialise + Quiecsnet mode.
		TASK_SEARCH_DEVICES_STATE_CONTROLGEAR_SEARCH_BROADCAST,									// Broadcast zoeken naar control gear
		TASK_SEARCH_DEVICES_STATE_CONTROLGEAR_SEARCH_DEVICE,									// search control gear device.
		TASK_SEARCH_DEVICES_STATE_CONTROLGEAR_REQUEST_PROPERTIES,
		TASK_SEARCH_DEVICES_STATE_CONTROLGEAR_READ_MEMORYMAP,
		TASK_SEARCH_DEVICES_STATE_CONTROLGEAR_QUERY_GROUPS,
		TASK_SEARCH_DEVICES_STATE_CONTROLGEAR_QUERY_SCENELEVELS,
		TASK_SEARCH_DEVICES_STATE_CONTROLGEAR_WAIT_DONE,
		TASK_SEARCH_DEVICES_STATE_CONTROLDEVICES_SEARCH_BROADCAST,
		TASK_SEARCH_DEVICES_STATE_CONTROLDEVICES_SEARCH_DEVICE,
		TASK_SEARCH_DEVICES_STATE_CONTROLDEVICES_REQUEST_PROPERTIES,
		TASK_SEARCH_DEVICES_STATE_CONTROLDEVICES_READ_MEMORYMAP,
		TASK_SEARCH_DEVICES_STATE_CONTROLDEVICES_REQUEST_INSTANCES,
		TASK_SEARCH_DEVICES_STATE_CONTROLDEVICES_WAIT_DONE,
		TASK_COMMISSIONING_STATE_CONTROLGEAR_ERASE,
		TASK_COMMISSIONING_STATE_CONTROLGEAR_SEARCH_UNADDRESSED,
		TASK_COMMISSIONING_STATE_CONTROLGEAR_ADDRESS,
		TASK_COMMISSIONING_STATE_CONTROLDEVICES_ERASE,
		TASK_COMMISSIONING_STATE_CONTROLDEVICES_SEARCH_UNADDRESSED,
		TASK_COMMISSIONING_STATE_CONTROLDEVICES_ADDRESS,
		TASK_COMMISSIONING_STATE_CONTROLDEVICES_INSTANCES_AUTOCFG,
		TASK_ADDRESSING_SINGLE_DEVICE,
		TASK_ADDRESSING_DEVICES_SWAPADDRESSES,
		TASK_CONFIG_CONTROLGEAR_PROPERTIES,
		TASK_CONFIG_CONTROLGEAR_SCENES,
		TASK_CONFIG_CONTROLGEAR_SINGLE_SCENE,
		TASK_CONFIG_CONTROLGEAR_DIMMERCURVE,
		TASK_CONFIG_CONTROLDEVICE_INSTANCE_ENABLE_ONOFF,
		TASK_CONFIG_CONTROLDEVICE_INSTANCES,
		TASK_CONTROLGEAR_QUERY_ACTUAL_VALUE,
		TASK_COMMISSIONING_STATES_MAX
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	} State_t;

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/

// Principe:
//  - Bij een fout zal de fsm/helper een FSM Error entry aanmaken en doorgeven aan de ctx.
//    De FSM blijft verder doen 
//  - Wanneer het algoritme gedaan is worden de fouten nagekeken + weergeven in een property page.
//	  *) PropertyPage 1:
//		 Overzicht met de fouten.
//		 Mogelijke oplossingen / opnieuw zoeken naar de devices ??
//	  *) PropertyPage 2:
//		 Detail: ListView - Address, Naam, Serie nummer, Probleem in een kort string.
//
//	  Opm:
//		- Property sheet moet modeless gemaakt worden + zichtbaar blijven tijdens het oplossen van de fouten.
//		- Bij fouten mogen de wijzigen niet opgeslagen worden (evt vragen aan de gebruiker.)
//		- System bus fouten kunnen niet hersteld worden (1 property page is voldoende.)

class CAppDaliFsmErrorEntry {
public:
	CAppDaliFsmErrorEntry();
	CAppDaliFsmErrorEntry(const CAppDaliFsmErrorEntry& r);
	CAppDaliFsmErrorEntry(DaliErrorCode_t errorCode, DaliDeviceType_t deviceType, int shortAddress, const CString strDescription = CString());
	~CAppDaliFsmErrorEntry();

	DaliErrorCode_t getErrorCode() const;
	void setErrorCode(DaliErrorCode_t code);

	int getShortAddress() const;
	void setShortAddress(int address);

	DaliDeviceType_t getDaliDeviceType() const;
	void setDaliDeviceType(DaliDeviceType_t type);

	unsigned long long  getSerialNumber() const;
	void setSerialNumber(unsigned long long serialNumber);

	const CString& getDeviceName() const;
	void setDeviceName(const CString& str);

	const CString& getStrErrorMsg() const;
	void setStrErrorMsg(const CString& str);

	// static CString toShortString(DaliFsmErrorCode_t code);		//( Weergave in de list view (Not found, Replaced, ...)

private:
	DaliErrorCode_t m_errorCode;					// Invullen door de FSM (algoritmes)
	int m_shortAddress;								// Invullen door de FSM (algoritmes)
	DaliDeviceType_t m_daliDeviceType;				// Invullen door de FSM (algoritmes)
	unsigned long long m_serialNumber;				// Database info (enkel wanneer er een offline database is?)
	CString m_strName;								// Database info (enkel wanneer er een offline database is?)
	CString m_strErrorMsg;							// Uitgebreide error info.
};



// Klasse CAppDaliFsmErrorEntries
// --> Deze wordt bijgehouden in de FSM context.
// --> Copy wordt bijgehouden in de DALI subsysteem (zodat de modeless dialoog kan blijven bestaan)
class CAppDaliFsmErrorEntries {
public:
	CAppDaliFsmErrorEntries();
	virtual ~CAppDaliFsmErrorEntries();

	void clear();
	bool get(int index, CAppDaliFsmErrorEntry** ppEntry);
	int getCount() const;

	// Clonen van de entries...
	CAppDaliFsmErrorEntries* clone();

	// Utility functie:
	// Mogen de fout entry enkel bijvoegen wanneer deze nog niet aanwezig is voor een device address...
	static bool addErrorEntry(CAppDaliFsmErrorEntries* pEntries,const CAppDaliFsmErrorEntry& r);

	// Utility functie.
	// Verwijderen van de entry uit de lijst.
	static bool removeErrorEntry(CAppDaliFsmErrorEntries* pEntries, DaliDeviceType_t deviceType, int deviceShortAddress); //  , DaliErrorCode_t errorCode);

protected:
	bool add(const CAppDaliFsmErrorEntry& r);

private:
	CList<CAppDaliFsmErrorEntry* > m_listEntries;
};

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
class CAppDaliFsmContext {
public:
	CAppDaliFsmContext();
	~CAppDaliFsmContext();

	void clearStats();
	int getShortAddress() const {
		return this->m_shortAddress;
	}
	State_t getState() const {
		return this->m_state;
	}
	Task_t getTask() const {
		return this->m_task;
	}
	void addAssigned(DaliDeviceType_t type, int shortAddress);
	void addError(const CAppDaliFsmErrorEntry& entry);

	// Een copy/clone van de lijst met fouten.
	// De client moet zelf de fouten dealloceren wanneer niet meer nodig zijn.
	CAppDaliFsmErrorEntries* getCopyFsmErrors();

private:
	Task_t m_task;
	State_t m_state;
	int m_shortAddress;
	int m_stateCounter;
	CList<int> m_listAssignedControlGear;					//	Lijst van addressen van control gear.
	CList<int> m_listAssignedInputDevices;					//	Lijst van addressen van control input devices.
	CAppDaliFsmErrorEntries m_listErrors;

	friend class CAppDaliFsm;
};
/***********************************************************************************/

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
class CAppDaliObjectDT8ColorData {
public:
	CAppDaliObjectDT8ColorData( );

	enum {
		MASK_VALUE_BYTE	= 0xFF,
		MASK_VALUE_SHORT = 0xFFFF
	};

	void clear( );

	void setColorType(int colorType,bool init = true);
	int getColorType( ) const;

	// Color type Coordinate XY
	void setCoordinateX(int v);
	int getCoordinateX() const;

	void setCoordinateY(int v);
	int getCoordinateY() const;

	// Color type Tuneable TC
	void setColorTemperature(int v);
	int getColorTemperature( ) const;

	// Color Type RGBWAF
	void setColorDimLevelRed(int v);
	int getColorDimLevelRed() const;

	void setColorDimLevelGreen(int v);
	int getColorDimLevelGreen() const;

	void setColorDimLevelBlue(int v);
	int getColorDimLevelBlue() const;

	void setColorDimLevelWhite(int v);
	int getColorDimLevelWhite() const;

	void setColorDimLevelAmber(int v);
	int getColorDimLevelAmber() const;

	void setColorDimLevelFreeColor(int v);
	int getColorDimLevelFreeColor() const;

	enum { MAX_PRIMARY_CHANNELS = 6 };
	void setColorDimLevelChannel(int idx,int v);
	int getColorDimLevelChannel(int idx) const;

private:

	// Color temperature Tuneable white.
	struct ColorTempValueTC {
		int value;
	};

	// Color data voor color type XY.
	struct ColorValueXY {
		int x;
		int y;
	};

	// Color data voor color type Tuneable white.
	struct ColorValuePrimaryNChannels {
		int values[MAX_PRIMARY_CHANNELS];
	};

	// Color data voor RGBWAF.
	struct ColorValueRGBWAF {
		int red;
		int green;
		int blue;
		int white;
		int amber;
		int freeColor;
	};

	union ColorValues {
		struct ColorValueXY colorValueXY;	
		struct ColorValueRGBWAF colorValueRGBWAF;
		struct ColorValuePrimaryNChannels colorValuePrimary;
	};

	// Oppassen:
	// DT8 Objecten kunnen gecombineerd worden als RGB+TC, XY+TC,...
	int m_type;												// Std op -1
	ColorValues values;										// Kleuren RGBWAF, X-Y en PrimaryN
	struct ColorTempValueTC colorTempValues;				// Kleur temp TC
};

/***********************************************************************************/
struct CAppDaliObjectDT8ColorData_Utils {

public:

	// Bepalen of een color type geldig is.
	static bool isValidColorType(const CAppDaliObjectDT8ColorData& ref);

	// Bepalen van het type color die actief is.
	static bool isTypeColorTemperature(const CAppDaliObjectDT8ColorData& ref);
	static bool isTypeColorCoordinateXY(const CAppDaliObjectDT8ColorData& ref);
	static bool isTypeColorRGBWAF(const CAppDaliObjectDT8ColorData& ref);
	static bool isTypeColorPrimaryChannelN(const CAppDaliObjectDT8ColorData& ref);

	// Deze functie bepaalt of er een sfeer moet aangemaakt worden door de configurator.
	// Doet een controle op het colortype en op de velden.
	// Wanneer een bepaald colortype actief is worden de velden gecontroleerd.
	// Wanneer alle waarden gemaskeerd zijn dan is deze niet aktief.
	static bool isActive(const CAppDaliObjectDT8ColorData& ref);
	static bool isActiveTypeColorTemperature(const CAppDaliObjectDT8ColorData& ref);
	static bool isActiveTypeColorCoordinateXY(const CAppDaliObjectDT8ColorData& ref);
	static bool isActiveTypeColorRGBWAF(const CAppDaliObjectDT8ColorData& ref);
	static bool isActiveTypeColorPrimaryChannelN(const CAppDaliObjectDT8ColorData& ref);
};

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
struct CAppDaliObjectDT8ColorSpace_Utils {

public:
	// Tabel entry met een vooraf bepaalde kleurcombinatie.
	struct ColorTableEntry {
		const char* name;
		uint8_t r, g, b;
		double x, y;
	};

	// X-Y Coordinate.
	struct DaliColorSpace_XY {
		double x;
		double y;
	};

	// RGB Waarde.
	struct DaliColorSpace_RGB {
		uint8_t r, g, b;
	};

	// Tabel met vooraf bepaalde kleurcombinaties.
	static const ColorTableEntry m_colorTable[];
	static int getColorTableNrOfEntries();

	// Grootte van de tabel
	// const size_t colorTableSize = sizeof(colorTable) / sizeof(Color);

	// Conversie van een RGB naar een XY waarde.
	static struct DaliColorSpace_XY convert_rgbToXy(double red, double green, double blue);

	// Zet een xy coördinaat om naar een RGB kleur
	static DaliColorSpace_RGB convert_xyToRGB(double x, double y, double brightness = 1.0);

	// DALI waarden: -1 en 255 (mask)
	static DaliColorSpace_RGB normalize(DaliColorSpace_RGB& r);

	// RGB Kleuren aanpassen met een witte component.
	// Het extra wit zorgt ervoor dat de kleuren helder worden.
	// Oppassen: Waarde tussen 0 & 254, 255 is mask not used.
	static DaliColorSpace_RGB applyWhite(DaliColorSpace_RGB& r,int white);

	// RGB Kleuren aanpassen met een witte & amber component.
	// Het extra wit zorgt ervoor dat de kleuren helder worden.
	// Het extra amber zorgt ervoor dat de kleuren warmer worden.
	// Oppassen: Waarde tussen 0 & 254, 255 is mask not used.
	static DaliColorSpace_RGB applyWhiteAmber(DaliColorSpace_RGB& r,int white,int amber);


	// Conversie van DALI waarde naar een Kelvin waarde.
	enum {
		_DALI_SYSTEM_ONE_MILLION = (1000000),

		// Waarde in Kelvin.
		DALI_SYSTEM_VALUE_TEMPERATURE_TC_KELVIN_MIN = 200,
		DALI_SYSTEM_VALUE_TEMPERATURE_TC_KELVIN_MAX = 20000,
		DALI_SYSTEM_VALUE_TEMPERATURE_TC_KELVIN_MASK = (DALI_SYSTEM_VALUE_TEMPERATURE_TC_KELVIN_MAX + 1),

		// Waarden in het DALI-bus systeem.
		DALI_SYSTEM_VALUE_TEMPERATURE_TC_MIREK_MIN = (_DALI_SYSTEM_ONE_MILLION/DALI_SYSTEM_VALUE_TEMPERATURE_TC_KELVIN_MAX),
		DALI_SYSTEM_VALUE_TEMPERATURE_TC_MIREK_MAX = (_DALI_SYSTEM_ONE_MILLION/DALI_SYSTEM_VALUE_TEMPERATURE_TC_KELVIN_MIN),
		DALI_SYSTEM_VALUE_TEMPERATURE_TC_MIREK_MASK = 0xFFFF,

		// XY Waarden.
		// DALI_SYSTEM_VALUE_COLOR_XY

	};

	static int convertFromKelvin2Mirek(int value);
	static int convertFromMirek2Kelvin(int value);

	// Waarde 0-65536 converteren naar een XY waarde.
	static double convertToXYValue(int value);
	static int convertFromXYValue(double value);
	

protected:
	static const DaliColorSpace_RGB RGB_COLOR_WHITE;
	static const DaliColorSpace_RGB RGB_COLOR_AMBER;

	// Clamp functie om waardes af te snijden tussen 0 en 255
	static uint8_t clamp(double value);

	// Waarde blenden.
	static uint8_t BlendChannel(uint8_t base, uint8_t blendValue, uint8_t target);

	// Gamma correctie.
	static double gammaCorrect(double channel);
};

/***********************************************************************************/
};
};
/***********************************************************************************/