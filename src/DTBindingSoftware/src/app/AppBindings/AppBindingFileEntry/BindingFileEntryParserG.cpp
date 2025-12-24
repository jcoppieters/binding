// BindingFileEntryParserG.cpp: implementation of the CBindingFileEntryParserG class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingFileEntryParserG.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingFileEntryParserG::CBindingFileEntryParserG() :
CBindingFileEntryParser()
{

}

CBindingFileEntryParserG::~CBindingFileEntryParserG()
{

}

// Sedert versie 0x0902: controleren op de lengte.
BOOL CBindingFileEntryParserG::IsTypeOf()
{
	ASSERT(m_szBindingFileEntry.IsEmpty() == FALSE);

	if (m_szBindingFileEntry.GetLength()>15)
	{
		if ((m_szBindingFileEntry.GetAt(14) == 'G') || 
			(m_szBindingFileEntry.GetAt(15) == 'G'))
		{
			return (m_szBindingFileEntry.Find('>',14) == -1);
		}
	}
	return FALSE;
}

/**
 *				   0123456789abc
 * 0000XX_XX_XXXX_GA0000YYUYYEYY=UXXFXXDXXXXS
 */
BOOL CBindingFileEntryParserG::ParseReturnUnit(const char c)
{
	static BYTE bNodeAddress = 0x00;
	static BYTE bUnitAddress = 0x00;
	static BYTE bEvent = 0x00;

	switch(m_nParserCount)
	{
		case 0:
		{
			//TRACE("\nParse Go Binding");

			bNodeAddress = 0;
			bUnitAddress = 0;
			bEvent = 0;

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

		case 0x0a:
		{
			if (c != 'E') return FALSE;
		} break;

		case 0x0b:
		case 0x0c:
		{
			BYTE bTemp;
			if (AsciiToHex(c,&bTemp) == TRUE)
			{
				bEvent |= (bTemp << (4*(9-m_nParserCount)));
			}
			else
			{
				return FALSE;
			}
		} break;	
		
		case 0x0D:
		{
			if (c == '=')
			{
				m_ReturnUnit.SetNodeAddress(bNodeAddress);
				m_ReturnUnit.SetUnitAddress(bUnitAddress);
				m_ReturnUnit.BindingData.Set_MessageCode(bEvent);		

				m_nParserCount = 0;
				CurrentState = static_cast<ParseAction>
					(&CBindingFileEntryParserG::ParseForwardUnit);
			
				return TRUE;
			}
		} break;
	}

	m_nParserCount++;
	return TRUE;
}

