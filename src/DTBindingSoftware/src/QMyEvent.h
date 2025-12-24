// QMyEvent.h: interface for the QMyEvent class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QMYEVENT_H__A8A4DA96_CC8F_4C0D_8810_75271FA2104D__INCLUDED_)
#define AFX_QMYEVENT_H__A8A4DA96_CC8F_4C0D_8810_75271FA2104D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct QMyEvent : public QEvent  
{
	LPARAM lParam;
};

#endif // !defined(AFX_QMYEVENT_H__A8A4DA96_CC8F_4C0D_8810_75271FA2104D__INCLUDED_)
