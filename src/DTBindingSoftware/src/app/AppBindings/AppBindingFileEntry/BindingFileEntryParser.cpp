// BindingFileEntryParser.cpp: implementation of the CBindingFileEntryParser class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingFileEntryParser.h"

#include "app\appbindings\appbindingunit\bindingunit_def.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CBindingFileEntryParser::CBindingFileEntryParser()
{
	m_szBindingFileEntry.Empty();
}

CBindingFileEntryParser::CBindingFileEntryParser(CString s) :
m_szBindingFileEntry(s)
{
	ASSERT(m_szBindingFileEntry.IsEmpty() == FALSE);
}

CBindingFileEntryParser::~CBindingFileEntryParser()
{

}

BOOL CBindingFileEntryParser::Parse(CString s)
{
	m_szBindingFileEntry = s;

	ASSERT(m_szBindingFileEntry.IsEmpty() == FALSE);

	if (IsTypeOf() == TRUE)
	{
		CurrentState = &CBindingFileEntryParser::ParseNodeAddress;	// CHANGES_MVS_2008
		m_nParserCount = 0;

		const int nLen = m_szBindingFileEntry.GetLength();
		for (int i=0;i<nLen;i++)
		{
			const char c = m_szBindingFileEntry.GetAt(i);
			
			if (CurrentState != NULL)
			{			
				if ((this->*CurrentState)(c) == FALSE)
				{
					return FALSE;
				}
			}

			// Fill In Data ...
		}
		return TRUE;
	}
	return FALSE;
}


BOOL CBindingFileEntryParser::AsciiToHex(const char c,BYTE *bHex)
{
	ASSERT(bHex);

	if (c >= '0' && c<= '9')
	{	
		*bHex = c - '0';
		return TRUE;
	}
	else if (c>='A' && c<='F')
	{
		*bHex = c - 'A' + 10;
		return TRUE;
	}
	else if (c>='a' && c<='f')
	{
		*bHex = c - 'a' + 10; 
		return TRUE;
	}	

	return FALSE;
}

/**
 * 0123456
 * 0000XX_
 */
BOOL CBindingFileEntryParser::ParseNodeAddress(const char c)
{
	static BOOL fHex = TRUE;
	
	if (m_nParserCount == 0)
	{
		fHex = TRUE;
	}
	else if (m_nParserCount >= 4 && m_nParserCount < 6)
	{
		BYTE bTemp;

		if (AsciiToHex(c,&bTemp) == TRUE)
		{
			if (fHex == TRUE) 
			{
				m_bNodeAddress = bTemp << 4;
				fHex = FALSE;
			}
			else
			{
				m_bNodeAddress |= bTemp;
				fHex = TRUE;
			}
		}
	}
	else if (m_nParserCount == 6 && c=='_')
	{
		m_nParserCount = 0;
		CurrentState = &CBindingFileEntryParser::ParseBindingFlags;	// CHANGES_MVS_2008
		return TRUE;	// !!!!
	}
	else if (m_nParserCount >= 6)
	{
		return FALSE;
	}

	m_nParserCount++;
	return TRUE;
}

/**
 * XX_
 */
BOOL CBindingFileEntryParser::ParseBindingFlags(const char c)
{
	switch(m_nParserCount)
	{
		case 0:
		{	
			BYTE bTemp;
			if (AsciiToHex(c,&bTemp) == TRUE) 
			{
				m_bBindingFlags = bTemp << 4;
				m_nParserCount++;						
				return TRUE;
			}		

		} break;

		case 1:
		{			
			BYTE bTemp;
			if (AsciiToHex(c,&bTemp) == TRUE) 
			{
				m_bBindingFlags |= bTemp;
				m_nParserCount++;						
				return TRUE;
			}			
		} break;

		case 2:
		{	
			if (c == '_')
			{
				m_nParserCount = 0;
				CurrentState = &CBindingFileEntryParser::ParseBindingNumber;	// CHANGES_MVS_2008
				return TRUE;
			}
			return FALSE;

		} break;
	}
	return FALSE;
}

/**
 * 0123
 * XXXX
 */
