// NodeLCDScreenmapFSMInfo.cpp: implementation of the CNodeLCDScreenmapFSMInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "NodeLCDScreenmapFSMInfo.h"

#include "CanNode.h"
#include "utils/app/DisplayDevice.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
CNodeLCDScreenmapFSMInfo* CNodeLCDScreenmapFSMInfo::GetInstance()
{
	static CNodeLCDScreenmapFSMInfo instance;
	return (&instance);
}
//////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////
#if (USE_SMAP_ENERGYMEASURE == 1)
///////////////////////////////////////////////////////////////////////
CNodeLCDSmapEntryFsm::CNodeLCDSmapEntryFsm(CCanNode* pNode) : 
	m_pRefNode(pNode), m_smapListID( ) { }

CNodeLCDSmapEntryFsm::~CNodeLCDSmapEntryFsm( ) {
	m_smapListID.RemoveAll();		
}

void CNodeLCDSmapEntryFsm::addSmapID(CScreenmapID id) {
	m_smapListID.AddTail( id );
}

CCanNode* CNodeLCDSmapEntryFsm::GetNode( ) {
	return m_pRefNode;
}

CScreenmapIDListIterator CNodeLCDSmapEntryFsm::createScreenmapIDIterator( ) {
	return CScreenmapIDListIterator ( m_smapListID );
}	

///////////////////////////////////////////////////////////////////////
CNodeLCDSmapEntriesFsm::CNodeLCDSmapEntriesFsm( ) : m_entries( ) { };

CNodeLCDSmapEntriesFsm::~CNodeLCDSmapEntriesFsm( ) {
	Clear( );
}

BOOL CNodeLCDSmapEntriesFsm::Add(CNodeLCDSmapEntryFsm* p) {
	if (NULL == p) {
		return FALSE;
	}
	m_entries.AddTail(p);
	return TRUE;
}

BOOL CNodeLCDSmapEntriesFsm::Remove(CNodeLCDSmapEntryFsm* p) {
	POSITION pos = m_entries.Find(p);
	if (0 == pos) {
		return FALSE;
	}
	m_entries.RemoveAt(pos);
	return TRUE;	
}

CNodeLCDSmapEntryFsm* CNodeLCDSmapEntriesFsm::Get(BYTE index) const {
	CNodeLCDSmapEntryFsm *p = NULL;
	if (m_entries.GetCount() == 0) {
		return NULL;
	}

	int i = 0;
	POSITION pos = m_entries.GetHeadPosition();		
	while(pos!=NULL) {
		if ((p = (CNodeLCDSmapEntryFsm*) m_entries.GetNext(pos)) != NULL) {				
			if (i == index) {
				return p;
			}
		}						
		i++;
	}			
	return NULL;
}

int CNodeLCDSmapEntriesFsm::Count() const {
	return (m_entries.GetCount());
}

void CNodeLCDSmapEntriesFsm::Clear() {
	POSITION pos = m_entries.GetHeadPosition();		
	while(pos!=NULL) {
		CNodeLCDSmapEntryFsm *p = NULL;
		if ((p = (CNodeLCDSmapEntryFsm*) m_entries.GetNext(pos)) != NULL) {						
			delete p;
			p = NULL;
		}
	}						
	m_entries.RemoveAll();
}

#endif 

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CNodeLCDScreenmapFSMInfo::CNodeLCDScreenmapFSMInfo() :
	m_fLocked( FALSE ) 
#if (USE_SMAP_ENERGYMEASURE == 1)
	,m_entries()
#endif 
{
	Clear();
}

CNodeLCDScreenmapFSMInfo::~CNodeLCDScreenmapFSMInfo() {
	Clear();
}

///////////////////////////////////////////////////////////////////////

#if (USE_SMAP_ENERGYMEASURE == 0)
void CNodeLCDScreenmapFSMInfo::AddNodeToSmapFsm(CCanNode *pNode) {
	m_Nodes.Add(pNode);
}

CCanNode* CNodeLCDScreenmapFSMInfo::GetSmapCurrNode(int index) {
	return (m_Nodes.Get(index));
}

int CNodeLCDScreenmapFSMInfo::Count() {
	return (m_Nodes.Count());
}
#endif

void CNodeLCDScreenmapFSMInfo::Clear() {
	m_CntSend = 0;
	m_szError.Empty();
	m_TaskFlags = 0;
#if (USE_SMAP_ENERGYMEASURE == 0)
	m_Nodes.Clear();
	m_screenmapListID.RemoveAll( );	
#endif 
	m_type = TYPE_SCREENMAP_UPDATE_UNITS;

#if (USE_SMAP_ENERGYMEASURE == 1)
	m_entries.Clear();
#endif 
}

#if (USE_SMAP_ENERGYMEASURE == 1)

