#include "stdafx.h"
#include "BindingSoftware.h"
#include "MainFrm.h"

#define MAIN


/*************************************************************************//**
*
* @attention
*		QHSM main applicatie:\n
*		Mogen geen Modale MessageBoxes gebruiken in de QHsm Transition chain.
*		--> QHQM-toestanden = Run to completion
*
****************************************************************************/

/***************************************************************************

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!																		!
! 	aanpassen van software versie									!
!		--> Macro : CURRENT_SW_VERSION in SoftWareVersion.h file.		!
!		Ook de string resources aanpassen !!!!!!!!!!!					!
!																		!
!		--> Modem software versie aanpassen.							!
!			Macro: CURRENT_MODEM_SW_VERSION								!
!																		!
!		OPPASSEN: Moeten CNodeCreateDynUnits_Util aanpassen wanneer		!
!				  er nieuwe unit types ondersteund worden				!
!																		!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


********************************************************************************

Component AddFlow :
Licence Number : 4118 F501 2D99 3234
User : Duotecno


Settings :
Resource Component Settings :
Flags :
CANREFLEX LINKS = FALSE
CANSTRETCH LINKS = FALSE

Control
SHOW PROPERTY PAGES = NONE

*********************************************************************************/


#include "BindingSoftwareDoc.h"
#include "MyWindowMessages.h"
#include "MyWindowTimers.h"
#include "MyException.h"
#include "TXNodeMessage.h"
#include "TXUnitMessage.h"
#include "NodeLCDScreenmapFSMInfo.h"
#include "ExportHtmlFilesApp.h"
#include "appdlgtasks.h"
#include "QHSMBindingConversionFromNodes.h"
#include "QMyEvent.h"
#include "MemoryDumpInfo.h"
#include "ApplicationDlgAppearance.h"
#include "BindingUiElement.h"
#include "BindingUiElements.h"
#include "ExportGenerateTables.h"

#include "app\appipc\appipccfgtool\ipluginconfigtoolapp.h"	

#include "app\AppCfg\AppCfgIRTX\ConfigIRTXFileInfo.h"
#include "app\AppIPC\AppIPCUpdateMgr\updatemgripc.h"		// since v0639
#include "app\Appbindings\appbindingproperty\BindingPropertyStrings.h"
#include "app\appbindings\appbindingconv2nodes\bindingconversiontonodes_utils.h"
#include "app\appfilelog\appfilelogapi.h"
#include "appdlgdaliworker.h"				
#include "app/appdali/appdalimanager.h"
#include "communication/communication.h"
#include "appremotecfg/AppRemoteCfgLoginInfo.h"			// Library.
#include "appremotecfg/AppRemoteCfgNodeService.h"		// Library.
#include "appremotecfg/IAppRemoteCfgApi.h"				// Library.
#include "app/appremotecfg/AppRemoteCfgNodes.h"
#include "app/appudpcfgserver/appudpservercfg.h"				// UDP Config...
#include "app/appipc/appipcadvinstupdater/appipcadvinstupdater.h"

#include "app\appCfg\AppCfginfo\cfginfoproductID_def.h"				// sedert V16.36 bepalen van de sbox ultra
#include "app/appcfg/appcfgbrowse/appcfgbrowseinfo.h"
#include "app\AppCfg\AppCfgAlgView\ConfigAlgViewUnits.h"
#include "app\appCfg\AppCfginfo\cfginfonode.h"	

#include "AppLogging.h"
						
// utils 
#include "utils/win32/ShellCommands.h"	
#include "utils/win32/ShellFunctions.h"	
#include "utils/win32/NotifyError.h"
#include "utils/win32/win32filefind.h"

// Views.
#include "views/intro/Splash.h"
#include "views/intro/PropertySheetNewApp.h"
#include "views/nodeprops/PropertyNodesSheet.h"
#include "views/unitprops/DlgUnitProp.h"				
#include "views/bindingprops/DlgBindingUnitProp.h"		
#include "views/bindingprops/DlgBindingOperator.h"	
#include "views/bindings/DlgBindingsNotFound.h"
#include "views/bindings/DlgAppBindingsVerify.h"
#include "views/bindings/DlgConversionFromNodes.h"
#include "views/comm/DlgCanFTP.h"		
#include "views/comm/DlgCommTracer.h"
#include "views/comm/DlgCommCanLog.h"		
#include "views/comm/DlgMessageTransfer.h"
#include "views/comm/DlgModemStatus.h"
#include "views/comm/DlgEngineeringRs232.h"
#include "views/comm/DlgNetworkCanStatus.h"
#include "views/debug/DlgDebugGeneral.h"	
#include "views/debug/DlgDebugBindingConversion.h"	
#include "views/common/DlgNodeServicesCfg.h"	
#include "views/apptasks/DlgMainApplicationTask.h"
#include "views/SWUpdate/dlgoptions.h"						
#include "views/SWUpdate/dlgsoftwareupdate.h"
#include "views/dali/PropertySheetDali.h"
#include "views/udpcfg/dlgUDPLanConfig.h"
#include "views/udpcfg/DlgUDPLANSettings.h"	
#include "views/udpcfg/dlgudplandevices.h"
#include "views/subsystems/dlgfirmwareupdate.h"	
#include "views/subsystems/dlgviewirtxcodes.h"			
#include "views/subsystems/DlgMultimedia.h"					// Wordt niet meer gebruikt?
#include "views/subsystems/DlgUnitExtProp.h"				// Wordt niet meer gebruikt?
#include "views/remotecfg/DlgToolsRemoteCfg.h"				// zie USE_TOOLS_APPREMOTECFG

#if(0)
	#include "utils/mfc/ToolTipCtrl.h"
#endif

	
// ------------------------------------
/*
#if(USE_TOOLTIP_CONTROLS == 1)
#include "app\apphelper\apphelpermfc\ToolTipCtrl.h"
#endif 
*/
// ------------------------------------
#include "TMGlobals.h"
using namespace TMCommunication;
using namespace TMGlobalHSM;
using namespace TMGraphics;
using namespace TMNodeDatabase;
using namespace TMBindingDatabase;
using namespace TMGlobalSettings;
using namespace TMApplicationViews;

#include "TMConvenience.h"
using namespace TMConvenience;
// ------------------------------------

/////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
#if (ENABLE_TRACE_PROPERTYNODES_SHEET == 1)
	#define TRACER_INFO(x)		AppLogging::getInstance().addInfo(x)
	#define TRACER_WARN(x)		AppLogging::getInstance().addWarning(x)
	#define TRACER_ERROR(x)		AppLogging::getInstance().addError(x)
#else 
	#define TRACER_INFO(x)
	#define TRACER_WARN(x)
	#define TRACER_ERROR(x)	
#endif
/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_BUTTON_VIEW_BINDINGS, OnButtonViewBindings)
	ON_COMMAND(ID_BUTTON_VIEW_NODES, OnButtonViewNodes)
	ON_COMMAND(ID_BUTTON_VIEW_UNITS, OnButtonViewUnits)
	ON_WM_TIMER()
	ON_COMMAND(ID_BUTTON_AND, OnButtonAnd)
	ON_COMMAND(ID_BUTTON_OR, OnButtonOr)
	ON_COMMAND(ID_BUTTON_TIMER, OnButtonTimer)
	ON_COMMAND(ID_BUTTON_XOR, OnButtonXor)
	ON_COMMAND(ID_DEBUG_BINDINGCONVERSION, OnDebugBindingconversion)
	ON_COMMAND(ID_BUTTON_ADD, OnButtonAdd)
	ON_COMMAND(ID_BUTTON_DELETE, OnButtonDelete)
	ON_COMMAND(ID_BUTTON_NEW, OnButtonNew)
	ON_COMMAND(ID_BUTTON_DISCONNECT, OnButtonDisconnect)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_DISCONNECT, OnUpdateButtonDisconnect)
	ON_COMMAND(ID_BUTTON_CONNECT, OnButtonConnect)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_CONNECT, OnUpdateButtonConnect)
	ON_COMMAND(ID_BUTTON_TRACER, OnButtonTracer)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_VIEW_BINDINGS, OnUpdateButtonViewBindings)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_VIEW_NODES, OnUpdateButtonViewNodes)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_VIEW_UNITS, OnUpdateButtonViewUnits)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_AND, OnUpdateButtonAnd)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_OR, OnUpdateButtonOr)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_TIMER, OnUpdateButtonTimer)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_XOR, OnUpdateButtonXor)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_ADD, OnUpdateButtonAdd)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_NEW, OnUpdateButtonNew)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_DELETE, OnUpdateButtonDelete)
	ON_COMMAND(ID_BUTTON_BINDINGS_SENDALL, OnButtonBindingsSendall)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_BINDINGS_SENDALL, OnUpdateButtonBindingsSendall)
	ON_COMMAND(ID_BUTTON_BINDINGS_SENDMODIFIED, OnButtonBindingsSendmodified)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_BINDINGS_SENDMODIFIED, OnUpdateButtonBindingsSendmodified)
	ON_COMMAND(ID_VIEW_SORTNODESBYNAME, OnViewSortnodesbyname)
	ON_COMMAND(ID_VIEW_SORTNODESBYPHADDRESS, OnViewSortnodesbyphaddress)
	ON_COMMAND(ID_VIEW_SORTNODESBYLOGICALADDRESS, OnViewSortnodesbylogicaladdress)
	ON_COMMAND(ID_VIEW_SORT_BINDINGS, OnViewSortBindings)
	ON_COMMAND(ID_BUTTON_COLLAPSE, OnButtonCollapse)
	ON_COMMAND(ID_FILE_EXPORT, OnFileExport)
	ON_COMMAND(ID_BUTTON_EXPAND, OnButtonExpand)
	ON_COMMAND(ID_BUTTON_SORT_UNITS, OnButtonSortUnits)
	ON_COMMAND(ID_BUTTON_SORT_NODES, OnButtonSortNodes)
	ON_COMMAND(ID_VIEW_UNITSBYADDRESS, OnViewUnitsbyaddress)
	ON_COMMAND(ID_VIEW_UNITSBYNAME, OnViewUnitsbyname)
	ON_COMMAND(ID_VIEW_UNITSONLYNAME, OnViewUnitsonlyname)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_SORT_NODES, OnUpdateButtonSortNodes)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_SORT_UNITS, OnUpdateButtonSortUnits)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SORT_BINDINGS, OnUpdateViewSortBindings)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_COLLAPSE, OnUpdateButtonCollapse)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_EXPAND, OnUpdateButtonExpand)
	ON_UPDATE_COMMAND_UI(ID_VIEW_UNITSONLYNAME, OnUpdateViewUnitsonlyname)
	ON_UPDATE_COMMAND_UI(ID_VIEW_UNITSBYNAME, OnUpdateViewUnitsbyname)
	ON_UPDATE_COMMAND_UI(ID_VIEW_UNITSBYADDRESS, OnUpdateViewUnitsbyaddress)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SORTNODESBYLOGICALADDRESS, OnUpdateViewSortnodesbylogicaladdress)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SORTNODESBYNAME, OnUpdateViewSortnodesbyname)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SORTNODESBYPHADDRESS, OnUpdateViewSortnodesbyphaddress)
	ON_COMMAND(ID_VIEW_SORTNODESONLYNAME, OnViewSortnodesonlyname)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SORTNODESONLYNAME, OnUpdateViewSortnodesonlyname)
	ON_UPDATE_COMMAND_UI(ID_TREEVIEW_BINDINGS_NAMEANDNUMBER, OnUpdateTreeviewBindingsNameandnumber)
	ON_COMMAND(ID_TREEVIEW_BINDINGS_NAMEANDNUMBER, OnTreeviewBindingsNameandnumber)
	ON_COMMAND(ID_TREEVIEW_BINDINGS_NUMBERANDNAME, OnTreeviewBindingsNumberandname)
	ON_UPDATE_COMMAND_UI(ID_TREEVIEW_BINDINGS_NUMBERANDNAME, OnUpdateTreeviewBindingsNumberandname)
	ON_COMMAND(ID_TREEVIEW_BINDINGS_ONLYNAME, OnTreeviewBindingsOnlyname)
	ON_UPDATE_COMMAND_UI(ID_TREEVIEW_BINDINGS_ONLYNAME, OnUpdateTreeviewBindingsOnlyname)
	ON_COMMAND(ID_BUTTON_AUTO, OnButtonAuto)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_AUTO, OnUpdateButtonAuto)
	ON_COMMAND(ID_DIMMERS_ONOFF, OnDimmersOnoff)
	ON_COMMAND(ID_DIMMERS_PIRTIMED, OnDimmersPirtimed)
	ON_UPDATE_COMMAND_UI(ID_DIMMERS_PIRTIMED, OnUpdateDimmersPirtimed)
	ON_UPDATE_COMMAND_UI(ID_DIMMERS_ONOFF, OnUpdateDimmersOnoff)
	ON_COMMAND(ID_RELAIS_ONOFF, OnRelaisOnoff)
	ON_COMMAND(ID_RELAIS_PIRTIMEDON, OnRelaisPirtimedon)
	ON_UPDATE_COMMAND_UI(ID_RELAIS_ONOFF, OnUpdateRelaisOnoff)
	ON_UPDATE_COMMAND_UI(ID_RELAIS_PIRTIMEDON, OnUpdateRelaisPirtimedon)
	ON_COMMAND(ID_AUTOBINDING_ENABLE, OnAutobindingEnable)
	ON_UPDATE_COMMAND_UI(ID_AUTOBINDING_ENABLE, OnUpdateAutobindingEnable)
	ON_COMMAND(ID_SENS_COOLING, OnSensCooling)
	ON_UPDATE_COMMAND_UI(ID_SENS_COOLING, OnUpdateSensCooling)
	ON_COMMAND(ID_SENS_HEATING, OnSensHeating)
	ON_UPDATE_COMMAND_UI(ID_SENS_HEATING, OnUpdateSensHeating)
	ON_COMMAND(ID_MOODS_LONG, OnMoodsLong)
	ON_UPDATE_COMMAND_UI(ID_MOODS_LONG, OnUpdateMoodsLong)
	ON_COMMAND(ID_MOODS_SHORT, OnMoodsShort)
	ON_UPDATE_COMMAND_UI(ID_MOODS_SHORT, OnUpdateMoodsShort)
	ON_COMMAND(ID_CONFIGURATION, OnConfiguration)
	ON_UPDATE_COMMAND_UI(ID_CONFIGURATION, OnUpdateConfiguration)
	ON_COMMAND(ID_MULTIMEDIA, OnMultimedia)
	ON_WM_CLOSE()
	ON_COMMAND(ID_MODEMSTATUS, OnModemstatus)
	ON_UPDATE_COMMAND_UI(ID_MODEMSTATUS, OnUpdateModemstatus)
	ON_COMMAND(ID_NETWORK_COMMUNICATION, OnNetworkCommunication)
	ON_COMMAND(ID_NETWORK_PHBROWSE, OnNetworkPhbrowse)
	ON_COMMAND(ID_NETWORK_REQUESTSTATES, OnNetworkRequeststates)
	ON_COMMAND(ID_NETWORK_AUTOPNP, OnNetworkAutopnp)
	ON_COMMAND(ID_NETWORK_READALLBINDINGS, OnNetworkReadallbindings)
	ON_COMMAND(ID_NETWORK_WRITEALLBINDINGS, OnNetworkWriteallbindings)
	ON_COMMAND(ID_TOOLS_IMPORT, OnToolsImport)
	ON_COMMAND(ID_TOOLS_CONVERSION, OnToolsConversion)
	ON_COMMAND(ID_TOOLS_NICMEDIUM, OnToolsNicmedium)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_NICMEDIUM, OnUpdateToolsNicmedium)
	ON_COMMAND(ID_NETWORK_MESSAGETRANSFER, OnNetworkMessagetransfer)
	ON_UPDATE_COMMAND_UI(ID_NETWORK_MESSAGETRANSFER, OnUpdateNetworkMessagetransfer)
	ON_COMMAND(ID_DEBUG_SHOW_TASKDIALOG, OnDebugShowTaskdialog)
	ON_UPDATE_COMMAND_UI(ID_DEBUG_SHOW_TASKDIALOG, OnUpdateDebugShowTaskdialog)
	ON_COMMAND(ID_VIEW_MODE_ADVANCED, OnViewModeAdvanced)
	ON_UPDATE_COMMAND_UI(ID_VIEW_MODE_ADVANCED, OnUpdateViewModeAdvanced)
	ON_COMMAND(ID_VIEW_MODE_NORMAL, OnViewModeNormal)
	ON_UPDATE_COMMAND_UI(ID_VIEW_MODE_NORMAL, OnUpdateViewModeNormal)
	ON_COMMAND(ID_VIEW_MODE_SIMPLE, OnViewModeSimple)
	ON_UPDATE_COMMAND_UI(ID_VIEW_MODE_SIMPLE, OnUpdateViewModeSimple)
	ON_COMMAND(ID_HELP_BINDINGEXAMPLES, OnHelpBindingexamples)
	ON_COMMAND(ID_DEBUG_GENERAL, OnDebugGeneral)
	ON_UPDATE_COMMAND_UI(ID_DEBUG_GENERAL, OnUpdateDebugGeneral)
	ON_COMMAND(ID_NETWORK_STATUS, OnNetworkStatus)
	ON_UPDATE_COMMAND_UI(ID_NETWORK_STATUS, OnUpdateNetworkStatus)
	ON_COMMAND(ID_NETWORK_CANFTP, OnNetworkCanftp)
	ON_UPDATE_COMMAND_UI(ID_NETWORK_CANFTP, OnUpdateNetworkCanftp)
	ON_COMMAND(ID_TOOLS_CONFIGURATOR, OnToolsConfigurator)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_CONFIGURATOR, OnUpdateToolsConfigurator)
	ON_UPDATE_COMMAND_UI(ID_MULTIMEDIA, OnUpdateMultimedia)
	ON_UPDATE_COMMAND_UI(ID_HELP_BINDINGEXAMPLES, OnUpdateHelpBindingexamples)
	ON_COMMAND(ID_AUTOBINDING_ADD, OnAutobindingAdd)
	ON_UPDATE_COMMAND_UI(ID_AUTOBINDING_ADD, OnUpdateAutobindingAdd)
	ON_COMMAND(ID_NETWORK_ENGINEERCANLOGGER, OnButtonEngineerCanLog)
	ON_UPDATE_COMMAND_UI(ID_NETWORK_ENGINEERCANLOGGER, OnUpdateButtonEngineerCanLog)
	ON_COMMAND(ID_HELP_MANUALHARDWARE, OnHelpManualHardware)
	ON_COMMAND(ID_HELP_MANUALSOFTWARE1, OnHelpManualSoftwarePart1)
	ON_COMMAND(ID_HELP_MANUALSOFTWARE2, OnHelpManualSoftwarePart2)
	ON_COMMAND(ID_HELP_MANUALSOFTWAREEXPERT, OnHelpManualSoftwareExpert)
	ON_UPDATE_COMMAND_UI(ID_HELP_MANUALHARDWARE, OnUpdateHelpManualHardware)
	ON_UPDATE_COMMAND_UI(ID_HELP_MANUALSOFTWARE1, OnUpdateHelpManualSoftwarePart1)
	ON_UPDATE_COMMAND_UI(ID_HELP_MANUALSOFTWARE2, OnUpdateHelpManualSoftwarePart2)
	ON_UPDATE_COMMAND_UI(ID_HELP_MANUALSOFTWAREEXPERT, OnUpdateHelpManualSoftwareExpert)
	//}}AFX_MSG_MAP
	
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_CONNECTED,OnUpdateIndicatorConnected)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_COMMUNICATION,OnUpdateIndicatorCommunication)

	ON_MESSAGE(WM_NODENAME_CHANGED,OnNodeNameChanged)
	ON_MESSAGE(WM_UNITNAME_CHANGED,OnUnitNameChanged)
	
	ON_MESSAGE(WM_SHOW_NODE_PROPERTIES,OnShowNodeProperties)
	ON_MESSAGE(WM_SHOW_UNIT_PROPERTIES,OnShowUnitProperties)
	ON_MESSAGE(WM_SHOW_NODE_UDPLANCFG,OnShowNodeUDPLanCfg)

	ON_MESSAGE(WM_UNIT_STATUS_CHANGED,OnUnitStatusChanged)
	ON_MESSAGE(WM_NODE_STATUS_CHANGED,OnNodeStatusChanged)
	ON_MESSAGE(WM_NODE_ETHERNETNWK_STATUS,onNodeEthernetNwkStatus)

	ON_MESSAGE(WM_DISPLAY_MESSAGE,OnDisplayMessage)
	ON_MESSAGE(WM_CLEAR_MESSAGES,OnClearMessages)
	ON_MESSAGE(WM_DISPLAY_TRACER_MESSAGEERROR,OnDisplayTracerMessageError)

	ON_MESSAGE(WM_TREE_NODE_COLLAPSED,OnTreeNodeCollapsed)
	ON_MESSAGE(WM_TREE_NODE_EXPANDED,OnTreeNodeExpanded)
	
	ON_MESSAGE(WM_BROWSEPNP_BROWSE_OK,OnBrowsePnpBrowseOk)
	ON_MESSAGE(WM_BROWSEPNP_PNP_OK,OnBrowsePnpPnpOK)
	ON_MESSAGE(WM_BROWSEPNP_ERROR,OnBrowsePnpError)
	ON_MESSAGE(WM_BROWSEPNP_CANCEL,OnBrowsePnpCancel)
	ON_MESSAGE(WM_BROWSEPNP_NORESPONSE,OnBrowsePnpNoResponse)
	ON_MESSAGE(WM_BROWSEPNP_DUPLICATES,OnBrowsePnpDuplicates)
	ON_MESSAGE(WM_BROWSEPNP_FACTORYADDRESS,OnBrowsePnpFactoryAddress)

	ON_MESSAGE(WM_SHOW_DIALOG_UNITPROPERTIES,OnShowDlgUnitProperties)

	ON_MESSAGE(WM_ADDFLOW_ADDUNIT,OnAddFlowAddUnit)
	ON_MESSAGE(WM_ADDFLOW_DRAGGING_UNIT,OnAddFlowDraggingUnit)
	ON_MESSAGE(WM_ADDFLOW_DROP_UNIT	,OnAddFlowDropUnit)
	ON_MESSAGE(WM_ADDFLOW_REMOVE_CONTROL,OnAddFlowRemoveControl)

	ON_MESSAGE(WM_BINDINGENTRY_ADD,OnBindingEntryAdd)
	ON_MESSAGE(WM_BINDINGENTRY_DELETE,OnBindingEntryDelete)
	ON_MESSAGE(WM_BINDINGENTRY_EDIT,OnBindingEntryEdit)
	ON_MESSAGE(WM_BINDINGENTRY_AUTO,OnBindingEntryAuto)

	ON_MESSAGE(WM_SHOW_DIALOG_BINDINGUNIT,OnShowDlgBindingUnit)
	ON_MESSAGE(WM_DESTROY_DIALOG_BINDINGUNIT,OnDestroyDlgBindingUnit)
	ON_MESSAGE(WM_SHOW_DIALOG_BINDINGTIMER,OnShowDlgBindingTimer)

	ON_MESSAGE(WM_BINDINGDATABASE_NAMECHANGED,OnBindingDatabaseNameChanged)

	ON_MESSAGE(WM_BINDINGSNODE_ERASE_START,OnBindingsNodeEraseStart)
	ON_MESSAGE(WM_BINDINGSNODE_ERASE_DONE,OnBindingsNodeEraseDone)
	ON_MESSAGE(WM_BINDINGSNODE_ERASE_ERROR,OnBindingsNodeEraseError)
	ON_MESSAGE(WM_BINDINGSNODE_WRITE_START,OnBindingsNodeWriteStart)
	ON_MESSAGE(WM_BINDINGSNODE_WRITE_DONE,OnBindingsNodeWriteDone)
	ON_MESSAGE(WM_BINDINGSNODE_WRITE_ERROR,OnBindingsNodeWriteError)
	ON_MESSAGE(WM_BINDINGSNODE_READ_START,OnBindingsNodeReadStart)
	ON_MESSAGE(WM_BINDINGSNODE_READ_DONE,OnBindingsNodeReadDone)
	ON_MESSAGE(WM_BINDINGSNODE_READ_ERROR,OnBindingsNodeReadError)
	ON_MESSAGE(WM_BINDINGSNODE_FSMWRITE_DONE,OnBindingsNodeFSMWriteDone)
	ON_MESSAGE(WM_BINDINGSNODE_FSMREAD_DONE,OnBindingsNodeFSMReadDone)

	ON_MESSAGE(WM_APPLICATION_FILENEW_PRE, OnApplicationFileNewPre)
	ON_MESSAGE(WM_APPLICATION_FILENEW,OnApplicationFileNew)
	ON_MESSAGE(WM_APPLICATION_FILESAVE_PRE, OnApplicationFileSavePre)
	ON_MESSAGE(WM_APPLICATION_FILESAVE_POST, OnApplicationFileSavePost)
	ON_MESSAGE(WM_APPLICATION_NOTIFY_BINDING_ADD,OnApplicationNotifyBindingAdd)
	ON_MESSAGE(WM_APPLICATION_ONLINEIDLE,OnApplicationOnlineIdle)	
	
	//ON_MESSAGE(WM_FSM_RENAMENODEUNITS_ENDED,OnFsmRenameNodeUnitsEnded)
	//ON_MESSAGE(WM_BINDINGCONVERSION_DEBUGINFO,OnBindingConversionDebugInfo)	
	//ON_MESSAGE(WM_SCREENMAP_FSM_DONEOK,OnScreenmapFSMDoneOk)
	//ON_MESSAGE(WM_SCREENMAP_FSM_DONEERROR,OnScreenmapFSMDoneError)
	//ON_MESSAGE(WM_SCREENMAP_FSM_USERCANCEL,OnScreenmapFSMUserCancel)
	//ON_MESSAGE(WM_MEMORYDUMP_FSM_DONEOK,OnMemoryDumpFSMDoneOk)
	//ON_MESSAGE(WM_MEMORYDUMP_FSM_DONEERROR,OnMemoryDumpFSMDoneError)
	//ON_MESSAGE(WM_MEMORYDUMP_FSM_USERCANCEL,OnMemoryDumpFSMUserCancel)
	//ON_MESSAGE(WM_MMCONFIG_FSM_DONEOK,OnMMConfigFSMDoneOk)
	//ON_MESSAGE(WM_MMCONFIG_FSM_DONEERROR,OnMMConfigFSMDoneError)
	//ON_MESSAGE(WM_MMCONFIG_FSM_USERCANCEL,OnMMConfigFSMUserCancel)	
	//ON_MESSAGE(WM_REQUESTSTATUSSEN_FSM_DONE,OnRequestStatussenDone)
	//ON_MESSAGE(WM_REQUESTSTATUSSEN_FSM_CANCELLED,OnRequestStatussenCancelled)
	//ON_MESSAGE(WM_REQUESTSTATUSSEN_FSM_START,OnRequestStatussenStart)


	/* TM 20060330 */
	ON_MESSAGE(WM_COMMUNICATION_MESSAGE_RECEIVED,OnCommunicationMessageReceived)
	ON_MESSAGE(WM_COMMUNICATION_MESSAGE_SEND,OnCommunicationMessageSend)
	ON_MESSAGE(WM_COMMUNICATION_COMMAND_RECEIVED,OnCommunicationCommandReceived)
	//ON_MESSAGE(WM_COMMUNICATION_COMMAND_SEND,OnCommunicationCommandSend)
	ON_MESSAGE(WM_COMMUNICATION_ERROR,OnCommunicationError)
	// ON_MESSAGE(WM_COMMUNICATION_EVMESSAGEERROR,OnCommunicationEvMessageError)

	

	ON_MESSAGE(WM_COMMUNICATION_MODEM_STATUS,OnCommunicationModemStatus)
	ON_MESSAGE(WM_COMMUNICATION_CONNECTION_LOST, OnCommunicationConnectionLost)
	ON_MESSAGE(WM_COMMUNICATION_MODEMLOGGERSTATUS, OnCommunicationModemLoggerStatus)

	ON_MESSAGE(WM_QHSM_EVENT,OnWmQhsmEvent)											// QHSM Events ...
	ON_MESSAGE(WM_QHSM_INITIALISE,OnWmQhsmInitialise)								// QHSM Events ...
	ON_MESSAGE(WM_QFSM_CONVERTFROMNODES_EVENT,OnWmConvertFromNodesEvent)			// QHSM Events ...
	ON_MESSAGE(WM_QFSM_CONVERTFROMNODES_INITIALISE,OnWmConvertFromNodesInitialise)	// QHSM Events ...
	ON_MESSAGE(WM_GENERICTASK_FSM_DONE,OnWmGenericTaskDone)							// Generic Tasks 
	ON_MESSAGE(WM_GENERICTASK_FSM_ERROR,OnWmGenericTaskError)						// Generic Tasks 
	ON_MESSAGE(WM_GENERICTASK_FSM_CANCELLED,OnWmGenericTaskCancelled)				// Generic Tasks 
	ON_COMMAND(ID_VIEW_UNIT_EXT_PROP, &CMainFrame::OnViewUnitExtProp)
	ON_UPDATE_COMMAND_UI(ID_VIEW_UNIT_EXT_PROP, &CMainFrame::OnUpdateViewUnitExtProp)
	ON_COMMAND(ID_VIEW_IRTXCODES, &CMainFrame::OnViewIrtxcodes)
	ON_UPDATE_COMMAND_UI(ID_VIEW_IRTXCODES, &CMainFrame::OnUpdateViewIrtxcodes)
	ON_MESSAGE(WM_PROPERTYSHEET_NODES_ENTER,&CMainFrame::OnPropertySheetEnter)
	ON_MESSAGE(WM_PROPERTYSHEET_NODES_EXIT,&CMainFrame::OnPropertySheetExit)
	ON_COMMAND(ID_HELP_MODBUSFUNCTIONS, &CMainFrame::OnHelpModbusfunctions)
	ON_UPDATE_COMMAND_UI(ID_HELP_MODBUSFUNCTIONS, &CMainFrame::OnUpdateHelpModbusfunctions)
	ON_COMMAND(ID_AUTOAUDIO_SIMPLE, &CMainFrame::OnAutoaudioSimple)
	ON_COMMAND(ID_AUTOAUDIO_SRCINC, &CMainFrame::OnAutoaudioSrcinc)
	ON_UPDATE_COMMAND_UI(ID_AUTOAUDIO_SIMPLE, &CMainFrame::OnUpdateAutoaudioSimple)
	ON_UPDATE_COMMAND_UI(ID_AUTOAUDIO_SRCINC, &CMainFrame::OnUpdateAutoaudioSrcinc)
	ON_MESSAGE(WM_APPLICATION_USERMESSAGE, &CMainFrame::OnApplicationUserMessage)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_REMOTECONFIG, OnUpdateMenuToolsRemoteCfg)
	ON_COMMAND(ID_TOOLS_REMOTECONFIG, &CMainFrame::OnMenuToolsRemoteCfg)
	ON_MESSAGE(WM_SHOW_DIALOG_FIRMWAREUPDATE, &CMainFrame::OnShowDialogFirmwareUpdate)		// Firmware update dialoog.
	ON_COMMAND(IDR_VIEW_OPTIONS_SOFTWAREUPDATE, &CMainFrame::OnViewOptionsSoftwareUpdate)
	ON_UPDATE_COMMAND_UI(IDR_VIEW_OPTIONS_SOFTWAREUPDATE, &CMainFrame::OnUpdateViewOptionsSoftwareUpdate)
	ON_COMMAND(IDR_HELP_SOFTWAREUPDATES, &CMainFrame::OnHelpSoftwareupdates)
	ON_COMMAND(ID_NETWORK_LAN_DEVICES, &CMainFrame::OnNetworkLanDevices)
	ON_COMMAND(ID_NETWORK_LAN_SETTINGS, &CMainFrame::OnNetworkLanSettings)
	ON_COMMAND(ID_VIEW_ENERGYMANAGEMENTCONFIG, &CMainFrame::OnViewEnergymanagementconfig)
	ON_COMMAND(ID_VIEW_DALIBUS_CONFIG, &CMainFrame::OnViewDaliBusconfig)
	ON_UPDATE_COMMAND_UI(ID_VIEW_DALIBUS_CONFIG, &CMainFrame::OnUpdateViewDaliBusconfig)
	ON_MESSAGE(WM_APPLICATION_NODE_SHOW_SERVICES, &CMainFrame::OnApplicationNodeShowServices)
	ON_MESSAGE(WM_APPLICATION_CONFIGURATOR_DALI_RELOAD, &CMainFrame::onApplicationConfiguratorDaliReload)
	ON_MESSAGE(WM_APPLICATION_OPEN_DALISUBBUS_CFG, &CMainFrame::onApplicationConfiguratorDaliOpenCfg)
	ON_MESSAGE(WM_APPLICATION_BROWSE_SINGLE_NODE, &CMainFrame::onApplicationBrowseSingleNode)
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTipNeedText)
	ON_MESSAGE(WM_APPLICATION_UNITCONFIG_DONE,&CMainFrame::OnApplicationUnitConfigDone)
	ON_MESSAGE(WM_APPLICATION_DALIBRIDGECONFIG_DONE,&CMainFrame::OnApplicationDaliBridgeConfigDone)
	ON_MESSAGE(WM_COPYDATA, &CMainFrame::OnCopyData)
	ON_MESSAGE(WM_APPLICATION_NOTIFY_ASSIGNED_LA,&CMainFrame::OnApplicationNotifyAssignedNodeAddress)
	ON_MESSAGE(WM_NOTIFY_PROBLEMSOLVED_RESTORED_NODEADDRESS, &CMainFrame::OnApplicationNotifyProblemSolveRestoredNodeAddress)
	END_MESSAGE_MAP()


