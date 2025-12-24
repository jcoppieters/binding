/**********************************************************//**
 * @file	ConfigExternalClass
 * @brief	header bestand voor de CConfigExternalClass klasse.
 *
 * @author	Mitchell Tom
 * @date	20090423
 *
 * @since	0x0506
 *************************************************************/
#ifndef DUOTECNO_CONFIGEXTERNALCLASS_INTERFACE
#define DUOTECNO_CONFIGEXTERNALCLASS_INTERFACE
/*************************************************************/

/**********************************************************//**
 * @brief	Basisklasse voor het laden en aanmaken van de 
 *			configuratie klassen.
 *************************************************************/
struct CConfigExternalClass
{
public:
	virtual ~CConfigExternalClass() { };

public:
	virtual BOOL ParseHeader(char *szAttribute,char *szData) = 0;
	virtual BOOL ParseData(char *szAttribute,char *szData) = 0;
	virtual void Initialise(void) = 0;
	virtual void dump( ) = 0;

public:
	/*******************************************************//**
     * @brief	Het aantal instanties van dit type.
	 * 
	 *			vb:\n
	 *			- UnitsConfig : f(aantal) units.
	 *			- mood header : altijd 1 
	 *			- mood entry : f(aantal) entries.
	 *			- audio room : f(aantal) audio rooms.
	 *			- audio source : altijd 8
	 *	
	 **********************************************************/
	virtual int GetMax(void) const = 0;


	/*******************************************************//**
	 * @brief	Serializable convert.	 
	 *			Omzetting van de klasse naar een array van bytes.	
	 **********************************************************/
	virtual int GetData(int index,BYTE *pbData) = 0;


	/*******************************************************//**
     * @brief	Wordt deze instantie gebruikt.
	 **********************************************************/
	virtual BOOL IsUsed(int index) = 0;	

	/*******************************************************//**
     * @brief	De start index waarmee de configuratie begint.
	 **********************************************************/
	// virtual int getStartIndex( ) const = 0;


	/*******************************************************//**
     * @brief	De entry opvragen als een cfg string.
	 *			Bevat header en datavelden.
	 *			Moet gebruikt worden om de cfg op te slaan.
     * @since	versie 0x0C05
	 *
	 * @attention	Is geen pure virtuele functie omdat niet
	 *				alle klassen opslaan ondersteunen.
	 **********************************************************/
	virtual BOOL GetEntryStrCfg(
			int index,
			char* pszTempBuffer,
			int maxBufferLength)	{ return FALSE; }

};

/*************************************************************/
#endif
/*************************************************************/
