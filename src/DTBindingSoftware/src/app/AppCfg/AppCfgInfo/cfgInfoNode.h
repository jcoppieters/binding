#ifndef INCLUDED_DT_NODECONFIG_INFO_H
#define INCLUDED_DT_NODECONFIG_INFO_H
/*******************************************************/
class CCanNode;
/*******************************************************/

/**************************************************//**
 * @brief	
 * Info ivm de external configuratie voor een CCanNode.
 * Deze worden ook als vlaggen gebruikt.
 *
 * @attention 
 * De bestaande niet meer veranderen.\n
 * Nieuwe toevoegen aan het einde.
 *****************************************************/
enum ExternalConfigtype
{
	EXTERNAL_CONFIG_NONE			= 0x0000,	// geen configuratie.
	EXTERNAL_CONFIG_MULTIMEDIA		= 0x0001,	// (1<<0)
	EXTERNAL_CONFIG_MOODS			= 0x0002,	// (1<<1)
	EXTERNAL_CONFIG_UNITS_CREATE	= 0x0004,	// (1<<2)
	EXTERNAL_CONFIG_IRRX			= 0x0008,	// (1<<3)
	EXTERNAL_CONFIG_IRTX			= 0x0010,	// (1<<4)
	EXTERNAL_CONFIG_SCHEDULE        = 0x0020,	// (1<<5)
	EXTERNAL_CONFIG_SMAP_MAIN		= 0x0040,	// (1<<6)
	EXTERNAL_CONFIG_SMAP_MYHOME		= 0x0080,	// (1<<7)
	EXTERNAL_CONFIG_SMAP_AVROOM		= 0x0100,	// (1<<8)
	EXTERNAL_CONFIG_GATEWAY			= 0x0200,	// (1<<9)
	EXTERNAL_CONFIG_ROOMCTRL_AUDIO	= 0x0400,	// (1<<10)
	EXTERNAL_CONFIG_BOSEST			= 0x0800,	// (1<<11)
	EXTERNAL_CONFIG_LEDSCFG			= 0x1000,	// (1<<12)
	EXTERNAL_CONFIG_SONOSSRC		= 0x2000,	// (1<<13)
	EXTERNAL_CONFIG_AVSETTINGS		= 0x4000,	// (1<<14)	TM,V14.05
	EXTERNAL_CONFIG_AVMATRIX		= (1<<15),	// TM,v15.02: CHANGES_AV_UNIVERSAL
	EXTERNAL_CONFIG_TRIGGERS		= (1<<16),	// TM,v15.02: CHANGES_AV_UNIVERSAL
	EXTERNAL_CONFIG_MOBILE_APP		= (1<<17),	// TM,v15.06
	EXTERNAL_CONFIG_DALI			= (1<<18),	// TM,v16.33
};

struct CNodeExtConfigInfo_Util
{
	static bool isSupported(CCanNode* pNode);
	static bool isSupported(CCanNode* pNode,ExternalConfigtype type);
	static unsigned long getSupportedFlags(CCanNode* pNode);
};

/******************************************************/

/**************************************************//**
 * @brief	
 *	Info ivm de bitmap configuratie voor een CCanNode.\n
 *	De verschillende node types hebben een ander geheugen
 *	map.
 *
 * @attention 
 * De bestaande niet meer veranderen.\n
 * Nieuwe toevoegen aan het einde.
 *****************************************************/
enum NodeBitmapConfigtype
{
	NODEBITMAP_CONFIG_TYPE_NONE			= 0x0000,
	NODEBITMAP_CONFIG_TYPE_V62_BASIC	= 0x0001,
	NODEBITMAP_CONFIG_TYPE_V63_BASIC	= 0x0002,
	NODEBITMAP_CONFIG_TYPE_V64_BASIC	= 0x0004,
	NODEBITMAP_CONFIG_TYPE_V64_HC4		= 0x0008,
	NODEBITMAP_CONFIG_TYPE_V64_HC5		= 0x0010,
	NODEBITMAP_CONFIG_TYPE_V65			= 0x0020,
	NODEBITMAP_CONFIG_TYPE_V66			= 0x0040
};
struct CNodeBitmapConfig_Util
{
	static bool isSupported(CCanNode* pNode);
	static NodeBitmapConfigtype getNodetype(CCanNode* pNode);
	static bool getRemoteDirectory(CCanNode* pNode,char* szFileName,int maxLength);
};
/******************************************************/


