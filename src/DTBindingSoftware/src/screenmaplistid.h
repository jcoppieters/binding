#ifndef INCLUDED_SCREENMAPLISTID_H
#define INCLUDED_SCREENMAPLISTID_H

#include "TListIterator.h"

#ifndef INCLUDED_SCREENMAPID_H
	#include "screenmapid.h"
#endif

#include <afxtempl.h>

/////////////////////////////////////////////////////////////////////////////

typedef CList<CScreenmapID, CScreenmapID> CScreenmapIDList;
typedef TListIterator<CScreenmapID, CScreenmapID> CScreenmapIDListIterator;

/////////////////////////////////////////////////////////////////////////////


#endif