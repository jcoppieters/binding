/**********************************************************************
 * @file	helperconvert.cpp
 * @brief	Implementatie bestand met helper convert functies.
 * @author	Mitchell Tom
 * @date	20120403
 *
 *********************************************************************/

#include "stdafx.h"
#include "helperconvert.h"

unsigned short convertFrom2Complement(
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

short convertTo2Complement(
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