static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CONNECTED,
	ID_INDICATOR_FREE1,
	ID_INDICATOR_COMMUNICATION,
	ID_INDICATOR_FREE2,

/*	ID_INDICATOR_CAPS,ID_INDICATOR_NUM, ID_INDICATOR_SCRL, */
};

/////////////////////////////////////////////////////////////////////////////
namespace
{

	BOOL removeMenu(CMenu* pMenu,CString szRemove)
	{		
		BOOL fResult = FALSE;
		if (pMenu != 0)
		{
			const unsigned int MenuItems = pMenu->GetMenuItemCount();			
			for (unsigned int i=0;i<MenuItems;i++)
			{
				CString szTest;
				if (pMenu->GetMenuString(i, szTest, MF_BYPOSITION))
				{
					if (szTest == szRemove)
					{
						fResult = pMenu->DeleteMenu(i , MF_BYPOSITION);
						ASSERT( fResult );
					}
				}
			}					
		}
		return fResult;
	}

	BOOL removeMenuItem(CMenu* pMenu, CString szSubMenu, int ID)
	{
		BOOL fResult = FALSE;
		if (pMenu != 0)
		{
			const unsigned int MenuItems = pMenu->GetMenuItemCount();			
			for (unsigned int i=0;i<MenuItems;i++)
			{
				CString szTest;
				if (pMenu->GetMenuString(i, szTest, MF_BYPOSITION))
				{
					if (szTest == szSubMenu)
					{
						CMenu* const pSubMenu = pMenu->GetSubMenu(i);
						if (pSubMenu)
						{
							fResult = pSubMenu->RemoveMenu(ID, MF_BYCOMMAND);
							break;
						}
					}
				}
			}					
		}
		return fResult;
	}
};
/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame() :
	m_fWriteBindingsBusy(FALSE), m_pAppRemoteCfgLoginInfo(0), m_pAppRemoteCfgNodes(0), m_pAppUDPServerCfg(0),
		m_stateStartApp(START_APP_INIT), m_flagUpdateNodeServices(FALSE), m_problemSolveWizard(NULL)
{
	TRACE("\nCMainFrame::CMainFrame()");	

	ENSURE(this->m_problemSolveWizard == NULL);
}

CMainFrame::~CMainFrame()
{
	// Save Settings !!!!!
	const BOOL fResult = TMGetGlobalSettings()->Save();
	ASSERT(fResult);

	if (NULL != m_pAppRemoteCfgLoginInfo) {
		delete m_pAppRemoteCfgLoginInfo;
		m_pAppRemoteCfgLoginInfo = NULL;
	}
	if (NULL != m_pAppRemoteCfgNodes) {
		delete m_pAppRemoteCfgNodes;
		m_pAppRemoteCfgNodes = NULL;
	}
	if (NULL != m_pAppUDPServerCfg) {
		delete m_pAppUDPServerCfg;
		m_pAppUDPServerCfg = NULL;
	}
	if (NULL != this->m_problemSolveWizard) {
		delete this->m_problemSolveWizard;
		this->m_problemSolveWizard = NULL;
	}

	DestroyApplication();
	CComApplication::Destroy();
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	// TM_CHANGES_BUG_0144:
	// Initialisatie...
	CApplicationGraphics::GetInstance()->Init();

	m_CommStatusIndicator = STATUS_OFFLINE;

	ASSERT(START_APP_INIT == m_stateStartApp);
	m_stateStartApp = START_APP_CREATE;

	// CG: The following line was added by the Splash Screen component.
	// CSplashWnd::ShowSplashScreen(this);

	const int nResult = InitialiseApplication(lpCreateStruct);		
	this->SetTimer(TIMERID_MAINFRAME_APPSTART,10,(TIMERPROC) NULL);
	return nResult;
}


