#include "StdAfx.h"
#include "BindingSoftware.h"
#include "appjson.h"
#include "json.h"
/***********************************************************************************/


/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
// Deze template functies als friend toevoegen.
template <class TYPE>
static bool helper_setValueString(char* key, struct json_object *val, void* pvData, const char* myKey, void (TYPE::*pf)(const CString&)) {
	CString str;
	if (!CAppDaliJson_Base::helper_getValueString(key, val, myKey, &str)) {
		return false;
	}

	TYPE* const dest = reinterpret_cast<TYPE*>(pvData);
	if (0 == dest) {
		return false;
	}
	(dest->*pf)(str);
	return true;
}

template <class TYPE>
static bool helper_setValueInteger(char* key, struct json_object *val, void* pvData, const char* myKey, void (TYPE::*pf)(int)) {
	int value = -1;
	if (!CAppDaliJson_Base::helper_getValueInt(key, val, myKey, &value)) {
		return false;
	}
	TYPE* const dest = reinterpret_cast<TYPE*>(pvData);
	if (0 == dest) {
		return false;
	}
	(dest->*pf)(value);
	return true;
}

template <class TYPE>
static bool helper_setValueHexInt(char* key, struct json_object *val, void* pvData, const char* myKey, void (TYPE::*pf)(int)) {
	int value = -1;
	if (!CAppDaliJson_Base::helper_getValueHexInt(key, val, myKey, &value)) {
		return false;
	}
	TYPE* const dest = reinterpret_cast<TYPE*>(pvData);
	if (0 == dest) {
		return false;
	}
	(dest->*pf)(value);
	return true;
}

template <class TYPE>
static bool helper_setValueBoolean(char* key, struct json_object *val, void* pvData, const char* myKey, void (TYPE::*pf)(bool)) {
	bool value = false;
	if (!CAppDaliJson_Base::helper_getValueBoolean(key, val, myKey, &value)) {
		return false;
	}
	TYPE* const dest = reinterpret_cast<TYPE*>(pvData);
	if (0 == dest) {
		return false;
	}
	(dest->*pf)(value);
	return true;
}

/***********************************************************************************/
/***********************************************************************************/
/***********************************************************************************/
CAppJsonParser::CAppJsonParser() : m_arrParser() { }
CAppJsonParser::~CAppJsonParser() { }

CAppJsonParser::JsonParserResult_t CAppJsonParser::helper_parse(const std::string& str, void* pvData) {
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
	const JsonParserResult_t result = this->helper_parse(jobj, pvData);
	json_object_put(jobj);
	return result;
}

CAppJsonParser::JsonParserResult_t CAppJsonParser::helper_parse(struct json_object* jobj, void* pvData) {
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

std::string CAppJsonParser::removeBOM(const std::string& str) {
    const std::string BOM = "\xEF\xBB\xBF";
    if (str.compare(0, BOM.size(), BOM) == 0) {
        return str.substr(BOM.size()); // Verwijder BOM
    }
    return str;
}

// integer of een string integer...
bool CAppJsonParser::helper_getValueInt(char* key, struct json_object *val, const char* myKey, int* value) {
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

bool CAppJsonParser::helper_getValueHexInt(char* key, struct json_object *val, const char* myKey, int* value) {
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

bool CAppJsonParser::helper_getValueString(char* key, struct json_object *val, const char* myKey, CString* value) {
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

bool CAppJsonParser::helper_getValueBoolean(char* key, struct json_object *val, const char* myKey, bool* value) {
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

bool CAppJsonParser::loadFile(const char* fileName, std::string* strDest) {
	if (NULL == fileName || NULL == strDest) {
		REQUIRE(NULL != fileName);
		REQUIRE(NULL != strDest);
		return false;
	}
	CFile f;
	if (!f.Open(fileName, CFile::modeRead)) {
		return false;
	}
	DWORD dwRead = 0;
	do {
		BYTE buffer[4096];
		memset(buffer, 0, sizeof(buffer));
		dwRead = f.Read(buffer, 4096);
		if (dwRead>0) {
			strDest->append((const char*)buffer, (int)dwRead);
		}
	} while (dwRead > 0);
	f.Close();
	return true;
}
/***********************************************************************************/