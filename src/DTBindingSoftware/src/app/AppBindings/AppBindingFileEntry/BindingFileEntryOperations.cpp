// BindingFileEntryOperations.cpp: implementation of the CBindingFileEntryOperations class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingFileEntryOperations.h"

#include "BindingFileEntry.h"
#include "app\appbindings\appbindingunit\bindingunit_api.h"
//#include "BindingUnit.h"
#include "BindingUnits.h"
#include "BindingUnitFactory.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
//////////////////////////////////////////////////////////////////////
#define DEBUG_LOCAL_ON	1
//////////////////////////////////////////////////////////////////////
#if (DEBUG_LOCAL_ON==1)
#define TRACER(x)				TRACE x
#define DUMP_REFERENCES(x,y)	dumpReferences(x,y)
#else
#define TRACER(x ...)
#define DUMP_REFERENCES(x,y)
#endif
//////////////////////////////////////////////////////////////////////

#if (DEBUG_LOCAL_ON==1)

static 
void dumpReferences(
	const CString& s,
	const CStringArray& arr)
{
	const int count=arr.GetCount();

	TRACE("References:\r\n");
	TRACE("String='%s'\r\n",s);
	for (int i=0;i<count;++i)
	{
		TRACE("ref %02d:%s\r\n",i,arr.GetAt(i));
	}
	TRACE("References: End --\r\n");
}

#endif 

//////////////////////////////////////////////////////////////////////


// CHANGES_BINDINGS_REFERENCES
// Zoeken naar het aantal references in een binding string entry.
// Zoeken naar aantal 'U' die tussen de 'C' en '=' liggen.
// Oppassen references die meerdere keren voorkomen mogen maar 1 keer gerekend worden...

static 
int calculateNrOfReferences(
	const CString& strBindingFileEntry)
{
	// Zoeken tussen 'C' en '=',
	// Index start bij 14 en maxIndex verwijst naar '=
	int retValCount=0;
	const int maxIndex = strBindingFileEntry.Find("=",0);
	if (maxIndex>0)
	{
		// Starten bij de 'C'.
		int nIndex = 14;	

		// String array bevat references (bewaard als string)
		// Dezelfde reference die meerdere keren voorkomt in een string mag maar 1 keer in
		// rekenning gebracht worden.

		CStringArray arrayStrReferences;
		while ((nIndex=strBindingFileEntry.Find("U",nIndex+1))!= -1) 
		{
			if (nIndex<maxIndex)
			{
				const char* const szTemp = &((LPCTSTR)strBindingFileEntry)[nIndex-7];
				if ((0!=szTemp) && (strlen(szTemp)>13))
				{
					// Zoeken in array naar dezelfde reference strings...
					const int nStringEntries = arrayStrReferences.GetCount();
					CString szReferences(szTemp,13);
					int i;

					for(i=0;i<nStringEntries;++i)
					{
						if (szReferences == arrayStrReferences[i])
							break;
					}

					if (i==nStringEntries)
					{
						arrayStrReferences.Add(szReferences);	
					}
				}
				else
				{
					ASSERT(0);
				}
			}
			retValCount=arrayStrReferences.GetCount();
		}

		DUMP_REFERENCES(strBindingFileEntry,arrayStrReferences);
	}
	return retValCount;
}

static
int calculateCBindingConditionLength(
	const CString& strBindingFileEntry)
{
	const int maxIndex = strBindingFileEntry.Find("=",0)+1;
	int length = 0;

	if (maxIndex>0)
	{
		// Starten bij de 'C'.
		int index = 14;

		while(index<maxIndex)
		{
			const char ch=strBindingFileEntry.GetAt(index);
			switch(ch)
			{
				case '+':
				case '*':
				case '^':
				{
					// Operator: conditie lengte +1
					length += 1;
				} break;

				case '!':
				{
					// Unaire operator: conditie lengte +1
					length += 1;
				} break;

				case '(':
				case ')':
				{
					// Haakjes: conditie lengte +1
					length += 1;
				} break;

				case 'T':
				{
					// Timer: conditie lengte +2
					// 'T' + Byte idx van de timer
					length += 2;
				} break;

				case 'U':
				{
					// Unit: conditie lengte +2
					// 'U' + Byte idx van de reference
					length += 2;
				} break;

				case '=':
				{
					// Einde van de conditie string.
					// '=' en het null char.
					length += 2;
				} break;
			}

			index++;

		} /* while(index<maxIndex) */ 
	}
	return length;
}

