#ifndef DUOTECNO_PLUGIN_ICONFIGTOOLAPP_H
#define DUOTECNO_PLUGIN_ICONFIGTOOLAPP_H
/*******************************************************/
namespace DUOTECNO {
namespace PLUGIN {
/*******************************************************/

/*******************************************************/
enum View
{
	VIEW_UNITS				= 0,
	VIEW_MOODS				= 1,
	VIEW_IRTX				= 2,
	VIEW_IRRX				= 3,
	VIEW_TEMP				= 4,		
	VIEW_TIME				= 5,		
	VIEW_SCHEDULE			= 6,
	VIEW_BITMAPS			= 7,
	VIEW_FTP				= 8,
	VIEW_SMAP_UNITS			= 9,
	VIEW_SMAP_MAIN			= 10,
	VIEW_SMAP_MYHOME		= 11,
	VIEW_SMAP_AVROOM		= 12,
	VIEW_AVMM				= 13,
	VIEW_CONFIG_GATEWAY		= 14,
	VIEW_ROOMCTRL_AUDIOCFG	= 15,
	VIEW_BOSESTCFG			= 16,
	VIEW_LEDSCFG			= 17,
	VIEW_SONOSSRCCFG		= 18,
	VIEW_AVSETTINGS			= 19,
	VIEW_AVMATRIX			= 20,		
	VIEW_TRIGGERS			= 21,		
	VIEW_MOBILE_APP			= 22,
	VIEW_FIRMWARE_UPDATE	= 23,		
	VIEW_DALI				= 24,		
	VIEW_ENERGYMANAGER		= 25,		
	VIEW_NONE				= -1
};

/*******************************************************/
enum Command
{
	COMMAND_NEW			= 0,	// Binding soft -> Configurator
	COMMAND_NAME		= 1,	// Binding soft -> Configurator
	COMMAND_CLOSE		= 2,	// Binding soft -> Configurator
	COMMAND_SHOW		= 3,	// Binding soft -> Configurator
	COMMAND_SAVE		= 4,	// Binding soft -> Configurator
	COMMAND_ACK			= 5,	// Not used
	COMMAND_REFRESH		= 6,	// Binding soft -> Configurator	
	COMMAND_EXPORT		= 7,	// Binding soft -> Configurator
	COMMAND_LOAD		= 8,	// Binding soft -> Configurator
	COMMAND_UPOAD_DONE	= 9,	// Configurator --> Binding soft (Status)
	COMMAND_NONE		= -1
};
/*******************************************************/


/*******************************************************/
/*******************************************************/
/*******************************************************/
struct ICommandStringParser {
	virtual ~ICommandStringParser() = 0;

	virtual Command getCommand() const = 0;
	virtual CString getFullName() const = 0;
	virtual View getView() const = 0;
	virtual int getNodeAddress( ) const = 0;
};
/*******************************************************/


/*******************************************************/
/*******************************************************/
/*******************************************************/
struct IConfigToolApp
{
	static IConfigToolApp& getInstance( );

	/************************************************//**
	 * @brief	View tonen.
	 * @detail	Nog niet aktief = NEW commando\n
	 *			Wel aktief = SHOW commando.
	 * @param	view [IN] de gewenste view.
	 * @param	bNodeAddress [IN] het la adres.	
	 ***************************************************/
	virtual void show(View view,BYTE bNodeAddress) = 0;	

	/************************************************//**
	 * @brief	View Data opnieuw inladen.
	 * @detail	LOAD commando
	 * @param	view [IN] de gewenste view.
	 * @param	bNodeAddress [IN] het la adres.	
	 ****************************************************/
	virtual void load(View view, BYTE bNodeAddress) = 0;

	/************************************************//**
	 * @brief	Tool openen in normale mode.
	 * @detail	Nog niet aktief = NEW commando\n
	 *			Wel aktief = SHOW commando.
	 ****************************************************/
	virtual void show( ) = 0;

	/************************************************//**
	 * @brief	Nieuwe applicatie.
	 * @detail	Nieuwe directory
	 ***************************************************/
	virtual void newDirectory( ) = 0;
	
	/************************************************//**
     * @brief	View afsluiten.
	 ***************************************************/
	virtual void close( ) = 0;

	/************************************************//**
     * @brief	Save 
	 * @detail	project opslaan.
	 ***************************************************/
	virtual void save( ) = 0;

	/************************************************//**
     * @brief	Het display refreshen 
	 * @detail	Als er een algoritme werd uitgevoerd moet
	 *			het display gefreshed worden.
	 *			dit is vnl: bij het uploaden van bitmaps.
	 * @brief	since v0x0565
	 ***************************************************/
	virtual void refreshDisplay( ) = 0;

	/************************************************//**
     * @brief	HTML bestanden exporteren.
	 * @detail	De configurator de opdracht geven dat er
	 *			HTML bestanden moeten geexporteerd worden.
	 * @brief	since v0x0565
	 ***************************************************/
	virtual void exportHTMLFiles( )		= 0;

	/************************************************//**
     * @brief	Zoeken naar configurator window
	 * @since	v0x0630
	 * @param	pWnd [OUT] pointer naar window.
	 * @return	TRUE wnr gevonden.
	 * @attention	pWnd is een tijdelijke pointer !!
	 ***************************************************/
	virtual BOOL findWindowConfigurator(CWnd** pWnd) = 0;


	/************************************************//** 
     * @brief	Parsen van een ontvangen bericht.
     * @sinnce  V16.43
	 * @param	s is ASCII command string
	 * @return	Pointer naar de parser.
	 *		    Pointer: De client moet deze opnieuw dealloceren.
	 *			NULL: Wanneer het bericht niet kon geparsed worden
	 ****************************************************/
	virtual ICommandStringParser* parseReceived(CString& s) = 0;
};
/*******************************************************/

/*******************************************************/
/*******************************************************/
/*******************************************************/
// Free functies om te testen.
extern int pluginconfigtoolapp_testcase(int testcase);


/*******************************************************/
};
};
/*******************************************************/
#endif
/*******************************************************/

