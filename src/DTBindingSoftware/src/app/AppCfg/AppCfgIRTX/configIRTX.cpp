#include "stdafx.h"
#include "ConfigIRTX.h"

#include "App\AppCfg\AppCfgCanFtp\ConfigCanFtp_Info.h"

#include "TXNodeMessage.h"
#include "nodeMess.h"
#include "communicationLogfile.h"

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
///////////////////////////// CConfigCanIRTX /////////////////////
//////////////////////////////////////////////////////////////////
void CConfigIRTX::onFTPStarted(
	const CConfigCanFTP_FileEntry& entry)
{
	TRACER("CConfigCanIRTX::onFTPStarted\n");

	_waitForAck.bNodeAddress = entry.getNodeAddress( );
	_waitForAck.flag = false;
};

void CConfigIRTX::onFTPDone(
	const CConfigCanFTP_FileEntry& entry)
{
	TRACER("CConfigCanIRTX::onFTPStopped\n");

	const BYTE bNodeAddress = entry.getNodeAddress( );
	if (_waitForAck.flag == false &&
		_waitForAck.bNodeAddress == bNodeAddress)
	{
		CTXNodeDatabaseManagment::getInstance()->SendReload(bNodeAddress,DATABASE_ID_IRTXCODES);		
		_waitForAck.flag = true;
	}
};

void CConfigIRTX::onReceivedDatabaseACK(
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
			case DATABASE_OBJECT_LOAD: { /* Absorbe */ } break;		
			case DATABASE_ERROR_NOT_IMPLEMENTED:
			{
				// De database kon niet herladen worden, dus moet de node manueel gereset worden.
				CString szMessage;
				szMessage.Format("Node 0x%02x: IRTX database configuration send, Please Reset the node ...",
								 bNodeAddress);
				CDisplayDevice::DisplayInfoMsg( szMessage );				
			} break;

			case DATABASE_ERROR_SERIALIZATION_BUSY:
			{
				// Deze fout komt voor wanneer de serializatie gepausseerd werd.
				// Tijdens de update node database wordt de serializatie gepausseerd. Indien er een 
				// node niet gevonden werd dan blijft de serializatie gepausseerd en kunnen er geen
				// IRTX codes geconfigureerd worden. 
				// In het setup menu van de IRTX codes zijn de codes leeg.
				CString szMessage;
				szMessage.Format("The IRTX database configuration failed because the node 0x%02x was busy."
								 "A possible reason is because the node was busy updating the slave nodes.\n"
								 "Retry to send the configuration after the master is done updating all slave nodes."
								 "If the problem is not solved reset the node and try again...",
								 bNodeAddress);				
				CDisplayDevice::DisplayErrorMsg( szMessage );
				AfxMessageBox(szMessage,MB_ICONWARNING|MB_OK);
			} break;

		}
		_waitForAck.flag = false;
		_waitForAck.bNodeAddress = 0xFF;
	}
}
///////////////////////////////////////////////////////////////////