CCanNode* CNodeLCDScreenmapFSMInfo::GetSmapCurrNode(int i) {
	CNodeLCDSmapEntryFsm* const pEntry = m_entries.Get(i);
	if (NULL == pEntry) {
		return NULL;
	}
	return pEntry->GetNode();		
}

int CNodeLCDScreenmapFSMInfo::Count() {
	return m_entries.Count();
}

void CNodeLCDScreenmapFSMInfo::DumpInfo() {
	const int nCount = this->Count();
	const int nSendCnt = this->GetSendCounter();
	CString s;
	CCanNode *pNode = NULL;

	CDisplayDevice::Clear();

	if (Is_CancelFlag() == TRUE) {
		CDisplayDevice::DisplayInfoMsg("Update LCD Screenmap Cancelled by user ...");							

		for (int i=0;i<nSendCnt;i++) {
			pNode = this->GetSmapCurrNode(i);
			if (NULL != pNode) {
				s.Format("Node '%s' : Update screenmap done!", pNode->szNodeName);
				CDisplayDevice::DisplayInfoMsg(s);
			}			
		}

		for (int i=nSendCnt;i<nCount;i++) {
			pNode = this->GetSmapCurrNode(i);
			if (NULL != pNode) {
				s.Format("Node '%s' : Update screenmap CANCELLED BY USER", pNode->szNodeName);
				CDisplayDevice::DisplayInfoMsg(s);
			}				
		}
	}
	else if (Is_ErrorFlag() == TRUE) {		
		CDisplayDevice::DisplayInfoMsg("Update LCD Screenmap error ...");
					
		for (int i=0;i<nSendCnt;i++) {
			pNode = this->GetSmapCurrNode(i);
			if (NULL != pNode) {
				s.Format("Node '%s' : Update screenmap done!", pNode->szNodeName);
				CDisplayDevice::DisplayInfoMsg(s);
			}
		}

		for (int i=nSendCnt;i<nCount;i++) {
			pNode = this->GetSmapCurrNode(i);
			if (NULL != pNode) {
				s.Format("Node '%s' : Update screenmap FAILED", pNode->szNodeName);
				CDisplayDevice::DisplayInfoMsg(s);			
			}
		}

		s = GetErrorMessage();
		if (s.IsEmpty() == FALSE) {
			CDisplayDevice::DisplayInfoMsg(s);
		}
	}
	else {
		CDisplayDevice::DisplayInfoMsg("Update LCD Screenmap done ...");	

		for (int i=0;i<nCount;i++) {
			pNode = this->GetSmapCurrNode(i);
			if (NULL != pNode) {
				s.Format("Node '%s' : Update screenmap done!",pNode->szNodeName);
				CDisplayDevice::DisplayInfoMsg(s);				
			}
		}
	}	
}
#endif

#if (USE_SMAP_ENERGYMEASURE == 0)
void CNodeLCDScreenmapFSMInfo::DumpInfo() {
	const int nCount = Count();
	const int nSendCnt = GetSendCounter();
	CString s;
	CCanNode *pNode;

	CDisplayDevice::Clear();

	if (Is_CancelFlag() == TRUE) {
		CDisplayDevice::DisplayInfoMsg("Update LCD Screenmap Cancelled by user ...");							

		for (int i=0;i<nSendCnt;i++) {
			if ((pNode = GetSmapCurrNode(i)) != NULL) {
				s.Format("Node '%s' : Update screenmap done!", pNode->szNodeName);
				CDisplayDevice::DisplayInfoMsg(s);
			}
		}

		for (int i=nSendCnt;i<nCount;i++) {
			if ((pNode = GetSmapCurrNode(i)) != NULL) {
				s.Format("Node '%s' : Update screenmap CANCELLED BY USER", pNode->szNodeName);
				CDisplayDevice::DisplayInfoMsg(s);
			}
		}
	}
	else if (Is_ErrorFlag() == TRUE) {		
		CDisplayDevice::DisplayInfoMsg("Update LCD Screenmap error ...");
					
		for (int i=0;i<nSendCnt;i++) {
			if ((pNode = GetSmapCurrNode(i)) != NULL) {
				s.Format("Node '%s' : Update screenmap done!", pNode->szNodeName);
				CDisplayDevice::DisplayInfoMsg(s);
			}
		}

		for (int i=nSendCnt;i<nCount;i++) {
			if ((pNode = GetSmapCurrNode(i)) != NULL) {	
				s.Format("Node '%s' : Update screenmap FAILED", pNode->szNodeName);
				CDisplayDevice::DisplayInfoMsg(s);
			}
		}

		s = GetErrorMessage();
		if (s.IsEmpty() == FALSE) {
			CDisplayDevice::DisplayInfoMsg(s);
		}
	}
	else {
		CDisplayDevice::DisplayInfoMsg("Update LCD Screenmap done ...");	

		for (int i=0;i<nCount;i++) {
			if ((pNode = GetSmapCurrNode(i)) != NULL) {
				s.Format("Node '%s' : Update screenmap done!",pNode->szNodeName);
				CDisplayDevice::DisplayInfoMsg(s);
			}
		}
	}	
}
#endif


