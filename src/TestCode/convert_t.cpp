#include "stdafx.h"
#include "convert_t.h"

#include "limits.h"
#include <iostream>

#include "assert.h"

using namespace std;

DEFINE_THIS_FILE;

/**********************************************************************
 * signed value converteren van 2Complement.
 * Integers worden standaard als two's complement weergegeven.
 *
 * param sValue [IN]: signed waarde in 2Complement.
 *		 1=0x0001, 0=0x0000, -1=0xFFFF, -2=0xFFFE
 *
 * return unsigned waarde.
 *		  1= 0x0001, 0=0x000, -1=0x8001, -2=0x8002
 *
 * note Deze functie kan gebruikt worden om een short weer te geven 
 *		als 2 bytes.
 *********************************************************************/
static unsigned short convertFrom2Complement(
	short sValue)
{		
	unsigned short usRetVal;

	if (sValue < 0)
	{	
		usRetVal = sValue ^ 0x7FFF;
		usRetVal += 1;

		// Deze code is ook goed:
		// usRetVal = (sValue & 0x7FFF) * -1;	// 0xFFFF wordt --> 0x8001
	}
	else
	{
		usRetVal = sValue;
	}
	return usRetVal;	
}

/**********************************************************************
 * unsigned value converteren naar 2Complement.
 * 
 * param usValue: unsigned waarde.
 *		  1= 0x0001, 0=0x000, -1=0x8001, -2=0x8002
 *		 
 * return signed waarde in 2Complement.
 *		  1=0x0001, 0=0x0000, -1=0xFFFF, -2=0xFFFE
 *
 *********************************************************************/
static short convertTo2Complement(
  unsigned short usValue)
{
	short sRetVal;

	if (usValue & 0x8000)
	{
		// Deze code is ook goed:
		//sRetVal = usValue ^ 0x7FFF;
		//sRetVal += 1;
		
		sRetVal = (usValue & 0x7FFF) * -1;	// 0x8001 wordt 0xFFFF	
	}
	else
	{
		sRetVal = usValue;
	}
	return sRetVal;
}


void Convert_Test( )
{
	short sTestValue;

	const int RANGE_MIN = SHRT_MIN + 1;
	const int RANGE_MAX = SHRT_MAX;

	for (int i=RANGE_MIN;i<RANGE_MAX;i++)
	{
		const unsigned short temp = convertFrom2Complement(i);
		sTestValue = convertTo2Complement(temp);
	
		ASSERT(i == sTestValue);
	}
}