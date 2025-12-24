#ifndef DUOTECNO_CONFIG_IRTX_H
#define	DUOTECNO_CONFIG_IRTX_H


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

///////////////////////////////////////////////////////////////////
class CConfigCanFTP_FileEntry;		// Voorwaartse declaratie.
///////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
///////////////////////////// CConfigIRTX ////////////////////////
//////////////////////////////////////////////////////////////////
struct CConfigIRTX
{
	// Voor de IRTX configuratie wordt gebruik gemaakt van
	// de CAN FTP fsm.
	// Dit is enkel voor de nieuwe TFT software versies.
	// Deze callback functies versturen additionele commandos
	// nodig voor de IRTX configuratie.
	// Dit is nodig om de IRTX database te herladen.

	static void onFTPStarted(
		const CConfigCanFTP_FileEntry& entry);

	static void onFTPDone(
		const CConfigCanFTP_FileEntry& entry);


	// Deze functie wordt aangeroepen in de CCommApplication
	// messageHandler: MessageHandler_EV_DATABASEMANAGMENT
	static void onReceivedDatabaseACK(
		BYTE bNodeAddress,
		BYTE bObjectCode);
};
///////////////////////////////////////////////////////////////////
#endif