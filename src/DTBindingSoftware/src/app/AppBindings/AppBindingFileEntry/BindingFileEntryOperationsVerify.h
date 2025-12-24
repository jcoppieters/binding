// BindingFileEntryOperationsVerify.h: interface for the CBindingFileEntryOperationsVerify class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGFILEENTRYOPERATIONSVERIFY_H__5F7ABC3C_85F7_4574_9FE3_14EABD97EF41__INCLUDED_)
#define AFX_BINDINGFILEENTRYOPERATIONSVERIFY_H__5F7ABC3C_85F7_4574_9FE3_14EABD97EF41__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "BindingFileEntryOperations.h"

class CBindingFileEntryParser;
class CBindingFileEntryParserP;
class CBindingUnit;
class CBindingPropertyUnit;

////////////////////////////////////////////////////////////////////////////////

/**
 *	used by CBindingConversionFromNodesOperations
 */
class CBindingFileEntryOperationsVerify : public CBindingFileEntryOperations  
{
public:
	CBindingFileEntryOperationsVerify() {};
	CBindingFileEntryOperationsVerify(CString s);
	virtual ~CBindingFileEntryOperationsVerify();

protected:
	BOOL VerifyPBinding(CBindingFileEntryParserP const *parser);

public:
	/**
	 * Check if Binding is correct for type CBindingPropertyUnit...
	 */
	BOOL Verify();	

	/**
	 * Correct bindingEntry - Only called by client if Verify fails...
	 */
	BOOL Correct(CString& szOut);

	/**
	 * Check if string can be parsed ...
	 */
	BOOL IsValid();
	
};


//////////////////////////////////////////////////////////////////////////////////


class CBindingFileEntryOperationsVerifyP : public CBindingFileEntryOperationsVerify
{
	friend class CBindingFileEntryOperationsVerify;

protected:
	CBindingFileEntryParserP* const m_pParser;
	CBindingPropertyUnit* m_pBindingPropertyUnit;

public:
	CBindingFileEntryOperationsVerifyP(CString s,
									   CBindingFileEntryParser* const pParser);

	CBindingFileEntryOperationsVerifyP(CBindingFileEntryParser* const pParser);

	virtual ~CBindingFileEntryOperationsVerifyP();

protected:
	BOOL FindPropertyUnit();

protected:
	virtual BOOL Verify();

	/**
	 * Correct FC_UNITSENSSET + BindingNr : 0x0000
	 */
	virtual BOOL Correct(CString& szOut);
};

//////////////////////////////////////////////////////////////////////////////////

// Todo for derived Types ...


//////////////////////////////////////////////////////////////////////////////////



#endif // !defined(AFX_BINDINGFILEENTRYOPERATIONSVERIFY_H__5F7ABC3C_85F7_4574_9FE3_14EABD97EF41__INCLUDED_)

