// BindingFileConstructFactory.cpp: implementation of the CBindingFileConstructFactory class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingFileConstructFactory.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingFileConstructFactory::CBindingFileConstructFactory(CBindingEntryInfo* pBindingEntryInfo,CStringArray* pArrayBindingStrings) :
m_pBindingEntryInfo(pBindingEntryInfo), m_pArrayBindingStrings(pArrayBindingStrings)
{
	m_pBindingFileConstruct = NULL;
}

CBindingFileConstructFactory::~CBindingFileConstructFactory()
{
	if (m_pBindingFileConstruct != NULL) 
	{
		delete m_pBindingFileConstruct;
		m_pBindingFileConstruct = NULL;
	}
}

CBindingFileConstructFormatter* CBindingFileConstructFactory::Create()
{
	try
	{
		if (m_pBindingFileConstruct == NULL)
		{
			switch(m_pBindingEntryInfo->Get_Type())
			{			
				case TYPE_I_BINDING:
				{
					m_pBindingFileConstruct = new 
						CBindingFileConstructFormatterI(m_pBindingEntryInfo,m_pArrayBindingStrings);										

				} break;

				case TYPE_P_BINDING:
				{
					ASSERT(0);				

				} break;

				case TYPE_BG_BINDING:
				{
					m_pBindingFileConstruct = new 
						CBindingFileConstructFormatterBG(m_pBindingEntryInfo,m_pArrayBindingStrings);	
								
				} break;

				case TYPE_BGN_BINDING:
				{
					m_pBindingFileConstruct = new 
						CBindingFileConstructFormatterBGN(m_pBindingEntryInfo,m_pArrayBindingStrings);	

				} break;

				case TYPE_BC_BINDING:
				{
					m_pBindingFileConstruct = new 
						CBindingFileConstructFormatterBC(m_pBindingEntryInfo,m_pArrayBindingStrings);	

				} break;

				case TYPE_BCN_BINDING:
				{
					m_pBindingFileConstruct = new 
						CBindingFileConstructFormatterBCN(m_pBindingEntryInfo,m_pArrayBindingStrings);

				} break;
		
				case TYPE_UNDEFINED_BINDING:
				{
					ASSERT(0);					
				
				} break;
			}
		}
	}

	catch (CMemoryException *e)
	{
		e->ReportError();
		e->Delete();	
	}
	return m_pBindingFileConstruct;
}