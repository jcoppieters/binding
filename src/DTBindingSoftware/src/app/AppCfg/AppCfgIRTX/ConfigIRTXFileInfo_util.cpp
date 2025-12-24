#include "stdafx.h"
#include "ConfigIRTXFileInfo_Util.h"
#include "ConfigIRTXFileInfo.h"
#include <stdio.h>

#include "utils/win32/win32filefind.h"

#include "TMGlobals.h"
using namespace TMGlobalSettings;


void CConfigIRTXFileInfo_Util::getFunctionName(
	CConfigIRTXFileInfo& ref,
	int index,
	CString* ps,
	BOOL fReload)
{
	bool result = false;
	if ((fReload == TRUE && ref.load()) || (fReload == FALSE)) {
		if (ref.isLoaded()) {
			CConfigIRTXFile& refFile = ref.getConfigFile( );	
			if (index < refFile.getCount()) {
				CConfigIRTXFileEntry* const pEntry = refFile.get(index);
				if (pEntry) {
					ps->Format("Ircode %d - %s",index,pEntry->getHeader().getFunctionName());	
					result = true;
				}	
			}
		}
	}

	if (!result) {
		ps->Format("Ircode %d",index);
	}
};