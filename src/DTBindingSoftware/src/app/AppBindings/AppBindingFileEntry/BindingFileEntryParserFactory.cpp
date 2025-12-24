// BindingFileEntryParserFactory.cpp: implementation of the CBindingFileEntryParserFactory class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingFileEntryParserFactory.h"

#include "BindingFileEntry.h"
#include "BindingFileEntryParserB.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBindingFileEntryParserFactory::CBindingFileEntryParserFactory(CString s)
{
	m_szBindingFileEntry = s;
	m_pBindingFileEntryParser = 0;
}

CBindingFileEntryParserFactory::~CBindingFileEntryParserFactory()
{
	if ( m_pBindingFileEntryParser != 0 )
	{
		delete m_pBindingFileEntryParser;
		m_pBindingFileEntryParser = 0;
	}
}
///////////////////////////////////////////////////////////////////////
CBindingFileEntryParserB* CBindingFileEntryParserFactory::Get_TypeB()
{
	if (m_pBindingFileEntryParser == 0)
	{
		m_pBindingFileEntryParser = Get_TypeB(m_szBindingFileEntry);
		return (static_cast<CBindingFileEntryParserB*> (m_pBindingFileEntryParser));
	}
	return 0;
}
CBindingFileEntryParserC* CBindingFileEntryParserFactory::Get_TypeC()
{
	if (m_pBindingFileEntryParser == 0)
	{
		m_pBindingFileEntryParser = Get_TypeC(m_szBindingFileEntry);
		return (static_cast<CBindingFileEntryParserC*> (m_pBindingFileEntryParser));
	}
	return 0;
}
CBindingFileEntryParserCN* CBindingFileEntryParserFactory::Get_TypeCN()
{
	if (m_pBindingFileEntryParser == 0)
	{
		m_pBindingFileEntryParser = Get_TypeGN(m_szBindingFileEntry);
		return (static_cast<CBindingFileEntryParserCN*> (m_pBindingFileEntryParser));
	}
	return 0;
}
CBindingFileEntryParserG* CBindingFileEntryParserFactory::Get_TypeG()
{
	if (m_pBindingFileEntryParser == 0)
	{
		m_pBindingFileEntryParser = Get_TypeG(m_szBindingFileEntry);
		return (static_cast<CBindingFileEntryParserG*> (m_pBindingFileEntryParser));
	}
	return 0;
}
CBindingFileEntryParserGN* CBindingFileEntryParserFactory::Get_TypeGN()
{
	if (m_pBindingFileEntryParser == 0)
	{
		m_pBindingFileEntryParser = Get_TypeGN(m_szBindingFileEntry);
		return (static_cast<CBindingFileEntryParserGN*> (m_pBindingFileEntryParser));
	}
	return 0;
}
CBindingFileEntryParserI* CBindingFileEntryParserFactory::Get_TypeI()
{
	if (m_pBindingFileEntryParser == 0)
	{
		m_pBindingFileEntryParser = Get_TypeI(m_szBindingFileEntry);
		return (static_cast<CBindingFileEntryParserI*> (m_pBindingFileEntryParser));
	}
	return 0;
}
CBindingFileEntryParserN* CBindingFileEntryParserFactory::Get_TypeN()
{
	if (m_pBindingFileEntryParser == 0)
	{
		m_pBindingFileEntryParser = Get_TypeN(m_szBindingFileEntry);
		return (static_cast<CBindingFileEntryParserN*> (m_pBindingFileEntryParser));
	}
	return 0;
}
CBindingFileEntryParserNN* CBindingFileEntryParserFactory::Get_TypeNN()
{
	if (m_pBindingFileEntryParser == 0)
	{
		m_pBindingFileEntryParser = Get_TypeNN(m_szBindingFileEntry);
		return (static_cast<CBindingFileEntryParserNN*> (m_pBindingFileEntryParser));
	}
	return 0;
}
CBindingFileEntryParserP* CBindingFileEntryParserFactory::Get_TypeP()
{
	if (m_pBindingFileEntryParser == 0)
	{
		m_pBindingFileEntryParser = Get_TypeP(m_szBindingFileEntry);
		return (static_cast<CBindingFileEntryParserP*> (m_pBindingFileEntryParser));
	}
	return 0;
}
///////////////////////////////////////////////////////////////////////
CBindingFileEntryParser* CBindingFileEntryParserFactory::Create(CString s)
{
	if (s.IsEmpty() == TRUE) {
		return 0;
	}

	CBindingFileEntry BindingFileEntry(s);
	CBindingFileEntryParser* const pSource = BindingFileEntry.Parse(); 

	if (pSource == 0) {
		return 0;
	}

	try
	{
		if (pSource->IsTypeB() == TRUE) 
		{
			return (new CBindingFileEntryParserB(
				*(static_cast<CBindingFileEntryParserB*>(pSource))));
		}
		else if (pSource->IsTypeC() == TRUE)
		{
			return (new CBindingFileEntryParserC(
				*(static_cast<CBindingFileEntryParserC*>(pSource))));
		}
		else if (pSource->IsTypeCN() == TRUE)
		{
			return (new CBindingFileEntryParserCN(
				*(static_cast<CBindingFileEntryParserCN*>(pSource))));
		}
		else if (pSource->IsTypeG() == TRUE)
		{
			return (new CBindingFileEntryParserG(
				*(static_cast<CBindingFileEntryParserG*>(pSource))));
		}
		else if (pSource->IsTypeGN() == TRUE)
		{
			return (new CBindingFileEntryParserGN(
				*(static_cast<CBindingFileEntryParserGN*>(pSource))));
		}
		else if (pSource->IsTypeI() == TRUE)
		{
			return (new CBindingFileEntryParserI(
				*(static_cast<CBindingFileEntryParserI*>(pSource))));
		}
		else if (pSource->IsTypeN() == TRUE)
		{
			return (new CBindingFileEntryParserN(
				*(static_cast<CBindingFileEntryParserN*>(pSource))));
		}
		else if (pSource->IsTypeNN() == TRUE)
		{
			return (new CBindingFileEntryParserNN(
				*(static_cast<CBindingFileEntryParserNN*>(pSource))));
		}
		else if (pSource->IsTypeP() == TRUE)
		{
			return (new CBindingFileEntryParserP(
				*(static_cast<CBindingFileEntryParserP*>(pSource))));
		}
		else 
		{
			return (new CBindingFileEntryParser(
				*(static_cast<CBindingFileEntryParser*>(pSource))));
		}
	
	}
	catch(CMemoryException* e)
	{
		e->ReportError();
		e->Delete();
	}
	return 0;
}

