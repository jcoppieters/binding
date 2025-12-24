// CommunicationLogfile.h: interface for the CCommunicationLogfile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMMUNICATIONLOGFILE_H__D85A9F2E_271D_447E_9C8E_A7E7E6652EDE__INCLUDED_)
#define AFX_COMMUNICATIONLOGFILE_H__D85A9F2E_271D_447E_9C8E_A7E7E6652EDE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CCommunicationLogfile  
{
protected:
	CCommunicationLogfile();
	CStringArray m_Strings;

public:
	static CCommunicationLogfile* GetInstance();
	virtual ~CCommunicationLogfile();

	void Add(const CString& s);
	void Clear();

	int Get(CString& szString);
};

#endif // !defined(AFX_COMMUNICATIONLOGFILE_H__D85A9F2E_271D_447E_9C8E_A7E7E6652EDE__INCLUDED_)
