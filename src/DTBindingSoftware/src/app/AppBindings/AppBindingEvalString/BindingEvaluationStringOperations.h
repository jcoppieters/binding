// BindingEvaluationStringOperations.h: interface for the CBindingEvaluationStringOperations class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGEVALUATIONSTRINGOPERATIONS_H__658C3419_A1FE_45B4_82CA_6ED1EF236549__INCLUDED_)
#define AFX_BINDINGEVALUATIONSTRINGOPERATIONS_H__658C3419_A1FE_45B4_82CA_6ED1EF236549__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



#include "BindingLinks.h"
#include "BindingEvaluationString.h"
#include "BindingEvaluationStringParser.h"

class CLog;
class CBindingUnits;
class CBindingOperators;


/**
 * Task : Update evaluation string: Replace ID's of units + timers.
 *
 *	ID: m_pOldBindingUnits --> ID: m_pNewBindingUnits
 *	ID: m_pOldBindingOperators --> ID: m_pNewBindingOperators
 *
 *
 */
class CBindingEvaluationStringOperations : public IBindingEvaluationStringParserListener
{
protected:
	CLog* const m_pErrorLog;
	CBindingEvaluationString* const m_pBindingEvaluationString;
	CBindingEvaluationStringParser* m_pParser;
	CBindingEvaluationString* m_pBindingNewEvaluationString;
	
	volatile BOOL m_fParseCorrect;
	volatile BOOL m_fParseFatalError;

	typedef enum
	{
		MODE_UPDATE = 0,
		MODE_VERIFY = 1

	} ENUM_FSM_MODE;

	ENUM_FSM_MODE CurrentMode;

protected:
	const CBindingUnits* m_pOldBindingUnits;
	CBindingUnits* m_pNewBindingUnits;
	const CBindingOperators* m_pOldBindingOperators;
	CBindingOperators* m_pNewBindingOperators;

public:
	CBindingEvaluationStringOperations(CBindingEvaluationString* p);

	CBindingEvaluationStringOperations(CBindingEvaluationString* p,
									   const CBindingUnits* const pOldUnits,
									   CBindingUnits* const pNewUnits,
									   const CBindingOperators* const pOldOperators,
									   CBindingOperators* const pNewOperators);

	virtual ~CBindingEvaluationStringOperations();

protected:
	void Init();

public:

	BOOL UpdateData();
	BOOL Append(const char c,
				CBindingEvaluationString* pNewEvaluationString);

	CLog* GetErrorLog() {
		return m_pErrorLog;
	}

public:
	virtual void OnInputUnit(unsigned short nID);	
	virtual void OnOutputUnit(unsigned short nID);
	virtual void OnNextUnit(unsigned short nID);
	virtual void OnTimer(unsigned short nID);
	virtual void OnCharOperator(const char c);
	virtual void OnCharSyntax(const char c);
	virtual void OnBrackets(const char c);
	virtual void OnError();

protected:
	BOOL ReplaceUnit(unsigned short nID);
	BOOL ReplaceUnit(unsigned short nID,int nEquals);
	BOOL ReplaceTimer(unsigned short nID);
	int CountEquals(unsigned short nID);
	
};

#endif // !defined(AFX_BINDINGEVALUATIONSTRINGOPERATIONS_H__658C3419_A1FE_45B4_82CA_6ED1EF236549__INCLUDED_)
