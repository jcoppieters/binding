// CommunicationLogfile.cpp: implementation of the CCommunicationLogfile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "CommunicationLogfile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
CCommunicationLogfile* CCommunicationLogfile::GetInstance()
{
	static CCommunicationLogfile instance;
	return (&instance);
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CCommunicationLogfile::CCommunicationLogfile()
{
	Clear();
}

CCommunicationLogfile::~CCommunicationLogfile()
{
	Clear();
}

void CCommunicationLogfile::Clear()
{
	m_Strings.RemoveAll();
}

void CCommunicationLogfile::Add(const CString& s)
{
	m_Strings.Add(s);
}

int CCommunicationLogfile::Get(CString& szString)
{
	szString.Empty();
	const int nElements = m_Strings.GetSize();	

	for (int i=0;i<nElements;i++)
	{
		szString += m_Strings.GetAt(i) + "\r\r\n";
	}
	return nElements;
}
