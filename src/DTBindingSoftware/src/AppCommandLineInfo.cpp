// AppCommandLineInfo.cpp: implementation of the CAppCommandLineInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "AppCommandLineInfo.h"

#include "app\appstate\appstate.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAppCommandLineInfo::CAppCommandLineInfo() { }
CAppCommandLineInfo::~CAppCommandLineInfo() { }

//////////////////////////////////////////////////////////////////////
void CAppCommandLineInfo::ParseParam(LPCTSTR lpszParam, BOOL bFlag, BOOL bLast) {	
	if (bFlag == TRUE) {
		CString sParam(lpszParam);
		sParam.MakeUpper();
		if (parseMode(sParam)) {
			// Mode param gevonden.
		}
		else if (parseTcpIpMode(sParam)) {
			// Enable tcp gevonden.
		}
		else if (!parseTcpRemoteCloudMode(sParam)) {
			// Enable tcp cloud gevonden.
		}
	}	
	CCommandLineInfo::ParseParam(lpszParam, bFlag, bLast);
}

bool CAppCommandLineInfo::parseMode(const CString& strUpper) {
	const CString szKey("MODE");
	const int index = strUpper.Find(szKey);
	if (index < 0) {
		return false;
	}
	
	const CString szValModeEngineering("ENG");
	const CString szValModeInstallation("INS");
	const CString szValModeSimulation("SIM");

	int i = 0;
	CAppState& refAppState = CAppState::getInstance();
	if ((i = strUpper.Find(szValModeEngineering,index)) != -1) {
		refAppState.setAppType(CAppState::APP_TYPE_ENGINEERING);				
	}
	else if ((i = strUpper.Find(szValModeInstallation,index)) != -1) {
		refAppState.setAppType(CAppState::APP_TYPE_INSTALLATION);				
	}
	else if ((i = strUpper.Find(szValModeSimulation,index)) != -1) {
		refAppState.setAppType(CAppState::APP_TYPE_SIMULATION);			
	}	
	return true;
}

bool CAppCommandLineInfo::parseTcpIpMode(const CString strUpper) {
	const CString szKey("ENABLE_TCP");
	if (0 != strUpper.Compare(szKey)) {
		return false;
	}
	CAppState::getInstance().getAppPermisions().setTcpCfgTouchscreenEnabled(true);
	return true;
}

// CHANGES_CLOUDBRIDGE_POC
// Commandline optie "-enable_tcp_cloud": Bij inschakeling verloopt remote TCP/IP-communicatie via de cloudserver i.p.v. direct via DNS.
bool CAppCommandLineInfo::parseTcpRemoteCloudMode(const CString strUpper) {
	const CString szKey("ENABLE_TCP_CLOUD");
	if (0 != strUpper.Compare(szKey)) {
		return false;
	}
	CAppState::getInstance().setEnableUseRemoteTCPCloudServer(true);
	return true;
}