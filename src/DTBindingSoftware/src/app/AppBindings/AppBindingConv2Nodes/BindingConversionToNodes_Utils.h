#pragma once

// Voorwaartse declaraties:
class CBindingEntries;
class CNodeDatabase;
class CPtrList;



// Functies:

struct CBindingConversionToNodes_Utils
{
public:

	typedef void (*pf_progress)(void* pvUserData,int step,int max);

	typedef enum
	{
		COMPARE_RESULT_SUCCESS		= 0,
		COMPARE_RESULT_EC_GENERAL	= -1,

	} CompareResult_t;

public:
	// Sedert versie 0x0902
	// Alle bindingentries uit de bindingdatabase overlopen en bijvoegen aan de binding
	// bestanden.
	// pListErrors bevat een lijst met bindingEntries die niet konden toegevoegd worden.
	// return waarde: 0 = succes, < 0 = fout.
	static 
	int AddBindingEntries(
		CBindingEntries& ref,
		CPtrList* pListErrors,
		void* pvUserData=0,
		pf_progress pf=0);

	static
	int AddBindingProperty(
		CNodeDatabase& refNodeDatabase,
		CPtrList* pListErrors,
		void* pvUserData=0,
		pf_progress pf=0);


	// Controleren verschillen dbase binding files & binding entries.
	static
	CompareResult_t SearchBindingEntriesInBindingFiles(
		CNodeDatabase& refNodeDatabase,
		CBindingEntries& ref,
		CPtrList* pListErrors,
		void* pvUserData=0,
		pf_progress pf=0);


	// Zoeken of er manuele bindings aanwezig zijn.
	// pStringArray [OUT] array met manuele binding strings.
	// return waarde: aantal
	static 
	int SearchManualBindingEntries(
		CStringArray* pStringArray,
		void* pvUserData=0,
		pf_progress pf=0);


	// refListErrors bevat referenties naar binding entries.
	static
	int formatStrArrECBindingEntriesInBindingFiles(
		CPtrList& refListErrors,
		CStringArray* pStringArray);


	// refListErrors bevat referenties naar binding property units.
	static
	int formatStrAttECBindingUnitsInBindingFiles(
		CPtrList& refListErrors,
		CStringArray* pStringArray);

};

