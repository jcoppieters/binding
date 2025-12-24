#include "stdafx.h"
#include "TMAlgoritmesCfg.h"

#include "app\appCfg\AppCfginfo\cfginfonode.h"	

#include "App\AppCfg\AppCfgMoods\ConfigMoodInfo.h"
#include "App\AppCfg\AppCfgUnits\ConfigUnitCreateInfo.h"
#include "app\AppCfg\AppCfgGatewayU\configgatewayuinfo.h"
#include "App\AppCfg\AppCfgIRRX\ConfigIRRXInfo.h"
#include "App\AppCfg\AppCfgschedule\ConfigScheduleInfo.h"
#include "App\AppCfg\AppCfgCanFtp\ConfigCANFTP.h"
#include "App\AppCfg\AppCfgCanFtp\ConfigCanFtp_Info.h"
#include "App\AppCfg\AppCfgIRTX\ConfigIRTX.h"	
#include "App\AppCfg\AppCfgsmap\configsmapvroominfo.h"	
#include "App\AppCfg\AppCfgsmap\configsmappageinfoinfo.h"
#include "App\AppCfg\AppCfgsmap\configsmapmenuinfo.h"
#include "App\AppCfg\AppCfgBitmaps\ConfigBitmaps.h"	
#include "app\AppCfg\AppCfgRoomCtrlAV\configRoomCtrlAVInfo.h"
#include "app\AppCfg\AppCfgBoseST\configBoseSTCfgInfo.h"
#include "app\AppCfg\AppCfgUiCtrl\configAppUiCtrlInfo.h"
#include "app\appcfg\appcfgsonossrc\configsonossrcitems.h"
#include "app/appcfg/appcfgsmaplite/appcfgsmaplitefsmmaster.h"		// since 0x0C05
#include "app/appcfg/appcfgsmaplite/appcfgsmapliteinfo.h"			// since 0x0C05
#include "app/appcfg/appcfgavsettings/configavsettings.h"			// since 0x0E05

#include "utils/mfc/ArrayErrors.h"
#include "utils/win32/ShellFunctions.h"	
#include "utils/win32/win32filefind.h"

#include "nodelcdscreenmapfsminfo.h"	
#include "nodescreenmapinfo.h"	// Smap lite versie nodig.

/////////////////////////////////////////////////////////////////////////////
#include "TMGlobals.h"
using namespace TMGlobalHSM;
using namespace TMNodeDatabase;
using namespace TMGlobalSettings;
using namespace TMCommunication;
/////////////////////////////////////////////////////////////////////////////

namespace TMAlgoritmesCfg
{
	CNotifierDefault g_defaultNotifier;

	template <class ERROR_FILE>
		void reportErrors(INotifier& refNotifier,ERROR_FILE& configFile)
	{
		const int errors = configFile.GetNrErrors();
		if (errors != 0) 
		{										
			for (int i=0;i<errors;i++) 
			{				
				refNotifier.AddMessage(configFile.GetErrors(i));												
			}				
		}
	}
	
	void reportErrors(INotifier& refNotifier,DUOTECNO::MFC_HELPER::CArrayErrors& arrErrors)
	{
		const int errors = arrErrors.GetNrErrors( );	
		if (errors != 0) 
		{										
			for (int i=0;i<errors;i++) 
			{
				const char* szString = 0;

				if (arrErrors.GetErrors(i,&szString))
				{	
					refNotifier.AddMessage(szString);							
				}
			}				
		}
	}

	// Helper...
	BOOL helper_addSystem( 
		CConfigCanFTP_Info* pInfo,
		CNodeSystemLog_Util::FileID fileID,
		CCanNode* pNode,
		BOOL fSuppressError=FALSE)
	{
		BOOL fResult=FALSE;

		if (CNodeSystemLog_Util::isSupported(pNode,fileID))
		{
			CConfigCanFTP_FileEntry* const pEntry = pInfo->create( );

			if (pEntry)
			{
				enum { MAX_FILENAME_LENGTH = 255 };
				char szFileNameLocal[ MAX_FILENAME_LENGTH ];
				char szFileNameRemote[ MAX_FILENAME_LENGTH ];

				if (CNodeSystemLog_Util::getFileNameRemote(
						pNode,
						fileID,
						szFileNameRemote,
						MAX_FILENAME_LENGTH))
				{
					if (CNodeSystemLog_Util::getFileNameLocal(
						pNode,
						fileID,
						szFileNameLocal,
						MAX_FILENAME_LENGTH))
					{
						// Sedert versie 0x0B01
						CString szFullFileNameLocal;

						szFullFileNameLocal.Format("%s\\%s",
									TMGetFileSettings()->GetFullyQualifiedPathName(),
									szFileNameLocal);

						pEntry->setNodeAddress(pNode->getNodeAddress());
						pEntry->setLocalFileName(szFullFileNameLocal); // szFileNameLocal);
						pEntry->setRemoteFileName(szFileNameRemote);
						if (fileID==CNodeSystemLog_Util::FILE_ID_SYSTEMLOG)
						{
							pEntry->setInfo("System log download");
						}
						else
						{
							pEntry->setInfo("System log download backup");
						}
						pEntry->setAction(CConfigCanFTP_FileEntry::ACTION_FTP_READ);
						pEntry->setCallbackStarted(0);
						pEntry->setCallbackDone(0);
						
						if(TRUE==fSuppressError)
						{
							// Sedert versie 0x0B01:
							// Bestand is niet altijd aanwezig, geen foutmelding geven.
							pEntry->setCtxFlags(
								CConfigCanFTP_FileEntry::CTXFLAG_READ_FILENOTFOUND_SUPPRESS_ERROR);
						}

						pInfo->addTask(pEntry);	
						fResult=TRUE;
					}			
				}
			}
		}
		return fResult;
	}


