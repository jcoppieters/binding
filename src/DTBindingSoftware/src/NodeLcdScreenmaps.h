// NodeLcdScreenmaps.h: interface for the CNodeLcdScreenmaps class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NODELCDSCREENMAPS_H__BFDD843F_A17C_44B7_B6D9_55E11FC5EEF9__INCLUDED_)
#define AFX_NODELCDSCREENMAPS_H__BFDD843F_A17C_44B7_B6D9_55E11FC5EEF9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


///////////////////////////////////////////////////////////////////////
#ifndef INCLUDED_DT_NODESCREENMAP_INFO_H
	#include "nodeScreenmapInfo.h"
#endif

#include <afxtempl.h>
///////////////////////////////////////////////////////////////////////
class CNodeDatabase;
struct STRUCT_UNIT;
class CCanNode;
class IScreenmap;
class CScreenmapID;
///////////////////////////////////////////////////////////////////////


/**************************************************************************//**
 * @class	CNodeLcdScreenmaps
 * @brief	CNodeLcdScreenmaps klasse.
 * @author	Mitchell Tom
 *
 * @changes	TM,v0571: Diverse aanpassingen.
 ******************************************************************************/
class CNodeLcdScreenmaps  
{
public:
	CNodeLcdScreenmaps(CNodeDatabase* pNodeDatabase,CCanNode* pNode,int maxScreenmaps,const CScreenmapID* pArrTypes);
	virtual ~CNodeLcdScreenmaps();

	BOOL IsModified(void);
	void Load(CString szFileName);
	void Save(CString szFileName);
	const IScreenmap* GetScreenmap(BYTE i) const;
	IScreenmap* GetScreenmap(BYTE i);
	const IScreenmap* FindScreenmap(const CScreenmapID& id) const;
	IScreenmap* FindScreenmap(const CScreenmapID& id);

	int getMaxScreenmaps( ) const {
		return m_MaxScreenmaps;
	}

	CCanNode* GetAssignedNode() {
		return m_pNode;	
	}

	static BOOL Copy(CNodeLcdScreenmaps* pDest,const CNodeLcdScreenmaps* pSrc);

protected:	
	void Initialize(void);
	BOOL FormatFileName(CString szPathName,int index,CString &szFileName);
	BOOL FormatDirectory(CString szPathName,CString &szDirectory);

private:
	CNodeDatabase* const m_pNodeDatabase;
	CCanNode* const m_pNode;
	const int m_MaxScreenmaps;							// maximum aantal screenmaps.
	CArray<IScreenmap*, IScreenmap*> m_arrScreenmaps;	// array van screenmaps
	const CScreenmapID* const m_parrScreenmapTypes;	// ptr naar array met screenmap types.
};

#endif // !defined(AFX_NODELCDSCREENMAPS_H__BFDD843F_A17C_44B7_B6D9_55E11FC5EEF9__INCLUDED_)
