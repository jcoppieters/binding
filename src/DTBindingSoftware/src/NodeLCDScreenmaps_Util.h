// NodeLCDScreenmaps_Util.h: interface for the CNodeLCDScreenmaps_Util class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NODELCDSCREENMAPS_UTIL_H__B5DC1C22_978B_4679_AB68_2FE85275934B__INCLUDED_)
#define AFX_NODELCDSCREENMAPS_UTIL_H__B5DC1C22_978B_4679_AB68_2FE85275934B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//////////////////////////////////////////////////////////////

class CNodeLcdScreenmaps;
class CConfigSmapSmapInfo;

//////////////////////////////////////////////////////////////

struct CNodeLcdScreenmaps_Util  
{
	static BOOL update_StartPage(
		CConfigSmapSmapInfo& refSmapInfo,	
		CNodeLcdScreenmaps* pScreenmaps);

	static BOOL update_StartPage(
		CNodeLcdScreenmaps& refScreenmaps,	
		CConfigSmapSmapInfo* pSmapInfo);
};

///////////////////////////////////////////////////////////////

#endif // !defined(AFX_NODELCDSCREENMAPS_UTIL_H__B5DC1C22_978B_4679_AB68_2FE85275934B__INCLUDED_)