	// TM, v11.01.
	// Sedert versie V11.01.
	// IMPLEMENTATIE IS NIET OK - Algoritme stopt van zodra er een node aanwezig is 
	// waarvan er een syslog bestand ontbreekt.
	void TMDownloadAllSystemLog(
		CNodeDatabase& refDocDatabase,
		INotifier& refNotifier)
	{
		BOOL fContinue=FALSE;
		CConfigCanFTP_Info& configCanFTP_Info = CConfigCanFTP_Info::getInstance();
		configCanFTP_Info.clear( );
		
		if (1)
		{
			for(CNodeIterator it(refDocDatabase.CreateNodeIterator());
				!it.IsDone();
				it.Next())
			{
				CCanNode* const pNode=it.CurrentItem();

				if(0!=pNode)
				{
					if (helper_addSystem(&configCanFTP_Info,
									 CNodeSystemLog_Util::FILE_ID_SYSTEMLOG,
									 pNode,
									 FALSE))
					{
						fContinue=TRUE;
					}

					// Oppassen: 
					// fSuppressError == TRUE want de FSM mag niet stoppen als er bepaalde 
					// logbestanden niet aanwezig zijn.

					if (helper_addSystem(&configCanFTP_Info,
									 CNodeSystemLog_Util::FILE_ID_SYSTEMLOG_BACKUP,
									 pNode,
									 TRUE))
					{
						fContinue=TRUE;
					}
				}
			}
		}

		if(TRUE==fContinue)
		{
			refNotifier.AddMessage("Start Downloading");
			TMGetGlobalHSM()->EvOperationFtpConfig( );		
		}
	}

