#include "stdafx.h"
#include "TMGlobals.h"

#include "ExportTextFilesApp.h"									// since 0x0565
#include "MainFrm.h"											// since V16.19: saveCfgLoginInfoExport

//////////////////////////////// Globale var /////////////////////////////
CPtrList g_listUnitsDelegation;
CPtrList g_listUnitsSensReadMeasuredValue;		// sedert versie 0x0C03
CPtrList g_listUnitsDimPIDControl;				// Sedert V16.00: USE_SENSOR_PIDCONTROL	
//////////////////////////////////////////////////////////////////////////


namespace TMApplicationViews
{
	CApplicationViewManager* TMGetViews() 
	{
		return (CApplicationViewManager::GetInstance());
	}
};

namespace TMBindingDatabase
{
	/**
	* Get Binding Database application functions 
	*/
	CBindingDatabaseApplication* TMGetBindingApp() 
	{
		return (CBindingDatabaseApplication::GetInstance());
	}

	CBindingEntries* TMGetBindingEntries() 
	{
		return (CBindingDatabaseApplication::GetInstance()->GetBindingEntries());
	}

	CBindingFiles* TMGetBindingFiles() 
	{
		return (CBindingDatabaseApplication::GetInstance()->GetBindingFiles());
	}

	CBindingConversionAutoOptions* TMGetAutoBindingOptions() 
	{
		return (CBindingDatabaseApplication::GetInstance()->GetAutoBindingOptions());	
	}
};


namespace TMGlobalSettings
{
	CApplicationGlobalSettings* TMGetGlobalSettings() 
	{
		return (CApplicationGlobalSettings::GetInstance());
	}

	CApplicationCommunicationSettings* TMGetCommunicationSettings() 
	{
		ASSERT(CApplicationGlobalSettings::GetInstance());
		return (CApplicationGlobalSettings::GetInstance()->GetCommunicationSettings());
	}

	CApplicationFileSettings* TMGetFileSettings()
	{
		ASSERT(CApplicationGlobalSettings::GetInstance());
		return (CApplicationGlobalSettings::GetInstance()->GetFileSettings());
	}

	CApplicationDisplaySettings* TMGetDisplaySettings()
	{
		ASSERT(CApplicationGlobalSettings::GetInstance());
		return (CApplicationGlobalSettings::GetInstance()->GetDisplaySettings());
	}

	CApplicationFileAccess* TMGetFileAccess() 
	{
		ASSERT(CApplicationGlobalSettings::GetInstance());
		return (CApplicationGlobalSettings::GetInstance()->GetFileAccess());
	}
};


namespace TMNodeDatabase
{
	CAppNodeDatabase* TMGetNodeDatabaseApp() 
	{
		return (CAppNodeDatabase::GetInstance());
	}

	/**
	* Get Nodedatabase used by document
	*/
	CNodeDatabase* TMGetDocumentNodes() 
	{
		return (CAppNodeDatabase::GetInstance()->GetNodesDocument());
	}

	/**
	* Get Nodedatabase used for communication
	*/
	CNodeDatabase* TMGetRuntimeNodes() 
	{
		return (CAppNodeDatabase::GetInstance()->GetNodesRuntime());
	}
};


namespace TMGraphics
{
	CApplicationGraphics* TMGetGraphics() 
	{
		return (CApplicationGraphics::GetInstance());
	}

	CApplicationGuiUnit* TMGetGuiUnit() 
	{
		return (CApplicationGuiUnit::GetInstance());
	}
};


namespace TMGlobalHSM
{
	/**
	 * Get Global QHSM statemachine ....
	*/
	CQHSMApplicationGlobal* TMGetGlobalHSM() 
	{
		return (CQHSMApplicationGlobal::GetInstance());
	}
};




namespace TMCommunication
{
	CComApplication* TMGetCommunicationApp() {
		return (CComApplication::GetInstance());
	}

	BOOL TMIsConnected() {
		return (CComApplication::GetInstance()->IsConnected());
	}

	const CCommunicationStatus& TMCommunicationGetStatus(){
		return (CComApplication::GetInstance()->GetCommunicationStatus());
	}
	const CSerialCanModemErrorStatus* TMGetModemErrorStatus()
	{
		const CSerialCanModem* const pSerialCanModem = TMGetModemInfo();
		if (NULL == pSerialCanModem) {
			return NULL;
		}
		return &pSerialCanModem->getErrorStatus();
	}
	const CSerialCanModem* TMGetModemInfo() {		
		return (CComApplication::GetInstance()->GetCommunicationStatus().GetModemInfo());		
	}
};

// sedert V16.19
namespace TMVaria
{
	void TMShowConfigurator(DUOTECNO::PLUGIN::View id,BYTE bNodeAddress) {
	
		TMExportNeededConfiguratorFiles( );
							
		// Openen van de configurator.
		using DUOTECNO::PLUGIN::IConfigToolApp;
		IConfigToolApp::getInstance().show(id,bNodeAddress);
	}


	void TMExportNeededConfiguratorFiles( ) {	
		// Export van de tekst bestanden...
		CExportTextFilesApp txtExport;		
		txtExport.StartExport( );					
			
		// Opslaan van:
		// - json project login server info
		// - json project node services info 
		CMainFrame* const pMainWnd = ((CMainFrame*)AfxGetMainWnd());
		if (NULL != pMainWnd) {
			pMainWnd->saveCfgLoginInfoExport();
			pMainWnd->saveCfgNodeServicesExport();
		}
	}
};
