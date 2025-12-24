// BindingFileWorker.h: interface for the CBindingFileWorker class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGFILEWORKER_H__CE651510_5FE0_42E2_9E01_E590F089DBA9__INCLUDED_)
#define AFX_BINDINGFILEWORKER_H__CE651510_5FE0_42E2_9E01_E590F089DBA9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



class CBindingFile;
class CBindingFileInfo;

// Bewerkingen op de binding strings in de binding files van de nodes.
class CBindingFileWorker  
{
	enum PARSINGSTATES 
	{
		STATE_NODEADDRESS=0,
		STATE_BINDINGFLAGS,
		STATE_BINDINGNR,
		STATE_BINDINGTYPE,
		STATE_EQUATION,
		STATE_FUNCTION,
		STATE_FORWARDADDRESS,
		STATE_WAIT_FOR_END
	};

protected:
	CBindingFile* const m_pBindingFile;
	CBindingFileInfo* m_pBindingFileInfo;

public:
	CBindingFileWorker(CBindingFile* pBindingFile);
	virtual ~CBindingFileWorker();

public:
	// CHANGES_BINDINGS_REFERENCES: Controle op MAX references toevoegd...
	// Oppassen: enkel op de bindingstrings die momenteel in de bindingfile aanwezig zijn.
	// Oppassen: Bij veranderen van een bestaande binding is deze functie niet van toepassing op deze 
	// binding. Deze binding wordt eerst gewist uit de binding files en opnieuw
	// toegevoegd.
	BOOL CalculateCBindingProperties();

	// CHANGES_CALCULATE_NROFSTRINGS
	// Oppassen: enkel op de bindingstrings die momenteel in de bindingfile aanwezig zijn.
	// Oppassen: Bij veranderen van een bestaande binding is deze functie niet van toepassing op deze 
	// binding. Deze binding wordt eerst gewist uit de binding files en opnieuw
	// toegevoegd.
	BOOL CalculateNrOfBindingStrings(void);

public:
	BOOL CheckNrOfBindings(int nEntries);
	BOOL CheckNrOfConditionBindings(int nEntries);
	BOOL CheckNrOfTimers(int nEntries);
	BOOL CheckNrOfReferences(int val);
};

#endif // !defined(AFX_BINDINGFILEWORKER_H__CE651510_5FE0_42E2_9E01_E590F089DBA9__INCLUDED_)