	// TM,0x0B01: Lokale bestandsnaam moet volledig pad zijn.
	void TMDownloadSystemLog(
		CCanNode* pNode,
		INotifier& refNotifier)
	{
		CConfigCanFTP_Info& configCanFTP_Info = CConfigCanFTP_Info::getInstance();
		configCanFTP_Info.clear( );
		
		if (1)
		{
			if (CNodeSystemLog_Util::isSupported(
					pNode,
					CNodeSystemLog_Util::FILE_ID_SYSTEMLOG))
			{
				CConfigCanFTP_FileEntry* const pEntry = configCanFTP_Info.create( );

				if (pEntry)
				{
					enum { MAX_FILENAME_LENGTH = 255 };
					char szFileNameLocal[ MAX_FILENAME_LENGTH ];
					char szFileNameRemote[ MAX_FILENAME_LENGTH ];

					if (CNodeSystemLog_Util::getFileNameRemote(
							pNode,
							CNodeSystemLog_Util::FILE_ID_SYSTEMLOG,
							szFileNameRemote,
							MAX_FILENAME_LENGTH))
					{
						if (CNodeSystemLog_Util::getFileNameLocal(
							pNode,
							CNodeSystemLog_Util::FILE_ID_SYSTEMLOG,
							szFileNameLocal,
							MAX_FILENAME_LENGTH))
						{
							// Sedert versie 0x0B01
							CString szFullFileNameLocal;

							szFullFileNameLocal.Format("%s\\%s",
										TMGetFileSettings()->GetFullyQualifiedPathName(),
										szFileNameLocal);

							pEntry->setNodeAddress(pNode->getNodeAddress());
							pEntry->setLocalFileName(szFullFileNameLocal); // szFileNameLocal);
							pEntry->setRemoteFileName(szFileNameRemote);
							pEntry->setInfo("System log download");
							pEntry->setAction(CConfigCanFTP_FileEntry::ACTION_FTP_READ);
							pEntry->setCallbackStarted(0);
							pEntry->setCallbackDone(0);
							configCanFTP_Info.addTask(pEntry);												
						}			
					}
				}
			}
		}

		if (1)
		{
			if (CNodeSystemLog_Util::isSupported(
					pNode,
					CNodeSystemLog_Util::FILE_ID_SYSTEMLOG_BACKUP))
			{
				CConfigCanFTP_FileEntry* const pEntry = configCanFTP_Info.create( );

				if (pEntry)
				{
					enum { MAX_FILENAME_LENGTH = 255 };
					char szFileNameLocal[ MAX_FILENAME_LENGTH ];
					char szFileNameRemote[ MAX_FILENAME_LENGTH ];

					if (CNodeSystemLog_Util::getFileNameRemote(
							pNode,
							CNodeSystemLog_Util::FILE_ID_SYSTEMLOG_BACKUP,
							szFileNameRemote,
							MAX_FILENAME_LENGTH))
					{
						if (CNodeSystemLog_Util::getFileNameLocal(
							pNode,
							CNodeSystemLog_Util::FILE_ID_SYSTEMLOG_BACKUP,
							szFileNameLocal,
							MAX_FILENAME_LENGTH))
						{
							// Sedert versie 0x0B01
							CString szFullFileNameLocal;

							szFullFileNameLocal.Format("%s\\%s",
										TMGetFileSettings()->GetFullyQualifiedPathName(),
										szFileNameLocal);

							pEntry->setNodeAddress(pNode->getNodeAddress());
							pEntry->setLocalFileName(szFullFileNameLocal); // szFileNameLocal);
							pEntry->setRemoteFileName(szFileNameRemote);
							pEntry->setInfo("System log backup download");
							pEntry->setAction(CConfigCanFTP_FileEntry::ACTION_FTP_READ);
							pEntry->setCallbackStarted(0);
							pEntry->setCallbackDone(0);

							// Sedert versie 0x0B01:
							// Bestand is niet altijd aanwezig, geen foutmelding geven.
							pEntry->setCtxFlags(CConfigCanFTP_FileEntry::CTXFLAG_READ_FILENOTFOUND_SUPPRESS_ERROR);

							configCanFTP_Info.addTask(pEntry);												
						}			
					}
				}
			}
		}

		refNotifier.AddMessage("Start Downloading");
	
		TMGetGlobalHSM()->EvOperationFtpConfig( );			
	}	

	void TMUploadBoseSTCfg(
		CCanNode* pNode,
		const char* szFileDescription,
		INotifier& refNotifier)
	{
		CString szMessage;
		CConfigBoseSTCfgInfo& configInfo =
			CConfigBoseSTCfgInfo::getInstance( );

		if (configInfo.load(pNode->getNodeAddress( )))
		{			
			ASSERT( configInfo.isLoaded( ));

			szMessage.Format("%s file load done",szFileDescription);

			refNotifier.AddMessage(szMessage);		
			TMGetGlobalHSM()->EvOperationUploadBoseSTCfg();

			szMessage.Format("%s file send",szFileDescription);
			refNotifier.AddMessage(szMessage);			
		}
		else
		{
			szMessage.Format("%s file load failed",szFileDescription);
			refNotifier.AddMessage(szMessage);

			reportErrors(refNotifier, configInfo.getConfigFile( ) );						
		}			
	}

	void TMUploadGatewayCfg(
		CCanNode* pNode,
		INotifier& refNotifier)
	{
		CConfigGatewayUInfo& configInfo =
			CConfigGatewayUInfo::getInstance( );

		if (configInfo.load(pNode->getNodeAddress( )))
		{
			ASSERT( configInfo.isLoaded( ));

			refNotifier.AddMessage("gateway config file load done");		
			TMGetGlobalHSM()->EvOperationUploadGatewayCfg();

			refNotifier.AddMessage("gateway config file send...");			
		}
		else
		{
			refNotifier.AddMessage("gateway config file load failed");
			reportErrors(refNotifier, configInfo.getConfigFile( ) );						
		}			
	}

	void TMUploadRoomCtrlAudioCfg(
		CCanNode* pNode,
		INotifier& refNotifier)
	{
		CConfigRoomCtrlAVInfo& configInfo =
			CConfigRoomCtrlAVInfo::getInstance( );

		if (configInfo.load(pNode->getNodeAddress( )))
		{
			ASSERT( configInfo.isLoaded( ));

			refNotifier.AddMessage("Roomcontroller audio config file load done");		
			TMGetGlobalHSM()->EvOperationUploadRoomCtrlAudioCfg();

			refNotifier.AddMessage("Roomcontroller audio config file send...");			
		}
		else
		{
			refNotifier.AddMessage("Roomcontroller audio config file load failed");
			reportErrors(refNotifier, configInfo.getConfigFile( ) );						
		}				
	}

