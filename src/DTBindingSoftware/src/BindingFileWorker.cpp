// BindingFileWorker.cpp: implementation of the CBindingFileWorker class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingFileWorker.h"

#include "BindingFile1.h"
#include "BindingFileInfo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define DEBUG_LOCAL_ON		1
//////////////////////////////////////////////////////////////////////
#if (DEBUG_LOCAL_ON==1)
	#define TRACER(x)				TRACE x
#else
	#define TRACER(x ...)
#endif
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
#if (DEBUG_LOCAL_ON==1)
// Dumpen van alle references in 1 binding.
static
void dumpReferences(
	int bindingNR,
	const CStringArray& arr)
{
	const int length = arr.GetCount();

	TRACE("--- References: %04X ---\r\n",bindingNR);
	for (int i=0;i<length;++i)
	{
		const CString s(arr.GetAt(i));
		TRACE("ref %02d: %s\r\n",i,s);
	}
	TRACE("------------------------\r\n");
}
#endif
//////////////////////////////////////////////////////////////////////

CBindingFileWorker::CBindingFileWorker(CBindingFile* pBindingFile) :
m_pBindingFile(pBindingFile),
m_pBindingFileInfo(pBindingFile->GetBindingFileInfo())
{
	ASSERT(m_pBindingFile);
	ASSERT(m_pBindingFileInfo);
}

CBindingFileWorker::~CBindingFileWorker()
{

}

