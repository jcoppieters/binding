// ComApplication.cpp : implementation file
//
// TM_CHANGES_ALARM_ARITECH: Event handler EV_UNITALARMSTATUS om het verwerken van ATTR_CONFIG.
// Changes TM20120403: Nieuwe attributen voor de sens unit.
//
//////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "ComApplication.h"
// -------------------------------------------------------------
#include "MainFrm.h"
#include "communication/communication.h"
#include "communication/communicationStatus.h"
#include "communication/communicationconnectfsm.h"
#include "ComApplicationFactory.h"
#include "NodeDatabase.h"
#include "BindingFileFsm.h"
#include "BindingFile1.h"
#include "MyWindowTimers.h"				//TEMP ...
#include "MyWindowMessages.h"
#include "nodemess.h"
#include "MemoryBitmapInfo.h"
#include "MemoryDumpInfo.h"
#include "NodeRuntimeStatus.h"
#include "MessageError.h"
#include "Message_EV_INIT.h"
#include "CommunicationLogfile.h"
#include "RequestForNodeStatus.h"
#include "RenameNodeUnitsEngine.h"
#include "BindingsReadWriteFSM.h"
#include "NodeLCDScreenmapFSMMaster.h"
#include "MemoryDumpMasterFSM.h"
#include "FSMNicErrors.h"

#include "App/AppBrowsePnp/AppBrowsePnP.h"

#include "App\AppCfg\AppCfgMM\ConfigMultiMediaInfo.h"
#include "App\AppCfg\AppCfgMM\ConfigMultiMediaFSMMaster.h"
#include "App\AppCfg\AppCfgUnits\ConfigUnitFSMInfo.h"				// since 0x0560
#include "App\AppCfg\AppCfgUnits\ConfigUnitCreateInfo.h"			// since 0x0560
#include "App\AppCfg\AppCfgUnits\ConfigUnitFSMMaster.h"				// since 0x0560
#include "App\AppCfg\AppCfgMoods\ConfigMoodInfo.h"					// since 0x0560
#include "App\AppCfg\AppCfgMoods\ConfigMoodFSMMaster.h"				// since 0x0560
#include "App\AppCfg\AppCfgIRRX\ConfigIRRXFSMMaster.h"				// since 0x0560
#include "App\AppCfg\AppCfgIRRX\ConfigIRRXInfo.h"					// since 0x0560
#include "App\AppCfg\AppCfgschedule\ConfigScheduleFSMMaster.h"		// since 0x0560
#include "App\AppCfg\AppCfgschedule\ConfigScheduleInfo.h"			// since 0x0560
#include "App\AppCfg\AppCfgCanFtp\ConfigCanFtp.h"					// since 0x0562
#include "App\AppCfg\AppCfgCanFtp\ConfigCanFtp_info.h"				// since 0x0562
#include "App\AppCfg\AppCfgIRTX\configIRTX.h"						// since 0x0562
#include "App\AppCfg\AppCfgsmap\configsmappageinfofsmmaster.h"		// since 0x0571
#include "App\AppCfg\AppCfgsmap\configsmappageinfoinfo.h"			// since 0x0571
#include "App\AppCfg\AppCfgsmap\configsmapmenuinfo.h"				// since 0x0571
#include "App\AppCfg\AppCfgsmap\configsmapmenufsmmaster.h"			// since 0x0571
#include "App\AppCfg\AppCfgsmap\configsmapvroominfo.h"				// since 0x0610
#include "App\AppCfg\AppCfgsmap\configsmapvroomfsmmaster.h"			// since 0x0610
#include "app\AppCfg\AppCfgGatewayU\configgatewayuinfo.h"			// since 0x0620
#include "app\AppCfg\AppCfgGatewayU\configgatewayufsmmaster.h"		// since 0x0620
#include "app\AppCfg\AppCfgRoomCtrlAV\ConfigRoomCtrlAVInfo.h"		// since 0x0625
#include "app\AppCfg\AppCfgRoomCtrlAV\ConfigRoomCtrlAVFSMMaster.h"	// since 0x0625
#include "app\appcfg\appcfgbosest\configbosestcfginfo.h"			// since 0x0802
#include "app\appcfg\appcfgbosest\configbosestfsmmaster.h"			// since 0x0802
#include "app\appcfg\appcfguictrl\configappuictrlinfo.h"			// since 0x0804
#include "app\appcfg\appcfguictrl\configappuictrlfsmmaster.h"		// since 0x0804
#include "app\appcfg\appCfgSonosSrc\configSonosSrcItems.h"			// Since 0x0808
#include "app/appcfg/appcfgsmaplite/appcfgsmaplitefsmmaster.h"		// since 0x0C05
#include "app/appcfg/appcfgsmaplite/appcfgsmapliteinfo.h"			// since 0x0C05
#include "app\appCfg\AppCfginfo\cfginfoproductID_def.h"				// Nodig voor de product identifiers
#include "app\appstate\appstate.h"									// Sedert V16.14: Toelaten van de cfg via de ts.

#include "utils/win32/NotifyError.h"
#include "utils/app/DisplayDevice.h"

#include "AppLogging.h"
// ---------------------------------------
#include "TMGlobals.h"
using namespace TMNodeDatabase;
using namespace TMCommunication;			// Tijdelijk...
using namespace TMGlobalSettings;			// Tijdelijk...
// ----------------------------------------

// ---------------------------------------
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
enum { SEGMENTED_BLOCK_LENGTH = 110 };
/////////////////////////////////////////////////////////////////////////////
//#define DEBUG_SERIAL_RXMESSAGES
/*
#ifdef DEBUG_SERIAL_RXMESSAGES

	#define SERIAL_TRACE(x)			TRACE(x)
	#define SERIAL_TRACE1(x,y)		TRACE1(x,y)
	#define SERIAL_TRACE2(x,y,z)	TRACE2(x,y,z)
	#define SERIAL_TRACE3(x,y,z,w)	TRACE3(x,y,z,w)
#else
	#define SERIAL_TRACE(x)			
	#define SERIAL_TRACE1(x,y)		
	#define SERIAL_TRACE2(x,y,z)
	#define SERIAL_TRACE3(x,y,z,w)		
#endif
*/
/////////////////////////////////////////////////////////////////////////////
CComApplication* CComApplication::m_pThis = NULL;
CComApplicationFactory*	CComApplication::m_pFactory = NULL;
/////////////////////////////////////////////////////////////////////////////
void CComApplication::Initialise(CWnd *parent)
{
	try
	{
		if (m_pThis == NULL)
		{
			m_pThis = new CComApplication(parent);
			m_pThis->Create();

			ASSERT(m_pThis->GetSafeHwnd());

			m_pFactory = CComApplicationFactory::Create(m_pThis);					
		}
	}
	catch (CMemoryException *e)
	{
		e->ReportError();
		e->Delete();

		exit(-1);
	}
}

void CComApplication::Destroy()
{
	CComApplicationFactory::Destroy();
	m_pFactory = NULL;

	if (m_pThis) {
		if (m_pThis->m_pCommunicationObject) {			
			m_pThis->m_pCommunicationObject->closeConnections();
		}
		delete m_pThis;
		m_pThis = NULL;
	}
}
/////////////////////////////////////////////////////////////////////////////
const CMessageHandler CComApplication::MessageHandlers[256] =
{
	CMessageHandler(0x00,0),								// 0x00
	CMessageHandler(EV_UNITCONTROLTOGGLE,0),				// 0x01
	CMessageHandler(EV_UNITCONTROLPULSTOGGLE,0),			// 0x02
	CMessageHandler(EV_UNITCONTROLPULS,0),					// 0x03
	CMessageHandler(EV_UNITCONTROLSTATUS,	
					&CComApplication::MessageHandler_EV_UNITCONTROLSTATUS),		// 0x04
	CMessageHandler(EV_UNITDIMSTATUS,		
					&CComApplication::MessageHandler_EV_UNITDIMSTATUS),	// 0x05
	CMessageHandler(EV_UNITSWITCHSTATUS,	
					&CComApplication::MessageHandler_EV_UNITSWITCHSTATUS),	// 0x06
	CMessageHandler(EV_UNITSENSSTATUS,		
					&CComApplication::MessageHandler_EV_UNITSENSSTATUS),	// 0x07
	CMessageHandler(EV_UNITCONFIG,
					&CComApplication::MessageHandler_EV_UNITCONFIG),	// 0x08
	CMessageHandler(EV_UNITNAME,
					&CComApplication::MessageHandler_EV_UNITNAME),		// 0x09
	CMessageHandler(EV_NODENAME,
					&CComApplication::MessageHandler_EV_NODENAME),		// 0x0a
	CMessageHandler(EV_BINDINGRESPONSE,0),					// 0x0b
	CMessageHandler(EV_NODECONFIG,
					&CComApplication::MessageHandler_EV_NODECONFIG),	// 0x0c
	CMessageHandler(EV_NODERESOURCES,
					&CComApplication::MessageHandler_EV_NODERESOURCES),		// 0x0d
	CMessageHandler(EV_NODESTATUS,0),						// 0x0e
	CMessageHandler(EV_NODEPNPRESPONSE,0),					// 0x0f
	CMessageHandler(EV_NODEBINDINGENTRY,
					&CComApplication::MessageHandler_EV_NODEBINDINGENTRY),				// 0x10
	CMessageHandler(EV_MESSAGEERROR,
					&CComApplication::MessageHandler_EV_MESSAGEERROR),	// 0x11
	CMessageHandler(EV_INIT,
		&CComApplication::MessageHandler_EV_INIT),								// 0x12
	CMessageHandler(EV_NODEBINDINGSFILECONTROL,
					&CComApplication::MessageHandler_EV_NODEBINDINGSFILECONTROL),			// 0x13
	CMessageHandler(EV_UNITSELECTOR,0),						// 0x14
	CMessageHandler(EV_NODEEEPROMERASED,0),					// 0x15
	CMessageHandler(EV_NODETEXTSTRING,
					&CComApplication::MessageHandler_EV_NODETEXTSTRING),			// 0x16
	CMessageHandler(EV_UNITAUDIOSTATUS,		
					&CComApplication::MessageHandler_EV_UNITAUDIOSTATUS),						// 0x17
	CMessageHandler(EV_NODEDLLSTATUS,
					&CComApplication::MessageHandler_EV_NODEDLLSTATUS),			// 0x18
	CMessageHandler(EV_NODELCDSTATUS,0),					// 0x19
	CMessageHandler(EV_UNITDIRECTSET,0),					// 0x1a
	CMessageHandler(EV_NODEDIRECTSET,0),					// 0x1b
	CMessageHandler(EV_SLAVELCDBROWSINGDONE,0),				// 0x1c
	CMessageHandler(EV_NODELCDMASTERSTATUS,0),				// 0x1d
	CMessageHandler(EV_DATABASEMANAGMENT,
					&CComApplication::MessageHandler_EV_DATABASEMANAGMENT),		// 0x1e
	CMessageHandler(EV_NODEIRCODEDATABASECHANGED,0),		// 0x1f
	CMessageHandler(EV_NODEAVIRDEVICESDATABASECHANGED,0),	// 0x20
	CMessageHandler(EV_NODEAVIRDEVICESENDIRCODE,0),			// 0x21
	CMessageHandler(EV_UNITRC5IRRECEIVERSTATUS,		
					&CComApplication::MessageHandler_UnitStatus),	// 0x22
	CMessageHandler(EV_NODEAVIRMATRIXSTATUS,0),				// 0x23
	CMessageHandler(EV_FTPFILECONTROLDATA,
					&CComApplication::MessageHandler_EV_FTPFILECONTROLDATA),		// 0x24
	CMessageHandler(EV_FTPFILECONTROLSTATUS,
					&CComApplication::MessageHandler_EV_FTPFILECONTROLSTATUS),	// 0x25
	CMessageHandler(EV_UNITDUOSWITCHSTATUS,			
					&CComApplication::MessageHandler_EV_UNITDUOSWITCHSTATUS),	// 0x26
	CMessageHandler(EV_NODEPHYSICALADDRESS,
					&CComApplication::MessageHandler_EV_NODEPHYSICALADDRESS),		// 0x27 
	CMessageHandler(EV_PNPLOGICALADDRESSASSIGNED,
					&CComApplication::MessageHandler_EV_PNPLOGICALADDRESSASSIGNED),		// 0x28
	CMessageHandler(EV_LCDPMDBASEADDNODEENTRY,0),			// 0x29
	CMessageHandler(EV_LCDPMDBASEADDUNITENTRY,0),			// 0x2A 
	CMessageHandler(EV_LCDINFODIALOG,
					&CComApplication::MessageHandler_EV_LCDINFODIALOG),			// 0x2B 
	CMessageHandler(EV_IRCODEDATA,0),						// 0x2C 
	CMessageHandler(EV_UNITIRTXSTATUS,				
					&CComApplication::MessageHandler_UnitStatus),		// 0x2D 
	CMessageHandler(EV_RC5CODEMATCH,0),						// 0x2E 
	CMessageHandler(EV_IRCODEDATABASEERROR,0),				// 0x2F 
	CMessageHandler(EV_UNITSELECTORSTATUS,0),				// 0x30
	CMessageHandler(EV_UNITVIDEOMUXSTATUS,			
					&CComApplication::MessageHandler_UnitStatus),		// 0x31
	CMessageHandler(EV_UNITPS2INTELLIMOUSESTATUS,	
					&CComApplication::MessageHandler_UnitStatus),		// 0x32  
	CMessageHandler(EV_UNITSPEECHCOMMSTATUS,0),				// 0x33
	CMessageHandler(EV_LCDBUILDSCREENMAP,
					&CComApplication::MessageHandler_EV_LCDBUILDSCREENMAP),		// 0x34
	CMessageHandler(EV_EXTERNALCONFIG,
					&CComApplication::MessageHandler_EV_EXTERNALCONFIG),// 0x35
	CMessageHandler(EV_UNITAVMATRIXSTATUS,			
					&CComApplication::MessageHandler_EV_UNITAVMATRIXSTATUS),		// 0x36 : TM_AVMATRIX_INPUT
	CMessageHandler(EV_UNITALARMSTATUS,				
					&CComApplication::MessageHandler_EV_UNITALARMSTATUS),	// 0x37 : TM_CHANGES_ALARM_ARITECH
	CMessageHandler(0x38,0),								// 0x38
	CMessageHandler(0x39,0),								// 0x39
	CMessageHandler(EV_UNITENERGYMEASUREMENTSTATUS,
					&CComApplication::MessageHandler_EV_UNITENERGYMEASUREMENTSTATUS),		// 0x3A - IMPLEMENT_UNIT_ENERGY
	CMessageHandler(0x3B,0),								// 0x3B
	CMessageHandler(0x3C,0),								// 0x3C
	CMessageHandler(0x3D,0),								// 0x3D
	CMessageHandler(0x3E,0),								// 0x3E
	CMessageHandler(0x3F,0),								// 0x3F
	CMessageHandler(0x40,0),								// 0x40
	CMessageHandler(0x41,0),								// 0x41
	CMessageHandler(0x42,0),								// 0x42
	CMessageHandler(0x43,0),								// 0x43
	CMessageHandler(0x44,0),								// 0x44
	CMessageHandler(0x45,0),								// 0x45
	CMessageHandler(0x46,0),								// 0x46
	CMessageHandler(0x47,0),								// 0x47
	CMessageHandler(0x48,0),								// 0x48
	CMessageHandler(0x49,0),								// 0x49
	CMessageHandler(0x4A,0),								// 0x4A
	CMessageHandler(0x4B,0),								// 0x4B
	CMessageHandler(0x4C,0),								// 0x4C
	CMessageHandler(0x4D,0),								// 0x4D
	CMessageHandler(0x4E,0),								// 0x4E
	CMessageHandler(0x4F,0),								// 0x4F
	CMessageHandler(0x50,0),								// 0x50
	CMessageHandler(0x51,0),								// 0x51
	CMessageHandler(0x52,0),								// 0x52
	CMessageHandler(0x53,0),								// 0x53
	CMessageHandler(0x54,0),								// 0x54
	CMessageHandler(0x55,0),								// 0x55
	CMessageHandler(0x56,0),								// 0x56
	CMessageHandler(0x57,0),								// 0x57
	CMessageHandler(0x58,0),								// 0x58
	CMessageHandler(0x59,0),								// 0x59
	CMessageHandler(0x5A,0),								// 0x5A
	CMessageHandler(0x5B,0),								// 0x5B
	CMessageHandler(0x5C,0),								// 0x5C
	CMessageHandler(0x5D,0),								// 0x5D
	CMessageHandler(0x5E,0),								// 0x5E
	CMessageHandler(0x5F,0),								// 0x5F
	CMessageHandler(0x60,0),								// 0x60
	CMessageHandler(0x61,0),								// 0x61
	CMessageHandler(0x62,0),								// 0x62
	CMessageHandler(0x63,0),								// 0x63
	CMessageHandler(0x64,0),								// 0x64
	CMessageHandler(0x65,0),								// 0x65
	CMessageHandler(0x66,0),								// 0x66
	CMessageHandler(0x67,0),								// 0x67
	CMessageHandler(0x68,0),								// 0x68
	CMessageHandler(0x69,0),								// 0x69
	CMessageHandler(0x6A,0),								// 0x6A
	CMessageHandler(0x6B,0),								// 0x6B
	CMessageHandler(0x6C,0),								// 0x6C
	CMessageHandler(0x6D,0),								// 0x6D
	CMessageHandler(0x6E,0),								// 0x6E
	CMessageHandler(0x6F,0),								// 0x6F
	CMessageHandler(0x70,0),								// 0x70
	CMessageHandler(0x71,0),								// 0x71
	CMessageHandler(0x72,0),								// 0x72
	CMessageHandler(0x73,0),								// 0x73
	CMessageHandler(0x74,0),								// 0x74
	CMessageHandler(0x75,0),								// 0x75
	CMessageHandler(0x76,0),								// 0x76
	CMessageHandler(0x77,0),								// 0x77
	CMessageHandler(0x78,0),								// 0x78
	CMessageHandler(0x79,0),								// 0x79
	CMessageHandler(0x7A,0),								// 0x7A
	CMessageHandler(EV_NODEGATEWAYSTATUS,
			&CComApplication::MessageHandler_EV_NODEGATEWAYSTATUS),			// 0x7B
	CMessageHandler(EV_MEMORYDATADUMP,
			&CComApplication::MessageHandler_EV_MEMORYDATADUMP),			// 0x7C 
	CMessageHandler(EV_WRITEMEMORYDATADONE,
			&CComApplication::MessageHandler_EV_WRITEMEMORYDATADONE),	// 0x7D
	CMessageHandler(EV_NODEEEPROMREAD,0),					// 0x7E
	CMessageHandler(EV_NODEEEPROMWRITTEN,0),				// 0x7F
	CMessageHandler(FC_NODEMANAGEAVIRDEVICES,0),			// 0x80
	CMessageHandler(0x81,0),								// 0x81
	CMessageHandler(0x82,0),								// 0x82
	CMessageHandler(FC_UNITREQUESTDIMSTATUS,0),				// 0x83
	CMessageHandler(FC_UNITREQUESTCONFIG,0),				// 0x84
	CMessageHandler(0x85,0),								// 0x85
	CMessageHandler(FC_UNITREQUESTSWITCHSTATUS,0),			// 0x86
	CMessageHandler(FC_UNITREQUESTCONTROLSTATUS,0),			// 0x87
	CMessageHandler(FC_UNITSENSSET,0),						// 0x88
	CMessageHandler(FC_UNITREQUESTSENSSTATUS,0),			// 0x89
	CMessageHandler(FC_NODEREQUESTCONFIG,0),				// 0x8a
	CMessageHandler(FC_UNITSETNAME,0),						// 0x8b
	CMessageHandler(FC_NODESETNAME,0),						// 0x8c
	CMessageHandler(FC_UNITREQUESTNAME,0),					// 0x8d
	CMessageHandler(FC_NODEREQUESTNAME,0),					// 0x8e
	CMessageHandler(FC_NODEREQUESTSTATUS,0),				// 0x8f
	CMessageHandler(FC_BROWSEFORNODES,0),					// 0x90
	CMessageHandler(FC_BROWSEFORNEWNODES,0),				// 0x91
	CMessageHandler(0x92,0),								// 0x92
	CMessageHandler(FC_UNITCONFIG,0),						// 0x93
	CMessageHandler(FC_NODECONFIG,0),						// 0x94
	CMessageHandler(FC_NODEREQUESTDLLSTATUS,0),				// 0x95
	CMessageHandler(FC_NODESETMASTERLCDNODELA,0),			// 0x96
	CMessageHandler(FC_NODEMASTERLCDSHUTDOWN,0),			// 0x97
	CMessageHandler(FC_NODEBINDINGSFILECONTROL,0),			// 0x98
	CMessageHandler(FC_NODEBINDINGENTRY,0),					// 0x99
	CMessageHandler(FC_UNITRESET,0),						// 0x9a
	CMessageHandler(FC_NODERESET,0),						// 0x9b
	CMessageHandler(FC_NODELCDSHUTDOWN,0),					// 0x9c
	CMessageHandler(FC_NODEGETRESOURCES,0),					// 0x9d
	CMessageHandler(FC_SLAVELCDINITIATEBROWSING,0),			// 0x9e
	CMessageHandler(FC_UNITAUDIOSET,0),						// 0x9f
	CMessageHandler(FC_NODEERASEEEPROM,0),					// 0xa0
	CMessageHandler(FC_NODELCDSET,0),						// 0xa1 
	CMessageHandler(FC_UNITDIMSET,0),						// 0xa2
	CMessageHandler(FC_UNITSWITCHSET,0),					// 0xa3
	CMessageHandler(FC_UNITREQUESTLCDSTATUS,0),				// 0xa4
	CMessageHandler(FC_UNITREQUESTAUDIOSTATUS,0),			// 0xa5
	CMessageHandler(FC_UNITLCDVIRTUALSET,0),				// 0xa6
	CMessageHandler(FC_UNITREQUESTLCDVIRTUALSTATUS,0),	    // 0xa7
	CMessageHandler(FC_UNITCONTROLSET,0),					// 0xa8
	CMessageHandler(FC_NODELCDMASTERSET,0),					// 0xa9
	CMessageHandler(FC_NODELCDMASTERTIMEDATE,0),			// 0xaa
	CMessageHandler(FC_NODELCDMASTERREQUESTSTATUS,0),		// 0xab
	CMessageHandler(FC_UNITFEEDBACKCONFIG,0),				// 0xac
	CMessageHandler(FC_UNITIRTXSET,0),						// 0xad
	//CMessageHandler(FC_UNITENTERTAINMENTROOMSET,0),			// 0xae
	//CMessageHandler(FC_UNITREQUESTENTERTAINMENTROOMSTATUS,0), // 0xaf
	CMessageHandler(FC_FTPFILECONTROL,0),					// 0xae : TM20100317
	CMessageHandler(FC_REQUESTFTPFILECONTROL,0),				// 0xaf : TM20100317
	CMessageHandler(FC_NODEAVIRCONNECTIONMATRIXSET,0),		// 0xb0
	CMessageHandler(FC_NODEREQUESTAVIRMATRIXSTATUS,0),		// 0xb1
	CMessageHandler(FC_NODEAVIRDEVICESENDIRCODE,0),			// 0xb2
	CMessageHandler(FC_UNITRC5IRRECEIVERSET,0),				// 0xb3
	CMessageHandler(FC_UNITREQUESTRC5IRRECEIVERSTATUS,0),	// 0xb4
	CMessageHandler(FC_DATABASEMANAGEMENT,0),				// 0xb5
	CMessageHandler(FC_UNITDUOSWITCHSET,0),					// 0xb6
	CMessageHandler(FC_UNITREQUESTDUOSWITCHSTATUS,0),		// 0xb7
	CMessageHandler(FC_BROWSEFORPHYSICALNODES,0),			// 0xb8   
	CMessageHandler(FC_PNPASSIGNLOGICALADDRESS,0),		    // 0xb9  
	CMessageHandler(FC_LCDPMDBASEADDNODEENTRY,0),			// 0xBA 
	CMessageHandler(FC_LCDPMDBASEADDUNITENTRY,0),			// 0xBB 
	CMessageHandler(FC_LCDINFODIALOG,		
					&CComApplication::MessageHandler_FC_LCDINFODIALOG),	// 0xBC 
	CMessageHandler(FC_PNPPHYSADRACK,0),					// 0xBD 
	CMessageHandler(FC_GETIRCODE,0),						// 0xBE
	CMessageHandler(FC_UNITREQUESTIRTXSTATUS,0),			// 0xBF 
	CMessageHandler(FC_CHECKRC5CODE,0),						// 0xC0 
	CMessageHandler(FC_UNITVIDEOMUXSET,0),					// 0xC1   
	CMessageHandler(FC_UNITREQUESTVIDEOMUXSTATUS,0),		// 0xC2 
	CMessageHandler(FC_UNITPS2INTELLIMOUSESET,0),			// 0xC3
	CMessageHandler(FC_UNITREQUESTPS2INTELLIMOUSESTATUS,0), // 0xC4
	CMessageHandler(FC_UNITSPEECHCOMMSET,0),				// 0xC5
	CMessageHandler(FC_UNITREQUESTSPEECHCOMMSTATUS,0),		// 0xC6
	CMessageHandler(FC_NODEMASTERLCDTAPISET,0),				// 0xC7
	CMessageHandler(FC_LCDBUILDSCREENMAP,0),				// 0xC8 
	CMessageHandler(FC_EXTERNALCONFIG,0),					// 0xC9 
	CMessageHandler(FC_UNITAVMATRIXSET,0),					// 0xCA 
	CMessageHandler(FC_UNITREQUESTAVMATRIXSTATUS,0),		// 0xCB 
	CMessageHandler(FC_UNITALARMSET,0),						// 0xCC
	CMessageHandler(FC_UNITREQUESTALARMSTATUS,0),			// 0xCD
	CMessageHandler(0xCE,0),								// 0xCE
	CMessageHandler(0xCF,0),								// 0xCF
	CMessageHandler(0xD0,0),								// 0xD0
	CMessageHandler(0xD1,0),								// 0xD1
	CMessageHandler(0xD2,0),								// 0xD2
	CMessageHandler(0xD3,0),								// 0xD3
	CMessageHandler(0xD4,0),								// 0xD4
	CMessageHandler(0xD5,0),								// 0xD5
	CMessageHandler(0xD6,0),								// 0xD6
	CMessageHandler(0xD7,0),								// 0xD7
	CMessageHandler(0xD8,0),								// 0xD8
	CMessageHandler(0xD9,0),								// 0xD9
	CMessageHandler(0xDA,0),								// 0xDA
	CMessageHandler(0xDB,0),								// 0xDB
	CMessageHandler(0xDC,0),								// 0xDC
	CMessageHandler(0xDD,0),								// 0xDD
	CMessageHandler(0xDE,0),								// 0xDE
	CMessageHandler(0xDF,0),								// 0xDF
	CMessageHandler(0xE0,0),								// 0xE0
	CMessageHandler(0xE1,0),								// 0xE1
	CMessageHandler(0xE2,0),								// 0xE2
	CMessageHandler(0xE3,0),								// 0xE3
	CMessageHandler(0xE4,0),								// 0xE4
	CMessageHandler(0xE5,0),								// 0xE5
	CMessageHandler(0xE6,0),								// 0xE6
	CMessageHandler(0xE7,0),								// 0xE7
	CMessageHandler(0xE8,0),								// 0xE8
	CMessageHandler(0xE9,0),								// 0xE9
	CMessageHandler(0xEA,0),								// 0xEA
	CMessageHandler(0xEB,0),								// 0xEB
	CMessageHandler(0xEC,0),								// 0xEC
	CMessageHandler(0xED,0),								// 0xED
	CMessageHandler(0xEE,0),								// 0xEE
	CMessageHandler(0xEF,0),								// 0xEF
	CMessageHandler(0xF0,0),								// 0xF0
	CMessageHandler(0xF1,0),								// 0xF1
	CMessageHandler(0xF2,0),								// 0xF2
	CMessageHandler(0xF3,0),								// 0xF3
	CMessageHandler(0xF4,0),								// 0xF4
	CMessageHandler(0xF5,0),								// 0xF5
	CMessageHandler(0xF6,0),								// 0xF6
	CMessageHandler(0xF7,0),								// 0xF7		
	CMessageHandler(FC_NODEGATEWAYSET,0),					// 0xF8		
	CMessageHandler(FC_NODEGATEWAYREQUESTSTATUS,0),			// 0xF9	
	CMessageHandler(FC_UNLOCKLCD,0),						// 0xFA 
	CMessageHandler(FC_LOCKLCD,0),							// 0xFB 
	CMessageHandler(FC_GETMEMORYDATA,0),					// 0xFC
	CMessageHandler(FC_WRITEMEMORYDATA,0),					// 0xFD 
	CMessageHandler(FC_NODEEEPROMREAD,0),					// 0xfe
	CMessageHandler(FC_NODEEEPROMWRITE,0)					// 0xff
};

//////////////////////////////////////////////////////////////////////////////
// CComApplication

CComApplication::CComApplication(CWnd *pCtrl) : 
	m_pCommunicationObject(NULL), 
	m_pControl((CMainFrame*)pCtrl)
{		
	m_pCommunicationObject = new CCommunicationObject(*this);
	if (NULL == TMGetRuntimeNodes()) {	
		ASSERT(0);
		exit(-1);
	}

	// Sedert V16.26: zie USE_WORKAROUND_MESSAGEERROR_ABSORBE
	m_absorbeDisplayMsg.m_flagAbsorbe = false;
	m_absorbeDisplayMsg.m_nodeAddress = 0xff;
}

CComApplication::~CComApplication()
{
	if (NULL != m_pCommunicationObject) {
		delete m_pCommunicationObject;
		m_pCommunicationObject = NULL;
	}
}


BEGIN_MESSAGE_MAP(CComApplication, CWnd)
	//{{AFX_MSG_MAP(CComApplication)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		ON_WM_TIMER()
	//}}AFX_MSG_MAP

	ON_MESSAGE(WM_COMM_MESSAGE_RECEIVED,OnMessageReceived)
	ON_MESSAGE(WM_COMM_MESSAGE_SEND,OnMessageSend)	
	ON_MESSAGE(WM_SCREENMAP_FSM_TRIGGER_CANCEL,OnScreenmapFSMTriggerCancel)
	ON_MESSAGE(WM_SCREENMAP_FSM_TRIGGER_OK,OnScreenmapFSMTriggerOk)
	ON_MESSAGE(WM_SCREENMAP_FSM_TRIGGER_ERROR,OnScreenmapFSMTriggerError)
	ON_MESSAGE(WM_CLASSFACTORY_DESTROY,OnClassFactoryDestroy)
	ON_MESSAGE(WM_COMMUNICATION_MODEM_STATUS, OnCommunicationModemStatus)
	ON_MESSAGE(WM_COMMUNICATION_EVENT, OnComDataReceived )
		
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////

