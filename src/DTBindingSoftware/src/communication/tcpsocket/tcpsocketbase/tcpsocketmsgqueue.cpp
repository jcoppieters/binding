#include "stdafx.h"
#include "tcpsocketmsgqueue.h"
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
CTCPSocketMsgQueue::CTCPSocketMsgQueue(int maxItems) : 
	m_maxItems(maxItems), m_list() { }

CTCPSocketMsgQueue::~CTCPSocketMsgQueue() {
	this->clear();
}

void CTCPSocketMsgQueue::clear() {
	while(!m_list.isEmpty()) {
		CTCPSocketCommFrame* const item = m_list.removeHead();
		ASSERT(NULL != item);
		if (NULL != item) {
			delete item;			
		}
	}
	ASSERT(m_list.isEmpty());
}

BOOL CTCPSocketMsgQueue::add(CTCPSocketCommFrame* newMsg) {
	ASSERT(NULL != newMsg);
	if (m_list.getCount()>=m_maxItems) {
		ASSERT(m_list.getCount()<m_maxItems);
		return FALSE;
	}
	m_list.addTail(newMsg);
	return TRUE;
}

BOOL CTCPSocketMsgQueue::get(CTCPSocketCommFrame* ppNewMsg) {
	if (m_list.isEmpty()) {
		return FALSE;
	}
	CTCPSocketCommFrame* const item = m_list.removeHead();
	if (NULL == item) {
		ASSERT(NULL != item);
		return FALSE;
	}

	*ppNewMsg = *item;
	delete item;
	return TRUE;	
}