CBindingFileEntryParserB* CBindingFileEntryParserFactory::Get_TypeB(CString s)
{
	CBindingFileEntryParserB* pNew = 0;	
	CBindingFileEntry BindingFileEntry(s);

	try
	{							
		CBindingFileEntryParserB* const pBindingFileEntryType = 
			static_cast<CBindingFileEntryParserB*>(BindingFileEntry.Parse());

		if (pBindingFileEntryType != 0)
		{		
			if (pBindingFileEntryType->IsTypeB() == TRUE)
			{
				pNew = new CBindingFileEntryParserB(*pBindingFileEntryType);
			}
		}
	}	
	catch(CMemoryException* e)
	{
		e->ReportError();
		e->Delete();
		
		if (pNew != 0) 
		{
			delete pNew;
		}
		return 0;
	}

	return pNew;
}

CBindingFileEntryParserC* CBindingFileEntryParserFactory::Get_TypeC(CString s)
{
	CBindingFileEntryParserC* pNew = 0;	
	CBindingFileEntry BindingFileEntry(s);

	try
	{							
		CBindingFileEntryParserC* const pBindingFileEntryType = 
			static_cast<CBindingFileEntryParserC*>(BindingFileEntry.Parse());

		if (pBindingFileEntryType != 0)
		{		
			if (pBindingFileEntryType->IsTypeC() == TRUE)
			{
				pNew = new CBindingFileEntryParserC(*pBindingFileEntryType);
			}
		}
	}	
	catch(CMemoryException* e)
	{
		e->ReportError();
		e->Delete();
		
		if (pNew != 0) 
		{
			delete pNew;
		}
		return 0;
	}

	return pNew;
}

CBindingFileEntryParserCN* CBindingFileEntryParserFactory::Get_TypeCN(CString s)
{
	CBindingFileEntryParserCN* pNew = 0;	
	CBindingFileEntry BindingFileEntry(s);

	try
	{							
		CBindingFileEntryParserCN* const pBindingFileEntryType = 
			static_cast<CBindingFileEntryParserCN*>(BindingFileEntry.Parse());

		if (pBindingFileEntryType != 0)
		{		
			if (pBindingFileEntryType->IsTypeCN() == TRUE)
			{	
				pNew = new CBindingFileEntryParserCN(*pBindingFileEntryType);
			}
		}
	}	
	catch(CMemoryException* e)
	{
		e->ReportError();
		e->Delete();
		
		if (pNew != 0) 
		{
			delete pNew;
		}
		return 0;
	}

	return pNew;
}

