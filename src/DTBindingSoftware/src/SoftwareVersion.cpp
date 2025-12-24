#include "stdafx.h"
#include "softwareversion.h"
/*******************************************************************/

// Oude BECK USB Modem.
// Ondersteund alle functies vanaf V65.24
static bool checkModemVersion_v65(BYTE bMajor,BYTE bMinor) {

	enum { CURRENT_MODEM_SW_VERSION_V65	= 0x6524 };

	const int CURRENT_MODEM_SW_VERSION_MAJOR = (CURRENT_MODEM_SW_VERSION_V65&0xFF00)>>8;
	const int CURRENT_MODEM_SW_VERSION_MINOR = CURRENT_MODEM_SW_VERSION_V65&0x00FF;
	if (bMajor != CURRENT_MODEM_SW_VERSION_MAJOR ||
		bMinor == 0xFF ||
		bMinor < CURRENT_MODEM_SW_VERSION_MINOR)
	{
		return false;
	}	
	return true;
}

// Nieuwe NXP USB Modem.
// Vanaf firmware versie V90.xx.xx
static bool checkModemVersion_v90(BYTE bMajor,BYTE bMinor) {
	return (bMajor == 0x90 ? true : false);		
}

/********************************************************************/
bool checkModemSoftwareVersion(BYTE bMajor, BYTE bMinor)
{
	bool result = false;		
	result = checkModemVersion_v90(bMajor,bMinor);
	if (!result) {		
		result = checkModemVersion_v65(bMajor,bMinor);
	}
	return result;
}
/*******************************************************************/