BOOL CBindingFileEntryParser::ParseBindingNumber(const char c)
{
	ASSERT(m_nParserCount<4);

	BYTE bTemp;

	if (AsciiToHex(c,&bTemp) == TRUE) 
	{
		switch(m_nParserCount)
		{
			case 0:			
			{
				m_usBindingNr = bTemp << 12;	

			} break;

			case 1:
			{
				m_usBindingNr |= bTemp << 8;

			} break;

			case 2:
			{
				m_usBindingNr |= bTemp << 4;

			} break;

			case 3:
			{
				m_usBindingNr |= bTemp;

				m_nParserCount = 0;
				CurrentState = &CBindingFileEntryParser::ParseBindingType;		// CHANGES_MVS_2008
				return TRUE;

			} break;			
		}

		m_nParserCount++;
		return TRUE;
	}

	return FALSE;
}

/**
 * _X   X
 */
BOOL CBindingFileEntryParser::ParseBindingType(const char c)
{
	switch (c)
	{
		case '_':
		{
			m_nParserCount = 0;
			return TRUE;

		} break;

		case 'N':
		case 'I':
		case 'G':
		case 'B':
		{	
			m_nParserCount = 0;
			CurrentState = &CBindingFileEntryParser::ParseReturnUnit;	// CHANGES_MVS_2008
			return TRUE;
		}break;

		case 'C':
		{
			m_nParserCount = 0;
			CurrentState = &CBindingFileEntryParser::ParseConditionString;	// CHANGES_MVS_2008			
			return TRUE;
		}break;		

		case 'P':
		{
			m_nParserCount = 0;
			CurrentState = &CBindingFileEntryParser::ParseForwardUnit;	// CHANGES_MVS_2008
			return TRUE;

		}break;

		default:
		{
			ASSERT(0);	

		} break;
	}
	return FALSE;
}


BOOL CBindingFileEntryParser::ParseReturnUnit(const char c)
{
	ASSERT(0);
	return FALSE;
}

/**
 * Used By : 
 *	- P-Binding
 *	- C-Binding
 *  - G-Binding
 *	- N-Binding
 * 
 */
BOOL CBindingFileEntryParser::ParseForwardUnit(const char c)
{
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
			bFunctionData = 0;
			bLength = 0;
			ZeroMemory(&bData,6*sizeof(BYTE));

			if (c != 'U') return FALSE;

		} break;

		case 1:
		case 2:
		{
			BYTE bTemp;
			if (AsciiToHex(c,&bTemp) == TRUE)
			{
				bUnitAddress |= (bTemp << (4*(2-m_nParserCount)));
			}
			else
			{
				return FALSE;
			}
		} break;

		case 3:
		{
			if (c != 'F') return FALSE;

		} break;

		case 4:
		case 5:
		{
			BYTE bTemp;
			if (AsciiToHex(c,&bTemp) == TRUE)
			{
				bFunctionData |= (bTemp << (4*(5-m_nParserCount)));
			}
			else
			{
				return FALSE;
			}
		} break;

		case 6:
		{
			if (c != 'D') return FALSE;

		} break;

		case 7:
		case 8:
		{
			BYTE bTemp;
			if (AsciiToHex(c,&bTemp) == TRUE)
			{
				bLength |= (bTemp << (4*(8-m_nParserCount)));
			}
			else
			{
				return FALSE;
			}
		} break;

		default:
		{
			if (c == 'S' || c == '>') 
			{
				m_ForwardUnit.SetNodeAddress(m_bNodeAddress);
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

				if (c == '>')
				{
					CurrentState = &CBindingFileEntryParser::ParseNextUnit;		// CHANGES_MVS_2008			
				}
				else
				{
					CurrentState = NULL;							
				}
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

BOOL CBindingFileEntryParser::ParseNextUnit(const char c)
{
	static BYTE bNodeAddress;
	static BYTE bUnitAddress;

	switch (m_nParserCount)
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

		case 0x0A:
		{
			if (c == 'S')
			{
				m_NextUnit.SetNodeAddress(bNodeAddress);
				m_NextUnit.SetUnitAddress(bUnitAddress);
				m_NextUnit.SetInOutType(UNIT_TYPE_OUTPUT);
				m_NextUnit.BindingData.Set_BindingFlags(m_bBindingFlags);

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

BOOL CBindingFileEntryParser::ParseConditionString(const char )
{
	ASSERT(0);
	return FALSE;
}