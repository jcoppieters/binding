#ifndef INCLUDED_APPREMOTECFGNODESERVICE_H
#define INCLUDED_APPREMOTECFGNODESERVICE_H
/**************************************************************/
// Sedert V1.8
/**************************************************************/
#ifndef INCLUDED_APPREMOTECFG_DEF_H
	#include "appremotecfgdef.h"
#endif 
/**************************************************************/
namespace DUOTECNO {
/**************************************************************/
class AppRemoteCfgNode;
class AppRemoteCfgNodeService;
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
class AppRemoteCfgNodes
{
public: 
	AppRemoteCfgNodes();
	~AppRemoteCfgNodes();

	void clear( ) { this->deallocate(); }
	int getCount() const;
	bool getNode(int idx,AppRemoteCfgNode** ppService) const;
	bool findNode(int nodeAddress,AppRemoteCfgNode** ppService);
	bool findNode(const CString& strMacAddress, AppRemoteCfgNode** ppService);	// Sedert V2.3
	bool addNode(AppRemoteCfgNode* pNode);		// Moet een geldig node address hebben.
	bool deleteNode(int nodeAddress);

protected:
	void deallocate();

private:
	CList<AppRemoteCfgNode*> m_nodes;		// Lijst van nodes...
};

/**************************************************************/
/**************************************************************/
/**************************************************************/
class AppRemoteCfgNode
{
public: 
	AppRemoteCfgNode(int nodeAddress = 0xff);
	~AppRemoteCfgNode();

	// nodeAddress
	void setNodeAddress(int nodeAddress);
	int getNodeAddress() const {
		return m_nodeAddress;
	}

	// physical Address
	void setPhysicalAddress(int v);
	unsigned long getPhysicalAddress() const {
		return m_physicalAddress;
	}

	// mac-address	--> Todo: controle of deze geldig is ??
	void setMacAddress(const CString& v) {
		m_macAddress = v;
	}
	const CString& getMacAddress() const {
		return m_macAddress;
	}

	// Host name local.
	void setHostnameLocal(const CString& v) {
		m_hostnameLocal = v;
	}
	const CString& getHostnameLocal() const {
		return m_hostnameLocal;
	}

	// Host name remote.
	void setHostnameRemote(const CString& v) {
		m_hostnameRemote = v;
	}
	const CString& getHostnameRemote() const {
		return m_hostnameRemote;
	}

	// Password.
	void setPassword(const CString& v) {
		m_password = v;
	}
	const CString& getPassword() const {
		return m_password;
	}

	// Opvragen van de services...
	int getCount() const;
	bool getService(int idx,AppRemoteCfgNodeService** ppService) const;
	bool findService(AppRemoteCfgService_t type,AppRemoteCfgNodeService** ppService);
	bool addService(AppRemoteCfgNodeService* pService);		// Moet een geldig type hebben.
	bool deleteService(AppRemoteCfgService_t type);

protected:
	void deallocate();

private:
	int m_nodeAddress;								// Key...
	unsigned long m_physicalAddress;				
	CString m_macAddress;							// "xx:xx:xx:xx:xx:xx"
	CString m_hostnameLocal;						// local ip-adres.
	CString m_hostnameRemote;						// remote ip-adres / dns.
	CString m_password;								// password van de node. (evt later nog overschrijven door de service.)
	CList<AppRemoteCfgNodeService*> m_services;		// Lijst van beschikbare services...
};
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
class AppRemoteCfgNodeService
{
public: 
	AppRemoteCfgNodeService(AppRemoteCfgService_t type = SERVICE_UNKNOWN);
	~AppRemoteCfgNodeService();

	// type
	void setType(AppRemoteCfgService_t t);
	const AppRemoteCfgService_t getType() const {
		return m_type;
	}

	// Port local.
	int getPortLocal() const {
		return m_portLocal;
	}
	void setPortLocal(int port) {
		m_portLocal = port;
	}

	// Port remote.
	int getPortRemote() const {
		return m_portRemote;
	}
	void setPortRemote(int port) {
		m_portRemote = port;
	}

private:
	AppRemoteCfgService_t m_type;			// Key
	int m_portLocal;
	int m_portRemote;
};
/**************************************************************/
}; /* namespace DUOTECNO */
/**************************************************************/
#endif 
/**************************************************************/