	// Sedert V16.40.
	// Data validator meegeven voor het controleren van de unit namen.
	struct CConfigUnitCreateDataValidatorAlg : public ICConfigUnitCreateDataValidator {
		public:
			virtual ~CConfigUnitCreateDataValidatorAlg( ) { }

			// Zoeken naar een unit naam in node database.
			// Wanneer aanwezig return waarde TRUE, Anders FALSE.
			virtual BOOL getName(int nodeAddress, int unitAddress, CString* pStringName) {	
				REQUIRE(nodeAddress >= 0 && nodeAddress <= 0xff);
				REQUIRE(unitAddress >= 0 && unitAddress <= 0xff);
				if (NULL == pStringName) {
					REQUIRE(NULL != pStringName);
					return FALSE;
				}
				CNodeDatabase* const pNodeDatabase = TMGetDocumentNodes();
				if (NULL == pNodeDatabase) {
					ASSERT(NULL != pNodeDatabase);
					return FALSE;
				}
				const CString str(pNodeDatabase->GetUnitName(nodeAddress,unitAddress));
				if (str.IsEmpty()) {
					return FALSE;
				}			
				*pStringName = str;
				return TRUE;		
			}
	};


	// Sedert V16.40.
	// Data validator meegeven voor het controleren van de unit namen.
	void TMUploadUnitCreate(CCanNode* pNode, INotifier& refNotifier) {
		static CConfigUnitCreateDataValidatorAlg dataValidator;
		CConfigUnitCreateInfo& configInfo = CConfigUnitCreateInfo::getInstance( );
		if (configInfo.load(pNode->getNodeAddress( ),&dataValidator)) {		
			ASSERT( configInfo.isLoaded() );
			refNotifier.AddMessage("Unit config file load done");			
			TMGetGlobalHSM()->EvOperationUploadUnitConfig();
			refNotifier.AddMessage("unit config file send...");			
		}
		else {
			refNotifier.AddMessage("Unit config file load failed");			
			reportErrors(refNotifier, configInfo.getConfigFile( ));			
		}					
	}

	// sedert V16.47:CHANGES_UPLOAD_UNITS
	// Deze functie is nodig voor het automatich herstellen/doorsturen van de units bij het vervangen van een 
	// module.
	// In deze functie wordt het unit configuratie bestand ingelezen ter voorbereiding van het doorsturen.
	//
	// Deze functie wordt aangeroepen door CQHSMTaskBuilder::Build_UploadUnitsCreateConfigReplaceNode
	BOOL TMLoadFileUnitCreate(int nodeAddress, INotifier& refNotifier)  {
		static CConfigUnitCreateDataValidatorAlg dataValidator;
		CConfigUnitCreateInfo& configInfo = CConfigUnitCreateInfo::getInstance( );
		if (FALSE == configInfo.load(nodeAddress,&dataValidator)) {		
			return FALSE;
		}
		return TRUE;		
	}

	void TMUploadMoods(
		CCanNode* pNode,
		INotifier& refNotifier)
	{
		CConfigMoodInfo& configInfo = CConfigMoodInfo::getInstance( );

		if (configInfo.load(pNode->getNodeAddress( )))
		{
			ASSERT( configInfo.isLoaded() );

			refNotifier.AddMessage("Mood config file load done");			
			TMGetGlobalHSM()->EvOperationUploadMoodConfig();

			refNotifier.AddMessage("mood config file send...");			
		}
		else
		{
			refNotifier.AddMessage("Mood config file load failed");
			reportErrors(refNotifier, configInfo.getConfigFile( ) );			
		}			
	}

	void TMUploadIRRX(
		CCanNode* pNode,
		INotifier& refNotifier)
	{
		CConfigIRRXInfo& configInfo =
			CConfigIRRXInfo::getInstance( );

		if (configInfo.load(pNode->getNodeAddress( )))
		{
			ASSERT( configInfo.isLoaded( ));

			refNotifier.AddMessage("ir rx config file load done");			
			TMGetGlobalHSM()->EvOperationUploadIRRXConfig();

			refNotifier.AddMessage("ir rx config file send...");			
		}
		else
		{
			refNotifier.AddMessage("ir rx config file load failed");
			reportErrors(refNotifier, configInfo.getConfigFile( ) );			
		}			
	}

