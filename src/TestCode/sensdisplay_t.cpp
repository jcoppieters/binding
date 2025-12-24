#include "stdafx.h"
#include "sensdisplay_t.h"

#include <iostream>
#include <iomanip>

using namespace std;

static void displayBug(float i,short value)
{
	// Probleem wanneer short wordt gebruikt bij de getallen
	// -9 tem 00.
	// Delen door 10 geeft altijd resultaat 0 waardoor er geen
	// '-' voorkomt.

	cout << std::setw(4) << setprecision(1) << std::fixed << (i/10) 
		 << " - " << std::setw(4) << value/10 << "." << abs(value%10)
		 << (i > -10 && i < 0 ? " ERROR" : "" )
		 << endl;	
}

static void displayOk(float i,short value)
{
	cout << std::setw(4) << setprecision(1) << std::fixed << (i/10) 
		 << " - " << std::setw(4)
		 << (i > -10 && i < 0 ? "-" : "" )
		 <<  value/10 << "." << abs(value%10)
		 << endl;	
}

void SensDisplay_Test( )
{	
	float f = 0;

	const int MIN_RANGE = -20;
	const int MAX_RANGE = 20;
	const int STEP = 2;

	for (float i=MIN_RANGE;i<MAX_RANGE;i+=STEP)
	{
		displayBug(i, (short) i);
		displayOk(i, (short) i);
	}
}