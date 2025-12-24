// LogFileConversionFromNodesErrors.h: interface for the CLogFileConversionFromNodesErrors class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGFILECONVERSIONFROMNODESERRORS_H__B4491ED7_2912_4226_9237_3313E2470C7F__INCLUDED_)
#define AFX_LOGFILECONVERSIONFROMNODESERRORS_H__B4491ED7_2912_4226_9237_3313E2470C7F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "LogFileConversionFromNodes.h"


class CLogFileConversionFromNodesErrors : public CLogFileConversionFromNodes  
{
public:
	CLogFileConversionFromNodesErrors();
	virtual ~CLogFileConversionFromNodesErrors();

protected:
	virtual BOOL _Start();
	virtual BOOL _Stop();
};


#endif // !defined(AFX_LOGFILECONVERSIONFROMNODESERRORS_H__B4491ED7_2912_4226_9237_3313E2470C7F__INCLUDED_)