static
BOOL helper_IsConditionBinding(
	const CString& str)
{
	return ((str.GetAt(14)=='C')||(str.GetAt(15)=='C'));
}
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingFileEntryOperations::CBindingFileEntryOperations(CString s) :
m_szBindingFileEntry( s )
{
	ASSERT( m_szBindingFileEntry.IsEmpty() == FALSE );
}

CBindingFileEntryOperations::~CBindingFileEntryOperations()
{

}

unsigned short CBindingFileEntryOperations::GetBindingNr() const
{
	CBindingFileEntry BindingFileEntry(m_szBindingFileEntry);
	CBindingFileEntryParser* pParser = BindingFileEntry.Parse();
	if (pParser != 0)
	{
		return(pParser->GetBindingNr());
	}
	return 0xFFFF;
}

BOOL CBindingFileEntryOperations::ReplaceBindingNr(unsigned short usBindingNr,CString& szReturn)
{
	if (m_szBindingFileEntry.IsEmpty() == TRUE) {
		return FALSE;
	}

	CBindingFileEntry BindingFileEntry(m_szBindingFileEntry);
	CBindingFileEntryParser* pParser = BindingFileEntry.Parse();
	if (pParser != 0)
	{
		szReturn = m_szBindingFileEntry;

		CString szTemp;
		CString szTemp2;

		// TM 20070728 : Must be Upper!!!
		szTemp.Format("%02X_%04X",
					  pParser->GetBindingFlags(),
					  pParser->GetBindingNr());

		szTemp2.Format("%02X_%04X",
					   pParser->GetBindingFlags(),
					   usBindingNr);

		szTemp.MakeUpper();
		szTemp2.MakeUpper();

		const int n = szReturn.Replace(szTemp,szTemp2);
		if (n != 0)
		{
			return TRUE;
		}	
		else
		{
			ASSERT(0);
			return FALSE;
		}
	}
	return FALSE;
}

BOOL CBindingFileEntryOperations::ReplaceMessageCode(BYTE bMessageCode,CString& szReturn)
{
	if (m_szBindingFileEntry.IsEmpty() == TRUE) {
		return FALSE;
	}

	CBindingFileEntry BindingFileEntry(m_szBindingFileEntry);
	CBindingFileEntryParser* pParser = BindingFileEntry.Parse();
	if (pParser != 0)
	{
		if (pParser->IsTypeB() == TRUE) {
			ASSERT(0);
			return FALSE;
		}

		const CBindingUnit* const pForwardUnit = pParser->GetForwardUnit();
		if (pForwardUnit == 0) {
			ASSERT( pForwardUnit );
			return FALSE;
		}
	
		const BYTE bLength = pForwardUnit->BindingData.Get_Length();	

		if (bLength != 0)
		{
			szReturn = m_szBindingFileEntry;

			CString szTemp;
			CString szTemp2;

			szTemp.Format("U%02XF%02XD%02X",pForwardUnit->GetUnitAddress(),
											pForwardUnit->BindingData.Get_MessageCode(),
											bLength);

			szTemp2.Format("U%02XF%02XD%02X",pForwardUnit->GetUnitAddress(),
											 bMessageCode,
											 bLength);
	
			szTemp.MakeUpper();
			szTemp2.MakeUpper();

			const int n = szReturn.Replace(szTemp,szTemp2);
			if (n != 0)
			{	
				return TRUE;
			}
		}
	}
	return FALSE;
}


