// MyException.cpp: implementation of the CMyException class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "MyException.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMyException::CMyException(UINT nResourceID)
{
	CString str;

	if (!strClassError.LoadString(IDS_BASE_EXCEPTION))
		strClassError = "";

	if (!str.LoadString(nResourceID))
		str = "";

	szErrorMsg.Format("%s\n%s",strClassError,str);
}

CMyException::~CMyException()
{
}

//////////////////////////////////////////////////////////////////////

CExceptionBindingInterface::CExceptionBindingInterface(UINT nResourceID)
{
	CString str;
	if (!strClassError.LoadString(IDS_BINDINGINTERFACE_EXCEPTION))
		strClassError = "";

	if (!str.LoadString(nResourceID))
		str = "";

	szErrorMsg.Format("%s\n%s",strClassError,str);
}
CExceptionBindingInterface::~CExceptionBindingInterface()
{

}

//////////////////////////////////////////////////////////////////////
CExceptionNodeDatabaseInterface::CExceptionNodeDatabaseInterface(UINT nResourceID)
{
	CString str;
	if (!strClassError.LoadString(IDS_NODEDATABASEINTERFACE_EXCEPTION))
		strClassError = "";

	if (!str.LoadString(nResourceID))
		str = "";

	szErrorMsg.Format("%s\n%s",strClassError,str);
}

CExceptionNodeDatabaseInterface::~CExceptionNodeDatabaseInterface()
{

}