int CMainFrame::InitialiseApplication(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	const CAppState::Type_t appType = CAppState::getInstance().getAppType();
	if (CAppState::APP_TYPE_INSTALLATION==appType)
	{
		if (!m_wndViewToolBar.CreateEx(this, TBSTYLE_BUTTON, WS_CHILD | WS_VISIBLE | CBRS_TOP 
			| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
			!m_wndViewToolBar.LoadToolBar(IDR_SWITCH_VIEWS))
		{
			TRACE0("Failed to create toolbar\n");
			return -1;      // fail to create
		}

		// Sedert V16.43: Tooltips aanmaken voor de m_wndViewToolBar
		// De weergave van de teksten gebeurd in de OnToolTipNeedText( )  callback functie.
		if (m_tooltipViewToolBar.Create(this)) {
			m_tooltipViewToolBar.AddTool(&m_wndViewToolBar);
			m_tooltipViewToolBar.Activate(TRUE);        
		}
		else {
			 TRACE0("Failed to create tooltip control\n");
		}

		if (!m_wndControlsToolBar.CreateEx(this, TBSTYLE_BUTTON, WS_CHILD | WS_VISIBLE | CBRS_TOP
			| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
			!m_wndControlsToolBar.LoadToolBar(IDR_BINDING_CONTROLS))
		{
			TRACE0("Failed to create toolbar\n");
			return -1;      // fail to create
		}


#ifndef _DEBUG
		// TM 20080121 : Remove debug from the Menu bar
		removeMenu(GetMenu(), "Debug"); 
#endif

#if(USE_TOOLS_APPREMOTECFG == 0)
		removeMenu(GetMenu(), "&Tools"); 
		removeMenuItem(GetMenu(), "&Tools", ID_TOOLS_REMOTECONFIG);		
#endif 
	
		// TM 20190509: Tools --> Configurator verwijderen...
		removeMenu(GetMenu(), "&Tools_old"); 


		// TM 20100511: &Tools is zichtbaar.
		// Duotecno configurator wordt gestart.
		// Multimedia config + Network status wordt niet meer gebruikt.

		removeMenuItem(GetMenu(), "&Tools_old", ID_MULTIMEDIA);
		removeMenuItem(GetMenu(), "&Tools_old", ID_TOOLS_NICMEDIUM);


		// V16.29: Engineer CAN-Logger menu item verwijderen.		
		removeMenuItem(GetMenu(), "&Network", ID_NETWORK_ENGINEERCANLOGGER);
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// SetText indicators FREE1 & FREE2
	m_wndStatusBar.SetPaneText(2,"",TRUE);
	m_wndStatusBar.SetPaneText(4,"",TRUE);

	TRACE("\nCMainFrame::OnCreate");
	TRACE("\nInitialise Application ...");

	CComApplication::Initialise(this);

	// Create + Load Global application settings ...
	TMGetGlobalSettings()->Load();				

	if (TMGetCommunicationApp() == 0) 
	{
		ASSERT(TMGetCommunicationApp());
		exit(-1);
	}	
	TRACE("\nInitialise Application Done...");
	return 0;
}

BOOL CMainFrame::OnToolTipNeedText(UINT id, NMHDR *pNMHDR, LRESULT *pResult) {	
	TOOLTIPTEXTA* pTTT = (TOOLTIPTEXTA*)pNMHDR;       
	UINT toolbarItemIdentifier = pNMHDR->idFrom;

/*
	CButton* const pWnd = (CButton*) this->GetDlgItem(toolbarItemIdentifier);
	if ((NULL != pWnd) && (FALSE == pWnd->IsWindowEnabled())) {
		// Geen tooltip nodig want het venster is niet enabled.
		return TRUE;
	}
*/
    switch (toolbarItemIdentifier) {
		case ID_BUTTON_VIEW_BINDINGS: {
			 pTTT->lpszText = "View bindings";  						  
		} break;
		case ID_BUTTON_VIEW_UNITS: {
			pTTT->lpszText = "View units";  
		} break;
		case ID_BUTTON_VIEW_NODES: {
			pTTT->lpszText = "View nodes";  
		} break;
		case ID_BUTTON_BINDINGS_SENDALL: {
			pTTT->lpszText = "Send all name and binding configuration.";  			
		} break;
		case ID_BUTTON_BINDINGS_SENDMODIFIED: {
			pTTT->lpszText = "Send modified binding configuration.";  			 								 
		} break;
		case ID_BUTTON_CONNECT: {
			pTTT->lpszText = "Connect to the Duotecno CAN-bus system";  
		} break;
		case ID_BUTTON_DISCONNECT: {
			pTTT->lpszText = "Disconnect from the Duotecno CAN-bus system";  
		} break;
		case ID_BUTTON_TRACER: {
			pTTT->lpszText = "Show communication tracer";  
		} break;
		case ID_BUTTON32816: {
			pTTT->lpszText = "Sort Bindings by names";
		} break;
		case ID_BUTTON_SORT_NODES: {
			pTTT->lpszText = "Toggle sort mode for the nodes.";
		} break;		
		case ID_BUTTON_EXPAND: {
			pTTT->lpszText = "Expand all nodes.";					
		} break;
		case ID_BUTTON_COLLAPSE: {
			pTTT->lpszText = "Collaps all nodes.";						 
		} break;
		case ID_BUTTON_SORT_UNITS: {
			pTTT->lpszText = "Toggle view mode for the units.";
		} break;

		// TOOLBAR
		case ID_BUTTON_AND: {
			pTTT->lpszText = "Add 'And'-operator";
		} break;
			case ID_BUTTON_OR: {
			pTTT->lpszText = "Add 'Or'-operator";
		} break;
		case ID_BUTTON_XOR: {
			pTTT->lpszText = "Add 'Xor'-operator";
		} break;
		case ID_BUTTON_TIMER: {
			pTTT->lpszText = "Add 'timer'-operator";
		} break;
		case ID_BUTTON_ADD: {
			pTTT->lpszText = "Add binding to the database";
		} break;
		case ID_BUTTON_DELETE: {
			pTTT->lpszText = "Delete the binding";
		} break;
		case ID_BUTTON_NEW: {
			pTTT->lpszText = "New binding";
		} break;
		case ID_BUTTON_AUTO: {
			pTTT->lpszText = "Create auto binding";			 
		} break;
	}	
    *pResult = 0;
    return TRUE;  // We handled the tooltip
}

/////////////////////////////////////////////////////////////////////////////
/**
 * CQHSMApplicationGlobal ...
 */
LRESULT CMainFrame::OnWmQhsmInitialise(WPARAM wParam,LPARAM lParam)
{
	TMGetGlobalHSM()->init();
	return TRUE;
}

LRESULT CMainFrame::OnWmQhsmEvent(WPARAM wParam,LPARAM lParam)
{
	if (TMGetGlobalHSM()->Locked() == FALSE)
	{
		QMyEvent evt;
		evt.sig = wParam;	
		evt.lParam = lParam;

		TMGetGlobalHSM()->dispatch(&evt);
		return TRUE;
	}
	else
	{
		//ASSERT(0);
		return FALSE;
	}
}


/**
 * CBindingConversionFromNodes 
 */
LONG CMainFrame::OnWmConvertFromNodesEvent(WPARAM wParam,LPARAM lParam)
{
	QMyEvent evt;
	evt.sig = wParam;	
	evt.lParam = lParam;

	CQHSMBindingConversionFromNodes::GetInstance()->dispatch(&evt);
	return TRUE;
}

LONG CMainFrame::OnWmConvertFromNodesInitialise(WPARAM wParam,LPARAM lParam)
{
	CQHSMBindingConversionFromNodes::GetInstance()->init();
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnClose() 
{
	this->KillTimer(TIMER_MAINFRAME_APPDALI_POLL_INTERVAL_MSEC);

	if (m_stateStartApp == START_APP_UPDATE_BUSY) {
		this->DestroyWindow();
	}
	else {
		TMGetGlobalHSM()->EvOperationShutdown();		
	}
}



//////////////////////////////////////////////////////////////////////////////////
// TASK =		1) Initialise Pointers ...			
//				3) WINDOW PLACEMENT = MAXIMIZED
//				4) InitViews 
//
// INVOKED BY : FrameWork - Everytime frame gets active ...
//				OnFileNew - OnFileOpen - ...
//
//////////////////////////////////////////////////////////////////////////////////
void CMainFrame::ActivateFrame(int nCmdShow) 
{
	// static BOOL fInit = FALSE;
	
	CFrameWnd::ActivateFrame(nCmdShow);

	WINDOWPLACEMENT lpWndPl;
	GetWindowPlacement(&lpWndPl);
	lpWndPl.showCmd = SW_SHOWMAXIMIZED;
	SetWindowPlacement(&lpWndPl);


#if(0)
	if (fInit == FALSE) 
	{			
		const CAppState::Type_t appType = CAppState::getInstance().getAppType();
		if (CAppState::APP_TYPE_INSTALLATION==appType)
		{
			TMGetViews()->InitViews();
		}

		PostMessage(WM_QHSM_INITIALISE);				
		PostMessage(WM_QFSM_CONVERTFROMNODES_INITIALISE);
		fInit = TRUE;
	}
#endif 

}


LONG CMainFrame::OnApplicationFileNewPre(WPARAM wParam,LPARAM lParam)
{
	// Dit bericht wordt SYNCHROON verstuurd als er een fileNew aangevraagd
	// wordt.
	// returns: TRUE: Er mag een nieuw project geopend worden.
	// returns: FALSE: Er mag geen nieuw project geopend worden.

	LONG result = FALSE;
	const char* szMessage = "Are you sure you want start an new installation ?";
	if (AfxMessageBox(szMessage,MB_OKCANCEL | MB_ICONQUESTION) == IDOK)
	{	
		result = TRUE;	// Er mag een nieuw document geopend worden.
	}	

	if (result)
	{	
		// Er werd een nieuwe applicatie gestart,
		// als de configuratie tool nog open staat moet deze afgesloten worden.
		// MOETEN DIT HIER DOEN, omdat de bestandsnaam van het document nog geldig is.
		// Dit is niet het geval in DestroyApplication( ).
		
		using namespace DUOTECNO::PLUGIN;
		IConfigToolApp::getInstance().close( );	
	}
	return result;
}

LONG CMainFrame::OnApplicationFileNew(WPARAM wParam,LPARAM lParam)
{			
	// Dit bericht wordt ASSYNCHROON verstuurd als er een fileNew aangevraagd wordt.
	TMGetGlobalHSM()->EvOperationNewApplication();
	return TRUE;
}

LONG CMainFrame::OnApplicationFileSavePre(WPARAM wParam,LPARAM lParam)
{
	// Dit bericht wordt SYNCHROON verstuurd voordat het bestand wordt gesaved.
	const CAppState::Type_t appType = CAppState::getInstance().getAppType();
	if (CAppState::APP_TYPE_INSTALLATION==appType)
	{	
		// Configuratie tool laten weten dat het huidige bestand gesaved zal worden.
		using namespace DUOTECNO::PLUGIN;
		IConfigToolApp::getInstance().save( );
	}
	return TRUE;
}

LONG CMainFrame::OnApplicationFileSavePost(WPARAM wParam,LPARAM lParam)
{
	// Dit bericht wordt SYNCHROON verstuurd nadat het bestand werd gesaved.
	const CAppState::Type_t appType = CAppState::getInstance().getAppType();
	if (CAppState::APP_TYPE_INSTALLATION==appType)
	{	
		// Configuratie tool laten weten dat dit het projectdirectory mogelijk veranderd is.
		using namespace DUOTECNO::PLUGIN;
		IConfigToolApp::getInstance().newDirectory( );
	}
	return TRUE;
}

// Sedert V16.15:
// TCP/IP connectie: Afhankelijk van het node type van de server de tekst weergeven.
// Melding: TCP/IP homeserver of TCP/IP touchscreen.
// Weergave van het host address, port + vermeldin connectie in lokaal of remote network.
LONG CMainFrame::OnCommunicationModemStatus(WPARAM wParam,LPARAM lParam)
{		
	const CCommunicationStatus& communicationStatus = CComApplication::GetInstance()->GetCommunicationStatus();
	if (communicationStatus.connected()) {	
	
		ClearScrollView( );

		const CCommunicationServerInfo& serverInfo = communicationStatus.getServerInfo();
		const int softwareVersion = serverInfo.getSoftwareVersion();
		CString s;		
		if (CCommunicationServerInfo::TCPIP == serverInfo.getCommunicationType()) {
			CString strNodeType("TCP/IP homeserver");
			CString strTcpConnectInfo;
			const CCommunicationServerInfo::NodeType_t nodeType = serverInfo.getNodeType();
			if (CCommunicationServerInfo::NODE_HOMESERVER == nodeType) {
				strNodeType = CString("TCP/IP homeserver");
			}
			else if (CCommunicationServerInfo::NODE_TOUCHSCREEN == nodeType) {
				strNodeType = CString("TCP/IP touchscreen");
			}

			CString strConnectionMethod("Connected local network");
			if (CONNECTION_METHOD_REMOTE_DIRECT == serverInfo.getConnectionMethod()) {
				strConnectionMethod = "Connected remote via port forwarding";
			}
			else if (CONNECTION_METHOD_REMOTE_CLOUD == serverInfo.getConnectionMethod()) {
				strConnectionMethod = "Connected remote via cloud server";
			}

			strTcpConnectInfo.Format("HostAddress: '%s', port: %d, version= V%02X.%02X. %s",
				serverInfo.getTcpHostName(), 
				serverInfo.getTcpSocketPort(),  			
				(softwareVersion/256),
				(softwareVersion%256),
				strConnectionMethod);

			s.Format("Successfull connected to the %s. Server info: %s.",strNodeType, strTcpConnectInfo);					
		}
		else {		
			s.Format("Successfull connected to the modem via communication port:COM%d,"
					 "modem firmware version=v%02x,modem address=0x%02x",
					 serverInfo.getSerialComport(),	((softwareVersion & 0xFF00)>>8), serverInfo.getNodeAddress());
		}
		DisplayScrollMessage(s);
		TMGetGlobalHSM()->EvSignalCommunicationOnline();
	}
	else 
	{				
		if (STATUS_OFFLINE != m_CommStatusIndicator) {
			CString s;
			s.Format("Disconnected...");
			DisplayScrollMessage(s);
		}

		m_fWriteBindingsBusy = FALSE;						// zie BUG-0177: opnieuw de knoppen 'add','delete','auto' enablen.
		TMGetGlobalHSM()->EvSignalCommunicationOffline();
		m_CommStatusIndicator = STATUS_OFFLINE;
	}
	
	CPropertySheetNewApp* const pDlgStart = CPropertySheetNewApp::get();
	if ((0 != pDlgStart) && (0 != pDlgStart->GetSafeHwnd())) {
		pDlgStart->PostMessage( WM_COMMUNICATION_MODEM_STATUS );
	}
	const CAppState::Type_t appType = CAppState::getInstance().getAppType();
	if (CAppState::APP_TYPE_ENGINEERING==appType) {
		if (CDlgEngineeringRS232::IsActive() == TRUE) {
			CDlgEngineeringRS232::GetInstance()->PostMessage(WM_COMMUNICATION_MODEM_STATUS);
		}
	}

	this->PostMessage(WM_COMMUNICATION_MODEMLOGGERSTATUS);

	return TRUE;
}

LONG CMainFrame::OnCommunicationModemLoggerStatus(WPARAM wParam,LPARAM lParam) {	
	CDlgCommCanLog::Update();
	return TRUE;
}

// Sedert V16.07: Melding geven aan de gebruiker wanneer de socket connectie verbroken werd.
// Sedert V16.26: Indien de connect dialoog aktief mag er geen pop-up bericht gegeven worden.
//				  Reden: Mogelijk omdat de gebruiker in de dialoog een disconnect uitgevoerd heeft.
//				  (De globale fsm is nog niet in de juiste toestand...)
LONG CMainFrame::OnCommunicationConnectionLost(WPARAM wParam,LPARAM lParam) {
	CPropertySheetNewApp* const pDlgStartConnect = CPropertySheetNewApp::get();
	if ((0 != pDlgStartConnect) && 
		(0 != pDlgStartConnect->GetSafeHwnd()) && 
		(DIALOGMODE_APPLICATION_CONNECT == pDlgStartConnect->getDlgMode())) 
	{	
		// Absorbe message - don't show popup 
		return TRUE;
	}
	AfxMessageBox("Connection with the server is lost.",MB_ICONERROR);
	return TRUE;
}


/**
 * Operations for communication
 */
void CMainFrame::OnButtonDisconnect() 
{
	TMGetGlobalHSM()->EvOperationCommunicationOffline();
}

// Sedert V16.26
// Indien we nog niet geconnecteerd zijn wordt de start dialoog tonen.
// Via de connect dialoog kunnen we de connectie methode kiezen.
void CMainFrame::OnButtonConnect()  
{
	if (TMIsConnected()) {
		return;
	}

	const BOOL finished = this->showPropertySheetNewApp(DIALOGMODE_APPLICATION_CONNECT);
	if (TMIsConnected()) {
		if (finished) {
			TMGetGlobalHSM()->EvSignalCommunicationOnline();
		}
		else {
			// Cancel button --> Disconnect.
			TMGetGlobalHSM()->EvOperationCommunicationOffline();
		}
	}
}


/**
 * Operations for writing binings ...
 */	
void CMainFrame::OnButtonBindingsSendall() 
{	
	CString szMessage("Would you like to send all names to the network ?");

	if (AfxMessageBox(szMessage,MB_ICONQUESTION | MB_YESNO) == IDYES) 
	{
		TMGetGlobalHSM()->EvOperationUploadProgFile();
	}
	else 
	{
		TMGetGlobalHSM()->EvOperationUploadAllBindings();
	}
}

void CMainFrame::OnButtonBindingsSendmodified() 
{		
	if (TMGetBindingApp()->ModifiedBindingFiles() == TRUE) 
	{
		TMGetGlobalHSM()->EvOperationUploadModifiedBindings();
	}
}

/**
 * Events from CBindingsReadWriteFsm 
 */	
LONG CMainFrame::OnBindingsNodeEraseStart(WPARAM wParam,LPARAM lParam) 
{
	const BYTE bNodeAddress = (BYTE) wParam & 0xFF;
	m_fWriteBindingsBusy=TRUE;
	return TRUE;
}

LONG CMainFrame::OnBindingsNodeEraseDone(WPARAM wParam,LPARAM lParam) 
{
	const BYTE bNodeAddress = (BYTE) wParam & 0xFF;
	// Look For bindingEntries of Nodes ... Mark As Not Send ...
	return TRUE;
}
	
LONG CMainFrame::OnBindingsNodeEraseError(WPARAM wParam,LPARAM lParam) 
{		
	TMGetGlobalHSM()->EvSignalTaskError();
	const BYTE bNodeAddress = (BYTE) wParam & 0xFF;
	CString szMessage;	
	szMessage.Format(_T("Error-Bindings not erased from node 0x%02x!"),bNodeAddress);				
	DisplayScrollMessage(szMessage);
	m_fWriteBindingsBusy = FALSE;
	return TRUE;
}

LONG CMainFrame::OnBindingsNodeWriteStart(WPARAM wParam,LPARAM lParam)
{
	const BYTE bNodeAddress = (BYTE) wParam & 0xFF;
	m_fWriteBindingsBusy=TRUE;
	return TRUE;
}

LONG CMainFrame::OnBindingsNodeWriteDone(WPARAM wParam,LPARAM lParam)
{
	const BYTE bNodeAddress = (BYTE) wParam & 0xFF;
	return TRUE;
}

LONG CMainFrame::OnBindingsNodeWriteError(WPARAM wParam,LPARAM lParam)
{
	TMGetGlobalHSM()->EvSignalTaskError();

	const BYTE bNodeAddress = (BYTE) wParam & 0xFF;

	// TM 20081222: De mouse cursor veranderen terwijl de UI thread bezig is.
	HCURSOR oldCursur = SetCursor(LoadCursor(NULL, IDC_WAIT));		
	TMGetBindingApp()->SetAllBindingsSendFailed(bNodeAddress);	
	TMGetViews()->OnBindingdatabaseUpdate();
	SetCursor( oldCursur );

	m_fWriteBindingsBusy=FALSE;

	if (1)
	{
		CString szMessage;
		szMessage.Format(_T("Error - Bindings not written to node %02x !"),	bNodeAddress);
		DisplayScrollMessage(szMessage);
	}
	return TRUE;
}

LONG CMainFrame::OnBindingsNodeReadStart(WPARAM wParam,LPARAM lParam) 
{
	const BYTE bNodeAddress = (BYTE) wParam & 0xFF;
	return TRUE;
}

LONG CMainFrame::OnBindingsNodeReadDone(WPARAM wParam,LPARAM lParam) 
{
	const BYTE bNodeAddress = (BYTE) wParam & 0xFF;
	// TM 20060511
	TMGetGlobalHSM()->EvSignalTaskDone();
	return TRUE;
}


LONG CMainFrame::OnBindingsNodeReadError(WPARAM wParam,LPARAM lParam) 
{
	const BYTE bNodeAddress = (BYTE) wParam & 0xFF;		
	// TM 20060511
	TMGetGlobalHSM()->EvSignalTaskError();
	return TRUE;
}

LONG CMainFrame::OnBindingsNodeFSMWriteDone(WPARAM wParam,LPARAM lParam) 
{	
	TMGetGlobalHSM()->EvSignalTaskDone();
	
	HCURSOR oldCursur = SetCursor(LoadCursor(NULL, IDC_WAIT));	
	TMGetBindingApp()->SetAllSend();	
	TMGetViews()->OnBindingdatabaseUpdate();
	SetCursor(oldCursur);

	m_fWriteBindingsBusy=FALSE;
	return TRUE;
}

LONG CMainFrame::OnBindingsNodeFSMReadDone(WPARAM wParam,LPARAM lParam) 
{
	TMGetGlobalHSM()->EvSignalTaskDone();
	return TRUE;
}

/**
 * Events from Generic Application Tasks 
 */
LONG CMainFrame::OnWmGenericTaskDone(WPARAM wParam,LPARAM lParam)
{
	TMGetGlobalHSM()->EvSignalTaskDone();		
	return TRUE;
}
LONG CMainFrame::OnWmGenericTaskError(WPARAM wParam,LPARAM lParam)
{
	TMGetGlobalHSM()->EvSignalTaskError();		
	return TRUE;
}
LONG CMainFrame::OnWmGenericTaskCancelled(WPARAM wParam,LPARAM lParam)
{
	TMGetGlobalHSM()->EvSignalTaskCancel();		
	return TRUE;
}



/**
 * Events from CBrowsePnpEngine ...
 */
LONG CMainFrame::OnBrowsePnpPnpOK(WPARAM wParam,LPARAM lParam)
{		
	CAppStateUtil::setAppBrowseError(CAppState::getInstance(),CAppBrowseError::BROWSE_EC_SUCCESS);
		
	CString szMessage("PnP algorithm done."
					  "All nodes have successfull been assigned a logical address.");
	DisplayScrollMessage(szMessage);					

	TMGetGlobalHSM()->EvSignalTaskDone();
	return TRUE;
}

LONG CMainFrame::OnBrowsePnpBrowseOk(WPARAM wParam,LPARAM lParam)
{
	CAppStateUtil::setAppBrowseError(CAppState::getInstance(),CAppBrowseError::BROWSE_EC_SUCCESS);

	CString szMessage("Browse for nodes done...");
	DisplayScrollMessage(szMessage);

	TMGetGlobalHSM()->EvSignalTaskDone();
	return TRUE;
}

LONG CMainFrame::OnBrowsePnpError(WPARAM wParam,LPARAM lParam)
{
	CAppStateUtil::setAppBrowseError(CAppState::getInstance(),CAppBrowseError::BROWSE_EC_ERROR);

	CString szMessage("Browse/Pnp error...");
	DisplayScrollMessage(szMessage);

	TMGetGlobalHSM()->EvSignalTaskError();
	return TRUE;
}

LONG CMainFrame::OnBrowsePnpCancel(WPARAM wParam,LPARAM lParam)
{
	CAppStateUtil::setAppBrowseError(CAppState::getInstance(),CAppBrowseError::BROWSE_EC_CANCEL);

	CString szMessage("Browse/Pnp cancelled by user...");
	DisplayScrollMessage(szMessage);

	TMGetGlobalHSM()->EvSignalTaskCancel();
	return TRUE;
}

LONG CMainFrame::OnBrowsePnpNoResponse(WPARAM wParam,LPARAM lParam)
{			
	CAppStateUtil::setAppBrowseError(CAppState::getInstance(),CAppBrowseError::BROWSE_EC_NO_RESPONSE);

	CString szMessage("Browse/Pnp stopped - node not responding");
	DisplayScrollMessage(szMessage);
	
	TMGetGlobalHSM()->EvSignalTaskError();
	return TRUE;
}

LONG CMainFrame::OnBrowsePnpDuplicates(WPARAM wParam,LPARAM lParam)
{
	CAppStateUtil::setAppBrowseError(CAppState::getInstance(),CAppBrowseError::BROWSE_EC_DUPLICATES_FOUND);

	CString szMessage("Browse/Pnp stopped - duplicate addresses found");
	DisplayScrollMessage(szMessage);

	TMGetGlobalHSM()->EvSignalTaskError();
	return TRUE;
}

// Sedert V16.44:
// Wanneer er factory addressen gevonden werden bij de PNP controleren of er reeds een master aanwezig is in het systeem.
// Indien nee:
//	- Touchscreens aanwezig: Voorkeur deze instellen als master.
//  - Smartbox ultra aanwezig: Via de wizard instellen.
//
// Oppassen:
//	- Wanneer er geen master aanwezig is wordt er een messagebox getoond met een melding.
//    Deze kunnen we niet in CMainFrame toepassen. Dit kan enkel in de CPropertyWizard klasse.
//    Reden: QhsmFsm.
LONG CMainFrame::OnBrowsePnpFactoryAddress(WPARAM wParam,LPARAM lParam) {
	CAppBrowseError::BrowseError browseError = CAppBrowseError::BROWSE_EC_FACTORY_ADDRESSES_FOUND;
	CAppNodeDatabase* const pAppNodeDatabase = CAppNodeDatabase::GetInstance();
	if (NULL != pAppNodeDatabase) {
		// In de runtime node database controleren of er een master aanwezig is.
		if (!pAppNodeDatabase->runtimeNodeDatabase_isMasterPresent()) {			
			// Geen master aanwezig...
			CList<int> list;
			if (pAppNodeDatabase->runtimeNodeDatabase_foundTouchscreenNodesWithFactoryAddress(list)) {
				// Er werden touchscreens gevonden met een 0xFE. Voorstellen om deze eerst als master in te stellen...						
			}
			else if (pAppNodeDatabase->runtimeNodeDatabase_foundSmartboxUltraNodesWithFactoryAddresses(list)) {			
				// Er werden smartbox ultra nodes gevonden met een 0xFE adres.
				// Hier de browse error aanpassen: Deze zorgt ervoor dat er een extra optie bijkomt.
				browseError = CAppBrowseError::BROWSE_EC_FACTORY_ADDRESSES_FOUND_ASSIGN_MASTER;
			}
		}		
	}

	// Doorgeven van de browse error.
	// Standaard fout is BROWSE_EC_FACTORY_ADDRESSES_FOUND, Wanneer we een master kunnen toekennen via de software wordt dit BROWSE_EC_FACTORY_ADDRESSES_FOUND_ASSIGN_MASTER
	CAppStateUtil::setAppBrowseError(CAppState::getInstance(),browseError);

	CString szMessage("Browse/Pnp stopped - factory addresses found. Proceed via the Problem-Solving wizard.");
	DisplayScrollMessage(szMessage);
	
	// TODO: DOORGEVEN AAN DE GLOBAL FSM DAT DE TAAK DIALOOG NIET MOET GETOOND WORDEN OMDAT ER EEN POP-UP MELDING KOMT...
	TMGetGlobalHSM()->EvSignalTaskError();
	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers
BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{	
	const CAppState::Type_t appType = CAppState::getInstance().getAppType();
	if (CAppState::APP_TYPE_INSTALLATION==appType)
	{
		return (TMGetViews()->CreateMainSplitter(this,pContext));	
	}
	else
	{
		const BOOL fResult = CFrameWnd::OnCreateClient(lpcs,pContext);
		ASSERT(fResult);
		return fResult;
	}
	
	return TRUE;
}

// Sedert V16.40: Wanneer er een naam aangepast wordt door de gebruiker de modified vlag zetten.
// Dit zorgt ervoor dat bij het afsluiten gevraagd wordt om het bestand op te slaan.
LONG CMainFrame::OnNodeNameChanged(WPARAM wParam,LPARAM lParam) {	
	ASSERT(TMGetCommunicationApp());	
	const BYTE bNodeAddress = (BYTE) ((wParam & 0xFF00)>>8);
	CString szMessage(reinterpret_cast<const char*>(lParam));	
	TMGetDocumentNodes()->ChangeNodeName(bNodeAddress,szMessage);
	TMGetCommunicationApp()->RenameNode(bNodeAddress);
	this->SetModifiedFlag();	// V16.40
	return (TMGetViews()->OnNodeNameChanged(wParam,lParam));
}

// Sedert V16.40: Wanneer er een naam aangepast wordt door de gebruiker de modified vlag zetten.
// Dit zorgt ervoor dat bij het afsluiten gevraagd wordt om het bestand op te slaan.
LONG CMainFrame::OnUnitNameChanged(WPARAM wParam,LPARAM lParam) {	
	ASSERT(TMGetCommunicationApp());	
	const BYTE bNodeAddress = (BYTE) ((wParam & 0xFF00)>>8);
	const BYTE bUnitAddress = (BYTE) (wParam & 0xFF);
	CString szMessage(reinterpret_cast<const char*>(lParam));	
	TMGetDocumentNodes()->ChangeUnitName(bNodeAddress, bUnitAddress, szMessage);
	TMGetCommunicationApp()->RenameUnit(bNodeAddress,bUnitAddress);		
	this->SetModifiedFlag();	// V16.40
	return(TMGetViews()->OnUnitNameChanged(wParam,lParam));
}


//////////////////////////////////////////////////////////////////////////
// TASK =	Show Node Properties Dlg ...
//			1) Find Node in Document nodedatabase 
//			2) Create Modeless CPropertyNodesSheet 
//
// INVOKED BY : CNodeUnitListCtrl::OnRButtonDblClk
//
// ARGUMENTS : WPARAM holds NodeAddress
//////////////////////////////////////////////////////////////////////////

#if(0)
static void setNodePropertiesActivePage(CPropertyNodesSheet* pSheet,LPARAM lParam) {
	if (NULL == pSheet) {
		return;
	}
	switch(lParam) {
		case NODE_PROPERTIES_ACTIVEPAGE_DEFAULT: { /* absorbe */ } break;
		case NODE_PROPERTIES_ACTIVEPAGE_RESOURCES: {
			pSheet->setActivePageNodeResources();										   
		} break;
		case NODE_PROPERTIES_ACTIVEPAGE_CONFIG: {
			pSheet->setActivePageNodeConfig();												
		} break;
		case NODE_PROPERTIES_ACTIVEPAGE_SCREENMAP: {
			pSheet->setActivePageNodeScreenmap();										   
		} break;
		case NODE_PROPERTIES_ACTIVEPAGE_CONTROL: {
			pSheet->setActivePageNodeControl();										 
		} break;
	}
}
#endif 

// BUG-0169 (zie ook BUG-0176)
//
// Sedert V16.14: Soms een probleem dat de dialoog niet opent.
// Aanpassingen:
//	- (1) Wanneer er een instantie aangemaakt werd controleren of HWND (=create) gelukt is.
//  - (2) Wanneer de create( ) mislukt dan de instantie verwijderen.
//
// Aanpassingen: BUG_0169_CHANGES_PROPERTY_NODESSHEET
// Sedert V16.17: Nog altijd een probleem dat de dialoog opent en daarna korstondig opnieuw sluit.
//  - (3) Opnieuw proberen om de dialoog te openen wanneer het mislukt.
//		  Via een retry counter beperken dat de postMessage( ) niet oneindig aangeroepen wordt.
//  - (4) Wanneer we dialoog verwijderen omdat deze van een andere node is dan via een postMessage( )
//		  opnieuw de create aanroepen. Vroeger gebeurde de create( ) ook in deze dialoog...
//
// Sedert V16.28:
// - Logging.
// - (5) Controle doen op SetActiveWindow()
//	     Indien deze functie mislukt (omdat de applicatie niet de actieve window was) dan zal de 
//		 de Destroy( ) aangeroepen worden en via PostMessage() opnieuw aangeroepen worden om getoond 
//		 te worden.
//
// BUG-0176
// Wanneer deze aangeroepen werd vanuit de treeview dan werd de property dialoog soms getoond met een
// verkeerde node (vorige node).
// Mogelijk ook een probleem BUG-0176?
// 
LONG CMainFrame::OnShowNodeProperties(WPARAM wParam,LPARAM lParam)
{	
	const BYTE bLogicalNodeAddress = (BYTE) (wParam & 0xFF);	
	CCanNode *pNode = NULL;	
	int i = 0;
	if (!TMGetDocumentNodes()->FindNode(bLogicalNodeAddress,&i,&pNode)) {
		CString msg;
		msg.Format("CPropertyNodeSheet failed to show because of unknown node address 0x%02x",bLogicalNodeAddress);
		TRACER_ERROR(msg);
		return TRUE;
	}

	static int retry = 0;	// (3)
	ASSERT(pNode);
	CPropertyNodesSheet* pPropertyPages = CPropertyNodesSheet_Holder::Get();
	if (NULL != pPropertyPages) {
		// Sedert V16.14: Controle doen of de dialoog aangemaakt werd. (1)
		if (NULL != pPropertyPages->GetSafeHwnd()) {
			CCanNode* const pAssignedNode = pPropertyPages->GetAssignedNode();
			ASSERT(pAssignedNode);				
			if ((pAssignedNode != NULL) && (pAssignedNode->bLogicalAddress == pNode->bLogicalAddress)) {													
				BOOL failed = FALSE;	// (5)					
				if (NULL == pPropertyPages->SetActiveWindow()) {		// (5)				
					CString msg;
					const DWORD dwError = GetLastError();
					msg.Format("CPropertyNodeSheet setActiveWindow() failed. Error=%d",dwError);
					TRACER_ERROR(msg);
					failed = TRUE;
				}								
				if (FALSE == failed) {	// (5)								
					pPropertyPages->setPage(static_cast<PropertyNodesSheetPage_t>(lParam));		
					retry = 0;	// (3)
					return TRUE;
				}					
			}	
		}
		else {
			TRACER_WARN("CPropertyNodeSheet instance has no valid handle.");
		}

		// Sedert V16.17: Opnieuw aanroepen via een postMessage... (4)		
		CPropertyNodesSheet_Holder::Destroy();
		retry = 0;	// (3)
		PostMessage(WM_SHOW_NODE_PROPERTIES,wParam,lParam);
		return TRUE;
	}

	// Create New Dialog 
	ASSERT(NULL == pPropertyPages);
	pPropertyPages = CPropertyNodesSheet_Holder::Create(this,pNode,static_cast<PropertyNodesSheetPage_t>(lParam));
	if (NULL != pPropertyPages) {
		pPropertyPages->ShowWindow(SW_SHOWNORMAL);				
		retry = 0;	// (3)
	}
	else {
		ASSERT(0);

		// Sedert V16.17: Opnieuw proberen... (3)
		if (++retry<=2) {			
			TRACER_WARN("CPropertyNodesSheet failed to create. Retrying...");
			PostMessage(WM_SHOW_NODE_PROPERTIES,wParam,lParam);
		}
		else {
			TRACER_WARN("CPropertyNodesSheet failed to create. All retries used.");
		}	
	}	
	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
// TASK =	TODO : Show Unit Properties Dlg ...
//			1) Find Node in Document nodedatabase 
//			2) Create Modeless Dialog
//
// INVOKED BY : CNodeUnitListCtrl::OnRButtonDblClk
//
// ARGUMENTS : WPARAM holds NodeAddress
//			   LPARAM holds UnitAddress
// 
// NOTE : NOT IMPLEMENTED ...
//////////////////////////////////////////////////////////////////////////
LONG CMainFrame::OnShowUnitProperties(WPARAM wParam,LPARAM lParam)
{
	return TRUE;
}


// Sedert V16.27: Dialoog tonen voor de UDP LAN configuratie.
// Wordt aangeroepen vanuit pop-up context menu van de 
//	- Node form view, 
/// - Node treeview 
//	- UDP LAN configurator dialog.
//
// Params: 
// - Wanneer deze functie aangeroepen wordt vanuit de nodeview & treeview dan is het device
//   aanwezig in de node database. In dit geval wordt het node address toegekend aan wParam.
//	 --> wParam = Node Address - lParam = 0xFFFFFFFF
// - Wanneer deze functie aangeroepen wordt door de UDP LAN Configuratie dialoog wordt een
//   ingekorte versie van het MAC-Address toegekend aan lParam (er is niet altijd een LA beschikaar.)
///	 --> wParam = 0xFF - lParam = MAC-Address.
// 
// - wParam = node address. (Tree & Node List view)
// - lParam = MAC-Address.	(UDP LAN Configuratie.)
//
// NOTE_OFFLINE:
//   Wanneer een device niet gevonden werd in de laatste scan wordt er een messagebox getoond.
LONG CMainFrame::OnShowNodeUDPLanCfg(WPARAM wParam,LPARAM lParam) {
	
	static CAppDlgUDPLanCfg_Host host(this->getAppUDPServerCfg(), this->getAppRemoteCfgNodeServices(), this->getAppRemoteCfgLoginInfo());

	BOOL showMsgOffline = FALSE;				// NOTE_OFFLINE
	BOOL showMsgNodeNotFoundInNwk = FALSE;
	if (0xFF != wParam) {
		ASSERT(0xFFFFFFFF == lParam);
		const BYTE bLogicalNodeAddress = (BYTE) (wParam & 0xFF);	
		CCanNode *pNode = NULL;	
		int i;
		if (TMGetDocumentNodes()->FindNode(bLogicalNodeAddress,&i,&pNode)) {
			ASSERT(pNode);			
			AppUDPDevices& refDeviceNodes = this->getAppUDPServerCfg().getDeviceList();	
			const AppUDPDevice* pDevice = NULL;
			if (refDeviceNodes.find((int) pNode->getPhysicalAddress(),&pDevice)) {
				ASSERT(NULL != pDevice);					
				if (pDevice->getLastFound()) {
					CDlgUDPLanConfig::Show(this, host,*pDevice);		
				}
				else {
					showMsgOffline = TRUE;
				}
			}
			else {
				showMsgNodeNotFoundInNwk = TRUE;
			}
		}
	}
	else if (0xFFFFFFFF != lParam) {	
		ASSERT(0xFF == wParam);
		AppUDPDevices& refDevices = this->getAppUDPServerCfg().getDeviceList();				
		const int count = refDevices.getCount();
		showMsgNodeNotFoundInNwk = TRUE;
		for (int i=0;i<count;++i) {
			AppUDPDevice* pDevice = NULL;
			if (refDevices.getAt(i,&pDevice)) {
				ASSERT(NULL != pDevice);
				if (lParam == AppUDPDevice_Utils::convertMACAddress2Uint32(*pDevice)) {									
					showMsgNodeNotFoundInNwk = FALSE;
					if (pDevice->getLastFound()) {
						CDlgUDPLanConfig::Show(this, host,*pDevice);					
					}
					else {
						showMsgOffline = TRUE;
					}
				}				
			}
		}	
	}

	if (showMsgOffline) {
		// zie NOTE_OFFLINE
		const CString msg("The device is currently offline and cannot be found on the Ethernet LAN.");						  
		this->MessageBox(msg,"Cannot configure - Device not found.",MB_OK | MB_ICONINFORMATION);
		return TRUE;
	}
	if (showMsgNodeNotFoundInNwk) {
		const CString msg("The device could not be detected on the Ethernet LAN. "
						  "Verify that the device is correctly connected to the network.");
		this->MessageBox(msg,"Cannot configure - Device not found.",MB_OK | MB_ICONINFORMATION);
		return TRUE;
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// TASK = Interface messges to CScrollView ...
//
// INVOKED BY: CMainFrame invokes ClearScrollMessage + DisplayScrollMessage...
//			   other SendSessage(WM_DISPLAY_MESSAGE) + (WM_CLEAR_MESSAGE)
//
// CAREFULLY :	Giving strings as arguments must always be in same thread
//				of execution.
//				Therefore we always use : SendMessage instead of PostMessage
//
//////////////////////////////////////////////////////////////////////////
void CMainFrame::ClearScrollView(void) 
{
	const CAppState::Type_t appType = CAppState::getInstance().getAppType();

	if (CAppState::APP_TYPE_INSTALLATION==appType)
	{
		TMGetViews()->OnClearMessages();
	}
	else if (CAppState::APP_TYPE_ENGINEERING==appType)
	{
		CDlgEngineeringRS232::ClearInfo();
	}
}

void CMainFrame::DisplayScrollMessage(const CString& szMessage) 
{
	const CAppState::Type_t appType = CAppState::getInstance().getAppType();
	if (CAppState::APP_TYPE_INSTALLATION==appType) {		
		TMGetViews()->OnDisplayMessage((WPARAM)((LPCSTR)(szMessage)));
	}
	else if (CAppState::APP_TYPE_ENGINEERING==appType) {	
		CDlgEngineeringRS232::AddInfo(szMessage);
	}
}

LONG CMainFrame::OnDisplayMessage(WPARAM wParam,LPARAM lParam) {
	const CString szMessage(reinterpret_cast<const char*>(wParam));
	this->DisplayScrollMessage(szMessage);
	return TRUE;
}

LONG CMainFrame::OnClearMessages(WPARAM wParam,LPARAM lParam) {
	this->ClearScrollView();
	return TRUE;
}

LONG CMainFrame::OnDisplayTracerMessageError(WPARAM wParam,LPARAM lParam) {
	const CString szMessage(reinterpret_cast<const char*>(wParam));
	this->AddToErrorTracer(szMessage);
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////
LONG CMainFrame::OnUnitStatusChanged(WPARAM wParam,LPARAM lParam)
{
	const CAppState::Type_t appType = CAppState::getInstance().getAppType();

	if (CAppState::APP_TYPE_INSTALLATION==appType)
	{
		TMGetViews()->OnUnitStatusChanged(wParam,lParam);
		CDlgUnitProp::OnUnitStatusChanged((BYTE) wParam & 0xFF, (BYTE) lParam & 0xFF);
		CDlgBindingUnitProp::OnUnitStatusChanged((BYTE) wParam & 0xFF, (BYTE) lParam & 0xFF);
		return TRUE;
	}
	return TRUE;
}



///////////////////////////////////////////////////////////////////////////
// Task = Copy Specific runtime statussen to Doc. Nodes ...
//
// INVOKED BY : Message send by CComApplication::ParseNodeObjectRxMessage		  
//
// Carefully :	1) Must only be done after Doc. Database is initialised ...
//				2) Can Only be done if Application Type = IDLE or when busy
//				   Writing bindings ...
//
// Forward Messages To :	1) CMyFrameWnd --> NodesForm
//							2) CNodePropertySheets
//
///////////////////////////////////////////////////////////////////////////

// Sedert V16.32: Wanneer de document node upgedate werd door de runtime node dan moet de modified vlag gezet worden.
//				  De modified vlag zorgt ervoor dat bij het afsluiten er gevraagd wordt om de gegevens op te slaan.
//
// Sedert V16.32: Oplossing BUG-0175:
//				  De Applicatie bleef in de toestand IDLE staan bij het doorsturen van de bindings.
//				  Aangepast naar APP_STATE_BUSY_BINDINGS_WRITE en APP_STATE_BUSY_CONFIGURATIONFILE_WRITE
//				  Wanneer er vanuit offline mode naar online mode gegaan wordt dan staat de applicatie in de mode 'APP_CONTINUE_ONLINE'
//				  en niet in de toestand 'APP_IDLE'.
//				  Hierdoor werd de database node niet upgedate met de runtime specs (XOR, NrOfBinding entries) van de netwerk node.
LONG CMainFrame::OnNodeStatusChanged(WPARAM wParam,LPARAM lParam)
{	
	const CAppState& refAppState = CAppState::getInstance();
	const CAppState::Type_t appType = refAppState.getAppType();

	if (CAppState::APP_TYPE_INSTALLATION==appType) {		
		const CAppState::States_t state = refAppState.getAppState();				// Zie BUG-0175
		if ((CAppStateUtil::getAppMode(refAppState) == CAppMode::APP_IDLE) ||
			(state == CAppState::APP_STATE_BUSY_BINDINGS_WRITE) || (state == CAppState::APP_STATE_BUSY_CONFIGURATIONFILE_WRITE)) 
		{	
			const BYTE bNodeAddress = wParam & 0xFF;

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// When changing to CAppNodeDatabase catch the exceptions --> CAN HAPPEN
	// That a status is received from a node currently not in the database !!!!
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		
			BOOL statusNodeChanged = FALSE;				// Sedert V16.32: Bij verandering de modified vlag zetten.
			const BOOL fResult = TMGetNodeDatabaseApp()->UpdateNodeWithRTSpecs(bNodeAddress,&statusNodeChanged);
			if (fResult == TRUE) {			
				TMGetViews()->OnNodeStatusChanged(wParam,lParam);
				if (CPropertyNodesSheet_Holder::Get() != 0) {
					CPropertyNodesSheet_Holder::Get()->SendMessage(WM_NODE_STATUS_CHANGED,wParam,lParam);	
				}				
				CDlgNetworkCanStatus::Update(bNodeAddress);

				if (statusNodeChanged) {
					this->SetModifiedFlag();			// Sedert V16.32
				}
			}
			ASSERT( fResult );
		}
	}
	else if (CAppState::APP_TYPE_ENGINEERING==appType)
	{
		BOOL statusNodeChanged = FALSE;
		const BYTE bNodeAddress = wParam & 0xFF;	
		TMGetNodeDatabaseApp()->UpdateNodeWithRTSpecs(bNodeAddress,&statusNodeChanged);
	
	}
	return TRUE;
}


// Node network status veranderd.
// wParam = physical node address - dit kan ook 0xffffff zijn.
LONG CMainFrame::onNodeEthernetNwkStatus(WPARAM wParam,LPARAM lParam) {
	const CAppState& refAppState = CAppState::getInstance();
	const CAppState::Type_t appType = refAppState.getAppType();
	if (CAppState::APP_TYPE_INSTALLATION==appType) {		
		TMGetViews()->onNodeEthernetNwkStatus(wParam,lParam);		
	}
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////
LONG CMainFrame::OnTreeNodeCollapsed(WPARAM wParam,LPARAM lParam) 
{
	const CAppState::Type_t appType = CAppState::getInstance().getAppType();
	if (CAppState::APP_TYPE_INSTALLATION==appType)
	{
		return(TMGetViews()->OnTreeNodeCollapsed(wParam,lParam));
	}
	return TRUE;
}

LONG CMainFrame::OnTreeNodeExpanded(WPARAM wParam,LPARAM lParam) 
{
	const CAppState::Type_t appType = CAppState::getInstance().getAppType();
	if (CAppState::APP_TYPE_INSTALLATION==appType)
	{
		return(TMGetViews()->OnTreeNodeExpanded(wParam,lParam));
	}
	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
// Communication Notifications

LONG CMainFrame::OnCommunicationMessageReceived(WPARAM wParam,LPARAM lParam)
{
	const CCommunicationAppFrame& frame = TMCommunicationGetStatus().getLastRxFrame();
	const CCommunicationAppFrame::Types type = frame.GetType();
	if (CCommunicationAppFrame::_MESSAGE_RECEIVED == type) {		
		const CString tempMsg = frame.toString();	
		if (!tempMsg.IsEmpty()) {
			CString msg;
			msg.Format("RX: %s",frame.toString());		
			AddToCommunicationTracer(msg);
		}
		m_CommStatusIndicator = STATUS_DATA_RECIEVED;
	}
	if (CCommunicationAppFrame::_MESSAGE_LOG == type) {
		const CString tempMsg = frame.toString();	
		if (!tempMsg.IsEmpty()) {			
			const CString msg = frame.toString();
			AddToEngineerCanLogger(msg);
		}
	}
	else {
		ASSERT(CCommunicationAppFrame::_MESSAGE_RECEIVED == type);
	}
	return TRUE;
}

LONG CMainFrame::OnCommunicationMessageSend(WPARAM wParam,LPARAM lParam)
{			
	const CCommunicationAppFrame& frame = TMCommunicationGetStatus().getLastTxFrame();
	CString tempMsg = frame.toString();	
	if (!tempMsg.IsEmpty()) {
		CString msg;
		msg.Format("TX: %s",frame.toString());		
		AddToCommunicationTracer(msg);

		// Bij de tcp/ip communicatie wordt er een indicatie gegeven bij het versturen van een 
		// bericht omdat er geen ACK ontvangen wordt.
		if (CCommunicationServerInfo::TCPIP == TMCommunicationGetStatus().getServerInfo().getCommunicationType()) {
			TMGetGlobalHSM()->EvSignalCommunicationNotifyAck();
		}
	}

	m_CommStatusIndicator = STATUS_DATA_SEND;

	return TRUE;
}

LONG CMainFrame::OnCommunicationCommandReceived(WPARAM wParam,LPARAM lParam)
{	
	const CCommunicationAppFrame& frame = TMCommunicationGetStatus().getLastRxFrame();
	const CCommunicationAppFrame::Types type = frame.GetType();
	if (type == CCommunicationAppFrame::_COMMAND_RECEIVED_ACK) {			
		TMGetGlobalHSM()->EvSignalCommunicationNotifyAck();			// MOET DEZE HIER OF KAN DEZE NOG OP EEN ANDERE PLAATS ???		
		m_CommStatusIndicator = STATUS_RX_ACK;
	}
	else if (type == CCommunicationAppFrame::_COMMAND_RECEIVED) {
		CDlgModemStatus::Update();	
	}

	const CString tempMsg = frame.toString();	
	if (!tempMsg.IsEmpty()) {
		CString msg;
		msg.Format("RX: %s",frame.toString());		
		AddToCommunicationTracer(msg);
	}
	else {
		ASSERT(!tempMsg.IsEmpty());
	}
	return TRUE;
}

LONG CMainFrame::OnCommunicationError(WPARAM wParam,LPARAM lParam)
{
	const CSerialCanModemErrorStatus* const pErrorStatus = TMGetModemErrorStatus();
	if (pErrorStatus != NULL){
		const CSerialCanModemErrorStatus::ENUM_MODEM_ERROR_TYPE type = pErrorStatus->GetType();
		switch (type) {
			case CSerialCanModemErrorStatus::MODEM_ERROR_MAX_ACK_RETRIES:
			{
				DisplayScrollMessage("Communication error: Modem not responding-Max ack retries");	

				const CString msg("ERROR: Modem maximum retries!");
				AddToCommunicationTracer(msg);	
				AddToErrorTracer(msg);
			}break;
			case CSerialCanModemErrorStatus::MODEM_ERROR_BUFFERS_FULL:
			{
				DisplayScrollMessage("Communication error: Modem error buffers full");				
			
				const CString msg("ERROR: Modem buffers full!");
				AddToCommunicationTracer(msg);	
				AddToErrorTracer(msg);
			}break;
			case CSerialCanModemErrorStatus::MODEM_ERROR_NO_RESPONSE:
			{
				DisplayScrollMessage("Communication error: Modem not responding");					
			
				const CString msg("ERROR: Modem no response!");
				AddToCommunicationTracer(msg);	
				AddToErrorTracer(msg);
			}break;
			case CSerialCanModemErrorStatus::MODEM_ERROR_RECEIVED_BAD_CHECKSUM:
			{
				DisplayScrollMessage("Communication error: Modem error received bad checksum");
				
				const CString msg("ERROR: Bad checksum received!");
				AddToCommunicationTracer(msg);	
				AddToErrorTracer(msg);
			}break;								
		} /* end switch (pErrorStatus->GetType())*/			

		m_CommStatusIndicator = STATUS_ERROR;

	}/* if (pErrorStatus != NULL) */					
	return TRUE;
}


///////////////////// ON_UPDATE_MESSAGES-CStatusBar //////////////////////

// Sedert versie 0x0800: Optimalisatie.
void CMainFrame::OnUpdateIndicatorConnected(CCmdUI *pCmdUI)
{	
	if (NULL == pCmdUI) {
		return;
	}

	CStatusBar* const pStatusBar = ((CStatusBar*) pCmdUI->m_pOther);
	pCmdUI->Enable(TRUE);
	if (pStatusBar != NULL) {	
		HICON hIcon = NULL;
		if (m_CommStatusIndicator == STATUS_OFFLINE) {
			hIcon=TMGetGraphics()->gIconDisConnected;
		}
		else {
			hIcon=TMGetGraphics()->gIconConnected;
		}	
		if (NULL != hIcon) {
			pStatusBar->GetStatusBarCtrl().SetIcon(1,hIcon);
		}
	}
}

// Sedert versie 0x0800: Optimalisatie.

void CMainFrame::OnUpdateIndicatorCommunication(CCmdUI *pCmdUI)
{	
	if (NULL == pCmdUI) {
		return;
	}
	CStatusBar* const pStatusBar = ((CStatusBar*) pCmdUI->m_pOther);
	pCmdUI->Enable(FALSE);
	if (pStatusBar != NULL) {		
		HICON hIcon = NULL;		
		switch(m_CommStatusIndicator) {
			case STATUS_OFFLINE:
				hIcon = TMGetGraphics()->gIconCommDisconnected;		
			break;
			case STATUS_ERROR:
				hIcon = TMGetGraphics()->gIconCommError;
			break;
			case STATUS_DATA_RECIEVED:
				hIcon = TMGetGraphics()->gIconCommReply;
			break;
			case STATUS_DATA_SEND:
				hIcon = TMGetGraphics()->gIconCommSend;
			break;
			case STATUS_RX_ACK:
				hIcon = TMGetGraphics()->gIconCommACK;
			break;			
		}						
		if (NULL != hIcon) {
			pStatusBar->GetStatusBarCtrl().SetIcon(3,hIcon);
		}
	} // if (pStatusBar != NULL)
}

/************************************************************************************************/
// Na het uitvoeren van de browse for nodes (request for states)
void CMainFrame::CompareNodeDatabases(void) {
	CAppState& refAppState = CAppState::getInstance();
	try {
		const CAppDbasePostBrowseError::Result result = TMGetNodeDatabaseApp()->CompareNodeDatabases();
		CAppStateUtil::setAppDbasePostBrowseErrorResult(refAppState,result);
	}
	catch(CMyException *e) {
		ASSERT(0);
		AfxMessageBox(e->GetError(),MB_ICONSTOP);
		CAppStateUtil::setAppDbasePostBrowseErrorResult(refAppState,CAppDbasePostBrowseError::DBASE_EC_NODES_INCOMPATIBLE);
	}
}

// Na het uitvoeren van de browse for single node.
void CMainFrame::CompareNodes(int nodeAddress) {
	CAppState& refAppState = CAppState::getInstance();
	try {
		const CAppDbasePostBrowseError::Result result = TMGetNodeDatabaseApp()->CompareNode(nodeAddress);
		CAppStateUtil::setAppDbasePostBrowseErrorResult(refAppState,result);
	}
	catch(CMyException *e) {
		ASSERT(0);
		AfxMessageBox(e->GetError(),MB_ICONSTOP);
		CAppStateUtil::setAppDbasePostBrowseErrorResult(refAppState,CAppDbasePostBrowseError::DBASE_EC_NODES_INCOMPATIBLE);
	}
}		
/************************************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// TASK = Show RuntimeDatabase Browse Info ...
// INVOKED BY : CMainFrame::FSMApplication
///////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::ShowBrowseInfo(void)
{			
	int iNrOfNodes;	

	if (TMGetRuntimeNodes()->NROfNodes(&iNrOfNodes)) 
	{
		CString szMessage;	
		szMessage.Format("Installation contains %02d nodes",iNrOfNodes);	
		DisplayScrollMessage(szMessage);
	}

	if (iNrOfNodes != 0) 
	{	
		CNodeDatabaseOperations NodeDatabaseOperations(TMGetRuntimeNodes());
		
		if (NodeDatabaseOperations.GetNROfNodesType(NODE_PIN_STD_UNITS,&iNrOfNodes)) 
		{
			CString szMessage;	
			szMessage.Format("%02d standard nodes",iNrOfNodes);
			DisplayScrollMessage(szMessage);
		}

		if (NodeDatabaseOperations.GetNROfNodesType(NODE_LCD,&iNrOfNodes)) 
		{
			CString szMessage;	
			szMessage.Format("%02d LCD nodes",iNrOfNodes);		
			DisplayScrollMessage(szMessage);
		}

		if (NodeDatabaseOperations.GetNROfNodesType(NODE_PIN_GATEWAY,&iNrOfNodes)) 
		{
			if (iNrOfNodes > 0)
			{
				CString szMessage;	
				szMessage.Format("%02d gateway nodes",iNrOfNodes);
				DisplayScrollMessage(szMessage);
			}
		}
	}
	return TRUE;
}


// Controle op Winsock dll 2.2
static BOOL checkVersionWinsocket( ) {
	WSADATA wsa;	
	if (WSAStartup(MAKEWORD(2,2),&wsa) != 0) {
		//TRACE(("Failed. Error Code : %d",WSAGetLastError()));        
		return FALSE;
	}
	if (LOBYTE(wsa.wVersion) != 2 || HIBYTE(wsa.wVersion) != 2) {
		TRACE(("Wrong version\r\n"));
		WSACleanup( );
		return FALSE;
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnTimer(UINT nIDEvent) 
{
	switch(nIDEvent)
	{
		case TIMERID_MAINFRAME_PULSTOGGLE:			
		{		
			TMGetGuiUnit()->OnTimerExpired();
		} break;

		case TIMERID_MAINFRAME_APPSTART:
		{
			switch(m_stateStartApp) {
				case START_APP_CREATE: {			


					{
						CString msg;
						msg.Format("Application started version V%02d.%02d",(CURRENT_SW_VERSION & 0xFF00) >> 8, (CURRENT_SW_VERSION & 0x00FF));																		    
						AppLogging::getInstance().addInfo(msg);
					}

					CSplashWnd::ShowSplashScreen(this);

					const CAppState::Type_t appType = CAppState::getInstance().getAppType();
					if (CAppState::APP_TYPE_INSTALLATION==appType) {			
						
						this->KillTimer(TIMERID_MAINFRAME_APPSTART);

						// Controle doen op de win32 socket versie.
						// Moeten versie V2.2 hebben.
						if (!checkVersionWinsocket()) {
							MessageBox("Winsocket V2.2 is required\n" 
									   "Ws2_32.dll is missing or cannot be found.\n"
									   "Cannot continue.",
									   NULL, MB_ICONERROR | MB_OK);
							this->DestroyWindow();
							return;
						}

						m_stateStartApp = START_APP_UPDATE_CHECK;
						this->SetTimer(TIMERID_MAINFRAME_APPSTART,250,(TIMERPROC) NULL);						
					}
					else {
						m_stateStartApp = START_APP_GUI;
						this->SetTimer(TIMERID_MAINFRAME_APPSTART,750,(TIMERPROC) NULL);
					}									
				} break;				

				case START_APP_UPDATE_CHECK:
				{
					this->KillTimer(TIMERID_MAINFRAME_APPSTART);

					AppIPCAdvInstallerUpdater advInstallerUpdater;
					if (AppIPCAdvInstallerUpdater_Util::checkUpdatesAtStartup(this,advInstallerUpdater)) {
						m_stateStartApp = START_APP_UPDATE_BUSY;
						this->SetTimer(TIMERID_MAINFRAME_APPSTART, 1000, (TIMERPROC)NULL);
					}
					else {
						m_stateStartApp = START_APP_GUI;
						this->SetTimer(TIMERID_MAINFRAME_APPSTART, 500, (TIMERPROC)NULL);
					}										
				} break;

				case START_APP_UPDATE_BUSY: {								
					// Afsluiten van de applicatie...					
					this->KillTimer(TIMERID_MAINFRAME_APPSTART);
					this->PostMessage(WM_CLOSE);														
				} break;

				case START_APP_GUI: {

					// Splashscreen verwijderen + GUI applicatie opstarten...
					CSplashWnd::Close();

					const CAppState::Type_t appType = CAppState::getInstance().getAppType();
					if (CAppState::APP_TYPE_INSTALLATION==appType) {
						TMGetViews()->InitViews();
					}

					PostMessage(WM_QHSM_INITIALISE);				
					PostMessage(WM_QFSM_CONVERTFROMNODES_INITIALISE);

					this->SetTimer(TIMERID_UDPCONFIG_SCAN,1000,(TIMERPROC) NULL);	
					this->KillTimer(TIMERID_MAINFRAME_APPSTART);
					m_stateStartApp = START_APP_DONE;

				} break;

				case START_APP_INIT:
				case START_APP_DONE: {
					ASSERT(0);			 
				} break;
			}
		} break;

		case TIMERID_CONVERTFROMNODES_NEXT:
		{
			CQHSMBindingConversionFromNodes::GetInstance()->EvAfterTimerNext();
		} break;

		case TIMERID_UDPCONFIG_SCAN: {
			this->KillTimer(TIMERID_UDPCONFIG_SCAN);	
			AppUDPServerCfg& ref = getAppUDPServerCfg();
			if (ref.getScanEnabled()) {
				ref.clearDeviceList( );
				ref.requestScan( );
			}
			this->SetTimer(TIMERID_UDPCONFIG_UPDATE,ref.getIntervalUpdateWaitMsec(),(TIMERPROC) NULL);
		} break;
		case TIMERID_UDPCONFIG_UPDATE: {
			this->KillTimer(TIMERID_UDPCONFIG_UPDATE);			
			AppUDPServerCfg& ref = getAppUDPServerCfg();
			if (ref.getScanEnabled()) {
				CList<AppUDPDevice*> listModified;
				if (ref.updateDeviceList(&listModified)) {					
					POSITION pos = listModified.GetHeadPosition();
					while(pos!=NULL)  {		
						const AppUDPDevice* const pDevice = listModified.GetNext(pos);
						if (NULL == pDevice) {
							ASSERT(NULL != pDevice);
							continue;		// continue while(...)
						}

						// Sedert V16.28:
						// Wannneer de netwerk gegevens van een UDP device veranderd zijn of er is een 
						// nieuw device gevonden deze updaten in de node services lijst.
						//
						// NOTE: Een node(service) wordt enkel in de lijst upgedate/toegevoegd wanneer de
						// node via het physical node address terug te vinden is in de nodedatabase.
						if (!m_flagUpdateNodeServices) {							
							ASSERT(START_APP_DONE == m_stateStartApp);
							if (START_APP_DONE == m_stateStartApp) {
								this->UpdateServerDeviceNodes(*pDevice);
							}														
						}

						// Sedert V16.28
						// Wannneer de netwerk gegevens van een UDP device veranderd zijn controleren van
						// de nwk gegevens van de DT18-PRO en deze updaten wanneer deze veranderd zijn...
						AppRemoteCfgLoginInfo_Util::updateLoginAfterUdpDeviceCfg(&this->getAppRemoteCfgLoginInfo(),
								const_cast<AppUDPDevice&>(*pDevice),AppRemoteCfgLoginInfo_Util::MASK_CONFIG_NWK);

						// Versturen van een bericht naar de applicatie.
						// wParam = physical node address (Kan ook 0xffffff zijn.)
						const int physicalNodeAddress = pDevice->getPhysicalAddress();
						this->PostMessage(WM_NODE_ETHERNETNWK_STATUS,physicalNodeAddress,0);
					}
				}


				// Sedert V16.28:
				// Alle devices overlopen en de node service lijst updaten.
				// Wordt vanuit de globale FSM aangeroepen m.b.v de m_flagUpdateNodeServices vlag.
				// Dit is wanneer de toestand in Offline / Online IDLE komt.
				// 
				// NOTE: Een node(service) wordt enkel in de lijst upgedate/toegevoegd wanneer de
				// node via het physical node address terug te vinden is in de nodedatabase.
				//
				// Is nodig zodat alle devices een juiste koppeling hebben logical node address,
				// Physical node address, MAC-Address en IP-adres.
				if (m_flagUpdateNodeServices) {					
					ASSERT(START_APP_DONE == m_stateStartApp);
					this->UpdateServerDeviceNodes();
					m_flagUpdateNodeServices = FALSE;
				}
		
				if (CDlgUDPLanDevices::IsActive()) {
					CDlgUDPLanDevices::getInstance()->PostMessage(WM_UPDATE_CONTROLS);
				}
			}
			this->SetTimer(TIMERID_UDPCONFIG_SCAN,ref.getIntervalScanMsec(),(TIMERPROC) NULL);
		} break;

		case TIMERID_MAINFRAME_APPDALI_POLL: {			
			using namespace DUOTECNO::DALI;
			CAppDaliManager& ref = CAppDaliManager::getInstance();
			CAppDaliManager::poll(ref, TIMER_MAINFRAME_APPDALI_POLL_INTERVAL_MSEC/1000);											 
		} break;
	}
	CFrameWnd::OnTimer(nIDEvent);
}

////////////////////////////////////////////////////////////////

LONG CMainFrame::OnShowDlgUnitProperties(WPARAM wParam,LPARAM lParam) {
	CCanNode *pNode=0;	
	int iEntryNR;
	const BYTE bNodeAddress = (BYTE) ((wParam & 0xFF00) >> 8);	
	if (TMGetDocumentNodes()->FindNode(bNodeAddress, &iEntryNR, &pNode)) {
		const BYTE bUnitAddress = (BYTE) (wParam & 0xFF);
		if (bUnitAddress == 0xFF) {			
			return(CDlgUnitProp::Destroy());
		}
		else {		
			STRUCT_UNIT *pUnit;			
			if (TMGetDocumentNodes()->FindUnit(pNode,bUnitAddress,&iEntryNR,&pUnit)) {		
				ASSERT(NULL != pNode);
				ASSERT(NULL != pUnit);
				const CString strLabelText = TMGetNodeDatabaseApp()->getUnitTextLabel(pNode->getNodeAddress(),bUnitAddress);				
				return(CDlgUnitProp::Show(pNode,pUnit,strLabelText));			
			}
		}
	}	
	return FALSE;
}


LONG CMainFrame::OnAddFlowAddUnit(WPARAM wParam,LPARAM lParam)
{
	CCanNode *pNode;	
	int iEntryNR;

	const BYTE bNodeAddress = (BYTE) (wParam & 0xFF);
	
	if (TMGetDocumentNodes()->FindNode(bNodeAddress,&iEntryNR,&pNode)) 
	{		
		STRUCT_UNIT *pUnit;
		const BYTE bUnitAddress = (BYTE) (lParam & 0xFF);

		if (TMGetDocumentNodes()->FindUnit(pNode, bUnitAddress, &iEntryNR, &pUnit)) 
		{				
			return (TMGetViews()->OnAddFlowAddUnit((WPARAM) pNode,(LPARAM) pUnit));	
		}
	}	
	
	ASSERT(0);
	return FALSE;
}


LONG CMainFrame::OnAddFlowDraggingUnit(WPARAM wParam,LPARAM lParam)
{	
	CCanNode *pNode;
	int iEntryNR;

	const BYTE bNodeAddress = (BYTE) ((wParam & 0xFF00)>>8);

	if (TMGetDocumentNodes()->FindNode(bNodeAddress, &iEntryNR, &pNode)) 
	{		
		STRUCT_UNIT *pUnit;
		const BYTE bUnitAddress = (BYTE) (wParam & 0xFF);

		if (TMGetDocumentNodes()->FindUnit(pNode, bUnitAddress, &iEntryNR, &pUnit))
		{	
			return(TMGetViews()->OnAddFlowDraggingUnit((WPARAM) pUnit,lParam));		
		}
	}	

	ASSERT(0);
	return FALSE;
}

LONG CMainFrame::OnAddFlowDropUnit(WPARAM wParam,LPARAM lParam)
{
	CCanNode *pNode;	
	int iEntryNR;

	const BYTE bNodeAddress = (BYTE) ((wParam & 0xFF00)>>8);
	
	if (TMGetDocumentNodes()->FindNode(bNodeAddress, &iEntryNR,&pNode)) 
	{		
		STRUCT_UNIT *pUnit;
		const BYTE bUnitAddress = (BYTE) (wParam & 0xFF);

		if (TMGetDocumentNodes()->FindUnit(pNode,bUnitAddress,&iEntryNR,&pUnit)) 
		{				
			TMGetViews()->OnAddFlowDropUnit((WPARAM) pUnit,lParam);
			return TRUE;
		}
	}	

	ASSERT(0);
	return FALSE;
}

LONG CMainFrame::OnAddFlowRemoveControl(WPARAM wParam,LPARAM lParam)
{
	return (TMGetViews()->OnAddFlowRemoveControl(wParam,lParam));
}

/////////////////////////////////////////////////////////////////////////////

void CMainFrame::OnDebugBindingconversion() 
{		
	CDlgDebugBindingConversion::Show();
}

// DEBUG
CBindingEntry* CMainFrame::GetBindingEntry(void)
{
	return((CBindingEntry*) TMGetViews()->GetBindingEntry());
}


////////////////////////////////////////////////////////////////////////
// TM 20040726 : Implementing automatic binding generation ...
LONG CMainFrame::OnBindingEntryAuto(WPARAM wParam,LPARAM lParam)
{
	CBindingEntry *const pCurrBindingEntry = GetBindingEntry();
	if (pCurrBindingEntry == NULL) {
		ASSERT(NULL != pCurrBindingEntry);
		return FALSE;
	}
		
	if (pCurrBindingEntry->GetBindingNr() == 0x0000)  {			
		BindingStatusError::BindingError_t bindingError = BindingStatusError::BINDING_SUCCESS;
		const BOOL result = TMGetBindingApp()->AddAuto(pCurrBindingEntry,&bindingError);										
		if (!result) {
			// TODO:
			// Kunnen hier eventueel controleren waarom de auto binding niet kon toegevoegd worden...
		}
		return result;
	}				
	return FALSE;
}


//
// Sedert V0x0800: CHANGES_BINDINGENTRYADD_FAILED
//
// BUG_0170_CHANGES_SOLVE (**) 
// Sedert V16.17: BUG-0170
// Controle doen waarom een binding niet kon toegevoegd worden..
// Indien een binding niet kan toegevoegd worden omdat het niet kan verwerkt worden (bvb units niet gelinked...)
// dan de binding niet verwijderen uit de GUI want deze blijft aanwezig in de bindingentries en in de bindingfiles.
// De binding wordt verwijderd uit de GUI views bij een database fout en wanneer deze niet kan 
// toegevoegd worden aan de node binding files.
// 
LONG CMainFrame::OnBindingEntryAdd(WPARAM wParam,LPARAM lParam)
{
	CBindingEntry *const pCurrBindingEntry = GetBindingEntry();
	if (NULL == pCurrBindingEntry) {			
		ASSERT(NULL != pCurrBindingEntry);
		return FALSE;

	}
		
	const unsigned short usCurrBindingEntryID = pCurrBindingEntry->GetID( );
	const unsigned short usBindingNr = pCurrBindingEntry->GetBindingNr();

	CString szEditName(TMGetViews()->GetEditControlBindingName());
	if (szEditName.IsEmpty() == FALSE) {
		const CString szBindingName(pCurrBindingEntry->GetName());		
		if (strcmp(szEditName,szBindingName)) {
			pCurrBindingEntry->ChangeName(szEditName);
		}
	}

	BindingStatusError::BindingError_t bindingError = BindingStatusError::BINDING_SUCCESS;
	const unsigned short nId = TMGetBindingApp()->Add(pCurrBindingEntry,&bindingError);
	if (nId == 0) {
		// indien 'nID == 0' --> Mislukt om een binding toe te voegen.
		if (usCurrBindingEntryID != 0) {
			// Indien 'usCurrBindingEntryID != 0' dan is het een bestaande binding die aangepast werd
					
			// BUG_0170_CHANGES_SOLVE (**) 
			if (BindingStatusError::BINDING_EC_DBASE_ENTRIES == bindingError ||
				BindingStatusError::BINDING_EC_BINDINGFILES == bindingError) 
			{					
				TMGetViews()->OnBindingEntryDelete((WPARAM) usCurrBindingEntryID,0);
			}
		}
		return FALSE;
	}
	return TRUE;		
}



LONG CMainFrame::OnBindingEntryDelete(WPARAM wParam,LPARAM lParam)
{
	CBindingEntry *const pCurrBindingEntry = GetBindingEntry();
	
	if (pCurrBindingEntry != NULL) 
	{
		const CString szName(pCurrBindingEntry->GetName());
		const unsigned short nID = pCurrBindingEntry->GetID();		
		const unsigned short usBindingNr = pCurrBindingEntry->GetBindingNr();

		if (nID != 0) 
		{			
			if (TMGetBindingApp()->Delete(pCurrBindingEntry))
			{
				CString szMessage;

				// Update GUI - ScrollView Messages ...
				ClearScrollView();

				szMessage.Format(_T("Deleted bindingnr 0x%04x, name '%s'"),usBindingNr,szName);
				DisplayScrollMessage(szMessage);

				szMessage.Format(_T("Ready for new Binding ..."));
				DisplayScrollMessage(szMessage);

				return (TMGetViews()->OnBindingEntryDelete((WPARAM) nID,lParam));			
			}		
		}
	}
	
	ASSERT(0);
	return FALSE;
}

LONG CMainFrame::OnBindingEntryEdit(WPARAM wParam,LPARAM lParam)
{
	if (TMGetViews()->IsBindingViewActive()) 
	{
		const unsigned short nID = (wParam & 0xFFFF);

		if (nID != 0) 
		{				
			SendMessage(WM_DESTROY_DIALOG_BINDINGUNIT);
		
			CBindingEntry *const pEdit = TMGetBindingApp()->Update(nID);
			
			if (pEdit != NULL) 
			{										
				DisplayScrollBindingInfo(pEdit);		// Display Info Of Binding ...
				
				return (TMGetViews()->OnBindingEntryEdit((WPARAM) pEdit,0));
			}
			else 
			{			
				if (AfxMessageBox("Unknown BindingEntry in Database|Would you like to remove?",
								   MB_ICONQUESTION | MB_YESNO) 
								   == IDYES) 
				{
					// zie BUG-0153 - TODO LOW_PRIO
					// Indien de CBindingDatabaseApplication::Update( ) mislukt en de binding entry 
					// is wel in de database dan wordt de binding enkel uit de GUI views gewist 
					// maar niet uit de binding Entry database

					// In principe moeten we hier de CBindingDatabaseApplication::Delete( )
					// uitvoeren...

					TMGetViews()->OnBindingEntryDelete((WPARAM) nID,lParam);	
				}			
			}				
		}
	}
	return TRUE;
}

/**
 * Date 20060322 Changed function 
 */
void CMainFrame::DisplayScrollBindingInfo(CBindingEntry *pEntry)
{
	ASSERT(pEntry);

	if (pEntry != NULL)
	{
		CExportGenerateBindingsTable BindingTable;
		BindingTable.Generate_BindingTable(pEntry);

		CString szMessage;
	
		ClearScrollView();

		int j;
		int nUnits=BindingTable.GetNrInputs();

		for (j=0;j<nUnits;j++) 
		{
			CString strName, strDescription, strID;

			if(!BindingTable.GetInputName(j,&strName)) ASSERT(0);		
			if(!BindingTable.GetInputDescription(j,&strDescription)) ASSERT(0);
			//if(!BindingTable.GetInputId(j,&strID)) ASSERT(0);

			szMessage.Format("IN: %s : %s",strName,strDescription);
			DisplayScrollMessage(szMessage);
		};
		
		nUnits=BindingTable.GetNrOutputs();

		for (j=0;j<nUnits;j++) 
		{
			CString strName, strDescription, strID;

			if(!BindingTable.GetOutputName(j,&strName)) ASSERT(0);		
			if(!BindingTable.GetOutputDescription(j,&strDescription)) ASSERT(0);
			//if(!BindingTable.GetOutputId(j,&strID)) ASSERT(0);

			szMessage.Format("OUT: %s : %s",strName,strDescription);
			DisplayScrollMessage(szMessage);
		};

		int nOperators = BindingTable.GetNrTimers();
		for(j=0;j<nOperators;j++) 
		{
			CString strType, strValue, strID;

			if(!BindingTable.GetTimerType(j,&strType)) ASSERT(0);
			if(!BindingTable.GetTimerValue(j,&strValue)) ASSERT(0);
			//if(!BindingTable.GetTimerID(j,&strID)) ASSERT(0);

			szMessage.Format("TIMERS: %s, %s",strType,strValue);
			DisplayScrollMessage(szMessage);
		}
	}
}

LONG CMainFrame::OnShowDlgBindingTimer(WPARAM wParam,LPARAM lParam)
{			
	CDlgBindingUnitProp::Destroy();
	CDlgBindingOperator::Destroy();

	CBindingEntry *const pCurrBindingEntry = GetBindingEntry();
	if (pCurrBindingEntry == NULL)  
	{
		AfxMessageBox("Fatal Error");
		return FALSE;
	}

	CBindingOperator *pBindingOperator = NULL;

	if (1)
	{	
		int i;
		const int nCount = pCurrBindingEntry->GetColOperator()->Count();
		const int nControlID = wParam & 0x0000FFFF;
		
		for (i=0;i<nCount;i++) 
		{
			if ((pBindingOperator = pCurrBindingEntry->GetColOperator()->Get(i)) == 0)
			{
				ASSERT(0);
				return 0;
			}

			if (pBindingOperator->GetID() == nControlID) 
			{
				break;
			}
		}

		if (i>=nCount) 
		{
			AfxMessageBox("Fatal Error");
			return FALSE;
		}	
	}

	CPoint TopLeft;
	TopLeft.x = (lParam & 0x7FFF0000) >> 16;
	TopLeft.y = (lParam & 0x7FFF);	
			
	return (CDlgBindingOperator::Show(pBindingOperator,TopLeft));
}

LONG CMainFrame::OnShowDlgBindingUnit(WPARAM wParam,LPARAM lParam)
{
	CDlgBindingUnitProp::Destroy();
	CDlgBindingOperator::Destroy();	
	
	CBindingEntry *const pCurrBindingEntry = GetBindingEntry();	
	if (pCurrBindingEntry == NULL) {
		AfxMessageBox("Fatal Error");
		return FALSE;
	}
	CBindingUnit *pBindingUnit = 0;
	if (1) {	
		const int nControlID = (wParam & 0xFFFF0000) >> 16;		
		const BYTE bNodeAddress = (wParam & 0xFF00) >> 8;
		const BYTE bUnitAddress = (wParam & 0x00FF);	
		CBindingUnits::Iterator it(pCurrBindingEntry->GetColUnit()->CreateIterator());
		for (it.Begin();!it.IsDone();it.Next()) {
			CBindingUnit* const pTemp = it.CurrentItem( );
			ASSERT( pTemp );
			if ((pTemp->GetID() == nControlID) &&
				(pTemp->GetUnitAddress() == bUnitAddress) &&
				(pTemp->GetNodeAddress() == bNodeAddress)) 
			{
				pBindingUnit = pTemp;
				break;
			}
		}
		if (pBindingUnit == 0) {
			AfxMessageBox("Fatal Error");
			return FALSE;
		}
	}
	
	CPoint TopLeft;
	TopLeft.x = (lParam & 0x7FFF0000) >> 16;
	TopLeft.y = (lParam & 0x7FFF);	

	// Sedert V16.42: Label meegeven.
	ASSERT(NULL != pBindingUnit);
	const CString strLabelInfo = TMGetNodeDatabaseApp()->getUnitTextLabel(pBindingUnit->GetNodeAddress(),pBindingUnit->GetUnitAddress());
	return (CDlgBindingUnitProp::Show(pBindingUnit,TopLeft,strLabelInfo));
}


LONG CMainFrame::OnDestroyDlgBindingUnit(WPARAM wParam,LPARAM lParam)
{
	CDlgBindingUnitProp::Destroy();
	CDlgBindingOperator::Destroy();
	return TRUE;
}


/**
 * Date: 20060322 : Changed function ...
 */
LONG CMainFrame::OnBindingDatabaseNameChanged(WPARAM wParam,LPARAM lParam)
{	
	CString szNewName;

	szNewName.Format("%s",lParam);

	if (TMGetBindingEntries() != NULL)
	{
		CBindingEntry *pBindingEntry;
		unsigned short usIdentifier = (wParam & 0x0000FFFF);
				
		if ((pBindingEntry = TMGetBindingEntries()->Get(usIdentifier)) != NULL) 
		{
			pBindingEntry->ChangeName(szNewName);

			return (TMGetViews()->OnBindingDatabaseNameChanged(wParam,lParam));
		}
		else
		{
			CString szError("Error Can't find bindingEntry in BindingDatabase !");
			AfxMessageBox(szError);
		}	
	}
	else
	{
		CString szError("Unable to retrieve pointer to CBindingEntries singleton");
		AfxMessageBox(szError);
	}
	return FALSE;
}


void CMainFrame::OnUpdateButtonBindingsSendall(CCmdUI* pCmdUI) 
{
	ASSERT(TMGetCommunicationApp());
	pCmdUI->Enable(TMIsConnected());
}

void CMainFrame::OnUpdateButtonBindingsSendmodified(CCmdUI* pCmdUI) 
{
	ASSERT(TMGetCommunicationApp());
	pCmdUI->Enable(TMIsConnected());	
}
////////////////////////////////////////////////////////////////////////////////
////////////////////// TOOLBAR BUTTONS AND/OR/XOR/TIMER ////////////////////////
////////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnButtonAnd()		
{	
	TMGetViews()->OnButtonAnd();	
}
void CMainFrame::OnButtonOr()		
{	
	TMGetViews()->OnButtonOr();	
}
void CMainFrame::OnButtonTimer()	
{	
	TMGetViews()->OnButtonTimer(); 
}
void CMainFrame::OnButtonXor()		
{	
	TMGetViews()->OnButtonXor();	
}

void CMainFrame::OnUpdateButtonAnd(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TMGetViews()->IsBindingViewActive());
}
void CMainFrame::OnUpdateButtonOr(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TMGetViews()->IsBindingViewActive());
}
void CMainFrame::OnUpdateButtonTimer(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TMGetViews()->IsBindingViewActive());
}
void CMainFrame::OnUpdateButtonXor(CCmdUI* pCmdUI)  
{
	pCmdUI->Enable(TMGetViews()->IsBindingViewActive());
}

///////////////////////////////////////////////////////////////////////////////
//////////////////////TOOLBAR ADD/DELETE/NEW  /////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CMainFrame::SetModifiedFlag(BOOL flag)  {
	CTimerDoc* pDoc = NULL;
	if ((pDoc = (CTimerDoc*) GetActiveDocument()) != NULL) {
		pDoc->SetModifiedFlag(flag);
	}
}

void CMainFrame::OnButtonAdd()  {	
	if (TMGetViews()->IsBindingViewActive())  {
		SendMessage(WM_DESTROY_DIALOG_BINDINGUNIT);
		CBindingEntry *const pCurrBindingEntry = GetBindingEntry();
		SendMessage(WM_BINDINGENTRY_ADD,(WPARAM) pCurrBindingEntry);
		SetModifiedFlag();
	}
}

void CMainFrame::OnButtonDelete()  {
	if (TMGetViews()->IsBindingViewActive())  {
		CBindingEntry *const pCurrBindingEntry = GetBindingEntry();
		if (NULL != pCurrBindingEntry) {			
			BOOL flagRemove = TRUE;			
			if (CBindingEntry::getFlagAutoGenerated(*pCurrBindingEntry)) {
				if (IDNO == MessageBox("This binding was automatically generated at the start of a new project.\n"
								       "Are you sure you want to remove the binding?",
									   "Remove binding",
										 MB_ICONQUESTION | MB_YESNO)) {
					flagRemove = FALSE;
				}		
			}

			if (flagRemove) {
				SendMessage(WM_DESTROY_DIALOG_BINDINGUNIT);		
				SendMessage(WM_BINDINGENTRY_DELETE, (WPARAM) pCurrBindingEntry);
				SetModifiedFlag();
			}
		}
	}
}

void CMainFrame::OnButtonNew()  {
	if (TMGetViews()->OnBindingEntryNew() == TRUE) {
		SendMessage(WM_DESTROY_DIALOG_BINDINGUNIT);
		ClearScrollView();
		CString szMessage(_T("Ready for new Binding ..."));
		DisplayScrollMessage(szMessage);
		SetModifiedFlag();
	}
}

void CMainFrame::OnButtonAuto() {
	if (TMGetViews()->IsBindingViewActive()) {
		SendMessage(WM_DESTROY_DIALOG_BINDINGUNIT);
		CBindingEntry *const pCurrBindingEntry = GetBindingEntry();
		SendMessage(WM_BINDINGENTRY_AUTO,(WPARAM) pCurrBindingEntry);
		SetModifiedFlag();
	}	
}
/////////////////////////////////////////////////////////////////////////
void CMainFrame::OnUpdateButtonAuto(CCmdUI* pCmdUI) 
{		
	BOOL fEnable = FALSE;
	if (FALSE==m_fWriteBindingsBusy && TMGetViews()->IsBindingViewActive()) 
	{
		CBindingEntry *const pCurrBindingEntry = GetBindingEntry();
		if (pCurrBindingEntry != NULL) 
		{
			if (pCurrBindingEntry->GetBindingNr() == 0x0000) 
			{
				fEnable = TRUE;
			}
		}
		ASSERT(pCurrBindingEntry);
	}
	pCmdUI->Enable(fEnable);
}

void CMainFrame::OnUpdateButtonAdd(CCmdUI* pCmdUI) 
{	
	BOOL fEnable = FALSE;
	if (FALSE==m_fWriteBindingsBusy && TMGetViews()->IsBindingViewActive()) 
	{
		CBindingEntry *const pCurrBindingEntry = GetBindingEntry();		
		if (pCurrBindingEntry != NULL) 
		{
			CBindingLinks *const pBindingLinks = pCurrBindingEntry->GetColLink();
			if (pBindingLinks != NULL) 
			{
				if (pBindingLinks->Count() != 0) 
				{
					fEnable = TRUE;	
				}
			}
			ASSERT(pBindingLinks);
		}
		ASSERT(pCurrBindingEntry);
	}
	pCmdUI->Enable(fEnable);
}

void CMainFrame::OnUpdateButtonNew(CCmdUI* pCmdUI)  
{
	pCmdUI->Enable(TMGetViews()->IsBindingViewActive());
}

void CMainFrame::OnUpdateButtonDelete(CCmdUI* pCmdUI) 
{
	BOOL fEnable = FALSE;
	if (FALSE==m_fWriteBindingsBusy && TMGetViews()->IsBindingViewActive()) 
	{
		CBindingEntry *const pCurrBindingEntry = GetBindingEntry();		
		if (pCurrBindingEntry != NULL) 
		{
			if (pCurrBindingEntry->GetBindingNr() != 0x0000) 
			{
				fEnable = TRUE;
			}
		}
		ASSERT(pCurrBindingEntry);
	}
	pCmdUI->Enable(fEnable);
}

///////////////////////////////////////////////////////////////////////////////

void CMainFrame::OnAutobindingAdd() 
{
	OnButtonAuto( );
}

void CMainFrame::OnUpdateAutobindingAdd(CCmdUI* pCmdUI) 
{
	OnUpdateButtonAuto(pCmdUI);
}

///////////////////////////////////////////////////////////////////////////////
////////////////////////////// NETWORK  ///////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnUpdateButtonDisconnect(CCmdUI* pCmdUI) {
	pCmdUI->Enable(TMIsConnected());	
}

void CMainFrame::OnUpdateButtonConnect(CCmdUI* pCmdUI) {
	pCmdUI->Enable(!TMIsConnected());	
}

void CMainFrame::OnButtonTracer() {
	CDlgCommTracer::Show();
}

void CMainFrame::OnButtonEngineerCanLog() {
	CDlgCommCanLog::Show();
}

// Engineer CAN Logger:
// Enkel tonen wanneer
// - Expert mode.
// - Online
// - Installation mode.
// - Ondersteund door de modem V90
void CMainFrame::OnUpdateButtonEngineerCanLog(CCmdUI* pCmdUI) {
	BOOL enable = FALSE;
	if (TMGetDisplaySettings()->IsModeExtended()) {
		const CAppState::Type_t appType = CAppState::getInstance().getAppType();
		if (CAppState::APP_TYPE_INSTALLATION==appType) {
			const CCommunicationStatus& communicationStatus = CComApplication::GetInstance()->GetCommunicationStatus();
			if (communicationStatus.connected()) {		
				enable = communicationStatus.getServerInfo().getModemCanLogSupported();

			}
		}
	}
	pCmdUI->Enable(enable);
}

///////////////////////////////////////////////////////////////////////////////
////////////////////////// SWITCHING VIEWS ////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void CMainFrame::showBindingsForm( )	/* TM20120830 */
{
	if (!TMGetViews()->IsBindingViewActive())
	{	
		TMGetViews()->ShowBindingsForm(); 
	}
}

void CMainFrame::OnButtonViewBindings()  
{
	TMGetViews()->ShowBindingsForm(); 
}
void CMainFrame::OnButtonViewNodes()  
{
	SendMessage(WM_DESTROY_DIALOG_BINDINGUNIT);		
	TMGetViews()->ShowNodesForm(); 
}
void CMainFrame::OnButtonViewUnits()  
{
	SendMessage(WM_DESTROY_DIALOG_BINDINGUNIT);		
	TMGetViews()->ShowUnitsForm(); 
}

void CMainFrame::OnUpdateButtonViewBindings(CCmdUI* pCmdUI)  
{
	pCmdUI->Enable(!TMGetViews()->IsBindingViewActive()); 
}
void CMainFrame::OnUpdateButtonViewNodes(CCmdUI* pCmdUI)  
{
	pCmdUI->Enable(!TMGetViews()->IsNodesViewActive()); 
}
void CMainFrame::OnUpdateButtonViewUnits(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!TMGetViews()->IsUnitsViewActive()); 
}

void CMainFrame::AddToCommunicationTracer(const CString& s)
{
	const CAppState::Type_t appType = CAppState::getInstance().getAppType();
	if (CAppState::APP_TYPE_INSTALLATION==appType) {
		if (CDlgCommTracer::IsActive() == TRUE) {
			CDlgCommTracer::AddMsgTracerComm(s);		
		}
	}
	else if (CAppState::APP_TYPE_ENGINEERING==appType) {
		if (CDlgEngineeringRS232::IsActive() == TRUE) {
			CDlgEngineeringRS232::Add(s);
		}
	}
}

void CMainFrame::AddToErrorTracer(const CString& s) {
	const CAppState::Type_t appType = CAppState::getInstance().getAppType();
	if (CAppState::APP_TYPE_INSTALLATION==appType) {
		if (CDlgCommTracer::IsActive() == TRUE) {
			CDlgCommTracer::AddMsgTracerError(s);
		}
	}
}

void CMainFrame::AddToEngineerCanLogger(const CString& s) {
	const CAppState::Type_t appType = CAppState::getInstance().getAppType();
	if (CAppState::APP_TYPE_INSTALLATION==appType) {
		if (CDlgCommCanLog::IsActive() == TRUE) {
			CDlgCommCanLog::Add(s);
		}
	}
}

/////////////////////////////////////////////////////////////////////////

void CMainFrame::ShowNodesView(void)	
{ 
	const CAppState::Type_t appType = CAppState::getInstance().getAppType();
	if (CAppState::APP_TYPE_INSTALLATION==appType)
	{
		TMGetViews()->ShowNodesForm(); 
	}
}

void CMainFrame::ClearViews(void) { 
	const CAppState::Type_t appType = CAppState::getInstance().getAppType();
	if (CAppState::APP_TYPE_INSTALLATION==appType) {
		TMGetViews()->ClearViews();
	}
}

void CMainFrame::UpdateViews(int nodeAddress)	{ 
	const CAppState::Type_t appType = CAppState::getInstance().getAppType();
	if (CAppState::APP_TYPE_INSTALLATION==appType) {
		TMGetViews()->UpdateViews(nodeAddress);
	}
}


/***
 * Destroy all Dialogs ...
 */
void CMainFrame::DestroyApplication()
{	
	DUOTECNO::DALI::CAppDaliManager::destroy();

	CDlgNetworkCanStatus::Destroy();
	CDlgMessageTransfer::Destroy();

	CDlgBindingUnitProp::Destroy();
	CDlgBindingOperator::Destroy();
	CDlgUnitProp::Destroy();

	CPropertyNodesSheet_Holder::Destroy();
	CPropertySheetDali_Holder::Destroy();

	CDlgViewIRTXCodes::Destroy( );
	CPropertySheetNewApp::destroy();

	if (CDlgCanFTP::isActive()) {
		CDlgCanFTP::getInstance()->DestroyWindow( );	
	}

#if (USE_TOOLS_APPREMOTECFG == 1)
	if (CDlgToolsRemoteCfg::isActive()) {
		CDlgToolsRemoteCfg::getInstance()->DestroyWindow();
	}
#endif

#ifdef _DEBUG
	if (CDlgDebugGeneral::isActive()) {
		CDlgDebugGeneral::getInstance()->DestroyWindow();
	}	
#endif

	if (CDlgUDPLanConfig::IsActive()) {
		CDlgUDPLanConfig::getInstance()->DestroyWindow();
	}
	if (CDlgUDPLanDevices::IsActive()) {
		CDlgUDPLanDevices::getInstance()->DestroyWindow();
	}

	const CAppState::Type_t appType = CAppState::getInstance().getAppType();
	if (CAppState::APP_TYPE_INSTALLATION==appType)
	{
		// De applicatie werd gesloten

		// Na een filenew wordt deze handler ook aangeroepen
		// maar de bestandsnaam is niet meer geldig.
		// Daarom in OnApplicationFileNewPre( ).

		using namespace DUOTECNO::PLUGIN;
		IConfigToolApp::getInstance().close( );
	}
}

// Return waarde TRUE wanneer er op de 'finish' knop gedrukt werd
BOOL CMainFrame::showPropertySheetNewApp(PropertyPageNewAppMode_t mode) {
	BOOL Result=FALSE;
	AppUDPServerCfg& refUDPServerCfg = getAppUDPServerCfg();
	refUDPServerCfg.setTimingsScanMode(AppUDPServerCfg::SCANMODE_FAST);
	DUOTECNO::AppRemoteCfgLoginInfo& refCgfLoginInfo = this->getAppRemoteCfgLoginInfo();
	DUOTECNO::AppRemoteCfgNodes& refCfgNodeServices = this->getAppRemoteCfgNodeServices();
	CAppDlgStartDialog_Host host(refCgfLoginInfo, refUDPServerCfg, refCfgNodeServices);			// Deze implementeert de Applicatie taken.
	CPropertySheetNewApp* const pDlg = CPropertySheetNewApp::create(this,mode,host);
	if (NULL != pDlg) {		
		if (ID_WIZFINISH == pDlg->DoModal()) {
			Result = TRUE;	// Finish = true !
		}
		CPropertySheetNewApp::destroy();
	}
	refUDPServerCfg.setTimingsScanMode(AppUDPServerCfg::SCANMODE_NORMAL);
	return Result;
}

///////////////////////////////////////////////////////////////////////////
// TASK : Show Application Start Dialog 
// INVOKED BY : CMainFrame::ActicateFrame ...
///////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::ShowApplicationStartDialog(void){
	return this->showPropertySheetNewApp(DIALOGMODE_APPLICATION_START);
}

// Sedert V16.10: Alle project data wissen.
void CMainFrame::initNewApplicationData( ) {
	using namespace DUOTECNO;

	// De algemene databases wissen.
	TMInitialiseDatabases();

	// Bestand & project settings wissen.
	TMGetFileSettings()->Clear();

	// De project gegevens wissen + naam van het project wijzigen...
	// CTimerDocument::m_fDataLoaded zorgt ervoor wanneer we een nieuw project starten 
	// er gevraagd wordt om het project op te slaan.
	CDocument* pDoc = GetActiveDocument();
	if (pDoc != 0) {
		pDoc->DeleteContents();
	}
	// title van het project.
	SetWindowText("New project - Duotecno");

	// Server login Cfg info wissen.
	AppRemoteCfgLoginInfo& refCfgLogin = this->getAppRemoteCfgLoginInfo();
	refCfgLogin.clear( );

	// Node services wissen.
	AppRemoteCfgNodes& refCfgNodeServices = this->getAppRemoteCfgNodeServices();
	refCfgNodeServices.clear( );

	// UDP Config: UDP Device list + alle proxy data wissen.
	AppUDPServerCfg& refAppUDPServerCfg =  this->getAppUDPServerCfg();
	refAppUDPServerCfg.eraseData();

	// CAppDaliManager: Moeten alle instanties wissen + timer stoppen...
	this->KillTimer(TIMER_MAINFRAME_APPDALI_POLL_INTERVAL_MSEC);
	using DUOTECNO::DALI::CAppDaliManager;
	CAppDaliManager::getInstance().erase();	

	// Alle applicatie toestand taken wissen.
	CAppState::getInstance().getTaskEntries().clear();
}

// Sedert V16.10
// Kunnen hier de runtime properties van de node service updaten.
void CMainFrame::UpdateServerLoginCfg( ) {
	using namespace DUOTECNO;
	const CCommunicationStatus& communicationStatus = CComApplication::GetInstance()->GetCommunicationStatus();
	if (communicationStatus.connected()) {	
		const CCommunicationServerInfo& serverInfo = communicationStatus.getServerInfo();		
		if (CCommunicationServerInfo::TCPIP == serverInfo.getCommunicationType()) {
			AppRemoteCfgLoginInfo& refCfgLogin = this->getAppRemoteCfgLoginInfo();

			// Runtime properties van de server: node address, physical node address...
			refCfgLogin.setNodeAddress(serverInfo.getNodeAddress());
			refCfgLogin.setPhysicalAddress(serverInfo.getPhysicalNodeAddress());

			// NodeServices updaten met de communicatie gegevens van de server waarmee we geconnecteerd zijn...
			AppRemoteCfgNodesTCPIPService::updateDevicesAfterLogin(&this->getAppRemoteCfgNodeServices(),refCfgLogin);		
		}
	}
}


// Sedert V16.28: Updaten van de node(services) met de devices gevonden in het LAN netwerk.
// Enkel de devices die aanwezig zijn in de nodedatabase (controle via physical node address) 
// worden upgedate of toegevoegd.
// 
// Deze functie aanroepen wanneer de applicatie in online / offline idle mode komt.
// Oppassen: Komt ook in online IDLE mode na het uitvoeren van een taak.
// 
// Als er modules vervangen werden (physical address veranderd) zijn deze na een pnp/browse
// upgedate in de node database. Mbv deze functie worden alle devices upgedate met hun juiste
// physical / logisch adres en netwerk instellingen.
void CMainFrame::UpdateServerDeviceNodes() {	
	AppUDPDevices& refDeviceNodes = this->getAppUDPServerCfg().getDeviceList();
	const int count = refDeviceNodes.getCount();
	for (int i = 0; i < count; ++i) {
		AppUDPDevice* pDevice = NULL;
		if (refDeviceNodes.getAt(i, &pDevice)) {
			ASSERT(NULL != pDevice);
			this->UpdateServerDeviceNodes(*pDevice);
		}
	}
}

// Sedert V16.28: Updaten van de node(services) met de gegevens van een device gevonden in
// het LAN netwerk.
// Het device moet aanwezig zijn in de nodedatase (zelfde physical node address).
//
// Functie wordt aangeroepen door de UpdateServerDeviceNodes() en wanneer de UDP scan een 
// nieuw device of een device met aangepaste lokale ip instellingen gevonden heeft.
void CMainFrame::UpdateServerDeviceNodes(const AppUDPDevice& r) {	
	const int physicalAddress = r.getPhysicalAddress();
	if (physicalAddress > 0 && physicalAddress < 0xffffff) {
		CCanNode* pNode = NULL;
		int dummy = -1;
		if (TMGetDocumentNodes()->FindPhysicalNode(physicalAddress, &dummy, &pNode)) {
			ASSERT(NULL != pNode);
			AppRemoteCfgNodes_Util::updateDeviceByLogicalAddress(&this->getAppRemoteCfgNodeServices(),pNode->getNodeAddress(),r);						
		}
	}
}


// Sedert V16.09
BOOL CMainFrame::loadCfgLoginInfo( ) {
	const char* pathName = TMGetFileSettings()->GetFullyQualifiedPathName();
	if (0 == pathName) {
		ASSERT(0 != pathName);
		return FALSE;
	}
	DUOTECNO::AppRemoteCfgLoginInfo& refInfo = this->getAppRemoteCfgLoginInfo();
	CString fileName, szSubDir;
	szSubDir.LoadString(IDS_FILENAME_SUBDIRCONFIG);
	fileName.Format("%s%s//projectLogin.dat",pathName,szSubDir);
	if (TRUE == appremotecfg_loadLoginInfo(fileName,&refInfo)) {
		return true;
	}
	return false;
}

// Sedert V16.09
BOOL CMainFrame::saveCfgLoginInfo( ) {
	const char* pathName = TMGetFileSettings()->GetFullyQualifiedPathName();
	if (0 == pathName) {
		ASSERT(0 != pathName);
		return FALSE;
	}
	const DUOTECNO::AppRemoteCfgLoginInfo& refInfo = this->getAppRemoteCfgLoginInfo();
	CString fileName, szSubDir;
	szSubDir.LoadString(IDS_FILENAME_SUBDIRCONFIG);
	fileName.Format("%s%s//projectLogin.dat",pathName,szSubDir);
	if (TRUE == appremotecfg_saveLoginInfo(fileName,refInfo)) {
		if (1)
		{
			CString szInfo;
			szInfo.Format(_T("Save file : '%s'"),fileName);
			this->DisplayScrollMessage(szInfo);
		}
		return true;
	}
	return false;
}

// sedert V16.19
BOOL CMainFrame::saveCfgLoginInfoExport( ) {

	CTimerDoc* pDoc = reinterpret_cast<CTimerDoc*>(GetActiveDocument());
	if (0 == pDoc) {
		return FALSE;
	}	
	const BOOL fPathNameEmpty = pDoc->GetPathName().IsEmpty();
	if (fPathNameEmpty) {
		return FALSE;		// Nog geen data opgeslagen...
	}
	const char* pathName = TMGetFileSettings()->GetFullyQualifiedPathName();
	if (0 == pathName) {
		ASSERT(0 != pathName);
		return FALSE;
	}
	const DUOTECNO::AppRemoteCfgLoginInfo& refInfo = this->getAppRemoteCfgLoginInfo();
	CString fileName, szSubDir;
	szSubDir.LoadString(IDS_FILENAME_SUBDIRCONFIG);
	fileName.Format("%s%s//projectLoginInfo.json",pathName,szSubDir);
	if (TRUE == appremotecfg_exportLoginInfo(fileName,refInfo)) {
		return true;
	}
	return false;
}


// Sedert V16.10
BOOL CMainFrame::loadCfgNodeServices( ) {
	const char* pathName = TMGetFileSettings()->GetFullyQualifiedPathName();
	if (0 == pathName) {
		ASSERT(0 != pathName);
		return FALSE;
	}
	DUOTECNO::AppRemoteCfgNodes& refInfo = this->getAppRemoteCfgNodeServices();
	CString fileName, szSubDir;
	szSubDir.LoadString(IDS_FILENAME_SUBDIRCONFIG);
	fileName.Format("%s%s//projectnodeservices.dat",pathName,szSubDir);
	if (TRUE == appremotecfg_loadNodeServices(fileName,&refInfo)) {
		return true;
	}
	return false;
}

// Sedert V16.10
BOOL CMainFrame::saveCfgNodeServices( ) {
	const char* pathName = TMGetFileSettings()->GetFullyQualifiedPathName();
	if (0 == pathName) {
		ASSERT(0 != pathName);
		return FALSE;
	}
	const DUOTECNO::AppRemoteCfgNodes& refInfo = this->getAppRemoteCfgNodeServices();
	CString fileName, szSubDir;
	szSubDir.LoadString(IDS_FILENAME_SUBDIRCONFIG);
	fileName.Format("%s%s//projectnodeservices.dat",pathName,szSubDir);
	if (TRUE == appremotecfg_saveNodeServices(fileName,refInfo)) {	
		if (1)
		{
			CString szInfo;
			szInfo.Format(_T("Save file : '%s'"),fileName);
			this->DisplayScrollMessage(szInfo);
		}
		return true;
	}
	return false;
}

// Sedert V16.28: Export van de project node services.
BOOL CMainFrame::saveCfgNodeServicesExport( ) {
	CTimerDoc* pDoc = reinterpret_cast<CTimerDoc*>(GetActiveDocument());
	if (0 == pDoc) {
		return FALSE;
	}	
	const BOOL fPathNameEmpty = pDoc->GetPathName().IsEmpty();
	if (fPathNameEmpty) {
		return FALSE;		// Nog geen data opgeslagen...
	}
	const char* pathName = TMGetFileSettings()->GetFullyQualifiedPathName();
	if (0 == pathName) {
		ASSERT(0 != pathName);
		return FALSE;
	}
	const DUOTECNO::AppRemoteCfgNodes& refInfo = this->getAppRemoteCfgNodeServices();
	CString fileName, szSubDir;
	szSubDir.LoadString(IDS_FILENAME_SUBDIRCONFIG);
	fileName.Format("%s%s//projectnodeservices.json",pathName,szSubDir);
	if (TRUE == appremotecfg_exportNodeServices(fileName,refInfo)) {
		return true;
	}
	return false;
}

DUOTECNO::AppRemoteCfgLoginInfo& CMainFrame::getAppRemoteCfgLoginInfo() {
	if (0 == m_pAppRemoteCfgLoginInfo) {
		m_pAppRemoteCfgLoginInfo = new DUOTECNO::AppRemoteCfgLoginInfo();
		ASSERT(0 != m_pAppRemoteCfgLoginInfo);
	}
	return *m_pAppRemoteCfgLoginInfo;
}

DUOTECNO::AppRemoteCfgNodes& CMainFrame::getAppRemoteCfgNodeServices() {
	if (0 == m_pAppRemoteCfgNodes) {
		m_pAppRemoteCfgNodes = new DUOTECNO::AppRemoteCfgNodes();
		ASSERT(0 != m_pAppRemoteCfgNodes);
	}
	return *m_pAppRemoteCfgNodes;
}

AppUDPServerCfg& CMainFrame::getAppUDPServerCfg() {
	if (0 == m_pAppUDPServerCfg) {
		m_pAppUDPServerCfg = new AppUDPServerCfg( );
	}
	return *m_pAppUDPServerCfg;
}

// Sedert V16.36:
// - Wanneer er Sbox ultra nodes aanwezig zijn dan de mogelijkheid om de DALI subbus te configureren.
void CMainFrame::updateAppStateViewModes( ) {
	bool enableMenuItemDaliCfg = false;		// Enabled wanneer er Smartbox ultra nodes aanwezig zijn.
	CNodeIterator it(TMGetDocumentNodes()->CreateNodeIterator());
	for (it.Begin();!it.IsDone();it.Next()) {
		CCanNode* const pNode = it.CurrentItem( );
		if ((NULL != pNode) && (PRODUCT_ID_HC_BOX_ULTRA == pNode->GetProductID())) {
			enableMenuItemDaliCfg = true;
			break;
		}
	}
	CAppState::getInstance().setEnableMenuItemDaliCfg(enableMenuItemDaliCfg);
}

BOOL CMainFrame::LoadDocumentFile(CStringArray* pStrArray)
{
	// CHANGES_INCOMPATIBLE_VERSION:
	// Controle of het bestand correct ingeladen werd.

	BOOL fResult = FALSE;
	CTimerDoc* pDoc = NULL;		

	if ((pDoc = (CTimerDoc*) GetActiveDocument( )) != NULL)
	{	
		// Deze moet in feite niet manueel aangeroepen worden ?
		pDoc->DeleteContents();						

		fResult = pDoc->OpenApplicationFile(pStrArray);

		if ((TRUE==fResult)&&(0!=pStrArray)) {
			CAppFileLogAPI::createLogFile(APPFILELOG_BINDINGFILES_LOAD_EC,*pStrArray);			
		}
	}	
	return fResult;
}

BOOL CMainFrame::VerifyDocumentBindingDatabases(const CStringArray* pStrArray)
{
	const BOOL fResult=TRUE;

	CDlgAppBindingsVerify dlg(this,pStrArray);
	dlg.DoModal( );

	return fResult;
}


BOOL CMainFrame::TaskSetDateToMasterLcd()
{
	CCanNode *pNode;
	int iEntryNR;

	if (TMGetDocumentNodes()->FindNode(0xFC, &iEntryNR, &pNode)) {
		CTime t(CTime::GetCurrentTime());
		CString s(t.Format(" %Y%m%d"));	
	
		if (strcmp(s,pNode->szNodeName) != 0) {
			const WPARAM wParam = 0x0000FC00;
			const LPARAM lParam = (LPARAM) ((LPCTSTR) s);	// CHANGES_MVS_2008_CSTRING
			SendMessage(WM_NODENAME_CHANGED,wParam,lParam);
		}	
	}
	return TRUE;
}

#if(0)
void CMainFrame::IPCUpdateMgrSendUpdateString()
{
	updatemgripc_SendUpdateString( );
}
#endif 

////////////////// NEW 20040622 /////////////////////
void CMainFrame::OnViewSortnodesbyname()  {
	TMGetViews()->OnViewSortnodesbyname();
}
void CMainFrame::OnViewSortnodesbyphaddress() {
	TMGetViews()->OnViewSortnodesbyphaddress();	
}
void CMainFrame::OnViewSortnodesbylogicaladdress() {
	TMGetViews()->OnViewSortnodesbylogicaladdress();
}
void CMainFrame::OnViewSortnodesonlyname() {
	TMGetViews()->OnViewSortnodesonlyname();	
}
void CMainFrame::OnViewSortBindings() {
	TMGetViews()->OnViewSortBindings();
}
void CMainFrame::OnTreeviewBindingsNameandnumber() {
	TMGetViews()->OnTreeviewBindingsNameandnumber();
}
void CMainFrame::OnTreeviewBindingsNumberandname() {
	TMGetViews()->OnTreeviewBindingsNumberandname();
}
void CMainFrame::OnTreeviewBindingsOnlyname() {
	TMGetViews()->OnTreeviewBindingsOnlyname();
}
void CMainFrame::OnButtonSortNodes() {
	TMGetViews()->OnButtonSortNodes();
}
void CMainFrame::OnButtonCollapse() {
	TMGetViews()->OnButtonCollapse();	
}

void CMainFrame::OnFileExport() 
{
	CTimerDoc* pDoc = NULL;

	if ((pDoc = (CTimerDoc*) GetActiveDocument( )) != NULL) 
	{
		//TM 20040702 Wait Cursur while busy ...
		HCURSOR oldCursur = SetCursor(LoadCursor(NULL, IDC_WAIT));
		pDoc->ExportHtmlFiles();
	
		DUOTECNO::PLUGIN::IConfigToolApp::getInstance().exportHTMLFiles( );
		SetCursor(oldCursur);
	}
}


void CMainFrame::OnButtonExpand() 
{
	TMGetViews()->OnButtonExpand();
}

void CMainFrame::OnButtonSortUnits() 
{
	TMGetViews()->OnButtonSortUnits();
}

void CMainFrame::OnViewUnitsbyaddress() 
{
	TMGetViews()->OnViewUnitsbyaddress();	
}

void CMainFrame::OnViewUnitsbyname() 
{
	TMGetViews()->OnViewUnitsbyname();		
}

void CMainFrame::OnViewUnitsonlyname() 
{
	TMGetViews()->OnViewUnitsonlyname();		
}

void CMainFrame::OnUpdateButtonSortNodes(CCmdUI* pCmdUI) 
{
	BOOL fResult = FALSE;

	if (TMGetViews()->IsTreeNodesActive()) fResult = TRUE;
	else if (TMGetViews()->IsTreeBindingsActive()) fResult = TRUE;
	
	pCmdUI->Enable(fResult);
}
void CMainFrame::OnUpdateButtonSortUnits(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TMGetViews()->IsTreeNodesActive());	
}

void CMainFrame::OnUpdateViewSortBindings(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(FALSE);
}

void CMainFrame::OnUpdateButtonCollapse(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TMGetViews()->IsTreeNodesActive());	
}

void CMainFrame::OnUpdateButtonExpand(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TMGetViews()->IsTreeNodesActive());	
}

void CMainFrame::OnUpdateViewUnitsonlyname(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TMGetViews()->IsTreeNodesActive());	
}

