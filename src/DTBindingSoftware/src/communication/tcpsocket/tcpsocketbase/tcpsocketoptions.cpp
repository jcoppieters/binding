#include "stdafx.h"
#include "tcpsocketoptions.h"
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
CTCPSocketOptions::CTCPSocketOptions( ) : m_flags(0) {
	m_flags += FLAG_OPTION_LOGIN_SECURED;
	m_flags += FLAG_OPTION_LOGIN_SUBSCRIBE_CANMSG;
}
	
void CTCPSocketOptions::setOptionLoginSecure(BOOL flag) {
	if (flag) {
		m_flags |= FLAG_OPTION_LOGIN_SECURED;
	}
	else {
		m_flags &= ~FLAG_OPTION_LOGIN_SECURED;
	}
}

BOOL CTCPSocketOptions::getOptionLoginSecure() const {
	return ((FLAG_OPTION_LOGIN_SECURED == (m_flags & FLAG_OPTION_LOGIN_SECURED)) ? TRUE : FALSE);
}

void CTCPSocketOptions::setOptionLoginSubscribeCanMsg(BOOL flag) {
	if (flag) {
		m_flags |= FLAG_OPTION_LOGIN_SUBSCRIBE_CANMSG;
	}
	else {
		m_flags &= ~FLAG_OPTION_LOGIN_SUBSCRIBE_CANMSG;
	}
}

BOOL CTCPSocketOptions::getOptionLoginSubscribeCanMsg( ) const {
	return ((FLAG_OPTION_LOGIN_SUBSCRIBE_CANMSG == (m_flags & FLAG_OPTION_LOGIN_SUBSCRIBE_CANMSG)) ? TRUE : FALSE);
}

void CTCPSocketOptions::setOptionLoginSubscribeDALIMsg(BOOL flag) {
	if (flag) {
		m_flags |= FLAG_OPTION_LOGIN_SUBSCRIBE_DALIMSG;
	}
	else {
		m_flags &= ~FLAG_OPTION_LOGIN_SUBSCRIBE_DALIMSG;
	}
}
BOOL CTCPSocketOptions::getOptionLoginSubscribeDALIMsg( ) const {
	return ((FLAG_OPTION_LOGIN_SUBSCRIBE_DALIMSG == (m_flags & FLAG_OPTION_LOGIN_SUBSCRIBE_DALIMSG)) ? TRUE : FALSE);
}

void CTCPSocketOptions::setOptionLoginAllowSlaveConnect(BOOL flag) {
	if (flag) {
		m_flags |= FLAG_OPTION_LOGIN_ALLOW_SLAVE;
	}
	else {
		m_flags &= ~FLAG_OPTION_LOGIN_ALLOW_SLAVE;
	}
}
BOOL CTCPSocketOptions::getOptionLoginAllowSlaveConnect( ) const {
	return ((FLAG_OPTION_LOGIN_ALLOW_SLAVE == (m_flags & FLAG_OPTION_LOGIN_ALLOW_SLAVE)) ? TRUE : FALSE);
}

/*****************************************************************/
