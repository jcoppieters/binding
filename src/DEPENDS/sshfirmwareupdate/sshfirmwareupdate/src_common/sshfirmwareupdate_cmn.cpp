#include "stdafx.h"
#include "sshfirmwareupdate_cmn.h"
/***************************************************************************************************/
#define ASSERT			assert
/***************************************************************************************************/
namespace DUOTECNO {
/***************************************************************************************************/
SshMessage::SshMessage() : 
	m_type(TYPE_MSG_EMPTY), m_status(TASK_STATUS_UNKNOWN), m_szMsg(NULL) { }

SshMessage::SshMessage(msgType_t type,const char* msg) : 
	m_type(type), m_status(TASK_STATUS_UNKNOWN), m_szMsg(NULL) {
	this->setMsg(msg);
}
SshMessage::SshMessage(SshTaskStatus_t status, const char* msg) : 
	m_type(TYPE_MSG_STATUS), m_status(status), m_szMsg(NULL) {
	this->setMsg(msg);
}
SshMessage::SshMessage(const SshMessage& r) : 
	m_type(r.m_type), m_status(r.m_status), m_szMsg(NULL) {
	this->setMsg(r.m_szMsg);
	
}
SshMessage::~SshMessage( ) {
	dealloc();
}

const SshMessage& SshMessage::operator=(const SshMessage& r) {
	if (this==&r) {
		return *this;
	}
	dealloc();
	this->m_status = r.m_status;
	this->m_type = r.m_type;
	this->setMsg(r.m_szMsg);
	return *this;
}

void SshMessage::setMsg(const char* msg) {
	ASSERT(this->m_szMsg == NULL);
	if (NULL != msg) {
		int length = strlen(msg);
		if (length>0) {
			length += 1;
			this->m_szMsg = new char[length];
			memcpy(this->m_szMsg,msg,length);		
		}
	}
}

void SshMessage::dealloc() {
	if (NULL != m_szMsg) {
		delete [] m_szMsg;
		m_szMsg = NULL;
	}
}
/***************************************************************************************************/
}; /* namespace DUOTECNO { */
/***************************************************************************************************/