#include "stdafx.h"
#include "jsonbase.h"
#include "json.h"
/**************************************************************/
namespace DUOTECNO {
/**************************************************************/

/**************************************************************/
/************************ Free functions **********************/
/**************************************************************/
// Oppassen: PRIVATE KEY NIET VERANDEREN !!
const char* json_getAuthPrivateKey( ) {
	static const char* const strPrivateKey =
		"C8xJR0VGAw3ITUaN3N+YWI0DiNicJC1EgOCZnEzZnAxgbHysAEDwpPwEUfQhzXw4CZikxJR0VGAw3ITUaN3N+YWI0DhEeKC8iNicJC1EgOCZnA3cHFSMFBypROjIt3";
	return strPrivateKey;
}

/**************************************************************/
/**************************************************************/
/**************************************************************/
JsonBaseClass::JsonBaseClass() : m_arrParser() { }
JsonBaseClass::~JsonBaseClass() { }

HttpRCfgResult_t JsonBaseClass::parse(AppRCfgResult* dest, const std::string& str) {
	return RESULT_EC_JSON_NOT_IMPLEMENTED;
};

// Sedert V2.0:
// Controle op het type json: Moet een JsonObject zijn.
HttpRCfgResult_t JsonBaseClass::helper_parse(const std::string& str, void* pvData) {	
	if (str.empty()) {
		return RESULT_EC_JSON_PARSE_DATA_NULL;
	}
	json_object* jobj = json_tokener_parse(str.c_str());
	if (0 == jobj) {
		return RESULT_EC_JSON_PARSE_OBJECT_NULL;
	}
	const json_type type = json_object_get_type(jobj);
	if (json_type_null == type) {
		json_object_put(jobj);
		return RESULT_EC_JSON_PARSE_OBJECT_NULL;
	}
	if (json_type_object != type) {
		json_object_put(jobj);
		return RESULT_EC_JSON_PARSE_WRONG_TYPE;		// V2.0
	}
	const HttpRCfgResult_t result = this->helper_parse(jobj, pvData);
	json_object_put(jobj);
	return result;
}

HttpRCfgResult_t JsonBaseClass::helper_parse(struct json_object* jobj, void* pvData) {
	const int length = m_arrParser.GetSize();
	json_object_object_foreach(jobj, key, val) {
		for (int i = 0; i<length; ++i) {
			PF_PARSER pf = m_arrParser[i];
			if (0 != pf) {
				if ((this->*pf)(key, val, pvData)) {
					break; // break for loop...
				}
			}
		}
	}
	return RESULT_SUCCESS;
}

// integer of een string integer...
bool JsonBaseClass::helper_getValueInt(char* key, struct json_object *val, const char* myKey, int* value) {
	if (0 != strcmp(key, myKey)) {
		return false;
	}

	const json_type type = json_object_get_type(val);
	if (json_type_int == type) {
		*value = json_object_get_int(val);
		return true;
	}
	else if (json_type_string == type) {
		const char* const s = json_object_get_string(val);
		if (0 == s) {
			return false;
		}
		int temp = -1;
		if (sscanf(s, "%d", &temp) >= 1) {
			*value = temp;
			return true;
		}
	}
	else {
		ASSERT(0);
	}
	return false;
}

bool JsonBaseClass::helper_getValueHexInt(char* key, struct json_object *val, const char* myKey, int* value) {
	if (0 != strcmp(key, myKey)) {
		return false;
	}
	if (json_type_string != json_object_get_type(val)) {
		ASSERT(json_type_string != json_object_get_type(val));
		return false;
	}

	const char* const s = json_object_get_string(val);
	if (0 == s && strlen(s)<3) {
		return false;
	}

	int temp = -1;
	if ((s[0] != '0') && (s[1] != 'x' || s[1] != 'X')) {
		return false;
	}
	if (sscanf(s, "0x%x", &temp) <= 0) {
		return false;
	}
	*value = temp;
	return true;
}

bool JsonBaseClass::helper_getValueString(char* key, struct json_object *val, const char* myKey, CString* value) {
	if (0 != strcmp(key, myKey)) {
		return false;
	}
	if (json_type_string != json_object_get_type(val)) {
		ASSERT(json_type_string != json_object_get_type(val));
		return false;
	}
	*value = CString(json_object_get_string(val));
	return true;
}

bool JsonBaseClass::helper_getValueBoolean(char* key, struct json_object *val, const char* myKey, bool* value) {
	if (0 != strcmp(key, myKey)) {
		return false;
	}

	const json_type type = json_object_get_type(val);
	if (json_type_boolean != type) {
		ASSERT(json_type_boolean != type);
		return false;
	}
	const json_bool v = json_object_get_boolean(val);
	*value = (v == 0 ? false : true);
	return true;
}
/**************************************************************/

/**************************************************************/
}; /* namespace DUOTECNO */
/**************************************************************/