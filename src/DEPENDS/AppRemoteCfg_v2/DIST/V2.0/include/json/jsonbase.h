#ifndef INCLUDED_JSONBASE_H
#define INCLUDED_JSONBASE_H
/**************************************************************/
#ifndef INCLUDED_HTTPRCFG_CMN_H
	#include "httprcfg/httprcfg_cmn.h"
#endif 
/*************************************************************/
#include "json_object.h"
#include <string>			// std::string	
/**************************************************************/
namespace DUOTECNO {
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
class JsonBaseClass
{
public:
	JsonBaseClass();
	virtual ~JsonBaseClass();
	virtual HttpRCfgResult_t parse(AppRCfgResult* dest, const std::string& str);

	static bool helper_getValueInt(char* key, struct json_object *val, const char* myKey, int* value);
	static bool helper_getValueHexInt(char* key, struct json_object *val, const char* myKey, int* value);
	static bool helper_getValueString(char* key, struct json_object *val, const char* myKey, CString* value);
	static bool helper_getValueBoolean(char* key, struct json_object *val, const char* myKey, bool* value);

protected:
	HttpRCfgResult_t helper_parse(const std::string& str, void* pvData);
	HttpRCfgResult_t helper_parse(struct json_object* object, void* pvData);

protected:
	typedef bool (JsonBaseClass::*PF_PARSER)(char*, struct json_object*, void*);
	CArray<PF_PARSER> m_arrParser;
};


/**************************************************************/
/************************ Free functions **********************/
/**************************************************************/
// Private key die gebruikt wordt om data op te slaan.
// Login info / Server info.
extern const char* json_getAuthPrivateKey( );
/**************************************************************/
}; /* namespace DUOTECNO */
/**************************************************************/
#endif 
/**************************************************************/