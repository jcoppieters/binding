#include "stdafx.h"
#include "AppCfgBrowseInfo.h"


AppCfgBrowseInfo& AppCfgBrowseInfo::getInstance( ) {
	static AppCfgBrowseInfo _instance;
	return _instance;
}

AppCfgBrowseInfo::AppCfgBrowseInfo( ) : m_mode(MODE_IDLE), m_nodeAddress(-1) { }

void AppCfgBrowseInfo::setMode(Mode_t mode) {
	this->m_mode = mode;
}
AppCfgBrowseInfo::Mode_t AppCfgBrowseInfo::getMode() const {
	return this->m_mode;
}

void AppCfgBrowseInfo::setNodeAddress(int nodeAddress) {
	this->m_nodeAddress = nodeAddress;
}
int AppCfgBrowseInfo::getNodeAddress( ) const {
	return this->m_nodeAddress;
}

