// StringParser.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "StringParser.h"

#include <iostream>
#include <conio.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// The one and only application object

CWinApp theApp;

using namespace std;


enum { MAX_TESTSTRINGS = 5 };
static const char* szTestStrings[ MAX_TESTSTRINGS ] =
{
	"naam 1",
	"naam 1234",
	"naam 2535",
	"1234 naam",
	"abcdefghijklmn"
};


void parseString(
	const char* szInput,
	char* szOutput,
	int maxLength)
{
	int j = 0;
	char ch = '\0';

	for(j=0;(j<(maxLength)) && ((ch=szInput[j])!='\0') && (ch!='\n'); j++ )
	{
		szOutput[j] = (char)ch;
	}
  
	szOutput[j] = '\0';	
}


// 

void testcode_1()
{
	char szTempBuffer[ 255 ];

	for (int i=0;i<MAX_TESTSTRINGS;i++)
	{
		bool result = false;

		if (sscanf(szTestStrings[i],"%s",szTempBuffer) > 0)
		{
			if (strcmp(szTestStrings[i],szTempBuffer) == 0)
			{
				result = true;
			}
		}
	
		cout << "testcase 1 " << i << ": result = " << result << endl; 		
	}

	cout << "****" << endl;
}

void testcode_2()
{
	enum { MAX_LENGTH = 16 };
		
	char szTempBuffer[ MAX_LENGTH ];

	for (int i=0;i<MAX_TESTSTRINGS;i++)
	{
		bool result = false;

		parseString(szTestStrings[i],szTempBuffer,MAX_LENGTH-1);
	
		if (strcmp(szTestStrings[i],szTempBuffer) == 0)
		{
			result = true;
		}
			
		cout << "testcase 2 " << i << ": result = " << result << endl; 		
	}
	
	cout << "****" << endl;
}

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		cerr << _T("Fatal Error: MFC initialization failed") << endl;
		nRetCode = 1;
	}
	else
	{
		testcode_1( );
		testcode_2( );
		getch( );
	}

	return nRetCode;
}


