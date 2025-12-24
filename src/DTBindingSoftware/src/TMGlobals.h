#ifndef TM_GLOBALS
#define TM_GLOBALS


#ifndef EXTERN
	#ifdef MAIN
		#define EXTERN 
	#else
		#define EXTERN extern
	#endif
#endif

// ------------------------------------------
#include "app\appipc\appipccfgtool\ipluginconfigtoolapp.h"			// sedert V16.19
// ------------------------------------------
class CQHSMApplicationGlobal;
class CBindingDatabaseApplication;
class CNodeDatabase;
class CBindingEntries;
class CBindingFiles;
class CApplicationGraphics;
class CApplicationGuiUnit;
class CApplicationGlobalSettings;
class CComApplication;
class CAppNodeDatabase;
class CCommunicationStatus;
class CBindingConversionAutoOptions;
class CApplicationViewManager;
class CApplicationCommunicationSettings;
class CApplicationFileSettings;
class CApplicationFileAccess;
class CApplicationDisplaySettings;
class CModemApplicationFrame;
class CSerialCanModemErrorStatus;
class CSerialCanModemConnectStatus;
class CSerialCanModem;
class CAppState;
// ------------------------------------------
namespace TMApplicationViews
{
	EXTERN CApplicationViewManager* TMGetViews();

	#define INCLUDE_TMAPPLICATIONVIEWS	1
};

namespace TMGlobalSettings
{
	EXTERN CApplicationGlobalSettings* TMGetGlobalSettings();
	EXTERN CApplicationCommunicationSettings* TMGetCommunicationSettings();
	EXTERN CApplicationFileSettings* TMGetFileSettings();
	EXTERN CApplicationDisplaySettings* TMGetDisplaySettings();
	EXTERN CApplicationFileAccess* TMGetFileAccess();

	#define INCLUDE_TMGLOBALSETTINGS	1
};

namespace TMBindingDatabase
{
	EXTERN CBindingDatabaseApplication* TMGetBindingApp();
	EXTERN CBindingEntries* TMGetBindingEntries();
	EXTERN CBindingFiles* TMGetBindingFiles();
	EXTERN CBindingConversionAutoOptions* TMGetAutoBindingOptions();

	#define INCLUDE_TMBINDINGDATABASE	1
};

namespace TMNodeDatabase
{
	EXTERN CAppNodeDatabase* TMGetNodeDatabaseApp();
	EXTERN CNodeDatabase* TMGetDocumentNodes();
	EXTERN CNodeDatabase* TMGetRuntimeNodes();

	#define INCLUDE_TMNODEDATABASE		1
};

namespace TMGraphics
{
	EXTERN CApplicationGraphics* TMGetGraphics();
	EXTERN CApplicationGuiUnit* TMGetGuiUnit();

	#define INCLUDE_TMGRAPHICS			1
};

namespace TMGlobalHSM
{	
	EXTERN CQHSMApplicationGlobal* TMGetGlobalHSM();

	#define INCLUDE_TMGLOBALHSM			1
};

namespace TMCommunication
{
	EXTERN CComApplication* TMGetCommunicationApp();
	EXTERN const CCommunicationStatus& TMCommunicationGetStatus();	
	EXTERN const CSerialCanModemErrorStatus* TMGetModemErrorStatus();	
	EXTERN const CSerialCanModem* TMGetModemInfo();
	EXTERN BOOL TMIsConnected();

	#define INCLUDE_TMCOMMUNICATION		1
};


namespace TMVaria
{
	// Taken:
	// - Export alle bestanden die de configurator nodig heeft.
	// - Openen van de configurator op een bepaalde view.
	//
	// Gebruikt de functie: TMExportNeededConfiguratorFiles
	EXTERN void TMShowConfigurator(DUOTECNO::PLUGIN::View id,BYTE bNodeAddress);

	// Taken:
	// - Export alle bestanden die de configurator nodig heeft.
	EXTERN void TMExportNeededConfiguratorFiles( );
};



////////////////////////////////////////////////////////

// Lijst van units die delegation principe ondersteunen.
extern CPtrList g_listUnitsDelegation;

// Deze lijst wordt gebruikt om een Modbus sensor te koppelen aan een andere sensor unit die de meting doet.
// Het bevat alle sensor units die niet in een Modbus gateway node zitten.
extern CPtrList g_listUnitsSensReadMeasuredValue;	


// Sedert V16.00: USE_SENSOR_PIDCONTROL	
// Lijst van units die kunnen gebruikt worden door de sensor unit om een PWM waarde naartoe te sturen.
// Bevat een lijst met pointers naar STRUCT_UNIT
// Deze lijst wordt gebruikt om de property dialogen op te bouwen.
extern CPtrList g_listUnitsDimPIDControl;

////////////////////////////////////////////////////////


// -------------------------------------------------------
#ifdef INCLUDE_TMAPPLICATIONVIEWS
	#include "ApplicationViewManager.h"
#endif

#ifdef INCLUDE_MAINAPPLICATIONSTATE
	#include "MainApplicationState.h"
#endif

#ifdef INCLUDE_TMGLOBALSETTINGS
	#include "App\AppSettings\ApplicationGlobalSettings.h"
#endif

#ifdef INCLUDE_TMBINDINGDATABASE
	#include "BindingDatabaseApplication.h"
	#include "BindingFiles.h"
#endif

#ifdef INCLUDE_TMNODEDATABASE
	#include "app\appnodedbase\appnodedbase.h"
	#include "NodeDatabaseOperations.h"
	#include "NodeDatabase.h"
#endif

#ifdef INCLUDE_TMGRAPHICS
	#include "ApplicationGuiUnit.h"
	#include "ApplicationGraphics.h"
#endif

#ifdef INCLUDE_TMGLOBALHSM
	#include "QHSMApplicationGlobal.h"
	#include "app\appstate\appstate.h"	
#endif

#ifdef INCLUDE_TMCOMMUNICATION
	#include "ComApplication.h"
	#include "communication/communicationStatus.h"
#endif

// -------------------------------------------------------

#endif
