#include "stdafx.h"
#include "httprcfg_cmn.h"
/**************************************************************/
namespace DUOTECNO {
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
AppRCfgResult::AppRCfgResult() : m_resultCode(RESULT_SUCCESS),
	m_errorMsg(), m_errDetailMsg() { }


AppRCfgResult::ResultCode_t AppRCfgResult::getResultCode() const {
	return m_resultCode;
}
void AppRCfgResult::setResultCode(ResultCode_t code) {
	m_resultCode = code;
}

const CString& AppRCfgResult::getErrorMsg() const {
	return m_errorMsg;
}

void AppRCfgResult::setErrorMsg(const char* msg) {
	m_errorMsg = CString(msg);
}

const CString& AppRCfgResult::getErrorDetailMsg() const {
	return m_errDetailMsg;
}
void AppRCfgResult::setErrorDetailMsg(const char* msg) {
	m_errDetailMsg = CString(msg);
}
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
const char* toString(HttpRCfgResult_t res) {
	switch(res) {
		case RESULT_SUCCESS:
			return "success";
		case RESULT_EC_FAILED:
			return "general_error";

		case RESULT_EC_HTTP_FAILED:
			return "http_error";			
		case RESULT_EC_HTTP_URL:
			return "http_error_wrong_url";
		case RESULT_EC_HTTP_STATUSCODE_NOTFOUND:
			return "http_error_statuscode_notfound";
		case RESULT_EC_HTTP_STATUSCODE_BADREQUEST:
			return "http_error_statuscode_badrequest";
		case RESULT_EC_HTTP_STATUSCODE_NOTIMPLEMENTED:
			return "http_error_statuscode_notimplemented";	
		case RESULT_EC_HTTP_STATUSCODE_UNSUPPORTED_MEDIA:
			return "http_error_statuscode_unsupported_media";
		case RESULT_EC_HTTP_STATUSCODE_AUTHORIZE:
			return "http_error_statuscode_unauthorized";
		case RESULT_EC_HTTP_STATUSCODE_INTERNAL_SERVER:
			return "http_error_internal_server_error";
		case RESULT_EC_HTTP_STATUSCODE:
			return "http_error_statuscode";
		case RESULT_EC_HTTP_MULTIFORMDATA:
			return "http_error_multiformdata";
		case RESULT_EC_HTTP_CURL:
			return "http_error_curl";

		case RESULT_EC_JSON_FAILED:
			return "json_error";		
		case RESULT_EC_JSON_NOT_IMPLEMENTED:
			return "json_error_not_implemented";	
		case RESULT_EC_JSON_PARSE_DATA_NULL:
			return "json_parser_error_no_data";	
		case RESULT_EC_JSON_PARSE_OBJECT_NULL:
			return "json_parser_object_null";	
		case RESULT_EC_JSON_PARSE_WRONG_TYPE:
			return "json_parser_wrong_type";

		case RESULT_EC_PARAMS:
			return "wrong_parameters_error";
	}
	return "general_error";
}


HttpRCfgResult_t convertHTTPStatusCode(int httpStatusCode) {
	switch(httpStatusCode) {
		case 200: 
			return RESULT_SUCCESS;
		case 400:
			return RESULT_EC_HTTP_STATUSCODE_BADREQUEST;
		case 401:
			return RESULT_EC_HTTP_STATUSCODE_AUTHORIZE;
		case 404:
			return RESULT_EC_HTTP_STATUSCODE_NOTFOUND;
		case 415:
			return RESULT_EC_HTTP_STATUSCODE_UNSUPPORTED_MEDIA;
		case 500:
			return RESULT_EC_HTTP_STATUSCODE_INTERNAL_SERVER;
		case 501:
			return RESULT_EC_HTTP_STATUSCODE_NOTIMPLEMENTED;
	}
	return RESULT_EC_HTTP_STATUSCODE;	
}

/**************************************************************/
}; /* namespace DUOTECNO */
/**************************************************************/