#if defined(USE_MSCOMMCONTROL)
	// add eventsink map to hook OnComm event
	BEGIN_EVENTSINK_MAP(CComApplication, CWnd /*CFrameWnd*/)
		ON_EVENT(CComApplication, ID_COMMCTRL, 1 , OnCommEvent, VTS_NONE)
	END_EVENTSINK_MAP()

#endif

/////////////////////////////////////////////////////////////////////////////
BYTE CComApplication::MessageTransfer(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bMsgCode) 
{
	ASSERT(0 != m_pCommunicationObject);
	return (m_pCommunicationObject->MessageTransfer(bNodeAddress,bUnitAddress,bMsgCode));
}
BYTE CComApplication::MessageTransfer(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bMsgCode,BYTE bLength,CString szData) 
{
	ASSERT(0 != m_pCommunicationObject);
	return (m_pCommunicationObject->MessageTransfer(bNodeAddress,bUnitAddress,bMsgCode,bLength,szData));
}	
BYTE CComApplication::RequestModemStatus()
{
	ASSERT(0 != m_pCommunicationObject);
	return (m_pCommunicationObject->SendSerialModemCommand(PININSTALL_MODEMSTATUS));
}
BYTE CComApplication::RequestModemAddress()
{
	ASSERT(0 != m_pCommunicationObject);
	return (m_pCommunicationObject->SendSerialModemCommand(PININSTALL_ADDRESS));
}
BYTE CComApplication::RequestModemVersion()
{
	ASSERT(0 != m_pCommunicationObject);
	return (m_pCommunicationObject->SendSerialModemCommand(PININSTALL_SOFTWARE_VERSION));
}

BOOL CComApplication::SendModemCanLoggerOnOff(BOOL fState) {
	ASSERT(0 != m_pCommunicationObject);
	return (m_pCommunicationObject->SendSerialModemCANLoggerOnOff(fState));
}

BOOL CComApplication::sendServerTCPIPGetNodeInfo() {
	ASSERT(0 != m_pCommunicationObject);
	return (true == CCommunicationObject_Utils::sendRequestServerNodeInfo(m_pCommunicationObject));
}

BOOL CComApplication::sendServerTCPIPSetMaster() {
	ASSERT(0 != m_pCommunicationObject);
	return (true == CCommunicationObject_Utils::sendCfgServerAsMaster(m_pCommunicationObject));
}

// Sedert V16.26: zie USE_WORKAROUND_MESSAGEERROR_ABSORBE
void CComApplication::setAbsorbeMessageError(int nodeAddress) {
	m_absorbeDisplayMsg.m_flagAbsorbe = true;
	m_absorbeDisplayMsg.m_nodeAddress = nodeAddress;
}

//////////////////////////////////////////////////////////////////////////////
void CComApplication::ShutDownAllTasks()
{
	if (m_pFactory != NULL)
	{
		if (m_pFactory->Get_RenameNodeUnitsEngine() != NULL)
		{
			StopTimer( TIMERID_RENAME_TIMEOUT );
			m_pFactory->Destroy_RenameNodeUnitsEngine();
		}
		if (m_pFactory->Get_RequestForNodeStatussen() != NULL)
		{
			StopTimer( TIMERID_REQUESTSTATUSSEN_FSM );
			m_pFactory->Destroy_RequestForNodeStatussen();
		}
		if (m_pFactory->Get_MultiMediaConfigEngine() != NULL)
		{		
			StopTimer( TIMERID_MMCONFIG_ACKTIMEOUT );
			m_pFactory->Destroy_MultiMediaConfigEngine();
		}
		if (m_pFactory->Get_BindingsReadWriteEngine() != NULL)
		{
			StopTimer( TIMERID_BINDINGS_TIMEOUT );
			m_pFactory->Destroy_BindingsReadWriteEngine();
		}
		if (m_pFactory->Get_ScreenmapsUpdateEngine() != NULL)
		{
			StopTimer( TIMERID_SCREENMAP_ACKTIMEOUT );
			m_pFactory->Destroy_ScreenmapsUpdateEngine();
		}
		if (m_pFactory->Get_MemoryDumpEngine() != NULL)
		{
			StopTimer( TIMERID_MEMORYDUMP_ACKTIMEOUT );					
			m_pFactory->Destroy_MemoryDumpEngine();
		}
		if (m_pFactory->Get_BrowsePnpEngine() != NULL)
		{			
			StopTimer(TIMERID_BROWSE_TIMER);
			StopTimer(TIMERID_PNP_BROWSE_NORESPONSE);
			StopTimer(TIMERID_PNP_POLL);

			m_pFactory->Destroy_BrowsePnpEngine();
		}
		if (m_pFactory->Get_ModemConnectEngine() != NULL)
		{
			StopTimer( TIMERID_MODEMCONNECT_TIMEOUT );
			m_pFactory->Destroy_ModemConnectEngine();
		}
		if (m_pFactory->Get_UnitCreateFSMEngine() != NULL)
		{
			StopTimer( TIMERID_EXTERNALCONFIG_ACKTIMEOUT );
			m_pFactory->Destroy_UnitCreateFSMEngine();
		}
		if (m_pFactory->Get_UploadMoodFSMEngine() != NULL)
		{
			StopTimer( TIMERID_EXTERNALCONFIG_ACKTIMEOUT );
			m_pFactory->Destroy_UploadMoodFSMEngine( );
		}
		if (m_pFactory->Get_UploadIRRXFSMEngine() != NULL)
		{
			StopTimer( TIMERID_EXTERNALCONFIG_ACKTIMEOUT );
			m_pFactory->Destroy_UploadIRRXFSMEngine( );
		}
		if (m_pFactory->Get_UploadScheduleFSMEngine() != NULL)
		{
			StopTimer( TIMERID_EXTERNALCONFIG_ACKTIMEOUT );
			m_pFactory->Destroy_UploadScheduleFSMEngine( );
		}
		if (m_pFactory->Get_ConfigSmapPageInfoFSMEngine() != NULL)
		{
			StopTimer( TIMERID_EXTERNALCONFIG_ACKTIMEOUT );
			m_pFactory->Destroy_ConfigSmapPageInfoFSMEngine();
		}
		if (m_pFactory->Get_ConfigSmapMenuFSMEngine() != NULL)
		{
			StopTimer( TIMERID_EXTERNALCONFIG_ACKTIMEOUT );
			m_pFactory->Destroy_ConfigSmapMenuFSMEngine();
		}
		if (m_pFactory->Get_ConfigSmapAVRoomFSMEngine() != NULL)
		{
			StopTimer( TIMERID_EXTERNALCONFIG_ACKTIMEOUT );
			m_pFactory->Destroy_ConfigSmapAVRoomFSMEngine();
		}
		if (m_pFactory->Get_ConfigCanFTP() != 0)
		{
			// StopTimer( ... )
			m_pFactory->Destroy_ConfigCanFTP( );
		}
		if (m_pFactory->Get_ConfigGatewayUFSMEngine() != NULL)
		{
			StopTimer( TIMERID_EXTERNALCONFIG_ACKTIMEOUT );
			m_pFactory->Destroy_ConfigGatewayUFSMEngine();
		}
		if (m_pFactory->Get_ConfigRoomCtrlAVFSMEngine() != NULL)
		{
			StopTimer( TIMERID_EXTERNALCONFIG_ACKTIMEOUT );
			m_pFactory->Destroy_ConfigRoomCtrlAVFSMEngine();
		}
	}
}

LONG CComApplication::OnClassFactoryDestroy(WPARAM id, LPARAM)
{
	const ENUM_CLASSFACTORY_ID ClassFactoryID = (ENUM_CLASSFACTORY_ID) id;

	ASSERT(m_pFactory);

	switch (ClassFactoryID)
	{
		case CLASSFACTORY_ID_RENAME_ENGINE:
		{
			if (m_pFactory != NULL)
			{
				ASSERT(m_pFactory->Get_RenameNodeUnitsEngine());
				m_pFactory->Destroy_RenameNodeUnitsEngine();
			}
			
		} return TRUE;

		case CLASSFACTORY_ID_REQUEST_FOR_STATUSSEN:
		{
			if (m_pFactory != NULL)
			{
				ASSERT(m_pFactory->Get_RequestForNodeStatussen());
				m_pFactory->Destroy_RequestForNodeStatussen();
			}

		} return TRUE;

		case CLASSFACTORY_ID_MULTIMEDIA_CONFIG:
		{
			if (m_pFactory != NULL)
			{
				ASSERT(m_pFactory->Get_MultiMediaConfigEngine());
				m_pFactory->Destroy_MultiMediaConfigEngine();
			}		

		} return TRUE;

		case CLASSFACTORY_ID_BINDINGS_READWRITE_ENGINE:
		{
			if (m_pFactory != NULL)
			{
				ASSERT(m_pFactory->Get_BindingsReadWriteEngine());
				m_pFactory->Destroy_BindingsReadWriteEngine();
			}	

		} return TRUE;

		case CLASSFACTORY_ID_SCREENMAP_UPDATE_ENGINE:
		{
			if (m_pFactory != NULL)
			{
				ASSERT(m_pFactory->Get_ScreenmapsUpdateEngine());
				m_pFactory->Destroy_ScreenmapsUpdateEngine();
			}	
			
		} return TRUE;

		case CLASSFACTORY_ID_MEMORYDUMP_ENGINE:
		{
			if (m_pFactory != NULL)
			{
				ASSERT(m_pFactory->Get_MemoryDumpEngine());
				m_pFactory->Destroy_MemoryDumpEngine();
			}	

		} return TRUE;

		case CLASSFACTORY_ID_BROWSEPNP_ENGINE:
		{
			if (m_pFactory != NULL)
			{
				ASSERT(m_pFactory->Get_BrowsePnpEngine());
				m_pFactory->Destroy_BrowsePnpEngine();
			}	

		} return TRUE;

		case CLASSFACTORY_ID_MODEMCONNECT_ENGINE:
		{
			if (m_pFactory != NULL)
			{
				ASSERT(m_pFactory->Get_ModemConnectEngine());
				m_pFactory->Destroy_ModemConnectEngine();
			}
		} return TRUE;

		case CLASSFACTORY_ID_NICERRORSFSM_ENGINE:
		{
			if (m_pFactory != 0)
			{
				ASSERT(m_pFactory->Get_NicErrorsFSMEngine());
				m_pFactory->Destroy_NicErrorsFSMEngine();
			}
		} return TRUE;

		case CLASSFACTORY_ID_UNITCREATE_CONFIG:
		{
			if (m_pFactory != 0)
			{
				ASSERT(m_pFactory->Get_UnitCreateFSMEngine());
				m_pFactory->Destroy_UnitCreateFSMEngine();
			}
		} return TRUE;

		case CLASSFACTORY_ID_UPLOAD_MOODS:
		{
			if (m_pFactory != 0)
			{
				ASSERT(m_pFactory->Get_UploadMoodFSMEngine());
				m_pFactory->Destroy_UploadMoodFSMEngine();
			}
		} return TRUE;

		case CLASSFACTORY_ID_UPLOAD_IRRX:
		{
			if (m_pFactory != 0)
			{
				ASSERT(m_pFactory->Get_UploadIRRXFSMEngine());
				m_pFactory->Destroy_UploadIRRXFSMEngine();
			}
		} return TRUE;

		case CLASSFACTORY_ID_UPLOAD_SCHEDULE:
		{
			if (m_pFactory != 0)
			{
				ASSERT(m_pFactory->Get_UploadScheduleFSMEngine());
				m_pFactory->Destroy_UploadScheduleFSMEngine();
			}
		} return TRUE;

		case CLASSFACTORY_ID_FTP:
		{
			if (m_pFactory != 0)
			{
				ASSERT(m_pFactory->Get_ConfigCanFTP());
				m_pFactory->Destroy_ConfigCanFTP();
			}
		} return TRUE;

		case CLASSFACTORY_ID_UPLOAD_SMAP_PAGEINFO:
		{
			if (m_pFactory != 0)
			{
				ASSERT(m_pFactory->Get_ConfigSmapPageInfoFSMEngine());
				m_pFactory->Destroy_ConfigSmapPageInfoFSMEngine();
			}		
		} return TRUE;

		case CLASSFACTORY_ID_UPLOAD_SMAP_MENU:
		{
			if (m_pFactory != 0)
			{
				ASSERT(m_pFactory->Get_ConfigSmapMenuFSMEngine());
				m_pFactory->Destroy_ConfigSmapMenuFSMEngine();
			}		
		} return TRUE;

		case CLASSFACTORY_ID_UPLOAD_SMAP_AVROOM:
		{
			if (m_pFactory != 0)
			{
				ASSERT(m_pFactory->Get_ConfigSmapAVRoomFSMEngine());
				m_pFactory->Destroy_ConfigSmapAVRoomFSMEngine();
			}		
		} return TRUE;

		case CLASSFACTORY_ID_UPLOAD_GATEWAY_CFG:
		{
			if (m_pFactory)
			{
				ASSERT(m_pFactory->Get_ConfigGatewayUFSMEngine());
				m_pFactory->Destroy_ConfigGatewayUFSMEngine();
			}
		} return TRUE;

		case CLASSFACTORY_ID_UPLOAD_ROOMCTRL_AUDIOCFG:
		{
			if (m_pFactory)
			{
				ASSERT(m_pFactory->Get_ConfigRoomCtrlAVFSMEngine());
				m_pFactory->Destroy_ConfigRoomCtrlAVFSMEngine();
			}
		} return TRUE;

		case CLASSFACTORY_ID_UPLOAD_BOSEST_CFG:
		{
			if (m_pFactory)
			{
				ASSERT(m_pFactory->Get_ConfigBoseSTFSMEngine());
				m_pFactory->Destroy_ConfigBoseSTFSMEngine();
			}
		} return TRUE;

		case CLASSFACTORY_ID_UPLOAD_LEDSCFG:
		{
			if (m_pFactory)
			{
				ASSERT(m_pFactory->Get_ConfigLedsCfgFSMEngine());
				m_pFactory->Destroy_ConfigLedsCfgFSMEngine();
			}
		} return TRUE;

		case CLASSFACTORY_ID_UPLOAD_SMAPLITE:
		{
			if (m_pFactory)
			{
				ASSERT(m_pFactory->Get_ConfigSmapLiteFSMEngine());
				m_pFactory->Destroy_ConfigSmapLiteFSMEngine();
			}
		} return TRUE;
				
		default:
		{
			ASSERT(0);
		} break;
	}
	return TRUE;
}

// Sedert V16.35 homeclient socket opvragen dmv de node.
// Mbv de node wordt bepaald welke opties er ondersteund worden...
// Smartbox ultra: Secured login + Dali messages + Mag ook als slave (sedert V16.38)
ITcpSocketHomeClient* CComApplication::getSocketHomeClient(CCanNode& refNode) {
	const int nodeAddress = refNode.getNodeAddress();	

	// Indien reeds bestaand niet aanmaken maar deze gebruiken.
	ITcpSocketHomeClient* pSocketHomeClient = m_pCommunicationObject->getSocketHomeClient(nodeAddress);
	if (NULL != pSocketHomeClient) {	
		return pSocketHomeClient;
	}

	// Bepalen van de opties ifv de node properties.
	BOOL optionSubscribeDALIMessages = FALSE;
	BOOL optionSecureLogin = FALSE;
	BOOL optionAllowConnectSlave = FALSE;
	if (0x66 == refNode.GetSoftwareVersion()) {
		if (PRODUCT_ID_HC_BOX_ULTRA == refNode.GetProductID()) {
			optionSubscribeDALIMessages = TRUE;
			optionSecureLogin = TRUE;
			optionAllowConnectSlave = TRUE;	
		}
	}

	CTCPSocketOptions tcpSocketOptions;
	tcpSocketOptions.setOptionLoginSubscribeCanMsg(FALSE);
	tcpSocketOptions.setOptionLoginSecure(optionSecureLogin);
	tcpSocketOptions.setOptionLoginSubscribeDALIMsg(optionSubscribeDALIMessages);
	tcpSocketOptions.setOptionLoginAllowSlaveConnect(optionAllowConnectSlave);

	// Aanmaken van de socket.
	return m_pCommunicationObject->createSocketHomeClient(nodeAddress,tcpSocketOptions);
}


// Home Client Master functies.
// - V65: TCP/IP Homeserver als master.
// - V66: LCD & TCP/IP als master.
bool CComApplication::isSocketHomeClientMasterSupported(CCanNode* pNode) {
	bool result = false;	
	if (pNode->bNodeFlags & NODE_MASTER_LCD) {
		const BYTE softwareVersion = pNode->GetSoftwareVersion();
		switch(softwareVersion) {
			case 0x65: {
				const BYTE bNodeType = pNode->bNodeType;
				if (bNodeType == NODE_PIN_GATEWAY) {
					result = true;
				}
			} break;
			case 0x66: {
				const BYTE bNodeType = pNode->bNodeType;
				if (bNodeType == NODE_PIN_GATEWAY || bNodeType == NODE_LCD) {
					result = true;
				}
			} break;		
		}
	}
	return result;
}

void CComApplication::closeConnections( ) {
	if (NULL != this->m_pCommunicationObject) {
		this->m_pCommunicationObject->closeConnections();
		this->GetCommunicationStatus().setConnected(FALSE);	// Deze moet hier komen !!!
	}
}

