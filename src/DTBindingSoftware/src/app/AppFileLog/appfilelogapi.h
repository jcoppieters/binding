#pragma once


extern
const char* const APPFILELOG_BINDINGFILES_LOAD_EC;
extern
const char* const APPFILELOG_BINDINGFILES_COMP_EC;
extern
const char* const APPFILELOG_BINDINGFILES_ADD_EC;
extern
const char* const APPFILELOG_BINDINGFILES_PROPERTY_ADD_EC;
extern
const char* const APPFILELOG_BINDINGFILES_MANUALBINDINGS;


// Klasse.

struct CAppFileLogAPI
{
public:
	static
	void formatFullName(
		CString* pStrFullName,
		const char* szFileName);

	static
	void createLogFile(
		const char* szFileName,
		CStringArray& refStringsError);

	static
	void saveLogFile(
		const char* szFileNameFull,
		CStringArray& refStringsError);

	static
	void showLogFileErrors(
		const char* szMessage,
		const char* szFileNameFull,
		CStringArray& refStringsError);

	static
	void showLogFileErrors(
		const char* szFileName);
};