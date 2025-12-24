// BindingConvertHelper.h: interface for the CBindingConvertHelper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGCONVERTHELPER_H__6A476465_7E40_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_BINDINGCONVERTHELPER_H__6A476465_7E40_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BindingConstants.h"

class CBindingConvertHelper  
{
public:
	CBindingConvertHelper();
	virtual ~CBindingConvertHelper();

public:
	BOOL ConvertOperatorType2Char(const OperatorType Type,char *c);
	BOOL ParseLogOperators(CString szData,char *cData,int OperatorNr=0);
	BOOL ParseInputs(CString szData,char *cData,unsigned short *nId,int nUnitNr=0);
	BOOL ParseTOperators(CString szData,char *cData,unsigned short *nId,BOOL *fInverse,int OperatorNr=0);
	BOOL ParseOutputs(CString szData,char *cData,unsigned short *nId,int nUnitNr=0);
	int ParseNInputs(CString szData);
	int ParseNrOfOperators(CString szData);
	int ParseNOutputs(CString szData);

	void DebugInfo(CString szString);
};

#endif // !defined(AFX_BINDINGCONVERTHELPER_H__6A476465_7E40_11D8_B865_0050BAC412B1__INCLUDED_)
