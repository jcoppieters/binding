#pragma once

#include "json_object.h"	// 3rd party json library.
#include <string>			// std::string	

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
class CAppJsonParser{
public:
	CAppJsonParser();
	~CAppJsonParser();

	typedef enum {
		RESULT_SUCCESS = 0,
		RESULT_EC_JSON_PARSE_DATA_NULL = -1,
		RESULT_EC_JSON_PARSE_OBJECT_NULL = -2,
		RESULT_EC_JSON_PARSE_WRONG_TYPE = -3,
		RESULT_EC_JSON_PARSE_NOT_IMPLEMENTED = -4
	} JsonParserResult_t;

public:
	// Dit zijn dezelfde functies als in de 'JsonBaseClass' klasse.
	static bool helper_getValueInt(char* key, struct json_object *val, const char* myKey, int* value);
	static bool helper_getValueHexInt(char* key, struct json_object *val, const char* myKey, int* value);
	static bool helper_getValueString(char* key, struct json_object *val, const char* myKey, CString* value);
	static bool helper_getValueBoolean(char* key, struct json_object *val, const char* myKey, bool* value);


	// Verwijderen van de BOM (Byte Order Mark.)
	// Deze BOM wordt std door Visual studio c# aan het begin van een json bestand geplaatst.
	static std::string removeBOM(const std::string& str);

protected:
	JsonParserResult_t helper_parse(const std::string& str, void* pvData);
	JsonParserResult_t helper_parse(struct json_object* object, void* pvData);
	bool loadFile(const char* fileName,std::string* strDest);

protected:
	typedef bool (CAppJsonParser::*PF_PARSER)(char*, struct json_object*, void*);
	CArray<PF_PARSER> m_arrParser;
};
/***********************************************************************************/