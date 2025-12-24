#include "stdafx.h"
#include "dimmerconfig_t.h"

#include <iostream>

using namespace std;

static void UpdateConfig(BYTE bConfig)
{
	BOOL fPirDelayedOff = ((bConfig & 0x80) == 0x80);
	BOOL fPirDelayedOn = ((bConfig & 0x40) == 0x40);
	BOOL fSolidState = ((bConfig & 0x02) == 0x02);

	cout << (int) bConfig << "," 
		 << (int) fPirDelayedOff << "," 
		 << (int) fPirDelayedOn << ","
		 << (int) fSolidState 
		 << "\n";
}

void DimmerConfig_Test( )
{
	BYTE bConfig = 0;
	UpdateConfig(bConfig);

	bConfig |= 0x80;
	UpdateConfig(bConfig);

	bConfig |= 0x40;
	UpdateConfig(bConfig);

	bConfig |= 0x02;
	UpdateConfig(bConfig);

	bConfig &= 0x02^0xFF;
	UpdateConfig(bConfig);

	bConfig &= 0x40^0xFF;
	UpdateConfig(bConfig);

	bConfig &= 0x80^0xFF;
	UpdateConfig(bConfig);
}