	void TMUploadScheduling(
		CCanNode* pNode,
		INotifier& refNotifier)
	{
		CConfigScheduleInfo& configInfo =
			CConfigScheduleInfo::getInstance( );

		if (configInfo.load(pNode->getNodeAddress( )))
		{
			ASSERT( configInfo.isLoaded( ) );

			refNotifier.AddMessage("schedule config file load done");		
			TMGetGlobalHSM()->EvOperationUploadScheduleConfig();

			refNotifier.AddMessage("schedule config file send...");			
		}
		else
		{
			refNotifier.AddMessage("schedule config file load failed");
			reportErrors(refNotifier, configInfo.getConfigFile( ) );			
		}		
	}

	void TMUploadIRTX(CCanNode* pNode,INotifier& refNotifier)
	{
		CConfigCanFTP_Info& configCanFTP_Info = CConfigCanFTP_Info::getInstance();
		configCanFTP_Info.clear( );		
		CConfigCanFTP_FileEntry* const pEntry = configCanFTP_Info.create( );

		if (pEntry)
		{
			enum { MAX_FILENAME_LENGTH = 255 };
			char szFileNameLocal[ MAX_FILENAME_LENGTH ];
			char szFileNameRemote[ MAX_FILENAME_LENGTH ];

			if (CNodeIRTXConfig_Util::getFileNameLocal(pNode,szFileNameLocal,MAX_FILENAME_LENGTH))
			{
				if (CNodeIRTXConfig_Util::getFileNameRemote(pNode,szFileNameRemote,MAX_FILENAME_LENGTH))
				{
					CString szFilename;
					szFilename.Format("%s%s",TMGetFileSettings()->GetFullyQualifiedPathName(),szFileNameLocal);

					// Enkel als het een geldig bestand is.
					if (FindFile(szFilename))
					{
						// De CConfigIRTX::onFTPDone callback functie stuurt een
						// bericht naar de node dat hij zijn database moet herladen
						// en een flush IRTX bericht versturen naar alle IRTX units.

						pEntry->setNodeAddress( pNode->getNodeAddress() );
						pEntry->setLocalFileName(szFilename);
						pEntry->setRemoteFileName(szFileNameRemote);
						pEntry->setInfo("IRTX configuration");
						pEntry->setAction(CConfigCanFTP_FileEntry::ACTION_FTP_WRITE);
						pEntry->setCallbackStarted(&CConfigIRTX::onFTPStarted);
						pEntry->setCallbackDone(&CConfigIRTX::onFTPDone);
						configCanFTP_Info.addTask(pEntry);
					
						refNotifier.AddMessage("IRTX Configuration file send...");						
						TMGetGlobalHSM()->EvOperationFtpConfig( );	
					}
					else
					{
						refNotifier.AddMessage("IRTX Configuration file not found");							
					}
				}
			}
		}	
	}

	void TMUploadSmapAVRoom(
		CCanNode* pNode,
		INotifier& refNotifier)
	{
		CConfigSmapAVRoomInfo& configInfo =
			CConfigSmapAVRoomInfo::getInstance( );

		if (configInfo.load(pNode->getNodeAddress()))
		{
			ASSERT( configInfo.isLoaded( ) );

			refNotifier.AddMessage("AV-Room config file load done");			
			TMGetGlobalHSM()->EvOperationUploadSmapAVRoom();

			refNotifier.AddMessage("AV-Room config file send...");			
		}
		else
		{
			refNotifier.AddMessage("AV-Room config file load failed");
			reportErrors(refNotifier, configInfo.getConfigFile( ) );			
		}		
	}


#if(USE_SMAP_ENERGYMEASURE == 1)

	// Aanpassingen V16.21: Screenmapping van de virtuele energie meet units.
	// Gebruik van een CNodeLCDScreenmapFSMInfo_Util klasse.
	void TMUploadSmapMyHome(CCanNode* pNode,INotifier& refNotifier) {
		CConfigSmapPageInfoInfo& ref = CConfigSmapPageInfoInfo::getInstance( );
		if (ref.load( pNode->getNodeAddress())) {									
			CNodeLCDScreenmapFSMInfo* const pRefScreenmapsUpdateInfo = CNodeLCDScreenmapFSMInfo::GetInstance();		
			if (pRefScreenmapsUpdateInfo && pRefScreenmapsUpdateInfo->IsLocked() == FALSE) {			

				pRefScreenmapsUpdateInfo->Clear();

				CString strError;
				if (!CNodeLCDScreenmapFSMInfo_Util::addNode(pRefScreenmapsUpdateInfo, pNode, CNodeLCDScreenmapFSMInfo::TYPE_SCREENMAP_UPDATE_MYHOME, &strError)) {
					AfxMessageBox(strError);
					return;
				}	
																		
				TMGetGlobalHSM()->EvOperationUploadSmapMyHome( );
				refNotifier.AddMessage("screenmapping file send...");												
			}			
		}
		else {
			refNotifier.AddMessage("Screenmapping config file not found");				
		}
	}

