// ConfigExternalHelper.h: interface for the CConfigExternalHelper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONFIGEXTERNALHELPER_H__11B94B60_7982_4A67_BAD7_9379289AC9AD__INCLUDED_)
#define AFX_CONFIGEXTERNALHELPER_H__11B94B60_7982_4A67_BAD7_9379289AC9AD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct CConfigExternalHelper  
{
	static bool parseString(
		const char* szInput,
		char* szOutput,
		int maxLength);

	static bool stripEndOfString(
		char* szString,
		int maxLength,
		char ch);

	static bool toLowerString(
		const char* szInput,
		char* szOutput,
		int maxLength);
};

#endif // !defined(AFX_CONFIGEXTERNALHELPER_H__11B94B60_7982_4A67_BAD7_9379289AC9AD__INCLUDED_)