BOOL CBindingFileWorker::CheckNrOfBindings(int nEntries)
{
	ASSERT(m_pBindingFileInfo);

	if (m_pBindingFileInfo != NULL)
	{
		if (m_pBindingFileInfo->GetNodeMaxBindings()-m_pBindingFileInfo->GetNrBinding() >= nEntries)
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CBindingFileWorker::CheckNrOfConditionBindings(int nEntries)
{
	ASSERT(m_pBindingFileInfo);

	if (m_pBindingFileInfo != NULL)
	{
		if ((m_pBindingFileInfo->GetNodeMaxCBindings()-m_pBindingFileInfo->GetNrCBindings())>=nEntries)
		{
			return TRUE;
		}
	}
	return FALSE;
}


BOOL CBindingFileWorker::CheckNrOfTimers(int nEntries)
{
	ASSERT(m_pBindingFileInfo);

	if (m_pBindingFileInfo != NULL)
	{
		if ((m_pBindingFileInfo->GetNodeMaxTimers()-m_pBindingFileInfo->GetNrTimers())>=nEntries)
		{
			return TRUE;
		}
	}
	return FALSE;
}

// Zie CHANGES_BINDINGS_REFERENCES
BOOL CBindingFileWorker::CheckNrOfReferences(int nEntries)
{
	ASSERT(m_pBindingFileInfo);

	if (m_pBindingFileInfo != NULL)
	{
		if ((m_pBindingFileInfo->GetNodeMaxReferences()-m_pBindingFileInfo->GetNrReferences())>=nEntries)
		{
			return TRUE;
		}
	}
	return FALSE;	
}



// CHANGES_CALCULATE_NROFSTRINGS
// Bewerkingen op de binding strings in de binding files van de nodes.
// Oppassen: Enkel bewerkingen op de binding strings die aanwezig zijn in de binding files.
// Oppassen: Bij het toevoegen van een nieuwe binding of veranderen van een bestaande binding 
// zijn de binding strings niet aanwezig in de binding files van de nodes.
BOOL CBindingFileWorker::CalculateNrOfBindingStrings(void)
{
	BOOL fResult = FALSE;
	ASSERT(m_pBindingFile);

	if (m_pBindingFile)
	{
		const int NrOfBindings = m_pBindingFile->Count();
		CBindingFileInfo_Util::reset(m_pBindingFileInfo);
		CBindingFileInfo_Util::setNrOfBindings(m_pBindingFileInfo,NrOfBindings);
		fResult=TRUE;
	}
	return fResult;
}

// Bewerkingen op de binding strings in de binding files van de nodes.
// Oppassen: Enkel bewerkingen op de binding strings die aanwezig zijn in de binding files.
// Oppassen: Bij het toevoegen van een nieuwe binding of veranderen van een bestaande binding 
// zijn de binding strings niet aanwezig in de binding files van de nodes.
BOOL CBindingFileWorker::CalculateCBindingProperties(void)
{
	ASSERT(m_pBindingFile);

	PARSINGSTATES CurrentState;
	const int NrOfBindings = m_pBindingFile->Count();
	CBindingFileInfo_Util::reset(m_pBindingFileInfo);
	CBindingFileInfo_Util::setNrOfBindings(m_pBindingFileInfo,NrOfBindings);

	// Array bevat de string references in één binding string.
	CStringArray strArrayReferences;

#if (DEBUG_LOCAL_ON==1)
	int currBindingNR;
#endif

	BOOL fBusyCBinding = FALSE;
	
	for (int i=0,idxString=0;
		 i<NrOfBindings;
		 i++,idxString=0) 
	{
		const CString s(m_pBindingFile->GetString(i));			
		const int nLength = s.GetLength();
		CurrentState = STATE_NODEADDRESS;
		
		for (int j=0;j<nLength;j++) 
		{
			const char c = s.GetAt(j);
			
			switch(CurrentState)
			{
				case STATE_NODEADDRESS:		
				{
#if (DEBUG_LOCAL_ON==1)
					currBindingNR = -1;
#endif
					idxString++;

					if (c == '_') 
					{
						if (idxString != 7) 
						{
							ASSERT(0);
							return FALSE;
						}

						idxString = 0;
						fBusyCBinding = FALSE;
						CurrentState = STATE_BINDINGFLAGS;					
					}
				} break;

				case STATE_BINDINGFLAGS:
				{
					idxString++;

					if (c == '_') 
					{
						if (idxString != 3) 
						{
							ASSERT(0);
							return FALSE;
						}

						idxString = 0;
						CurrentState = STATE_BINDINGNR;
					}
				} break;

				case STATE_BINDINGNR:
				{
					if (idxString++ == 4) 
					{
#if (DEBUG_LOCAL_ON==1)
						if (j>4)
						{
							const char* const szTemp = &((LPCTSTR)s)[j-4];
							if ((0!=szTemp) && (strlen(szTemp)>4))
							{
								int tempVal;
								if (sscanf(szTemp,"%04x",&tempVal)==1)
								{
									currBindingNR = tempVal;
								}
							}
						}
#endif
						CurrentState = STATE_BINDINGTYPE;
					}

				} break;

				case STATE_BINDINGTYPE:
				{
					if (c == 'C' || c == 'c') 
					{												
						CBindingFileInfo_Util::incNrOfCBindings(m_pBindingFileInfo);

						fBusyCBinding=TRUE;
						idxString = 0;											
						strArrayReferences.RemoveAll();

						CurrentState = STATE_EQUATION;
					}
					else 
					{
						CurrentState = STATE_WAIT_FOR_END;
					}
				} break;

				case STATE_EQUATION:
				{	
					idxString++;

					if (c == 'T'  || c == 't') 
					{				
						ASSERT(fBusyCBinding==TRUE);
						CBindingFileInfo_Util::incNrOfTimers(m_pBindingFileInfo);
					}
// zie CHANGES_BINDINGS_REFERENCES
					else if (c == 'U' || c == 'u')
					{
						if (TRUE==fBusyCBinding)
						{							
							if (j>7)
							{
								const char* const szTemp = &((LPCTSTR)s)[j-7];

								if ((0!=szTemp) && (strlen(szTemp)>13))
								{
									const int count = strArrayReferences.GetCount();
									int i;

									CString szReference(szTemp,13);

									// Moeten controleren dat het telkens een andere reference is
									// Een zelfde reference is altijd in dezelfde binding en heeft hetzelfde node,
									// unit address en event.
													
									for(i=0;i<count;++i)
									{
										if (strArrayReferences[i] == szReference)
										{
											break;
										}
									}

									if (i==count)
									{
										if (strArrayReferences.Add(szReference)>=0)
										{									
											CBindingFileInfo_Util::incNrOfReferences(m_pBindingFileInfo);
										}
									}
								}
							}
						}
					}
					else if ((c == '*') ||
            				 (c == '+') ||
							 (c == '^') ||
            				 (c == '!') ||
            				 (c == '(') ||
            				 (c == ')'))            				 
					{
						ASSERT(fBusyCBinding==TRUE);
					}
					else if (c == '=') 
					{
						idxString = 0;				

						if (TRUE==fBusyCBinding)
						{
#if (DEBUG_LOCAL_ON==1)
							// Hier alle references dumpen in een binding ...
							dumpReferences(currBindingNR,strArrayReferences);
#endif
						}
						
						CurrentState = STATE_FUNCTION;
					}
				} break;

				case STATE_FUNCTION:
				{
					if (c == '>') 
					{
						idxString = 0;
						CurrentState = STATE_FORWARDADDRESS;
					}
					else if (c=='S' || c=='s') 
					{
						if (j != nLength-1) 
						{
							ASSERT(0);
							return FALSE;
						}				
					}
				} break;

				case STATE_FORWARDADDRESS:
				{
					if (c=='S' || c=='s') 
					{
						if (j != nLength-1) 
						{
							ASSERT(0);
							return FALSE;
						}
					}
				} break;

				case STATE_WAIT_FOR_END:
				{
					if (c=='S' || c=='s') 
					{		
						if (j != nLength-1) 
						{
							ASSERT(0);
							return FALSE;
						}
					}
				} break;
			}		
		}
	}
	return TRUE;
}