/**************************************************//**
 * @brief	
 *	Info ivm de multimedia configuratie voor een CCanNode.\n
 *	V65 heeft een andere configuratie dan de V64.
 *
 * @attention 
 * De bestaande niet meer veranderen.\n
 * Nieuwe toevoegen aan het einde.
 *****************************************************/
enum NodeMultimediaConfigType
{
	NODEMULTIMEDIA_CONFIG_TYPE_NONE		= 0x0000,
	NODEMULTIMEDIA_CONFIG_TYPE_V64		= 0x0001,
	NODEMULTIMEDIA_CONFIG_TYPE_V65		= 0x0002,
	NODEMULTIMEDIA_CONFIG_TYPE_V66		= 0x0003		/* 20181003 */
};

struct CNodeMultimediaConfig_Util
{
	static bool isSupported(CCanNode* pNode);
	static NodeMultimediaConfigType getNodeType(CCanNode* pNode);	
};
/******************************************************/


/**************************************************//**
 * @brief	
 *	Info ivm de backup configuratie voor een CCanNode.\n
 *	Ieder node tupe heeft een andere geheugen map.
 *
 * @attention 
 * De bestaande niet meer veranderen.\n
 * Nieuwe toevoegen aan het einde.
 *****************************************************/
enum NodeBackupConfigType
{
	NODEBACKUP_CONFIG_TYPE_NONE			= 0x0000,
	NODEBACKUP_CONFIG_TYPE_V62_BASIC	= 0x0001,
	NODEBACKUP_CONFIG_TYPE_V63_BASIC	= 0x0002,
	NODEBACKUP_CONFIG_TYPE_V64_BASIC	= 0x0004,
	NODEBACKUP_CONFIG_TYPE_V64_HC4		= 0x0008,
	NODEBACKUP_CONFIG_TYPE_V64_HC5		= 0x0010,
	NODEBACKUP_CONFIG_TYPE_V65			= 0x0020,
	NODEBACKUP_CONFIG_TYPE_V66			= 0x0040
};
struct CNodeBackupConfig_Util
{
	static bool isSupported(CCanNode* pNode);
	static NodeBackupConfigType getNodeType(CCanNode* pNode);
};
/*******************************************************/


/*************************************************//**
 * @brief
 *	Bit mask die bepaalt welke units er dynamisch 
 *  kunnen aangemaakt worden.
 *
 * @attention 
 * De bestaande niet meer veranderen.\n
 * Nieuwe toevoegen aan het einde.
 ****************************************************/
enum ExternalConfigUnitsTypes
{
	CONFIGFLAG_CREATE_UNIT_TYPE_DIM				= 0x000001ul,
	CONFIGFLAG_CREATE_UNIT_TYPE_SWITCH			= 0x000002ul,
	CONFIGFLAG_CREATE_UNIT_TYPE_CONTROL			= 0x000004ul,
	CONFIGFLAG_CREATE_UNIT_TYPE_SENS			= 0x000008ul,
	CONFIGFLAG_CREATE_UNIT_TYPE_AUDIO			= 0x000010ul,
	CONFIGFLAG_CREATE_UNIT_TYPE_LCD				= 0x000020ul,	// deprecated
	CONFIGFLAG_CREATE_UNIT_TYPE_LCD_VIRTUAL		= 0x000040ul,
	CONFIGFLAG_CREATE_UNIT_TYPE_DUOSWITCH		= 0x000080ul,
	CONFIGFLAG_CREATE_UNIT_TYPE_SELECTOR		= 0x000100ul,
	CONFIGFLAG_CREATE_UNIT_TYPE_BOSERS			= 0x000200ul,
	CONFIGFLAG_CREATE_UNIT_TYPE_AVMATRIX		= 0x000400ul,
	CONFIGFLAG_CREATE_UNIT_TYPE_IRTX			= 0x000800ul,
	CONFIGFLAG_CREATE_UNIT_TYPE_RC5RX			= 0x001000ul,
	CONFIGFLAG_CREATE_UNIT_TYPE_VIDEOMUX		= 0x002000ul,
	CONFIGFLAG_CREATE_UNIT_TYPE_PS2INTELLIMOUSE	= 0x004000ul,
	CONFIGFLAG_CREATE_UNIT_TYPE_SPEECHCOMM		= 0x008000ul,	// deprecated
	CONFIGFLAG_CREATE_UNIT_TYPE_SPMATRIX		= 0x010000ul,	// deprecated
	CONFIGFLAG_CREATE_UNIT_TYPE_ALARM			= 0x020000ul
};

