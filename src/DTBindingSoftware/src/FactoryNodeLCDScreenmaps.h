// FactoryNodeLCDScreenmaps.h: interface for the CFactoryNodeLCDScreenmaps class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FACTORYNODELCDSCREENMAPS_H__0C293A82_A7B9_4CAA_BAF6_7854B6588D34__INCLUDED_)
#define AFX_FACTORYNODELCDSCREENMAPS_H__0C293A82_A7B9_4CAA_BAF6_7854B6588D34__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CNodeLcdScreenmaps;		// voorwaartse declaratie.
class CCanNode;					// voorwaartse declaratie.
class CNodeDatabase;			// voorwaartse declaratie.

struct CFactoryNodeLCDScreenmaps  
{
	enum Type
	{
		TYPE_SCREENMAP_UPDATE_UNITS	= 0,
		TYPE_SCREENMAP_UPDATE_MYHOME,
		TYPE_SCREENMAP_UPDATE_MAIN,
	};

	static CNodeLcdScreenmaps* create(
		CNodeDatabase* pNodeDatabase,
		CCanNode* pNode, 
		Type type = TYPE_SCREENMAP_UPDATE_UNITS);
};

#endif // !defined(AFX_FACTORYNODELCDSCREENMAPS_H__0C293A82_A7B9_4CAA_BAF6_7854B6588D34__INCLUDED_)
