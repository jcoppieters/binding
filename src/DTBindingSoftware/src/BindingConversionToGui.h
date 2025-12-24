// BindingConversionToGui.h: interface for the CBindingConversionToGui class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGCONVERSIONTOGUI_H__B40AFD41_7D7E_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_BINDINGCONVERSIONTOGUI_H__B40AFD41_7D7E_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



//////////////////////////////////////////////////////////////////////
#include "BindingConstants.h"

//class CMainFrame;
#include "BindingLink.h"
#include "BindingLinks.h"
#include "BindingUnits.h"
///#include "BindingUnit.h"
#include "app\appbindings\appbindingunit\bindingunit_api.h"
#include "BindingOperators.h"
#include "BindingOperator.h"
#include "BindingEntry.h"
#include "BindingGuiComponent.h"
#include "BindingConvertHelper.h"

//////////////////////////////////////////////////////////////////////
class CBindingConversionToGui  
{	
public:
	CBindingConversionToGui();
	virtual ~CBindingConversionToGui();

	//void StartDebug(int nTemplate);		//Test
	BOOL ProcessBindingEntry(CBindingEntry *pEntry);

public:
	CBindingEntry *pBindingEntry;		// For Debug -Public
	CBindingGuiComponent *pGuiComponent;

private:
	
	CStringArray szArrayLinkInfo;
	//CString szEvaluationString;
	CString szEvalOut,szEvalIn;

	CBindingLinks *pColLinks;
	CBindingOperators *pColOperators;
	CBindingUnits *pColUnits;

	//void Init(void);
	BOOL ProcessBindingEntry(void);

	BOOL ParseInputString(void);
	BOOL ParseOutputString(void);
	BOOL SetCalculatedPosition(void);	

	// Helper functions
	int FormatLevelString(CString*);
	BOOL AddOutputToString(const CString szOutput,int nIndex);
	BOOL ReplaceString(CString szData,CString *szEvalIn,CString *szReplace);
	BOOL SearchForOperator(CString szData,unsigned short *nId,char *cControlType,BOOL *fInverse);
};

#endif // !defined(AFX_BINDINGCONVERSIONTOGUI_H__B40AFD41_7D7E_11D8_B865_0050BAC412B1__INCLUDED_)
