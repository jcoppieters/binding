// BindingFileEntryParserC.h: interface for the CBindingFileEntryParserC class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGFILEENTRYPARSERC_H__945C5DD5_7F16_4650_94CD_3D1FE66A632C__INCLUDED_)
#define AFX_BINDINGFILEENTRYPARSERC_H__945C5DD5_7F16_4650_94CD_3D1FE66A632C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BindingFileEntryParser.h"
#include "BindingUnits.h"
#include "BindingOperators.h"

#include "App\AppBindings\AppBindingEvalString\BindingEvaluationString.h"

class CBindingFileEntryParserC : public CBindingFileEntryParser  
{
	friend class CBindingFileEntry;

protected:

	typedef BOOL (CBindingFileEntryParserC::*ParseSubAction)
		(const char);

protected:

	CBindingUnits m_BindingUnits;
	CBindingOperators m_BindingOperators;
	CBindingEvaluationString m_BindingEvalString;

	//BOOL m_fInverseUnit;	// New 20060615

protected:
	CBindingFileEntryParserC();

public:
	virtual ~CBindingFileEntryParserC();

protected:

	/**
	 * Sub FSM ...
	 */
	ParseSubAction CurrentSubState;

	BOOL ParseBindingUnit(const char c);
	BOOL ParseTimerOperator(const char c);
	BOOL ParseNormal(const char c);

	/**
	 * Main FSM ...
	 */
	virtual BOOL IsTypeOf();
	virtual BOOL ParseConditionString(const char c);

public:
	virtual BOOL IsTypeC() const {
		return TRUE;
	}

public:

	CBindingOperators* GetBindingOperators() {
		return (&m_BindingOperators);
	}
	CBindingUnits* GetBindingUnits() {
		return (&m_BindingUnits);
	}
	CBindingEvaluationString* GetBindingEvaluationString() {
		return (&m_BindingEvalString);
	}
};

#endif // !defined(AFX_BINDINGFILEENTRYPARSERC_H__945C5DD5_7F16_4650_94CD_3D1FE66A632C__INCLUDED_)