// CHANGES_AV_UNIVERSAL
enum ExternalConfigUnitsCreateType
{
	UNITSCREATE_TYPE_DEFAULT = 0,				// Standaard via de CAN-bus versturen.
	UNITSCREATE_TYPE_UNITCONFIG_IP	= 1,		// Worden via IP verstuurd met extra config veld.
};

struct CNodeCreateDynUnits_Util
{	
	static unsigned long getSupportedTypes(CCanNode* pNode);	
	static ExternalConfigUnitsCreateType getNodeType(CCanNode* pNode); // CHANGES_AV_UNIVERSAL
};


/*******************************************************/
struct CNodeIRTXConfig_Util
{
	static bool isSupported(CCanNode* pNode);
	static bool getFileNameLocal(CCanNode* pNode,char* szFileName,int maxLength);
	static bool getFileNameRemote(CCanNode* pNode,char* szFileName,int maxLength);
};

/*******************************************************/

// Sedert V16.29
// De Resources blocks3 bevat extra product info vlaggen.
// Voor de DT18-PRO kunnen we op deze manier bepalen of de camera server ondersteund wordt.

enum {
	PRODUCT_DT18PRO_FLAG_CAMERASERVER			= (1<<0)
};

enum {
	AVSETTINGS_CFG_FLAGS_CAMERA					= (1<<0),
	AVSETTINGS_CFG_FLAGS_INDOORVIDEOSIP			= (1<<1)
};

struct CNodeAVSettingsConfig_Util
{
	static bool isSupported(CCanNode* pNode);
	static bool getFileNameLocal(CCanNode* pNode,char* szFileName,int maxLength);
	static bool getFileNameRemote(CCanNode* pNode,char* szFileName,int maxLength);
	static int getSupportedFlags(CCanNode* pNode);
};


/*******************************************************/
struct CNodeSonosSrcItemsConfig_Util
{

#ifdef SKIP
	static bool isSupported(
		CCanNode* pNode);
#endif

	static bool getFileNameLocal(CCanNode* pNode,char* szFileName,int maxLength);
	static bool getFileNameRemote(CCanNode* pNode,char* szFileName,int maxLength);
};

/*******************************************************/

struct CNodeSystemLog_Util
{
	enum FileID
	{
		FILE_ID_SYSTEMLOG			= 0,
		FILE_ID_SYSTEMLOG_BACKUP	= 1
	};

	static bool isSupported(CCanNode* pNode,FileID fileid);
	static bool getFileNameLocal(CCanNode* pNode,FileID fileid,char* szFileName,int maxLength);
	static bool getFileNameRemote(CCanNode* pNode,FileID fileid,char* szFileName,int maxLength);
};

/*******************************************************/

struct CCanNodeInfo_Util
{
public:
	static bool formatSoftwareVersion(const CCanNode& refNode,CString& szDest);
};

/******************************************************/

// Voor de V66 node.
enum ConfigSupportHTTP
{
	CONFIG_SUPPORT_HTTP_NONE			= 0,
	CONFIG_SUPPORT_HTTP_MOODS			= (1<<0),			
	CONFIG_SUPPORT_HTTP_UNITS			= (1<<1),
	CONFIG_SUPPORT_HTTP_IRTX			= (1<<2),		
	CONFIG_SUPPORT_HTTP_SCHEDULE    	= (1<<3),	
	CONFIG_SUPPORT_HTTP_AVMATRIX		= (1<<4),
	CONFIG_SUPPORT_HTTP_TRIGGERACTIONS 	= (1<<5),
	CONFIG_SUPPORT_HTTP_DALI			= (1<<6)
};			