BOOL CBindingFileEntryOperations::ConvertPropertyString(CString &szOut,BOOL fProcessed)
{
	BOOL fResult = FALSE;

	if (m_szBindingFileEntry.IsEmpty() == TRUE) {
		return FALSE;
	}

	CBindingFileEntry BindingFileEntry(m_szBindingFileEntry);
	CBindingFileEntryParser* const pBindingFileEntryType = BindingFileEntry.Parse();
	
	const unsigned short usBindingNr = pBindingFileEntryType->GetBindingNr();

	szOut = m_szBindingFileEntry;
	
	if (fProcessed == FALSE)
	{		
		if (usBindingNr < 0x9000)	// Not Processed --> Set BindingNr to 0x9000
		{
			const unsigned short usNewBindingNr = 0x9000;										
			fResult = ReplaceBindingNr(usNewBindingNr,szOut);
			ASSERT( fResult );
		}				
	}
	else
	{	
		if (usBindingNr != 0x0000)	// Processed --> Set BindingNr to 0x0000	
		{
			const unsigned short usNewBindingNr = 0x0000;									
			fResult = ReplaceBindingNr(usNewBindingNr,szOut);
			ASSERT( fResult );
		}		
	}	

	TRACE1("\nszIn = %s",m_szBindingFileEntry);
	TRACE1("\nszOut = %s",szOut);
	
	return fResult;
}

BYTE CBindingFileEntryOperations::GetNodeAddress() const 
{
	ASSERT( m_szBindingFileEntry.IsEmpty() == FALSE );

	if (m_szBindingFileEntry.IsEmpty() == TRUE) 
	{
		return 0xFF;
	}

	int temp = -1;
	BYTE bNodeAddress = 0xFF;

	if (sscanf((LPCTSTR) m_szBindingFileEntry,			// CHANGES_MVS_2008_SSCANF
				"0000%02X_",
				&temp) > 0)
	{
		ASSERT( temp >= 0 && temp <= 0xFF);
		bNodeAddress = static_cast<BYTE>(temp);
	}
	else
	{	
		AfxMessageBox("Error - sscanf( ... )");
		ASSERT(0);	
	}
	return bNodeAddress;
}

BYTE CBindingFileEntryOperations::GetNrOfTimers() const
{
	ASSERT( m_szBindingFileEntry.IsEmpty() == FALSE );

	if (m_szBindingFileEntry.IsEmpty() == TRUE) {
		return 0;
	}

	BYTE nCnt = 0;
	int nIndex = 0;

	while ( (nIndex = m_szBindingFileEntry.Find("T",nIndex+1)) != -1 ) 
	{
		nCnt++;
	}

	return nCnt;
}

int CBindingFileEntryOperations::GetNrOfReferences() const
{
	int retValCount=0;

	ASSERT(m_szBindingFileEntry.IsEmpty() == FALSE);

	if (m_szBindingFileEntry.IsEmpty() != TRUE) 
	{	
		retValCount = calculateNrOfReferences(m_szBindingFileEntry);
	}
	return retValCount;
}

BOOL CBindingFileEntryOperations::ConditionBinding() const
{
	ASSERT( m_szBindingFileEntry.IsEmpty() == FALSE );

	if (m_szBindingFileEntry.IsEmpty() == TRUE) 
	{
		return FALSE;
	}
	return helper_IsConditionBinding(m_szBindingFileEntry);
}

int CBindingFileEntryOperations::GetCBindingConditionLength(void)
{
	int retValCount=-1;
	if (m_szBindingFileEntry.IsEmpty()==FALSE) 
	{
		if (TRUE==helper_IsConditionBinding(m_szBindingFileEntry))
		{
			retValCount=calculateCBindingConditionLength(m_szBindingFileEntry);		
		}
	}
	return retValCount;
}


/**
 * BOOL GetBindingUnits(CBindingUnits &BindingUnits)
 * 
 * 
 */
