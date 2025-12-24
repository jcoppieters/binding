// BindingFileEntryParserN.cpp: implementation of the CBindingFileEntryParserN class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingFileEntryParserN.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingFileEntryParserN::CBindingFileEntryParserN() :
CBindingFileEntryParser()
{

}

CBindingFileEntryParserN::~CBindingFileEntryParserN()
{

}

// Sedert versie 0x0902 extra controle op de lengte.
BOOL CBindingFileEntryParserN::IsTypeOf()
{
	ASSERT(m_szBindingFileEntry.IsEmpty() == FALSE);

	if (m_szBindingFileEntry.GetLength()>15)
	{
		if ((m_szBindingFileEntry.GetAt(14) == 'N') || 
			(m_szBindingFileEntry.GetAt(15) == 'N'))
		{
			return (m_szBindingFileEntry.Find('>',14) == -1);
		}
	}
	return FALSE;
}


/**			
 *				   0123456789
 * 0000XX_XX_XXXX_NA0000YYUYY=UXXFXXDXXXXS
 */
BOOL CBindingFileEntryParserN::ParseReturnUnit(const char c)
{
	static BYTE bNodeAddress = 0x00;
	static BYTE bUnitAddress = 0x00;

	switch(m_nParserCount)
	{
		case 0:
		{
			//TRACE("\nParse NEXT Binding");

			bNodeAddress = 0;
			bUnitAddress = 0;		

			if (c != 'A') return FALSE;

		} break;

		case 1:
		case 2:
		case 3:
		case 4:
		{
			// ABsorbe ...
			ASSERT(c == '0');

		} break;

		case 5:
		case 6:
		{
			BYTE bTemp;
			if (AsciiToHex(c,&bTemp) == TRUE)
			{
				bNodeAddress |= (bTemp << (4*(6-m_nParserCount)));
			}
			else
			{
				return FALSE;
			}
		} break;

		case 7:
		{
			if (c != 'U') return FALSE;
		} break;

		case 8:
		case 9:
		{
			BYTE bTemp;
			if (AsciiToHex(c,&bTemp) == TRUE)
			{
				bUnitAddress |= (bTemp << (4*(9-m_nParserCount)));
			}
			else
			{
				return FALSE;
			}
		} break;

		case 0x0A:
		{
			if (c == '=')
			{
				m_ReturnUnit.SetNodeAddress(bNodeAddress);
				m_ReturnUnit.SetUnitAddress(bUnitAddress);					

				m_nParserCount = 0;
				CurrentState = static_cast<ParseAction>
					(&CBindingFileEntryParserN::ParseForwardUnit);
			
				return TRUE;
			}
		} break;
	}

	m_nParserCount++;
	return TRUE;
}

CBindingUnit* CBindingFileEntryParserN::GetReturnUnit()
{
	return (&m_ReturnUnit);
}


