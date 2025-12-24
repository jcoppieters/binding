// BindingFileEntryParserFactory.h: interface for the CBindingFileEntryParserFactory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGFILEENTRYPARSERFACTORY_H__94967F30_C4DE_4584_B92A_FC3CF0787279__INCLUDED_)
#define AFX_BINDINGFILEENTRYPARSERFACTORY_H__94967F30_C4DE_4584_B92A_FC3CF0787279__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CBindingFileEntryParser;
class CBindingFileEntryParserB;
class CBindingFileEntryParserC;
class CBindingFileEntryParserCN;
class CBindingFileEntryParserG;
class CBindingFileEntryParserGN;
class CBindingFileEntryParserI;
class CBindingFileEntryParserN;
class CBindingFileEntryParserNN;
class CBindingFileEntryParserP;


class CBindingFileEntryParserFactory  
{
protected:
	CString m_szBindingFileEntry;
	CBindingFileEntryParser* m_pBindingFileEntryParser;

public:
	CBindingFileEntryParserFactory(CString s);
	virtual ~CBindingFileEntryParserFactory();

public:	
	CBindingFileEntryParserB* Get_TypeB();
	CBindingFileEntryParserC* Get_TypeC();
	CBindingFileEntryParserCN* Get_TypeCN();
	CBindingFileEntryParserG* Get_TypeG();
	CBindingFileEntryParserGN* Get_TypeGN();
	CBindingFileEntryParserI* Get_TypeI();
	CBindingFileEntryParserN* Get_TypeN();
	CBindingFileEntryParserNN* Get_TypeNN();
	CBindingFileEntryParserP* Get_TypeP();

public:	
	static CBindingFileEntryParserB* Get_TypeB(CString s);
	static CBindingFileEntryParserC* Get_TypeC(CString s);
	static CBindingFileEntryParserCN* Get_TypeCN(CString s);
	static CBindingFileEntryParserG* Get_TypeG(CString s);
	static CBindingFileEntryParserGN* Get_TypeGN(CString s);
	static CBindingFileEntryParserI* Get_TypeI(CString s);
	static CBindingFileEntryParserN* Get_TypeN(CString s);
	static CBindingFileEntryParserNN* Get_TypeNN(CString s);
	static CBindingFileEntryParserP* Get_TypeP(CString s);

public:
	static CBindingFileEntryParser* Create(CString s);
};

#endif // !defined(AFX_BINDINGFILEENTRYPARSERFACTORY_H__94967F30_C4DE_4584_B92A_FC3CF0787279__INCLUDED_)