BOOL CBindingFileEntryOperations::GetBindingUnits(CBindingUnits &BindingUnits)
{
	CBindingFileEntry BindingFileEntry( m_szBindingFileEntry );
	CBindingFileEntryParser* pBindingFileEntryType = BindingFileEntry.Parse();
	
	if (pBindingFileEntryType != 0)
	{		
		/**
		 * Absorbe P-Bindings ...
		 */
		if (pBindingFileEntryType->IsTypeP() == TRUE)
		{			
			return TRUE;
		}

		/**
		 * Get BindingUnits ...
		 */
		if ((pBindingFileEntryType->IsTypeC() == TRUE) ||
			(pBindingFileEntryType->IsTypeCN() == TRUE))
		{
			BindingUnits = *(static_cast<CBindingFileEntryParserC*>
								(pBindingFileEntryType)->GetBindingUnits());
			return TRUE;
		}	

		/**
		 * Get ForwardUnit
		 */
		if ((pBindingFileEntryType->IsTypeB() == TRUE) ||
			(pBindingFileEntryType->IsTypeI() == TRUE) ||
			(pBindingFileEntryType->IsTypeG() == TRUE) ||
			(pBindingFileEntryType->IsTypeGN() == TRUE) ||
			(pBindingFileEntryType->IsTypeN() == TRUE) ||
			(pBindingFileEntryType->IsTypeNN() == TRUE) )
		{										
			CBindingUnit* const pBindingUnit = const_cast<CBindingUnit*>
				(pBindingFileEntryType->GetForwardUnit());

			if (pBindingUnit != 0)
			{
				/**
				 * Clone CBindingUnit Base class to CBindingUnit
				 */
				ASSERT( pBindingUnit->GetBindingUnitType() == 0);

				CBindingUnit* const pNewUnit = CBindingUnitFactory::Clone(pBindingUnit);
				if (pNewUnit != 0) 
				{
					ASSERT( pNewUnit->GetBindingUnitType() == 0);

					const unsigned short nId = BindingUnits.Add(pNewUnit);	
					if (nId == 0) 
					{
						ASSERT( nId );
						return FALSE;
					}
				}
			}											
		}		

		/**
		 * Get Return units ..
		 */
		if (pBindingFileEntryType->IsTypeB() == TRUE)
		{		
			CBindingUnit* const pBindingUnit = const_cast<CBindingUnit*>
				(static_cast<CBindingFileEntryParserB*>(pBindingFileEntryType)->GetReturnUnit());

			if (pBindingUnit != 0)
			{
				ASSERT(pBindingUnit->GetBindingUnitType() == 0);

				CBindingUnit* const pNewUnit = CBindingUnitFactory::Clone(pBindingUnit);
				if (pNewUnit != 0) 
				{
					ASSERT(pNewUnit->GetBindingUnitType() == 0);

					const unsigned short nId = BindingUnits.Add(pNewUnit);	
					if (nId == 0) 
					{
						ASSERT( nId );
						return FALSE;
					}
				}
			}							
			return TRUE;
		}		
		else if ((pBindingFileEntryType->IsTypeG() == TRUE) ||
				 (pBindingFileEntryType->IsTypeGN() == TRUE))
		{ 
			CBindingUnit* const pBindingUnit = const_cast<CBindingUnit*>
				(static_cast<CBindingFileEntryParserG*>(pBindingFileEntryType)->GetReturnUnit());

			if (pBindingUnit != 0)
			{
				ASSERT(pBindingUnit->GetBindingUnitType() == 0);

				CBindingUnit* const pNewUnit = CBindingUnitFactory::Clone(pBindingUnit);
				if (pNewUnit != 0) 
				{
					ASSERT(pNewUnit->GetBindingUnitType() == 0);

					const unsigned short nId = BindingUnits.Add(pNewUnit);	
					if (nId == 0) 
					{
						ASSERT( nId );
						return FALSE;
					}
				}
			}			
		}			
		else if (pBindingFileEntryType->IsTypeI() == TRUE)
		{
			CBindingUnit* const pBindingUnit = const_cast<CBindingUnit*>
				(static_cast<CBindingFileEntryParserI*>(pBindingFileEntryType)->GetReturnUnit());

			if (pBindingUnit != 0)
			{
				ASSERT(pBindingUnit->GetBindingUnitType() == 0);
				CBindingUnit* const pNewUnit = CBindingUnitFactory::Clone(pBindingUnit);
				if (pNewUnit != 0) 
				{
					ASSERT(pNewUnit->GetBindingUnitType() == 0);
					const unsigned short nId = BindingUnits.Add(pNewUnit);	
					if (nId == 0) 
					{
						ASSERT( nId );
						return FALSE;
					}
				}
			}	

			return TRUE;
		} 
		else if ((pBindingFileEntryType->IsTypeN() == TRUE) ||
				 (pBindingFileEntryType->IsTypeNN() == TRUE))
		{		
			CBindingUnit* const pBindingUnit = const_cast<CBindingUnit*>
				(static_cast<CBindingFileEntryParserN*>(pBindingFileEntryType)->GetReturnUnit());

			if (pBindingUnit != 0)
			{
				ASSERT(pBindingUnit->GetBindingUnitType() == 0);
				CBindingUnit* const pNewUnit = CBindingUnitFactory::Clone(pBindingUnit);
				if (pNewUnit != 0) 
				{
					ASSERT(pNewUnit->GetBindingUnitType() == 0);
					const unsigned short nId = BindingUnits.Add(pNewUnit);	
					if (nId == 0) 
					{
						ASSERT( nId );
						return FALSE;
					}
				}
			}						
		}

		
		/** 
		 * Get Next Unit 
		 */
		if ((pBindingFileEntryType->IsTypeNN() == TRUE) ||
			//(pBindingFileEntryType->IsTypeN() == TRUE) ||
			(pBindingFileEntryType->IsTypeGN() == TRUE) )
		{
			CBindingUnit* const pBindingUnit = const_cast<CBindingUnit*>
				(pBindingFileEntryType->GetNextUnit());

			if (pBindingUnit != 0)
			{
				ASSERT(pBindingUnit->GetBindingUnitType() == 0);

				CBindingUnit* const pNewUnit = CBindingUnitFactory::Clone(pBindingUnit);
				if (pNewUnit != 0) 
				{
					ASSERT(pNewUnit->GetBindingUnitType() == 0);
					const unsigned short nId = BindingUnits.Add(pNewUnit);	
					if (nId == 0) 
					{
						ASSERT( nId );
						return FALSE;
					}	
				}
			}
			return TRUE;
		}	
		
		return TRUE;
	}
	return FALSE;
}

