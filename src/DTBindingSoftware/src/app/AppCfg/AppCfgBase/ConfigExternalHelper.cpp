// ConfigExternalHelper.cpp: implementation of the CConfigExternalHelper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ConfigExternalHelper.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

///////////////////////////////////////////////////////////////////////
bool CConfigExternalHelper::
parseString(
	const char* szInput,
	char* szOutput,
	int maxLength)
{
	bool result = false;
	int j = 0;
	char ch = '\0';

	for(j=0;(j<(maxLength)) && ((ch=szInput[j])!='\0') && (ch!='\n'); j++ )
	{
		szOutput[j] = (char) ch;
		result = true;
	}
  
	szOutput[j] = '\0';	
	return result;
}

bool CConfigExternalHelper::
stripEndOfString(
	char* szString,
	int maxLength,
	char ch)
{
	ASSERT( szString );

	bool result = false;
	const char* const pDest = strrchr(szString,ch);

	if (pDest != 0)
	{
		const int index = pDest - &szString[0];

		if (index >=0 && index < maxLength)
		{
			szString[ index ] = '\0';
			result = true;
		}
	}
	return result;
}

bool CConfigExternalHelper::toLowerString(
	const char* szInput,
	char* szOutput,
	int maxLength)
{
	bool result = false;
	int j = 0;
	char ch = '\0';

	for(j=0;(j<(maxLength)) && ((ch=szInput[j])!='\0') && (ch!='\n'); j++ )
	{
		int temp = tolower(ch);					// CHANGES_MVS_2008_WARNINGS
		ASSERT( temp >= 0 && temp <= 127);		// CHANGES_MVS_2008_WARNINGS

		szOutput[j] = static_cast<char>(temp);	// CHANGES_MVS_2008_WARNINGS 
		result = true;
	}
  
	szOutput[j] = '\0';	
	return result;	
}





///////////////////////////////////////////////////////////////////////
