// BindingConversion.h: interface for the CBindingConversion class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGCONVERSION_H__A914A3DD_772F_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_BINDINGCONVERSION_H__A914A3DD_772F_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////
#include "BindingConstants.h"
#include "Binding2Controls.h"
#include "BindingConnections.h"
#include "BindingLink.h"
#include "BindingLinks.h"
#include "BindingUnits.h"
//#include "BindingUnit.h"
#include "app\appbindings\appbindingunit\bindingunit_api.h"
#include "BindingOperators.h"
#include "BindingOperator.h"
#include "BindingEntry.h"
//////////////////////////////////////////////////////////////////////

enum BindingType
{
	Type_BG = 0, 
	Type_BGN,
	Type_BC, 
	Type_BCN,
	Type_I, 
	Type_P
};

class CBindingConversion  
{
public:
	CBindingConversion();
	virtual ~CBindingConversion();				
	BOOL ProcessBindingEntry(CBindingEntry *pEntry);

private:	
	void Init(void);
	BOOL BuildEvalStringLevel(void);
	BOOL CheckTypeBinding();
	BOOL GenerateEvalString(void);
	BOOL GenerateCExpression(CString szData,CString *szResult,int nInputs);
	BOOL GenerateTExpression(CString szData,CString *szResult);
	BOOL GenerateUExpression(CString szData,CString *szResult);
	void DebugInfo(CString szString);	

	// Helper Functions
	BOOL ParseInput(CString szData,CString *szResult,int nIndex);
	BOOL ParseOutput(CString szData,CString *szResult);
	int ParseNInputs(CString szData);
	int GetStartIndex(CString szData,int nInput);

	BOOL GetControlType(CString szData,char *cControlType);
	BOOL GetIdentifier(CString szData,unsigned short *nId);
	BOOL GetInverted(CString szData,BOOL *fInverted);
	BOOL BuildIdentifierString(CString *szResult,unsigned short nId,char cControlType,BOOL fInverse=FALSE);
	BOOL ConvertOtoX(CString szData,CString *szResult);
	BOOL ConvertTtoY(CString szData,CString *szResult);
	BOOL ProcessBindingUnits(void);
	BOOL ParseStringToBindingUnits(void);
	void CheckBindingName(void);

public:
	CBindingEntry *pBindingEntry;		// For Debug -Public

private:
	CBindingLinks *pColLinks;
	CBindingOperators *pColOperators;
	CBindingUnits *pColUnits;
	int _nTimers;
	int _nOperators;
	int _nOutputs;
	int _nInputs;
	int _nInverted;
	unsigned short nFirstId;
	unsigned short nLastId;
	BindingType TypeOfBinding;		// not yet implemented
	CStringArray szArrayLinkInfo;
	CString szEvaluationString;
};

#endif // !defined(AFX_BINDINGCONVERSION_H__A914A3DD_772F_11D8_B865_0050BAC412B1__INCLUDED_)
