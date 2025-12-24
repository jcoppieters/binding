#include "stdafx.h"
#include "BindingSoftware.h"
#include "communicationsettings.h"
/*****************************************************************/
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
CCommunicationSettings::CCommunicationSettings() : m_method(UNKNOWN),
	m_pSettingsUsb(NULL), m_pSettingsTcp(NULL) { }

CCommunicationSettings::~CCommunicationSettings( ) {
	deallocSettingsUsb();
	deallocSettingsTcp();
}

void CCommunicationSettings::deallocSettingsUsb() {
	if (NULL != m_pSettingsUsb) {
		delete m_pSettingsUsb;
		m_pSettingsUsb = NULL;
	}
}

void CCommunicationSettings::deallocSettingsTcp() {
	if (NULL != m_pSettingsTcp) {
		delete m_pSettingsTcp;
		m_pSettingsTcp = NULL;
	}
}

BOOL CCommunicationSettings::set(const CCommunicationSettingsUsb& settings) {
	if (m_pSettingsUsb == &settings) {
		return TRUE;
	}
	m_method = UNKNOWN;
	deallocSettingsUsb();
	if (!settings.valid()) {
		return FALSE;
	}
	m_pSettingsUsb = new CCommunicationSettingsUsb(settings);
	ASSERT(NULL != m_pSettingsUsb);
	m_method = USB;
	return TRUE;	
}

BOOL CCommunicationSettings::set(const CCommunicationSettingsTcp& settings) {
	if (m_pSettingsTcp == &settings) {
		return TRUE;
	}
	m_method = UNKNOWN;
	deallocSettingsTcp();
	if (!settings.valid()) {
		return FALSE;
	}
	m_pSettingsTcp = new CCommunicationSettingsTcp(settings);
	ASSERT(NULL != m_pSettingsTcp);
	m_method = TCPIP;
	return TRUE;	
}

void CCommunicationSettings::clear() {
	deallocSettingsUsb();
	deallocSettingsTcp();
	m_method = UNKNOWN;
}
/*****************************************************************/

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
CCommunicationSettingsUsb::CCommunicationSettingsUsb(int comport) :
m_comport(comport) 
{ 
	ASSERT(valid());
}

CCommunicationSettingsUsb::CCommunicationSettingsUsb(const CCommunicationSettingsUsb& r) :
m_comport(r.m_comport) { 
	ASSERT(valid());
}

BOOL CCommunicationSettingsUsb::valid() const {
	return ((m_comport>0 && m_comport<=255) ? TRUE : FALSE);
}
/*****************************************************************/

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
CCommunicationSettingsTcp::CCommunicationSettingsTcp(ConnectionMethod_t method, const CString& hostName,int port,const CString& pwd, const CString& cloudServerLoginToken) :
	m_method(method), m_hostName(hostName), m_pwd(pwd), m_port(port), m_cloudServerLoginToken(cloudServerLoginToken)  {
	ASSERT(valid());
}

CCommunicationSettingsTcp::CCommunicationSettingsTcp(const CCommunicationSettingsTcp& r) : 
	m_method(r.m_method), m_hostName(r.m_hostName), m_pwd(r.m_pwd), m_port(r.m_port), m_cloudServerLoginToken(r.m_cloudServerLoginToken) {
	ASSERT(valid());
}

BOOL CCommunicationSettingsTcp::valid() const {
	if (m_hostName.IsEmpty() || m_pwd.IsEmpty() || m_port <= 0) {
		return FALSE;
	}
	if (CONNECTION_METHOD_REMOTE_CLOUD == m_method && m_cloudServerLoginToken.IsEmpty()) {
		return FALSE;
	}
	return TRUE;
}
/*****************************************************************/