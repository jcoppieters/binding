#ifndef DUOTECNO_CONFIG_BITMAPS_H
#define	DUOTECNO_CONFIG_BITMAPS_H


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

///////////////////////////////////////////////////////////////////
class CConfigCanFTP_FileEntry;		// Voorwaartse declaratie.
///////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
///////////////////////////// CConfigIRTX ////////////////////////
//////////////////////////////////////////////////////////////////
struct CConfigBitmaps
{
	// Voor de bitmap configuratie wordt gebruik gemaakt van
	// de CAN FTP fsm.
	// Dit is enkel voor de nieuwe TFT software versies.
	// Deze callback functies kopieren de bestanden naar de
	// sended bitmaps mappen

	static void onFTPStarted(
		const CConfigCanFTP_FileEntry& entry);

	static void onFTPDone(
		const CConfigCanFTP_FileEntry& entry);

};
///////////////////////////////////////////////////////////////////
#endif