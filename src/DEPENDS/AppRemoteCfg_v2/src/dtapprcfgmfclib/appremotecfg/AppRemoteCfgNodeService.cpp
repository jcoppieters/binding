#include "stdafx.h"
#include "AppRemoteCfgNodeService.h"

#define REQUIRE			ASSERT
#define ALLEGE			ASSERT
/**************************************************************/
namespace DUOTECNO {
/**************************************************************/

template <class T>
static void list_deleteAll(CList<T*>* dest) {
	if (0 == dest)
		return;

	while (!dest->IsEmpty()) {
		T* const p = reinterpret_cast<T*>(dest->RemoveHead());
		ASSERT(p);
		delete p;
	}
	ASSERT(dest->GetCount() == 0);
}

template <class T>
bool list_getItem(const CList<T*>& src, int idx, T** ppFile) {
	const int count = src.GetCount();
	if (idx >= count) {
		REQUIRE(idx<count);
		return false;
	}

	POSITION pos = src.GetHeadPosition();
	for (int i = 0; i < count; i++)
	{
		T* pItem = src.GetNext(pos);
		if (idx == i) {
			*ppFile = pItem;
			return true;
		}
	}
	return false;
}

/**************************************************************/
/**************************************************************/
/**************************************************************/
AppRemoteCfgNodes::AppRemoteCfgNodes() : m_nodes() {}
AppRemoteCfgNodes::~AppRemoteCfgNodes() {
	deallocate();
}

void AppRemoteCfgNodes::deallocate() {
	list_deleteAll(&this->m_nodes);	
}

int AppRemoteCfgNodes::getCount() const {
	return m_nodes.GetCount();
}

bool AppRemoteCfgNodes::getNode(int idx,AppRemoteCfgNode** ppService) const{
	return list_getItem(m_nodes,idx,ppService);
}

bool AppRemoteCfgNodes::findNode(int nodeAddress,AppRemoteCfgNode** ppService) {
	const int count = m_nodes.GetCount();
	for (int i=0;i<count;++i) {
		AppRemoteCfgNode* p = 0;
		if (list_getItem(m_nodes,i,&p)) {
			if (p->getNodeAddress() == nodeAddress) {
				*ppService = p;
				return true;
			}
		}
	}
	return false;
}

bool AppRemoteCfgNodes::findNode(const CString& strMacAddress, AppRemoteCfgNode** ppService) {
	if (strMacAddress.IsEmpty()) {
		return false;
	}
	const int count = m_nodes.GetCount();
	for (int i = 0; i<count; ++i) {
		AppRemoteCfgNode* p = 0;
		if (list_getItem(m_nodes, i, &p)) {
			if (0 == p->getMacAddress().Compare(strMacAddress)) {			
				*ppService = p;
				return true;
			}
		}
	}
	return false;
}

bool AppRemoteCfgNodes::addNode(AppRemoteCfgNode* pNode) {
	if (0 == pNode) {
		ASSERT(0 != pNode);
		return false;
	}

	// Controle node address...
	const int nodeAddress = pNode->getNodeAddress();
	if (nodeAddress<0 || nodeAddress>0xff) {
		ASSERT(nodeAddress>=0 && nodeAddress<=0xff);
		return false;
	}

	if (NULL != m_nodes.Find(pNode)) {
		// Node is reeds aanwezig in de lijst...
		return TRUE;
	}
	
	// Verwijderen van een node indien er reeds ene bestaat voor dit node address...
	deleteNode(nodeAddress);
	// Toevoegen van de node...
	m_nodes.AddTail(pNode);	
	return true;
}

bool AppRemoteCfgNodes::deleteNode(int nodeAddress) {
	const int count = m_nodes.GetCount();
	for (int i=0; i<count; ++i) {
		POSITION pos = m_nodes.FindIndex(i);
		if (NULL != pos) {
			AppRemoteCfgNode* const pTemp = m_nodes.GetAt(pos);		
			if ((0!= pTemp)  && (nodeAddress == pTemp->getNodeAddress())) {			
				m_nodes.RemoveAt(pos);
				delete pTemp;
				return true;
			}
		}
	}
	return false;
}
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
AppRemoteCfgNode::AppRemoteCfgNode(int nodeAddress) : 
	m_nodeAddress(nodeAddress), m_physicalAddress(0xffffff), 
	m_macAddress(), m_hostnameLocal(), m_hostnameRemote(), m_password(),
	m_services() 
{ 		
}

AppRemoteCfgNode::~AppRemoteCfgNode() {
	deallocate();	
}

void AppRemoteCfgNode::deallocate() {
	list_deleteAll(&this->m_services);	
}

void AppRemoteCfgNode::setNodeAddress(int nodeAddress) {
	if (nodeAddress >= 0x00 && nodeAddress <= 0xff) {
		m_nodeAddress = nodeAddress;
	}
	else {
		m_nodeAddress = 0xFF;
	}
}
void AppRemoteCfgNode::setPhysicalAddress(int v) {
	if (v >= 0 && v <= 0xffffff) {
		m_physicalAddress = v;
	}
	else {
		m_physicalAddress = 0xffffff;
	}
}

// Opvragen van de services...
int AppRemoteCfgNode::getCount() const {
	return m_services.GetCount();
}

bool AppRemoteCfgNode::getService(int idx,AppRemoteCfgNodeService** ppService) const {
	return list_getItem(m_services,idx,ppService);
}

bool AppRemoteCfgNode::findService(AppRemoteCfgService_t type,AppRemoteCfgNodeService** ppService) {
	const int count = m_services.GetCount();
	for (int i=0;i<count;++i) {
		AppRemoteCfgNodeService* p = 0;
		if (list_getItem(m_services,i,&p)) {
			if (p->getType() == type) {
				*ppService = p;
				return true;
			}
		}
	}
	return false;
}

bool AppRemoteCfgNode::addService(AppRemoteCfgNodeService* pService) {
	if (0 == pService) {
		ASSERT(0 != pService);
		return false;
	}

	// Controle type.
	const AppRemoteCfgService_t type = pService->getType();
	if (SERVICE_UNKNOWN == type) {
		return false;
	}

	if (NULL != m_services.Find(pService)) {
		// Node is reeds aanwezig in de lijst...
		return TRUE;
	}

	// Verwijderen van de oude service met zelfde type.
	deleteService(type);

	// Toevoegen van de service.
	m_services.AddTail(pService);
	return true;
}

bool AppRemoteCfgNode::deleteService(AppRemoteCfgService_t type) {
	const int count = m_services.GetCount();
	for (int i=0; i<count; ++i) {
		POSITION pos = m_services.FindIndex(i);
		if (NULL != pos) {
			AppRemoteCfgNodeService* const pTemp = m_services.GetAt(pos);		
			if ((0!= pTemp) && (type == pTemp->getType())) {			
				m_services.RemoveAt(pos);
				delete pTemp;
				return true;
			}
		}
	}
	return false;
}

/**************************************************************/
/**************************************************************/
/**************************************************************/
AppRemoteCfgNodeService::AppRemoteCfgNodeService(AppRemoteCfgService_t type) :
	m_type(type), m_portLocal(-1), m_portRemote(-1) { }
AppRemoteCfgNodeService::~AppRemoteCfgNodeService() {}

void AppRemoteCfgNodeService::setType(AppRemoteCfgService_t t) {
	m_type = t;	
}
/**************************************************************/
}; /* namespace DUOTECNO */
/**************************************************************/