#if (USE_SMAP_ENERGYMEASURE == 0)
CScreenmapIDListIterator CNodeLCDScreenmapFSMInfo::createScreenmapIDIterator( ) {
	return CScreenmapIDListIterator ( m_screenmapListID );
}

void CNodeLCDScreenmapFSMInfo::addScreenmapID(CScreenmapID id) {
	m_screenmapListID.AddTail( id );
}
#endif

/////////////////////////////////////////////////////////////////////////////////////////////

#if (USE_SMAP_ENERGYMEASURE == 1)
BOOL CNodeLCDScreenmapFSMInfo_Util::addNode(CNodeLCDScreenmapFSMInfo* m_pThis,CCanNode* pNode,CNodeLCDScreenmapFSMInfo::Type type, CString* pStrError)
{
	if (NULL == pNode || NULL == m_pThis) {
		if (NULL != pStrError) {
			const CString szError("Screenmaps FSM internal error - Wrong parameters");
			*pStrError = szError;
		}
		ASSERT(0);
		return FALSE;
	}

	CNodeLCDSmapEntryFsm* pNewEntry = new CNodeLCDSmapEntryFsm(pNode);
	if (NULL == pNewEntry) {
		if (NULL != pStrError) {
			const CString szError("Screenmaps FSM internal error - Failed to create new entry.");
			*pStrError = szError;
		}
		m_pThis->Clear();
		ASSERT(0);
		return FALSE;
	}

	m_pThis->setType(type);

	if (CNodeLCDScreenmapFSMInfo::TYPE_SCREENMAP_UPDATE_UNITS == type) {

		pNewEntry->addSmapID(CScreenmapID(SCREENMAP_DIMMER));
		pNewEntry->addSmapID(CScreenmapID(SCREENMAP_SWITCH));
		pNewEntry->addSmapID(CScreenmapID(SCREENMAP_CONTROLS));
		pNewEntry->addSmapID(CScreenmapID(SCREENMAP_SENS));
		pNewEntry->addSmapID(CScreenmapID(SCREENMAP_DUOSWITCH));
		pNewEntry->addSmapID(CScreenmapID(SCREENMAP_AUDIO));

		if (pNode->GetSoftwareVersion() == 0x66 && pNode->GetSoftwareVersionRev()>=0x11) {
			pNewEntry->addSmapID(CScreenmapID(SCREENMAP_ENERGYMEASURE));
		}
	}
	else if (CNodeLCDScreenmapFSMInfo::TYPE_SCREENMAP_UPDATE_MYHOME == type) {
	
		pNewEntry->addSmapID(CScreenmapID(SCREENMAP_V65_MYHOME));
		pNewEntry->addSmapID(CScreenmapID(SCREENMAP_V65_MYHOME_SENS));	
	
	}
	else if (CNodeLCDScreenmapFSMInfo::TYPE_SCREENMAP_UPDATE_MAIN == type) {
	
		pNewEntry->addSmapID(CScreenmapID(SCREENMAP_V65_MAIN_MOODS));
		pNewEntry->addSmapID(CScreenmapID(SCREENMAP_V65_MAIN_SENSOR));
		pNewEntry->addSmapID(CScreenmapID(SCREENMAP_V65_MAIN_AUDIO));		
	}

							
	if (FALSE == m_pThis->AddFsmEntry(pNewEntry)) {
		if (NULL != pStrError) {
			const CString szError("Screenmaps FSM internal error - Failed to add new entry");
			*pStrError = szError;
		}
		m_pThis->Clear();
		delete pNewEntry;
		ASSERT(0);
		return FALSE;
	}				
	return TRUE;
}

CScreenmapIDListIterator CNodeLCDScreenmapFSMInfo_Util::createSmapIterator(CNodeLCDScreenmapFSMInfo* m_pThis, BYTE bNodeAddress) {

	static CScreenmapIDList emptyList;

	if (NULL == m_pThis) {
		ASSERT(0);
		return emptyList;
	}

	const int count = m_pThis->m_entries.Count();
	for (int i=0;i<count;++i) {
		CNodeLCDSmapEntryFsm* const pEntry = m_pThis->m_entries.Get(i);
		ASSERT(NULL != pEntry);
		if (NULL != pEntry) {
			CCanNode* const pNode = pEntry->GetNode();
			ASSERT(NULL != pNode);
			if (NULL != pNode && bNodeAddress == pNode->getNodeAddress()) {
				return pEntry->createScreenmapIDIterator();		
			}		
		}
	}
	return emptyList;
}

#endif
/////////////////////////////////////////////////////////////////////////////////////////////