struct CNodeConfigSupportHttpInfo_Util
{
	static bool isSupported(CCanNode* pNode);
	static bool isSupported(CCanNode* pNode,ConfigSupportHTTP type);
	static unsigned long getSupportedFlags(CCanNode* pNode);
};


/******************************************************/
// Sedert Version 16.04		
struct CNodeFirmware_Util
{
	static bool isSupported(CCanNode* pNode);
};	

/******************************************************/
// Sedert V16.41:
// Specifieke klasse voor de smartbox ultra.
struct CNodeConfigInfo_SmartboxUltra {

	// Opvragen of de node van het type smartbox ultra is.
	static bool isType(CCanNode* pNode);

	// Het laatste unit address opvragen van de interne units. 
	// -1 indien ongeldig
	static int getUnitAddressLastInternalUnit(CCanNode* pNode);

	// Het laatste unit address opvragen van de interne dimmer units. 
	// -1 indien ongeldig
	static int getUnitAddressLastInternalUnit(CCanNode* pNode,int unitType);
	static int getUnitAddressFirstInternalUnit(CCanNode* pNode,int unitType);

	// Opvragen van de tekst label info.
	// De tekst die weergeven wordt op de sticker van de module.
	static CString getStringLabelInfo(CCanNode* pNode, int unitType, int unitAddress);
};

/******************************************************/
// Sedert V16.42:
// Specifieke klasse voor de touch buttons.
struct CNodeConfigInfo_TouchButtons {

	// Opvragen of de node van het type touchbutton is.
	static bool isType(CCanNode* pNode);

	// Opvragen van de tekst label info.
	// De tekst die weergeven wordt op de sticker van de module.
	static CString getStringLabelInfo(CCanNode* pNode, int unitType, int unitAddress);
};
/******************************************************/
// Sedert V16.42:
// Specifieke klasse voor de Serinity buttons.
struct CNodeConfigInfo_SerenityButtons {

	// Opvragen of de node van het type touchbutton is.
	static bool isType(CCanNode* pNode);

	// Opvragen van de tekst label info.
	// De tekst die weergeven wordt op de sticker van de module.
	static CString getStringLabelInfo(CCanNode* pNode, int unitType, int unitAddress);
};
/******************************************************/
// Sedert V16.42:
// Specifieke klasse voor de Velocity buttons.
struct CNodeConfigInfo_VelocityButtons {

	// Opvragen of de node van het type touchbutton is.
	static bool isType(CCanNode* pNode);

	// Opvragen van de tekst label info.
	// De tekst die weergeven wordt op de sticker van de module.
	static CString getStringLabelInfo(CCanNode* pNode, int unitType, int unitAddress);
};

/******************************************************/

// Sedert V16.47
struct CNodeConfigInfo_Touchscreen_V66 {

	// Opvragen of de node van het type LCD is.
	static bool isType(CCanNode* pNode);

	// Het laatste unit address opvragen van de interne units. 
	// -1 indien ongeldig
	static int getUnitAddressLastInternalUnit(CCanNode* pNode);
};

/******************************************************/
// Sedert V16.47
struct CNodeConfigInfo_DT10_AU {

	// Opvragen of de node van het type LCD is.
	static bool isType(CCanNode* pNode);

	// Het laatste unit address opvragen van de interne units. 
	// -1 indien ongeldig
	static int getUnitAddressLastInternalUnit(CCanNode* pNode);
};

/******************************************************/
// Sedert V16.47
struct CNodeConfigInfo_DT18_PRO {

	// Opvragen of de node van het type LCD is.
	static bool isType(CCanNode* pNode);

	// Het laatste unit address opvragen van de interne units. 
	// -1 indien ongeldig
	static int getUnitAddressLastInternalUnit(CCanNode* pNode);
};

/******************************************************/
#endif
/******************************************************/