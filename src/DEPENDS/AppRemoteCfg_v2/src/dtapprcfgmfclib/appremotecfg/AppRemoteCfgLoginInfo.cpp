#include "stdafx.h"
#include "AppRemoteCfgLoginInfo.h"

/**************************************************************/
#define REQUIRE				ASSERT
/**************************************************************/
namespace DUOTECNO {
/**************************************************************/
AppRemoteCfgLoginInfo::AppRemoteCfgLoginInfo() : 
	m_typeLogin(LOGIN_METHOD_OFFLINE), m_password() , m_preferedLogin(HOSTADDRESS_TYPE_LOCAL),
	m_nodeAddress(0xff), m_physicalAddress(0xffffff)
{

	m_local.m_port = -1;
	m_local.m_hostAddress.Empty();

	m_remote.m_port = -1;
	m_remote.m_hostAddress.Empty();
}

AppRemoteCfgLoginInfo::~AppRemoteCfgLoginInfo() {

}

void AppRemoteCfgLoginInfo::clear() {
	m_typeLogin = LOGIN_METHOD_OFFLINE;
	m_password.Empty(); 
	m_preferedLogin = HOSTADDRESS_TYPE_LOCAL;
	m_nodeAddress = 0xff; 
	m_physicalAddress = 0xffffff;
	m_local.m_port = -1;
	m_local.m_hostAddress.Empty();
	m_remote.m_port = -1;
	m_remote.m_hostAddress.Empty();
}
/**************************************************************/
}; /* end namespace*/
/**************************************************************/