void CComApplication::NotifyOnTaskDone(ENUM_COMAPPLICATION_TASK id)
{
	ENUM_CLASSFACTORY_ID ClassFactoryId = CLASSFACTORY_ID_UNDEFINED;

	switch (id)
	{
		case COMAPPLICATION_TASK_ID_RENAME_ENGINE:
		{
			m_pControl->PostMessage(WM_GENERICTASK_FSM_DONE);

			ASSERT(m_pFactory->Get_RenameNodeUnitsEngine());

			if (m_pFactory->Get_RenameNodeUnitsEngine() != 0)
			{
				ClassFactoryId = CLASSFACTORY_ID_RENAME_ENGINE;
			}
		} break;

		case COMAPPLICATION_TASK_ID_MULTIMEDIA_CONFIG:
		{
			m_pControl->SendMessage(WM_GENERICTASK_FSM_DONE);

			ASSERT(m_pFactory->Get_MultiMediaConfigEngine());

			if (m_pFactory->Get_MultiMediaConfigEngine() != 0)
			{
				ClassFactoryId = CLASSFACTORY_ID_MULTIMEDIA_CONFIG;	
			}
		} break;

		case COMAPPLICATION_TASK_ID_UNITCREATE_CONFIG:
		{
			m_pControl->SendMessage(WM_GENERICTASK_FSM_DONE);
			
			ASSERT(m_pFactory->Get_UnitCreateFSMEngine());

			if (m_pFactory->Get_UnitCreateFSMEngine() != 0)
			{
				ClassFactoryId = CLASSFACTORY_ID_UNITCREATE_CONFIG;
			}
		} break;

		case COMAPPLICATION_TASK_ID_UPLOAD_MOODS:
		{
			m_pControl->SendMessage(WM_GENERICTASK_FSM_DONE);
			
			ASSERT(m_pFactory->Get_UploadMoodFSMEngine());

			if (m_pFactory->Get_UploadMoodFSMEngine() != 0)
			{
				ClassFactoryId = CLASSFACTORY_ID_UPLOAD_MOODS;
			}	
		} break;

		case COMAPPLICATION_TASK_ID_UPLOAD_IRRX:
		{
			m_pControl->SendMessage(WM_GENERICTASK_FSM_DONE);
			
			ASSERT(m_pFactory->Get_UploadIRRXFSMEngine());

			if (m_pFactory->Get_UploadIRRXFSMEngine() != 0)
			{
				ClassFactoryId = CLASSFACTORY_ID_UPLOAD_IRRX;
			}	
		} break;

		case COMAPPLICATION_TASK_ID_UPLOAD_SCHEDULE:
		{
			m_pControl->SendMessage(WM_GENERICTASK_FSM_DONE);
			
			ASSERT(m_pFactory->Get_UploadScheduleFSMEngine());

			if (m_pFactory->Get_UploadScheduleFSMEngine() != 0)
			{
				ClassFactoryId = CLASSFACTORY_ID_UPLOAD_SCHEDULE;
			}	
		} break;

		case COMAPPLICATION_TASK_ID_REQUEST_FOR_STATUSSEN:
		{
			m_pControl->SendMessage(WM_GENERICTASK_FSM_DONE,0,0);
			
			ASSERT(m_pFactory->Get_RequestForNodeStatussen());

			if (m_pFactory->Get_RequestForNodeStatussen() != 0)
			{
				ClassFactoryId = CLASSFACTORY_ID_REQUEST_FOR_STATUSSEN;
			}
		} break;

		case COMAPPLICATION_TASK_ID_BINDINGS_READWRITE_ENGINE:
		{
			ASSERT(0);

		} break;

		case COMAPPLICATION_TASK_ID_SCREENMAP_UPDATE_ENGINE:
		{
			m_pControl->PostMessage(WM_GENERICTASK_FSM_DONE);

			ASSERT(m_pFactory->Get_ScreenmapsUpdateEngine());
			
			if (m_pFactory->Get_ScreenmapsUpdateEngine() != 0)
			{
				ClassFactoryId = CLASSFACTORY_ID_SCREENMAP_UPDATE_ENGINE;
			}
	
		} break;

		case COMAPPLICATION_TASK_ID_MEMORYDUMP_ENGINE:
		{
			m_pControl->SendMessage(WM_GENERICTASK_FSM_DONE);

			ASSERT(m_pFactory->Get_MemoryDumpEngine());

			if (m_pFactory->Get_MemoryDumpEngine() != 0)
			{
				ClassFactoryId = CLASSFACTORY_ID_MEMORYDUMP_ENGINE;
			}

		} break;
	
		case COMAPPLICATION_TASK_ID_BROWSEPNP_ENGINE:
		{
			ASSERT(0);

		} break;

		case COMAPPLICATION_TASK_ID_MODEMCONNECT_ENGINE:
		{
			ASSERT(m_pFactory->Get_ModemConnectEngine());

			if (m_pFactory->Get_ModemConnectEngine() != NULL)
			{
				ClassFactoryId = CLASSFACTORY_ID_MODEMCONNECT_ENGINE;
			}

			PostMessage( WM_COMMUNICATION_MODEM_STATUS );	

		} break;

		case COMAPPLICATION_TASK_ID_NICERRORSFSM_ENGINE:
		{
			m_pControl->PostMessage(WM_GENERICTASK_FSM_DONE);

			ASSERT(m_pFactory->Get_NicErrorsFSMEngine());
		
			if (m_pFactory->Get_NicErrorsFSMEngine() != 0)
			{
				ClassFactoryId = CLASSFACTORY_ID_NICERRORSFSM_ENGINE;		
			}

		} break;

		case COMAPPLICATION_TASK_ID_FTP:
		{
			m_pControl->SendMessage(WM_GENERICTASK_FSM_DONE);
			
			ASSERT(m_pFactory->Get_ConfigCanFTP());

			if (m_pFactory->Get_ConfigCanFTP() != 0)
			{
				ClassFactoryId = CLASSFACTORY_ID_FTP;
			}		
			
		} break;		

		case COMAPPLICATION_TASK_ID_UPLOAD_SMAP_PAGEINFO:
		{
			m_pControl->SendMessage(WM_GENERICTASK_FSM_DONE);
			
			ASSERT(m_pFactory->Get_ConfigSmapPageInfoFSMEngine());

			if (m_pFactory->Get_ConfigSmapPageInfoFSMEngine() != 0)
			{
				ClassFactoryId = CLASSFACTORY_ID_UPLOAD_SMAP_PAGEINFO;
			}		
			
		} break;

		case COMAPPLICATION_TASK_ID_UPLOAD_SMAP_MENU:
		{
			m_pControl->SendMessage(WM_GENERICTASK_FSM_DONE);
			
			ASSERT(m_pFactory->Get_ConfigSmapMenuFSMEngine());

			if (m_pFactory->Get_ConfigSmapMenuFSMEngine() != 0)
			{
				ClassFactoryId = CLASSFACTORY_ID_UPLOAD_SMAP_MENU;
			}		
			
		} break;

		case COMAPPLICATION_TASK_ID_UPLOAD_SMAP_AVROOM:
		{
			m_pControl->SendMessage(WM_GENERICTASK_FSM_DONE);
			
			ASSERT(m_pFactory->Get_ConfigSmapAVRoomFSMEngine());

			if (m_pFactory->Get_ConfigSmapAVRoomFSMEngine() != 0)
			{
				ClassFactoryId = CLASSFACTORY_ID_UPLOAD_SMAP_AVROOM;
			}		

		} break;

		case COMAPPLICATION_TASK_ID_UPLOAD_GATEWAYCFG:
		{
			m_pControl->SendMessage(WM_GENERICTASK_FSM_DONE);
			
			ASSERT(m_pFactory->Get_ConfigGatewayUFSMEngine());

			if (m_pFactory->Get_ConfigGatewayUFSMEngine() != 0)
			{
				ClassFactoryId = CLASSFACTORY_ID_UPLOAD_GATEWAY_CFG;
			}		

		} break;

		case COMAPPLICATION_TASK_ID_UPLOAD_ROOMCTRL_AUDIOCFG:
		{
			m_pControl->SendMessage(WM_GENERICTASK_FSM_DONE);
			
			ASSERT(m_pFactory->Get_ConfigRoomCtrlAVFSMEngine());

			if (m_pFactory->Get_ConfigRoomCtrlAVFSMEngine() != 0)
			{
				ClassFactoryId = CLASSFACTORY_ID_UPLOAD_ROOMCTRL_AUDIOCFG;
			}		
		} break;

		case COMAPPLICATION_TASK_ID_UPLOAD_BOSEST_CFG:
		{
			m_pControl->SendMessage(WM_GENERICTASK_FSM_DONE);
			
			ASSERT(m_pFactory->Get_ConfigBoseSTFSMEngine());

			if (m_pFactory->Get_ConfigBoseSTFSMEngine() != 0)
			{
				ClassFactoryId = CLASSFACTORY_ID_UPLOAD_BOSEST_CFG;
			}		
		} break;

		case COMAPPLICATION_TASK_ID_UPLOAD_LEDSCFG:
		{
			m_pControl->SendMessage(WM_GENERICTASK_FSM_DONE);
			
			ASSERT(m_pFactory->Get_ConfigLedsCfgFSMEngine());

			if (m_pFactory->Get_ConfigLedsCfgFSMEngine() != 0)
			{
				ClassFactoryId = CLASSFACTORY_ID_UPLOAD_LEDSCFG;
			}		
		} break;

		case COMAPPLICATION_TASK_ID_UPLOAD_SMAPLITECFG:
		{
			m_pControl->SendMessage(WM_GENERICTASK_FSM_DONE);

			ASSERT(m_pFactory->Get_ConfigSmapLiteFSMEngine());

			if (m_pFactory->Get_ConfigSmapLiteFSMEngine() != 0)
			{
				ClassFactoryId = CLASSFACTORY_ID_UPLOAD_SMAPLITE;
			}	
		} break;

		default:
		{
			ASSERT(0);

		} break;
	}

	if (ClassFactoryId != CLASSFACTORY_ID_UNDEFINED)
	{
		PostMessage( WM_CLASSFACTORY_DESTROY,
					 ClassFactoryId );
	}
}
void CComApplication::NotifyOnTaskError(ENUM_COMAPPLICATION_TASK id)
{
	ENUM_CLASSFACTORY_ID ClassFactoryId = CLASSFACTORY_ID_UNDEFINED;

	switch (id)
	{
		case COMAPPLICATION_TASK_ID_RENAME_ENGINE:
		{
			ASSERT(0);
		} break;

		case COMAPPLICATION_TASK_ID_MULTIMEDIA_CONFIG:
		{
			m_pControl->SendMessage(WM_GENERICTASK_FSM_ERROR);

			ASSERT(m_pFactory->Get_MultiMediaConfigEngine());
		
			if (m_pFactory->Get_MultiMediaConfigEngine() != 0)
			{
				ClassFactoryId = CLASSFACTORY_ID_MULTIMEDIA_CONFIG;
			}						
		} break;

		case COMAPPLICATION_TASK_ID_UNITCREATE_CONFIG:
		{
			m_pControl->SendMessage(WM_GENERICTASK_FSM_ERROR);

			ASSERT(m_pFactory->Get_UnitCreateFSMEngine());
		
			if (m_pFactory->Get_UnitCreateFSMEngine() != 0)
			{
				ClassFactoryId = CLASSFACTORY_ID_UNITCREATE_CONFIG;
			}						
		} break;

		case COMAPPLICATION_TASK_ID_UPLOAD_MOODS:
		{
			m_pControl->SendMessage(WM_GENERICTASK_FSM_ERROR);

			ASSERT(m_pFactory->Get_UploadMoodFSMEngine());
		
			if (m_pFactory->Get_UploadMoodFSMEngine() != 0)
			{
				ClassFactoryId = CLASSFACTORY_ID_UPLOAD_MOODS;
			}	
		} break;

		case COMAPPLICATION_TASK_ID_UPLOAD_IRRX:
		{
			m_pControl->SendMessage(WM_GENERICTASK_FSM_ERROR);

			ASSERT(m_pFactory->Get_UploadIRRXFSMEngine());
		
			if (m_pFactory->Get_UploadIRRXFSMEngine() != 0)
			{
				ClassFactoryId = CLASSFACTORY_ID_UPLOAD_IRRX;
			}	
		} break;

		case COMAPPLICATION_TASK_ID_UPLOAD_SCHEDULE:
		{
			m_pControl->SendMessage(WM_GENERICTASK_FSM_ERROR);

			ASSERT(m_pFactory->Get_UploadScheduleFSMEngine());
		
			if (m_pFactory->Get_UploadScheduleFSMEngine() != 0)
			{
				ClassFactoryId = CLASSFACTORY_ID_UPLOAD_SCHEDULE;
			}	
		} break;

		case COMAPPLICATION_TASK_ID_REQUEST_FOR_STATUSSEN:
		{
			ASSERT(0);

		} break;

		case COMAPPLICATION_TASK_ID_BINDINGS_READWRITE_ENGINE:
		{
			ASSERT(0);

		} break;

		case COMAPPLICATION_TASK_ID_SCREENMAP_UPDATE_ENGINE:
		{		
			m_pControl->PostMessage(WM_GENERICTASK_FSM_ERROR);

			ASSERT(m_pFactory->Get_ScreenmapsUpdateEngine());
	
			if (m_pFactory->Get_ScreenmapsUpdateEngine() != 0)
			{
				ClassFactoryId = CLASSFACTORY_ID_SCREENMAP_UPDATE_ENGINE;
			}
		} break;

		case COMAPPLICATION_TASK_ID_MEMORYDUMP_ENGINE:
		{		
			m_pControl->SendMessage(WM_GENERICTASK_FSM_ERROR);

			ASSERT(m_pFactory->Get_MemoryDumpEngine());

			if (m_pFactory->Get_MemoryDumpEngine() != 0)
			{
				ClassFactoryId = CLASSFACTORY_ID_MEMORYDUMP_ENGINE;
			}
		} break;
	
		case COMAPPLICATION_TASK_ID_BROWSEPNP_ENGINE:
		{
			ASSERT(0);

		} break;

		case COMAPPLICATION_TASK_ID_MODEMCONNECT_ENGINE:
		{		
			ASSERT(m_pFactory->Get_ModemConnectEngine());

			if (m_pFactory->Get_ModemConnectEngine() != 0)
			{
				ClassFactoryId = CLASSFACTORY_ID_MODEMCONNECT_ENGINE;
			}
			PostMessage( WM_COMMUNICATION_MODEM_STATUS );

		} break;

		case COMAPPLICATION_TASK_ID_NICERRORSFSM_ENGINE:
		{
			m_pControl->PostMessage(WM_GENERICTASK_FSM_ERROR);

			ASSERT(m_pFactory->Get_NicErrorsFSMEngine());
				
			if (m_pFactory->Get_NicErrorsFSMEngine() != 0)
			{
				ClassFactoryId = CLASSFACTORY_ID_NICERRORSFSM_ENGINE;	
			}
		} break;	

		case COMAPPLICATION_TASK_ID_FTP:
		{
			m_pControl->PostMessage(WM_GENERICTASK_FSM_ERROR);

			ASSERT(m_pFactory->Get_ConfigCanFTP());
				
			if (m_pFactory->Get_ConfigCanFTP() != 0)
			{
				ClassFactoryId = CLASSFACTORY_ID_FTP;	
			}
		} break;

		case COMAPPLICATION_TASK_ID_UPLOAD_SMAP_PAGEINFO:
		{
			m_pControl->PostMessage(WM_GENERICTASK_FSM_ERROR);
			
			ASSERT(m_pFactory->Get_ConfigSmapPageInfoFSMEngine());

			if (m_pFactory->Get_ConfigSmapPageInfoFSMEngine() != 0)
			{
				ClassFactoryId = CLASSFACTORY_ID_UPLOAD_SMAP_PAGEINFO;
			}		
			
		} break;

		case COMAPPLICATION_TASK_ID_UPLOAD_SMAP_MENU:
		{
			m_pControl->SendMessage(WM_GENERICTASK_FSM_ERROR);
			
			ASSERT(m_pFactory->Get_ConfigSmapMenuFSMEngine());

			if (m_pFactory->Get_ConfigSmapMenuFSMEngine() != 0)
			{
				ClassFactoryId = CLASSFACTORY_ID_UPLOAD_SMAP_MENU;
			}		
			
		} break;

		case COMAPPLICATION_TASK_ID_UPLOAD_SMAP_AVROOM:
		{
			m_pControl->SendMessage(WM_GENERICTASK_FSM_ERROR);
			
			ASSERT(m_pFactory->Get_ConfigSmapAVRoomFSMEngine());

			if (m_pFactory->Get_ConfigSmapAVRoomFSMEngine() != 0)
			{
				ClassFactoryId = CLASSFACTORY_ID_UPLOAD_SMAP_AVROOM;
			}		

		} break;

		case COMAPPLICATION_TASK_ID_UPLOAD_GATEWAYCFG:
		{
			m_pControl->SendMessage(WM_GENERICTASK_FSM_ERROR);
			
			ASSERT(m_pFactory->Get_ConfigGatewayUFSMEngine());

			if (m_pFactory->Get_ConfigGatewayUFSMEngine() != 0)
			{
				ClassFactoryId = CLASSFACTORY_ID_UPLOAD_GATEWAY_CFG;
			}		

		} break;

		case COMAPPLICATION_TASK_ID_UPLOAD_ROOMCTRL_AUDIOCFG:
		{
			m_pControl->SendMessage(WM_GENERICTASK_FSM_ERROR);
			
			ASSERT(m_pFactory->Get_ConfigRoomCtrlAVFSMEngine());

			if (m_pFactory->Get_ConfigRoomCtrlAVFSMEngine() != 0)
			{
				ClassFactoryId = CLASSFACTORY_ID_UPLOAD_ROOMCTRL_AUDIOCFG;
			}					
		} break;

		case COMAPPLICATION_TASK_ID_UPLOAD_BOSEST_CFG:
		{
			m_pControl->SendMessage(WM_GENERICTASK_FSM_ERROR);
			
			ASSERT(m_pFactory->Get_ConfigBoseSTFSMEngine());

			if (m_pFactory->Get_ConfigBoseSTFSMEngine() != 0)
			{
				ClassFactoryId = CLASSFACTORY_ID_UPLOAD_BOSEST_CFG;
			}					

		} break;

		case COMAPPLICATION_TASK_ID_UPLOAD_LEDSCFG:
		{
			m_pControl->SendMessage(WM_GENERICTASK_FSM_ERROR);
			
			ASSERT(m_pFactory->Get_ConfigLedsCfgFSMEngine());

			if (m_pFactory->Get_ConfigLedsCfgFSMEngine() != 0)
			{
				ClassFactoryId = CLASSFACTORY_ID_UPLOAD_LEDSCFG;
			}					
		} break;

		case COMAPPLICATION_TASK_ID_UPLOAD_SMAPLITECFG:
		{
			m_pControl->SendMessage(WM_GENERICTASK_FSM_ERROR);

			ASSERT(m_pFactory->Get_ConfigSmapLiteFSMEngine());

			if (m_pFactory->Get_ConfigSmapLiteFSMEngine() != 0)
			{
				ClassFactoryId = CLASSFACTORY_ID_UPLOAD_SMAPLITE;
			}	

		} break;

		default:
		{
			ASSERT(0);

		} break;
	}

	if (ClassFactoryId != CLASSFACTORY_ID_UNDEFINED)
	{
		PostMessage( WM_CLASSFACTORY_DESTROY,
					 ClassFactoryId );
	}
}
void CComApplication::NotifyOnTaskCancelled(ENUM_COMAPPLICATION_TASK id)
{
	ENUM_CLASSFACTORY_ID ClassFactoryId = CLASSFACTORY_ID_UNDEFINED;

	switch (id)
	{
		case COMAPPLICATION_TASK_ID_RENAME_ENGINE:
		{
			ASSERT(0);

		} break;

		case COMAPPLICATION_TASK_ID_MULTIMEDIA_CONFIG:
		{
			m_pControl->SendMessage(WM_GENERICTASK_FSM_CANCELLED);

			ASSERT(m_pFactory->Get_MultiMediaConfigEngine());

			if (m_pFactory->Get_MultiMediaConfigEngine() != 0)
			{
				ClassFactoryId = CLASSFACTORY_ID_MULTIMEDIA_CONFIG;
			}
						
		} break;

		case COMAPPLICATION_TASK_ID_UNITCREATE_CONFIG:
		{
			m_pControl->SendMessage(WM_GENERICTASK_FSM_CANCELLED);

			ASSERT(m_pFactory->Get_UnitCreateFSMEngine());

			if (m_pFactory->Get_UnitCreateFSMEngine() != 0)
			{
				ClassFactoryId = CLASSFACTORY_ID_UNITCREATE_CONFIG;
			}
						
		} break;

		case COMAPPLICATION_TASK_ID_UPLOAD_MOODS:
		{
			m_pControl->SendMessage(WM_GENERICTASK_FSM_CANCELLED);

			ASSERT(m_pFactory->Get_UploadMoodFSMEngine());

			if (m_pFactory->Get_UploadMoodFSMEngine() != 0)
			{
				ClassFactoryId = CLASSFACTORY_ID_UPLOAD_MOODS;
			}	
		} break;

		case COMAPPLICATION_TASK_ID_UPLOAD_IRRX:
		{
			m_pControl->SendMessage(WM_GENERICTASK_FSM_CANCELLED);

			ASSERT(m_pFactory->Get_UploadIRRXFSMEngine());

			if (m_pFactory->Get_UploadIRRXFSMEngine() != 0)
			{
				ClassFactoryId = CLASSFACTORY_ID_UPLOAD_IRRX;
			}	
		} break;

		case COMAPPLICATION_TASK_ID_UPLOAD_SCHEDULE:
		{
			m_pControl->SendMessage(WM_GENERICTASK_FSM_CANCELLED);

			ASSERT(m_pFactory->Get_UploadScheduleFSMEngine());

			if (m_pFactory->Get_UploadScheduleFSMEngine() != 0)
			{
				ClassFactoryId = CLASSFACTORY_ID_UPLOAD_SCHEDULE;
			}	
		} break;

		case COMAPPLICATION_TASK_ID_REQUEST_FOR_STATUSSEN:
		{
			ASSERT(0);

		} break;

		case COMAPPLICATION_TASK_ID_BINDINGS_READWRITE_ENGINE:
		{
			ASSERT(0);

		} break;

		case COMAPPLICATION_TASK_ID_SCREENMAP_UPDATE_ENGINE:
		{		
			m_pControl->PostMessage(WM_GENERICTASK_FSM_CANCELLED);

			ASSERT(m_pFactory->Get_ScreenmapsUpdateEngine());
	
			if (m_pFactory->Get_ScreenmapsUpdateEngine() != 0)
			{
				ClassFactoryId = CLASSFACTORY_ID_SCREENMAP_UPDATE_ENGINE;
			}

		} break;

		case COMAPPLICATION_TASK_ID_MEMORYDUMP_ENGINE:
		{		
			m_pControl->SendMessage(WM_GENERICTASK_FSM_CANCELLED);

			ASSERT(m_pFactory->Get_MemoryDumpEngine());

			if (m_pFactory->Get_MemoryDumpEngine() != 0)
			{
				ClassFactoryId = CLASSFACTORY_ID_MEMORYDUMP_ENGINE;
			}

		} break;
	
		case COMAPPLICATION_TASK_ID_BROWSEPNP_ENGINE:
		{
			ASSERT(0);

		} break;

		case COMAPPLICATION_TASK_ID_MODEMCONNECT_ENGINE:
		{				
			ASSERT(0);

		} break;

		case COMAPPLICATION_TASK_ID_NICERRORSFSM_ENGINE:
		{	
			m_pControl->PostMessage(WM_GENERICTASK_FSM_CANCELLED);
	
			ASSERT(m_pFactory->Get_NicErrorsFSMEngine());

			if (m_pFactory->Get_NicErrorsFSMEngine() != 0)
			{
				ClassFactoryId = CLASSFACTORY_ID_NICERRORSFSM_ENGINE;	
			}

		} break;	

		case COMAPPLICATION_TASK_ID_FTP:
		{
			m_pControl->PostMessage(WM_GENERICTASK_FSM_CANCELLED);
	
			ASSERT(m_pFactory->Get_ConfigCanFTP());

			if (m_pFactory->Get_ConfigCanFTP() != 0)
			{
				ClassFactoryId = CLASSFACTORY_ID_FTP;	
			}
		} break;

		case COMAPPLICATION_TASK_ID_UPLOAD_SMAP_PAGEINFO:
		{
			m_pControl->PostMessage(WM_GENERICTASK_FSM_CANCELLED);
			
			ASSERT(m_pFactory->Get_ConfigSmapPageInfoFSMEngine());

			if (m_pFactory->Get_ConfigSmapPageInfoFSMEngine() != 0)
			{
				ClassFactoryId = CLASSFACTORY_ID_UPLOAD_SMAP_PAGEINFO;
			}		
			
		} break;

		case COMAPPLICATION_TASK_ID_UPLOAD_SMAP_MENU:
		{
			m_pControl->SendMessage(WM_GENERICTASK_FSM_CANCELLED);
			
			ASSERT(m_pFactory->Get_ConfigSmapMenuFSMEngine());

			if (m_pFactory->Get_ConfigSmapMenuFSMEngine() != 0)
			{
				ClassFactoryId = CLASSFACTORY_ID_UPLOAD_SMAP_MENU;
			}		
			
		} break;

		case COMAPPLICATION_TASK_ID_UPLOAD_SMAP_AVROOM:
		{
			m_pControl->SendMessage(WM_GENERICTASK_FSM_CANCELLED);
			
			ASSERT(m_pFactory->Get_ConfigSmapAVRoomFSMEngine());

			if (m_pFactory->Get_ConfigSmapAVRoomFSMEngine() != 0)
			{
				ClassFactoryId = CLASSFACTORY_ID_UPLOAD_SMAP_AVROOM;
			}		

		} break;

		case COMAPPLICATION_TASK_ID_UPLOAD_GATEWAYCFG:
		{
			m_pControl->SendMessage(WM_GENERICTASK_FSM_CANCELLED);
			
			ASSERT(m_pFactory->Get_ConfigGatewayUFSMEngine());

			if (m_pFactory->Get_ConfigGatewayUFSMEngine() != 0)
			{
				ClassFactoryId = CLASSFACTORY_ID_UPLOAD_GATEWAY_CFG;
			}		

		} break;

		case COMAPPLICATION_TASK_ID_UPLOAD_ROOMCTRL_AUDIOCFG:
		{
			m_pControl->SendMessage(WM_GENERICTASK_FSM_CANCELLED);
			
			ASSERT(m_pFactory->Get_ConfigRoomCtrlAVFSMEngine());

			if (m_pFactory->Get_ConfigRoomCtrlAVFSMEngine() != 0)
			{
				ClassFactoryId = CLASSFACTORY_ID_UPLOAD_ROOMCTRL_AUDIOCFG;
			}		
		} break;

		case COMAPPLICATION_TASK_ID_UPLOAD_BOSEST_CFG:
		{
			m_pControl->SendMessage(WM_GENERICTASK_FSM_CANCELLED);
			
			ASSERT(m_pFactory->Get_ConfigBoseSTFSMEngine());

			if (m_pFactory->Get_ConfigBoseSTFSMEngine() != 0)
			{
				ClassFactoryId = CLASSFACTORY_ID_UPLOAD_BOSEST_CFG;
			}		
		} break;

		case COMAPPLICATION_TASK_ID_UPLOAD_LEDSCFG:
		{
			m_pControl->SendMessage(WM_GENERICTASK_FSM_CANCELLED);
			
			ASSERT(m_pFactory->Get_ConfigLedsCfgFSMEngine());

			if (m_pFactory->Get_ConfigLedsCfgFSMEngine() != 0)
			{
				ClassFactoryId = CLASSFACTORY_ID_UPLOAD_LEDSCFG;
			}		
		} break;

		case COMAPPLICATION_TASK_ID_UPLOAD_SMAPLITECFG:
		{
			m_pControl->SendMessage(WM_GENERICTASK_FSM_CANCELLED);

			ASSERT(m_pFactory->Get_ConfigSmapLiteFSMEngine());

			if (m_pFactory->Get_ConfigSmapLiteFSMEngine() != 0)
			{
				ClassFactoryId = CLASSFACTORY_ID_UPLOAD_SMAPLITE;
			}	
		} break;

		default:
		{
			ASSERT(0);

		} break;
	}

	if (ClassFactoryId != CLASSFACTORY_ID_UNDEFINED)
	{
		PostMessage(WM_CLASSFACTORY_DESTROY,ClassFactoryId );
	}
}
///////////////////////////////////////////////////////////////////////////////////

// Sedert V16.15:
// Bijhouden in de CCommunicationServerInfo of we lokaal of remote aan het werken zijn.
LONG CComApplication::OnCommunicationModemStatus(WPARAM wParam,LPARAM lParam)
{		
	const BOOL connectionOpen = m_pCommunicationObject->IsConnectionOpen();

	// Globale connect/disconnect.
	CCommunicationStatus& communicationStatus = GetCommunicationStatus();
	communicationStatus.setConnected(connectionOpen);

	CCommunicationServerInfo& serverInfo = communicationStatus.getServerInfo();
	if (connectionOpen) {		
		const CCommunicationObject::type_t type = m_pCommunicationObject->getType();
		if (CCommunicationObject::TCPIP == type) {
			serverInfo.setCommunicationType(CCommunicationServerInfo::TCPIP);
			serverInfo.setConnectionMethod(m_pCommunicationObject->getTcpConnectionMethod());						// sedert v16.15
		}
		else if (CCommunicationObject::SERIAL == type) {
			serverInfo.setCommunicationType(CCommunicationServerInfo::SERIAL);
			serverInfo.setSerialComport(m_pCommunicationObject->GetComPort());
			serverInfo.setConnectionMethod(CONNECTION_METHOD_LOCAL);												// sedert v16.15
		}
		else {
			ASSERT(0);
		}
	}
	else{
		serverInfo.clear();		// ????
	}
	m_pControl->PostMessage( WM_COMMUNICATION_MODEM_STATUS );
	return TRUE;
}

// protected...
void CComApplication::SetConnected(BOOL fConnected) 
{
	CDisplayDevice::Clear();

	if (fConnected == TRUE) {		
		if (m_pFactory->Get_ModemConnectEngine() == NULL) {
			m_pFactory->Create_ModemConnectEngine();
		}
		ASSERT(NULL != m_hWnd);

		CCommunicationConnectFsm* pFsm = m_pFactory->Get_ModemConnectEngine();
		if (NULL == pFsm) {
			ASSERT(NULL != pFsm);
			return;
		}

		// Sedert V16.14: Via batch file toelaten of er cfg toegelaten is via de touchscreen.
		pFsm->setEnableCfgTouchscreen(CAppState::getInstance().getAppPermisions().getTcpCfgTouchscreenEnabled());

		if (!pFsm->startConnect(m_hWnd,m_pCommunicationObject)) {
			const CString msgError("Connect failed - Check communication settings.");
			CDisplayDevice::DisplayErrorMsg(msgError);
		}
	}
	else {
		if (m_pFactory->Get_ModemConnectEngine() != NULL) {
			PostMessage( WM_CLASSFACTORY_DESTROY, CLASSFACTORY_ID_MODEMCONNECT_ENGINE);
		}		

		GetCommunicationStatus().setCanLoggerEnabled(FALSE);
		m_pCommunicationObject->CloseConnection();		
		PostMessage( WM_COMMUNICATION_MODEM_STATUS );
	}
}

void CComApplication::setSerialUsbConnect(int port) {
	if (NULL == m_pCommunicationObject) {
		ASSERT(0 != m_pCommunicationObject);
		return;
	}
	if (IsConnected()) {
		return;
	}
	if (port<0 || port>255) {
		port = TMGetCommunicationSettings()->GetDefaultComPort();
	}

	const CCommunicationSettingsUsb settings(port);
	m_pCommunicationObject->setSettings(settings);	
	SetConnected(TRUE);
}

// Connectie ifv van de vorige instellingen.
// Deze is ok wanneer we al eens via de startdialoog geconnecteerd hebben.
void CComApplication::setConnect() {
	SetConnected(TRUE);
}

void CComApplication::setTcpConnect(ConnectionMethod_t method, const CString& hostName, int port, const CString& pwd, const CString& cloudServerLoginToken) {
	if (NULL == m_pCommunicationObject) {
		ASSERT(0 != m_pCommunicationObject);
		return;
	}
	if (IsConnected()) {
		return;
	}
	const CCommunicationSettingsTcp settings(method, hostName, port, pwd, cloudServerLoginToken);	
	m_pCommunicationObject->setSettings(settings);	
	SetConnected(TRUE);
}

void CComApplication::setDisconnect() {
	SetConnected(FALSE);
}

BOOL CComApplication::IsConnected(void) 
{
	return GetCommunicationStatus().connected();
}

BOOL CComApplication::Start_RequestForStatussen(int nodeAddress) {
	ASSERT(m_pFactory->Get_RequestForNodeStatussen()==NULL);
	if (IsConnected())  {
		if (m_pFactory->Get_RequestForNodeStatussen() == NULL) {	
			if (m_pFactory->Create_RequestForNodeStatussen() != NULL) {						
				ASSERT(m_pFactory->Get_RequestForNodeStatussen());
				if ((nodeAddress < 0) || (nodeAddress > 0xff)) {
					m_pFactory->Get_RequestForNodeStatussen()->Start();
				}
				else {
					m_pFactory->Get_RequestForNodeStatussen()->Start(nodeAddress);
				}
				return TRUE;
			}
			ASSERT(0);
		}
	}
	return FALSE;
}

BOOL CComApplication::Cancel_RequestForStatussen(void) {
	if (IsConnected())  {
		ASSERT(m_pFactory->Get_RequestForNodeStatussen());
		if (m_pFactory->Get_RequestForNodeStatussen() != NULL) {
			m_pFactory->Get_RequestForNodeStatussen()->Cancel();
			return TRUE;
		}
	}
	return FALSE;
}


BOOL CComApplication::Start_WriteAllBindings(void) 
{
	ASSERT( m_pFactory->Get_BindingsReadWriteEngine() == NULL);

	if (m_pFactory->Get_BindingsReadWriteEngine() == NULL)
	{
		if (m_pFactory->Create_BindingsReadWriteEngine() != NULL)
		{
			m_pFactory->Get_BindingsReadWriteEngine()->WriteAllBindings();
			return TRUE;
		}
	}

	ASSERT( m_pFactory->Get_BindingsReadWriteEngine() );
	ASSERT(0);

	return FALSE;
}
BOOL CComApplication::Start_ReadAllBindings(void) 
{
	ASSERT( m_pFactory->Get_BindingsReadWriteEngine() == NULL);

	if (m_pFactory->Get_BindingsReadWriteEngine() == NULL)
	{
		if (m_pFactory->Create_BindingsReadWriteEngine() != NULL)
		{
			m_pFactory->Get_BindingsReadWriteEngine()->ReadAllBindings();
			return TRUE;
		}
	}

	ASSERT( m_pFactory->Get_BindingsReadWriteEngine() );
	ASSERT(0);
	return FALSE;
}

BOOL CComApplication::Start_WriteModifiedBindings(void) 
{
	ASSERT( m_pFactory->Get_BindingsReadWriteEngine() == NULL);
	if (m_pFactory->Get_BindingsReadWriteEngine() == NULL)
	{
		if (m_pFactory->Create_BindingsReadWriteEngine() != NULL)
		{
			m_pFactory->Get_BindingsReadWriteEngine()->WriteModifiedBindings();
		
			return TRUE;
		}		
	}
	ASSERT( m_pFactory->Get_BindingsReadWriteEngine() );
	ASSERT(0);
	return FALSE;
}

///////////////////////////////////////////////////////////////////////

void CComApplication::NotifyEraseBindingsNodeStart(BYTE bNodeAddress)
{
	ASSERT( m_pFactory->Get_BindingsReadWriteEngine() != NULL);
	m_pControl->SendMessage(WM_BINDINGSNODE_ERASE_START,(WPARAM) bNodeAddress);
}
void CComApplication::NotifyEraseBindingsNodeDone(BYTE bNodeAddress) 
{
	ASSERT( m_pFactory->Get_BindingsReadWriteEngine() != NULL);
	m_pControl->SendMessage(WM_BINDINGSNODE_ERASE_DONE,(WPARAM) bNodeAddress);
}
void CComApplication::NotifyEraseBindingsNodeError(BYTE bNodeAddress) 
{
	ASSERT( m_pFactory->Get_BindingsReadWriteEngine() != NULL);
	m_pControl->SendMessage(WM_BINDINGSNODE_ERASE_ERROR,(WPARAM) bNodeAddress);

	// Destroy Engine !
	PostMessage( WM_CLASSFACTORY_DESTROY,
				 CLASSFACTORY_ID_BINDINGS_READWRITE_ENGINE );
}
void CComApplication::NotifyWriteBindingsNodeStart(BYTE bNodeAddress) 
{
	ASSERT( m_pFactory->Get_BindingsReadWriteEngine() != NULL);
	m_pControl->SendMessage(WM_BINDINGSNODE_WRITE_START,(WPARAM) bNodeAddress);
}
void CComApplication::NotifyWriteBindingsNodeDone(BYTE bNodeAddress) 
{
	ASSERT( m_pFactory->Get_BindingsReadWriteEngine() != NULL);
	m_pControl->SendMessage(WM_BINDINGSNODE_WRITE_DONE,(WPARAM) bNodeAddress);
}
void CComApplication::NotifyWriteBindingsNodeError(BYTE bNodeAddress) 
{
	ASSERT( m_pFactory->Get_BindingsReadWriteEngine() != NULL);
	m_pControl->SendMessage(WM_BINDINGSNODE_WRITE_ERROR,(WPARAM) bNodeAddress);

	// Destroy Engine !
	PostMessage( WM_CLASSFACTORY_DESTROY,
				 CLASSFACTORY_ID_BINDINGS_READWRITE_ENGINE );
}

void CComApplication::NotifyReadBindingsNodeStart(BYTE bNodeAddress)
{
	ASSERT( m_pFactory->Get_BindingsReadWriteEngine() != NULL);
	m_pControl->SendMessage(WM_BINDINGSNODE_READ_START,(WPARAM) bNodeAddress);
}
void CComApplication::NotifyReadBindingsNodeDone(BYTE bNodeAddress) 
{
	ASSERT( m_pFactory->Get_BindingsReadWriteEngine() != NULL);
	m_pControl->SendMessage(WM_BINDINGSNODE_READ_DONE,(WPARAM) bNodeAddress);
}
void CComApplication::NotifyReadBindingsNodeError(BYTE bNodeAddress) 
{
	ASSERT( m_pFactory->Get_BindingsReadWriteEngine() != NULL);
	m_pControl->SendMessage(WM_BINDINGSNODE_READ_ERROR,(WPARAM) bNodeAddress);

	// Destroy Engine !
	PostMessage( WM_CLASSFACTORY_DESTROY,
				 CLASSFACTORY_ID_BINDINGS_READWRITE_ENGINE );
}

void CComApplication::NotifyWriteBindingsFSMDone(void) 
{
	ASSERT( m_pFactory->Get_BindingsReadWriteEngine() != NULL);
	m_pControl->SendMessage(WM_BINDINGSNODE_FSMWRITE_DONE);

	// Destroy Engine !
	PostMessage( WM_CLASSFACTORY_DESTROY,
				 CLASSFACTORY_ID_BINDINGS_READWRITE_ENGINE );
}
void CComApplication::NotifyReadBindingsFSMDone(void) 
{
	ASSERT( m_pFactory->Get_BindingsReadWriteEngine() != NULL);
	m_pControl->SendMessage(WM_BINDINGSNODE_FSMREAD_DONE);

	// Destroy Engine !
	PostMessage( WM_CLASSFACTORY_DESTROY,
				 CLASSFACTORY_ID_BINDINGS_READWRITE_ENGINE );
}

// Sedert V16.07: zie ook NOTE_DISCONNECT_WINSOCK en NOTE_WINSOCK_KEEPALIVE.
// 
// Opmerking: Disconnect event wordt niet verstuurd bij een USB modem wanneer de verbinding
// verbroken werd.
void CComApplication::NotifyConnectStatusChanged(BOOL statusConnect) {
	if (!statusConnect) {

		TRACE0("----> SOCKET DISCONNECTED\r\n");

		// Krijgen enkel een event bij de tcp/ip socket - GEEN event bij de USB-Modem.

		// Eerst de Communication status aanpassen !!!
		CCommunicationStatus& refCommunicationStatus = this->GetCommunicationStatus();
		refCommunicationStatus.setConnected(FALSE);

		// Event doorsturen naar de applicatie (na het aanpassen van de communicatie status).
		m_pControl->PostMessage( WM_COMMUNICATION_MODEM_STATUS );
	}	
}

/////////////////////////////////////////////////////////////////////////////
void CComApplication::Fire_OnNodeStatusChanged(BYTE bNodeAddress) {
	m_pControl->PostMessage(WM_NODE_STATUS_CHANGED,(WPARAM) bNodeAddress);
}

void CComApplication::Fire_OnUnitStatusChanged(BYTE bNodeAddress,BYTE bUnitAddress) {
	m_pControl->PostMessage(WM_UNIT_STATUS_CHANGED,(WPARAM) bNodeAddress,(LPARAM) bUnitAddress);
}

///////////////////////////////////////////////////////////////////////////////
void CComApplication::OnCommEvent() 
{	
	#if !defined(USE_MSCOMMCONTROL)
		ASSERT( 0 );
	#endif
	#if defined(USE_MSCOMMCONTROL)
	if (m_pSerialComm != 0)	
		m_pCommunicationObject->ParseRXMessage();		
	#endif
}

LONG CComApplication::OnComDataReceived(WPARAM wParam,LPARAM lParam)
{
	#if defined(USE_MSCOMMCONTROL)
		ASSERT( 0 );
	#endif
	#if !defined(USE_MSCOMMCONTROL)
	m_pCommunicationObject->ParseRxSerialData();		
	#endif
	return TRUE;
}

/*****************************************************************************
 * Notifications From CSerialCommObject !!!
 */

// Sedert V16.43: Deze moet synchroon verstuurd worden naar communicatie tracer anders is 
// de weergave verkeerd.
void CComApplication::NotifyMessageSend(void) { 
	SendMessage(WM_COMM_MESSAGE_SEND,0,0);	
}

void CComApplication::NotifyReceivedMessage(void){ 
	PostMessage(WM_COMM_MESSAGE_RECEIVED,0,0);
}

void CComApplication::NotifyModemCommunicationError(void) {
	if (AfxGetMainWnd() != NULL) 
		AfxGetMainWnd()->SendMessage(WM_COMMUNICATION_ERROR);
}
void CComApplication::NotifyReceivedModemStatus(void) {

	if (m_pFactory->Get_ModemConnectEngine() != NULL)				
		m_pFactory->Get_ModemConnectEngine()->EvSignalModemStatus();

	if (AfxGetMainWnd() != NULL) 
		AfxGetMainWnd()->SendMessage(WM_COMMUNICATION_COMMAND_RECEIVED);
}

void CComApplication::NotifyReceivedModemAddress(void) {

	if (m_pFactory->Get_ModemConnectEngine() != NULL)				
		m_pFactory->Get_ModemConnectEngine()->EvSignalModemAddress();

	if (AfxGetMainWnd() != NULL) 
		AfxGetMainWnd()->SendMessage(WM_COMMUNICATION_COMMAND_RECEIVED);
}

