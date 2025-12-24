#pragma once

class CConfigIRTXFileInfo;
////////////////////////////////////////////////

struct CConfigIRTXFileInfo_Util
{
	/**
	 * @brief	Functie naam opvragen
	 *
	 * @param	fReload [IN]: Bepaalt of het configbestand met herladen worden.
	 */
	static void getFunctionName(
		CConfigIRTXFileInfo& ref,
		int index,
		CString* s,
		BOOL fReload = FALSE);
};

////////////////////////////////////////////////