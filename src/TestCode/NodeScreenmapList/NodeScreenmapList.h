
#if !defined(AFX_NODESCREENMAPLIST_H__A176361B_5323_4E19_A027_BE4B5861DB47__INCLUDED_)
#define AFX_NODESCREENMAPLIST_H__A176361B_5323_4E19_A027_BE4B5861DB47__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include <afxtempl.h>




/**
 * Klasse die bijhoudt welke screenmaps er moeten doorgestuurd worden per node.
 */
class CScreenmapIDList
{

private:

	/**
	 * Lijst : Screenmap identifiers
	 */
	typedef CList<int, int> CListScreenmapID;


	/**
	 * Map: Key Node address, Value = lijst met screenmapIDs
	 */
	typedef CMap<int, int, CListScreenmapID*, CListScreenmapID*> CMapScreenmapID;

	/**
	 * De map
	 */
	CMapScreenmapID m_map;

public:
	CScreenmapIDList( );
	~CScreenmapIDList( );

	void add(BYTE bNodeAddress, BYTE bScreenmapID);
	void dump( );
	void destroy( );
};



#endif // !defined(AFX_NODESCREENMAPLIST_H__A176361B_5323_4E19_A027_BE4B5861DB47__INCLUDED_)
