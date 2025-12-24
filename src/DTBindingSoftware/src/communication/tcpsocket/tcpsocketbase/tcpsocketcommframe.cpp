#include "stdafx.h"
#include "tcpsocketcommframe.h"
/*****************************************************************/
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/

// m_cmd --> MODEM_RXMESSAGE    0x04, WRITE = 0x00
CTCPSocketCommFrame::CTCPSocketCommFrame() : m_cmd(0x04),
	m_msgCode(0xff), m_nodeAddress(0xff), m_unitAddress(0xff),m_length(0),m_pbData(NULL) { }

CTCPSocketCommFrame::CTCPSocketCommFrame(int msgCode,int nodeAddress,int unitAddress,int length,const BYTE* pbData,type_t type) : 
	m_cmd(READ == type ? 0x04 : 0x00), m_msgCode(msgCode), m_nodeAddress(nodeAddress), m_unitAddress(unitAddress),m_length(-1),
	m_pbData(NULL)
{	
	if (msgCode<0 || msgCode>0xff) {
		ASSERT(msgCode>=0 && msgCode<=0xff);
		m_msgCode = 0xff;
	}
	if (nodeAddress<0 || nodeAddress>0xff) {
		ASSERT(nodeAddress>=0 && nodeAddress<=0xff);
		m_nodeAddress = 0xff;
	}
	if (unitAddress<0 || unitAddress>0xff) {
		ASSERT(unitAddress>=0 && unitAddress<=0xff);
		m_unitAddress = 0xff;
	}
	if (length<0 || length>0xff) {
		ASSERT(length>=0 && length<=0xff);		
	}
	setData(length,pbData);
}

CTCPSocketCommFrame::~CTCPSocketCommFrame() {
	if (NULL != m_pbData) {
		delete [] m_pbData;
		m_pbData = NULL;
	}
}

void CTCPSocketCommFrame::setData(int length,const BYTE* pbData) {
	ASSERT(this->m_pbData == NULL);
	if (length<0 || NULL == pbData) {
		this->m_length = 0;
		return;
	}

	this->m_length = length;
	m_pbData = new BYTE[length];
	if (NULL != m_pbData) {
		for (int i=0;i<length;++i) {
			m_pbData[i] = pbData[i];
		}		
	}	
}

BYTE CTCPSocketCommFrame::GetModemCmd() const {
	return m_cmd;
}
BYTE CTCPSocketCommFrame::GetMsgCode() const {
	return m_msgCode;
}
BYTE CTCPSocketCommFrame::GetNodeAddress() const {
	return m_nodeAddress;
}
BYTE CTCPSocketCommFrame::GetUnitAddress() const {
	return m_unitAddress;
}
BYTE CTCPSocketCommFrame::GetDataBYTE(BYTE i) const {
	if ((i>=m_length) || (NULL == m_pbData)) {
		ASSERT(i<m_length);
		ASSERT(NULL != m_pbData);
		return 0xff;
	}
	return m_pbData[i];
}
BYTE CTCPSocketCommFrame::GetCmdDataBYTE(BYTE i) const {
	return 0xff;	// specifiek voor de modem.
}
BYTE CTCPSocketCommFrame::GetMessageLength(void) const {
	return m_length;
}

CTCPSocketCommFrame& CTCPSocketCommFrame::operator=(const CTCPSocketCommFrame& item) {
	if (this == &item) {
		return *this;
	}
	if (NULL != this->m_pbData) {
		delete this->m_pbData;
		this->m_pbData = NULL;
	}

	// copieren van de velden...
	this->m_cmd = item.m_cmd;
	this->m_msgCode = item.m_msgCode;
	this->m_nodeAddress = item.m_nodeAddress;
	this->m_unitAddress = item.m_unitAddress;
	setData(item.m_length,item.m_pbData);

	// return waarde.
	return *this;
}

bool CTCPSocketCommFrame::operator==(CTCPSocketCommFrame& r) const {
	if (this == &r) {
		return true;
	}
	if (this->m_cmd != r.m_cmd) {
		return false;
	}
	if (this->m_msgCode != r.m_msgCode) {
		return false;
	}
	if (this->m_nodeAddress != r.m_nodeAddress) {
		return false;
	}
	if (this->m_unitAddress != r.m_unitAddress) {
		return false;
	}
	if (this->m_length != r.m_length) {
		return false;
	}
	for (int i=0;i<m_length;++i) {
		if (this->m_pbData[i] != r.m_pbData[i]) {
			return false;
		}
	}
	return true;
}

/*****************************************************************/