	// Aanpassingen V16.21: Screenmapping van de virtuele energie meet units.
	// Gebruik van een CNodeLCDScreenmapFSMInfo_Util klasse.
	void TMUploadSmapMain(CCanNode* pNode,INotifier& refNotifier) {
		CConfigSmapMenuInfo& ref = CConfigSmapMenuInfo::getInstance( );
		if (ref.load( pNode->getNodeAddress())) {
			CNodeLCDScreenmapFSMInfo* const pRefScreenmapsUpdateInfo = CNodeLCDScreenmapFSMInfo::GetInstance();		
			if (pRefScreenmapsUpdateInfo && pRefScreenmapsUpdateInfo->IsLocked() == FALSE) {				

				pRefScreenmapsUpdateInfo->Clear();

				CString strError;
				if (!CNodeLCDScreenmapFSMInfo_Util::addNode(pRefScreenmapsUpdateInfo, pNode, CNodeLCDScreenmapFSMInfo::TYPE_SCREENMAP_UPDATE_MAIN, &strError)) {
					AfxMessageBox(strError);
					return;
				}				

				TMGetGlobalHSM()->EvOperationUploadSmapMenu( );
				refNotifier.AddMessage("screenmapping file send...");
			}
			else {
				refNotifier.AddMessage("Error: screenmapping file not send");		
			}
		}
		else {
			refNotifier.AddMessage("Screenmapping config file not found");
		}
	}

#else
	void TMUploadSmapMyHome(CCanNode* pNode,INotifier& refNotifier) {
		CConfigSmapPageInfoInfo& ref = CConfigSmapPageInfoInfo::getInstance( );
		if (ref.load( pNode->getNodeAddress())) {									
			CNodeLCDScreenmapFSMInfo* const pRefScreenmapsUpdateInfo = CNodeLCDScreenmapFSMInfo::GetInstance();		
			if (pRefScreenmapsUpdateInfo && pRefScreenmapsUpdateInfo->IsLocked() == FALSE) {			
				pRefScreenmapsUpdateInfo->Clear();
				pRefScreenmapsUpdateInfo->AddNodeToSmapFsm(pNode);																
				pRefScreenmapsUpdateInfo->setType(CNodeLCDScreenmapFSMInfo::TYPE_SCREENMAP_UPDATE_MYHOME);			
				pRefScreenmapsUpdateInfo->addScreenmapID(CScreenmapID(SCREENMAP_V65_MYHOME));
				pRefScreenmapsUpdateInfo->addScreenmapID(CScreenmapID(SCREENMAP_V65_MYHOME_SENS));				
							
				TMGetGlobalHSM()->EvOperationUploadSmapMyHome( );
				refNotifier.AddMessage("screenmapping file send...");												
			}			
		}
		else {
			refNotifier.AddMessage("Screenmapping config file not found");				
		}
	}

	void TMUploadSmapMain(CCanNode* pNode,INotifier& refNotifier) {
		CConfigSmapMenuInfo& ref = CConfigSmapMenuInfo::getInstance( );
		if (ref.load( pNode->getNodeAddress())) {
			CNodeLCDScreenmapFSMInfo* const pRefScreenmapsUpdateInfo = CNodeLCDScreenmapFSMInfo::GetInstance();		
			if (pRefScreenmapsUpdateInfo && pRefScreenmapsUpdateInfo->IsLocked() == FALSE) {				
				pRefScreenmapsUpdateInfo->Clear();
				pRefScreenmapsUpdateInfo->AddNodeToSmapFsm(pNode);															
				pRefScreenmapsUpdateInfo->setType(CNodeLCDScreenmapFSMInfo::TYPE_SCREENMAP_UPDATE_MAIN);			
				pRefScreenmapsUpdateInfo->addScreenmapID(CScreenmapID(SCREENMAP_V65_MAIN_MOODS));
				pRefScreenmapsUpdateInfo->addScreenmapID(CScreenmapID(SCREENMAP_V65_MAIN_SENSOR));
				pRefScreenmapsUpdateInfo->addScreenmapID(CScreenmapID(SCREENMAP_V65_MAIN_AUDIO));															
				TMGetGlobalHSM()->EvOperationUploadSmapMenu( );
				refNotifier.AddMessage("screenmapping file send...");
			}
			else {
				refNotifier.AddMessage("Error: screenmapping file not send");		
			}
		}
		else {
			refNotifier.AddMessage("Screenmapping config file not found");
		}
	}
#endif

	

