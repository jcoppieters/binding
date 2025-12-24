// LogFileConversionConstruct.h: interface for the CLogFileConversionConstruct class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGFILECONVERSIONCONSTRUCT_H__AA94CCC4_7526_4FF1_A3CD_8DA1C26B88E4__INCLUDED_)
#define AFX_LOGFILECONVERSIONCONSTRUCT_H__AA94CCC4_7526_4FF1_A3CD_8DA1C26B88E4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CLog;
#include "LogFileConversionFromNodes.h"

class CLogFileConversionConstruct : public CLogFileConversionFromNodes  
{
public:
	CLogFileConversionConstruct();
	virtual ~CLogFileConversionConstruct();

public:
	void Report(CLog* const pLog,BOOL fShowStrings = TRUE);
	void Init(FILE* fp);

};

#endif // !defined(AFX_LOGFILECONVERSIONCONSTRUCT_H__AA94CCC4_7526_4FF1_A3CD_8DA1C26B88E4__INCLUDED_)