void CMainFrame::OnUpdateViewUnitsbyname(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TMGetViews()->IsTreeNodesActive());	
}

void CMainFrame::OnUpdateViewUnitsbyaddress(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TMGetViews()->IsTreeNodesActive());
}

void CMainFrame::OnUpdateViewSortnodesbylogicaladdress(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TMGetViews()->IsTreeNodesActive());	
}

void CMainFrame::OnUpdateViewSortnodesbyname(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TMGetViews()->IsTreeNodesActive());
}

void CMainFrame::OnUpdateViewSortnodesbyphaddress(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TMGetViews()->IsTreeNodesActive());	
}

void CMainFrame::OnUpdateViewSortnodesonlyname(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TMGetViews()->IsTreeNodesActive());	
}

void CMainFrame::OnUpdateTreeviewBindingsNameandnumber(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TMGetViews()->IsTreeBindingsActive());	
}

void CMainFrame::OnUpdateTreeviewBindingsOnlyname(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TMGetViews()->IsTreeBindingsActive());		
}
void CMainFrame::OnUpdateTreeviewBindingsNumberandname(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TMGetViews()->IsTreeBindingsActive());		
}


/**
 * Date 20060322 : Changed function
 */
LONG CMainFrame::OnApplicationNotifyBindingAdd(WPARAM wParam,LPARAM lParam)
{
	CString szMessage;
	WPARAM wParamId;
	
	ClearScrollView();

	szMessage.Format(_T("Add Binding to database ..."));
	DisplayScrollMessage(szMessage);

	szMessage.Format(_T("Ready for new Binding ..."));
	DisplayScrollMessage(szMessage);

	if (TMGetBindingEntries() != NULL)
	{
		CBindingEntry *pBindingEntry;		
		const unsigned short nBindingId = ((unsigned short) (wParam & 0xFFFF));

		if ((pBindingEntry = TMGetBindingEntries()->Get(nBindingId)) != NULL)
		{
			CBindingUnits *pBindingUnits;
		
			// MUST UPDATE ALL NAMES OF UNITS ...
			if ((pBindingUnits = pBindingEntry->GetColUnit()) != NULL)
			{
				CBindingUnits::Iterator it(pBindingUnits->CreateIterator());
				for (it.Begin();!it.IsDone();it.Next())
				{
					CBindingUnit* const pBindingUnit = it.CurrentItem();
					ASSERT( pBindingUnit );
		
					wParamId = (pBindingUnit->GetNodeAddress() << 8);
					wParamId |= pBindingUnit->GetUnitAddress();

					szMessage = pBindingUnit->GetName();

					TMGetViews()->OnUnitNameChanged(wParamId,(LPARAM) (LPCTSTR)(szMessage)); // CHANGES_MVS_2008_CSTRING

				} /* end for loop */

				return (TMGetViews()->OnBindingEntryAdd((WPARAM) nBindingId,lParam));
			}
			else
			{
				CString szError("Unable to retrieve pointer to CBindingUnits");
				TRACE1("%s",szError);
				ASSERT(0);		
			}
		}
		else
		{
			CString szError("Unable to retrieve pointer to CBindingEntry");
			TRACE1("%s",szError);
			ASSERT(0);			
		}
	}
	else
	{
		CString szError("Unable to retrieve pointer to CBindingEntries singleton");
		AfxMessageBox(szError);
		ASSERT(0);			
	}
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnDimmersOnoff()  
{
	if (TMGetAutoBindingOptions() != NULL)
	{
		TMGetAutoBindingOptions()->SetAutoDimmerOption(DIMMER_AUTO_OPTION_ONOFF);
	}
}
void CMainFrame::OnDimmersPirtimed() 
{
	if (TMGetAutoBindingOptions() != NULL)
	{
		TMGetAutoBindingOptions()->SetAutoDimmerOption(DIMMER_AUTO_OPTION_PIRON);
	}
}
void CMainFrame::OnUpdateDimmersPirtimed(CCmdUI* pCmdUI) 
{
	if (TMGetAutoBindingOptions() != NULL)
	{
		if (TMGetAutoBindingOptions()->GetAutoDimmerOption() == DIMMER_AUTO_OPTION_PIRON) 
		{
			pCmdUI->SetCheck(1);
		}
		else 
		{
			pCmdUI->SetCheck(0);
		}
	}
}
void CMainFrame::OnUpdateDimmersOnoff(CCmdUI* pCmdUI) 
{
	if (TMGetAutoBindingOptions() != NULL)
	{
		if (TMGetAutoBindingOptions()->GetAutoDimmerOption() == DIMMER_AUTO_OPTION_ONOFF) 
		{
			pCmdUI->SetCheck(1);
		}
		else 
		{
			pCmdUI->SetCheck(0);
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnRelaisOnoff() 
{ 
	if (TMGetAutoBindingOptions() != NULL)
	{
		TMGetAutoBindingOptions()->SetAutoRelaisOption(RELAIS_AUTO_OPTION_ONOFF);
	}
}

void CMainFrame::OnRelaisPirtimedon() 
{ 
	if (TMGetAutoBindingOptions() != NULL)
	{
		TMGetAutoBindingOptions()->SetAutoRelaisOption(RELAIS_AUTO_OPTION_PIRON);
	}
}
void CMainFrame::OnUpdateRelaisOnoff(CCmdUI* pCmdUI) 
{
	if (TMGetAutoBindingOptions() != NULL)
	{
		if (TMGetAutoBindingOptions()->GetAutoRelaisOption() == RELAIS_AUTO_OPTION_ONOFF)
		{
			pCmdUI->SetCheck(1);
		}
		else
		{
			pCmdUI->SetCheck(0);	
		}
	}
}
void CMainFrame::OnUpdateRelaisPirtimedon(CCmdUI* pCmdUI) 
{
	if (TMGetAutoBindingOptions() != NULL)
	{
		if (TMGetAutoBindingOptions()->GetAutoRelaisOption() == RELAIS_AUTO_OPTION_PIRON) 
		{
			pCmdUI->SetCheck(1);
		}
		else 
		{
			pCmdUI->SetCheck(0);	
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnAutobindingEnable() 
{	

}
void CMainFrame::OnUpdateAutobindingEnable(CCmdUI* pCmdUI)
{	

}
////////////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnSensCooling() 
{
	if (TMGetAutoBindingOptions() != NULL)
	{
		TMGetAutoBindingOptions()->SetAutoSensOption(SENS_AUTO_OPTION_COOLING);
	}
}

void CMainFrame::OnSensHeating() 
{ 
	if (TMGetAutoBindingOptions() != NULL)
	{
		TMGetAutoBindingOptions()->SetAutoSensOption(SENS_AUTO_OPTION_HEATING);
	}
}

void CMainFrame::OnUpdateSensCooling(CCmdUI* pCmdUI) 
{
	if (TMGetAutoBindingOptions() != NULL)
	{
		if (TMGetAutoBindingOptions()->GetAutoSensOption() == SENS_AUTO_OPTION_COOLING) 
		{
			pCmdUI->SetCheck(1);
		}
		else
		{
			pCmdUI->SetCheck(0);		
		}
	}
}

void CMainFrame::OnUpdateSensHeating(CCmdUI* pCmdUI) 
{
	if (TMGetAutoBindingOptions() != NULL)
	{
		if (TMGetAutoBindingOptions()->GetAutoSensOption() == SENS_AUTO_OPTION_HEATING)
		{ 
			pCmdUI->SetCheck(1);
		}
		else 
		{
			pCmdUI->SetCheck(0);
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnMoodsLong() 
{
	if (TMGetAutoBindingOptions() != NULL)
	{
		TMGetAutoBindingOptions()->SetAutoMoodOption(MOODS_AUTO_OPTION_LONG); 
	}
}
void CMainFrame::OnMoodsShort() 
{
	if (TMGetAutoBindingOptions() != NULL)
	{
		TMGetAutoBindingOptions()->SetAutoMoodOption(MOODS_AUTO_OPTION_SHORT); 
	}
}

void CMainFrame::OnUpdateMoodsLong(CCmdUI* pCmdUI) 
{
	if (TMGetAutoBindingOptions() != NULL)
	{
		if (TMGetAutoBindingOptions()->GetAutoMoodOption() == MOODS_AUTO_OPTION_LONG) 
		{
			pCmdUI->SetCheck(1);
		}
		else
		{
			pCmdUI->SetCheck(0);
		}
	}
}

void CMainFrame::OnUpdateMoodsShort(CCmdUI* pCmdUI) 
{
	if (TMGetAutoBindingOptions() != NULL)
	{
		if (TMGetAutoBindingOptions()->GetAutoMoodOption() == MOODS_AUTO_OPTION_SHORT) 
		{
			pCmdUI->SetCheck(1);
		}
		else
		{
			pCmdUI->SetCheck(0);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////

// todo : Clear all Databases ...
void CMainFrame::HandleFatalError(CMyException *e)
{
	CString szTerminate(LPCSTR(IDS_EXCEPTION_CANNOTCONTINUE));
	AfxMessageBox(szTerminate,MB_ICONSTOP);
	
	if (e != NULL) delete e;

	ASSERT(0);		
	exit(-1);	
}


// Sedert versie 0x0800
// Controle op de retValue van de PropertySheet.
// Functie geeft TRUE terug wanneer op FINISH gedrukt wordt.
//
// Sedert V16.16:
// Opvragen of er opnieuw moet gebrowsed worden.
// - Bij de naam verschillen (copy van nwk naar nodedbase) is een nieuwe browse niet 
//   nodig.
BOOL CMainFrame::ShowProblemSolveWizard(BOOL processDatabaseActions,WizardTasks_t wizardTask)
{	
	if (NULL != this->m_problemSolveWizard) {
		ASSERT(NULL == this->m_problemSolveWizard);
		return FALSE;
	}
	// Instantie aanmaken van de problem solve wizard.
	this->m_problemSolveWizard = new CPropertyWizard(this,wizardTask);
	if (NULL == this->m_problemSolveWizard) {		
		ASSERT(NULL != this->m_problemSolveWizard);
		return FALSE;
	}
	
	// Modaal dialoog: Retval bevat het resultaat.
	const INT_PTR retVal = this->m_problemSolveWizard->DoModal();

	if (processDatabaseActions) {
		// Verwerken van de resultaten van de problem solve wizard.
		ASSERT(NULL != this->m_problemSolveWizard);
		this->processProblemSolveWizardActions(*this->m_problemSolveWizard,retVal);
	}

	// Problem solve wizard niet meer zichtbaar - verwijderen van de referentie.
	INVARIANT(NULL != this->m_problemSolveWizard);
	if (NULL != this->m_problemSolveWizard) {
		delete this->m_problemSolveWizard;
		this->m_problemSolveWizard = NULL;
	}
	return (IDOK==retVal || ID_WIZFINISH==retVal);
}

// Evaluatie van de resultaten van de problem solve wizard.
void CMainFrame::processProblemSolveWizardActions(CPropertyWizard& ref,INT_PTR retVal) {
	CAppGlobalDbaseActions::Actions appDbaseAction = CAppGlobalDbaseActions::APPDBASE_ACTION_IDLE;
	if (IDCANCEL==retVal) {
		appDbaseAction = CAppGlobalDbaseActions::APPDBASE_ACTION_BROWSE;
	}
	else if (IDOK==retVal || ID_WIZFINISH==retVal)
	{
		if (ref.IsAutoPnpSelected()) {
			appDbaseAction = CAppGlobalDbaseActions::APPDBASE_ACTION_PNP;
		}
		else if ((ref.IsSendBindingsSelected())&&(ref.IsSendNamesSelected())) {
			appDbaseAction = CAppGlobalDbaseActions::APPDBASE_ACTION_SENDPROGFILE;
		}
		else if (ref.IsSendBindingsSelected()) {
			appDbaseAction = CAppGlobalDbaseActions::APPDBASE_ACTION_SENDBINDINGS;
		}
		else if (ref.IsSendNamesSelected()) {
			appDbaseAction = CAppGlobalDbaseActions::APPDBASE_ACTION_SENDNAMES;
		}	
		else if (ref.IsBrowseAgain()) {
			appDbaseAction = CAppGlobalDbaseActions::APPDBASE_ACTION_BROWSE;
		}	
		else {
			appDbaseAction = CAppGlobalDbaseActions::APPDBASE_ACTION_IDLE;	
		}
	}
	CAppStateUtil::setDbaseActions(CAppState::getInstance(),appDbaseAction);	
}

void CMainFrame::OnConfiguration() 
{
	ShowProblemSolveWizard(FALSE,WIZARD_USER_SELECTION);
}

void CMainFrame::OnUpdateConfiguration(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TMIsConnected());	
}


/////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::StoreMemoryDumpInfo(void)
{
	CMemoryDumpTaskInfo *pTaskInfo = NULL;	
	CString szInfo,szTask;

	BYTE bNodeAddress = CMemoryDumpInfo::GetInstance()->GetAssignedNodeAddress();

	CMemoryDumpInfo::GetInstance()->StoreLogFile();

	szInfo.Format(_T("-------------------------------- MEMORY DUMP INFO -----------------------------"));
	DisplayScrollMessage(szInfo);

	for (int i=0;i<MEMORYDUMPINFO_TYPE_MAX;i++) 
	{
		if (CMemoryDumpInfo::GetInstance()->IsUpload((ENUM_MEMORYDUMPINFO_TYPE) i)) 
		{		
			pTaskInfo = CMemoryDumpInfo::GetInstance()->GetTaskInfo(MEMORYDUMPINFO_TASK_UPLOAD,
													(ENUM_MEMORYDUMPINFO_TYPE) i);
			if (pTaskInfo != NULL) 
			{
				szInfo.Format(_T("Logical Node 0x%02x : %s"),bNodeAddress,pTaskInfo->GetTaskString());
				DisplayScrollMessage(szInfo);
			}
		}

		if (CMemoryDumpInfo::GetInstance()->IsDownload((ENUM_MEMORYDUMPINFO_TYPE) i)) 
		{				
			pTaskInfo = CMemoryDumpInfo::GetInstance()->GetTaskInfo(MEMORYDUMPINFO_TASK_DOWNLOAD,
													(ENUM_MEMORYDUMPINFO_TYPE) i);
			if (pTaskInfo != NULL) 
			{
				if (pTaskInfo->IsTaskDone()) 
				{
					szInfo.Format(_T("Logical Node 0x%02x : %s"),bNodeAddress,pTaskInfo->GetVerifyString());
					DisplayScrollMessage(szInfo);
				}
				else 
				{
					szInfo.Format(_T("Logical Node 0x%02x : %s"),bNodeAddress,pTaskInfo->GetTaskString());
					DisplayScrollMessage(szInfo);
				}
			}
		}
	}

	return TRUE;
}

void CMainFrame::OnMultimedia() 
{
	const CAppState::Type_t appType = CAppState::getInstance().getAppType();
	if (CAppState::APP_TYPE_INSTALLATION==appType)
	{
		CDlgMultimedia dlg;
		dlg.DoModal();
	}
}

void CMainFrame::OnUpdateMultimedia(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( FALSE );	
}

void CMainFrame::OnModemstatus() 
{
	if (TMGetCommunicationApp() != NULL)
	{		
		CDlgModemStatus::Show();	
	}
}

void CMainFrame::OnUpdateModemstatus(CCmdUI* pCmdUI) 
{
	CComApplication* const pInstance = TMGetCommunicationApp();
	if (NULL == pInstance || !pInstance->IsConnected()) {
		pCmdUI->Enable(FALSE);	
	}	
	pCmdUI->Enable(CCommunicationServerInfo::SERIAL  == pInstance->GetCommunicationStatus().getServerInfo().getCommunicationType());	
}


/**
 * Engineering mode !!!
 */
void CMainFrame::OnNetworkCommunication() 
{
	const CAppState::Type_t appType = CAppState::getInstance().getAppType();

	ASSERT(CAppState::APP_TYPE_ENGINEERING==appType);

	if (CAppState::APP_TYPE_ENGINEERING==appType)
	{
		CDlgEngineeringRS232::Show();
	}
}

void CMainFrame::OnNetworkPhbrowse() 
{
	const CAppState::Type_t appType = CAppState::getInstance().getAppType();

	ASSERT(CAppState::APP_TYPE_ENGINEERING==appType);

	if (CAppState::APP_TYPE_ENGINEERING==appType)
	{
		TMGetGlobalHSM()->EvOperationBrowseForPhysicalNodes();
	}
}

void CMainFrame::OnNetworkRequeststates() 
{
	const CAppState::Type_t appType = CAppState::getInstance().getAppType();

	ASSERT(CAppState::APP_TYPE_ENGINEERING==appType);

	if (CAppState::APP_TYPE_ENGINEERING==appType)
	{
		TMGetGlobalHSM()->EvOperationRequestForStates();
	}
}

void CMainFrame::OnNetworkAutopnp() 
{
	const CAppState::Type_t appType = CAppState::getInstance().getAppType();

	ASSERT(CAppState::APP_TYPE_ENGINEERING==appType);

	if (CAppState::APP_TYPE_ENGINEERING==appType)
	{
		TMGetGlobalHSM()->EvOperationAutomaticPnp();
	}
	
}

void CMainFrame::OnNetworkReadallbindings() 
{
	const CAppState::Type_t appType = CAppState::getInstance().getAppType();
	
	ASSERT(CAppState::APP_TYPE_ENGINEERING==appType);

	if (CAppState::APP_TYPE_ENGINEERING==appType)
	{
		TMGetGlobalHSM()->EvOperationReadAllBindings();
	}
}

void CMainFrame::OnNetworkWriteallbindings() 
{
	const CAppState::Type_t appType = CAppState::getInstance().getAppType();

	ASSERT(CAppState::APP_TYPE_ENGINEERING==appType);

	if (CAppState::APP_TYPE_ENGINEERING==appType)
	{
		TMGetGlobalHSM()->EvOperationUploadAllBindings();
	}
}

void CMainFrame::OnToolsImport() 
{
	TMGetGlobalHSM()->EvOperationDocumentLoad();
}

void CMainFrame::OnToolsConversion() 
{
	TMGetGlobalHSM()->EvConvertToBindingDatabase(EV_OPERATION_SHOW);
}

void CMainFrame::OnToolsNicmedium() 
{	
	const CAppState::Type_t appType = CAppState::getInstance().getAppType();
	if (CAppState::APP_TYPE_INSTALLATION==appType)
	{
		CDlgNetworkCanStatus::Show();	
	}
}

void CMainFrame::OnUpdateToolsNicmedium(CCmdUI* pCmdUI) 
{
	BOOL fEnable = FALSE;
	const CAppState::Type_t appType = CAppState::getInstance().getAppType();

	if (TMIsConnected() && CAppState::APP_TYPE_INSTALLATION==appType)
		fEnable = TRUE;

	pCmdUI->Enable(fEnable);	
}

void CMainFrame::OnNetworkMessagetransfer() 
{
	const CAppState::Type_t appType = CAppState::getInstance().getAppType();
	if (CAppState::APP_TYPE_INSTALLATION==appType)
	{
		CDlgMessageTransfer::Show();	
	}
}

// Engineer Network Messagetransfer:
// Enkel tonen wanneer
// - Expert mode.
// - Online
// - Installation mode.
void CMainFrame::OnUpdateNetworkMessagetransfer(CCmdUI* pCmdUI) 
{
	BOOL fEnable = FALSE;
	if (TMGetDisplaySettings()->IsModeExtended()) {
		const CAppState::Type_t appType = CAppState::getInstance().getAppType();
		if (TMIsConnected() && CAppState::APP_TYPE_INSTALLATION==appType)
			fEnable = TRUE;
	}
	pCmdUI->Enable(fEnable);		
}


// Sedert V16.10: getDataLoaded()
// Indien er geen data geladen is dan ook vragen om het project te saven.
//
// Sedert V16.11: getDataSaved() - zie BUG-0166
// Indien er nog geen data geladen of opgeslagen werd vragen om het project op te slaan.
//
// Sedert V16.41: Een extra vlag om te bepalen of de Configrator moet gerefreshed worden.
LONG CMainFrame::OnApplicationOnlineIdle(WPARAM wParam,LPARAM lParam)
{
	const BOOL notifyConfigurator = (BOOL) wParam;
	BOOL exportConfiguratorFiles = notifyConfigurator;

	CTimerDoc* pDoc = reinterpret_cast<CTimerDoc*>(GetActiveDocument());
	if (pDoc != 0) {
		const BOOL fPathNameEmpty = pDoc->GetPathName().IsEmpty();
		const BOOL fDataLoaded = pDoc->getDataLoaded();
		const BOOL fDataSaved = pDoc->getDataSaved();
		if ((TRUE==fPathNameEmpty) || ((FALSE == fDataLoaded) && (FALSE == fDataSaved))) {
			CString szText("Please first save your project file");
			if (AfxMessageBox(szText, MB_ICONQUESTION |MB_OKCANCEL ) == IDOK) {
				if (pDoc->SaveApplicationDirectory( )) {
					exportConfiguratorFiles = FALSE;		// Reeds opgeslagen.
				}
			}
		}	
	}

	// De Configurator melden: 
	// - Export van de bestanden 
	// - Vragen om een refresh uit te voeren met de nieuwe data.
	if (notifyConfigurator) {		
		if(exportConfiguratorFiles) {		
			TMVaria::TMExportNeededConfiguratorFiles();
		}	
		DUOTECNO::PLUGIN::IConfigToolApp::getInstance().refreshDisplay( );	
	}
	return TRUE;
}


/*
void CMainFrame::OnNetworkNewinstallation() 
{
	// TODO: Add your command handler code here	
}
*/

/*
void CMainFrame::OnDebugConversionfromnodes() 
{
	CDlgConversionFromNodes::Show();
}
*/

/*
void CMainFrame::OnToolsConversion() 
{
	PostMessage(WM_QHSM_EVENT,
				EV_OPERATION_CONVERT_TO_BINDINGDATABASE,
				EV_OPERATION_SHOW);
}

void CMainFrame::OnUpdateToolsConversion(CCmdUI* pCmdUI) 
{	
	//pCmdUI->Enable(TMGetGlobalHSM()->DatabaseInitialised());
}
*/

void CMainFrame::OnDebugShowTaskdialog() 
{
	CDlgMainApplicationTask* pDlg = CDlgMainApplicationTask::GetInstance();
	if (pDlg != 0)
	{	
		CString szMessage("EV_MESSAGEEROR : UNIT NOT FOUND, Received from node 0x08, Unit 0x00\r\r\n"		
						  "EV_MESSAGEEROR : UNIT NOT FOUND, Received from node 0x08, Unit 0x00\r\r\n"
						  "EV_MESSAGEEROR : UNIT NOT FOUND, Received from node 0x08, Unit 0x00\r\r\n"
						  "EV_MESSAGEEROR : UNIT NOT FOUND, Received from node 0x08, Unit 0x00\r\r\n"
						  "EV_MESSAGEEROR : UNIT NOT FOUND, Received from node 0x08, Unit 0x00\r\r\n"
						  "EV_MESSAGEEROR : UNIT NOT FOUND, Received from node 0x08, Unit 0x00\r\r\n"
						  "EV_MESSAGEEROR : UNIT NOT FOUND, Received from node 0x08, Unit 0x00\r\r\n"
						  "EV_MESSAGEEROR : UNIT NOT FOUND, Received from node 0x08, Unit 0x00\r\r\n"
						  "EV_MESSAGEEROR : UNIT NOT FOUND, Received from node 0x08, Unit 0x00\r\r\n"
						  "EV_MESSAGEEROR : UNIT NOT FOUND, Received from node 0x08, Unit 0x00\r\r\n"
						  "EV_MESSAGEEROR : UNIT NOT FOUND, Received from node 0x08, Unit 0x00\r\r\n"
						  "EV_MESSAGEEROR : UNIT NOT FOUND, Received from node 0x08, Unit 0x00\r\r\n"
						  "EV_MESSAGEEROR : UNIT NOT FOUND, Received from node 0x08, Unit 0x00\r\r\n");

		pDlg->SetModeTaskErrorLog(szMessage);
	}
}

void CMainFrame::OnUpdateDebugShowTaskdialog(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(FALSE);	
}

void CMainFrame::OnViewModeAdvanced() 
{
	if (TMGetDisplaySettings() != 0)	
		TMGetDisplaySettings()->SetModeExtended();		
}

void CMainFrame::OnUpdateViewModeAdvanced(CCmdUI* pCmdUI) 
{
	if (TMGetDisplaySettings() != 0)
	{		
		pCmdUI->SetCheck(TMGetDisplaySettings()->IsModeExtended());	
	}	
}

void CMainFrame::OnViewModeNormal() 
{
	if (TMGetDisplaySettings() != 0)				
		TMGetDisplaySettings()->SetModeNormal();		
}

void CMainFrame::OnUpdateViewModeNormal(CCmdUI* pCmdUI) 
{
	if (TMGetDisplaySettings() != 0)
	{		
		pCmdUI->SetCheck(TMGetDisplaySettings()->IsModeNormal());	
	}		
}

void CMainFrame::OnViewModeSimple() 
{
	if (TMGetDisplaySettings() != 0)				
		TMGetDisplaySettings()->SetModeSimple();		
}

void CMainFrame::OnUpdateViewModeSimple(CCmdUI* pCmdUI) 
{
	if (TMGetDisplaySettings() != 0)
	{		
		pCmdUI->SetCheck(TMGetDisplaySettings()->IsModeSimple());	
	}	
}


void CMainFrame::OnDebugGeneral() 
{
#ifdef _DEBUG
	CDlgDebugGeneral* const pDlg = CDlgDebugGeneral::getInstance( );
	if (pDlg) {
		pDlg->Show( );
	}
#else
	// absorbe
#endif
}

void CMainFrame::OnUpdateDebugGeneral(CCmdUI* pCmdUI) 
{
#ifdef _DEBUG
	const bool fVisible = true;		
#else
	const bool fVisible = false;
#endif

	pCmdUI->Enable(fVisible);	
}

void CMainFrame::OnNetworkStatus() 
{
	const CAppState::Type_t appType = CAppState::getInstance().getAppType();
	if (CAppState::APP_TYPE_INSTALLATION==appType)
	{
		CDlgNetworkCanStatus::Show();	
	}	
}

void CMainFrame::OnUpdateNetworkStatus(CCmdUI* pCmdUI) 
{
	const CAppState::Type_t appType = CAppState::getInstance().getAppType();

	pCmdUI->Enable(TMIsConnected() && CAppState::APP_TYPE_INSTALLATION==appType);
}

void CMainFrame::OnNetworkCanftp() 
{
	const CAppState::Type_t appType = CAppState::getInstance().getAppType();
	if (CAppState::APP_TYPE_INSTALLATION==appType)
	{
		CDlgCanFTP* const pDlg = CDlgCanFTP::getInstance( );
		if (pDlg)
		{
			pDlg->Show( );
		}
	}
}

// Engineer Network CAN FTP:
// Enkel tonen wanneer
// - Expert mode.
// - Online
// - Installation mode.
void CMainFrame::OnUpdateNetworkCanftp(CCmdUI* pCmdUI) 
{
	BOOL fEnable = FALSE;
	if (TMGetDisplaySettings()->IsModeExtended()) {
		const CAppState::Type_t appType = CAppState::getInstance().getAppType();
		if (TMIsConnected() && CAppState::APP_TYPE_INSTALLATION==appType) {
			fEnable = TRUE;
		}
	}
	pCmdUI->Enable(fEnable);	
}

void CMainFrame::OnToolsConfigurator() 
{
	const CAppState::Type_t appType = CAppState::getInstance().getAppType();
	if (CAppState::APP_TYPE_INSTALLATION==appType)
	{
		// Todo Controleren of het bestand aanwezig is !!!!

		// TM20100812
		// CExportTextFilesApp export;
		// export.StartExport( );

		// Gaan hier geen export doen van het bestand want we zijn niet zeker
		// dat er reeds een project directory werd aangemaakt.

		// De configurator openen met alle menu buttons enabled.
		using namespace DUOTECNO::PLUGIN;
		IConfigToolApp::getInstance().show( );
	}	
}

void CMainFrame::OnUpdateToolsConfigurator(CCmdUI* pCmdUI) 
{
	static BOOL fInit = TRUE;
	static BOOL visible = FALSE;

	using namespace DUOTECNO::PLUGIN;

	const CAppState::Type_t appType = CAppState::getInstance().getAppType();
	if (CAppState::APP_TYPE_INSTALLATION==appType)
	{
		TRACE("Check Tools Configurator\n");
	
		if ( fInit )
		{
			visible = CFileFindFunctions::find(
				TMGetFileSettings()->dtConfigurator.getFullPathName() );

			fInit = FALSE;
		}
	}

	pCmdUI->Enable( visible );	
}


void CMainFrame::OnViewUnitExtProp()
{
	const CAppState::Type_t appType = CAppState::getInstance().getAppType();
	if (CAppState::APP_TYPE_INSTALLATION==appType)
	{
		CDlgUnitExtProp* const pDlg = CDlgUnitExtProp::getInstance( );
		if (pDlg)
		{
			pDlg->Show( );
		}
	}
}

void CMainFrame::OnUpdateViewUnitExtProp(CCmdUI *pCmdUI)
{
	BOOL fEnable = FALSE;
	const CAppState::Type_t appType = CAppState::getInstance().getAppType();

	if (TMIsConnected() && CAppState::APP_TYPE_INSTALLATION==appType)
		fEnable = TRUE;

	pCmdUI->Enable(fEnable);
}

void CMainFrame::OnViewIrtxcodes()
{
	const CAppState::Type_t appType = CAppState::getInstance().getAppType();
	if (CAppState::APP_TYPE_INSTALLATION==appType)
	{
		if (CConfigIRTXFileInfo::getInstance().load() == TRUE)
		{
			CDlgViewIRTXCodes* const pDlg = CDlgViewIRTXCodes::getInstance( );
			if (pDlg)
			{
				pDlg->attach(&CConfigIRTXFileInfo::getInstance().getConfigFile());
				pDlg->Show( );
			}
		}
	}
}

void CMainFrame::OnUpdateViewIrtxcodes(CCmdUI *pCmdUI)
{
	BOOL fEnable = FALSE;
	const CAppState::Type_t appType = CAppState::getInstance().getAppType();

	if (CAppState::APP_TYPE_INSTALLATION==appType && 
		CConfigIRTXFileInfo::getInstance().isAvailable())
	{		
		fEnable = TRUE;		
	}

	pCmdUI->Enable(fEnable);
}

LRESULT CMainFrame::OnPropertySheetEnter(WPARAM wParam,LPARAM lParam)
{
	/**
	 * Property sheet van de nodes wordt geopend.
	 */

	return TRUE;
}

LRESULT CMainFrame::OnPropertySheetExit(WPARAM wParam,LPARAM lParam)
{
	/**
	 * Property sheet van de nodes werd gesloten.
	 */

	if (wParam == 0xFC)	/* Master Node Address */
	{	
		const CAppState::Type_t appType = CAppState::getInstance().getAppType();
		if (CAppState::APP_TYPE_INSTALLATION==appType)
		{
			// Configuratie bestand van de IRTX codes opnieuw laden.
			if (CConfigIRTXFileInfo::getInstance().load())
			{
				if (CDlgViewIRTXCodes::isActive())
				{
					CDlgViewIRTXCodes::getInstance( )->Update( );
				}
			}
		}
	}
	return TRUE;
}


void CMainFrame::OnAutoaudioSimple()
{
	CBindingConversionAutoOptions* const p = TMGetAutoBindingOptions();

	if (NULL!=p)
	{
		p->SetAutoAudioOption(AUDIO_AUTO_OPTION_SOURCE_SIMPLE);
	}
}

void CMainFrame::OnAutoaudioSrcinc()
{
	CBindingConversionAutoOptions* const p = TMGetAutoBindingOptions();

	if (NULL!=p)
	{
		p->SetAutoAudioOption(AUDIO_AUTO_OPTION_SOURCE_INC);
	}
}

void CMainFrame::OnUpdateAutoaudioSimple(CCmdUI *pCmdUI)
{
	CBindingConversionAutoOptions* const p = TMGetAutoBindingOptions();

	if (NULL!=p)
	{
		if (p->GetAutoAudioOption() == AUDIO_AUTO_OPTION_SOURCE_SIMPLE) 
		{
			pCmdUI->SetCheck(1);
		}
		else 
		{
			pCmdUI->SetCheck(0);
		}
	}
}

void CMainFrame::OnUpdateAutoaudioSrcinc(CCmdUI *pCmdUI)
{
	CBindingConversionAutoOptions* const p = TMGetAutoBindingOptions();

	if (NULL!=p)
	{
		if (p->GetAutoAudioOption() == AUDIO_AUTO_OPTION_SOURCE_INC) 
		{
			pCmdUI->SetCheck(1);
		}
		else 
		{
			pCmdUI->SetCheck(0);
		}
	}
}

LONG CMainFrame::OnApplicationUserMessage(WPARAM wParam,LPARAM lParam)
{
	switch(wParam)
	{
		case USERMESSAGE_UPDATE_DATABASE_NEEDED:
		{			
			CString s(_T("The configuration has been send succesfully.\n"
						 "These configuration settings only work after the master has executed an 'Update database'."));

			AfxMessageBox(s,MB_ICONINFORMATION|MB_OK);

		} break;
	}
	
	return TRUE;
}


// Zoeken naar het ip-address via het logisch node address.
// 1) Device node database ?
// 2) In de lijst van de node services.
// 3) onder de config map ?
BOOL CMainFrame::findNodeIPAddress(unsigned long ulPhysicalNodeAddress, BYTE bNodeAddress,CString* pstrIPAddress) {

	// 1) Device node database ?
	// Zoeken in de lijst van de gevonden devices.
	// Dit gebuerd via het physical node address.
	if (ulPhysicalNodeAddress > 0 && ulPhysicalNodeAddress < 0xffffff) {
		AppUDPDevices& refDevices = this->getAppUDPServerCfg().getDeviceList();
		const AppUDPDevice* pDevice = NULL;
		if (refDevices.find(ulPhysicalNodeAddress,&pDevice)) {
			*pstrIPAddress = pDevice->getIpAddress();
			return TRUE;
		}						
	}	


	// 2) Zoeken in de lijst van de node services.
	// Sedert V16.28
	// Zoeken in de nodeservices...
	int dummyPort = 0;
	CString szTemp;
	if (AppRemoteCfgNodes_Util::getNodeLocalHostName(this->getAppRemoteCfgNodeServices(),bNodeAddress, &szTemp)) {
		*pstrIPAddress = szTemp;
		return TRUE;
	}	

	
	// 3) Onder de config map.
	// Het ip-address zoeken in een bestand. 
	// De configurator slaat het ip-address op van een node onde de datamap.
	// Voorbeeld voor de master: 'TouchscreenIPAddress_fc.txt'
	BOOL result = FALSE;
	CString szFileName;
	const char* pathName = TMGetFileSettings()->GetFullyQualifiedPathName();
	if (NULL != pathName) {
		CString szSubDir;
		szSubDir.LoadString(IDS_FILENAME_SUBDIR_DATA);		
		szFileName.Format("%s%s\\TouchscreenIPAddress_%02x.txt",pathName,szSubDir,bNodeAddress);
	}
	if (!szFileName.IsEmpty()) {						
		FILE* const fs = fopen(szFileName,"r");
		if (fs != NULL) {		 
			char buffer[128];
			memset(buffer,0,sizeof(buffer));		
			if (fread(buffer,sizeof(char),sizeof(buffer)-1,fs)>0) {			
				int v[4] = { 0,0,0,0 };
				if (4 == sscanf(buffer,"%d.%d.%d.%d",&v[0],&v[1],&v[2],&v[3])) {
					pstrIPAddress->Format("%d.%d.%d.%d",(v[0] & 0xff), (v[1] & 0xff), (v[2] & 0xff), (v[3] & 0xff));
					result = TRUE;
				}			
			}
			fclose(fs);
		}	
	}
	return result;
}

// Tonen van de dialoog om firmware updates uit te voeren.
// Wordt assynchroon aangeroepen via postmessage. (WM_SHOW_DIALOG_FIRMWAREUPDATE)
// WParam: Node address van de node of 0xff.
// lParam: niet gebruikt.
LONG CMainFrame::OnShowDialogFirmwareUpdate(WPARAM wParam,LPARAM lParam) {

	// TODO: TODO_FIRMWARE_SSH_UPDATE_INSTANCE
	DUOTECNO::IAppRemoteCfgApi* m_pWorkerHttp = appremotecfg_create();
	if (NULL == m_pWorkerHttp) {
		return FALSE;
	}
	// TODO -- TOT HIER !!!!!!!!!!!!!!!!!!!!

	
	// Zoeken naar het ip-address van een node.
	// wParam bevat het logisch node address, kan ook 0xff zijn...	
	CString strIpAddress;	
	CString strDlgTitle;
	const int nodeAddress = (wParam & 0xff);
	if (nodeAddress>0 && nodeAddress<0xff) {
		CString strNodeName;
		unsigned long ulPhysicalNodeAddress = 0xffffff;
		CCanNode *pNode = NULL;		
		int i;
		if (TMGetDocumentNodes()->FindNode(nodeAddress,&i,&pNode)) {
			strNodeName = pNode->getNodeName();
			ulPhysicalNodeAddress = pNode->getPhysicalAddress();		
		}
		if (!findNodeIPAddress(ulPhysicalNodeAddress,nodeAddress,&strIpAddress)) {
			strIpAddress.Empty();
		}
		if (!strNodeName.IsEmpty()) {
			strDlgTitle.Format("Update firmware: Node '%s'",strNodeName);
		}
	}


	CDlgFirmwareUpdate dlg(this,strDlgTitle);						// Dialoog
	CDlgFirmwareUpdate_Host dlgHost(dlg);							// Events van de worker naar de host.
	AppFirmwareUpdaterWorker worker(*m_pWorkerHttp, dlgHost);		// Aanmaken van de worker.

	// Initialisatie van de node dialoog.
	dlg.init(worker,strIpAddress);

	// Openen van een modale dialoog... 
	INT_PTR nResponse = dlg.DoModal();

	// TODO: TODO_FIRMWARE_SSH_UPDATE_INSTANCE
	if (NULL != m_pWorkerHttp) {
		appremotecfg_destroy(&m_pWorkerHttp);
	}
	// TODO -- TOT HIER !!!!!!!!!!!!!!!!!!!!

	return TRUE;
}

void CMainFrame::OnUpdateMenuToolsRemoteCfg(CCmdUI *pCmdUI) {
#if (USE_TOOLS_APPREMOTECFG == 1)
	const BOOL visible = (CAppState::APP_TYPE_INSTALLATION == CAppState::getInstance().getAppType()) && 
						 (TMGetDisplaySettings()->IsModeExtended());
	pCmdUI->Enable(visible);	
#else
	pCmdUI->Enable(FALSE);	
#endif 
}

void CMainFrame::OnMenuToolsRemoteCfg() {	
#if (USE_TOOLS_APPREMOTECFG == 1)
	if (CAppState::APP_TYPE_INSTALLATION==CAppState::getInstance().getAppType()) {
		CDlgToolsRemoteCfg* const pDlg = CDlgToolsRemoteCfg::getInstance( );
		if (pDlg) {
			pDlg->Show( );
		}
	}
#endif
}

void CMainFrame::OnViewOptionsSoftwareUpdate() {
	CDlgOptions dlg;
	dlg.DoModal();
}

void CMainFrame::OnUpdateViewOptionsSoftwareUpdate(CCmdUI *pCmdUI) {
	const BOOL visible = (CAppState::APP_TYPE_INSTALLATION == CAppState::getInstance().getAppType());
	pCmdUI->Enable(visible);	
}

void CMainFrame::OnHelpSoftwareupdates()
{
	CDlgSoftwareUpdate dlg;
	dlg.DoModal();
}

void CMainFrame::OnNetworkLanDevices()
{
	CDlgUDPLanDevices::Show(this,this->getAppUDPServerCfg());
}

void CMainFrame::OnNetworkLanSettings() {
	CDlgUDPLANSettings dlg;
	if (IDOK == dlg.DoModal()) {
		const CString strNwkAdapter = dlg.getNetworkAdapterAddress();
		this->getAppUDPServerCfg().selectNetworkAdapter(strNwkAdapter);
	}
}


// Sedert V16.29: Openen van de binding manuals.
void CMainFrame::OnHelpBindingexamples() {
	TCommandShell<CNotifyError_PdfOpener> cmd(TMGetFileSettings()->m_bindingExamples.getFullPathName(),"open");
	cmd.Execute( );
}
void CMainFrame::OnHelpModbusfunctions() {
	TCommandShell<CNotifyError_PdfOpener> cmd(TMGetFileSettings()->m_SensModbusFunctions.getFullPathName(),"open");
	cmd.Execute( );
}
void CMainFrame::OnHelpManualHardware() {
	TCommandShell<CNotifyError_PdfOpener> cmd(TMGetFileSettings()->m_manualHardware.getFullPathName(),"open");
	cmd.Execute( );
}
void CMainFrame::OnHelpManualSoftwarePart1() {
	TCommandShell<CNotifyError_PdfOpener> cmd(TMGetFileSettings()->m_manualSoftwarePart1.getFullPathName(),"open");
	cmd.Execute( );
}
void CMainFrame::OnHelpManualSoftwarePart2() {
	TCommandShell<CNotifyError_PdfOpener> cmd(TMGetFileSettings()->m_manualSoftwarePart2.getFullPathName(),"open");
	cmd.Execute( );
}
void CMainFrame::OnHelpManualSoftwareExpert() {
	TCommandShell<CNotifyError_PdfOpener> cmd(TMGetFileSettings()->m_manualSoftwareExpert.getFullPathName(),"open");
	cmd.Execute( );
} 

void CMainFrame::OnUpdateHelpBindingexamples(CCmdUI* pCmdUI) {
	pCmdUI->Enable(TMGetFileSettings()->m_bindingExamples.found());	
}
void CMainFrame::OnUpdateHelpManualHardware(CCmdUI* pCmdUI) {
	pCmdUI->Enable(TMGetFileSettings()->m_manualHardware.found());
}
void CMainFrame::OnUpdateHelpManualSoftwarePart1(CCmdUI* pCmdUI) {
	pCmdUI->Enable(TMGetFileSettings()->m_manualSoftwarePart1.found());
}
void CMainFrame::OnUpdateHelpManualSoftwarePart2(CCmdUI* pCmdUI) {
	pCmdUI->Enable(TMGetFileSettings()->m_manualSoftwarePart2.found());
}
void CMainFrame::OnUpdateHelpManualSoftwareExpert(CCmdUI* pCmdUI) {
	pCmdUI->Enable(TMGetFileSettings()->m_manualSoftwareExpert.found());
}
void CMainFrame::OnUpdateHelpModbusfunctions(CCmdUI *pCmdUI) {
	pCmdUI->Enable(TMGetFileSettings()->m_SensModbusFunctions.found());	
}
void CMainFrame::OnViewEnergymanagementconfig() {
	using namespace DUOTECNO::PLUGIN;
	IConfigToolApp::getInstance().show(VIEW_ENERGYMANAGER, 0xff);
}


void CMainFrame::OnViewDaliBusconfig() {
	const BYTE bNodeAddress = 0xFF;
	this->showDaliSubbusConfiguration(0xFF);
}

LONG CMainFrame::onApplicationConfiguratorDaliOpenCfg(WPARAM wParam, LPARAM lParam) {
	const BYTE bNodeAddress = (BYTE)wParam & 0xFF;
	this->showDaliSubbusConfiguration(bNodeAddress);	
	return TRUE;
}

// Mogelijk wanneer er smartbox ultra nodes aanwezig zijn.
// zie updateAppStateViewModes( )
void CMainFrame::OnUpdateViewDaliBusconfig(CCmdUI* pCmdUI) {
	const BOOL visible = ((CAppState::getInstance().getEnableMenuItemDaliCfg() == true) ? TRUE : FALSE);
	pCmdUI->Enable(visible);
}


// Oppassen:
// Dit bericht mag enkel syncroon verstuurd worden want bevat een ptr naar een CCanNode.
LONG CMainFrame::OnApplicationNodeShowServices(WPARAM wParam,LPARAM lParam) {
	CCanNode* const pNode = (CCanNode*)(wParam);
	if (NULL == pNode) {
		ASSERT(pNode != NULL);
		return TRUE;
	}
	AppRemoteCfgNodes& refCfg = this->getAppRemoteCfgNodeServices();
	CAppDlgNodeServicesCfg_Host host(refCfg);
	CDlgNodeServicesCfg dlg(NULL,&host,pNode);
	dlg.DoModal();

	return TRUE;
}

// Sedert V16.40
// WM_APPLICATION_CONFIGURATOR_DALI_RELOAD + node address
// Het Json export bestand met gegevens van de DALI SubBus is gewijzigd en opgeslagen.
// IPC bericht versturen naar de Duotecno Configurator.
LONG CMainFrame::onApplicationConfiguratorDaliReload(WPARAM wParam,LPARAM lParam) {
	using namespace DUOTECNO::PLUGIN;
	ASSERT(wParam>=0 && wParam<=0xFF);
	const BYTE bNodeAddress = (BYTE) wParam;
	IConfigToolApp::getInstance().load(VIEW_DALI,bNodeAddress);
	return TRUE;
}

LONG CMainFrame::onApplicationBrowseSingleNode(WPARAM wParam, LPARAM lParam) {
	if (wParam < 0 || wParam > 0xff) {
		ASSERT(wParam>=0 && wParam<=0xFF);
		return FALSE;
	}
	if (FALSE == CComApplication::GetInstance()->IsConnected()) {
		ASSERT(0);	// GUI is disabled wanneer we niet geconnecteerd zijn....
		return FALSE;
	}
	if (CAppState::APP_STATE_IDLE != CAppState::getInstance().getAppState()) {
		ASSERT(0);	// GUI is disabled wanneer we bezig zijn met een taak.
		return FALSE;
	}

	AppCfgBrowseInfo& ref = AppCfgBrowseInfo::getInstance();
	if (AppCfgBrowseInfo::MODE_IDLE != ref.getMode()) {
		ASSERT(0);		// Mag niet voorkomen, na het uitvoeren van een browse single node moet deze op MODE_IDLE komen.
		return FALSE;
	}

	const BYTE bNodeAddress = (BYTE) wParam;	
		
	{
		// Controleren of de node aanwezig is in de node database.
		int iEntryNR = 0;
		CCanNode* pNode = NULL;
		if (FALSE == TMGetDocumentNodes()->FindNode(bNodeAddress, &iEntryNR, &pNode)) {
			ASSERT(0);
			return FALSE;
		}
	}

	if (TRUE == lParam) {	
		// Vragen of er moet gebrowsed worden op de installatie.
		const CString strHeader("Browse single node and check for new units?");
		const CString strMessage("The Duotecno node in the CAN network is being scanned to read "
								 "its properties and check for new units.\r\n\r\n"
								 "Would you like to proceed?");
		if (IDNO == this->MessageBox(strMessage, strHeader, MB_YESNO | MB_ICONQUESTION)) {
			return FALSE;
		}
	}

	// Info invullen + Starten met de fsm.
	ref.setMode(AppCfgBrowseInfo::MODE_BROWSE_SINGLE_NODE);
	ref.setNodeAddress(bNodeAddress);
	TMGetGlobalHSM()->EvOperationBrowseSingleNode( );
	return TRUE;
}

LONG CMainFrame::OnApplicationUnitConfigDone(WPARAM wParam, LPARAM lParam) {
	if (wParam < 0 || wParam > 0xff) {
		ASSERT(wParam>=0 && wParam<=0xFF);
		return FALSE;
	}
	if (FALSE == CComApplication::GetInstance()->IsConnected()) {	
		const CString strHeader("Unit configuration uploaded.");
		const CString strMessage("After uploading the unit configuration, it is recommended to refresh the node. "
							     "For the moment the project is not opened in online mode.\r\n\r\n"
								 "Be sure next time to connect online and browse the Duotecno CAN-bus system. "
								 "When the new units are found in the system the units are added to the configuration software and are ready for further setup.");
		this->MessageBox(strMessage, strHeader, MB_OK | MB_ICONHAND);
		return FALSE;
	}
	const CString strHeader("Unit configuration uploaded. Search for new units?");
	const CString strMessage("After uploading the unit configuration, it is recommended to refresh the node. "
						     "This process scans the node via the CAN bus to detect any new units. "
							 "Once detected, the units are added to the configuration software and are ready for further setup.\r\n\r\n"
						     "Would you like to proceed with this step now?");
	if (IDNO == this->MessageBox(strMessage, strHeader, MB_YESNO | MB_ICONQUESTION)) {
		return FALSE;
	}
	this->PostMessage(WM_APPLICATION_BROWSE_SINGLE_NODE,wParam,FALSE);	
	return TRUE;
}

LONG CMainFrame::OnApplicationDaliBridgeConfigDone(WPARAM wParam, LPARAM lParam) {
	if (wParam < 0 || wParam > 0xff) {
		ASSERT(wParam>=0 && wParam<=0xFF);
		return FALSE;
	}
	const BYTE bNodeAddress = (BYTE) wParam;

	// Inladen van de DALi bridge config.
	TMLoadDaliBridgeConfig(bNodeAddress);
	// Extra unit info die weergegeven wordt in de dialoog.
	TMSetDatabaseUnitExtraInfo(bNodeAddress);
	// Upgraden van de bindings. Zorgt ervoor dat de bestaande bindings het juiste type icoon tonen.
	TMUpgradeBindingUnitTypes(bNodeAddress);
	// Updaten van deze node.
	this->UpdateViews(bNodeAddress);		

	return TRUE;
}

LONG CMainFrame::OnCopyData(WPARAM wParam, LPARAM lParam) {
	PCOPYDATASTRUCT pCDS = (PCOPYDATASTRUCT)lParam;
	const char* const str = (char*)pCDS->lpData;
	if (NULL == str) {
		return FALSE;
	}
	
	CString receivedData(str, pCDS->cbData);
	if (receivedData.IsEmpty()) {
		return FALSE;
	}

	using namespace DUOTECNO::PLUGIN;
	ICommandStringParser* parser = IConfigToolApp::getInstance().parseReceived(receivedData);
	if (NULL == parser) {
		return FALSE;
	}
	const Command command = parser->getCommand();
	const View view = parser->getView();
	const int nodeAddress = parser->getNodeAddress();
	switch(command) {
		case COMMAND_UPOAD_DONE: {
			switch(view) {
				case VIEW_UNITS: {
					this->PostMessage(WM_APPLICATION_UNITCONFIG_DONE,nodeAddress); 
				} break;
				case VIEW_DALI: {
					this->PostMessageA(WM_APPLICATION_DALIBRIDGECONFIG_DONE,nodeAddress);	
				} break;
				default: {
					// absorbe... 
				} break;
			}					 
		} break;
		default: {
			// Absorbe... 
		} break;
	}

	if (NULL != parser) {
		delete parser;
	}
	return TRUE;
}

// wParam = Node adres
LONG CMainFrame::OnApplicationNotifyAssignedNodeAddress(WPARAM wParam, LPARAM lParam) {
	const int nodeAddress = (int) wParam;
	if (nodeAddress<0 || nodeAddress>0xFF) {
		ASSERT(0);
		return TRUE;
	}
	// Weergave scrollview.
	CString szMessage;			
	szMessage.Format(_T("Assigned logical address 0x%02x"),nodeAddress);									
	this->DisplayScrollMessage(szMessage);

	// Indien de problem solve wizard geopend is (via een modale dialoog) het bericht op zijn message queue plaatsen.
	if ((NULL != this->m_problemSolveWizard) && (NULL != this->m_problemSolveWizard->GetSafeHwnd())) {
		this->m_problemSolveWizard->PostMessage(CPropertyWizard::WM_NOTIFY_ADDRESS_ASSIGNED,nodeAddress);
	}
	return TRUE;
}

// Optimalisatie: Controle doen of een node extra units bevat. Standaard return waarde = true.
// Nodes die niet herkend worden of bij nodes met oude firmware die niet meer uitgeleverd wordt altijd 'true'
// als returnwaarde gegeven.
static bool checkNodeHoldsExtraCreatedUnits(CCanNode& refNode) {
	bool result = true;
	if (CNodeConfigInfo_SmartboxUltra::isType(&refNode)) {		
		const int nrOfUnits = refNode.getNROfUnits();	
		const int nrOfInternalUnits = CNodeConfigInfo_SmartboxUltra::getUnitAddressLastInternalUnit(&refNode)+1;
		if (nrOfUnits <= nrOfInternalUnits) {
			result = false;	// geen upload nodig: geen extra units bijgemaakt.
		}
	}
	else if (CNodeConfigInfo_Touchscreen_V66::isType(&refNode)) {
		const int nrOfUnits = refNode.getNROfUnits();	
		const int nrOfInternalUnits = CNodeConfigInfo_Touchscreen_V66::getUnitAddressLastInternalUnit(&refNode)+1;
		if (nrOfUnits <= nrOfInternalUnits) {
			result = false;	// geen upload nodig: geen extra units bijgemaakt.
		}
	}
	else if (CNodeConfigInfo_DT10_AU::isType(&refNode)) {
		const int nrOfUnits = refNode.getNROfUnits();	
		const int nrOfInternalUnits = CNodeConfigInfo_DT10_AU::getUnitAddressLastInternalUnit(&refNode)+1;
		if (nrOfUnits <= nrOfInternalUnits) {
			result = false;	// geen upload nodig: geen extra units bijgemaakt.
		}
	}
	else if (CNodeConfigInfo_DT18_PRO::isType(&refNode)) {
		const int nrOfUnits = refNode.getNROfUnits();	
		const int nrOfInternalUnits = CNodeConfigInfo_DT18_PRO::getUnitAddressLastInternalUnit(&refNode)+1;
		if (nrOfUnits <= nrOfInternalUnits) {
			result = false;	// geen upload nodig: geen extra units bijgemaakt.
		}
	}

	// ... andere types?

	return result;
}

// sedert V16.47: CHANGES_UPLOAD_UNITS
//
// Dit bericht wordt verstuurd vanuit de dialogen van het startscherm en de problem solve wizard.
// Er was een probleem met dit node adres en dit werd hersteld in de problem solve wizard.
// Deze wordt aangeroepen bij het ontdekken van Pre-Browse errors.
//
// Via deze kunnen er taken toegevoegd worden die gebeuren voor het starten van de browse for nodes.
// 
// Om te voorkomen dat we incompatible nodes hebben worden de units automatisch doorgestuurd voor het starten
// van de browse for nodes.
//
// 
// wParam = Node adres
// lParam = fysisch adres (Optioneel).
LONG CMainFrame::OnApplicationNotifyProblemSolveRestoredNodeAddress(WPARAM wParam, LPARAM lParam) {
	const int nodeAddress = (int) wParam;
	if (nodeAddress<0 || nodeAddress>0xFF) {
		ASSERT(0);
		return TRUE;
	}
	int dummy=0;
	CCanNode* pNode = NULL;
	if (FALSE == TMGetDocumentNodes()->FindNode(nodeAddress,&dummy,&pNode)) {
		return FALSE;
	}

	// Controleren van de mogelijkheden van deze node.
	CConfigAlgViewUnits algViewUnits(pNode,NULL);
	if (!algViewUnits.IsSupported()) {
		return FALSE;
	}	

	// Controle of units aangemaakt werden.
	// Standaard geeft deze functie 'true' terug. (Nodig voor onbekende node types.)
	const bool flagUploadUnitsNeeded = checkNodeHoldsExtraCreatedUnits(*pNode);
	if (!flagUploadUnitsNeeded) {
		return FALSE;
	}

	if (algViewUnits.IsUploadCanSupported()) {		
		if (algViewUnits.IsFileFound()) {
			// taak toevoegen aan de applicatie.
			// Na het sluiten van de Problem solve wizard worden deze taken uitgevoerd.
			CAppFsmNodeTaskEntries& refTaskEntries = CAppState::getInstance().getTaskEntries();
			const CAppFsmNodeTaskEntry taskEntry(CAppFsmNodeTaskEntry::APPFSM_TASK_SENDUNITS,nodeAddress);

			// een taak mag maar 1x toegevoegd worden.
			if (!CAppFsmNodeTaskEntries_Utils::find(refTaskEntries, taskEntry)) {
				refTaskEntries.addTask(taskEntry);
			}
		}
	}
	else {
		// TODO_CHANGES_UPLOAD_UNITS
		// DT10-AU: Units kunnen niet doorgestuurd worden via de CAN-bus.
		//
		// Momenteel kunnen we van hieruit niet de configurator openen omdat het fysisch adres  in de nodeconfiginfo.txt
		// nog niet aangepast is naar het nieuwe adres.
		// 
		// Een mogelijke optie zou zijn om de IPC call uit te breiden en het fysisch adres van deze module meegeven.
		// Een ander probleem is dat mogelijk de IP-instellingen nog niet ok zijn.
		//
		// 
		// Beste oplossing is momenteel om een message box te tonen met de melding dat er nog units moeten aangemaakt worden
		// via de configurator.
	}
	return TRUE;
}


void CMainFrame::showDaliSubbusConfiguration(BYTE bNodeAddress) {
	using namespace DUOTECNO::DALI;
	
	static CAppDlgDaliWorker appDaliWorker;

	this->KillTimer(TIMERID_MAINFRAME_APPDALI_POLL);

	// Oplossing BUG
	// Moeten hier de CAppDaliManager meegeven in de initDaliManager( )
	// Mogen deze niet meegeven in de ctor omdat de dalimanager opnieuw aangemaakt wordt bij een nieuw project.
	// 
	// Aanmaken van de implementaties + koppelen van de homeclients.
	// Als er geen instanties aanwezig zijn return( )
	CAppDaliManager& refAppDaliMananger = CAppDaliManager::getInstance();
	const BOOL result = appDaliWorker.initDaliManager(refAppDaliMananger);
	if (FALSE == result) {
		ASSERT(TRUE == result);
		return;
	}
	CPropertySheetDali* pInstance = CPropertySheetDali_Holder::Get();
	if (NULL == pInstance) {
		// Indien de dialoog nog niet geopend is dan moet deze geopend 
		// worden met het geselecteerd node address.
		appDaliWorker.setCurrentNodeAddress(bNodeAddress);

		// Aanmaken van de nieuwe instantie van het property sheet.
		pInstance = CPropertySheetDali_Holder::Create(this, appDaliWorker);
	}
	if (NULL == pInstance) {
		ASSERT(NULL != pInstance);
		return;
	}	

	this->SetTimer(TIMERID_MAINFRAME_APPDALI_POLL,TIMER_MAINFRAME_APPDALI_POLL_INTERVAL_MSEC,(TIMERPROC) NULL);
	pInstance->ShowWindow(SW_SHOWNORMAL);

}