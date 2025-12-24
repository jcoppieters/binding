// BindingFileEntryParserB.cpp: implementation of the CBindingFileEntryParserB class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingFileEntryParserB.h"

#include "app\appbindings\appbindingunit\bindingunit_def.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingFileEntryParserB::CBindingFileEntryParserB() :
CBindingFileEntryParser()
{

}

CBindingFileEntryParserB::~CBindingFileEntryParserB()
{

}

CBindingUnit* CBindingFileEntryParserB::GetForwardUnit()
{
	return (&m_ForwardUnit);
}
CBindingUnit* CBindingFileEntryParserB::GetReturnUnit()
{
	return (&m_ReturnUnit);
}

// Sedert versie 0x0902: Extra controle op de lengte.
BOOL CBindingFileEntryParserB::IsTypeOf()
{
	ASSERT(m_szBindingFileEntry.IsEmpty() == FALSE);

	return ((m_szBindingFileEntry.GetLength()>15)&&
		    ((m_szBindingFileEntry.GetAt(14) == 'B')|| 
			 (m_szBindingFileEntry.GetAt(15) == 'B')));
}


/**
 * Public
 */
/*
BOOL CBindingFileEntryParserB::IsTypeOf(CString s)
{
	if (s.IsEmpty() == FALSE)
	{
		m_szBindingFileEntry = s;
		return (IsTypeOf());
	}
	return FALSE;
}
*/


/**
 *				   0123456
 * 0000XX_XX_XXXX_BUYYEXX>A0000XXUXXS
 */
BOOL CBindingFileEntryParserB::ParseReturnUnit(const char c)
{	
	static BYTE bReturnUnit = 0x00;
	static BYTE bEvent = 0x00;

	switch(m_nParserCount)
	{
		case 0:
		{		
			//TRACE("\nParse Forward Binding");

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
					(&CBindingFileEntryParserB::ParseForwardUnit);

			
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
 *						  0123456789				   
 * 0000XX_XX_XXXX_BUYYEXX>A0000XXUXXS
 */
BOOL CBindingFileEntryParserB::ParseForwardUnit(const char c)
{	
	static BYTE bNodeAddress = 0;
	static BYTE bUnitAddress = 0;

	switch(m_nParserCount)
	{
		case 0:
		{
			bNodeAddress = 0;
			bUnitAddress = 0;

			if (c != 'A') return FALSE;

		} break;

		case 1:
		case 2:
		case 3:
		case 4:
		{
			// Absorbe ...
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
		
		case 10:
		{	
			if (c == 'S') 
			{
				// Fill In ForwardUnit ...
				m_ForwardUnit.SetNodeAddress(bNodeAddress);
				m_ForwardUnit.SetUnitAddress(bUnitAddress);
				m_ForwardUnit.SetInOutType(UNIT_TYPE_OUTPUT);
						
				m_nParserCount = 0;
				CurrentState = NULL;
				return TRUE;
			}
			return FALSE;			
		} break;
	}
	m_nParserCount++;
	return TRUE;
}
