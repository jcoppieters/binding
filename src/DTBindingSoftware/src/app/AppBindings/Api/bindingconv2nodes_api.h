#pragma once


// CBindingConv2Nodes_Api.

struct CBindingConv2Nodes_Api
{
public:
	typedef void (*pf_progress)(void* pvUserData,int step,int max);

	// Zoeken of alle binding entries in de binding files gevonden worden.
	static 
	BOOL SearchBindingEntriesInBindingFiles(
		CStringArray* pStringArrayEC,
		void* pvUserData=0,
		pf_progress pf=0);

	// Alle binding entries toevoegen aan de binding files.
	static 
	int AddBindingEntriesToBindingFiles(		
		CStringArray* pStringArrayEC,
		void* pvUserData=0,
		pf_progress pf=0);

	// Alle bindingstrings voor de binding property units toevoegen aan de binding files.
	static
	int AddBindingPropertyToBindingFiles(
		CStringArray* pStringArrayEC,
		void* pvUserData=0,
		pf_progress pf=0);

	// Zoeken of er manuele bindings gevonden worden.
	static
	int SearchManualBindings(
		CStringArray* pStringArray,
		void* pvUserData=0,
		pf_progress pf=0);
	
}; /* CBindingConv2Nodes_Api */
