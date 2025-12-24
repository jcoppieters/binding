#include "stdafx.h"
#include "AppRemoteCfgServers.h"

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

#if(0)
template <class T>
static void list_append(CList<T*>* dest, const CList<T*>& src) {
	if (0 == dest)
		return;

	POSITION pos = src.GetHeadPosition();
	while (pos) {
		T* const pTemp = src.GetNext(pos);
		ALLEGE(0 != pTemp);
		dest->AddTail(pTemp->clone());
	}
}
#endif 

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

template <class T>
static bool list_remove(CList<T*>* dest, const T& element) {
	if (0 == dest || 0 == dest->GetCount())
		return false;

	for (int i=0; i<dest->GetCount(); ++i) {
		POSITION pos = dest->FindIndex(i);
		if (NULL != pos) {
			T* const pTemp = dest->GetAt(pos);		
			if (0!= pTemp  && pTemp == &element) {			
				dest->RemoveAt(pos);
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
AppRemoteCfgServer::AppRemoteCfgServer() :
	m_friendlyName(), m_hostName(), m_password(), m_hostPort(-1),
	m_secured(false) { }

AppRemoteCfgServer::AppRemoteCfgServer(const AppRemoteCfgServer& r) :
	m_friendlyName(r.m_friendlyName), m_hostName(r.m_hostName), m_password(r.m_password), m_hostPort(r.m_hostPort),
	m_secured(r.m_secured)  { }

AppRemoteCfgServer::~AppRemoteCfgServer() { }

void AppRemoteCfgServer::setFriendlyName(const CString& v) {
	m_friendlyName = v;
}

void AppRemoteCfgServer::setHostName(const CString& v) {
	m_hostName = v;
}

void AppRemoteCfgServer::setPassword(const CString& v) {
	m_password = v;
}

void AppRemoteCfgServer::setHostPort(int port) {
	m_hostPort = port;
}

void AppRemoteCfgServer::setSecured(bool v) {
	m_secured = v;
}

bool AppRemoteCfgServer::equal(const AppRemoteCfgServer& r, const AppRemoteCfgServer& l) {
	return (equal(l, r.getHostName(), r.getHostPort()));
}

bool AppRemoteCfgServer::equal(const AppRemoteCfgServer& server, const CString& hostName, int port) {
	if (hostName.IsEmpty() || port < 0) {
		return false;
	}
	if ((0 == hostName.Compare(server.getHostName())) && (port == server.getHostPort())) {
		return true;
	}
	return false;
}

bool AppRemoteCfgServer::update(AppRemoteCfgServer* dest, const AppRemoteCfgServer& l) {
	dest->setFriendlyName(l.getFriendlyName());
	dest->setHostName(l.getHostName());
	dest->setPassword(l.getPassword());
	dest->setHostPort(l.getHostPort());
	dest->setSecured(l.getSecured());
	return true;
}
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
AppRemoteCfgServers::AppRemoteCfgServers() : 
	m_servers() { }

AppRemoteCfgServers::~AppRemoteCfgServers() {
	deallocate();
}

void AppRemoteCfgServers::clear() {
	deallocate();
}

void AppRemoteCfgServers::deallocate() {
	list_deleteAll(&this->m_servers);	
}

void AppRemoteCfgServers::add(AppRemoteCfgServer* item) {
	m_servers.AddTail(item);
}

int AppRemoteCfgServers::getCount() const {
	return m_servers.GetCount();
}

bool AppRemoteCfgServers::get(int index, AppRemoteCfgServer** ppItem) const {
	return list_getItem<AppRemoteCfgServer>(this->m_servers, index, ppItem);
}

bool AppRemoteCfgServers::remove(const AppRemoteCfgServer& item) {
	return list_remove<AppRemoteCfgServer>(&this->m_servers, item);
}

bool AppRemoteCfgServers::add(AppRemoteCfgServers* dest, const AppRemoteCfgServer& r) {
	if (0 == dest) {
		REQUIRE(0 != dest);
	}

	// Een item is hetzelfde wnr HostName & poort hetzelfde is.
	// Indien hetzelfde dan enkel de velden updaten...
	AppRemoteCfgServer* pEqualItem = 0;
	if (findEqual(*dest,r,&pEqualItem)) {	
		AppRemoteCfgServer::update(pEqualItem,r);
		return true;
	}
	
	// Indien het item niet gelijk is dan een nieuwe aanmaken.
	AppRemoteCfgServer* const pNewItem = new AppRemoteCfgServer(r);
	if (0 == pNewItem) {
		return false;
	}
	dest->add(pNewItem);
	return true;
}

bool AppRemoteCfgServers::findEqual(const AppRemoteCfgServers& servers, const AppRemoteCfgServer& l, AppRemoteCfgServer** ppDest) {
	const int count = servers.getCount();
	for (int i=0;i<count;++i) {
		AppRemoteCfgServer* pServer = 0;
		if (servers.get(i,&pServer)) {
			if (AppRemoteCfgServer::equal(l,*pServer)) {
				*ppDest = pServer;
				return true;
			}
		}
	}
	return false;
}

bool AppRemoteCfgServers::remove(AppRemoteCfgServers* dest, const CString& hostName, int port) {
	bool result = false;
	// const int count = dest->getCount();
	for (int i = 0; i<dest->getCount(); ++i) {
		AppRemoteCfgServer* pServer = 0;
		if (dest->get(i, &pServer)) {
			if (AppRemoteCfgServer::equal(*pServer,hostName,port)) {
				dest->remove(*pServer);				
				remove(dest,hostName,port);		// Oppassen: Recursieve aanroep: Alle items wissen...
				result = true;
				return true;
			}
		}
	}
	return result;
}
/**************************************************************/
}; /*  namespace DUOTECNO { */
/**************************************************************/