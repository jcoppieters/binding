/**********************************************************//**
 * @file	ConfigAppUiCtrlClasses
 * @brief	header bestand voor de CConfigExternalClass klasse.
 *
 * @author	Mitchell Tom
 * @date	20150806
 *
 * @since	0x0804
 *************************************************************/
#ifndef DUOTECNO_CONFIG_UICTRLCLASSES
#define DUOTECNO_CONFIG_UICTRLCLASSES
/*************************************************************/
#include "app\appcfg\appcfgbase\ConfigExternalClass.h"

#include "utils/mfc/IReportError.h"
#include "utils/mfc/TPtrList.h"
/*************************************************************/
class ostream;
/*************************************************************/

namespace DUOTECNO {
namespace DATA {

	#pragma pack(1)			/* Oppassen: #pragma pack() toevoegen aan einde */

	struct CSerLedCfgGeneral   
	{
		// algemene instellingen...
		uint8_t m_colorDefault;				// 0
		uint8_t m_lvlIntensity;				// 1
		uint8_t m_cfgData;					// 2
		uint8_t m_reserved[5];				// 3-7

		// Instellingen brightness ifv status fdbk
		uint8_t m_bNodeAddress;				// 8
		uint8_t m_bUnitAddress;				// 9
		uint8_t m_type;						// 10
		uint8_t m_lvlIntensityDay;			// 11
		uint8_t m_lvlIntensityNight;		// 12
		uint8_t m_reserved2[3];				// 13-15
	};

	/***********************************************************/
	struct SERCLASS_DATA_LEDCFG_TACTILEFDBK
	{
		uint8_t m_ColOn;					// 0
		uint8_t m_ColActive;				// 1
		uint8_t m_ColActiveLong;			// 2
		uint8_t m_Reserved;					// 3
	};

	struct SERCLASS_DATA_LEDCFG_FDBKDIM
	{
		uint8_t m_ColOff;					// 0
		uint8_t m_ColOn;					// 1
		uint8_t m_ColPirTimed;				// 2
		uint8_t m_Reserved;					// 3
	};

	struct SERCLASS_DATA_LEDCFG_FDBKSWITCH
	{
		uint8_t m_ColOff;					// 0
		uint8_t m_ColOn;					// 1
		uint8_t m_ColPirTimed;				// 2
		uint8_t m_Reserved;					// 3
	};

	struct SERCLASS_DATA_LEDCFG_FDBKDUOSWITCH
	{
		uint8_t m_ColorIdle;				// 0
		uint8_t m_ColorIdleUp;				// 1
		uint8_t m_ColorIdleDown;			// 2
		uint8_t m_ColorBusyUp;				// 3
		uint8_t m_ColorBusyDown;			// 4
		uint8_t m_Reserved[3];				// 5-7
	};

	struct SERCLASS_DATA_LEDCFG_FDBKVIRTUAL
	{
		uint8_t m_ColorOn;				// 0
		uint8_t m_ColorOff;				// 1
		uint8_t m_Reserved[2];			// 2-3
	};

	struct SERCLASS_DATA_LEDCFG_FDBKSENSSTATE
	{
		uint8_t m_ColorStable;			// 0
		uint8_t m_ColorHeating;			// 1
		uint8_t m_ColorCooling;			// 2
		uint8_t m_Reserved;				// 3
	};

	struct SERCLASS_DATA_LEDCFG_FDBKSENSPRESET
	{
		uint8_t m_ColorOff;				// 0
		uint8_t m_ColorPreset[4];		// 1-4
		uint8_t m_Reserved[3];			// 5-7
	};

	struct SERCLASS_DATA_LEDCFG_FDBKSENSSTATEONOFF
	{
		uint8_t m_ColorOff;				// 0
		uint8_t m_ColorOn;				// 1
		uint8_t m_Reserved[2];			// 2-3
	};

	struct SERCLASS_DATA_LEDCFG_FDBKAUDIO
	{
		uint8_t m_ColorOff;				// 0
		uint8_t m_ColorSrc[8];			// 1-8
		uint8_t m_Reserved[3];			// 9-11
	};

	// Deze klasse zorgt ervoor dat de serialzable data klassen altijd deze minimum
	// groote hebben.
	// Anders indien er klassen bijkomen die groter zijn dan zal de serializable
	// data niet meer compatible zijn...

	struct SERCLASS_DATA_LEDCFG_RESERVED
	{
		uint8_t m_Reserved[32];				// 32 bytes...
	};

	union SERCLASS_DATA_LEDCFG_UNION
	{
		// Tactile status feedback
		SERCLASS_DATA_LEDCFG_TACTILEFDBK m_fdbkTactile;