void CComApplication::NotifyReceivedModemVersion(void) {

	if (m_pFactory->Get_ModemConnectEngine() != NULL)				
		m_pFactory->Get_ModemConnectEngine()->EvSignalSoftwareVersion();

	if (AfxGetMainWnd() != NULL) 
		AfxGetMainWnd()->SendMessage(WM_COMMUNICATION_COMMAND_RECEIVED);
}

void CComApplication::NotifyModemCanLoggerStateChanged(void) {
	if (AfxGetMainWnd() != NULL) 
		AfxGetMainWnd()->SendMessage(WM_COMMUNICATION_MODEMLOGGERSTATUS);	
}

void CComApplication::NotifyReceivedAck(void) 
{	
	if (AfxGetMainWnd() != NULL) {
		AfxGetMainWnd()->SendMessage(WM_COMMUNICATION_COMMAND_RECEIVED);
	}		
	if (m_pFactory->Get_ModemConnectEngine() != NULL) {
		m_pFactory->Get_ModemConnectEngine()->EvSignalAckReceived();
	}	
}

void CComApplication::NotifyReceivedNack(void) 
{	
	if (AfxGetMainWnd() != NULL)
		AfxGetMainWnd()->SendMessage(WM_COMMUNICATION_COMMAND_RECEIVED);
}

LONG CComApplication::OnMessageSend(WPARAM wParam,LPARAM lParam)
{	
	if (AfxGetMainWnd() != NULL) {
		AfxGetMainWnd()->SendMessage(WM_COMMUNICATION_MESSAGE_SEND);
	}
	return TRUE;
}

LONG CComApplication::OnMessageReceived(WPARAM wParam,LPARAM lParam)
{
	const CCommunicationObject::ResultRxMsg_t res = m_pCommunicationObject->GetRxMsg();
	if (CCommunicationObject::RESULT_RX_MSG_MODEMFRAME == res) {

		ParseNodeObjectRxMessage();

		if (AfxGetMainWnd() != NULL) {
			AfxGetMainWnd()->SendMessage(WM_COMMUNICATION_MESSAGE_RECEIVED);
		}
		if (m_pFactory->Get_ModemConnectEngine() != NULL) {
			m_pFactory->Get_ModemConnectEngine()->EvSignalMessageReceived();
		}
	}
	else if (CCommunicationObject::RESULT_RX_MSG_LOG == res) {

		if (AfxGetMainWnd() != NULL) {
			AfxGetMainWnd()->SendMessage(WM_COMMUNICATION_MESSAGE_RECEIVED);
		}
	}
	return TRUE;
}

/*********************************************************************************/
void CComApplication::NotifyNicErrorsNodeFsmDone(BYTE bNodeAddress)
{
	CString szMessage;
	szMessage.Format("Node 0x%02x: Request CAN network status: Done",bNodeAddress);
	CDisplayDevice::DisplayInfoMsg(szMessage);
}

void CComApplication::NotifyNicErrorsNodeFsmError(BYTE bNodeAddress)
{
	CString szMessage;
	szMessage.Format("Node 0x%02x: Request CAN network status: Error",bNodeAddress);
	CDisplayDevice::DisplayErrorMsg(szMessage);
}

void CComApplication::NotifyNicErrorsNodeFsmRetry(BYTE bNodeAddress) {
	CString szMessage;
	szMessage.Format("Node 0x%02x: Request CAN network status: Response timeout, retry.", bNodeAddress);
	CDisplayDevice::DisplayInfoMsg(szMessage);
}
/** *****************************************************************
 *
 */
BOOL CComApplication::Start_NicErrorsFsm()
{
	BOOL fResult = FALSE;
	ASSERT(m_pFactory->Get_NicErrorsFSMEngine() == NULL);
	if (m_pFactory->Get_NicErrorsFSMEngine() == NULL)
	{
		if (m_pFactory->Create_NicErrorsFSMEngine() != NULL)
		{
			m_pFactory->Get_NicErrorsFSMEngine()->EvOperationStart();
			fResult = TRUE;
		}	
		ASSERT(m_pFactory->Get_NicErrorsFSMEngine() != NULL);
	}
	else
	{
		ASSERT(0);
	}
	return fResult;
}

BOOL CComApplication::Stop_NicErrorsFsm()
{
	BOOL fResult = FALSE;
	ASSERT(m_pFactory->Get_NicErrorsFSMEngine() != NULL);

	if (m_pFactory->Get_NicErrorsFSMEngine() != NULL)
	{		
		m_pFactory->Get_NicErrorsFSMEngine()->EvOperationCancel();			
		ASSERT(m_pFactory->Get_NicErrorsFSMEngine() != NULL);
		fResult = TRUE;
	}
	else
	{
		ASSERT(0);
	}
	return fResult;
}

void CComApplication::NotifyEventScreenmapFSMInfo(CString s)
{
	CDisplayDevice::DisplayInfoMsg(s);
}

/** **************************************************************************
 * Events to triggger Screenmaps Update LCD
 */
LONG CComApplication::OnScreenmapFSMTriggerCancel(WPARAM wParam,LPARAM lParam)
{
	ASSERT(m_pFactory->Get_ScreenmapsUpdateEngine());
	if (m_pFactory->Get_ScreenmapsUpdateEngine() != NULL)
	{
		m_pFactory->Get_ScreenmapsUpdateEngine()->OnEventCancel();
	}
	return TRUE;
}
LONG CComApplication::OnScreenmapFSMTriggerOk(WPARAM wParam,LPARAM lParam)
{
	ASSERT(m_pFactory->Get_ScreenmapsUpdateEngine());
	if (m_pFactory->Get_ScreenmapsUpdateEngine() != NULL)
	{
		m_pFactory->Get_ScreenmapsUpdateEngine()->OnEventDone();
	}
	return TRUE;
}
LONG CComApplication::OnScreenmapFSMTriggerError(WPARAM wParam,LPARAM lParam)
{
	ASSERT(m_pFactory->Get_ScreenmapsUpdateEngine());
	if (m_pFactory->Get_ScreenmapsUpdateEngine() != NULL)
	{
		m_pFactory->Get_ScreenmapsUpdateEngine()->OnEventError();
	}
	return TRUE;
}

/** *************************************************************************
 * Operations : Write - Cancel
 */
void CComApplication::Start_UploadLcdScreenmaps()
{
	ASSERT(m_pFactory->Get_ScreenmapsUpdateEngine() == NULL);
	if (m_pFactory->Get_ScreenmapsUpdateEngine() == NULL)
	{
		if (m_pFactory->Create_ScreenmapsUpdateEngine() != NULL)
		{
			m_pFactory->Get_ScreenmapsUpdateEngine()->Update();
		}	

		ASSERT(m_pFactory->Get_ScreenmapsUpdateEngine() != NULL);
	}
	else
	{
		ASSERT(0);
	}
}
void CComApplication::Cancel_UploadLcdScreenmaps(void)
{
	ASSERT(m_pFactory->Get_ScreenmapsUpdateEngine() != NULL);
	if (m_pFactory->Get_ScreenmapsUpdateEngine() != NULL)
	{
		m_pFactory->Get_ScreenmapsUpdateEngine()->CancelUpdateLCD();
	}
}
BOOL CComApplication::Cancel_MemoryDump(void)
{
	ASSERT(m_pFactory->Get_MemoryDumpEngine() != NULL);
	if (m_pFactory->Get_MemoryDumpEngine() != NULL) 
	{
		return (m_pFactory->Get_MemoryDumpEngine()->Cancel());
	}
	return FALSE;
}
BOOL CComApplication::Start_MemoryDump(void)
{	
	ASSERT(m_pFactory->Get_MemoryDumpEngine() == NULL);
	if (m_pFactory->Get_MemoryDumpEngine() == NULL)
	{					
		if (m_pFactory->Create_MemoryDumpEngine() != NULL)
		{
			if (CMemoryDumpInfo::GetInstance() != 0)
			{
				//ASSERT(CMemoryDumpInfo::GetInstance()->IsLocked());

				int i;
				CCanNode *pNode;
				const BYTE bNodeAddress = 
					CMemoryDumpInfo::GetInstance()->GetAssignedNodeAddress();
		
				if (TMGetRuntimeNodes()->FindNode(bNodeAddress, &i, &pNode)) 
				{
					return (m_pFactory->Get_MemoryDumpEngine()->
						StartMemoryDump(pNode,CMemoryDumpInfo::GetInstance()));
				}							
			}		
		}
	}
	ASSERT(0);
	return FALSE;
}

BOOL CComApplication::Start_UploadBitmaps(void)
{
	ASSERT(m_pFactory->Get_MemoryDumpEngine() == NULL);

	if (m_pFactory->Get_MemoryDumpEngine() == NULL)
	{							
		if (m_pFactory->Create_MemoryDumpEngine() != NULL)
		{
			if (CMemoryBitmapInfo::GetInstance() != 0)
			{
				int i;
				CCanNode *pNode;
				const BYTE bNodeAddress = 
					CMemoryBitmapInfo::GetInstance()->GetAssignedNodeAddress();
		
				if (TMGetRuntimeNodes()->FindNode(bNodeAddress, &i, &pNode)) 
				{
					return (m_pFactory->Get_MemoryDumpEngine()->StartUploadBitmaps(pNode,
						CMemoryBitmapInfo::GetInstance()));				
				}
			}					
		}	
	}

	ASSERT(0);
	return FALSE;
}

BOOL CComApplication::Start_UploadMultiMediaConfig() 
{
	ASSERT(m_pFactory->Get_MultiMediaConfigEngine() == NULL);

	if ((CConfigMultiMediaInfo::GetInstance() != NULL) && 
		(m_pFactory->Get_MultiMediaConfigEngine() == NULL))
	{	
		if (m_pFactory->Create_MultiMediaConfigEngine() != NULL)
		{
			return (m_pFactory->Get_MultiMediaConfigEngine()
						->Start(CConfigMultiMediaInfo::GetInstance()));
		}

		ASSERT(m_pFactory->Get_MultiMediaConfigEngine());
	}

	return FALSE;
}

BOOL CComApplication::Cancel_UploadMultiMediaConfig(void) 
{
	ASSERT(m_pFactory->Get_MultiMediaConfigEngine());

	if (m_pFactory->Get_MultiMediaConfigEngine() != NULL) 
	{
		return (m_pFactory->Get_MultiMediaConfigEngine()->Cancel());
	}

	return FALSE;
}

BOOL CComApplication::Start_UploadUnitCreateConfig( ) {
	ASSERT(m_pFactory->Get_UnitCreateFSMEngine() == NULL);
	if (m_pFactory->Get_UnitCreateFSMEngine() == NULL) {	
		if (m_pFactory->Create_UnitCreateFSMEngine() != NULL) {
			return (m_pFactory->Get_UnitCreateFSMEngine()->Start(CConfigUnitCreateInfo::getInstance()));
		}
		ASSERT(m_pFactory->Get_UnitCreateFSMEngine());
	}
	return FALSE;
}

BOOL CComApplication::Cancel_UploadUnitCreateConfig( )
{
	ASSERT(m_pFactory->Get_UnitCreateFSMEngine());

	if (m_pFactory->Get_UnitCreateFSMEngine() != NULL) 
	{
		return (m_pFactory->Get_UnitCreateFSMEngine()->Cancel());
	}
	return FALSE;
}
BOOL CComApplication::Start_UploadMoodConfig( )	
{
	ASSERT(m_pFactory->Get_UploadMoodFSMEngine() == NULL);

	if (m_pFactory->Get_UploadMoodFSMEngine() == NULL)
	{	
		if (m_pFactory->Create_UploadMoodFSMEngine() != NULL)
		{
			return (m_pFactory->Get_UploadMoodFSMEngine()
						->Start(CConfigMoodInfo::getInstance()));
		}
		ASSERT(m_pFactory->Get_UploadMoodFSMEngine());
	}
	return FALSE;
}
BOOL CComApplication::Cancel_UploadMoodConfig( )
{
	ASSERT(m_pFactory->Get_UploadMoodFSMEngine());

	if (m_pFactory->Get_UploadMoodFSMEngine() != NULL) 
	{
		return (m_pFactory->Get_UploadMoodFSMEngine()->Cancel());
	}
	return FALSE;
}
BOOL CComApplication::Start_UploadIRRXConfig( )
{
	ASSERT(m_pFactory->Get_UploadIRRXFSMEngine() == NULL);

	if (m_pFactory->Get_UploadIRRXFSMEngine() == NULL)
	{	
		if (m_pFactory->Create_UploadIRRXFSMEngine() != NULL)
		{
			return (m_pFactory->Get_UploadIRRXFSMEngine()
						->Start(CConfigIRRXInfo::getInstance()));
		}
		ASSERT(m_pFactory->Get_UploadIRRXFSMEngine());
	}
	return FALSE;
}			
BOOL CComApplication::Cancel_UploadIRRXConfig( )
{
	ASSERT(m_pFactory->Get_UploadIRRXFSMEngine());

	if (m_pFactory->Get_UploadIRRXFSMEngine() != NULL) 
	{
		return (m_pFactory->Get_UploadIRRXFSMEngine()->Cancel());
	}
	return FALSE;
}

BOOL CComApplication::Start_UploadGatewayCfg( )					// since 0x0620
{
	ASSERT(m_pFactory->Get_ConfigGatewayUFSMEngine() == NULL);

	if (m_pFactory->Get_ConfigGatewayUFSMEngine() == NULL)
	{	
		if (m_pFactory->Create_ConfigGatewayUFSMEngine() != NULL)
		{
			return (m_pFactory->Get_ConfigGatewayUFSMEngine()
						->Start(CConfigGatewayUInfo::getInstance()));
		}
		ASSERT(m_pFactory->Get_ConfigGatewayUFSMEngine());
	}
	return FALSE;
}

BOOL CComApplication::Cancel_UploadGatewayCfg( )				// since 0x0620
{
	ASSERT(m_pFactory->Get_ConfigGatewayUFSMEngine());

	if (m_pFactory->Get_ConfigGatewayUFSMEngine() != NULL) 
	{
		return (m_pFactory->Get_ConfigGatewayUFSMEngine()->Cancel());
	}
	return FALSE;
}


BOOL CComApplication::Start_UploadRoomCtrlAudioCfg( )
{
	ASSERT(m_pFactory->Get_ConfigRoomCtrlAVFSMEngine() == NULL);

	if (m_pFactory->Get_ConfigRoomCtrlAVFSMEngine() == NULL)
	{	
		if (m_pFactory->Create_ConfigRoomCtrlAVFSMEngine() != NULL)
		{
			return (m_pFactory->Get_ConfigRoomCtrlAVFSMEngine()
						->Start(CConfigRoomCtrlAVInfo::getInstance()));
		}
		ASSERT(m_pFactory->Get_ConfigRoomCtrlAVFSMEngine());
	}
	return FALSE;
}

BOOL CComApplication::Cancel_UploadRoomCtrlAudioCfg( )
{
	ASSERT(m_pFactory->Get_ConfigRoomCtrlAVFSMEngine());

	if (m_pFactory->Get_ConfigRoomCtrlAVFSMEngine() != NULL) 
	{
		return (m_pFactory->Get_ConfigRoomCtrlAVFSMEngine()->Cancel());
	}
	return FALSE;
}


// since 0x0802
BOOL CComApplication::Start_UploadBoseSTConfig( )
{
	ASSERT(m_pFactory->Get_ConfigBoseSTFSMEngine() == NULL);

	if (m_pFactory->Get_ConfigBoseSTFSMEngine() == NULL)
	{	
		if (m_pFactory->Create_ConfigBoseSTFSMEngine() != NULL)
		{
			return (m_pFactory->Get_ConfigBoseSTFSMEngine()
						->Start(CConfigBoseSTCfgInfo::getInstance()));
		}
		ASSERT(m_pFactory->Get_ConfigBoseSTFSMEngine());
	}
	return FALSE;
}

BOOL CComApplication::Cancel_UploadBoseSTConfig( )
{
	ASSERT(m_pFactory->Get_ConfigBoseSTFSMEngine());

	if (m_pFactory->Get_ConfigBoseSTFSMEngine() != NULL) 
	{
		return (m_pFactory->Get_ConfigBoseSTFSMEngine()->Cancel());
	}
	return FALSE;
}

// since 0x0804
BOOL CComApplication::Start_UploadLedsConfig( )
{
	ASSERT(m_pFactory->Get_ConfigLedsCfgFSMEngine() == NULL);

	if (m_pFactory->Get_ConfigLedsCfgFSMEngine() == NULL)
	{	
		if (m_pFactory->Create_ConfigLedsCfgFSMEngine() != NULL)
		{
			return (m_pFactory->Get_ConfigLedsCfgFSMEngine()
						->Start(CConfigAppUiCtrlInfo::getInstance()));
		}
		ASSERT(m_pFactory->Get_ConfigLedsCfgFSMEngine());
	}
	return FALSE;
}

BOOL CComApplication::Cancel_UploadLedsConfig( )
{
	ASSERT(m_pFactory->Get_ConfigLedsCfgFSMEngine());

	if (m_pFactory->Get_ConfigLedsCfgFSMEngine() != NULL) 
	{
		return (m_pFactory->Get_ConfigLedsCfgFSMEngine()->Cancel());
	}
	return FALSE;
}

BOOL CComApplication::Start_UploadSmapLiteCfg( )	// since 0x0C05
{	
	ASSERT(m_pFactory->Get_ConfigSmapLiteFSMEngine() == NULL);

	if (m_pFactory->Get_ConfigSmapLiteFSMEngine() == NULL)
	{	
		if (m_pFactory->Create_ConfigSmapLiteFSMEngine() != NULL)
		{
			return (m_pFactory->Get_ConfigSmapLiteFSMEngine()
						->Start(CAppCfgSmapLiteInfo::getInstance()));
		}
		ASSERT(m_pFactory->Get_ConfigSmapLiteFSMEngine());
	}
	return FALSE;
}

BOOL CComApplication::Cancel_UploadSmapLiteCfg( )	// since 0x0C05
{
	ASSERT(m_pFactory->Get_ConfigSmapLiteFSMEngine());

	if (m_pFactory->Get_ConfigSmapLiteFSMEngine() != NULL) 
	{
		return (m_pFactory->Get_ConfigSmapLiteFSMEngine()->Cancel());
	}
	return FALSE;
}

BOOL CComApplication::Start_UploadScheduleConfig( )
{
	ASSERT(m_pFactory->Get_UploadScheduleFSMEngine() == NULL);

	if (m_pFactory->Get_UploadScheduleFSMEngine() == NULL)
	{	
		if (m_pFactory->Create_UploadScheduleFSMEngine() != NULL)
		{
			return (m_pFactory->Get_UploadScheduleFSMEngine()
						->Start(CConfigScheduleInfo::getInstance()));
		}
		ASSERT(m_pFactory->Get_UploadScheduleFSMEngine());
	}
	return FALSE;
}
BOOL CComApplication::Cancel_UploadScheduleConfig( )
{
	ASSERT(m_pFactory->Get_UploadScheduleFSMEngine());

	if (m_pFactory->Get_UploadScheduleFSMEngine() != NULL) 
	{
		return (m_pFactory->Get_UploadScheduleFSMEngine()->Cancel());
	}
	return FALSE;
}

BOOL CComApplication::Start_ftpConfig()			// since 0x0562
{
	ASSERT( m_pFactory->Get_ConfigCanFTP() == 0);

	if (m_pFactory->Get_ConfigCanFTP() == 0)
	{
		if (m_pFactory->Create_ConfigCanFTP() != 0)
		{
			return (m_pFactory->Get_ConfigCanFTP()
				->Start(CConfigCanFTP_Info::getInstance()));
		}
		ASSERT( m_pFactory->Get_ConfigCanFTP() );
	}
	return FALSE;
}

BOOL CComApplication::Cancel_ftpConfig()		// since 0x0562
{
	ASSERT(m_pFactory->Get_ConfigCanFTP());

	if (m_pFactory->Get_ConfigCanFTP() != NULL) 
	{
		return (m_pFactory->Get_ConfigCanFTP()->Cancel());
	}
	return FALSE;
}


BOOL CComApplication::Start_uploadSmapPageInfo( )				// since 0x0571
{
	CConfigSmapPageInfoFSMMaster* p = m_pFactory->Get_ConfigSmapPageInfoFSMEngine();

	ASSERT( p == 0);

	if ( p == 0)
	{
		if (m_pFactory->Create_ConfigSmapPageInfoFSMEngine() != 0)
		{
			p = m_pFactory->Get_ConfigSmapPageInfoFSMEngine();

			return (p->Start(CConfigSmapPageInfoInfo::getInstance()));
		}

		ASSERT( m_pFactory->Get_ConfigSmapPageInfoFSMEngine() );
	}
	return FALSE;
}

BOOL CComApplication::Cancel_uploadSmapPageInfo( )				// since 0x0571
{
	CConfigSmapPageInfoFSMMaster* const p = m_pFactory->Get_ConfigSmapPageInfoFSMEngine();

	ASSERT(p);

	if (p != NULL) 
	{
		return (p->Cancel());
	}
	return FALSE;
}

BOOL CComApplication::Start_UploadSmapMenu( )
{
	CConfigSmapMenuFSMMaster* p = m_pFactory->Get_ConfigSmapMenuFSMEngine();

	ASSERT( p == 0);

	if ( p == 0)
	{
		if (m_pFactory->Create_ConfigSmapMenuFSMEngine() != 0)
		{
			p = m_pFactory->Get_ConfigSmapMenuFSMEngine();

			return (p->Start(CConfigSmapMenuInfo::getInstance()));
		}

		ASSERT( m_pFactory->Get_ConfigSmapMenuFSMEngine() );
	}
	return FALSE;
}

BOOL CComApplication::Cancel_UploadSmapMenu( )
{
	CConfigSmapMenuFSMMaster* const p = m_pFactory->Get_ConfigSmapMenuFSMEngine();

	ASSERT(p);

	if (p != NULL) 
	{
		return (p->Cancel());
	}
	return FALSE;
}

BOOL CComApplication::Start_UploadSmapAVRoom( )
{
	CConfigSmapAVRoomFSMMaster* p = m_pFactory->Get_ConfigSmapAVRoomFSMEngine();

	ASSERT( p == 0);

	if ( p == 0)
	{
		if (m_pFactory->Create_ConfigSmapAVRoomFSMEngine() != 0)
		{
			p = m_pFactory->Get_ConfigSmapAVRoomFSMEngine();

			return (p->Start(CConfigSmapAVRoomInfo::getInstance()));
		}

		ASSERT( m_pFactory->Get_ConfigSmapAVRoomFSMEngine() );
	}
	return FALSE;
}

