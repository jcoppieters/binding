#ifndef INCLUDED_BINDINGAPPCOMMON_H
#define INCLUDED_BINDINGAPPCOMMON_H
/********************************************************************/

// Sedert V16.17
struct BindingStatusError
{
	typedef enum {
		
		BINDING_SUCCESS = 0,		
		BINDING_EC_GENERAL = -1,		
		BINDING_EC_PROCESS = -10,				// Probleem met het processen van de binding entry
		BINDING_EC_DBASE_ENTRIES = -20,			// Probleem met de database van de entries.
		BINDING_EC_BINDINGFILES = -30			// Probleem met het toevoegen van de binding aan de binding files.
		
	} BindingError_t;
};


/********************************************************************/
#endif /* #ifndef INCLUDED_BINDINGAPPCOMMON_H */
/********************************************************************/