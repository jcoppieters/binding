#include "stdafx.h"
#include "tcpsocketcmn.h"
/*****************************************************************/
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
ISocketMsgObserver::~ISocketMsgObserver( ) { }
/*****************************************************************/

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
CTCPSocketCommSettings::CTCPSocketCommSettings() : m_method(CONNECTION_METHOD_LOCAL), m_hostName(), m_password(), m_port(-1), m_CloudServerToken() { }

CTCPSocketCommSettings::CTCPSocketCommSettings(ConnectionMethod_t method, const CString& hostName,int port, const CString& password, const CString& token) :
	m_method(method), m_hostName(hostName), m_port(port), m_password(password), m_CloudServerToken(token) { }
	
CTCPSocketCommSettings::~CTCPSocketCommSettings() { }

ConnectionMethod_t CTCPSocketCommSettings::getConnectionMethod() const {
	return m_method;
}
const CString& CTCPSocketCommSettings::getHostName() const {
	return m_hostName;
}
const CString& CTCPSocketCommSettings::getPassword() const {
	return m_password;
}
int CTCPSocketCommSettings::getPort() const {
	return m_port;
}
const CString& CTCPSocketCommSettings::getCloudServerToken() const {
	return m_CloudServerToken;
}
/*****************************************************************/