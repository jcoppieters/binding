// BindingFileEntryParserI.cpp: implementation of the CBindingFileEntryParserI class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingFileEntryParserI.h"

#include "app\appbindings\appbindingunit\bindingunit_def.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingFileEntryParserI::CBindingFileEntryParserI() :
CBindingFileEntryParser()
{

}

CBindingFileEntryParserI::~CBindingFileEntryParserI()
{

}

// Sedert versie 0x0900 controleren op de lengte.
BOOL CBindingFileEntryParserI::IsTypeOf()
{
	ASSERT(m_szBindingFileEntry.IsEmpty() == FALSE);

	return ((m_szBindingFileEntry.GetLength()>15)&&
		    ((m_szBindingFileEntry.GetAt(14)=='I')|| 
			 (m_szBindingFileEntry.GetAt(15)=='I')));
}


/**
 * = CBindingFileEntryTypeB
 *
 *				   012345
 * 0000XX_XX_XXXX_IUYYEYY>A0000XXUXXFXXDXXXXS
 */
BOOL CBindingFileEntryParserI::ParseReturnUnit(const char c)
{
	static BYTE bReturnUnit = 0x00;
	static BYTE bEvent = 0x00;

	switch(m_nParserCount)
	{
		case 0:
		{		
			//TRACE("\nParse Instruction Binding");

			bReturnUnit = 0;
			bEvent = 0;

			if (c != 'U') return FALSE;

		} break;

		case 1:
		case 2:
		{
			BYTE bTemp;
			if (AsciiToHex(c,&bTemp) == TRUE)
			{
				bReturnUnit |= (bTemp << (4*(2-m_nParserCount)));
			}
			else
			{
				return FALSE;
			}
		} break;

		case 3:
		{	
			if (c != 'E') return FALSE;

		} break;

		case 4:
		case 5:
		{
			BYTE bTemp;
			if (AsciiToHex(c,&bTemp) == TRUE)
			{
				bEvent |= (bTemp << (4*(5-m_nParserCount)));
			}
			else
			{
				return FALSE;
			}
		} break;

		case 6:
		{
			if (c == '>')
			{
				// Fill in m_ReturnUnit ...
				m_ReturnUnit.SetNodeAddress(m_bNodeAddress);
				m_ReturnUnit.SetUnitAddress(bReturnUnit);
				m_ReturnUnit.BindingData.Set_MessageCode(bEvent);
				m_ReturnUnit.BindingData.Set_BindingFlags(m_bBindingFlags);
				m_ReturnUnit.SetInOutType(UNIT_TYPE_INPUT);

				m_nParserCount = 0;
				CurrentState = static_cast<ParseAction>
					(&CBindingFileEntryParserI::ParseForwardUnit);

			
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		} break;
	}

	m_nParserCount++;
	return TRUE;
}

/**
 *						  0123456789abcdef012
 * 0000XX_XX_XXXX_IUXXEXX>A0000XXUXXFXXDXXXXS
 */
BOOL CBindingFileEntryParserI::ParseForwardUnit(const char c)
{
	static BYTE bNodeAddress;
	static BYTE bUnitAddress;
	static BYTE bFunctionData;
	static BYTE bLength;
	static BYTE bData[6];
	static BYTE bDataCnt;

	switch(m_nParserCount)
	{
		case 0:
		{	
			bDataCnt = 0;
			bUnitAddress = 0;
			bNodeAddress = 0;
			bFunctionData = 0;
			bLength = 0;
			ZeroMemory(&bData,6*sizeof(BYTE));

			if (c != 'A') return FALSE;

		} break;

		case 1:
		case 2:
		case 3:
		case 4:
		{
			ASSERT(c=='0');

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
			if (c != 'F') return FALSE;

		} break;

		case 0x0B:
		case 0x0C:
		{
			BYTE bTemp;
			if (AsciiToHex(c,&bTemp) == TRUE)
			{
				bFunctionData |= (bTemp << (4*(0x0C-m_nParserCount)));
			}
			else
			{
				return FALSE;
			}
		} break;

		case 0x0D:
		{
			if (c != 'D') return FALSE;

		} break;

		case 0x0E:
		case 0x0F:
		{
			BYTE bTemp;
			if (AsciiToHex(c,&bTemp) == TRUE)
			{
				bLength |= (bTemp << (4*(0x0F-m_nParserCount)));
			}
			else
			{
				return FALSE;
			}
		} break;

		default:
		{
			if (c == 'S')
			{
				m_ForwardUnit.SetNodeAddress(bNodeAddress);
				m_ForwardUnit.SetUnitAddress(bUnitAddress);
				m_ForwardUnit.SetInOutType(UNIT_TYPE_OUTPUT);

				m_ForwardUnit.BindingData.Set_BindingFlags(m_bBindingFlags);					
				m_ForwardUnit.BindingData.Set_MessageCode(bFunctionData);					
				m_ForwardUnit.BindingData.Set_Length(bLength);

				for (int i=0;i<6;i++)
				{
					m_ForwardUnit.BindingData.Set_Data(i,bData[i]);
				}	

				m_nParserCount = 0;

			
				CurrentState = NULL;										
				return TRUE;
			}		
			else
			{
				if (bDataCnt <= 12)
				{
					BYTE bTemp;
					if (AsciiToHex(c,&bTemp) == TRUE)
					{
						if ((bDataCnt%2) == 0)
						{
							bData[bDataCnt/2] = bTemp << 4;
						}
						else
						{
							bData[bDataCnt/2] |= bTemp;
						}
				
						bDataCnt++;
					}
					else
					{
						return FALSE;
					}
				}
				else
				{
					ASSERT(0);
					return FALSE;
				}
			}

		} break;
	}

	m_nParserCount++;
	return TRUE;
}
