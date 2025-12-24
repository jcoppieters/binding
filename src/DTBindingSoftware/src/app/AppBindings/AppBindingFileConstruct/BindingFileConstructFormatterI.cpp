// BindingFileConstructFormatterI.cpp: implementation of the CBindingFileConstructFormatterI class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingFileConstructFormatterI.h"

#include "app\appbindings\appbindingunit\bindingunit_def.h"
#include "App\Appbindings\AppbindingFileEntry\BindingFileEntryFormatterI.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingFileConstructFormatterI::CBindingFileConstructFormatterI(CBindingEntryInfo* pBindingEntryInfo,
																 CStringArray* pArrayBindingStrings) :
CBindingFileConstructFormatter(pBindingEntryInfo,pArrayBindingStrings)
{

}

CBindingFileConstructFormatterI::~CBindingFileConstructFormatterI()
{

}

BOOL CBindingFileConstructFormatterI::Format()
{
	CBindingUnit* pReturnUnit = NULL;
	CBindingUnit* pForwardUnit = NULL;
	CBindingUnit* pUnit = NULL;

	CBindingUnits *const pBindingUnits  = m_pBindingEntry->GetColUnit();
	ASSERT(pBindingUnits);

	for (int i=0;
		 i<pBindingUnits->Count();
		 i++) 
	{	
		if ((pUnit = pBindingUnits->Get(i)) != NULL)
		{
			if (pUnit->GetInOutType() == UNIT_TYPE_INPUT) 
			{
				pReturnUnit = pUnit;
			}
			else  
			{
				pForwardUnit = pUnit;
			}
		}
		
		ASSERT(pUnit);
	}

	//ASSERT(i == 2);		// CHANGES_MVS_2008
	ASSERT(pForwardUnit != NULL);
	ASSERT(pReturnUnit != NULL);
	ASSERT(pForwardUnit != pReturnUnit);

	if (pForwardUnit != NULL && pReturnUnit != NULL)
	{
		CBindingFileEntryFormatterI IBinding(m_pBindingEntryInfo,
											 pForwardUnit,
											 pReturnUnit);

		if (IBinding.Format() == TRUE) 
		{
			CString szBindingString = IBinding.GetBindingFileEntry();
			AddToStringArray(szBindingString);
			return TRUE;		
		}	
		ASSERT(0);
	}
	return FALSE;
}




