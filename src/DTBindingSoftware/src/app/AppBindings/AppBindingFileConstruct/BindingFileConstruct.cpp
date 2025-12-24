// BindingFileConstruct.cpp: implementation of the CBindingFileConstruct class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingFileConstruct.h"

#include "BindingEntryInfo.h"

#include "App\Appbindings\AppbindingFileEntry\BindingFileEntryParser.h"
#include "App\Appbindings\AppbindingFileEntry\BindingFileEntryOperations.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingFileConstruct::CBindingFileConstruct(CStringArray* pArrayBindingStrings) :
m_pArrayBindingStrings(pArrayBindingStrings) 
{	
	ASSERT( m_pArrayBindingStrings );
}

CBindingFileConstruct::~CBindingFileConstruct()
{
}

int CBindingFileConstruct::GetNrOfEntries(CStringArray* pStringArray,BYTE bNodeAddress)
{
	int nCount = 0;

	if (pStringArray != NULL)
	{
		const int max = pStringArray->GetSize();

		for (int i=0;i<max;i++)
		{					
			const CBindingFileEntryOperations Operations(pStringArray->GetAt(i));
			const BYTE _bNodeAddress = Operations.GetNodeAddress();

			if (bNodeAddress == _bNodeAddress) 
			{
				nCount++;
			}
		}
	}
	return nCount;
}