	void TMUploadBitmaps(
		CCanNode* pNode,
		INotifier& refNotifier)
	{
		CConfigCanFTP_Info& configCanFTP_Info =
			CConfigCanFTP_Info::getInstance();

		configCanFTP_Info.clear( );
		
		CString szFindFileName;
		szFindFileName.Format("%s\\config\\Bitmaps_%02x\\*.bgb",
							  TMGetFileSettings()->GetFullyQualifiedPathName(),
							  pNode->getNodeAddress());
	
		const int nrOfBitmaps = CFileFindFunctions::getCount(szFindFileName);
		int count = 0;

		// Alle bestanden die gevonden worden in de configuratie directory met
		// de extensie *.bgb moeten verstuurd worden.

		IFileFindIterator* it = 0;
		for (it = CFileFindFactory::createIterator(szFindFileName);
			 !it->isDone();
			 it->next())
		{
			CFileFindData* const pFileFindData = it->currentItem( );
			if (pFileFindData &&
				pFileFindData->getFileName() != 0)
			{										
				char szRemoteDirectory[64];
				if (CNodeBitmapConfig_Util::getRemoteDirectory(
					pNode,
					szRemoteDirectory,
					64))
				{
					// De bitmaps die moeten verstuurd worden bevinden zich in de directory:
					// "projectdirectory\config\Bitmaps_xx\"

					CString szFilename;
					CString szFileNameRemote;

					szFileNameRemote.Format("%s%s",
											szRemoteDirectory,
											pFileFindData->getFileName());

					szFilename.Format("%s\\config\\Bitmaps_%02x\\%s",
									  TMGetFileSettings()->GetFullyQualifiedPathName(),
									  pNode->getNodeAddress(),
									  pFileFindData->getFileName());
							
					CConfigCanFTP_FileEntry* const pEntry =
						configCanFTP_Info.create( );
					if (pEntry)
					{
						// De CConfigBitmaps::onFTPDone callback functie zorgt ervoor dat als
						// er een bitmap verstuurd werd, dit bgb bestand verplaatst worden naar
						// de sendedBitmap directory.

						CString szInfo;
						szInfo.Format("Bitmap configuration %d/%d",
									  count + 1,
									  nrOfBitmaps);

						pEntry->setNodeAddress( pNode->getNodeAddress() );
						pEntry->setLocalFileName(szFilename);
						pEntry->setRemoteFileName(szFileNameRemote);
						pEntry->setInfo(szInfo);
						pEntry->setAction(CConfigCanFTP_FileEntry::ACTION_FTP_WRITE);
						pEntry->setCallbackStarted(&CConfigBitmaps::onFTPStarted);
						pEntry->setCallbackDone(&CConfigBitmaps::onFTPDone);
						configCanFTP_Info.addTask(pEntry);	

						count++;
					}			
				}
			}
		}
		if (configCanFTP_Info.getCount( ) > 0)
		{
			refNotifier.AddMessage("Bitmap Configuration files send...");	
			TMGetGlobalHSM()->EvOperationFtpConfig( );
		}

		CFileFindFactory::destroyIterator( it );

		// Bij de laatste entry moeten we een callback registreren die de 
		// bestanden kopiert naar een andere map.
	}


	void TMUploadLedsCfg(
		CCanNode* pNode,
		INotifier& refNotifier)
	{
		CConfigAppUiCtrlInfo& configInfo=CConfigAppUiCtrlInfo::getInstance( );

		if (configInfo.load(pNode->getNodeAddress( )))
		{
			ASSERT(configInfo.isLoaded( ));

			refNotifier.AddMessage("LEDS config file load done");		
			TMGetGlobalHSM()->EvOperationUploadLedsCfg();
			refNotifier.AddMessage("LEDS config file send...");			
		}
		else
		{
			refNotifier.AddMessage("LEDS config file load failed");
			reportErrors(refNotifier, configInfo.getConfigFile( ) );			
		}		
	}

	void TMUploadSmapLiteCfg(
		CCanNode* pNode,
		INotifier& refNotifier)
	{
		CAppCfgSmapLiteInfo& configInfo=CAppCfgSmapLiteInfo::getInstance( );

		
		if (configInfo.load(pNode->getNodeAddress( )))
		{
			ASSERT(configInfo.isLoaded( ));

			// Room-e geen update database nodig, Smarttouch is er een update nodedatabase van de master nodig...
			configInfo.setUpdateDatabaseNeeded(CNodeScreenmapLiteInfo_Util::isUpdateNodeDbaseNeeded(pNode) == true ? TRUE : FALSE);

			refNotifier.AddMessage("Screenmap config file load done");		
			TMGetGlobalHSM()->EvOperationUploadSmapLite();
			refNotifier.AddMessage("Screenmap config file send...");			
		}
		else
		{
			refNotifier.AddMessage("Screenmap config file load failed");
			reportErrors(refNotifier, configInfo.getConfigFile( ) );			
		}		
	}

