// NodeLCDScreenmapFSMInfo.h: interface for the CNodeLCDScreenmapFSMInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NODELCDSCREENMAPFSMINFO_H__3793D0D0_0B65_4D88_8FD1_6A21262257E7__INCLUDED_)
#define AFX_NODELCDSCREENMAPFSMINFO_H__3793D0D0_0B65_4D88_8FD1_6A21262257E7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
///////////////////////////////////////////////////////////////////////
#include "PtrListNodes.h"

#ifndef INCLUDED_SCREENMAPTYPES_H
	#include "screenmaptypes.h"
#endif
#ifndef INCLUDED_SCREENMAPLISTID_H
	#include "screenmaplistid.h"
#endif
///////////////////////////////////////////////////////////////////////
class CCanNode;			// voorwaartse declaratie.
///////////////////////////////////////////////////////////////////////

// Aanpassingen V16.21: Screenmapping van de virtuele energie meet units.
// Bijhouden per node welke screenmapping er moet doorgestuurd worden.
// Dit is nodig omdat de energie meet units enkel naar de LCD nodes 
// met firmware >=V66.11.00 moet doorgestuurd worden.

#if (USE_SMAP_ENERGYMEASURE == 1)
///////////////////////////////////////////////////////////////////////
class CNodeLCDSmapEntryFsm
{
public:
	CNodeLCDSmapEntryFsm(CCanNode* pNode);
	virtual ~CNodeLCDSmapEntryFsm();

	void addSmapID(CScreenmapID id);
	CScreenmapIDListIterator createScreenmapIDIterator( );

	CCanNode* GetNode( );

private:
	CCanNode* const m_pRefNode;				// De CCanode.
	CScreenmapIDList m_smapListID;			// De lijst van smaps.
};

///////////////////////////////////////////////////////////////////////
class CNodeLCDSmapEntriesFsm
{
public:
	CNodeLCDSmapEntriesFsm( );
	virtual ~CNodeLCDSmapEntriesFsm( );

	BOOL Add(CNodeLCDSmapEntryFsm* p);
	BOOL Remove(CNodeLCDSmapEntryFsm* p);
	CNodeLCDSmapEntryFsm* Get(BYTE index) const;
	int Count() const;	
	void Clear();

private:
	CPtrList m_entries;
};

#endif 

///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////

class CNodeLCDScreenmapFSMInfo  
{
public:
	enum Type {
		TYPE_SCREENMAP_UPDATE_UNITS	= 0,
		TYPE_SCREENMAP_UPDATE_MYHOME,
		TYPE_SCREENMAP_UPDATE_MAIN,
	};

protected:
	enum {
		TASK_FLAG_ERROR				= 0x01,
		TASK_FLAG_CANCELLED			= 0x02
	};

#if (USE_SMAP_ENERGYMEASURE == 1)
	CNodeLCDSmapEntriesFsm m_entries;
#endif 
#if (USE_SMAP_ENERGYMEASURE == 0)
	CPtrListNodes m_Nodes;									// nodes die moeten upgedate worden.
	CScreenmapIDList m_screenmapListID;						// lijst van screenmap ids die moeten doorgestuurd worden.
#endif 
	BOOL m_fLocked;
	int m_CntSend;
	CString	m_szError;
	BYTE m_TaskFlags;
	Type m_type;

protected:
	CNodeLCDScreenmapFSMInfo();


public:	
	static CNodeLCDScreenmapFSMInfo* GetInstance();
	virtual ~CNodeLCDScreenmapFSMInfo();

	CCanNode* GetSmapCurrNode(int index);
	int Count();

#if (USE_SMAP_ENERGYMEASURE == 0)
	void AddNodeToSmapFsm(CCanNode *pNode);	
#endif 	
	void Clear();
#if (USE_SMAP_ENERGYMEASURE == 0)
	CScreenmapIDListIterator createScreenmapIDIterator( );
	void addScreenmapID(CScreenmapID id);
#endif

	/** Used by QHSM Application */
	void DumpInfo();

	BOOL IsLocked() {
		return (m_fLocked);
	}
	void SetLock(BOOL flag) {
		m_fLocked = flag;
	}

	void SetSendCounter(int i) {
		m_CntSend = i;
	}
	int GetSendCounter() {
		return m_CntSend;
	}
	
	void SetErrorMessage(CString s) {
		m_szError = s;
	}
	CString GetErrorMessage() {
		return m_szError;
	}

	void Set_ErrorFlag() {
		m_TaskFlags |= TASK_FLAG_ERROR;
	}
	BOOL Is_ErrorFlag() {
		return ((m_TaskFlags & TASK_FLAG_ERROR) == TASK_FLAG_ERROR);
	}

	void Set_CancelFlag()  {
		m_TaskFlags |= TASK_FLAG_CANCELLED;
	}
	BOOL Is_CancelFlag() {
		return ((m_TaskFlags & TASK_FLAG_CANCELLED) == TASK_FLAG_CANCELLED);
	}

	void Set_SuccessFlag() {
		m_TaskFlags = 0;
	}
	BOOL Is_SuccessFlag() {
		return (m_TaskFlags == 0);	
	}
	
	Type getType( ) const {
		return m_type;
	}
	void setType(Type type) {
		m_type = type;
	}

#if (USE_SMAP_ENERGYMEASURE == 1)
	BOOL AddFsmEntry(CNodeLCDSmapEntryFsm* p) {
		return m_entries.Add(p);
	}	
#endif

private:
#if (USE_SMAP_ENERGYMEASURE == 1)
	friend struct CNodeLCDScreenmapFSMInfo_Util;
#endif
};


/////////////////////////////////////////////////////////////////////////////////////////////
#if (USE_SMAP_ENERGYMEASURE == 1)

// Sedert V16.21: Screenmapping van de virtuele energie meet units.
// Bijhouden per node welke screenmapping er moet doorgestuurd worden.
// Dit is nodig omdat de energie meet units enkel naar de LCD nodes 
// met firmware >=V66.11.00 moet doorgestuurd worden.
struct CNodeLCDScreenmapFSMInfo_Util
{
	static BOOL addNode(CNodeLCDScreenmapFSMInfo* m_pThis,CCanNode* pNode,CNodeLCDScreenmapFSMInfo::Type type, CString* pStrError);
	static CScreenmapIDListIterator createSmapIterator(CNodeLCDScreenmapFSMInfo* m_pThis, BYTE bNodeAddress);
};
#endif
/////////////////////////////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_NODELCDSCREENMAPFSMINFO_H__3793D0D0_0B65_4D88_8FD1_6A21262257E7__INCLUDED_)
