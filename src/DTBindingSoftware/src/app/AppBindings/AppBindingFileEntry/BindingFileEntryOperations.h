// BindingFileEntryOperations.h: interface for the CBindingFileEntryOperations class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGFILEENTRYOPERATIONS_H__59017684_4A34_4FD3_8C8E_2B1177FEB7CB__INCLUDED_)
#define AFX_BINDINGFILEENTRYOPERATIONS_H__59017684_4A34_4FD3_8C8E_2B1177FEB7CB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CBindingUnits;


// CHANGES_BINDINGS_REFERENCES: Bepalen van het aantal references in een string
// CHANGES_BINDINGS_CONDITIONLENGTH: Bepalen van de conditie lengte van de C-Binding

// Acties uitgevoerd op een binding file entry string.
class CBindingFileEntryOperations  
{
public:
	CBindingFileEntryOperations() {};
	CBindingFileEntryOperations(CString s);
	virtual ~CBindingFileEntryOperations();

	BOOL ReplaceMessageCode(BYTE bMessageCode,CString& szReturn);
	BOOL ConvertPropertyString(CString &szOut,BOOL fProcessed);
	BOOL ReplaceBindingNr(unsigned short usBindingNr,CString& szReturn);
	BYTE GetNodeAddress() const;
	BYTE GetNrOfTimers() const;	
	int GetNrOfReferences() const;

	BOOL ConditionBinding() const;
	BOOL GetBindingUnits(CBindingUnits &BindingUnits);
	int GetCBindingConditionLength(void);
	unsigned short GetBindingNr() const;

	static BOOL normalize(CString szIn,CString* szOut);		// BUG-0100

	// Sedert versie 0x0902
	static BOOL compare(
		const CString& refStr1,
		const CString& refStr2);

	// Sedert versie 0x0902
	// 00000A_00_016DC(A000025U03E01+A000025U04E01)=U02FB6D0103S aanpassen aan
	// 00000A_00_016D_C(A000025U03E01+A000025U04E01)=U02FB6D0103S
	static void Upgrade(const CString s,CString* pSZOut);


protected:
	CString m_szBindingFileEntry;
};

#endif // !defined(AFX_BINDINGFILEENTRYOPERATIONS_H__59017684_4A34_4FD3_8C8E_2B1177FEB7CB__INCLUDED_)
