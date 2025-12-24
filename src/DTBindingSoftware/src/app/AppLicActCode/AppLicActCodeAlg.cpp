#include "stdafx.h"
#include "AppLicActCodeAlg.h"
	

// Oppassen: calculateCode( ) is dezelfde functie die tool gebruikt om codes te
// decoderen...

/********************************************************************
 * @brief	Encode/Decode value.
 * @param	ulValue [IN] Input waarde
 * @param	fEncode [IN] 
 *			TRUE = Encode value.
 *			FALSE = Decode value
 * @return	Decoded/encoded output value.
 *******************************************************************/
static 
unsigned long calculateCode(
   unsigned long ulValue,
   BOOL fEncode = TRUE)
{
	unsigned long ulRetVal = 0;
	const unsigned long ulXORValue = 0xAF85A35Aul;

	// 1) Waarde opspliten in HLSB, LMSB & LLSB
	// 2) Telkens de laagste nibble XOR met 0xf
	// 3) HMSB = HMSB +n[0]+n[1]+n[2]+n[3]  (n=nibble)
	// 4) Waarde opnieuw opbouwen via HLSB,LMSB & LLSB
	// 5) Waarde XOR doen moet een constante waarde.
	if (fEncode == TRUE)
	{
		const BYTE HLSB = (BYTE)((ulValue&0xFF0000)>>16)^0xf;
		const BYTE LMSB = (BYTE)((ulValue&0x00FF00)>>8)^0xf;
		const BYTE LLSB = (BYTE) (ulValue&0xFF)^0xf;

		ulRetVal = LLSB;
		ulRetVal += (LMSB<<8);
		ulRetVal += (HLSB+(LLSB&0xF)+((LLSB&0x0F)>>8)+(LMSB&0xF)+((LMSB&0xF0)>>8))<<16;

		ulRetVal ^= ulXORValue;
	}
	else
	{
		ulValue ^= ulXORValue;

		int HLSB = (int) ((ulValue&0xFFFF0000)>>16);
		BYTE LMSB = (int) ((ulValue&0x0000FF00)>>8);
		BYTE LLSB = (int)  (ulValue&0xFF);

		HLSB = HLSB-((LLSB&0xF)+((LLSB&0x0F)>>8)+(LMSB&0xF)+((LMSB&0xF0)>>8));

		ulRetVal = LLSB;
		ulRetVal += (LMSB<<8);
		ulRetVal += (HLSB<<16);
		ulRetVal ^= 0x0f0f0f;
	}
	return ulRetVal;
}

/******************************** public **********************************/

unsigned long CAppLicActCodeAlg::Decode(
		unsigned long ulValue)
{
	return calculateCode(ulValue,FALSE);
}