	void TMUploadSonosSrcCfg(
		CCanNode* pNode,
		INotifier& refNotifier)
	{
		CConfigCanFTP_Info& configCanFTP_Info =
			CConfigCanFTP_Info::getInstance();

		configCanFTP_Info.clear( );
		
		CConfigCanFTP_FileEntry* const pEntry =
			configCanFTP_Info.create( );

		if (pEntry)
		{
			enum { MAX_FILENAME_LENGTH = 255 };
			char szFileNameLocal[ MAX_FILENAME_LENGTH ];
			char szFileNameRemote[ MAX_FILENAME_LENGTH ];

			if (CNodeSonosSrcItemsConfig_Util::getFileNameLocal(
					pNode,
					szFileNameLocal,
					MAX_FILENAME_LENGTH))
			{
				if (CNodeSonosSrcItemsConfig_Util::getFileNameRemote(
						pNode,
						szFileNameRemote,
						MAX_FILENAME_LENGTH))
				{
					CString szFilename;
					szFilename.Format("%s%s",
									  TMGetFileSettings()->GetFullyQualifiedPathName(),
									  szFileNameLocal);

					// Enkel als het een geldig bestand is.

					if (FindFile(szFilename))
					{
						// De CConfigIRTX::onFTPDone callback functie stuurt een
						// bericht naar de node dat hij zijn database moet herladen
						// en een flush IRTX bericht versturen naar alle IRTX units.

						pEntry->setNodeAddress( pNode->getNodeAddress() );
						pEntry->setLocalFileName(szFilename);
						pEntry->setRemoteFileName(szFileNameRemote);
						pEntry->setInfo("Sonos source items configuration");
						pEntry->setAction(CConfigCanFTP_FileEntry::ACTION_FTP_WRITE);
						pEntry->setCallbackStarted(&CConfigSonosSrcItems::onFTPStarted);
						pEntry->setCallbackDone(&CConfigSonosSrcItems::onFTPDone);
						configCanFTP_Info.addTask(pEntry);
					
						refNotifier.AddMessage("Sonos source items configuration send...");						
						TMGetGlobalHSM()->EvOperationFtpConfig( );	
					}
					else
					{
						refNotifier.AddMessage("Sonos source items configuration file not found");							
					}
				}
			}
		}	
	}

	void TMUploadAVSettings(
		CCanNode* pNode,
		INotifier& refNotifier)
	{
		CConfigCanFTP_Info& configCanFTP_Info =
			CConfigCanFTP_Info::getInstance();

		configCanFTP_Info.clear( );
		
		CConfigCanFTP_FileEntry* const pEntry =
			configCanFTP_Info.create( );

		if (pEntry)
		{
			enum { MAX_FILENAME_LENGTH = 255 };
			char szFileNameLocal[ MAX_FILENAME_LENGTH ];
			char szFileNameRemote[ MAX_FILENAME_LENGTH ];

			if (CNodeAVSettingsConfig_Util::getFileNameLocal(
					pNode,
					szFileNameLocal,
					MAX_FILENAME_LENGTH))
			{
				if (CNodeAVSettingsConfig_Util::getFileNameRemote(
						pNode,
						szFileNameRemote,
						MAX_FILENAME_LENGTH))
				{
					CString szFilename;
					szFilename.Format("%s%s",
									  TMGetFileSettings()->GetFullyQualifiedPathName(),
									  szFileNameLocal);

					// Enkel als het een geldig bestand is.

					if (FindFile(szFilename))
					{
						// De CConfigAVSettings::onFTPDone callback functie stuurt een
						// bericht naar de node dat hij zijn database moet herladen				

						pEntry->setNodeAddress(pNode->getNodeAddress());
						pEntry->setLocalFileName(szFilename);
						pEntry->setRemoteFileName(szFileNameRemote);
						pEntry->setInfo("AV settings configuration");
						pEntry->setAction(CConfigCanFTP_FileEntry::ACTION_FTP_WRITE);
						pEntry->setCallbackStarted(&CConfigAVSettings::onFTPStarted);
						pEntry->setCallbackDone(&CConfigAVSettings::onFTPDone);
						configCanFTP_Info.addTask(pEntry);
					
						refNotifier.AddMessage("AV settings configuration file send...");						
						TMGetGlobalHSM()->EvOperationFtpConfig( );	
					}
					else
					{
						refNotifier.AddMessage("AV settings configuration file not found");							
					}
				}
			}
		}	
	}


};	// end namespace