// BindingFileEntryParser.h: interface for the CBindingFileEntryParser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGFILEENTRYPARSER_H__DADB6470_297E_4DE3_95D0_228D3519CF8A__INCLUDED_)
#define AFX_BINDINGFILEENTRYPARSER_H__DADB6470_297E_4DE3_95D0_228D3519CF8A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/******************************************************************/
//#include "BindingUnit.h"
#include "app\appbindings\appbindingunit\bindingunit_api.h"
/******************************************************************/

/***************************************************************//**
 * @class	CBindingFileEntryParser
 * @brief	Basisklasse voor een binding file entry parser.
 * @author	Mitchell Tom
 * 
 * @changes	TM,v0562: Klasse opgekuist, inline functies.
 ******************************************************************/
class CBindingFileEntryParser  
{
public:
	CBindingFileEntryParser();
	CBindingFileEntryParser(CString s);
	virtual ~CBindingFileEntryParser();
	virtual BOOL IsTypeB() const;
	virtual BOOL IsTypeC() const;
	virtual BOOL IsTypeG() const;
	virtual BOOL IsTypeCN() const;
	virtual BOOL IsTypeGN() const;
	virtual BOOL IsTypeI() const;
	virtual BOOL IsTypeN() const;
	virtual BOOL IsTypeNN() const;
	virtual BOOL IsTypeP() const;
	BYTE GetNodeAddress() const;
	BYTE GetBindingFlags() const;
	unsigned short GetBindingNr() const;
	const CBindingUnit* GetForwardUnit() const;
	const CBindingUnit* GetNextUnit() const;
	BOOL Parse(CString s);		// Operation !

protected:
	/* FSM State Handlers */
	BOOL ParseNodeAddress(const char c);
	BOOL ParseBindingFlags(const char c);
	BOOL ParseBindingNumber(const char c);
	BOOL ParseBindingType(const char c);	
	BOOL AsciiToHex(const char c,BYTE *bHex);
	virtual BOOL ParseReturnUnit(const char c);
	virtual BOOL ParseForwardUnit(const char c);
	virtual BOOL ParseNextUnit(const char c);
	virtual BOOL ParseConditionString(const char c);
	virtual BOOL IsTypeOf();

protected:
	typedef BOOL (CBindingFileEntryParser::*ParseAction)(const char);

	CString m_szBindingFileEntry;	
	BYTE m_bNodeAddress;
	BYTE m_bBindingFlags;
	unsigned short m_usBindingNr;
	CBindingUnit m_ForwardUnit;
	CBindingUnit m_NextUnit;
	int m_nParserCount;					/* FSM State */
	ParseAction CurrentState;			/* FSM State */
};
/******************************** inline functions *******************************/
inline BYTE CBindingFileEntryParser::GetNodeAddress() const 
{
	return m_bNodeAddress;
}
inline BYTE CBindingFileEntryParser::GetBindingFlags() const 
{
	return m_bBindingFlags;
}
inline unsigned short CBindingFileEntryParser::GetBindingNr() const 
{
	return m_usBindingNr;
}
inline const CBindingUnit* CBindingFileEntryParser::GetForwardUnit() const
{
	return &m_ForwardUnit;
}
inline const CBindingUnit* CBindingFileEntryParser::GetNextUnit() const 
{
	return &m_NextUnit;
}
inline BOOL CBindingFileEntryParser::IsTypeB() const 
{
	return FALSE;
}
inline BOOL CBindingFileEntryParser::IsTypeC() const 
{
	return FALSE;
}
inline BOOL CBindingFileEntryParser::IsTypeG() const 
{
	return FALSE;
}
inline BOOL CBindingFileEntryParser::IsTypeCN() const 
{
	return FALSE;
}
inline BOOL CBindingFileEntryParser::IsTypeGN() const 
{
	return FALSE;
}
inline BOOL CBindingFileEntryParser::IsTypeI() const 
{
	return FALSE;
}
inline BOOL CBindingFileEntryParser::IsTypeN() const 
{
	return FALSE;
}
inline BOOL CBindingFileEntryParser::IsTypeNN() const 
{
	return FALSE;
}
inline BOOL CBindingFileEntryParser::IsTypeP() const 
{
	return FALSE;
}
/*************************** protected inline functions **************************/
inline BOOL CBindingFileEntryParser::IsTypeOf()
{
	return FALSE;
}
/*********************************************************************************/

#endif // !defined(AFX_BINDINGFILEENTRYPARSER_H__DADB6470_297E_4DE3_95D0_228D3519CF8A__INCLUDED_)
