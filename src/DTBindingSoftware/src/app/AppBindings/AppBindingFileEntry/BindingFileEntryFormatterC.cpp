// BindingFileEntryFormatterC.cpp: implementation of the CBindingFileEntryFormatterC class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingFileEntryFormatterC.h"
#include "app\appbindings\appbindingunit\bindingunit_def.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingFileEntryFormatterC::CBindingFileEntryFormatterC(CBindingEntryInfo* pBindingEntryInfo,
														 CBindingUnit* pForwardUnit) :
CBindingFileEntryFormatter(pBindingEntryInfo),
m_pForwardUnit(pForwardUnit)
{
	ASSERT(m_pForwardUnit);
}

CBindingFileEntryFormatterC::~CBindingFileEntryFormatterC()
{

}


BOOL CBindingFileEntryFormatterC::Format()
{
	CBindingUnits* const pBindingUnits = m_pBindingEntry->GetColUnit();
	CBindingOperators* const pBindingOperators = m_pBindingEntry->GetColOperator();

	ASSERT(m_pBindingEntry);
	ASSERT(m_pForwardUnit);
	ASSERT(pBindingUnits);
	ASSERT(pBindingOperators);

	m_szBindingFileEntry.Empty();

	if (m_pBindingEntry != NULL && 
		m_pForwardUnit != NULL &&
		m_pBindingEntryInfo != NULL &&
		pBindingUnits != NULL &&
		pBindingOperators != NULL)
	{
		
		CString szBindingString;
		BYTE bUnitType = UNIT_TYPE_INPUT;
		//CBindingUnit* pNextUnit = NULL;

		CBindingEvaluationString* const pBindingEvaluationString =
			m_pBindingEntry->GetEvaluationString();

		if (pBindingEvaluationString == NULL)
		{
			ASSERT(0);
			return FALSE;
		}

		CString szEvaluationString = pBindingEvaluationString->Get();
	
		const BYTE bBindingFlags = m_pBindingEntry->GetBindingReturnFlags();
		ASSERT(bBindingFlags <= BINDINGFLAGS_STATUS_PREPULS);

		// Parse + Format szBindingString ...			
		szBindingString.Format("0000%02X_%02X_%04X_C%s",
							   m_pForwardUnit->GetNodeAddress(),
							   bBindingFlags,
							   m_pBindingEntry->GetBindingNr(),
							   szEvaluationString);	
		
	

		const char* const szDataToParse = (LPCTSTR) szEvaluationString;	// CHANGES_MVS_2008_CSTRING
		const int nLen = szEvaluationString.GetLength();

		for (int i=0;i<nLen;i++) 
		{
			const char c = szEvaluationString.GetAt(i);

			switch(c) 
			{
				case 'U':
				{
					if (bUnitType == UNIT_TYPE_INPUT) 
					{
						char ch;
						unsigned short nId;

						if (pBindingEvaluationString->ParseUnit(i,&nId,&ch) == TRUE)
						{
							CString szTemp;
							if (pBindingEvaluationString->FormatUnit(szTemp,nId,ch) == TRUE)
							{
								const int nIndex = szBindingString.Find(szTemp);
								if (nIndex != 0) 
								{
									CBindingUnit* const pUnit = pBindingUnits->Find(nId);
									if (pUnit != NULL)
									{								
										CString szEventString;
										szEventString.Format("A0000%02XU%02XE%02X%c",
															 pUnit->GetNodeAddress(),
															 pUnit->GetUnitAddress(),
															 pUnit->BindingData.Get_MessageCode(),
															 ch);
					
										const int n = szBindingString.Replace(szTemp,szEventString);
										if (n == 0)
										{
											ASSERT(n);
											return FALSE;
										}
									}
									else
									{									
										ASSERT(pUnit);
										return FALSE;
									}
								}
								else
								{
									ASSERT(0);
									return FALSE;
								}
							}
							else
							{
								ASSERT(0);
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

				case 'T':
				{											
					unsigned short nId;
					if (pBindingEvaluationString->ParseTimer(i,&nId) == FALSE)
					{
						ASSERT(0);
						return FALSE;
					}
			
					CBindingOperator* const pBindingOperator = pBindingOperators->Find(nId);
					ASSERT(pBindingOperator);
				
					if (pBindingOperator != NULL)
					{
						if (pBindingOperator->GetType() != PM_OPERATOR_TIMER)
						{
							ASSERT(0);
							return FALSE;
						}

						CString szTemp;
						if (pBindingEvaluationString->FormatTimer(szTemp,nId) == FALSE)
						{
							ASSERT(0);
							return FALSE;
						}					

						const BYTE bValue = ((CBindingTimerOperator*)pBindingOperator)->GetTimeValue();
						const TimerType Type = ((CBindingTimerOperator*)pBindingOperator)->GetTimerType();

						CString szEventString;
	
						switch(Type)
						{
							case PM_TIMER_TE: 
								szEventString.Format("Te%02X",bValue); 
							break;
					
							case PM_TIMER_TI: 
								szEventString.Format("Ti%02X",bValue); 						
							break;
					
							case PM_TIMER_TO: 
								szEventString.Format("To%02X",bValue); 
							break;
					
							case PM_TIMER_TF: 
								szEventString.Format("Tf%02X",bValue); 
							break;
					
							case PM_TIMER_TD: 
								szEventString.Format("Td%02X",bValue); 						
							break;

							case PM_TIMER_TR: 
								szEventString.Format("Tr%02X",bValue); 
							break;

							default: 
								ASSERT(0); 					
							return '\0';					
						}

						const int n = szBindingString.Replace(szTemp,szEventString);
						if (n == 0)
						{
							ASSERT(n);
							return FALSE;
						}
					}

					ASSERT(pBindingOperator);

				} break;

				case '=':
				{			
					unsigned short nId;

					if (pBindingEvaluationString->ParseUnit(i+1,&nId) == TRUE)
					{
						CString szTemp;
						if (pBindingEvaluationString->FormatUnit(szTemp,nId) == TRUE)
						{
							CString szTemp2 = '=' + szTemp;

							CBindingUnit* const pUnit = pBindingUnits->Find(nId);

							ASSERT(m_pForwardUnit == pUnit);

							if (pUnit != NULL)
							{								
								CString szEventString;
								szEventString.Format("=U%02XF%02XD",pUnit->GetUnitAddress(),
																	pUnit->BindingData.Get_MessageCode());
								
								// t holds length
								const int t = pUnit->BindingData.Get_Length();
	
								// First BYTE = Length
								CString szBindingFunctionData;
								szBindingFunctionData.Format("%02X",t);
				
								// Add Data
								szTemp.Empty();
								for (int j=0;j<t;j++) 
								{
									szTemp.Format(_T("%02X"),pUnit->BindingData.Get_Data(j));
									szBindingFunctionData += szTemp;				
								}

								// Check length 
								ASSERT(szBindingFunctionData.GetLength() == (t*2)+2);

								szEventString += szBindingFunctionData;
							
								const int n = szBindingString.Replace(szTemp2,szEventString);									
								bUnitType = UNIT_TYPE_OUTPUT;

								if (n == 0)
								{
									ASSERT(0);
									return FALSE;
								}
							}
							else
							{
								ASSERT(0);
								return FALSE;
							}
						}
						else
						{
							ASSERT(0);
							return FALSE;
						}
					}
					else
					{
						ASSERT(0);
						return FALSE;
					}
				} break;

			} //end switch(c) 

			if (bUnitType == UNIT_TYPE_OUTPUT)
			{
				break;
			}
		} // end for loop 	

		// format szBindingstring
		if (1)
		{	
			if (m_pBindingEntryInfo->Get_NOutputs() > 2) 
			{
				int j = 0;

				for (int i=0;
					 i<2;
					 i++) 
				{
					j = szBindingString.Find('>',j+1);
				}

				szBindingString.SetAt(j,'S');
				szBindingString.SetAt(j+1,'\0');

				// SetLength ...
				CString szTemp = szBindingString.Left(j+1);

				szBindingString.Empty();
				szBindingString = szTemp;
			}
			else 
			{
				szBindingString += 'S';
			}	
	
			szBindingString.FreeExtra();
			
			m_szBindingFileEntry = szBindingString;
			return TRUE;
		}
	}
	return FALSE;
}

