#include "stdafx.h"
#include "BindingSoftwareDoc.h"

#include "bindingconv2nodes_api.h"

#include "app\appbindings\appbindingconv2nodes\bindingconversiontonodes_utils.h"
#include "app\appfilelog\appfilelogapi.h"

// -----------------------------------
#include "TMGlobals.h"

using namespace TMBindingDatabase;
using namespace TMNodeDatabase;
// -----------------------------------


// SearchBindingEntriesInBindingFiles( ).
BOOL CBindingConv2Nodes_Api::SearchBindingEntriesInBindingFiles(
	CStringArray* pStringArrayEC,
	void* pvUserData,
	pf_progress pf)
{
	BOOL fResult=TRUE;
	CPtrList listErrors;
	CBindingConversionToNodes_Utils::CompareResult_t res = 
			CBindingConversionToNodes_Utils::COMPARE_RESULT_SUCCESS;
		
	// Moeten wel callback functies meegeven ...
	res=CBindingConversionToNodes_Utils::SearchBindingEntriesInBindingFiles(
			*TMGetDocumentNodes(),
			*TMGetBindingEntries(),
			&listErrors,
			pvUserData,
			pf);

	if (res==CBindingConversionToNodes_Utils::COMPARE_RESULT_SUCCESS)
	{
		fResult=TRUE;
	}
	else
	{
		CStringArray strArray;

		CBindingConversionToNodes_Utils::formatStrArrECBindingEntriesInBindingFiles(
			listErrors,
			&strArray);

		CAppFileLogAPI::createLogFile(
			APPFILELOG_BINDINGFILES_COMP_EC,
			strArray);

		if (0!=pStringArrayEC)
		{
			pStringArrayEC->Copy(strArray);
		}
		fResult=FALSE;
	}
	return fResult;
}


int CBindingConv2Nodes_Api::AddBindingEntriesToBindingFiles(		
	CStringArray* pStringArrayEC,
	void* pvUserData,
	pf_progress pf)
{
	BOOL fResult=TRUE;
	CPtrList listErrors;
	int retVal = 0;

	retVal=CBindingConversionToNodes_Utils::AddBindingEntries(
			*TMGetBindingEntries(),
			&listErrors,
			pvUserData,
			pf);

	if (!listErrors.IsEmpty())
	{
		CStringArray strArray;

		CBindingConversionToNodes_Utils::formatStrArrECBindingEntriesInBindingFiles(
			listErrors,
			&strArray);

		CAppFileLogAPI::createLogFile(
			APPFILELOG_BINDINGFILES_ADD_EC,
			strArray);

		if (0!=pStringArrayEC)
		{
			pStringArrayEC->Copy(strArray);
		}
	}
	return retVal;
}


int CBindingConv2Nodes_Api::AddBindingPropertyToBindingFiles(
	CStringArray* pStringArrayEC,
	void* pvUserData,
	pf_progress pf)
{
	BOOL fResult=TRUE;
	CPtrList listErrors;
	int retVal = 0;

	retVal=CBindingConversionToNodes_Utils::AddBindingProperty(
					*TMGetDocumentNodes(),
					&listErrors,
					pvUserData,
					pf);

	if (!listErrors.IsEmpty())
	{
		CStringArray strArray;

		CBindingConversionToNodes_Utils::formatStrAttECBindingUnitsInBindingFiles(
			listErrors,
			&strArray);

		CAppFileLogAPI::createLogFile(
			APPFILELOG_BINDINGFILES_PROPERTY_ADD_EC,
			strArray);

		if (0!=pStringArrayEC)
		{
			pStringArrayEC->Copy(strArray);
		}
	}
	return retVal;
}

int CBindingConv2Nodes_Api::SearchManualBindings(
	CStringArray* pStringArray,
	void* pvUserData,
	pf_progress pf)
{
	int retVal=0;
	CStringArray strArray;

	retVal = CBindingConversionToNodes_Utils::SearchManualBindingEntries(
				&strArray,
				pvUserData,
				pf);

	CAppFileLogAPI::createLogFile(
		APPFILELOG_BINDINGFILES_MANUALBINDINGS,
		strArray);

	if (0!=pStringArray)
	{
		pStringArray->Copy(strArray);
	}
	return retVal;
}