BOOL CBindingFileEntryOperations::normalize(
	CString szIn,
	CString* szOut )	// BUG-0100
{
	if (!szIn.IsEmpty( ))
	{	
		// Alle karakters in de binding string moeten uppercase zijn...
		szIn.MakeUpper( );

		if (szIn.Find('T') != -1)
		{
			// Timer types moeten altijd uppercase 'T' en lowercase type.
			struct ReplaceEntry
			{
				const char* szOld;
				const char* szNew;
			};

			static const ReplaceEntry table[ ] =
			{
				{ "TE", "Te" },
				{ "TI", "Ti" },
				{ "TO",	"To" },
				{ "TD",	"Td" },			
				{ "TF", "Tf" },
				{ "TR", "Tr" },
				{  0,	0 }
			};		
		
			for (const ReplaceEntry* p = &table[0];
				 p->szOld != 0;
				 p++)
			{
				szIn.Replace(p->szOld, p->szNew);			 
			}	
		}
		
		// Sedert Versie 0x0C06:
		// Zorgt ervoor dat er telkens een underscore aanwezig is tussen het
		// binding nummer en het binding type.
		CBindingFileEntryOperations::Upgrade(szIn,szOut);

		// *szOut = szIn;
		return TRUE;
	}
	return FALSE;
}



// Sedert versie 0x0902
// Voor de vergelijking:
// - Eerste beide string normalizen.
// - Alle strings upgraden naar nieuw formaat.
BOOL CBindingFileEntryOperations::compare(
	const CString& refStr1,
	const CString& refStr2)
{
	BOOL fResult=FALSE;
	CString s1;

	if (TRUE==normalize(refStr1,&s1))
	{
		CString s2;
		if (TRUE==normalize(refStr2,&s2))
		{
			CString s11;
			CString s21;

			Upgrade(s1,&s11);
			Upgrade(s2,&s21);
			

			if (0==s11.Compare(s21))
			{
				fResult=TRUE;
			}		
		}
	}
	return fResult;
}

// 0123456789ABCDEF
// 00000A_00_016DC(A000025U03E01+A000025U04E01)=U02FB6D0103S aanpassen aan
// 00000A_00_016D_C(A000025U03E01+A000025U04E01)=U02FB6D0103S
void CBindingFileEntryOperations::Upgrade(
	const CString s,
	CString* pStrOut)
{
	const int idx = 14;

	*pStrOut = s;

	if (pStrOut->GetAt(idx)!='_')
	{
		pStrOut->Insert(idx,'_');
	}	

	ASSERT(pStrOut->GetAt(idx)=='_');
}