		// Status feedback
		SERCLASS_DATA_LEDCFG_FDBKDIM m_fdbkDim;
		SERCLASS_DATA_LEDCFG_FDBKSWITCH m_fdbkSwitch;
		SERCLASS_DATA_LEDCFG_FDBKDUOSWITCH m_fdbkDuoswitch;
		SERCLASS_DATA_LEDCFG_FDBKVIRTUAL m_fdbkVirtual;
		SERCLASS_DATA_LEDCFG_FDBKSENSSTATE m_fdbkSensState;
		SERCLASS_DATA_LEDCFG_FDBKSENSPRESET m_fdbkSensPreset;
		SERCLASS_DATA_LEDCFG_FDBKSENSSTATEONOFF m_fdbkSensStateOnOff;
		SERCLASS_DATA_LEDCFG_FDBKAUDIO m_fdbkAudio;

		// Extra speciale klasse: RESERVED
		// Zorgt ervoor dat de serializable klassen altijd dezelfde groote
		// hebben...
		SERCLASS_DATA_LEDCFG_RESERVED m_fdbkReserved;
	};

	struct CSerLedCfg
	{
		uint8_t m_LedID;						// 0
		uint8_t m_function;						// 1
		uint8_t m_cfgData;						// 2
		uint8_t m_reserved[5];					// 3-7
		uint8_t m_bNodeAddress;					// 8
		uint8_t m_bUnitAddress;					// 9
		uint8_t m_type;							// 10
		uint8_t m_reserved2;					// 11
		SERCLASS_DATA_LEDCFG_UNION v;			// vanaf 12
	};

	#pragma pack()			/* !!!! */

}; /* namespace DUOTECNO */
}; /* namespace DATA */

/*************************************************************/

/****************************************************************/

class CConfigLedCfgGeneralClass : public CConfigExternalClass
{
protected:
	static const char* m_szHeader;
	DUOTECNO::MFC_HELPER::IReportError* const m_pErrorReport;
	
private:
	int m_CurrentIndex;
	DUOTECNO::DATA::CSerLedCfgGeneral m_serDataLedCfg;

public:
	CConfigLedCfgGeneralClass(DUOTECNO::MFC_HELPER::IReportError *ptr);
	virtual ~CConfigLedCfgGeneralClass();

public:
	virtual BOOL ParseHeader(char *szAttribute,char *szData);
	virtual BOOL ParseData(char *szAttribute,char *szData);
	virtual void Initialise(void);
	virtual int GetMax(void) const;
	virtual void dump( );

public:
	virtual int GetData(int index,BYTE *pbData);
	virtual BOOL IsUsed(int index);
	int getStartIndex( );

protected:
	void ReportError(const char* szMessage);
};

/***************************************************************/

/***************************************************************/
class CConfigLedCfgLedClass : public CConfigExternalClass
{
protected:

	typedef enum
	{
		STATE_IDLE=0,
		STATE_DATA,
		STATE_LEDFDBK_TACTILE,
		STATE_LEDFDBK_DIM,
		STATE_LEDFDBK_SWITCH,
		STATE_LEDFDBK_DUOSWITCH,
		STATE_LEDFDBK_VIRTUAL,
		STATE_LEDFDBK_SENSSTATE,
		STATE_LEDFDBK_SENSPRESET,
		STATE_LEDFDBK_SENSSTATEONOFF,
		STATE_LEDFDBK_AUDIO,
		STATE_UNDEFINED

	} State_t;

	static const char* m_szHeader;
	DUOTECNO::MFC_HELPER::IReportError* const m_pErrorReport;
	
private:
	int m_CurrentIndex;
	State_t m_State;
	DUOTECNO::DATA::CSerLedCfg* m_pCurrSerLedCfg;
	DUOTECNO::MFC_HELPER::TPtrList<DUOTECNO::DATA::CSerLedCfg> m_list;

public:
	CConfigLedCfgLedClass(DUOTECNO::MFC_HELPER::IReportError *ptr);
	virtual ~CConfigLedCfgLedClass();

public:
	virtual BOOL ParseHeader(char *szAttribute,char *szData);
	virtual BOOL ParseData(char *szAttribute,char *szData);
	virtual void Initialise(void);
	virtual int GetMax(void) const;
	virtual void dump( );

public:
	virtual int GetData(int index,BYTE *pbData);
	virtual BOOL IsUsed(int index);
	int getStartIndex( );

protected:
	void removeAll( );
	void ReportError(const char* szMessage);
	DUOTECNO::DATA::CSerLedCfg* GetEntry(int entry);
};

/*************************************************************/
#endif /* DUOTECNO_CONFIG_UICTRLCLASSES */
/*************************************************************/