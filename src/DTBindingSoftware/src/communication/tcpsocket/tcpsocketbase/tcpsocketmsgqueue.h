#ifndef INCLUDED_TCPSOCKET_TCPSOCKETMSGQUEUE_H
#define INCLUDED_TCPSOCKET_TCPSOCKETMSGQUEUE_H
/*****************************************************************/
#include "utils/mfc/TPtrList.h"
/*****************************************************************/
class CTCPSocketCommFrame;
/*****************************************************************/
class CTCPSocketMsgQueue
{
public:
	CTCPSocketMsgQueue(int maxItems);
	virtual ~CTCPSocketMsgQueue();

	// Alle berichten wissen.
	void clear();

	// Nieuwe toeveoegen.
	BOOL add(CTCPSocketCommFrame* newMsg);

	// Nieuwe opvragen, wordt gecopieerd, deze uit de queue wordt verwijderd.
	BOOL get(CTCPSocketCommFrame* ppNewMsg);

private:
	const int m_maxItems;
	DUOTECNO::MFC_HELPER::TPtrList<CTCPSocketCommFrame> m_list;
};
/*****************************************************************/
#endif
/*****************************************************************/