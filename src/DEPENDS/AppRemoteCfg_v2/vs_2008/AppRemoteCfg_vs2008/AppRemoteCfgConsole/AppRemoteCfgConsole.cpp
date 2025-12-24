// AppRemoteCfgConsole.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "httprcfg/httprcfg_api.h"
#include "apprcfg/apprcfgtasks.h"

#include <windows.h>		// sleep()
#include <conio.h>
#include <iostream>
#include <stdlib.h>
/**************************************************************/
static const char* path = "C:/developer/sw/desktop/dt/r&d/AppRemoteCfg/Debug";
static const int port = 8080;
static const int nodeAddress[ ] = { 0x03, 0x04, 0x06, 0x07, 0x08, 0x0a, 0x45, 0xfc };


#define TARGET_IP_ADDRESS		"192.168.0.164"
//#define TARGET_IP_ADDRESS		"192.168.0.239"
//#define TARGET_IP_ADDRESS		"192.168.0.250"
/**************************************************************/

using namespace DUOTECNO;


static void dump(const CStringList& list) 
{
	system("@cls||clear");					
	
	for(POSITION pos = list.GetHeadPosition(); pos != NULL; ) {
		std::cout << (LPCTSTR) list.GetNext(pos) << std::endl;			
	}
}

static void startTaskRequestNodeInfo( ) {

	std::cout << "Start request node info" << std::endl;

	AppRcfgTaskRequestNodeInfo task(TARGET_IP_ADDRESS, port);
	task.Start( );

	while(task.Running());	// Hier wachten...

	AppRcfgTaskRequestNodeInfo::Status_t status = task.getStatus();
	if (AppRcfgTaskRequestNodeInfo::STATUS_DONE == status) {
		system("@cls||clear");		
		std::cout << "Request node info done." << std::endl;

		AppCfgNodeInfo nodeInfo;
		if (task.getNodeInfo(&nodeInfo)) {			
			CStringList stringList;
			nodeInfo.format(&stringList);
			dump(stringList);		
		}
	}
	else if (AppRcfgTaskRequestNodeInfo::STATUS_CANCEL == status ||
		     AppRcfgTaskRequestNodeInfo::STATUS_ERROR == status) {
		std::cout << "Request node info failed" << std::endl;
		const char* const msg = task.getMsgError();
		if (0 != msg) {
			std::cout << "Msg: " << msg << std::endl;	
		}
	}
}

static void startTaskBindings(const char* strTask) {

	std::cout << "Start " << strTask << " bindings" << std::endl;

	// Taak aanmaken...

	AppCfgBindAction action(0 == strcmp(strTask,"erase") ? AppCfgBindAction::ERASE : AppCfgBindAction::WRITE);

	// Uploaden van de binding bestanden.
	const int numberOfNodes = sizeof(nodeAddress)/sizeof(int);
	for (int i=0;i<numberOfNodes;++i) {	
		action.add(nodeAddress[i]);		
	}

	// Taak uitvoeren...
	AppRcfgTaskBindings task(TARGET_IP_ADDRESS, port, path, action);
	task.Start(&action);

	// Wachten tot de taak gedaan is...
	AppCfgBindInfoNodes progressInfo;
	CStringList stringList;
	do {	
		if (task.logChanged(&stringList)) { 	
			dump(stringList);		
		}

		if (task.progressChanged(&progressInfo)) {
			// Absorbe: Op het einde dumpen...
		}

		::Sleep(1);
	} while(task.Running());


	AppRcfgTaskRequestNodeInfo::Status_t status = task.getStatus();
	if (AppRcfgTaskRequestNodeInfo::STATUS_DONE == status) {		
		system("@cls||clear");		
		std::cout << strTask << " bindings done..." << std::endl;

		stringList.RemoveAll();
		progressInfo.formatSummary(&stringList);
		dump(stringList);
	}
	else if (AppRcfgTaskRequestNodeInfo::STATUS_CANCEL == status ||
		     AppRcfgTaskRequestNodeInfo::STATUS_ERROR == status) {

		std::cout << strTask << " bindings failed" << std::endl;
		const char* const msg = task.getMsgError();
		if (0 != msg) {
			std::cout << "Msg: " << msg << std::endl;	
		}
	}	
}

static void startTaskEraseBindings( ) {
	startTaskBindings("erase");
}

static void startTaskWriteBindings( ) {
	startTaskBindings("write");
}

int _tmain(int argc, _TCHAR* argv[])
{
	// Curl init
	httprcfg_init( );

	// Request node info...
	{
		startTaskRequestNodeInfo( );
		std::cout << "Press enter to continue" << std::endl;
		_getch();
	}

	{
		startTaskEraseBindings( );
		std::cout << "Press enter to continue" << std::endl;
		_getch();
	}

	{
		startTaskWriteBindings( );	
		std::cout << "Press enter to exit" << std::endl;
		_getch();	
	}

	httprcfg_shutdown();
	return 0;
}
	


