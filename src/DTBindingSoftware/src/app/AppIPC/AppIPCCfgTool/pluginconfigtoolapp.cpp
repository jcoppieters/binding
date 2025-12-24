#include "stdafx.h"
#include "plugincommunications.h"
#include "pluginconfigtoolapp.h"
#include "TMGlobals.h"
using namespace TMGlobalSettings;
/*******************************************************/
namespace DUOTECNO {
namespace PLUGIN {
/*******************************************************/

/*******************************************************/
/******************* IConfigToolApp ********************/
/*******************************************************/
IConfigToolApp& IConfigToolApp::getInstance( )
{
	static CConfigToolApp _instance;
	return _instance;
}
/*******************************************************/

/*******************************************************/
/****************** private data ***********************/
/*******************************************************/
static const char* const _szCommand =		"CMD";
static const char* const _szName =			"NAME";
static const char* const _szView =			"VIEW";
static const char* const _szNodeAddress =	"LA";

static CString _szWindowName;	// De huidige windowname

static const char* const _ARRAY_STRINGS_COMMANDS[ ] = {
	"NEW",					// COMMAND_NEW	
	"NAME",					// COMMAND_NAME
	"CLOSE",				// COMMAND_CLOSE
	"SHOW",					// COMMAND_SHOW
	"SAVE",					// COMMAND_SAVE
	"ACK",					// COMMAND_ACK
	"REFRESH",				// COMMAND_REFRESH	= 6,		// since v0x0565
	"EXPORT",				// COMMAND_EXPORT	= 7,		// since v0x0565
	"LOAD",					// COMMAND_LOAD = 8,
	"UPLOAD_DONE"			// COMMAND_UPOAD_DONE = 9
};

static const char* const _ARRAY_STRINGS_VIEW[ ] = 
{
	"UNITS",				// VIEW_UNITS	
	"MOODS",				// VIEW_MOODS
	"IRTX",					// VIEW_IRTX
	"IRRX",					// VIEW_IRRX
	"TEMP",					// VIEW_TEMP
	"TIME",					// VIEW_TIME
	"SCHEDULE",				// VIEW_SCHEDULE
	"BITMAPS",				// VIEW_BITMAPS
	"FTP",					// VIEW_FTP
	"SMAPUNITS",			// VIEW_SMAP_UNITS
	"SMAPMAIN",				// VIEW_SMAP_MAIN
	"SMAPMYHOME",			// VIEW_SMAP_MYHOME
	"SMAPAVROOM",			// VIEW_SMAP_AVROOM
	"AVMM",					// VIEW_SMAP_AVMM
	"GATEWAYCFG",			// VIEW_CONFIG_GATEWAY
	"ROOMCTRLAUDIO",		// VIEW_ROOMCTRL_AUDIOCFG
	"BOSESTCFG",			// VIEW_BOSESTCFG		--> OPPASSEN: OOK VOOR SONOS
	"LEDSCFG",				// VIEW_LEDSCFG
	"SONOSSRCCFG",			// VIEW_SONOSSRCCFG
	"AVSETTINGS",			// VIEW_AVSETTINGS
	"AVMATRIX",				// VIEW_AVMATRIX		// CHANGES_AV_UNIVERSAL
	"TRIGGERS",				// VIEW_TRIGGERS		// CHANGES_AV_UNIVERSAL
	"APP",					// VIEW_MOBILE_APP
	"FIRMWARE",				// VIEW_FIRMWARE_UPDATE
	"DALI",					// VIEW_DALI
	"ENERGYMANAGER"			// VIEW_ENERGYMANAGER
};

/*******************************************************/
/***************private free functions *****************/
/*******************************************************/
static CWnd* findConfiguratorWindow(CString szWindowName = "")
{
	// _szWindowName onthoudt de vorige window naam.
	if (szWindowName.IsEmpty()) {
		_szWindowName = TMGetFileSettings()->dtConfigurator.getWindowName( ) + TMGetFileSettings()->GetTitle( );	
	}
	else {
		_szWindowName = szWindowName;
	}	
	CWnd* const pDestWindow = CMFCFindWindow::Find(_szWindowName, CMFCFindWindow::FLAG_NAME_WINDOW);	
	return pDestWindow;
}

static inline void formatFullPathName(CString* pString)
{
	*pString = "\"" + TMGetFileSettings()->GetFullyQualifiedFileName( ) + "\"";
}

static bool getCommand(Command command,CString* pStr) {
	bool retVal = false;
	ASSERT( command <= COMMAND_UPOAD_DONE );
	if (command != COMMAND_NONE) {
		*pStr = _ARRAY_STRINGS_COMMANDS[(int) command];
		retVal = true;
	}	
	return retVal;
}

static Command getCommand(const CString& str) {
	int retValue = COMMAND_NONE;
	for (int i=0;i<=(int)COMMAND_UPOAD_DONE;++i) {
		const char*  strTemp = _ARRAY_STRINGS_COMMANDS[i];
		if (NULL != strTemp) {
			if (0 == str.Compare(strTemp)) {
				retValue = i;
			}		
		}	
	}
	ENSURE(retValue == COMMAND_NONE || (retValue>=COMMAND_NEW && retValue<=COMMAND_UPOAD_DONE));
	return (Command) retValue;
}

static bool getView(View view,CString* pStr) {
	bool retVal = false;
	ASSERT(view <= VIEW_ENERGYMANAGER );	
	if (view != VIEW_NONE) {
		*pStr = _ARRAY_STRINGS_VIEW[(int) view];
		retVal = true;
	}	
	return retVal;
}

static View getView(const CString& str) {
	int retValue = VIEW_NONE;
	for (int i=0;i<=(int)VIEW_ENERGYMANAGER;++i) {
		const char*  strTemp = _ARRAY_STRINGS_VIEW[i];
		if (NULL != strTemp) {
			if (0 == str.Compare(strTemp)) {
				retValue = i;
			}		
		}	
	}
	ENSURE(retValue == VIEW_NONE || (retValue>=VIEW_UNITS && retValue<=VIEW_ENERGYMANAGER));
	return (View) retValue;
}

/*******************************************************/
BOOL CCommandStringFormatter::Build(CString* pString, Command command, const char* szFullPathName, View view,BYTE bNodeAddress) {	
	CString szTempParam;
	pString->Empty( );
	if (getCommand(command, &szTempParam)) {
		CString szTemp;
				
		// CMD=xxx;
		szTemp.Format("%s=%s;",_szCommand, szTempParam);
		*pString += szTemp;

		// NAME=xxx;
		if (szFullPathName) {
			szTemp.Format("%s=%s;",_szName, szFullPathName);
			*pString += szTemp;
		}

		// VIEW=xxx;
		if (getView(view,&szTempParam)) {
			szTemp.Format("%s=%s;",_szView, szTempParam);
			*pString += szTemp;
		}

		// LA=xxx;
		if (bNodeAddress != 0xFF) {
			szTemp.Format("%s=0x%02x;",_szNodeAddress,bNodeAddress);
			*pString +=szTemp;
		}
		//*pString += "\"";
		return TRUE;		
	}
	return FALSE;
};


/*******************************************************/
ICommandStringParser::~ICommandStringParser( ) { }
/*******************************************************/
CCommandStringParser::CCommandStringParser(CString& str) : 
	m_str(str), m_parsedValues() { }

BOOL CCommandStringParser::Parse( ) {
	if (this->m_str.IsEmpty()) {
		return FALSE;
	}

	// De inputstring splitsen op puntkomma
    int startPos = 0;
    CString token;
    
    while ((startPos = this->m_str.Find(';', startPos)) != -1) {
        token = this->m_str.Mid(0, startPos);  // Extract het token tot aan de puntkomma
        this->m_str = this->m_str.Mid(startPos + 1); // Verwijder het token en de puntkomma
        startPos = 0;
        
        // Nu splitsen we het token op basis van het gelijkteken '='
        int equalPos = token.Find('=');
        if (equalPos != -1) {
            CString key = token.Left(equalPos);					// De sleutel (voor '=')
            CString value = token.Mid(equalPos + 1);			// De waarde (na '=')
            this->m_parsedValues[key] = value; 
        }
    }

    // De laatste token na de laatste puntkomma verwerken
    if (!this->m_str.IsEmpty()) {
        int equalPos = this->m_str.Find('=');
        if (equalPos != -1) {
            CString key = this->m_str.Left(equalPos);
            CString value = this->m_str.Mid(equalPos + 1);
            this->m_parsedValues[key] = value;
        }
    }
	return (this->m_parsedValues.size() <= 0 ? FALSE : TRUE);
}

Command CCommandStringParser::getCommand() const {		
	if (this->m_parsedValues.size() <= 0) {
		return COMMAND_NONE;	
	}
	std::map<CString, CString>::const_iterator it= this->m_parsedValues.find(_szCommand);
	if (it == this->m_parsedValues.end()) {
		return COMMAND_NONE;
	}	
	const CString str = it->second.GetString();
	if (str.IsEmpty()) {
		return COMMAND_NONE;
	}
	const Command command = DUOTECNO::PLUGIN::getCommand((const CString&)str);
	return command;
}

CString CCommandStringParser::getFullName() const {
	if (this->m_parsedValues.size() <= 0) {
		return CString();
	}
	std::map<CString, CString>::const_iterator it= this->m_parsedValues.find(_szName);
	if (it == this->m_parsedValues.end()) {
		return CString();
	}
	const CString str = it->second.GetString();
	return str;
}

View CCommandStringParser::getView() const {
	if (this->m_parsedValues.size() <= 0) {
		return VIEW_NONE;	
	}
	std::map<CString, CString>::const_iterator it= this->m_parsedValues.find(_szView);
	if (it == this->m_parsedValues.end()) {
		return VIEW_NONE;
	}	
	const CString str = it->second.GetString();
	const View view = DUOTECNO::PLUGIN::getView((const CString&)str);
	return view;
}

int CCommandStringParser::getNodeAddress( ) const {
	if (this->m_parsedValues.size() <= 0) {
		return -1;	
	}
	std::map<CString, CString>::const_iterator it= this->m_parsedValues.find(_szNodeAddress);
	if (it == this->m_parsedValues.end()) {
		return -1;
	}	
	CString str = it->second.GetString();
	str.TrimLeft();
	str.TrimRight();
	if (str.GetLength() <= 0) {
		return -1;
	}       
	const int hexValue = _tcstoul(str, NULL, 16); 
	ASSERT(hexValue>=0 && hexValue<=0xFF);
	return hexValue;    
}
/*******************************************************/
/******************* CConfigToolApp ********************/
/*******************************************************/
CConfigToolApp::CConfigToolApp( ) { }
CConfigToolApp::~CConfigToolApp( ) { }
/*******************************************************/
void CConfigToolApp::sendMessage(CWnd* const pDestWindow, Command command, bool bringToFront,
	const char* szFullName, View view, BYTE bNodeAddress)
{
	CString szCommandString;
	if (CCommandStringFormatter::Build(&szCommandString,command,szFullName,view,bNodeAddress)) {
		if (!pDestWindow) {
			CMFCOpenFile::OpenFile(TMGetFileSettings()->dtConfigurator.getFullPathName(),szCommandString);
		}
		else {
			CMFCMsgCopyData msg(*AfxGetMainWnd(),szCommandString);
			SendWindowMessage(*pDestWindow, msg );	
			if (bringToFront) {
				pDestWindow->ShowWindow(SW_RESTORE);
				pDestWindow->BringWindowToTop( );
			}
		}
	}
}

/*******************************************************/
void CConfigToolApp::show( )
{
	CString szFullPathPrj;
	formatFullPathName(&szFullPathPrj);
				
	CWnd* const pDestWindow = findConfiguratorWindow( );	
	sendMessage(pDestWindow,
				pDestWindow == 0 ? COMMAND_NEW : COMMAND_SHOW,
				true,	// bring to front					
				szFullPathPrj);
}

void CConfigToolApp::show(
	View view, 
	BYTE bNodeAddress)
{
	if (view != VIEW_NONE)
	{
		CString szFullPathPrj;
		formatFullPathName(&szFullPathPrj);
				
		CWnd* const pDestWindow = findConfiguratorWindow( );	
		sendMessage(pDestWindow,
					pDestWindow == 0 ? COMMAND_NEW : COMMAND_SHOW,
					true,	// bring to front					
					szFullPathPrj,
					view,
					bNodeAddress);	
	}	
}

void CConfigToolApp::load(View view, BYTE bNodeAddress) {
	CWnd* const pDestWindow = findConfiguratorWindow( );	
	if (NULL == pDestWindow) {
		return;
	}

	CString szFullPathPrj;
	formatFullPathName(&szFullPathPrj);

	sendMessage(pDestWindow,
				COMMAND_LOAD,
				false,	// bring to front					
				szFullPathPrj,
				view,
				bNodeAddress);		
}

void CConfigToolApp::newDirectory( )
{
	// newDirectory wordt aangeroepen als het project reeds naar de 
	// nieuwe directory verwijst.
	// De window naam van de configurator is nog niet upgedate daarom
	// moet _szWindowName gebruikt worden.
	// (_szWindowName onthoudt de vorige window naam. )

	CWnd* const pDestWindow = findConfiguratorWindow(_szWindowName);	
	if (pDestWindow)
	{
		CString szFullPathPrj;
		formatFullPathName(&szFullPathPrj);
					
		sendMessage(pDestWindow,
					COMMAND_NEW,
					false,		// bring to front				
					szFullPathPrj);
	}
}

void CConfigToolApp::close( )
{
	CWnd* const pDestWindow = findConfiguratorWindow( );	
	if (pDestWindow)
	{
		// Enkel versturen wanneer het venster aktief is,
		// zal anders een nieuw venster openen.
		sendMessage(findConfiguratorWindow( ),COMMAND_CLOSE);					
	}
}

void CConfigToolApp::save( )
{
	CWnd* const pDestWindow = findConfiguratorWindow( );	
	if (pDestWindow)
	{
		// Enkel versturen wanneer het venster aktief is,
		// zal anders een nieuw venster openen.

		CString szFullPathPrj;
		formatFullPathName(&szFullPathPrj);

		sendMessage(findConfiguratorWindow( ),
					COMMAND_SAVE,
					false,
					szFullPathPrj);				
	}
}

void CConfigToolApp::refreshDisplay( )
{	
	CWnd* const pDestWindow = findConfiguratorWindow( );	
	if (pDestWindow)
	{
		// Enkel versturen wanneer het venster aktief is,
		// zal anders een nieuw venster openen.

		CString szFullPathPrj;
		formatFullPathName(&szFullPathPrj);

		sendMessage(findConfiguratorWindow( ),
					COMMAND_REFRESH,
					false,		// bring to front			
					szFullPathPrj);				
	}
}

void CConfigToolApp::exportHTMLFiles( )
{
	CWnd* const pDestWindow = findConfiguratorWindow( );	
	if (pDestWindow)
	{
		// Enkel versturen wanneer het venster aktief is,
		// zal anders een nieuw venster openen.

		CString szFullPathPrj;
		formatFullPathName(&szFullPathPrj);

		sendMessage(findConfiguratorWindow( ),
					COMMAND_EXPORT,
					false,		// bring to front			
					szFullPathPrj);				
	}
}

BOOL CConfigToolApp::findWindowConfigurator(CWnd** pWnd)
{
	*pWnd = findConfiguratorWindow( );
	if (*pWnd != 0)
	{
		return TRUE;
	}
	return FALSE;
}

ICommandStringParser* CConfigToolApp::parseReceived(CString& s) {
	if (s.IsEmpty()) {
		return NULL;
	}
	CCommandStringParser* parser = new CCommandStringParser(s);
	if (NULL == parser) {
		return NULL;
	}
	if (FALSE == parser->Parse()) {
		delete parser;
		return NULL;
	}
	return parser;
}
/*******************************************************/

int pluginconfigtoolapp_testcase(int testcase) {
	int result = -1;
	switch(testcase) {
		case 0: {			
			CConfigToolApp& refConfigToolApp = (CConfigToolApp&)(CConfigToolApp::getInstance());
			CWnd* const pWnd = AfxGetMainWnd();
			if (NULL == pWnd) {
				return -1;
			}
			CString strTitle;
			pWnd->GetWindowText(strTitle);
			CWnd* const pDestWindow = CMFCFindWindow::Find(strTitle,CMFCFindWindow::FLAG_NAME_WINDOW);
			if (NULL == pDestWindow) {
				return -1;
			}
			const Command command = COMMAND_UPOAD_DONE;
			const bool bringToFront = false;
			const char* const szFullName = "Not used";
			const View view = VIEW_UNITS;
			const BYTE bNodeAddress = 0xFC;

			refConfigToolApp.sendMessage(pDestWindow, command, bringToFront, szFullName, view, bNodeAddress);
			result = 0;

		} break;	
	}
	return result;	
}


/*******************************************************/
};
};
/*******************************************************/
