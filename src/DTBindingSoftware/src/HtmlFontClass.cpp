// HtmlFontClass.cpp: implementation of the CHtmlFontClass class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "HtmlFontClass.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHtmlFontClass::CHtmlFontClass(LPCSTR szName,LPCSTR szStyle) :
m_szName(szName), m_szStyle(szStyle)
{

}

CHtmlFontClass::~CHtmlFontClass()
{

}
