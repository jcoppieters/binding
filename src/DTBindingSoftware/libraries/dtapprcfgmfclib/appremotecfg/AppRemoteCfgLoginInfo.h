#ifndef INCLUDED_APPREMOTECFGLOGININFO_H
#define INCLUDED_APPREMOTECFGLOGININFO_H
/**************************************************************/
#ifndef INCLUDED_APPREMOTECFG_DEF_H
	#include "appremotecfgdef.h"
#endif 
/**************************************************************/
namespace DUOTECNO {
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
class AppRemoteCfgLoginInfo
{
public:
	typedef enum {		
		LOGIN_METHOD_OFFLINE = 0,
		LOGIN_METHOD_USB = 1,
		LOGIN_METHOD_TCPIP = 2
	} LoginMethodType_t;

	AppRemoteCfgLoginInfo();
	virtual ~AppRemoteCfgLoginInfo();
	void clear( );


	// Type Login method 
	void setMethod(LoginMethodType_t method) {
		m_typeLogin = method;
	}
	LoginMethodType_t getMethod() const {
		return m_typeLogin;
	}

	// Password
	void setPassword(const CString& pwd) {
		m_password = pwd;
	}
	const CString& getPassword() const {
		return m_password;
	}

	// Prefered login
	void setPreferedLogin(AppRemoteCfgHostAddress_t t) {
		m_preferedLogin = t;
	}
	AppRemoteCfgHostAddress_t getPreferedLogin() const {
		return m_preferedLogin;
	}

	// Local hostAddress
	void setLocalHostAddress(const CString& s) {
		m_local.m_hostAddress = s;
	}
	const CString& getLocalHostAddress() const {
		return m_local.m_hostAddress;
	}

	// Local Port
	void setLocalPort(int port) {
		m_local.m_port = port;
	}
	int getLocalPort() const {
		return m_local.m_port;
	}

	// Remote hostAddress
	void setRemoteHostAddress(const CString& s) {
		m_remote.m_hostAddress = s;
	}
	const CString& getRemoteHostAddress() const {
		return m_remote.m_hostAddress;
	}

	// Remote Port
	void setRemotePort(int port) {
		m_remote.m_port = port;
	}
	int getRemotePort() const {
		return m_remote.m_port;
	}

	// Runtime proprties: Logical node address.
	void setNodeAddress(int n) {
		m_nodeAddress = n;
	}
	int getNodeAddress( ) const {
		return m_nodeAddress;
	}
	// Runtime property: Physical node address.
	void setPhysicalAddress(unsigned long v) {
		m_physicalAddress = v;
	}
	unsigned long getPhysicalAddress( ) const {
		return m_physicalAddress;
	}

private:
	typedef struct {
		CString m_hostAddress;
		int m_port;	
	} ServerAddress;

	LoginMethodType_t m_typeLogin;
	CString m_password;
	AppRemoteCfgHostAddress_t m_preferedLogin;
	ServerAddress m_local;
	ServerAddress m_remote;

	// Runtime properties: deze worden ingevuld wanneer er een connectie met de server gemaakt werd.
	int m_nodeAddress;							// Runtime node address.
	unsigned long m_physicalAddress;			// Runtime physical address.
};

/**************************************************************/
}; /* end namespace*/
/**************************************************************/
#endif 
/**************************************************************/