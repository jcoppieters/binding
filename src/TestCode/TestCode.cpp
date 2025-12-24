// TestCode.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "dimmerconfig_t.h"
#include "sensdisplay_t.h"
#include "convert_t.h"

#include <iostream>

using namespace std;

void onAssert__(char const *file, unsigned line, int fatal)
{
	char ch = '\0';

	cout << "Assert Failure: " << file << " #" << line << endl;	
	cin >> ch;
}

int main(int argc, char* argv[])
{
	char ch = '\0';

	// DimmerConfig_Test( );
	Convert_Test( );
	SensDisplay_Test( );

	cin >> ch;

	return 0;
}

