#ifndef INCLUDED_HTTPRCFG_HTTP_H
#define INCLUDED_HTTPRCFG_HTTP_H
/**************************************************************/
#ifndef INCLUDED_HTTPRCFG_CMN_H
	#include "httprcfg_cmn.h"
#endif 

#include <string>
#include <list>
/**************************************************************/
namespace DUOTECNO {
/**************************************************************/
typedef std::list<std::pair<std::string,std::string>> HeaderList;
typedef std::list<std::pair<std::string,std::string>> FormParametersList;
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
class HttpRequest 
{
public:
	typedef enum {
		PROTOCOL_HTTP,
		PROTOCOL_HTTPS
	} protocol_t;

	HttpRequest(const char* hostAddress,int port, const char* endpoint, protocol_t protocol = PROTOCOL_HTTP);
	HttpRequest(const char* url, protocol_t protocol = PROTOCOL_HTTP);
	bool format(std::string* url) const;

	const HeaderList& headers() const;
	void addHeader(const std::pair<std::string,std::string> header);

	const char* getDataBody( ) const;
	void setDataBody(const char* body);

	void setVerboseOnOff(bool flag);
	bool getVerboseOnOff( ) const;

	void setDumpResponse(bool flag);
	bool getDumpResonse( ) const;

	protocol_t getProtocol() const { return m_protocol; }

	void setDestFileName(const char* fileName);
	const char* getFileName() const;

private:
	enum {
		FLAG_VERBOSE_ON			= (1<<0),
		FLAG_DUMP_RESPONSE		= (1<<1)
	};

	std::string m_url;
	std::string m_hostAddress;
	int m_port; 
	std::string m_endpoint;
	std::string m_bodyData;
	std::string m_destFileName;		// Moeten de response in een bestand opslaan.
	HeaderList m_headers;
	protocol_t m_protocol;
	int m_flags;
};
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
class HttpRequestMultiForm : public HttpRequest
{
public:
	HttpRequestMultiForm(const char* hostAddress,int port, const char* endpoint,const char* fileName,const char* fieldName = "file", protocol_t protocol = PROTOCOL_HTTP);

	const char* fieldName( ) const;
	const char* fileName( ) const;

	const FormParametersList& params() const;
	void addParam(const std::pair<std::string,std::string> param);

private:
	std::string m_field;
	std::string m_fileName;
	FormParametersList m_listParams;

	typedef HttpRequest Base;
};
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
class HttpResponse
{
public:
	HttpResponse( );

	void setDataBody(const std::string& str);
	const std::string& getDataBody( ) const;

	long getHttpStatusCode( ) const;
	void setHttpStatusCode(long v);

private:
	std::string m_dataBody;			
	long m_httpStatusCode;
};
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
class UriHelper
{
public:
	static std::string Encode(const char* source);
    //static string Decode(const string &encoded);
};

class AuthHelper
{
public:
	static std::string CreateBasicBase64Header(const char* str1, const char* str2);
	static std::string CreateEncryptedBasicBase64Header(const char* str1, const char* str2,const char* secretKey);

	// Data om op te slaan.
	// 1) Src string decoderen met de private key.
	// 2) Base64 string aanmaken.
	static std::string encode(const char* src, const char* strSecretKey);

	// Data ingeladen
	// 1) Base64 decoderen.
	// 2) Data encoderen met de private key.
	static std::string decode(const char* src, const char* strSecretKey);

protected:
	static int encrypt(const unsigned char* src,int srcLength,const char* strSecretKey,unsigned char* arr,int maxLength);
};


/**************************************************************/
/**************************************************************/
/**************************************************************/
class CharSetDecoderHelper 
{
public:
	// UTF-8 string omzetten naar een ANSI C-string.
	// Ontvangen data is standaard altijd UTF-8 data.
	static std::string change_encoding_from_UTF8_to_ANSI(const std::string& strUtf8);
};

/**************************************************************/
/**************************************************************/
/**************************************************************/
extern void httprcfg_httpInit( );
extern void httprcfg_httpShutdown( );

extern HttpRCfgResult_t httprcfg_GET(const HttpRequest& request,HttpResponse *pHttpResponse);
extern HttpRCfgResult_t httprcfg_POST(const HttpRequest& request,HttpResponse *pHttpResponse);
extern HttpRCfgResult_t httprcfg_POST(const HttpRequestMultiForm& request,HttpResponse *pHttpResponse);
extern HttpRCfgResult_t httprcfg_DELETE(const HttpRequest& request,HttpResponse *pHttpResponse);

/**************************************************************/
}; /*namespace DUOTECNO */
/**************************************************************/
#endif 
/**************************************************************/