BOOL CComApplication::Cancel_UploadSmapAVRoom( )
{
	CConfigSmapAVRoomFSMMaster* const p = m_pFactory->Get_ConfigSmapAVRoomFSMEngine();

	ASSERT(p);

	if (p != NULL) 
	{
		return (p->Cancel());
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////
void CComApplication::ResetCommunication()
{	
	m_pCommunicationObject->ResetSerialModem();	
}

/////////////////////////////////////////////////////////////////////////////
// CMyTempWindow message handlers
HWND CComApplication::Create()
{
	LPCTSTR classname;
	classname = AfxRegisterWndClass(0);

	CWnd::CreateEx(NULL,classname,NULL,NULL,1,1,1,1,NULL,NULL);
	ASSERT(m_hWnd!=NULL);
	
	// m_pCommunicationObject->create(m_hWnd);		
	return m_hWnd;
}

void CComApplication::PostNcDestroy()  
{
	m_pCommunicationObject->destroy();
	delete(this);
}

void CComApplication::OnTimer(UINT nIDEvent) 
{	
	switch(nIDEvent) 
	{
		case TIMERID_ACK_TIMEOUT:
		{			
			m_pCommunicationObject->EvAfterAckTimeout();				
		} break;

		case TIMERID_RETRANSMIT_TIMEOUT:			
		{			
			m_pCommunicationObject->EvAfterRetryTimeout();				
		} break;

		case TIMERID_PNP_BROWSE_NORESPONSE:	{
			ASSERT(m_pFactory->Get_BrowsePnpEngine());
			if (m_pFactory->Get_BrowsePnpEngine() != NULL) {
				m_pFactory->Get_BrowsePnpEngine()->Fire_EvTimerNoResponse();	
			}
		} break;
		
		case TIMERID_BROWSE_TIMER: {
			ASSERT(m_pFactory->Get_BrowsePnpEngine());
			KillTimer(TIMERID_BROWSE_TIMER);			
			if (m_pFactory->Get_BrowsePnpEngine() != NULL) {
				m_pFactory->Get_BrowsePnpEngine()->Fire_EvTimerBrowseTimeout();	
			}
		} break;	

		case TIMERID_PNP_POLL: {							   
			ASSERT(m_pFactory->Get_BrowsePnpEngine());
			if (m_pFactory->Get_BrowsePnpEngine() != NULL) {
				m_pFactory->Get_BrowsePnpEngine()->Fire_EvTimerPoll();	
			}							   
		} break;

		case TIMERID_SCREENMAP_ACKTIMEOUT:
		{			
			ASSERT(m_pFactory->Get_ScreenmapsUpdateEngine());

			KillTimer(TIMERID_SCREENMAP_ACKTIMEOUT);

			if (m_pFactory->Get_ScreenmapsUpdateEngine() != NULL)
			{
				m_pFactory->Get_ScreenmapsUpdateEngine()->EvAfterTimeout();
			}
		} break;

		case TIMERID_MEMORYDUMP_ACKTIMEOUT :
		{
			ASSERT(m_pFactory->Get_MemoryDumpEngine());

			KillTimer(TIMERID_MEMORYDUMP_ACKTIMEOUT);
			
			if (m_pFactory->Get_MemoryDumpEngine() != NULL)
			{
				m_pFactory->Get_MemoryDumpEngine()->EvAfterMemoryDumpTimeout();
			}

		} break;

		case TIMERID_MMCONFIG_ACKTIMEOUT:
		{
			ASSERT(m_pFactory->Get_MultiMediaConfigEngine());

			KillTimer(TIMERID_MMCONFIG_ACKTIMEOUT);
			
			if (m_pFactory->Get_MultiMediaConfigEngine() != NULL)
			{
				m_pFactory->Get_MultiMediaConfigEngine()->EvAfterConfigTimeout();
			}

		} break;

		case TIMERID_EXTERNALCONFIG_ACKTIMEOUT:
		{
			ASSERT(m_pFactory->Get_UnitCreateFSMEngine());

			KillTimer(TIMERID_EXTERNALCONFIG_ACKTIMEOUT);
			
			if (m_pFactory->Get_UnitCreateFSMEngine() != NULL)
			{
				m_pFactory->Get_UnitCreateFSMEngine()->EvAfterConfigTimeout();
			}	
			if (m_pFactory->Get_UploadMoodFSMEngine() != NULL)
			{
				m_pFactory->Get_UploadMoodFSMEngine()->EvAfterConfigTimeout();
			}
		} break;
	
		case TIMERID_REQUESTSTATUSSEN_FSM:
		{
			ASSERT(m_pFactory->Get_RequestForNodeStatussen());

			KillTimer(TIMERID_REQUESTSTATUSSEN_FSM);

			if (m_pFactory->Get_RequestForNodeStatussen() != NULL)
			{ 
				m_pFactory->Get_RequestForNodeStatussen()->EvAfterTimerExpiry();
			}			

		} break;

		case TIMERID_RENAME_TIMEOUT:			
		{
			ASSERT(m_pFactory->Get_RenameNodeUnitsEngine());

			KillTimer(TIMERID_RENAME_TIMEOUT);

			if (m_pFactory->Get_RenameNodeUnitsEngine() != NULL) 
			{
				m_pFactory->Get_RenameNodeUnitsEngine()->Fire_EvAckTimeout();	
			}

		} break;

		case TIMERID_BINDINGS_TIMEOUT:
		{
			ASSERT(m_pFactory->Get_BindingsReadWriteEngine());
			KillTimer(TIMERID_BINDINGS_TIMEOUT);			
			if (m_pFactory->Get_BindingsReadWriteEngine() != NULL) {
				m_pFactory->Get_BindingsReadWriteEngine()->EvAfterTimeout();
			}
		} break;

		case TIMERID_MODEMCONNECT_TIMEOUT:
		{
			//ASSERT(m_pFactory->Get_ModemConnectEngine());

			KillTimer(TIMERID_MODEMCONNECT_TIMEOUT);

			if (m_pFactory->Get_ModemConnectEngine() != NULL)
			{
				m_pFactory->Get_ModemConnectEngine()->EvAfterTimeout();
			}
			
		} break;

		case TIMERID_NICERRORSFSM_TIMEOUT:
		{
			KillTimer(TIMERID_NICERRORSFSM_TIMEOUT);

			if (m_pFactory->Get_NicErrorsFSMEngine() != 0)
			{
				m_pFactory->Get_NicErrorsFSMEngine()->EvSignalTimeout();
			}
			
		} break;
			
	}

	CWnd::OnTimer(nIDEvent);
}

//////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

// Versie 0x0700: Optimalisatie.
 void CComApplication::MessageHandler_EV_UNITCONTROLSTATUS()
 {		
 	CCanNode *pNode;				
	int iEntryNR;				
	const ICommunicationFrame* const pModemFrame = m_pCommunicationObject->GetRxModemFrame();
	const BYTE bNodeAddress = pModemFrame->GetNodeAddress();

	ASSERT(pModemFrame->GetMsgCode() == EV_UNITCONTROLSTATUS);

	if (TMGetRuntimeNodes()->FindNode(bNodeAddress,
									  &iEntryNR,
									  &pNode))
	{
		STRUCT_UNIT *pUnit;
		const BYTE bUnitAddress = pModemFrame->GetUnitAddress();
			
		if (TMGetRuntimeNodes()->FindUnit(pNode,
										  bUnitAddress,
										  &iEntryNR,
										  &pUnit))
		{									
		
			if (pUnit->bUnitType == UNIT_TYPE_CONTROL ||
				pUnit->bUnitType == UNIT_TYPE_LCD_VIRTUAL)
			{
				UNIT_CONTROL_STRUCT* const pControlUnitSpecs = 
					&pUnit->UnitSpecs.ControlUnit;

				pControlUnitSpecs->bControlStatus1=pModemFrame->GetDataBYTE(1);									
				pControlUnitSpecs->bControlStatus2=pModemFrame->GetDataBYTE(2);		
								
				Fire_OnUnitStatusChanged(bNodeAddress,bUnitAddress);

				CRequestForNodeStatus* const pFsm = m_pFactory->Get_RequestForNodeStatussen();
				if (pFsm != NULL)
				{
					pFsm->EvSignalStatusStatusReceived(bNodeAddress,
													   bUnitAddress,
													   pModemFrame->GetDataBYTE(0));								
				}
			}
		}
	}
}

// Versie 0x0700: Optimalisatie.
 void CComApplication::MessageHandler_EV_NODETEXTSTRING()
 {
	// Data bevat een tekst string:
	// Deze handler wordt enkel gebruikt om te debuggen ...
	
	const ICommunicationFrame* const pModemFrame = m_pCommunicationObject->GetRxModemFrame();
	BYTE bLength = pModemFrame->GetMessageLength();

	ASSERT(pModemFrame->GetMsgCode() == EV_NODETEXTSTRING);

	if (bLength > SEGMENTED_BLOCK_LENGTH) 					
		bLength = SEGMENTED_BLOCK_LENGTH;
					
	char szSegmentedData[SEGMENTED_BLOCK_LENGTH];	
	for (int i=0;i<bLength;i++) {
		szSegmentedData[i] = (char) pModemFrame->GetDataBYTE(i);		
	}				
	TRACE("EV_NODETEXTSTRING=%s",szSegmentedData);				
 }

// Versie 0x0700: Optimalisatie.
void CComApplication::MessageHandler_EV_UNITAUDIOSTATUS()
{ 	
 	CCanNode *pNode;				
	int iEntryNR;				
	const ICommunicationFrame* const pModemFrame = m_pCommunicationObject->GetRxModemFrame();	
	const BYTE bNodeAddress = pModemFrame->GetNodeAddress();

	 ASSERT(pModemFrame->GetMsgCode() == EV_UNITAUDIOSTATUS);

	if (TMGetRuntimeNodes()->FindNode(bNodeAddress,
									  &iEntryNR,
									  &pNode))
	{
		STRUCT_UNIT *pUnit;
		const BYTE bUnitAddress = pModemFrame->GetUnitAddress();
					
		if (TMGetRuntimeNodes()->FindUnit(pNode,
										  bUnitAddress,
										  &iEntryNR,
										  &pUnit))
		{			
			const BYTE bDataBYTE0 = pModemFrame->GetDataBYTE(0);

			switch (pUnit->bUnitType)
			{
				case UNIT_TYPE_BOSERS:
				{					
					switch(bDataBYTE0)
					{
						case AUDIO_OBJECT_ATTR_CONFIG:
						{
							pUnit->UnitSpecs.AudioUnit.bAudioConfig = 
								pModemFrame->GetDataBYTE(1);									
						}break;

             			case AUDIO_OBJECT_ATTR_SOURCE_ON:
						{
							const BYTE bDataByte1 = pModemFrame->GetDataBYTE(1);

							pUnit->UnitSpecs.AudioUnit.bAudioStatus2 &= 
								(AUDIO_SOURCESELECT_MASK^0xFF);

							if (bDataByte1 <= 6)
							{
								pUnit->UnitSpecs.AudioUnit.bAudioStatus2 |= bDataByte1;
							}										
						} break;
										
						case AUDIO_OBJECT_ATTR_ONOFF:
						{
							const BYTE bDataByte1 = pModemFrame->GetDataBYTE(1);

							if (bDataByte1) 
							{
								pUnit->UnitSpecs.AudioUnit.bAudioStatus2 |= 
									AUDIO_UNIT_ON; 
							}
							else 
							{
								pUnit->UnitSpecs.AudioUnit.bAudioStatus2 &= 
									(AUDIO_UNIT_ON^0xff);
							}
						   									   
						} break;
										
						case AUDIO_OBJECT_ATTR_STATE:
						{
						  pUnit->UnitSpecs.AudioUnit.bAudioStatus1 =
							  pModemFrame->GetDataBYTE(1);

						} break;

						case AUDIO_OBJECT_ATTR_TUNERFREQ:
						case AUDIO_OBJECT_ATTR_VOLUME:
						case AUDIO_OBJECT_ATTR_DISC:
						{

						} break; 
					} 
				} break;	
			
				case UNIT_TYPE_AUDIO_HC4:
				{
					switch(bDataBYTE0)
					{	
						case AUDIO_MATRIX_OBJECT_ATTR_CONFIG:
						{
							pUnit->UnitSpecs.AudioUnit.bAudioConfig = 
								pModemFrame->GetDataBYTE(1);								
						} break;

						case AUDIO_MATRIX_OBJECT_ATTR_DEST_STATUS:
						{
							pUnit->UnitSpecs.AudioUnit.bDestCurSel = pModemFrame->GetDataBYTE(1);

							for (int i=0;i<MAX_AUDIO_UNIT_DESTINATIONS;i++)
							{
								const BYTE bDataByte = pModemFrame->GetDataBYTE(2+i);

								if (bDataByte&0x80) 
								{
									pUnit->UnitSpecs.AudioUnit.bAudioStatus1 |= (0x01<<i);
								}
								else 
								{
									pUnit->UnitSpecs.AudioUnit.bAudioStatus1 &= ((0x01<<i)^0xFF);
								}

								if (!(bDataByte&0x40))
								{
									pUnit->UnitSpecs.AudioUnit.bDest[i] = 0xFF;
								}
								else 
								{
									pUnit->UnitSpecs.AudioUnit.bDestSrcSel[i] = (bDataByte&0x3F);
								}

							} // for (i=0;i<MAX_AUDIO_UNIT_DESTINATIONS;i++)
						} break;

						case AUDIO_OBJECT_ATTR_SOURCE_ON:
						{
							const BYTE bDataByte1 = pModemFrame->GetDataBYTE(1);
							const BYTE bDataByte2 = pModemFrame->GetDataBYTE(2);

							if ((bDataByte1<MAX_AUDIO_UNIT_DESTINATIONS) &&
								(bDataByte2<MAX_AUDIO_UNIT_SOURCES)) 
							{
								if ((pUnit->UnitSpecs.AudioUnit.bDest[bDataByte1] != 0xFF) &&
									(pUnit->UnitSpecs.AudioUnit.bSrc[bDataByte2] != 0xFF)) 
								{										
									pUnit->UnitSpecs.AudioUnit.bDestCurSel = bDataByte1;											
									pUnit->UnitSpecs.AudioUnit.bDestSrcSel[bDataByte1] = bDataByte2;
									pUnit->UnitSpecs.AudioUnit.bAudioStatus1 |= (0x01<<bDataByte1);			
								}									
							}
						} break;

						case AUDIO_OBJECT_ATTR_ONOFF:
						{									
							const BYTE bDataByte1 = pModemFrame->GetDataBYTE(1);
						
							if (bDataByte1 < MAX_AUDIO_UNIT_DESTINATIONS) 
							{		
								const BYTE bDataByte2 = pModemFrame->GetDataBYTE(2);
								pUnit->UnitSpecs.AudioUnit.bDestCurSel = bDataByte1;
								pUnit->UnitSpecs.AudioUnit.bAudioStatus1 = bDataByte2;	
							}
						} break;

						case AUDIO_OBJECT_ATTR_STATE:										
						{
							pUnit->UnitSpecs.AudioUnit.bAudioStatus1 = pModemFrame->GetDataBYTE(1);
						} break;
					}
				}break;

				case UNIT_TYPE_AUDIO:
				{
					switch(bDataBYTE0)
					{	
						case AUDIO_OBJECT_ATTR_CONFIG:
						{
							pUnit->UnitSpecs.AudioUnit.bAudioConfig = pModemFrame->GetDataBYTE(1);									
						}break;

						case AUDIO_OBJECT_ATTR_SOURCE_ON:
						{  	
							const BYTE bDataByte1 = pModemFrame->GetDataBYTE(1);

							pUnit->UnitSpecs.AudioUnit.bAudioStatus2 &= 
									(AUDIO_SOURCESELECT_MASK^0xFF);
									
							if (bDataByte1 <= 6) 
							{
								pUnit->UnitSpecs.AudioUnit.bAudioStatus2 |= bDataByte1;
							}

						} break;
										
						case AUDIO_OBJECT_ATTR_ONOFF:
						{
							const BYTE bDataByte1 = pModemFrame->GetDataBYTE(1);
							if (bDataByte1) 
							{
								pUnit->UnitSpecs.AudioUnit.bAudioStatus2 |= AUDIO_UNIT_ON; 
							}
							else 
							{
								pUnit->UnitSpecs.AudioUnit.bAudioStatus2 &= (AUDIO_UNIT_ON^0xff);
							}															   										   
						} break;
										
						case AUDIO_OBJECT_ATTR_STATE:
						{
							pUnit->UnitSpecs.AudioUnit.bAudioStatus1 = pModemFrame->GetDataBYTE(1);
						} break;										
					} 
				}break;
				
			} // end switch ...

			Fire_OnUnitStatusChanged(bNodeAddress,bUnitAddress);											

			CRequestForNodeStatus* const pFsm = m_pFactory->Get_RequestForNodeStatussen();
			if (pFsm != NULL) {
				pFsm->EvSignalStatusStatusReceived(bNodeAddress,bUnitAddress,bDataBYTE0);												
			}
			
		} // if (TMGetRuntimeNodes()->FindUnit(

	} // if (TMGetRuntimeNodes()->FindNode(
}


// Versie 0x0700: Optimalisatie.
void CComApplication::MessageHandler_EV_UNITSENSSTATUS()
{
	CCanNode *pNode;				
	int iEntryNR;		
	const ICommunicationFrame* const pModemFrame = m_pCommunicationObject->GetRxModemFrame();	
	const BYTE bNodeAddress = pModemFrame->GetNodeAddress();

	ASSERT(pModemFrame->GetMsgCode() == EV_UNITSENSSTATUS);

	if (TMGetRuntimeNodes()->FindNode(bNodeAddress,
									  &iEntryNR,
									  &pNode))
	{
		STRUCT_UNIT *pUnit;
		const BYTE bUnitAddress = pModemFrame->GetUnitAddress();
					
		if (TMGetRuntimeNodes()->FindUnit(pNode,
										  bUnitAddress,
										  &iEntryNR,
										  &pUnit))
		{		
			if (pUnit->bUnitType == UNIT_TYPE_SENS)
			{
				const BYTE bDataBYTE0 = pModemFrame->GetDataBYTE(0);

				switch(bDataBYTE0)
				{
					case SENS_OBJECT_ATTR_WORKING_SET_POINT:
					{
						pUnit->UnitSpecs.SensorUnit.sSensSetPointValue[pModemFrame->GetDataBYTE(3)] = 
							  pModemFrame->GetDataBYTE(1)*256 + 
							  pModemFrame->GetDataBYTE(2);
					} break;

					case SENS_OBJECT_ATTR_HYST_VALUES:
					{
						const BYTE bPreset = pModemFrame->GetDataBYTE(3);
						if (bPreset<4)
						{
							pUnit->UnitSpecs.SensorUnit.bSensPosHystValue[bPreset] = 
								pModemFrame->GetDataBYTE(1);
							pUnit->UnitSpecs.SensorUnit.bSensNegHystValue[bPreset] = 
								pModemFrame->GetDataBYTE(2);    
						}
					} break;

					case SENS_OBJECT_ATTR_PRESET:
					{
						pUnit->UnitSpecs.SensorUnit.bSensStatus2 &= 
							(SENS_PRESET_MASK^0xff);
						pUnit->UnitSpecs.SensorUnit.bSensStatus2 |= 
							(pModemFrame->GetDataBYTE(1)<<5);									
					} break;

					case SENS_OBJECT_ATTR_CONTROL_ONOFF:
					{
						const BYTE bDataBYTE1 = pModemFrame->GetDataBYTE(1);

						if (bDataBYTE1 == 1) 
						{
							pUnit->UnitSpecs.SensorUnit.bSensStatus2 |= 
								SENS_CONTROL_ON;
						}
						else if (bDataBYTE1 == 0) 
						{
							pUnit->UnitSpecs.SensorUnit.bSensStatus2 &= 
								(SENS_CONTROL_ON^0xff);
						}
					} break;

					case SENS_OBJECT_ATTR_SAMPLE_INTERVAL:
					{
						pUnit->UnitSpecs.SensorUnit.bSensInterval = 
							pModemFrame->GetDataBYTE(1);
					} break;

					case SENS_OBJECT_ATTR_CONFIG:
					{
						pUnit->UnitSpecs.SensorUnit.bSensConfig = 
							pModemFrame->GetDataBYTE(1);
					} break;

					case SENS_OBJECT_ATTR_CURRENT_MEASURE:
					{
						pUnit->UnitSpecs.SensorUnit.sSensCurrentValue = 
							pModemFrame->GetDataBYTE(1)*256 + 
							pModemFrame->GetDataBYTE(2);
					} break;

					case SENS_OBJECT_ATTR_STATE:
					{
						pUnit->UnitSpecs.SensorUnit.bSensStatus1 = 
							pModemFrame->GetDataBYTE(1);
					} break;

					case SENS_OBJECT_ATTR_HYST_OFFSETS:
					{
						pUnit->UnitSpecs.SensorUnit.bSensPosHystOffset = 
							pModemFrame->GetDataBYTE(1);
						pUnit->UnitSpecs.SensorUnit.bSensNegHystOffset = 
							pModemFrame->GetDataBYTE(2); 									
					} break;

					case SENS_OBJECT_ATTR_MEASUREDOFFSET:	/* TM20120404 */
					{						
						pUnit->UnitSpecs.SensorUnit.sMeasuredValueOffset = 
							pModemFrame->GetDataBYTE(1)*256 + 
							pModemFrame->GetDataBYTE(2);

					} break;
					
					case SENS_OBJECT_ATTR_NTC_VERSION:		/* TM20120404 */
					{			
						pUnit->UnitSpecs.SensorUnit.usNTCVersionInfo = 
							pModemFrame->GetDataBYTE(1)*256 + 
							pModemFrame->GetDataBYTE(2);

					} break;

					case SENS_OBJECT_USE_SELECTOR_DATA:
					case SENS_OBJECT_METHOD_FSINC_DEC:
					case SENS_OBJECT_METHOD_INC:
					case SENS_OBJECT_METHOD_DEC:
					case SENS_OBJECT_METHOD_FSONOFF:
					default:
					{
						// absorb                                  
					} break;
				}

				Fire_OnUnitStatusChanged(bNodeAddress,bUnitAddress);													  

				CRequestForNodeStatus* const pFsm = m_pFactory->Get_RequestForNodeStatussen();
				if (pFsm != NULL)
				{
					pFsm->EvSignalStatusStatusReceived(bNodeAddress,
													   bUnitAddress,
													   bDataBYTE0);								
				}
			}
		} // if (TMGetRuntimeNodes()->FindUnit(
	} // if (TMGetRuntimeNodes()->FindNode(
}


// Versie 0x0700: Optimalisatie.
void CComApplication::MessageHandler_EV_UNITDIMSTATUS()
{
	CCanNode *pNode;				
	int iEntryNR;		
	const ICommunicationFrame* const pModemFrame = m_pCommunicationObject->GetRxModemFrame();	
	const BYTE bNodeAddress = pModemFrame->GetNodeAddress();

	ASSERT(pModemFrame->GetMsgCode() == EV_UNITDIMSTATUS);

	if (TMGetRuntimeNodes()->FindNode(bNodeAddress,
									  &iEntryNR,
									  &pNode))
	{
		STRUCT_UNIT *pUnit;
		const BYTE bUnitAddress = pModemFrame->GetUnitAddress();
					
		if (TMGetRuntimeNodes()->FindUnit(pNode,
										  bUnitAddress,
										  &iEntryNR,
										  &pUnit))
		{									
			if (pUnit->bUnitType == UNIT_TYPE_DIM)
			{
				const BYTE bDataBYTE0 = pModemFrame->GetDataBYTE(0);

				switch(bDataBYTE0)
				{
					case DIMMER_OBJECT_ATTR_SPEED:
					{									
						pUnit->UnitSpecs.DimmerUnit.bDimSpeedUp=
							pModemFrame->GetDataBYTE(1);
						pUnit->UnitSpecs.DimmerUnit.bDimSpeedDown=
							pModemFrame->GetDataBYTE(2);
					}break;

					case DIMMER_OBJECT_ATTR_RANGE:
					{								
						pUnit->UnitSpecs.DimmerUnit.bDimMax=
							pModemFrame->GetDataBYTE(1);
						pUnit->UnitSpecs.DimmerUnit.bDimMin=
							pModemFrame->GetDataBYTE(2);   
					} break;

					case DIMMER_OBJECT_ATTR_VALUE:
					{
						pUnit->UnitSpecs.DimmerUnit.bDimCurrentVal=
							pModemFrame->GetDataBYTE(1);	
					} break;

					case DIMMER_OBJECT_ATTR_ONOFF:
					{
						pUnit->UnitSpecs.DimmerUnit.bDimStatus1 &= (DIM_PIR_ON^0xff);

						const BYTE bDataBYTE1 = pModemFrame->GetDataBYTE(1);

						if (bDataBYTE1 == 1) 
						{
							pUnit->UnitSpecs.DimmerUnit.bDimStatus1 &= 
								(DIM_UNIT_STATE_MASK^0xff);
							pUnit->UnitSpecs.DimmerUnit.bDimStatus1 |= 
								DIM_UNIT_ON_STATE;
						}
						else if (bDataBYTE1 == 0) 
						{
							pUnit->UnitSpecs.DimmerUnit.bDimStatus1 &= 
								(DIM_UNIT_STATE_MASK^0xff);
							pUnit->UnitSpecs.DimmerUnit.bDimStatus1 |= 
								DIM_UNIT_OFF_STATE;
						}
					}break;

					case DIMMER_OBJECT_ATTR_STATE:
					{								   
						pUnit->UnitSpecs.DimmerUnit.bDimStatus1=
							pModemFrame->GetDataBYTE(1);
					} break;

					case DIMMER_OBJECT_ATTR_CONFIG:
					{							   								
						pUnit->UnitSpecs.DimmerUnit.bDimConfig=
							pModemFrame->GetDataBYTE(1);
							
					} break;

					case DIMMER_OBJECT_USE_SELECTOR_DATA:
					case DIMMER_OBJECT_METHOD_UP:
					case DIMMER_OBJECT_METHOD_DN:
					case DIMMER_OBJECT_METHOD_FSUD:
					case DIMMER_OBJECT_METHOD_FSUDONOFF:
					case DIMMER_OBJECT_METHOD_OFF:
					case DIMMER_OBJECT_METHOD_ONCV:
					case DIMMER_OBJECT_METHOD_FSCVONOFF:
					case DIMMER_OBJECT_METHOD_FSCVOFF:
					default:
					{
					   // absorb
					} break;
				}		
	
				Fire_OnUnitStatusChanged(bNodeAddress,bUnitAddress);													  

				CRequestForNodeStatus* const pFsm = m_pFactory->Get_RequestForNodeStatussen();
				if (pFsm != NULL)
				{
					pFsm->EvSignalStatusStatusReceived(bNodeAddress,
													   bUnitAddress,
													   bDataBYTE0);								
				}
			}
		}
	}
}

// Versie 0x0700: Optimalisatie.
void CComApplication::MessageHandler_EV_UNITSWITCHSTATUS()
{
	CCanNode *pNode;				
	int iEntryNR;				
	const ICommunicationFrame* const pModemFrame = m_pCommunicationObject->GetRxModemFrame();	
	const BYTE bNodeAddress = pModemFrame->GetNodeAddress();

	ASSERT(pModemFrame->GetMsgCode() == EV_UNITSWITCHSTATUS);

	if (TMGetRuntimeNodes()->FindNode(bNodeAddress,
									  &iEntryNR,
									  &pNode))
	{
		STRUCT_UNIT *pUnit;
					
		const BYTE bUnitAddress = pModemFrame->GetUnitAddress();

		if (TMGetRuntimeNodes()->FindUnit(pNode,
										  bUnitAddress,
										  &iEntryNR,
										  &pUnit))
		{									
			if (pUnit->bUnitType == UNIT_TYPE_SWITCH)
			{
				const BYTE bDataBYTE0 = pModemFrame->GetDataBYTE(0);

				switch(bDataBYTE0)
				{
					case SWITCH_OBJECT_ATTR_ONOFF:
					{
						pUnit->UnitSpecs.RelayUnit.bSwitchStatus1 &= 
							(SWITCH_PIR_ON^0xff);

						const BYTE bDataBYTE1 = pModemFrame->GetDataBYTE(1);

						if (bDataBYTE1 == 1) 
						{
							pUnit->UnitSpecs.RelayUnit.bSwitchStatus1 &= 
								(SWITCH_UNIT_STATE_MASK^0xff);

							pUnit->UnitSpecs.RelayUnit.bSwitchStatus1 |= 
								SWITCH_UNIT_ON_STATE;
						}
						else if (bDataBYTE1 == 0) 
						{
							pUnit->UnitSpecs.RelayUnit.bSwitchStatus1 &= 
								(SWITCH_UNIT_STATE_MASK^0xff);

							pUnit->UnitSpecs.RelayUnit.bSwitchStatus1 |= 
								SWITCH_UNIT_OFF_STATE;
						}
					} break;

					case SWITCH_OBJECT_ATTR_STATE:
					{
						pUnit->UnitSpecs.RelayUnit.bSwitchStatus1=
							pModemFrame->GetDataBYTE(1);	
									
					} break;

					case SWITCH_OBJECT_ATTR_CONFIG:
					{ 
						pUnit->UnitSpecs.RelayUnit.bSwitchConfig=
							pModemFrame->GetDataBYTE(1);								
									
					} break;
								
					case SWITCH_OBJECT_ATTR_STOPTIMER:
					{ 
						pUnit->UnitSpecs.RelayUnit.bSwitchStopTimerReloadValue=
							pModemFrame->GetDataBYTE(1);
									
					} break;

					case SWITCH_OBJECT_USE_SELECTOR_DATA:
					case SWITCH_OBJECT_METHOD_OFF:
					case SWITCH_OBJECT_METHOD_ON:
					case SWITCH_OBJECT_METHOD_FSONOFF:
					default:
					{
						// absorb   
					} break;
				} 			
	
				Fire_OnUnitStatusChanged(bNodeAddress,bUnitAddress);													 

				CRequestForNodeStatus* const pFsm = m_pFactory->Get_RequestForNodeStatussen();
				if (pFsm != NULL)
				{
					pFsm->EvSignalStatusStatusReceived(bNodeAddress,
													   bUnitAddress,
													   bDataBYTE0);								
				}
			}
		}
	}
}

// Versie 0x0700: Optimalisatie.
// IMPLEMENT_UNIT_ENERGY
void CComApplication::MessageHandler_EV_UNITENERGYMEASUREMENTSTATUS( )
{
	const ICommunicationFrame* const pModemFrame = m_pCommunicationObject->GetRxModemFrame();
	if (0 != pModemFrame)
	{
		ASSERT(pModemFrame->GetMsgCode() == EV_UNITENERGYMEASUREMENTSTATUS);

		CCanNode *pNode;				
		int iEntryNR;				
		const BYTE bNodeAddress = pModemFrame->GetNodeAddress();

		if (TMGetRuntimeNodes()->FindNode(bNodeAddress,
										  &iEntryNR,
										  &pNode))
		{
			STRUCT_UNIT *pUnit;
					
			const BYTE bUnitAddress = pModemFrame->GetUnitAddress();

			if (TMGetRuntimeNodes()->FindUnit(pNode,
											  bUnitAddress,
											  &iEntryNR,
											  &pUnit))
			{									
				if (UNIT_TYPE_ENERGYMEASURE == pUnit->bUnitType)
				{
					const BYTE bDataBYTE0 = pModemFrame->GetDataBYTE(0);

					switch(bDataBYTE0)
					{
						case EM_ATTR_STATE:
						{
							const BYTE bStatus1 = pModemFrame->GetDataBYTE(1);
							const BYTE bStatus2 = pModemFrame->GetDataBYTE(2);
						} break;

						case EM_ATTR_CONFIG:
						{
							const BYTE bConfig = pModemFrame->GetDataBYTE(1);

						} break;

						case EM_ATTR_SUPPORTED:
						{
							const unsigned long ulSupportedFlags =
								(pModemFrame->GetDataBYTE(1)<<8) + pModemFrame->GetDataBYTE(2);

						} break;

						case EM_ATTR_MEASURESTATUS:
						{
						
						} break;

						case EM_ATTR_SAMPLE_INTERVAL:
						{
							const BYTE bSampleInterval = pModemFrame->GetDataBYTE(1);

						} break;

					} /* switch(bDataBYTE0) */

					Fire_OnUnitStatusChanged(bNodeAddress,bUnitAddress);													 

					CRequestForNodeStatus* const pFsm = m_pFactory->Get_RequestForNodeStatussen();
					if (pFsm != NULL)
					{
						pFsm->EvSignalStatusStatusReceived(bNodeAddress,
														   bUnitAddress,
														   bDataBYTE0);								
					}
				} 				
			}
		}
	}	
}

// Versie 0x0700: Optimalisatie.
void CComApplication::MessageHandler_EV_UNITDUOSWITCHSTATUS()
{
	CCanNode *pNode;				
	int iEntryNR;		
	const ICommunicationFrame* const pModemFrame = m_pCommunicationObject->GetRxModemFrame();
	const BYTE bNodeAddress = pModemFrame->GetNodeAddress();

	ASSERT(pModemFrame->GetMsgCode() == EV_UNITDUOSWITCHSTATUS);

	if (TMGetRuntimeNodes()->FindNode(bNodeAddress,
									  &iEntryNR,
									  &pNode))
	{
		STRUCT_UNIT *pUnit;
		const BYTE bUnitAddress = pModemFrame->GetUnitAddress();
					
		if (TMGetRuntimeNodes()->FindUnit(pNode,
										  bUnitAddress,
										  &iEntryNR,
										  &pUnit))
		{									
			if (pUnit->bUnitType == UNIT_TYPE_DUOSWITCH)
			{
				const BYTE bDataBYTE0 = pModemFrame->GetDataBYTE(0);

				switch(bDataBYTE0)
				{
					case DUOSWITCH_OBJECT_ATTR_UPSTOP:
					{
						const BYTE bDataBYTE1 = pModemFrame->GetDataBYTE(1);

						if (bDataBYTE1 == 1)
						{
							pUnit->UnitSpecs.DuoSwitchUnit.bDuoSwitchStatus1 &= 
								(DUOSWITCH_UNIT_STATE_MASK^0xff);
							pUnit->UnitSpecs.DuoSwitchUnit.bDuoSwitchStatus1 |= 
								DUOSWITCH_UNIT_UP_STATE;
						}
						else if (bDataBYTE1 == 0)
						{
							pUnit->UnitSpecs.DuoSwitchUnit.bDuoSwitchStatus1 &= 
								(DUOSWITCH_UNIT_STATE_MASK^0xff);
							pUnit->UnitSpecs.DuoSwitchUnit.bDuoSwitchStatus1 |= 
								DUOSWITCH_UNIT_STOP_STATE;
						}
					} break; 

					case DUOSWITCH_OBJECT_ATTR_DOWNSTOP:
					{
						const BYTE bDataBYTE1 = pModemFrame->GetDataBYTE(1);

						if (bDataBYTE1 == 1)
						{
							pUnit->UnitSpecs.DuoSwitchUnit.bDuoSwitchStatus1 &= 
								(DUOSWITCH_UNIT_STATE_MASK^0xff);
							pUnit->UnitSpecs.DuoSwitchUnit.bDuoSwitchStatus1 |= 
								DUOSWITCH_UNIT_DOWN_STATE;
						}
						else if (bDataBYTE1 == 0)
						{
							pUnit->UnitSpecs.DuoSwitchUnit.bDuoSwitchStatus1 &= 
								(DUOSWITCH_UNIT_STATE_MASK^0xff);
							pUnit->UnitSpecs.DuoSwitchUnit.bDuoSwitchStatus1 |= 
								DUOSWITCH_UNIT_STOP_STATE;
						}
					} break; 

					case DUOSWITCH_OBJECT_ATTR_STATE:
					{
						pUnit->UnitSpecs.DuoSwitchUnit.bDuoSwitchStatus1=
							pModemFrame->GetDataBYTE(1);
							
					} break; 

					case DUOSWITCH_OBJECT_ATTR_CONFIG:
					{
						pUnit->UnitSpecs.DuoSwitchUnit.bDuoSwitchConfig=
							pModemFrame->GetDataBYTE(1);
							
					} break; 
								
					case DUOSWITCH_OBJECT_ATTR_STOPTIMER:
					{
						pUnit->UnitSpecs.DuoSwitchUnit.bDuoSwitchStopTimerReloadValue=
							pModemFrame->GetDataBYTE(1);
								
					} break; 
								
					case DUOSWITCH_OBJECT_ATTR_SWITCHTIMER:
					{
						pUnit->UnitSpecs.DuoSwitchUnit.bDuoSwitchSwitchTimerReloadValue = 
							pModemFrame->GetDataBYTE(1);
								
					} break; 
				} 
	
				Fire_OnUnitStatusChanged(bNodeAddress,bUnitAddress);													

				CRequestForNodeStatus* const pFsm = m_pFactory->Get_RequestForNodeStatussen();
				if (pFsm != NULL)
				{
					pFsm->EvSignalStatusStatusReceived(bNodeAddress,
													   bUnitAddress,
													   bDataBYTE0);								
				}
			}
		}
	}
}

// Versie 0x0700: Optimalisatie.
void CComApplication::MessageHandler_EV_UNITALARMSTATUS()
{
	// TM_CHANGES_ALARM_ARITECH: EV_UNITALARMSTATUS event handler
	
	CCanNode *pNode;				
	int iEntryNR;		
	const ICommunicationFrame* const pModemFrame = m_pCommunicationObject->GetRxModemFrame();
	const BYTE bNodeAddress = pModemFrame->GetNodeAddress();

	ASSERT(pModemFrame->GetMsgCode() == EV_UNITALARMSTATUS);

	if (TMGetRuntimeNodes()->FindNode(bNodeAddress,
									  &iEntryNR,
									  &pNode))
	{
		STRUCT_UNIT *pUnit;
		const BYTE bUnitAddress = pModemFrame->GetUnitAddress();
					
		if (TMGetRuntimeNodes()->FindUnit(pNode,
										  bUnitAddress,
										  &iEntryNR,
										  &pUnit))
		{									
			if (pUnit->bUnitType == UNIT_TYPE_ALARM)
			{
				const BYTE bDataBYTE0 = pModemFrame->GetDataBYTE(0);

				switch(bDataBYTE0)
				{
					case ALARM_OBJECT_ATTR_CONFIG:
					{
						pUnit->UnitSpecs.AlarmUnit.bAlarmConfig = pModemFrame->GetDataBYTE(1);

					} break;				

					default:
					{
						// De andere methods worden voorlopig niet gebruikt ...

					} break;
				} 
	
				Fire_OnUnitStatusChanged(bNodeAddress,bUnitAddress);													

				CRequestForNodeStatus* const pFsm = m_pFactory->Get_RequestForNodeStatussen();
				if (pFsm != NULL)
				{
					pFsm->EvSignalStatusStatusReceived(bNodeAddress,
													   bUnitAddress,
													   bDataBYTE0);								
				}
			}
		}
	}
}

// Versie 0x0700: Optimalisatie.
void CComApplication::MessageHandler_EV_UNITAVMATRIXSTATUS()	// TM_AVMATRIX_INPUT
{	
	CCanNode *pNode;				
	int iEntryNR;				
	const ICommunicationFrame* const pModemFrame = m_pCommunicationObject->GetRxModemFrame();
	const BYTE bNodeAddress = pModemFrame->GetNodeAddress();

	ASSERT(pModemFrame->GetMsgCode() == EV_UNITAVMATRIXSTATUS);

	if (TMGetRuntimeNodes()->FindNode(bNodeAddress,
									  &iEntryNR,
									  &pNode))
	{
		STRUCT_UNIT *pUnit;
		const BYTE bUnitAddress = pModemFrame->GetUnitAddress();
					
		if (TMGetRuntimeNodes()->FindUnit(pNode,
										  bUnitAddress,
										  &iEntryNR,
										  &pUnit))
		{									
			if (pUnit->bUnitType == UNIT_TYPE_AVMATRIX)
			{
				const BYTE bDataBYTE0 = pModemFrame->GetDataBYTE(0);

				switch(bDataBYTE0)
				{
					case AVMATRIX_OBJECT_ATTR_CONFIG:
					{
						pUnit->UnitSpecs.AVMatrixUnit.bAVMatrixConfig = 
							pModemFrame->GetDataBYTE(1);

					} break;

					default:
					{
						
					} break;
				}

				Fire_OnUnitStatusChanged(bNodeAddress,bUnitAddress);													

				CRequestForNodeStatus* const pFsm = m_pFactory->Get_RequestForNodeStatussen();
				if (pFsm != NULL)
				{
					pFsm->EvSignalStatusStatusReceived(bNodeAddress,
													   bUnitAddress,
													   bDataBYTE0);								
				}
			}
		}
	}
}

// Versie 0x0700: Optimalisatie.
void CComApplication::MessageHandler_UnitStatus()
{
	CCanNode *pNode;				
	int iEntryNR;				
	const ICommunicationFrame* const pModemFrame = m_pCommunicationObject->GetRxModemFrame();
	const BYTE bNodeAddress = pModemFrame->GetNodeAddress();

	if (TMGetRuntimeNodes()->FindNode(bNodeAddress,
									  &iEntryNR,
									  &pNode))
	{
		STRUCT_UNIT *pUnit;
		const BYTE bUnitAddress = pModemFrame->GetUnitAddress();
					
		if (TMGetRuntimeNodes()->FindUnit(pNode,
										  bUnitAddress,
										  &iEntryNR,
										  &pUnit))
		{									
			CRequestForNodeStatus* const pFsm = m_pFactory->Get_RequestForNodeStatussen();
			if (pFsm != NULL)
			{
				pFsm->EvSignalStatusStatusReceived(bNodeAddress,
												   bUnitAddress,
												   pModemFrame->GetDataBYTE(0));								
			}

		}// end if findunit
	}// end if find node
}

void CComApplication::MessageHandler_FC_LCDINFODIALOG()
{
	const ICommunicationFrame* const pModemFrame = m_pCommunicationObject->GetRxModemFrame();
	ASSERT(pModemFrame->GetMsgCode() == FC_LCDINFODIALOG);

	CCanNode *pNode;
	int iEntryNR;

	if (TMGetRuntimeNodes()->FindNode(pModemFrame->GetNodeAddress(),&iEntryNR,&pNode))
	{			
		BYTE bSegmentedData[SEGMENTED_BLOCK_LENGTH];
		BYTE *pbSegmentedData = &bSegmentedData[0];
				
		const BYTE bLength = pModemFrame->GetMessageLength();								
		for (int i=0;i<bLength && i< SEGMENTED_BLOCK_LENGTH-1;i++) {																	
			*pbSegmentedData++ = pModemFrame->GetDataBYTE(i);						
		}				
					
		*pbSegmentedData = '\0';

		CString szMessage;
		szMessage.Format("INFO Message: Node 0x%02x: '%s'",
						 pNode->bLogicalAddress,
						 bSegmentedData);

		CDisplayDevice::DisplayInfoMsg(szMessage);
	}
}

void CComApplication::MessageHandler_EV_EXTERNALCONFIG()
{
	const ICommunicationFrame* const pModemFrame = m_pCommunicationObject->GetRxModemFrame();
	ASSERT(pModemFrame->GetMsgCode() == EV_EXTERNALCONFIG);

	const BYTE bObjectMember = pModemFrame->GetDataBYTE(0);
	const BYTE bRemoteAddress = pModemFrame->GetNodeAddress();

	switch( bObjectMember )
	{
		case VIDEOMUX_CONFIG_SET_OK:
		case ALARMZONE_CONFIG_SET_OK:
		case AVMATRIX_CONFIG_SET_OK:
		case VIDEOPHONE_CONFIG_SET_OK:
		case VIDEOMUXSOURCE_CONFIG_SET_OK:
		case VIDEOMUXDESTINATION_CONFIG_SET_OK:
		case SPEECHDEVINDOOR_CONFIG_SET_OK:
		case SPEECHDEVOUTDOOR_CONFIG_SET_OK:
	   	case AUDIO_CONFIG_SET_DESTINATION_OK:
		case AUDIO_CONFIG_SET_SOURCE_OK:
		case AUDIO_CONFIG_SET_ROOM_OK:
		case EXTERNAL_CONFIG_INITIALIZE_OK:
		case EXTERNAL_CONFIG_DONE_OK:				// since 0x0560
		{
			if (m_pFactory->Get_MultiMediaConfigEngine() != NULL)
			{
				m_pFactory->Get_MultiMediaConfigEngine()->EvSignalStatusConfigOk(bRemoteAddress);
			}

		}break;

		case VIDEOMUX_CONFIG_SET_ERROR:
		case ALARMZONE_CONFIG_SET_ERROR:
		case AVMATRIX_CONFIG_SET_ERROR:
		case VIDEOPHONE_CONFIG_SET_ERROR:
		case VIDEOMUXSOURCE_CONFIG_SET_ERROR:
		case VIDEOMUXDESTINATION_CONFIG_SET_ERROR:
		case SPEECHDEVINDOOR_CONFIG_SET_ERROR:
		case SPEECHDEVOUTDOOR_CONFIG_SET_ERROR:
	    case AUDIO_CONFIG_SET_DESTINATION_ERROR:
		case AUDIO_CONFIG_SET_ROOM_ERROR:
	    case EXTERNAL_CONFIG_INITIALIZE_ERROR:
		case EXTERNAL_CONFIG_DONE_ERROR:			// since 0x0560
		case AUDIO_CONFIG_SET_SOURCE_ERROR:
		{
			if (m_pFactory->Get_MultiMediaConfigEngine() != NULL)
			{
				m_pFactory->Get_MultiMediaConfigEngine()->EvSignalStatusConfigError(bRemoteAddress);
			}	
			
		}break;
		
		case UNIT_CONFIG_SET_OK:				// since 0x0560
		case UNIT_CONFIG_INITIALIZE_OK:			// since 0x0560
		case UNIT_CONFIG_DONE_OK:				// since 0x0560
		{
			if (m_pFactory->Get_UnitCreateFSMEngine() != 0)
			{
				m_pFactory->Get_UnitCreateFSMEngine()->EvSignalStatusConfigOk(bRemoteAddress);
			}

		} break;

		case UNIT_CONFIG_SET_ERROR:				// since 0x0560
		case UNIT_CONFIG_INITIALIZE_ERROR:		// since 0x0560
		case UNIT_CONFIG_DONE_ERROR:			// since 0x0560
		{
			if (m_pFactory->Get_UnitCreateFSMEngine() != 0)
			{
				m_pFactory->Get_UnitCreateFSMEngine()->EvSignalStatusConfigError(bRemoteAddress);
			}
			
		} break;

		case MOOD_CONFIG_DONE_OK:			// since 0x0560
		case MOOD_CONFIG_INITIALIZE_OK:		// since 0x0560
		case MOODHEADER_CONFIG_SET_OK:		// since 0x0560
		case MOODENTRY_CONFIG_SET_OK:
		{
			if (m_pFactory->Get_UploadMoodFSMEngine() != 0)
			{
				m_pFactory->Get_UploadMoodFSMEngine()->EvSignalStatusConfigOk(bRemoteAddress);
			}		

		} break;

		case MOOD_CONFIG_DONE_ERROR:		// since 0x0560
		case MOOD_CONFIG_INITIALIZE_ERROR:	// since 0x0560
		case MOODHEADER_CONFIG_SET_ERROR:	// since 0x0560
		case MOODENTRY_CONFIG_SET_ERROR:	// since 0x0560
		{
			if (m_pFactory->Get_UploadMoodFSMEngine() != 0)
			{
				m_pFactory->Get_UploadMoodFSMEngine()->EvSignalStatusConfigError(bRemoteAddress);
			}	
			
		} break;

		case IRRX_CONFIG_INITIALIZE_OK:		// since 0x0560
		case IRRX_CONFIG_DONE_OK:			// since 0x0560
		case IRRX_CONFIG_SET_OK:			// since 0x0560
		{
			if (m_pFactory->Get_UploadIRRXFSMEngine() != 0)
			{
				m_pFactory->Get_UploadIRRXFSMEngine()->EvSignalStatusConfigOk(bRemoteAddress);
			}	

		} break;
	

		case IRRX_CONFIG_INITIALIZE_ERROR:	// since 0x0560
		case IRRX_CONFIG_DONE_ERROR:		// since 0x0560
		case IRRX_CONFIG_SET_ERROR:			// since 0x0560	
		{
			if (m_pFactory->Get_UploadIRRXFSMEngine() != 0)
			{
				m_pFactory->Get_UploadIRRXFSMEngine()->EvSignalStatusConfigError(bRemoteAddress);
			}		

		} break;

		case GATEWAY_CONFIG_UNIV_SET_OK:
		case GATEWAY_CONFIG_UNIV_INITIALIZE_OK:
		case GATEWAY_CONFIG_UNIV_DONE_OK:
		{
			if (m_pFactory->Get_ConfigGatewayUFSMEngine() != 0)
			{
				m_pFactory->Get_ConfigGatewayUFSMEngine()->EvSignalStatusConfigOk(bRemoteAddress);
			}	

		} break;

		case GATEWAY_CONFIG_UNIV_SET_ERROR:
		case GATEWAY_CONFIG_UNIV_INITIALIZE_ERROR:
		case GATEWAY_CONFIG_UNIV_DONE_ERROR:
		{
			if (m_pFactory->Get_ConfigGatewayUFSMEngine() != 0)
			{
				m_pFactory->Get_ConfigGatewayUFSMEngine()->EvSignalStatusConfigError(bRemoteAddress);
			}	

		} break;

		case SCHEDULE_CONFIG_INITIALIZE_OK:		// since 0x0560
		case SCHEDULE_CONFIG_DONE_OK:			// since 0x0560
		case SCHEDULEITEM_CONFIG_SET_OK:		// since 0x0560
		case SCHEDULEEVENT_CONFIG_SET_OK:		// since 0x0560		
		{
			if (m_pFactory->Get_UploadScheduleFSMEngine() != 0)
			{
				m_pFactory->Get_UploadScheduleFSMEngine()->EvSignalStatusConfigOk(bRemoteAddress);
			}	

		} break;

		case SCHEDULE_CONFIG_INITIALIZE_ERROR:	// since 0x0560
		case SCHEDULE_CONFIG_DONE_ERROR:		// since 0x0560
		case SCHEDULEITEM_CONFIG_SET_ERROR:		// since 0x0560	
		case SCHEDULEEVENT_CONFIG_SET_ERROR:	// since 0x0560	
		{
			if (m_pFactory->Get_UploadScheduleFSMEngine() != 0)
			{
				m_pFactory->Get_UploadScheduleFSMEngine()->EvSignalStatusConfigError(bRemoteAddress);
			}
			
		} break;

		case SMAP_CONFIG_INITIALIZE_OK:			// since 0x0571
		case SMAP_CONFIG_PAGEINFO_HEADER_OK:
		case SMAP_CONFIG_PAGEINFO_DATA_OK:
		case SMAP_CONFIG_DONE_OK:
		{
			CConfigSmapPageInfoFSMMaster* const p = m_pFactory->Get_ConfigSmapPageInfoFSMEngine( );
			if (p)
			{
				p->EvSignalStatusConfigOk(bRemoteAddress);
			}

		} break;

		case SMAP_CONFIG_INITIALIZE_ERROR:		// since 0x0571
		case SMAP_CONFIG_PAGEINFO_HEADER_ERROR:
		case SMAP_CONFIG_PAGEINFO_DATA_ERROR:
		case SMAP_CONFIG_DONE_ERROR:
		{
			CConfigSmapPageInfoFSMMaster* const p = m_pFactory->Get_ConfigSmapPageInfoFSMEngine( );
			if (p)
			{
				p->EvSignalStatusConfigError(bRemoteAddress);
			}

		} break;

		case SMAP_MENU_CONFIG_INITIALIZE_OK:			// since 0x0571
		case SMAP_MENU_CONFIG_SET_OK:
		case SMAP_MENU_CONFIG_DONE_OK:
		{
			CConfigSmapMenuFSMMaster* const p = m_pFactory->Get_ConfigSmapMenuFSMEngine( );
			if (p)
			{
				p->EvSignalStatusConfigOk(bRemoteAddress);
			}

		} break;

		case SMAP_MENU_CONFIG_INITIALIZE_ERROR:			// since 0x0571
		case SMAP_MENU_CONFIG_SET_ERROR:
		case SMAP_MENU_CONFIG_DONE_ERROR:
		{
			CConfigSmapMenuFSMMaster* const p = m_pFactory->Get_ConfigSmapMenuFSMEngine( );
			if (p)
			{
				p->EvSignalStatusConfigError(bRemoteAddress);
			}

		} break;

		case SMAP_CONFIG_AVROOM_INITIALIZE_OK:			// since 0x0610
		case SMAP_CONFIG_AVROOM_SET_OK:
		case SMAP_CONFIG_AVROOM_DONE_OK:
		{
			CConfigSmapAVRoomFSMMaster* const p = m_pFactory->Get_ConfigSmapAVRoomFSMEngine( );
			if (p)
			{
				p->EvSignalStatusConfigOk(bRemoteAddress);
			}

		} break;

		case SMAP_CONFIG_AVROOM_INITIALIZE_ERROR:			// since 0x0610
		case SMAP_CONFIG_AVROOM_SET_ERROR:
		case SMAP_CONFIG_AVROOM_DONE_ERROR:
		{
			CConfigSmapAVRoomFSMMaster* const p = m_pFactory->Get_ConfigSmapAVRoomFSMEngine( );
			if (p)
			{
				p->EvSignalStatusConfigError(bRemoteAddress);
			}
		} break;

		case ROOMCTRL_AUDIOCFG_INITIALISE_OK:
		case ROOMCTRL_AUDIOCFG_DEST_SET_OK:
		case ROOMCTRL_AUDIOCFG_SRC_SET_OK:
		case ROOMCTRL_AUDIOCFG_TNRPRESET_SET_OK:
		case ROOMCTRL_AUDIOCFG_SRCFUNC_SET_OK:
		case ROOMCTRL_AUDIOCFG_DONE_OK:
		{
			CConfigRoomCtrlAVFSMMaster* const p = m_pFactory->Get_ConfigRoomCtrlAVFSMEngine( );
			if (p)
			{
				p->EvSignalStatusConfigOk(bRemoteAddress);
			}
		} break;

		case ROOMCTRL_AUDIOCFG_INITIALISE_ERROR:
		case ROOMCTRL_AUDIOCFG_DEST_SET_ERROR:
		case ROOMCTRL_AUDIOCFG_SRC_SET_ERROR:
		case ROOMCTRL_AUDIOCFG_TNRPRESET_SET_ERROR:
		case ROOMCTRL_AUDIOCFG_SRCFUNC_SET_ERROR:
		case ROOMCTRL_AUDIOCFG_DONE_ERROR:
		{
			CConfigRoomCtrlAVFSMMaster* const p = m_pFactory->Get_ConfigRoomCtrlAVFSMEngine( );
			if (p)
			{
				p->EvSignalStatusConfigError(bRemoteAddress);
			}					
		} break;

		/* since v0802 */
		case BOSEST_CONFIG_INITIALISE_OK:
		case BOSEST_CONFIG_RENDER_OK:
		case BOSEST_CONFIG_DONE_OK:
		{
			CConfigBoseStFSMMaster* const p = m_pFactory->Get_ConfigBoseSTFSMEngine( );
			if (p)
			{
				p->EvSignalStatusConfigOk(bRemoteAddress);
			}

		} break;

		/* since v0802 */
		case BOSEST_CONFIG_INITIALISE_ERROR:
		case BOSEST_CONFIG_RENDER_ERROR:
		case BOSEST_CONFIG_DONE_ERROR:
		{
			CConfigBoseStFSMMaster* const p = m_pFactory->Get_ConfigBoseSTFSMEngine( );
			if (p)
			{
				p->EvSignalStatusConfigError(bRemoteAddress);
			}
		} break;


		/* since v0804 */
		case LEDCFG_INIT_OK:
		case LEDCFG_DATA_OK:
		case LEDCFG_DATA_LED_OK:
		case LEDCFG_DONE_OK:
		{
			CConfigAppUiCtrlFSMMaster* const p = m_pFactory->Get_ConfigLedsCfgFSMEngine( );
			if (p)
			{
				p->EvSignalStatusConfigOk(bRemoteAddress);
			}
		} break;

		/* since v0804 */
		case LEDCFG_INIT_ERROR:
		case LEDCFG_DATA_ERROR:
		case LEDCFG_DATA_LED_ERROR:
		case LEDCFG_DONE_ERROR:
		{
			CConfigAppUiCtrlFSMMaster* const p = m_pFactory->Get_ConfigLedsCfgFSMEngine( );
			if (p)
			{
				p->EvSignalStatusConfigError(bRemoteAddress);
			}
		} break;

		// Since 0x0C05
		case SMAPLITE_CONFIG_INITIALISE_OK:
		case SMAPLITE_CONFIG_ENTRY_OK:
		case SMAPLITE_CONFIG_DONE_OK:
		{
			CAppCfgSmapLiteFSMMaster* const p = m_pFactory->Get_ConfigSmapLiteFSMEngine( );
			if (p)
			{
				p->EvSignalStatusConfigOk(bRemoteAddress);
			}
		} break;

		// Since 0x0C05
		case SMAPLITE_CONFIG_INITIALISE_ERROR:
		case SMAPLITE_CONFIG_ENTRY_ERROR:
		case SMAPLITE_CONFIG_DONE_ERROR:
		{
			CAppCfgSmapLiteFSMMaster* const p = m_pFactory->Get_ConfigSmapLiteFSMEngine( );
			if (p)
			{
				p->EvSignalStatusConfigError(bRemoteAddress);
			}
		} break;

		case EXTERNALCONFIG_NOT_IMPLEMENTED:
		{
			if (1)
			{
				if (m_pFactory->Get_MultiMediaConfigEngine() != NULL)
				{
					m_pFactory->Get_MultiMediaConfigEngine()->EvSignalStatusConfigError(bRemoteAddress);
				}	
			}

			if (1)
			{
				if (m_pFactory->Get_UnitCreateFSMEngine() != 0)
				{
					m_pFactory->Get_UnitCreateFSMEngine()->EvSignalStatusConfigError(bRemoteAddress);
				}			
			}

			if (1)
			{
				if (m_pFactory->Get_UploadMoodFSMEngine() != 0)
				{
					m_pFactory->Get_UploadMoodFSMEngine()->EvSignalStatusConfigError(bRemoteAddress);
				}				
			}			

			if (1)
			{
				if (m_pFactory->Get_UploadIRRXFSMEngine() != 0)
				{
					m_pFactory->Get_UploadIRRXFSMEngine()->EvSignalStatusConfigError(bRemoteAddress);
				}			
			}
		
			if (1)
			{
				if (m_pFactory->Get_UploadScheduleFSMEngine() != 0)
				{
					m_pFactory->Get_UploadScheduleFSMEngine()->EvSignalStatusConfigError(bRemoteAddress);
				}
			}
			
			if (1)
			{
				CConfigSmapPageInfoFSMMaster* const p = m_pFactory->Get_ConfigSmapPageInfoFSMEngine( );
				if (p)
				{
					p->EvSignalStatusConfigError(bRemoteAddress);
				}			
			}
		
			if (1)
			{
				CConfigSmapMenuFSMMaster* const p = m_pFactory->Get_ConfigSmapMenuFSMEngine( );
				if (p)
				{
					p->EvSignalStatusConfigError(bRemoteAddress);
				}
			}

			if (1)
			{
				CConfigSmapAVRoomFSMMaster* const p = m_pFactory->Get_ConfigSmapAVRoomFSMEngine( );
				if (p)
				{
					p->EvSignalStatusConfigError(bRemoteAddress);
				}
			}

			if (1)
			{
				CConfigGatewayUFsmMaster* const p = m_pFactory->Get_ConfigGatewayUFSMEngine( );
				if (p)
				{
					p->EvSignalStatusConfigError(bRemoteAddress);
				}			
			}

			if (1)
			{
				CConfigRoomCtrlAVFSMMaster* const p = m_pFactory->Get_ConfigRoomCtrlAVFSMEngine( );
				if (p)
				{
					p->EvSignalStatusConfigError(bRemoteAddress);
				}			
			}

			if (1)
			{
				CConfigBoseStFSMMaster* const p = m_pFactory->Get_ConfigBoseSTFSMEngine( );
				if (p)
				{
					p->EvSignalStatusConfigError(bRemoteAddress);
				}
			}

		} break;

	}//end switch ....
}

// Sedert V16.26: zie USE_WORKAROUND_MESSAGEERROR_ABSORBE
//
// Sedert V16.30: Onderscheid maken tussen de EV_MESSSAGEERROR berichten.
// De berichten die niet relevant zijn voor de configuratie worden enkel nog in de Communicatie tracer getoond.
// Vroeger werden deze in de scrollview en op het einde van de taak getoond.
void CComApplication::MessageHandler_EV_MESSAGEERROR()
{
	const ICommunicationFrame* const pModemFrame = m_pCommunicationObject->GetRxModemFrame();
	ASSERT(pModemFrame->GetMsgCode() == EV_MESSAGEERROR);
	
	// USE_WORKAROUND_MESSAGEERROR_ABSORBE
	if ((m_absorbeDisplayMsg.m_flagAbsorbe) && 
		(pModemFrame->GetNodeAddress() == m_absorbeDisplayMsg.m_nodeAddress) &&
		(pModemFrame->GetUnitAddress() == 0xff) && 
		(pModemFrame->GetDataBYTE(0) == MESSAGE_ERROR_CAN_TX_PROTOCOL_TIMEOUT)) {		
		m_absorbeDisplayMsg.m_flagAbsorbe = false;
		m_absorbeDisplayMsg.m_nodeAddress = 0xff;
		return;	// Absorbe - Fout bericht niet tonen in de gui.
	}

	const CMessageError_MessageError message(this,pModemFrame);
	const CMessageError_MessageError::LogLevel_t logLevel = message.getLogLevel();
	
	if (logLevel <= CMessageError_MessageError::LOGLEVEL_CONFIG) {		
		CCommunicationLogfile::GetInstance()->Add(message.ToSmallString());
		CDisplayDevice::DisplayErrorMsg(message.ToString());
	}
	if (logLevel <= CMessageError_MessageError::LOGLEVEL_DEBUG) {
		CDisplayDevice::DisplayTracerErrorMsg(message.ToSmallString());
	}
}


// Sedert V16.03:
// Bij het ontvangen van een EV_INIT de datum + tijd weergeven.
void CComApplication::MessageHandler_EV_INIT()
{
	const ICommunicationFrame* const pModemFrame = m_pCommunicationObject->GetRxModemFrame();
	ASSERT(pModemFrame->GetMsgCode() == EV_INIT);

	CMessage_EV_INIT message(this,pModemFrame);

	const CTime time(CTime::GetCurrentTime());
	CString szMessageExt;	
	szMessageExt.Format("%s%s",time.Format("%d %b, %H:%M:%S "),message.ToString());

	CDisplayDevice::DisplayErrorMsg(szMessageExt);
	CCommunicationLogfile::GetInstance()->Add(message.ToSmallString());
}


#define TRACER_RX_EV_NODEBINDINGENTRY(x)		TRACE x				// DEBUGGEN: TRACER INSCHAKELEN.
// #define TRACER_RX_EV_NODEBINDINGENTRY(x)		TRACE x				// DEBUGGEN: TRACER UITSCHAKELEN.

void CComApplication::MessageHandler_EV_NODEBINDINGENTRY() {	
	CCanNode *pNode = NULL;
	int iEntryNR;
	const ICommunicationFrame* const pModemFrame = m_pCommunicationObject->GetRxModemFrame();
	ASSERT(pModemFrame->GetMsgCode() == EV_NODEBINDINGENTRY);
	if (TMGetRuntimeNodes()->FindNode(pModemFrame->GetNodeAddress(),&iEntryNR,&pNode)) {																
		ASSERT(NULL != pNode);
		CBindingFile* const  pBindingFile = pNode->GetBindingFilePtr();
		if (NULL != pBindingFile) {								
			IBindingFileFSM* const pBindingFileFSM = pBindingFile->GetFSM();
			if (NULL == pBindingFileFSM) {
				ASSERT(NULL != pBindingFileFSM);
				return;
			}
			const BYTE bMethodData = pModemFrame->GetDataBYTE(0);
			switch(bMethodData) {
				case HEADER_READ: {
					// V17.02: Controle op het node address.
					CBindingsReadWriteFSM* const pFSM = m_pFactory->Get_BindingsReadWriteEngine();
					if ((NULL != pFSM) && (pNode->getNodeAddress()==pFSM->getCurrentNodeAddress())) {
						const unsigned short usTemp = pModemFrame->GetDataBYTE(1)*256 + pModemFrame->GetDataBYTE(2);										
						pBindingFileFSM->SetBindingNr(usTemp);
						pBindingFileFSM->SetBindingFlags(pModemFrame->GetDataBYTE(3));											
						TRACER_RX_EV_NODEBINDINGENTRY(("RX: EV_NODEBINDINGENTRY (0x%02x) - HEADER_READ\r\n",pNode->getNodeAddress()));
						pBindingFileFSM->EvSignalStatusHeaderRead();
					}							
				} break;

				case HEADER_WRITTEN: {
					// V17.02: Controle op het node address.
					CBindingsReadWriteFSM* const pFSM = m_pFactory->Get_BindingsReadWriteEngine();
					if ((NULL != pFSM) && (pNode->getNodeAddress()==pFSM->getCurrentNodeAddress())) {
						TRACER_RX_EV_NODEBINDINGENTRY(("RX: EV_NODEBINDINGENTRY (0x%02x) - HEADER_WRITTEN\r\n",pNode->getNodeAddress()));
						pBindingFileFSM->EvSignalStatusHeaderWritten();
					}
				} break;
					
				case DATA_READ: {			
					// V17.02: Controle op het node address.
					CBindingsReadWriteFSM* const pFSM = m_pFactory->Get_BindingsReadWriteEngine();
					if ((NULL != pFSM) && (pNode->getNodeAddress()==pFSM->getCurrentNodeAddress())) {				
						CString szData;													
						const int MessageLength = pModemFrame->GetMessageLength()-1;
						szData.GetBufferSetLength(MessageLength);                          							
						for (int i=0,n = MessageLength;i<n;i++)	{
							szData.SetAt(i,pModemFrame->GetDataBYTE(i+1));						
						}							
						TRACER_RX_EV_NODEBINDINGENTRY(("RX: EV_NODEBINDINGENTRY (0x%02x) - DATA_READ\r\n",pNode->getNodeAddress()));
						pBindingFileFSM->SetTempBindingData(szData);										
						pBindingFileFSM->EvSignalStatusDataRead();
					}
				} break;

				case DONE_READ:	{
					// V17.02: Controle op het node address.
					CBindingsReadWriteFSM* const pFSM = m_pFactory->Get_BindingsReadWriteEngine();
					if ((NULL != pFSM) && (pNode->getNodeAddress()==pFSM->getCurrentNodeAddress())) {	
						int i=0;	
						for (int n = pModemFrame->GetMessageLength()-1;	i<n; i++) {
							if (pModemFrame->GetDataBYTE(i+1) == 'S') break;
						}
						const int t=i+1;
						CString szData;	
						szData.GetBufferSetLength(t);
						if (1) {
							for (int i=0;i<t;i++) {
								szData.SetAt(i,pModemFrame->GetDataBYTE(i+1));							
							}
						}
						TRACER_RX_EV_NODEBINDINGENTRY(("RX: EV_NODEBINDINGENTRY (0x%02x) - DONE_READ\r\n",pNode->getNodeAddress()));
						pBindingFileFSM->SetTempBindingData(szData);											
						pBindingFileFSM->EvSignalStatusReadDone();
					}
				} break;

				case WRITE_DONE: {
					// V17.02: Controle op het node address.
					CBindingsReadWriteFSM* const pFSM = m_pFactory->Get_BindingsReadWriteEngine();
					if ((NULL != pFSM) && (pNode->getNodeAddress()==pFSM->getCurrentNodeAddress())) {
						TRACER_RX_EV_NODEBINDINGENTRY(("RX: EV_NODEBINDINGENTRY (0x%02x) - WRITE_DONE\r\n",pNode->getNodeAddress()));
						pBindingFileFSM->EvSignalStatusWriteDone();
					}
				} break;

				case DATA_WRITTEN: {
					// V17.02: Controle op het node address.
					CBindingsReadWriteFSM* const pFSM = m_pFactory->Get_BindingsReadWriteEngine();
					if ((NULL != pFSM) && (pNode->getNodeAddress()==pFSM->getCurrentNodeAddress())) {
						TRACER_RX_EV_NODEBINDINGENTRY(("RX: EV_NODEBINDINGENTRY (0x%02x) - DATA_WRITTEN\r\n",pNode->getNodeAddress()));
						pBindingFileFSM->EvSignalStatusDataWritten();
					}
				}break;
					
				case ENTRY_CLEARED: {
					// Sedert V17.0: Uitbreiding van de bindings.
					// Indien datalengte == 2 bytes => aantal bindings <=255 (= Byte)
					// Indien datalengte == 3 bytes => aantal bindings >255  (2 bytes)
					int bindingNrOfEntries = 0;	
					int bindingXORChecksum = 0;	
					const int messageLength = (pModemFrame->GetMessageLength()-1);
					ASSERT((messageLength == 2) || (messageLength == 3));
					if (3 == messageLength) {
						bindingNrOfEntries = (pModemFrame->GetDataBYTE(1) << 8) + pModemFrame->GetDataBYTE(2);
						bindingXORChecksum = pModemFrame->GetDataBYTE(3);
					}
					else {
						bindingNrOfEntries = pModemFrame->GetDataBYTE(1);
						bindingXORChecksum = pModemFrame->GetDataBYTE(2);
					}
				
#if(ENABLE_TRACE_BINDINGFILEINFO_INFO == 1)
					CString strInfoMessage;
					strInfoMessage.Format("Node 0x%02x: Binding ENTRY_CLEARED. Nr of Bindings=%d - XOR=0x%02x.",
						pNode->getNodeAddress(), bindingNrOfEntries,bindingXORChecksum);
					AppLogging::getInstance().addInfo(strInfoMessage);
#endif
					pNode->setNodeBindingsXORChecksum(bindingXORChecksum);
					pNode->setNodeBindingsNrEntries(bindingNrOfEntries);									
								
					// V17.02: Controle op het node address.
					CBindingsReadWriteFSM* const pFSM = m_pFactory->Get_BindingsReadWriteEngine();
					if ((NULL != pFSM) && (pNode->getNodeAddress()==pFSM->getCurrentNodeAddress())) {
						TRACER_RX_EV_NODEBINDINGENTRY(("RX: EV_NODEBINDINGENTRY (0x%02x) - ENTRY_CLEARED\r\n",pNode->getNodeAddress()));
				 		pBindingFileFSM->EvSignalStatusEntryCleared();
					}
				} break;

				case WR_CANCEL:	{
					// V17.02: Controle op het node address.
					CBindingsReadWriteFSM* const pFSM = m_pFactory->Get_BindingsReadWriteEngine();
					if ((NULL != pFSM) && (pNode->getNodeAddress()==pFSM->getCurrentNodeAddress())) {
						TRACER_RX_EV_NODEBINDINGENTRY(("RX: EV_NODEBINDINGENTRY (0x%02x) - WR_CANCEL\r\n",pNode->getNodeAddress()));
						pBindingFileFSM->EvSignalStatusWriteReadCancel();
					}
				} break;

				case WRONG_BINDING_ENTRY_NR:
				case WRITE_ERROR_WRONG_STATE:
				case READ_ERROR_WRONG_STATE:
				case BINDING_FILE_FULL:
				case BINDING_ENTRY_ERROR:
				case BINDING_FILE_CLOSED:
				case BINDING_FILE_EMPTY:
				case BINDING_FILE_CORRUPT:
				case BINDING_CONFIG_WRONG_STATE:
				case BINDING_TEMP_BUFFER_FULL: {						
					// V17.02: Controle op het node address.
					CBindingsReadWriteFSM* const pFSM = m_pFactory->Get_BindingsReadWriteEngine();
					if ((NULL != pFSM) && (pNode->getNodeAddress()==pFSM->getCurrentNodeAddress())) {
						TRACER_RX_EV_NODEBINDINGENTRY(("RX: EV_NODEBINDINGENTRY (0x%02x) - ERROR-Code=0x%02x\r\n",pNode->getNodeAddress(),bMethodData));
						pBindingFileFSM->EvSignalStatusError();						
					}
					CMessageError_NodeBindingEntry MessageError(this,pModemFrame);				
					CDisplayDevice::DisplayErrorMsg(MessageError.ToString());
					CCommunicationLogfile::GetInstance()->Add(MessageError.ToSmallString());
				} break;
			}
		}		
	}
}

void CComApplication::MessageHandler_EV_NODEBINDINGSFILECONTROL()
{
	CCanNode *pNode = NULL;
	int iEntryNR;
	const ICommunicationFrame* const pModemFrame = m_pCommunicationObject->GetRxModemFrame();
	const BYTE bNodeAddress = pModemFrame->GetNodeAddress();
	ASSERT(pModemFrame->GetMsgCode() == EV_NODEBINDINGSFILECONTROL);
	if (TMGetRuntimeNodes()->FindNode(bNodeAddress,&iEntryNR,&pNode)) {
		ASSERT(NULL != pNode);									
		const BYTE bMethodData = pModemFrame->GetDataBYTE(0);
		switch(bMethodData){
			case FILE_CLOSED: {			
				// Sedert V17.0: Uitbreiding van de bindings.
				// Indien datalengte == 2 bytes => aantal bindings <=255 (= Byte)
				// Indien datalengte == 3 bytes => aantal bindings >255  (2 bytes)
				int bindingNrOfEntries = 0;	
				int bindingXORChecksum = 0;	
				const int messageLength = (pModemFrame->GetMessageLength()-1);
				ASSERT((messageLength == 2) || (messageLength == 3));
				if (3 == messageLength) {
					bindingNrOfEntries = (pModemFrame->GetDataBYTE(1) << 8) + pModemFrame->GetDataBYTE(2);
					bindingXORChecksum = pModemFrame->GetDataBYTE(3);
				}
				else {
					bindingNrOfEntries = pModemFrame->GetDataBYTE(1);
					bindingXORChecksum = pModemFrame->GetDataBYTE(2);
				}
#if(ENABLE_TRACE_BINDINGFILEINFO_INFO == 1)
				CString strInfoMessage;
				strInfoMessage.Format("Node 0x%02x: Binding FILE_CLOSED. Nr of Bindings=%d - XOR=0x%02x.",
					pNode->getNodeAddress(), bindingNrOfEntries,bindingXORChecksum);
				AppLogging::getInstance().addInfo(strInfoMessage);
#endif

				pNode->setNodeBindingsXORChecksum(bindingXORChecksum);
				pNode->setNodeBindingsNrEntries(bindingNrOfEntries);		

				// V17.02: Controle op het node address.
				CBindingsReadWriteFSM* const pFSM = m_pFactory->Get_BindingsReadWriteEngine();
				if ((NULL != pFSM) && (pNode->getNodeAddress()==pFSM->getCurrentNodeAddress())) {
					CBindingFile* const  pBindingFile = pNode->GetBindingFilePtr();	
					if (NULL != pBindingFile) {
						IBindingFileFSM* const pBindingFileFSM = pBindingFile->GetFSM();
						if (NULL != pBindingFileFSM) {
							pBindingFileFSM->EvSignalStatusFileClosed();								
						}
					}
				}
			} break;

			case FILE_OPENED: {
				// Sedert V17.0: Uitbreiding van de bindings.
				// Indien datalengte == 2 bytes => aantal bindings <=255 (= Byte)
				// Indien datalengte == 3 bytes => aantal bindings >255  (2 bytes)
				int bindingNrOfEntries = 0;	
				int bindingXORChecksum = 0;	
				const int messageLength = (pModemFrame->GetMessageLength()-1);
				ASSERT((messageLength == 2) || (messageLength == 3));
				if (3 == messageLength) {
					bindingNrOfEntries = (pModemFrame->GetDataBYTE(1) << 8) + pModemFrame->GetDataBYTE(2);
					bindingXORChecksum = pModemFrame->GetDataBYTE(3);
				}
				else {
					bindingNrOfEntries = pModemFrame->GetDataBYTE(1);
					bindingXORChecksum = pModemFrame->GetDataBYTE(2);
				}

#if(ENABLE_TRACE_BINDINGFILEINFO_INFO == 1)
				CString strInfoMessage;
				strInfoMessage.Format("Node 0x%02x: Binding FILE_OPENED. Nr of Bindings=%d - XOR=0x%02x.",
					pNode->getNodeAddress(), bindingNrOfEntries,bindingXORChecksum);
				AppLogging::getInstance().addInfo(strInfoMessage);
#endif

				pNode->setNodeBindingsXORChecksum(bindingXORChecksum);
				pNode->setNodeBindingsNrEntries(bindingNrOfEntries);	
														
				// V17.02: Controle op het node address.
				CBindingsReadWriteFSM* const pFSM = m_pFactory->Get_BindingsReadWriteEngine();
				if ((NULL != pFSM) && (pNode->getNodeAddress()==pFSM->getCurrentNodeAddress())) {								
					CBindingFile* const  pBindingFile = pNode->GetBindingFilePtr();	
					if (NULL != pBindingFile) {
						IBindingFileFSM* const pBindingFileFSM = pBindingFile->GetFSM();
						if (NULL != pBindingFileFSM) {
							// MUST FIRST INVOKE SetBindingsToRead - Then SetFileEvent(FILE_OPENED);	
							pBindingFileFSM->SetBindingsToRead(bindingNrOfEntries);										
							pBindingFileFSM->EvSignalStatusFileOpened();					
						}
					}
				}							
			} break;

			case FILE_INFO:	{
				// Sedert V17.0: Uitbreiding van de bindings.
				// Indien datalengte == 2 bytes => aantal bindings <=255 (= Byte)
				// Indien datalengte == 3 bytes => aantal bindings >255  (2 bytes)
				int bindingNrOfEntries = 0;	
				int bindingXORChecksum = 0;	
				const int messageLength = (pModemFrame->GetMessageLength()-1);
				ASSERT((messageLength == 2) || (messageLength == 3));
				if (3 == messageLength) {
					bindingNrOfEntries = (pModemFrame->GetDataBYTE(1) << 8) + pModemFrame->GetDataBYTE(2);
					bindingXORChecksum = pModemFrame->GetDataBYTE(3);
				}
				else {
					bindingNrOfEntries = pModemFrame->GetDataBYTE(1);
					bindingXORChecksum = pModemFrame->GetDataBYTE(2);
				}
#if(ENABLE_TRACE_BINDINGFILEINFO_INFO == 1)
				CString strInfoMessage;
				strInfoMessage.Format("Node 0x%02x: Binding FILE_INFO. Nr of Bindings=%d - XOR=0x%02x.",
					pNode->getNodeAddress(), bindingNrOfEntries, bindingXORChecksum);
				AppLogging::getInstance().addInfo(strInfoMessage);
#endif

				pNode->setNodeBindingsXORChecksum(bindingXORChecksum);
				pNode->setNodeBindingsNrEntries(bindingNrOfEntries);																				
							
				if (m_pFactory->Get_BrowsePnpEngine() != 0) {
					m_pFactory->Get_BrowsePnpEngine()->Fire_EvNodeBindingInfo(pNode->getNodeAddress());
				}

				// V17.02: Controle op het node address.
				CBindingsReadWriteFSM* const pFSM = m_pFactory->Get_BindingsReadWriteEngine();
				if ((NULL != pFSM) && (pNode->getNodeAddress()==pFSM->getCurrentNodeAddress())) {
					CBindingFile* const  pBindingFile = pNode->GetBindingFilePtr();	
					if (NULL != pBindingFile) {
						IBindingFileFSM* const pBindingFileFSM = pBindingFile->GetFSM();
						if (NULL != pBindingFileFSM) {
							pBindingFileFSM->EvSignalStatusFileInfo();						
						}
					}
				}
			} break;

			case FILE_ERASED: {

				// Changes V16.31: Logging van de binding entry info
				const int bindingXORChecksum = 0;
				const int bindingNrOfEntries = 0;

#if(ENABLE_TRACE_BINDINGFILEINFO_INFO == 1)
				CString strInfoMessage;
				strInfoMessage.Format("Node 0x%02x: Binding FILE_ERASED. Nr of Bindings=%d - XOR=0x%02x.",
					pNode->getNodeAddress(), bindingNrOfEntries,bindingXORChecksum);
				AppLogging::getInstance().addInfo(strInfoMessage);
#endif

				pNode->setNodeBindingsXORChecksum(bindingXORChecksum);
				pNode->setNodeBindingsNrEntries(bindingNrOfEntries);	
											
				// V17.02: Controle op het node address.
				CBindingsReadWriteFSM* const pFSM = m_pFactory->Get_BindingsReadWriteEngine();
				if ((NULL != pFSM) && (pNode->getNodeAddress()==pFSM->getCurrentNodeAddress())) {
					CBindingFile* const  pBindingFile = pNode->GetBindingFilePtr();	
					if (NULL != pBindingFile) {
						IBindingFileFSM* const pBindingFileFSM = pBindingFile->GetFSM();
						if (NULL != pBindingFileFSM) {
							pBindingFileFSM->EvSignalStatusFileErased();								
						}
					}
				}
			} break;

			case CLOSE_ERROR:
			case OPEN_ERROR:
			case SHARE_VIOLATION:
			case BINDING_TEMP_BUFFER_IN_USE:										
			{
				// V17.02: Controle op het node address.
				CBindingsReadWriteFSM* const pFSM = m_pFactory->Get_BindingsReadWriteEngine();
				if ((NULL != pFSM) && (pNode->getNodeAddress()==pFSM->getCurrentNodeAddress())) {
					CBindingFile* const  pBindingFile = pNode->GetBindingFilePtr();	
					if (NULL != pBindingFile) {
						IBindingFileFSM* const pBindingFileFSM = pBindingFile->GetFSM();
						if (NULL != pBindingFileFSM) {
							pBindingFileFSM->EvSignalStatusFileError();					
						}
					}
				}

				CMessageError_NodeBindingFileControl MessageError(this,pModemFrame);							
				CDisplayDevice::DisplayErrorMsg(MessageError.ToString());
				CCommunicationLogfile::GetInstance()->Add(MessageError.ToSmallString());
			} break;																		                                                   					
						
			default:
			break;
		}// end switch Data[0] 

		Fire_OnNodeStatusChanged(bNodeAddress);
	}
}

// Versie 0x0700: Optimalisatie.
void CComApplication::MessageHandler_EV_LCDBUILDSCREENMAP()
{
	CCanNode *pNode;
	int iEntryNR;
	const ICommunicationFrame* const pModemFrame = m_pCommunicationObject->GetRxModemFrame();	
	const BYTE bNodeAddress = pModemFrame->GetNodeAddress();

	ASSERT(pModemFrame->GetMsgCode() == EV_LCDBUILDSCREENMAP);

	if (TMGetRuntimeNodes()->FindNode(bNodeAddress,&iEntryNR,&pNode))
	{	
		// Kunnen hier ifv het node type controleren...
		// Als het commando afkomstig is van een smarttouch oled is het SmapLite algoritme
		// die uitgevoerd werd...
		CNodeLCDScreenmapFSMMaster* const pFsm = m_pFactory->Get_ScreenmapsUpdateEngine();
		if (pFsm != NULL)
		{					 
			// Sedert versie 0x0700: Optimalisatie.
			const BYTE bMethodData = pModemFrame->GetDataBYTE(0);
			switch(bMethodData)
			{
				case SCREENMAP_INIT_OK:	
				{							
					pFsm->EvSignalStatusInitDone(bNodeAddress);
				} break;
				case SCREENMAP_INIT_ERROR:
				{															
					pFsm->EvSignalStatusInitError(bNodeAddress);
				} break;
				case SCREENMAP_ENTRY_OK :
				{																		
					pFsm->EvSignalStatusEntryDone(bNodeAddress);								
				} break;
				case SCREENMAP_ENTRY_ERROR :
				{															
					pFsm->EvSignalStatusEntryError(bNodeAddress);
				} break;											
				case SCREENMAP_ENTRY_UNIT_NOT_EXISTING:
				{							
					pFsm->EvSignalStatusUnitNotFound(bNodeAddress);
				} break;						
				case SCREENMAP_ENTRY_NODE_NOT_EXISTING:
				{							
					pFsm->EvSignalStatusNodeNotFound(bNodeAddress);
				} break;
				// Voor de nieuwe touchscreen...
				case SCREENMAP_DONE_SMAP_OK:		// nieuw
				{
					pFsm->EvSignalStatusDoneOk(bNodeAddress);
				} break;
				case SCREENMAP_DONE_SMAP_ERROR:		// nieuw
				{
					pFsm->EvSignalStatusDoneError(bNodeAddress);
				} break;						
				default:
				break;					
			}				 
		}
	}
}

void CComApplication::MessageHandler_EV_LCDINFODIALOG()
{
	const ICommunicationFrame* const pModemFrame = m_pCommunicationObject->GetRxModemFrame();
	ASSERT(pModemFrame->GetMsgCode() == EV_LCDINFODIALOG);
	CCanNode *pNode;
	int iEntryNR;
	const BYTE bNodeAddress = pModemFrame->GetNodeAddress();
	if (TMGetRuntimeNodes()->FindNode(bNodeAddress,&iEntryNR,&pNode)) {	
		if (m_pFactory->Get_MemoryDumpEngine() != NULL) {
			m_pFactory->Get_MemoryDumpEngine()->EvSignalStatusMemoryDumpStart(bNodeAddress);
		}
		if (m_pFactory->Get_ConfigCanFTP() != NULL) {
			m_pFactory->Create_ConfigCanFTP()->signalInfoDialogACK(bNodeAddress);
		}
	}
}

void CComApplication::MessageHandler_EV_MEMORYDATADUMP()
{
	const ICommunicationFrame* const pModemFrame = m_pCommunicationObject->GetRxModemFrame();
	ASSERT(pModemFrame->GetMsgCode() == EV_MEMORYDATADUMP);

	CCanNode *pNode;
	int iEntryNR;
	if (TMGetRuntimeNodes()->FindNode(pModemFrame->GetNodeAddress(),&iEntryNR,&pNode)) {				
		BYTE bLength = pModemFrame->GetMessageLength();
					
		if (bLength > SEGMENTED_BLOCK_LENGTH) 					
			bLength = SEGMENTED_BLOCK_LENGTH;
					
		BYTE bSegmentedData[SEGMENTED_BLOCK_LENGTH];	
		int t=0,j=0;
		for (int i=0;i<bLength;i++) 
		{
			t = pModemFrame->GetDataBYTE(i);
			bSegmentedData[i] = t;

			if (t == 0xFF) j++;						
		}				
				
		if (m_pFactory->Get_MemoryDumpEngine() != NULL)
		{
			m_pFactory->Get_MemoryDumpEngine()
				->EvSignalStatusMemoryDumpReadOk(pNode->bLogicalAddress,
												 &bSegmentedData[0],
												 bLength,
												 j);
		}
	}
}

void CComApplication::MessageHandler_EV_WRITEMEMORYDATADONE()
{
	const ICommunicationFrame* const pModemFrame = m_pCommunicationObject->GetRxModemFrame();
	ASSERT(pModemFrame->GetMsgCode() == EV_WRITEMEMORYDATADONE);

	CCanNode *pNode;
	int iEntryNR;
	if (TMGetRuntimeNodes()->FindNode(pModemFrame->GetNodeAddress(),&iEntryNR,&pNode)) {
		if (m_pFactory->Get_MemoryDumpEngine() != NULL) {
			m_pFactory->Get_MemoryDumpEngine()->EvSignalStatusMemoryDumpWriteOk(pNode->bLogicalAddress);
		}
	}
}

// V16.43: Deze functie gebruikt.
static BYTE helper_upgrade_unitTypeAudio(CCanNode *pNode, BYTE bUnitType) {
	BYTE bRetValue = bUnitType;
	if ((pNode->bNodeType == NODE_LCD ||  pNode->bNodeType == NODE_PIN_GATEWAY) && 
		(pNode->bNodeFlags & NODE_MASTER_LCD) &&
		(pNode->bNodeFlags & NODE_LCD_MATRIX_AUDIO)) 
	{					
		if (bUnitType == UNIT_TYPE_AUDIO) {
			bRetValue = UNIT_TYPE_AUDIO_HC4;			
		}
	}
	return bRetValue;
}

void CComApplication::MessageHandler_EV_UNITCONFIG() {	
	CCanNode* pNode = NULL;
	int iEntryNR;
	const ICommunicationFrame* const pModemFrame = m_pCommunicationObject->GetRxModemFrame();
	const BYTE bNodeAddress = pModemFrame->GetNodeAddress();
	const BYTE bUnitAddress = pModemFrame->GetUnitAddress();
	ASSERT(pModemFrame->GetMsgCode() == EV_UNITCONFIG);
	if (TMGetRuntimeNodes()->FindNode(bNodeAddress,&iEntryNR,&pNode)) {
		STRUCT_UNIT *pUnit = NULL;		
		if (TMGetRuntimeNodes()->FindUnit(pNode,bUnitAddress,&iEntryNR,&pUnit)) {		
			const BYTE bUnitType = helper_upgrade_unitTypeAudio(pNode,pModemFrame->GetDataBYTE(0));
			if (pUnit->bUnitType != bUnitType) {
				// Wat moeten we doen: Zijn incompatible units...	
				// als er al bindings aangemaakt werden voor deze unit dan is er een probleem?
				// Wordt dit opgevangen door de compare van de node / runtime unit?
			}
			pUnit->bUnitType=bUnitType;						
			pUnit->bUnitFlags=pModemFrame->GetDataBYTE(1);
			pUnit->bNodeLogicalAddress = bNodeAddress;			
			CNodeDatabase::InitRuntimeUnitSpecs(pUnit);			
		}
		else {		
			STRUCT_UNIT *pUnit = NULL;
			if (TMGetRuntimeNodes()->AddUnit(pNode, bUnitAddress, &iEntryNR, &pUnit)) {		
				const BYTE bUnitType = helper_upgrade_unitTypeAudio(pNode,pModemFrame->GetDataBYTE(0));
				pUnit->bUnitType=bUnitType;						
				pUnit->bUnitFlags=pModemFrame->GetDataBYTE(1);
				pUnit->bNodeLogicalAddress = bNodeAddress;											
				CNodeDatabase::InitRuntimeUnitSpecs(pUnit);				
			}					
		}
	}					
	if (m_pFactory->Get_BrowsePnpEngine() != NULL) 	
		m_pFactory->Get_BrowsePnpEngine()->Fire_EvBrowseGetUnitInfo(bNodeAddress,bUnitAddress);
}	

// Aanpassing V16.45:
// Unit naam enkel updaten wanneer we bezig zijn met het browsen (met deze unit).
void CComApplication::MessageHandler_EV_UNITNAME() {	
	const ICommunicationFrame* const pModemFrame = m_pCommunicationObject->GetRxModemFrame();	
	CCanNode *pNode = NULL;
	int iEntryNR;
	ASSERT(pModemFrame->GetMsgCode() == EV_UNITNAME);
	if (TMGetRuntimeNodes()->FindNode(pModemFrame->GetNodeAddress(), &iEntryNR, &pNode)) {
		STRUCT_UNIT *pUnit = NULL;
		if (TMGetRuntimeNodes()->FindUnit(pNode, pModemFrame->GetUnitAddress(), &iEntryNR, &pUnit)) {		
			const BYTE bMethodData = pModemFrame->GetDataBYTE(0);
			switch(bMethodData) {
				case OBJECT_NAME_WRITE_OK: {
					ASSERT((NULL != pNode) && (NULL != pUnit));
					CRenameNodeUnitsEngine* const pFsm = m_pFactory->Get_RenameNodeUnitsEngine();
					if (pFsm != NULL) 							
						pFsm->Fire_EvUnitNameAck();					
				} break;
							
				case OBJECT_NAME_READ_OK: {			
					ASSERT((NULL != pNode) && (NULL != pUnit));
					CBrowsePnPEngine* const pFsm = m_pFactory->Get_BrowsePnpEngine();
					if ((NULL != pFsm) && (pFsm->getBusyReadUnitName(pNode->getNodeAddress(),pUnit->bUnitAddress))) {
						const BYTE bLength = strlen(pUnit->szUnitName);													
						const BYTE bMessageLength = pModemFrame->GetMessageLength();
						if ((bLength+(bMessageLength-1)) < UNIT_NAME_LENGTH)  {		
							int i;
							for (i=0;i<bMessageLength-1;i++) {
								pUnit->szUnitName[bLength+i] = pModemFrame->GetDataBYTE(i+1);										
								if (pUnit->szUnitName[bLength+i] == '\0')  {																		 
									ASSERT(NULL != pFsm);
									pFsm->Fire_EvUnitNameEnd(pNode->getNodeAddress(),pUnit->bUnitAddress);								
									break;
								}								
								else  {
									pUnit->szUnitName[bLength+i+1]='\0';
								}									
							} // end for (i=0;i<bMessageLength-1;i++)
									
							if (i== bMessageLength-1)  {
								ASSERT(NULL != pFsm);					
								pFsm->Fire_EvUnitNameOk(pNode->getNodeAddress(), pUnit->bUnitAddress);
							}							
						}
						else {
							CBrowsePnPEngine* const pFsm = m_pFactory->Get_BrowsePnpEngine();
							pUnit->szUnitName[UNIT_NAME_LENGTH-1]='\0';
							ASSERT(NULL != pFsm);							
							pFsm->Fire_EvUnitNameEnd(pNode->getNodeAddress(), pUnit->bUnitAddress);									
						}
					}
				} break;
				case OBJECT_NAME_WRITE_ERROR: {
					ASSERT((NULL != pNode) && (NULL != pUnit));
					CRenameNodeUnitsEngine* const pFsm = m_pFactory->Get_RenameNodeUnitsEngine();
					if (pFsm != NULL)								 
						pFsm->Fire_EvUnitNameError();					
				} break;
							
				case OBJECT_NAME_READ_ERROR: {		
					ASSERT((NULL != pNode) && (NULL != pUnit));
					CBrowsePnPEngine* const pFsm = m_pFactory->Get_BrowsePnpEngine();
					if (pFsm != NULL) 								
						pFsm->Fire_EvUnitNameError(pNode->getNodeAddress(), pUnit->bUnitAddress);
				} break;
			} // switch(PMModemFrame.GetDataBYTE(0))
		}				
	}
}

// Aanpassing V16.45:
// Node naam enkel updaten wanneer we bezig zijn met het browsen (met deze node).
void CComApplication::MessageHandler_EV_NODENAME() {	
	const ICommunicationFrame* const pModemFrame = m_pCommunicationObject->GetRxModemFrame();	
	CCanNode *pNode = NULL;
	int iEntryNR;
	ASSERT(pModemFrame->GetMsgCode() == EV_NODENAME);
	if (TMGetRuntimeNodes()->FindNode(pModemFrame->GetNodeAddress(), &iEntryNR, &pNode)) {	
		const BYTE bMethodData = pModemFrame->GetDataBYTE(0);
		switch(bMethodData) {			
			case OBJECT_NAME_WRITE_OK: {
				CRenameNodeUnitsEngine* const pFsm = m_pFactory->Get_RenameNodeUnitsEngine();
				if (pFsm != NULL)			 
					pFsm->Fire_EvNodeNameAck();																
			} break;
			case OBJECT_NAME_READ_OK: {
				ASSERT(NULL != pNode);
				CBrowsePnPEngine* const pFsm = m_pFactory->Get_BrowsePnpEngine();
				if ((NULL != pFsm) && (pFsm->getBusyReadNodeName(pNode->getNodeAddress()))) {
					const BYTE bLength = strlen(pNode->szNodeName);
					const BYTE bMessageLength = pModemFrame->GetMessageLength();
					if ((bLength+(bMessageLength-1)) < NODE_NAME_LENGTH) {
						int i;
						for (i=0;i<bMessageLength-1;i++)  {
							pNode->szNodeName[bLength+i]= pModemFrame->GetDataBYTE(i+1);
							if (pNode->szNodeName[bLength+i] == '\0') {			
								ASSERT(NULL != pFsm);
								pFsm->Fire_EvNodeNameEnd(pNode->getNodeAddress());																										
								break;									
							}
							else  {
								pNode->szNodeName[bLength+i+1]='\0';
							}
						}
						if (i==(bMessageLength-1)) {	
							ASSERT(NULL != pFsm);
							pFsm->Fire_EvNodeNameOk(pNode->getNodeAddress());							
						}										  
					}
					else  {					
						pNode->szNodeName[NODE_NAME_LENGTH-1]='\0';														
						ASSERT(NULL != pFsm);
						pFsm->Fire_EvNodeNameEnd(pNode->getNodeAddress());												
					}
				}
			} break;						
			case OBJECT_NAME_WRITE_ERROR: {							
				CRenameNodeUnitsEngine* const pFsm = m_pFactory->Get_RenameNodeUnitsEngine();
				if (pFsm != NULL) 				
					pFsm->Fire_EvNodeNameError();																		
			} break;
										
			case OBJECT_NAME_READ_ERROR: {			
				ASSERT(NULL != pNode);
				CBrowsePnPEngine* const pFsm = m_pFactory->Get_BrowsePnpEngine();
				if (pFsm!= NULL) 							
					pFsm->Fire_EvNodeNameError(pNode->getNodeAddress());					
			} break;
		} // switch(PMModemFrame.GetDataBYTE(0))
	}
}

// Sedert V16.07: Controle op het nodetype.
// Wordt gebruikt in de EV_NODECONFIG en de EV_NODEPHYSICALADDRESS messagehandlers.
static BOOL helper_checkNodeType(int nodeType) {
	return ((NODE_PIN_STD_UNITS == nodeType) || 
		    (NODE_PIN_GATEWAY == nodeType) || 
			(NODE_PIN_MODEM == nodeType) || 
			(NODE_LCD == nodeType));
}


// Sedert V16.08: Controle op het nodetype.
// Enkel wanneer het nodetype herkend wordt de node properties aanpassen / node toevoegen aan de node database.
void CComApplication::MessageHandler_EV_NODECONFIG()	
{	
	const ICommunicationFrame* const pModemFrame = m_pCommunicationObject->GetRxModemFrame();
	ASSERT(pModemFrame->GetMsgCode() == EV_NODECONFIG);
	
	const BYTE bNodeAddress = pModemFrame->GetNodeAddress();
	const int nodeType = pModemFrame->GetDataBYTE(0);
	if (FALSE == helper_checkNodeType(nodeType)) {
		CString msgWarning;
		msgWarning.Format("Warning: The node 0x%02x responded with an unknown node type. This response will be ignored.", bNodeAddress);
		CDisplayDevice::DisplayInfoMsg(msgWarning);
	}
	else {
		CCanNode *pNode;			
		int iEntryNR;		
		CNodeDatabase* const pNodeDatabase = TMGetRuntimeNodes();
		if (FALSE==pNodeDatabase->FindNode(bNodeAddress,&iEntryNR,&pNode)) {						
			if(FALSE==pNodeDatabase->AddNode(bNodeAddress,&iEntryNR,&pNode)) {				
				// absorbe...
			}
			else {
				// Sedert V16.31: Melding geven.
				// Sedert V16.32: Extra info (zie ook BUG-0173).
				// Wanneer een node niet via de physical browse kon toegevoegd worden maar wel via de gewone browse for nodes 
				// dan wordt het physical address als 0xFFFFF weer gegeven.
				CString msgWarning;
				msgWarning.Format("Warning: Adding node 0x%02x to the node database. This node previously not responded to the physical browse for nodes. The physicaladdress is unknown and will be 0xffffff", 
								  bNodeAddress);
				CDisplayDevice::DisplayInfoMsg(msgWarning);

				pNode->SetBindingFilePtr(NULL);
				pNode->bNodeType = nodeType;
				pNode->bNROfUnits = pModemFrame->GetDataBYTE(1); 
				pNode->bNodeFlags = pModemFrame->GetDataBYTE(2);
				pNode->bDLLAccessControl = pModemFrame->GetDataBYTE(3);				
				pNode->AllocateRuntimeStatus();	/* TM 20060908 */
				ASSERT(CCanNode::NODE_TYPE_UNDEFINED != pNode->bNodeType);
			}
		}
		else {				
			pNode->bNodeType = nodeType;
			pNode->bNROfUnits = pModemFrame->GetDataBYTE(1); 
			pNode->bNodeFlags = pModemFrame->GetDataBYTE(2);
			pNode->bDLLAccessControl = pModemFrame->GetDataBYTE(3);
			pNode->AllocateRuntimeStatus();	/* TM 20060908 */
			ASSERT(CCanNode::NODE_TYPE_UNDEFINED != pNode->bNodeType);
		}			 
			 
		CBrowsePnPEngine* const pFsm = m_pFactory->Get_BrowsePnpEngine();
		if (pFsm!= NULL) 	
			pFsm->Fire_EvNodeConfig(bNodeAddress);
					 
		Fire_OnNodeStatusChanged(bNodeAddress);	
	}
}

// Versie 0x0700: Optimalisatie.
// Sedert 0x0643
void CComApplication::MessageHandler_EV_NODERESOURCES()
{
	const ICommunicationFrame* const pModemFrame = m_pCommunicationObject->GetRxModemFrame();
	CCanNode *pNode = NULL;
	int iEntryNR;

	ASSERT(pModemFrame->GetMsgCode() == EV_NODERESOURCES);

	if (TMGetRuntimeNodes()->FindNode(pModemFrame->GetNodeAddress(), &iEntryNR, &pNode) == FALSE)  {
		// ERROR NODE NOT FOUND 
	}			
	else 
	{		
		const BYTE bMethodData = pModemFrame->GetDataBYTE(0);
		int resourceBlock = -1;
		
		switch(bMethodData) {
			case RESOURCES_0:{												
				memset(pNode->pbResourcesBlock0,0,sizeof(pNode->pbResourcesBlock0));
				for (int i=0;i<RESOURCES_BLOCK0_LENGTH;i++) {
					 pNode->pbResourcesBlock0[i]=pModemFrame->GetDataBYTE(i+1);    
				}																		
				resourceBlock = RESOURCES_0;										
			} break;					
			
			case RESOURCES_1: {								
				memset(pNode->pbResourcesBlock1,0,sizeof(pNode->pbResourcesBlock1));
				for (int i=0;i<RESOURCES_BLOCK1_LENGTH;i++)  {
					 pNode->pbResourcesBlock1[i]=pModemFrame->GetDataBYTE(i+1);    
				}																		
				resourceBlock = RESOURCES_1;				
			} break;
					
			case RESOURCES_2: {			
				// Oppassen:
				// sedert versie 0x0700: Moeten een controle uitvoeren op de message length.
				// Modules die geen extra resource blocks ondersteunen versturen deze extra byte niet.
				int length = pModemFrame->GetMessageLength()-1;
				if (length > RESOURCES_BLOCK2_LENGTH) {
					length = RESOURCES_BLOCK2_LENGTH;
				}
				memset(pNode->pbResourcesBlock2,0,sizeof(pNode->pbResourcesBlock2));								
				for (int i=0;i<length;i++)	{			// oppassen: length gebruiken ipv RESOURCES_BLOCK2_LENGTH
					pNode->pbResourcesBlock2[i]=pModemFrame->GetDataBYTE(i+1);    
				}																		
				resourceBlock = RESOURCES_2;				
			} break;

			// sedert versie 0x0700
			case RESOURCES_3:{
				memset(pNode->pbResourcesBlock3,0,sizeof(pNode->pbResourcesBlock3));
				for (int i=0;i<RESOURCES_BLOCK3_LENGTH;i++) {
					pNode->pbResourcesBlock3[i]=pModemFrame->GetDataBYTE(i+1);    
				}																		
				resourceBlock = RESOURCES_3;				
			} break;

			// Sedert V17.00: Uitbreiding van het aantal bindings in een node.
			// Resource block 4 bevat de MSB waarden van de binding resources.
			case RESOURCES_4: {
				memset(pNode->pbResourcesBlock4,0,sizeof(pNode->pbResourcesBlock4));
				for (int i=0;i<RESOURCES_BLOCK4_LENGTH;i++) {
					pNode->pbResourcesBlock4[i]=pModemFrame->GetDataBYTE(i+1);    
				}																		
				resourceBlock = RESOURCES_4;				
			} break;
					
		} // switch(PMModemFrame.GetDataBYTE(0))	

		if (resourceBlock != -1) {
			CBrowsePnPEngine* const pBrowsePnpEngine = m_pFactory->Get_BrowsePnpEngine();
			if (pBrowsePnpEngine != NULL) {
				ASSERT(NULL != pNode);
				pBrowsePnpEngine->Fire_EvNodeResources(pNode->getNodeAddress(), resourceBlock);
			}
		}
	} // OK, Node found...
}

// Versie 0x0700: Optimalisatie.
void CComApplication::MessageHandler_EV_NODEDLLSTATUS()
{		
	CCanNode *pNode;
	int iEntryNR;
	const ICommunicationFrame* const pModemFrame = m_pCommunicationObject->GetRxModemFrame();
	const BYTE bNodeAddress = pModemFrame->GetNodeAddress();

	ASSERT(pModemFrame->GetMsgCode() == EV_NODEDLLSTATUS);

	if (TMGetRuntimeNodes()->FindNode(bNodeAddress,
									  &iEntryNR,
									  &pNode) 
									  == FALSE) 
	{
		// ERROR NODE NOT FOUND 
	}
	else 
	{
		ASSERT(pNode->GetRuntimeStatus() != 0);

		if (pNode->GetRuntimeStatus() != 0)
		{
			// Sedert versie 0x0700: Optimalisatie.
			const BYTE bMethodData = pModemFrame->GetDataBYTE(0);
			switch(bMethodData)
			{
				case 0:
				{
					for (int i=0;i<DLL_STATUS_BLOCK0_LENGTH;i++)						
					{								
						pNode->GetRuntimeStatus()->SetStatusBlock0(i,
							pModemFrame->GetDataBYTE(i+1));
					}

					if (m_pFactory->Get_NicErrorsFSMEngine() != 0)
					{
						m_pFactory->Get_NicErrorsFSMEngine()->
							EvSignalNicErrors0(pNode->bLogicalAddress);
					}

				} break;

				case 1:
				{
					for (int i=0;i<DLL_STATUS_BLOCK1_LENGTH;i++)					
					{						 							
						pNode->GetRuntimeStatus()->SetStatusBlock1(i,
							pModemFrame->GetDataBYTE(i+1));
					}

					if (m_pFactory->Get_NicErrorsFSMEngine() != 0)
					{
						m_pFactory->Get_NicErrorsFSMEngine()->
							EvSignalNicErrors1(pNode->bLogicalAddress);
					}

				} break;
			} 

			Fire_OnNodeStatusChanged(bNodeAddress);
		}		
	} // OK, Node found...
}


void CComApplication::MessageHandler_EV_DATABASEMANAGMENT( )
{
	const ICommunicationFrame* const pModemFrame = m_pCommunicationObject->GetRxModemFrame();

	if(0!=pModemFrame)
	{
		const BYTE bObjectMember=pModemFrame->GetDataBYTE(0);
		const BYTE bNodeAddress=pModemFrame->GetNodeAddress();

		ASSERT(pModemFrame->GetMsgCode() == EV_DATABASEMANAGMENT);

		// Tijdelijke code ???
		CConfigIRTX::onReceivedDatabaseACK(bNodeAddress,bObjectMember);

		// Tijdeljke code ???
		CConfigSonosSrcItems::onReceivedDatabaseACK(bNodeAddress,bObjectMember);
	}
}

// Sedert V16.08: Controle op het nodetype.
// Enkel wanneer een bekend node type dan wordt deze toegevoegd aan de node database of wordt het nodetype upgedate.
void CComApplication::MessageHandler_EV_NODEPHYSICALADDRESS()		
{
	const ICommunicationFrame* const pModemFrame = m_pCommunicationObject->GetRxModemFrame();
	ASSERT(pModemFrame->GetMsgCode() == EV_NODEPHYSICALADDRESS);

	CBrowsePnPEngine* const pFsm = m_pFactory->Get_BrowsePnpEngine();
	if (pFsm != NULL) 
	{
		if (pFsm->IsBusyPnp()) 
		{
			// Sedert versie 0x0800: pNode initialiseren met 0.
			CCanNode *pNode = 0;					
			int iEntryNR;
			const unsigned long ulPhysicalAddress = ((unsigned long)(pModemFrame->GetDataBYTE(1))<<16)+
													((unsigned long)(pModemFrame->GetDataBYTE(2))<<8)+
													((unsigned long)(pModemFrame->GetDataBYTE(3)));


			TRACE("RX EV_NODEPHYSICALADDRESS - Physical Address = 0x%06x\r\n",ulPhysicalAddress);
			
			// TM_CHANGES_NODE_0xFF
			// Controle op het logisch node address.
			const BYTE bNodeLogicalAddress = pModemFrame->GetNodeAddress();

			// Sedert V16.08: Controle op een geldig node type...
			const int nodeType = pModemFrame->GetDataBYTE(0);
			if ((NODE_ILLEGAL_ADDRESS==bNodeLogicalAddress) || (bNodeLogicalAddress<FIRST_VALID_LOGICAL_ADDRESS))
			{
				// Melding geven
				CString szMessage;
				szMessage.Format("Error add node.\n" 
								 "The node with physical address 0x%x could not be added to the database "
								 "because the logical node address 0x%02x is invalid",
								 ulPhysicalAddress,
								 bNodeLogicalAddress);

				AfxMessageBox(szMessage,MB_ICONSTOP);

				// Oppassen:
				// Kunnen hier geen fout naar de FSM versturen omdat de QHSM FSM door de messageBox
				// in de war komt.				
				// Beter optie zou zijn de PNP FSM zelf laten controleren en een Pre Browse fout geven.
				// Kunnen dan via de wizard de logische addressen herstellen.
			}
			else
			{
				CNodeDatabase* const pNodeDatabase = TMGetRuntimeNodes();
				// If node received does not exist, create new entry... 
				if (pNodeDatabase->FindPhysicalNode(ulPhysicalAddress,&iEntryNR,&pNode) == FALSE) 
				{						
					if (FALSE == helper_checkNodeType(nodeType)) {
						// Sedert V16.08: Indien we een ongeldig node type ontvangen dan de node niet toevoegen 
						// aan de node database...					

						// Sedert V16.31: melding geven.
						CString message;
						message.Format("Warning: The physical node 0x%06x responded with an unknown node type. this response will be ignored.",ulPhysicalAddress);
						CDisplayDevice::DisplayInfoMsg(message);
					}
					else {						
						if (pNodeDatabase->AddPhysicalNode(ulPhysicalAddress,&iEntryNR,&pNode) == FALSE) {											
							CString szMessage;
							szMessage.Format("Error add node.\n" 
											 "The node with physical address 0x%x could not be added to the database."
											 "A possible cause is a duplicate or wrong physical node address.",
											 ulPhysicalAddress);

							AfxMessageBox(szMessage,MB_ICONSTOP);
						}
						else 
						{						
							pNode->bLogicalAddress = bNodeLogicalAddress;											
							pNode->bNodeType = nodeType;
							pNode->SetBindingFilePtr(NULL);
							pNode->AllocateRuntimeStatus();				
						}
					}
				}
				else 
				{
					if (helper_checkNodeType(nodeType)) {						
						if (bNodeLogicalAddress != pNode->bLogicalAddress) {
							// Sedert V16.31: melding geven wanneer we een ander node address ontvangen.
							CString message;
							message.Format("Warning: The physical node 0x%06x responded with a different logical node address 0x%02x. Previous the node address was 0x%02x.",
										   ulPhysicalAddress, pNode->bLogicalAddress, bNodeLogicalAddress);
							CDisplayDevice::DisplayInfoMsg(message);
						}
						pNode->bLogicalAddress = bNodeLogicalAddress;			
						pNode->bNodeType = nodeType;
						pNode->AllocateRuntimeStatus();
					}					
				}																		
						 
				if (0!=pNode) {

					// Sedert versie 0x0800.
					// Controle of pNode een geldige pointer is.
					// Dit is mogelijk wanneer de node niet gevonden is en ook niet kon
					// toegevoegd worden...

#if(0)
					CString szData;
					szData.Format(_T("%06x"),pNode->ulPhysicalAddress);						
					MessageTransfer(BROADCAST_NODE,
									BROADCAST_UNIT,
									FC_PNPPHYSADRACK,
									3,	
									szData);			
					TRACE("Send FC_PNPPHYSADRACK -> Physical Address=0x%06x\r\n",pNode->ulPhysicalAddress);
#endif 

					// Sedert V16.43: Physical address acknowledge wordt verstuurd vanuit de PNP engine.					

					// Enkel een event naar de FSM versturen wnr de node toegevoegd werd.
					// Sedert V16.31: PhysicalAddress doorgeven aan de pnp fsm.
					pFsm->Fire_EvPNPPhysicalAddress(pNode->ulPhysicalAddress);						
				}						
			}
		}
	}
}

void CComApplication::MessageHandler_EV_NODEGATEWAYSTATUS()
{
	const ICommunicationFrame* const pModemFrame = m_pCommunicationObject->GetRxModemFrame();
	ASSERT(pModemFrame->GetMsgCode() == EV_NODEGATEWAYSTATUS);

	CCanNode *pNode;
	int iEntryNR;
	const BYTE bNodeAddress = pModemFrame->GetNodeAddress();

	if (TMGetRuntimeNodes()->FindNode(bNodeAddress,&iEntryNR,&pNode)) 
	{
		ASSERT(pNode->GetRuntimeStatus() != 0);
		ASSERT(pNode->bNodeType == NODE_PIN_GATEWAY);

		switch (pModemFrame->GetDataBYTE(0))
		{					
			case NODEGATEWAY_ATTR_STATE:
			{					
				if (pNode->GetRuntimeStatus() != 0 && 
					pNode->bNodeType == NODE_PIN_GATEWAY)
				{
					static_cast <CNodeGatewayRuntimeStatus*> (pNode->GetRuntimeStatus())
						->SetState(CNodeGatewayRuntimeStatus::Convert(pModemFrame->GetDataBYTE(1)));

					Fire_OnNodeStatusChanged(bNodeAddress);
				}
			} break;

			default:
			case NODEGATEWAY_OBJECT_DATABASE_ERASE:
			{
				// Absorbe ...
			} break;
		}				
	}						
}

// Versie 0x0700: Optimalisatie.
void CComApplication::MessageHandler_EV_PNPLOGICALADDRESSASSIGNED()
{	
	const ICommunicationFrame* const pModemFrame = m_pCommunicationObject->GetRxModemFrame();
	const BYTE bNodeAddress = pModemFrame->GetNodeAddress();
	ASSERT(pModemFrame->GetMsgCode() == EV_PNPLOGICALADDRESSASSIGNED);

	if (NULL != this->m_pControl) {
		this->m_pControl->SendMessage(WM_APPLICATION_NOTIFY_ASSIGNED_LA,bNodeAddress);
	}
	CBrowsePnPEngine* const pFsm = m_pFactory->Get_BrowsePnpEngine();
	if (pFsm != NULL) {				
		if (pFsm->IsBusyPnp()) {
			CCanNode *pNode;
			int iEntryNR;					
			if (TMGetRuntimeNodes()->FindNode(bNodeAddress,&iEntryNR,&pNode)) {
				pNode->bNodeType = pModemFrame->GetDataBYTE(0);
				pNode->bNROfUnits = pModemFrame->GetDataBYTE(1); 
				pNode->bNodeFlags = pModemFrame->GetDataBYTE(2);
				pNode->bDLLAccessControl = pModemFrame->GetDataBYTE(3);				
			}								
			pFsm->Fire_EvPNPAddressAssigned();									
		}
	}	
}

void CComApplication::MessageHandler_EV_FTPFILECONTROLDATA( )
{
	const ICommunicationFrame* const pModemFrame = m_pCommunicationObject->GetRxModemFrame();
	ASSERT(pModemFrame->GetMsgCode() == EV_FTPFILECONTROLDATA);
	if (m_pFactory->Get_ConfigCanFTP() != 0)
	{
		const BYTE bNodeAddress = pModemFrame->GetNodeAddress();
		BYTE bLength = pModemFrame->GetMessageLength();
					
		if (bLength > SEGMENTED_BLOCK_LENGTH) 					
			bLength = SEGMENTED_BLOCK_LENGTH;
					
		BYTE bSegmentedData[SEGMENTED_BLOCK_LENGTH];	
		int t=0,j=0;
		for (int i=0;i<bLength;i++) 
		{
			t = pModemFrame->GetDataBYTE(i);
			bSegmentedData[i] = t;

			if (t == 0xFF) j++;						
		}								
		m_pFactory->Get_ConfigCanFTP()->signalDataReceived(bNodeAddress,bSegmentedData,bLength);
	}
}

void CComApplication::MessageHandler_EV_FTPFILECONTROLSTATUS( )		
{
	const ICommunicationFrame* const pModemFrame = m_pCommunicationObject->GetRxModemFrame();
	ASSERT(pModemFrame->GetMsgCode() == EV_FTPFILECONTROLSTATUS);	
	if (m_pFactory->Get_ConfigCanFTP() != 0)
	{
		const BYTE bNodeAddress = pModemFrame->GetNodeAddress();
		const BYTE bMethod = pModemFrame->GetDataBYTE(0);
		unsigned long ulFileSize = 0;
		
		if (pModemFrame->GetMessageLength() >= 5)
		{
			ulFileSize =  ((unsigned long)(pModemFrame->GetDataBYTE(1))<< 24) +
						  ((unsigned long)(pModemFrame->GetDataBYTE(2))<< 16) +
						  ((unsigned long)(pModemFrame->GetDataBYTE(3))<< 8) +
						  ((unsigned long)(pModemFrame->GetDataBYTE(4)));
		}
		switch (bMethod)
		{			
			case FTP_OBJECT_OPEN_WRITE:
			{
				m_pFactory->Get_ConfigCanFTP()->signalWriteOpen(bNodeAddress,
																ulFileSize);
			} break;
			case FTP_OBJECT_OPEN_READ:
			{
				m_pFactory->Get_ConfigCanFTP()->signalReadOpen(bNodeAddress,
															   ulFileSize);	
			} break;
			case FTP_OBJECT_APPEND_DATA:
			{
				m_pFactory->Get_ConfigCanFTP()->signalDataAppended(bNodeAddress);																  	
			} break;
			case FTP_OBJECT_CLOSE:
			{
				m_pFactory->Get_ConfigCanFTP()->signalFileClosed(bNodeAddress,
																 ulFileSize);
			} break;
			case FTP_OBJECT_ABORT:
			{
				m_pFactory->Get_ConfigCanFTP()->signalFileAborted(bNodeAddress,
																  ulFileSize);	
			} break;
			case FTP_OBJECT_EC_WRONG_STATE:
			{
				m_pFactory->Get_ConfigCanFTP()->signalError(
					bNodeAddress,	
					FTP_ERROR_WRONG_STATE);
			} break;
			case FTP_OBJECT_EC_WRITE_DATA:
			{
				m_pFactory->Get_ConfigCanFTP()->signalError(
					bNodeAddress,	
					FTP_ERROR_WRITE_DATA);
			} break;
			case FTP_OBJECT_EC_READ_DATA:
			{
				m_pFactory->Get_ConfigCanFTP()->signalError(
					bNodeAddress,	
					FTP_ERROR_READ_DATA);	
			} break;
			case FTP_OBJECT_EC_OPEN_FILE:
			{
				m_pFactory->Get_ConfigCanFTP()->signalError(
					bNodeAddress,	
					FTP_ERROR_FILE_OPEN);	
			} break;
			case FTP_OBJECT_EC_CLOSE_FILE:
			{
				m_pFactory->Get_ConfigCanFTP()->signalError(
					bNodeAddress,	
					FTP_ERROR_FILE_CLOSE);	
			} break;
			case FTP_OBJECT_EC_GENERAL:
			{
				m_pFactory->Get_ConfigCanFTP()->signalError(
					bNodeAddress,	
					FTP_ERROR_GENERAL);	
			} break;
			case FTP_OBJECT_EC_GET_INFO:
			{
				m_pFactory->Get_ConfigCanFTP()->signalError(
					bNodeAddress,	
					FTP_ERROR_GET_INFO);		
			} break;
			case FTP_OBJECT_EC_NOT_IMPLEMENTED:
			{
				m_pFactory->Get_ConfigCanFTP()->signalError(
					bNodeAddress,	
					FTP_ERROR_NOT_IMPLEMENTED);	
			} break;
			default:
			{	
				ASSERT( 0 );
			} break;
		}
	}
}

void CComApplication::ParseNodeObjectRxMessage(void) {
	if (m_pThis != 0) {
		const BYTE bMessageCode = m_pCommunicationObject->GetRxModemFrame()->GetMsgCode();		
		if (m_pThis->MessageHandlers[bMessageCode].action != 0) {
			(m_pThis->*MessageHandlers[bMessageCode].action)();
		}
	}
}


///////////////////////////////////////////////////////////////////////
///					BROWSE + PNP INTERFACES !!!						///
///////////////////////////////////////////////////////////////////////

void CComApplication::NotifyBrowseOk(void) 
{	
	ASSERT(m_pFactory->Get_BrowsePnpEngine());

	m_pControl->PostMessage(WM_BROWSEPNP_BROWSE_OK,0,0);

	// Destroy !!!
	PostMessage( WM_CLASSFACTORY_DESTROY,
				 CLASSFACTORY_ID_BROWSEPNP_ENGINE );
}

void CComApplication::NotifyPnpOk(void) 
{
	ASSERT(m_pFactory->Get_BrowsePnpEngine());

	m_pControl->PostMessage(WM_BROWSEPNP_PNP_OK,0,0);

	// Destroy !!!
	PostMessage( WM_CLASSFACTORY_DESTROY,
				 CLASSFACTORY_ID_BROWSEPNP_ENGINE );
}

void CComApplication::NotifyBrowsePnpError(void) 
{
	ASSERT(m_pFactory->Get_BrowsePnpEngine());

	m_pControl->PostMessage(WM_BROWSEPNP_ERROR,0,0);

	// Destroy !!!
	PostMessage( WM_CLASSFACTORY_DESTROY,
				 CLASSFACTORY_ID_BROWSEPNP_ENGINE );
}

void CComApplication::NotifyBrowsePnpCancel(void) 
{
	ASSERT(m_pFactory->Get_BrowsePnpEngine());

	m_pControl->PostMessage(WM_BROWSEPNP_CANCEL,0,0);

	// Destroy !!!
	PostMessage( WM_CLASSFACTORY_DESTROY,
				 CLASSFACTORY_ID_BROWSEPNP_ENGINE );
}

void CComApplication::NotifyPnpDuplicatesFound(void)
{
	ASSERT(m_pFactory->Get_BrowsePnpEngine());

	m_pControl->PostMessage(WM_BROWSEPNP_DUPLICATES,0,0);

	// Destroy !!!
	PostMessage( WM_CLASSFACTORY_DESTROY,
				 CLASSFACTORY_ID_BROWSEPNP_ENGINE );
}

void CComApplication::NotifyPnpFactoryAddressFound(void) 
{
	ASSERT(m_pFactory->Get_BrowsePnpEngine());

	m_pControl->PostMessage(WM_BROWSEPNP_FACTORYADDRESS,0,0);

	// Destroy !!!
	PostMessage( WM_CLASSFACTORY_DESTROY,
				 CLASSFACTORY_ID_BROWSEPNP_ENGINE );
}

void CComApplication::NotifyBrowsePnpTimeout(void) 
{
	ASSERT(m_pFactory->Get_BrowsePnpEngine());

	m_pControl->PostMessage(WM_BROWSEPNP_NORESPONSE,0,0);

	// Destroy !!!
	PostMessage( WM_CLASSFACTORY_DESTROY,
				 CLASSFACTORY_ID_BROWSEPNP_ENGINE );
}

void CComApplication::BrowseSingleNode(BYTE bNodeAddress) {
	if (IsConnected() == TRUE)  {
		ASSERT(m_pFactory->Get_BrowsePnpEngine() == NULL);
		if (m_pFactory->Get_BrowsePnpEngine() == NULL) {
			if (m_pFactory->Create_BrowsePnpEngine() != NULL) {
				m_pFactory->Get_BrowsePnpEngine()->BrowseSingleNode(bNodeAddress);
			}
		}
		ASSERT(m_pFactory->Get_BrowsePnpEngine() != NULL);
	}
}

/* 
	BE CAREFUL WHEN USING 'BROWSEALLNODES' : 
	--> Browse all Nodes doesn't retrieve info of Physical addresses,
	    Auto-pnp stores nodes via physical addresses,
		when combining both duplicate addresses will be found !!
*/

void CComApplication::BrowseAllNodes(void) {	
	if (IsConnected() == TRUE) {
		ASSERT(m_pFactory->Get_BrowsePnpEngine() == NULL);	
		if (m_pFactory->Get_BrowsePnpEngine() == NULL) {
			if (m_pFactory->Create_BrowsePnpEngine() != NULL) {
				TMGetRuntimeNodes()->ClearDatabase();
				TMGetRuntimeNodes()->InitDatabase();			
				m_pFactory->Get_BrowsePnpEngine()->BrowseForNodes();
			}
		}
		ASSERT(m_pFactory->Get_BrowsePnpEngine() != NULL);
	}
}

void CComApplication::BrowseAllPhysicalNodes(void) {		
	if (IsConnected() == TRUE)  {
		ASSERT(m_pFactory->Get_BrowsePnpEngine() == NULL);	
		if (m_pFactory->Get_BrowsePnpEngine() == NULL) {
			if (m_pFactory->Create_BrowsePnpEngine() != NULL) {
				TMGetRuntimeNodes()->ClearDatabase();
				TMGetRuntimeNodes()->InitDatabase();										
				m_pFactory->Get_BrowsePnpEngine()->BrowseForPhysicalNodes();
			}
		}
		ASSERT(m_pFactory->Get_BrowsePnpEngine() != NULL);
	}	
}

void CComApplication::StartPnP(void) {
	if (IsConnected() == TRUE) {
		ASSERT(m_pFactory->Get_BrowsePnpEngine() == NULL);	
		if (m_pFactory->Get_BrowsePnpEngine() == NULL) {
			if (m_pFactory->Create_BrowsePnpEngine() != NULL) {
				TMGetRuntimeNodes()->ClearDatabase();
				TMGetRuntimeNodes()->InitDatabase();					
				m_pFactory->Get_BrowsePnpEngine()->AutoPnp();
			}
		}
		ASSERT(m_pFactory->Get_BrowsePnpEngine() != NULL);
	}
}

void CComApplication::CancelBrowsePnp(void) {
	ASSERT(m_pFactory->Get_BrowsePnpEngine() != NULL);
	if (m_pFactory->Get_BrowsePnpEngine() != NULL){
		m_pFactory->Get_BrowsePnpEngine()->UserCancel();
	}
}

/** ******************************************************************************************
 * Actions : RenameNodeUnitsEngine()
 */
void CComApplication::RenameUnit(BYTE bNodeAddress,BYTE bUnitAddress)
{	
	if ((IsConnected()) && 
		(bUnitAddress != BROADCAST_UNIT)) 
	{
		if (m_pFactory->Get_RenameNodeUnitsEngine() == NULL) 
		{		
			if (m_pFactory->Create_RenameNodeUnitsEngine() != NULL)
			{							
				m_pFactory->Get_RenameNodeUnitsEngine()
					->SendUnitName(bNodeAddress,bUnitAddress);
			}
		
			ASSERT(m_pFactory->Get_RenameNodeUnitsEngine());
		}
		else 
		{
			CString szMessage("Rename Unit/Nodes FSM busy\nCaused by node,unit not responding");
			AfxMessageBox(szMessage,MB_ICONSTOP);
		}
	}
}

void CComApplication::RenameNode(BYTE bNodeAddress)
{	
	if (IsConnected()) 
	{
		if (m_pFactory->Get_RenameNodeUnitsEngine() == NULL) 
		{		
			if (m_pFactory->Create_RenameNodeUnitsEngine() != NULL)
			{							
				m_pFactory->Get_RenameNodeUnitsEngine()
					->SendNodeName(bNodeAddress);
			}
		
			ASSERT(m_pFactory->Get_RenameNodeUnitsEngine());			
		}
		else
		{
			CString szMessage("Rename Unit/Nodes FSM busy\nCaused by node,unit not responding");
			AfxMessageBox(szMessage);
		}
	}
}
void CComApplication::Start_SendAllNames(void)
{	
	if (IsConnected()) 
	{
		if (m_pFactory->Get_RenameNodeUnitsEngine() == NULL) 
		{		
			if (m_pFactory->Create_RenameNodeUnitsEngine() != NULL)
			{		
				m_pFactory->Get_RenameNodeUnitsEngine()
					->SendAllNames();	
			}
								
			ASSERT(m_pFactory->Get_RenameNodeUnitsEngine());				
		}
		else 
		{
			AfxMessageBox("FATAL ERROR : Rename Node/Units FSM Busy",MB_ICONSTOP);
		}
	}
}

void CComApplication::Cancel_SendAllNames(void) 
{
	ASSERT(m_pFactory->Get_RenameNodeUnitsEngine());

	if (m_pFactory->Get_RenameNodeUnitsEngine() != NULL) 
	{
		m_pFactory->Get_RenameNodeUnitsEngine()
			->Fire_EvUserCancel();	
	}
}