CBindingFileEntryParserG* CBindingFileEntryParserFactory::Get_TypeG(CString s)
{
	CBindingFileEntryParserG* pNew = 0;	
	CBindingFileEntry BindingFileEntry(s);

	try
	{							
		CBindingFileEntryParserG* const pBindingFileEntryType = 
			static_cast<CBindingFileEntryParserG*>(BindingFileEntry.Parse());

		if (pBindingFileEntryType != 0)
		{		
			if (pBindingFileEntryType->IsTypeG() == TRUE)
			{	
				pNew = new CBindingFileEntryParserG(*pBindingFileEntryType);
			}
		}
	}	
	catch(CMemoryException* e)
	{
		e->ReportError();
		e->Delete();
		
		if (pNew != 0) 
		{
			delete pNew;
		}
		return 0;
	}

	return pNew;
}

CBindingFileEntryParserGN* CBindingFileEntryParserFactory::Get_TypeGN(CString s)
{
	CBindingFileEntryParserGN* pNew = 0;	
	CBindingFileEntry BindingFileEntry(s);

	try
	{							
		CBindingFileEntryParserGN* const pBindingFileEntryType = 
			static_cast<CBindingFileEntryParserGN*>(BindingFileEntry.Parse());

		if (pBindingFileEntryType != 0)
		{		
			if (pBindingFileEntryType->IsTypeGN() == TRUE)
			{	
				pNew = new CBindingFileEntryParserGN(*pBindingFileEntryType);
			}
		}
	}	
	catch(CMemoryException* e)
	{
		e->ReportError();
		e->Delete();
		
		if (pNew != 0) 
		{
			delete pNew;
		}
		return 0;
	}

	return pNew;
}

CBindingFileEntryParserI* CBindingFileEntryParserFactory::Get_TypeI(CString s)
{
	CBindingFileEntryParserI* pNew = 0;	
	CBindingFileEntry BindingFileEntry(s);

	try
	{							
		CBindingFileEntryParserI* const pBindingFileEntryType = 
			static_cast<CBindingFileEntryParserI*>(BindingFileEntry.Parse());

		if (pBindingFileEntryType != 0)
		{		
			if (pBindingFileEntryType->IsTypeI() == TRUE)
			{		
				pNew = new CBindingFileEntryParserI(*pBindingFileEntryType);
			}
		}
	}	
	catch(CMemoryException* e)
	{
		e->ReportError();
		e->Delete();
		
		if (pNew != 0) 
		{
			delete pNew;
		}
		return 0;
	}

	return pNew;
}

CBindingFileEntryParserN* CBindingFileEntryParserFactory::Get_TypeN(CString s)
{
	CBindingFileEntryParserN* pNew = 0;	
	CBindingFileEntry BindingFileEntry(s);

	try
	{							
		CBindingFileEntryParserN* const pBindingFileEntryType = 
			static_cast<CBindingFileEntryParserN*>(BindingFileEntry.Parse());

		if (pBindingFileEntryType != 0)
		{		
			if (pBindingFileEntryType->IsTypeN() == TRUE)
			{	
				pNew = new CBindingFileEntryParserN(*pBindingFileEntryType);
			}
		}
	}	
	catch(CMemoryException* e)
	{
		e->ReportError();
		e->Delete();
		
		if (pNew != 0) 
		{
			delete pNew;
		}
		return 0;
	}

	return pNew;
}

CBindingFileEntryParserNN* CBindingFileEntryParserFactory::Get_TypeNN(CString s)
{
	CBindingFileEntryParserNN* pNew = 0;	
	CBindingFileEntry BindingFileEntry(s);

	try
	{							
		CBindingFileEntryParserNN* const pBindingFileEntryType = 
			static_cast<CBindingFileEntryParserNN*>(BindingFileEntry.Parse());

		if (pBindingFileEntryType != 0)
		{		
			if (pBindingFileEntryType->IsTypeNN() == TRUE)
			{	
				pNew = new CBindingFileEntryParserNN(*pBindingFileEntryType);
			}
		}
	}	
	catch(CMemoryException* e)
	{
		e->ReportError();
		e->Delete();
		
		if (pNew != 0) 
		{
			delete pNew;
		}
		return 0;
	}

	return pNew;
}

CBindingFileEntryParserP* CBindingFileEntryParserFactory::Get_TypeP(CString s)
{
	CBindingFileEntryParserP* pNew = 0;	
	CBindingFileEntry BindingFileEntry(s);

	try
	{							
		CBindingFileEntryParserP* const pBindingFileEntryType = 
			static_cast<CBindingFileEntryParserP*>(BindingFileEntry.Parse());

		if (pBindingFileEntryType != 0)
		{		
			if (pBindingFileEntryType->IsTypeP() == TRUE)
			{	
				pNew = new CBindingFileEntryParserP(*pBindingFileEntryType);
			}
		}
	}	
	catch(CMemoryException* e)
	{
		e->ReportError();
		e->Delete();
		
		if (pNew != 0) 
		{
			delete pNew;
		}
		return 0;
	}

	return pNew;
}
