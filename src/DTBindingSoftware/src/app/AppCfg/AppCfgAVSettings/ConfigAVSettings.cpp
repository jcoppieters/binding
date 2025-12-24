#include "stdafx.h"
#include "ConfigAVSettings.h"

#include "App\AppCfg\AppCfgCanFtp\ConfigCanFtp_Info.h"
#include "TXNodeMessage.h"
#include "nodeMess.h"
#include "utils/app/DisplayDevice.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////
#define DEBUG_LOCAL_ON		0
//////////////////////////////////////////////////////////////////
namespace
{
	template <class PARAM>
		inline void TRACER(PARAM x)
	{
		#if (DEBUG_LOCAL_ON == 1)
		TRACE(x);
		#endif
	};

	template <class PARAM, class PARAM2>
		inline void TRACER(PARAM x, PARAM2 y)
	{
		#if (DEBUG_LOCAL_ON == 1)
		TRACE( x, y );
		#endif
	};


	struct WaitForAck
	{
		BYTE bNodeAddress;
		bool flag;
	};

	WaitForAck _waitForAck;
};
//////////////////////////////////////////////////////////////////
////////////////////////// CConfigAVSettings /////////////////////
//////////////////////////////////////////////////////////////////
void CConfigAVSettings::onFTPStarted(
	const CConfigCanFTP_FileEntry& entry)
{
	TRACER("CConfigAVSettings::onFTPStarted\n");

	_waitForAck.bNodeAddress = entry.getNodeAddress( );
	_waitForAck.flag = false;
};

void CConfigAVSettings::onFTPDone(
	const CConfigCanFTP_FileEntry& entry)
{
	TRACER("CConfigAVSettings::onFTPStopped\n");

	const BYTE bNodeAddress = entry.getNodeAddress( );
	if (_waitForAck.flag == false &&
		_waitForAck.bNodeAddress == bNodeAddress)
	{
		CTXNodeDatabaseManagment::getInstance()->SendReload(bNodeAddress,DATABASE_ID_AVSETTINGS);		
		_waitForAck.flag = true;
	}
};

void CConfigAVSettings::onReceivedDatabaseACK(
	BYTE bNodeAddress,
	BYTE bObjectCode)
{
	// Dit is zeker niet full-proof !!!!
	// Als de node niet antwoord met een EV_DATABASEMANAGEMENT functie (omdat een oude versie modem)
	// zal er geen melding gegeven worden.
	if (_waitForAck.flag == true &&
		_waitForAck.bNodeAddress == bNodeAddress)
	{
		switch (bObjectCode)
		{
			case DATABASE_OBJECT_LOAD: { /* Absorbe. */ } break;		
			case DATABASE_ERROR_NOT_IMPLEMENTED:
			{
				// De database kon niet herladen worden, dus moet de node manueel gereset worden.
				CString szMessage;
				szMessage.Format("Node 0x%02x: AVSettings database configuration send, Please Reset the node...",bNodeAddress);
				CDisplayDevice::DisplayInfoMsg( szMessage );				
			} break;
		}
		_waitForAck.flag = false;
		_waitForAck.bNodeAddress = 0xFF;
	}
}
///////////////////////////////////////////////////////////////////