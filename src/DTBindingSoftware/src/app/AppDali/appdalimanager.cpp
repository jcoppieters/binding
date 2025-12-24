#include "stdafx.h"
#include "appdalimanager.h"
#include "appdali.h"
/***********************************************************************************/
namespace DUOTECNO {
namespace DALI {
/***********************************************************************************/
CAppDaliManager* CAppDaliManager::m_pThis = NULL;
/***********************************************************************************/

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
CAppDaliManager& CAppDaliManager::getInstance() {
	if (NULL == m_pThis) {
		m_pThis = new CAppDaliManager();		
	}
	ASSERT(NULL != m_pThis);
	return *m_pThis;
}
	
void CAppDaliManager::destroy() {
	if (NULL == m_pThis) {
		return;
	}
	delete m_pThis;
	m_pThis = NULL;
}
/***********************************************************************************/

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
CAppDaliManager::CAppDaliManager() : m_map( ), m_currSelectedNodeAddress(-1) { }

CAppDaliManager::~CAppDaliManager() {
	this->erase( );
}
/***********************************************************************************/
BOOL CAppDaliManager::create(IAppDaliHost_App* parent,int nodeAddress,ITcpSocketHomeClient* pSocket,CAppDali** ppDaliInstance) {
	REQUIRE(nodeAddress>=0 && nodeAddress<=0xff);
	CAppDali* pInstance = NULL;
	if (this->m_map.Lookup(nodeAddress,pInstance)) {		
		*ppDaliInstance = pInstance;
		return FALSE;	// Bestaat al...
	}

	REQUIRE(NULL != parent);
	REQUIRE(NULL != pSocket);
	pInstance = new CAppDali(parent,nodeAddress,pSocket);	
	if (NULL == pInstance) {
		INVARIANT(NULL != pInstance);
		return NULL;
	}
	this->m_map.SetAt(nodeAddress,pInstance);
	ENSURE(NULL != pInstance);
	*ppDaliInstance = pInstance;
	return TRUE;
}

bool CAppDaliManager::remove(int nodeAddress) {
	CAppDali* pInstance = NULL;
	REQUIRE(nodeAddress>=0 && nodeAddress<=0xff);
	if (false == this->m_map.Lookup(nodeAddress,pInstance)) {		
		return false;
	}
	if (FALSE == this->m_map.RemoveKey(nodeAddress)) {
		return false;
	}
	if (NULL == pInstance) {
		ASSERT(NULL != pInstance);
		return false;
	}
	delete pInstance;
	return true;
}

void CAppDaliManager::erase() {
	POSITION pos = this->m_map.GetStartPosition();
	int keyValue;	
	while (pos != NULL) {
		CAppDali* pInstance = NULL;
		this->m_map.GetNextAssoc(pos, keyValue, pInstance);
		if (FALSE == this->m_map.RemoveKey(keyValue)) {
			ASSERT(0);
		}
		if (NULL != pInstance) {
			delete pInstance;
			pInstance = NULL;
		}
	}
	ASSERT(this->m_map.GetSize() == 0);
	this->m_currSelectedNodeAddress = -1;
}

BOOL CAppDaliManager::find(int nodeAddress,CAppDali** ppInstance) {	
	CAppDali* pInstance = NULL;
	REQUIRE(nodeAddress>=0 && nodeAddress<=0xff);
	if (false == this->m_map.Lookup(nodeAddress,pInstance)) {		
		return FALSE;
	}
	INVARIANT(NULL != pInstance);
	*ppInstance = pInstance;
	return TRUE;
}

// Indien er geen zijn moet de lijst opnieuw aangemaakt worden.
int CAppDaliManager::getCount() const {
	return this->m_map.GetCount();
}


// Opvragen van de huidig geselecteerd.
// Indien er nog geen geslecteerd was dan is het de eerste in de lijst.
int CAppDaliManager::getCurrentSelectedNode( ) {
	if (m_map.GetCount() <= 0) {
		this->m_currSelectedNodeAddress = -1;
		return -1;
	}

	CAppDali* pInstance = NULL;
	if (this->m_currSelectedNodeAddress < 0) {			
		int keyValue;
		POSITION pos = this->m_map.GetStartPosition();
		this->m_map.GetNextAssoc(pos, keyValue, pInstance);
		if (NULL == pInstance) {
			return -1;
		}
		ASSERT(keyValue>=0 && keyValue<=0xff);
		this->m_currSelectedNodeAddress = keyValue;
		return this->m_currSelectedNodeAddress;
	}

	if (FALSE == this->find(this->m_currSelectedNodeAddress,&pInstance)) {
		this->m_currSelectedNodeAddress = -1;
		return this->m_currSelectedNodeAddress;
	}
	if (NULL == pInstance) {
		INVARIANT(NULL != pInstance);
		this->m_currSelectedNodeAddress = -1;
		return this->m_currSelectedNodeAddress;
	}
	return this->m_currSelectedNodeAddress;
}


// Instellen van de huige geselecteerd.
// Deze moet aanwezig zijn in de lijst.
BOOL CAppDaliManager::setCurrentSelectedNode(int nodeAddress) {
	if (nodeAddress < 0 || nodeAddress > 0xff) {
		REQUIRE(nodeAddress>=0 && nodeAddress<=0xff);	
		return FALSE;
	}

	CAppDali* pInstance = NULL;
	if (FALSE == this->find(nodeAddress,&pInstance)) {		
		return FALSE;
	}
	if (NULL == pInstance) {
		INVARIANT(NULL != pInstance);
		return FALSE;
	}
	this->m_currSelectedNodeAddress = nodeAddress;
	ENSURE(nodeAddress>=0 && nodeAddress<=0xff);	
	return TRUE;
}

void CAppDaliManager::poll(CAppDaliManager& ref,int intervalSec) {
	if (ref.m_map.GetCount() <= 0) {
		return;
	}

	POSITION pos = ref.m_map.GetStartPosition();
	int keyValue;	
	while (pos != NULL) {
		CAppDali* pInstance = NULL;
		ref.m_map.GetNextAssoc(pos, keyValue, pInstance);
		if (NULL != pInstance) {
			pInstance->pollSystem(intervalSec);
		}
	}
}

/***********************************************************************************/
}; // namespace DUOTECNO {
}; // namespace DALI {
/***********************************************************************************/