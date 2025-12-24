#ifndef INCLUDED_HTTPRCFG_CMN_H
#define INCLUDED_HTTPRCFG_CMN_H
/**************************************************************/
namespace DUOTECNO {
/**************************************************************/
typedef enum 
{
	RESULT_SUCCESS = 0,
	RESULT_EC_FAILED = -1,

	RESULT_EC_HTTP_FAILED = -100,
	RESULT_EC_HTTP_URL = -101,
	RESULT_EC_HTTP_STATUSCODE_NOTFOUND = -102,
	RESULT_EC_HTTP_STATUSCODE_BADREQUEST = -103,
	RESULT_EC_HTTP_STATUSCODE_NOTIMPLEMENTED = -104,

	RESULT_EC_HTTP_STATUSCODE_UNSUPPORTED_MEDIA = -105,
	RESULT_EC_HTTP_STATUSCODE_AUTHORIZE = -106,					// Wanneer inloggen op HTTPS server en hebben geen password...
	RESULT_EC_HTTP_STATUSCODE_INTERNAL_SERVER = -107,

	// ...
	RESULT_EC_HTTP_STATUSCODE = -110,
	RESULT_EC_HTTP_MULTIFORMDATA = -120,
	RESULT_EC_HTTP_CURL = -200,

	RESULT_EC_JSON_FAILED = -300,
	RESULT_EC_JSON_NOT_IMPLEMENTED = -301,
	RESULT_EC_JSON_PARSE_DATA_NULL = -302,
	RESULT_EC_JSON_PARSE_OBJECT_NULL = -303,
	RESULT_EC_JSON_PARSE_WRONG_TYPE = -304,

	RESULT_EC_PARAMS = -400

} HttpRCfgResult_t;
/**************************************************************/
extern const char* toString(HttpRCfgResult_t res);
extern HttpRCfgResult_t convertHTTPStatusCode(int httpCode);
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
class AppRCfgResult
{
public:
	AppRCfgResult();

	typedef enum {
		RESULT_SUCCESS = 0, RESULT_ERROR
	}ResultCode_t;


	ResultCode_t getResultCode() const;
	void setResultCode(ResultCode_t code);

	const CString& getErrorMsg() const;
	void setErrorMsg(const char* msg);

	const CString& getErrorDetailMsg() const;
	void setErrorDetailMsg(const char* msg);

private:
	ResultCode_t m_resultCode;
	CString m_errorMsg;
	CString m_errDetailMsg;
};
/**************************************************************/
}; /* namespace DUOTECNO */
/**************************************************************/
#endif 
/**************************************************************/