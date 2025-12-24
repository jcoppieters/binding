#include "stdafx.h"
#include "httprcfg_http.h"

#include "httprcfg_base64.h"	// ref: https://renenyffenegger.ch/notes/development/Base64/Encoding-and-decoding-base-64-with-cpp/index#cpp-base64-cpp

#include "curl/curl.h"
#include <iostream>
#include <string>

#include "windows.h"			// Nodig voor omzetten van UTF-8 naar ANSI string.
/**************************************************************/
#define REQUIRE			ASSERT
/**************************************************************/
		
/***************************************************************
 * SKIP_HOSTNAME_VERIFICATION
 *
 * If the site you're connecting to uses a different host name that what
 * they have mentioned in their server certificate's commonName (or
 * subjectAltName) fields, libcurl will refuse to connect. You can skip
 * this check, but this will make the connection less secure.
 **************************************************************/
#define SKIP_HOSTNAME_VERIFICATION	1												// TODO DEZE VLAGGEN IN DE HTTPREQUEST
/**************************************************************
 * SKIP_PEER_VERIFICATION
 * 
 * If you want to connect to a site who isn't using a certificate that is
 * signed by one of the certs in the CA bundle you have, you can skip the
 * verification of the server's certificate. This makes the connection
 * A LOT LESS SECURE.
 *
 * If you have a CA cert for the server stored someplace else than in the
 * default bundle, then the CURLOPT_CAPATH option might come handy for
 * you.
 **************************************************************/
#define SKIP_PEER_VERIFICATION		1												// TODO DEZE VLAGGEN IN DE HTTPREQUEST
/**************************************************************/

/**************************************************************/
namespace DUOTECNO {
/**************************************************************/
using namespace std;


static size_t writefunc(void *ptr, size_t size, size_t nmemb, std::string *s);
static size_t writefunc_fp(void *ptr, size_t size, size_t nmemb, void *stream);

/**************************************************************/
/**************************************************************/
/**************************************************************/
HttpRequest::HttpRequest(const char* hostAddress,int port, const char* endpoint, protocol_t protocol) :
	m_url(), m_hostAddress(std::string(hostAddress)), m_port(port), m_endpoint(std::string(endpoint)), m_bodyData(), m_destFileName(),
	m_headers(), m_protocol(protocol), m_flags(0)  { }

HttpRequest::HttpRequest(const char* url, protocol_t protocol) : m_url(url), m_hostAddress(), m_port(-1),
	m_endpoint( ), m_bodyData( ), m_destFileName(), m_headers(), m_protocol(protocol), m_flags(0) { }


bool HttpRequest::format(std::string* url) const {
	
	if (m_url.length() > 0) {
		*url = m_url;
		return true;
	}

	// TODO: CONTROLE OP HOSTADDRESS.
	char strTemp[512];
	const char* strProtocol;
	if (PROTOCOL_HTTP == m_protocol) {
		strProtocol = "http://";
	}
	else if (PROTOCOL_HTTPS == m_protocol) {
		strProtocol = "https://";
	}

	if (m_port>0) {
		sprintf(strTemp,"%s%s:%d%s",strProtocol,m_hostAddress.c_str(),m_port,m_endpoint.c_str());
	}
	else {
		sprintf(strTemp,"%s%s:%s",strProtocol,m_hostAddress.c_str(),m_endpoint.c_str());
	}

	*url = std::string(strTemp);
	return true;
}

const std::list<std::pair<std::string,std::string>>& HttpRequest::headers() const {
	return m_headers;
}

void HttpRequest::addHeader(std::pair<std::string,std::string> header) {
	m_headers.push_back(header);
}

const char* HttpRequest::getDataBody( ) const {
	return m_bodyData.c_str();
}
void HttpRequest::setDataBody(const char* body) {
	m_bodyData = std::string(body);
}

void HttpRequest::setVerboseOnOff(bool flag) {
	if (flag) {
		m_flags |= FLAG_VERBOSE_ON;
	}
	else {
		m_flags &= ~FLAG_VERBOSE_ON;
	}
}
bool HttpRequest::getVerboseOnOff( ) const {
	return (FLAG_VERBOSE_ON == (m_flags & FLAG_VERBOSE_ON));
}

void HttpRequest::setDumpResponse(bool flag) {
	if (flag) {
		m_flags |= FLAG_DUMP_RESPONSE;
	}
	else {
		m_flags &= ~FLAG_DUMP_RESPONSE;
	}
}
bool HttpRequest::getDumpResonse( ) const {
	return (FLAG_DUMP_RESPONSE == (m_flags & FLAG_DUMP_RESPONSE));
}

void HttpRequest::setDestFileName(const char* fileName) {
	m_destFileName = fileName;
}
const char* HttpRequest::getFileName() const {
	return m_destFileName.c_str();
}
/**************************************************************/

/**************************************************************/
HttpRequestMultiForm::HttpRequestMultiForm(const char* hostAddress,int port, const char* endpoint,
										   const char* fileName,const char* fieldName, protocol_t protocol) :
	Base(hostAddress, port, endpoint, protocol), m_field(fieldName), m_fileName(fileName), m_listParams( ) { }

const char* HttpRequestMultiForm::fieldName( ) const {
	return m_field.c_str();
}
const char* HttpRequestMultiForm::fileName( ) const {
	return m_fileName.c_str();
}

const FormParametersList& HttpRequestMultiForm::params() const {
	return m_listParams;
}
void HttpRequestMultiForm::addParam(const std::pair<std::string,std::string> param) {
	m_listParams.push_back(param);
}
/**************************************************************/


/**************************************************************/
/**************************************************************/
/**************************************************************/
HttpResponse::HttpResponse( ) : m_dataBody( ), m_httpStatusCode(0) { }

void HttpResponse::setDataBody(const std::string& v) {
	m_dataBody = v;
}
const std::string& HttpResponse::getDataBody( ) const {
	return m_dataBody;
}

long HttpResponse::getHttpStatusCode( ) const {
	return m_httpStatusCode;
}
void HttpResponse::setHttpStatusCode(long v) {
	m_httpStatusCode = v;
}
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
std::string UriHelper::Encode(const char* src)
{
	CURL *curl = curl_easy_init();
	if (0 == curl || 0 == src) {
		return std::string();
	}
    char *cres = curl_easy_escape(curl, src, strlen(src));
    string res(cres);
    curl_free(cres);
    curl_easy_cleanup(curl);
    return res;
}

/**************************************************************/

std::string AuthHelper::CreateBasicBase64Header(const char* str1, const char* str2) {
	if (0 == str1 || 0 == str2) {
		return std::string();
	}

	std::string s = str1;
	s.append(":");
	s.append(str2);

	std::string strTemp = base64_encode(s.c_str());
	std::string strOut;
	if (strTemp.size() > 0) {
		strOut = "Basic ";
		strOut.append(strTemp);
	}
	ENSURE(strOut.size() > 0);
	return strOut;
}

std::string AuthHelper::CreateEncryptedBasicBase64Header(const char* str1, const char* str2,const char* secretKey) {
	if (0 == str1 || 0 == str2 || 0 == secretKey) {
		return std::string();
	}

	std::string plainHeader = str1;
	plainHeader.append(":");
	plainHeader.append(str2);

	const int length = plainHeader.size();
	if (length<=0) {
		return std::string();
	}

	// Oppassen:
	// Doordat we de string encrypteren mogen we de data niet in een string stopen (null-char).
	// Mogen een BYTE array gebruiken...
	unsigned char* arr = new unsigned char[length];
	if (NULL == arr) {
		return std::string();
	}
	
	const char* const src = plainHeader.c_str();
	const int srcLength = strlen(src);
	const int size = encrypt(reinterpret_cast<const unsigned char*>(src), srcLength, secretKey, arr, length);
	if (size<=0) {
		delete [] arr;
		return std::string();	
	}
	
	ASSERT(size==length);
	std::string strTemp = base64_encode(arr,size);
	std::string strOut;
	if (strTemp.size() > 0) {
		strOut = "Basic ";
		strOut.append(strTemp);
	}	
	delete [] arr;
	ENSURE(strOut.size() > 0);	
	return strOut;
}

// Data om op te slaan.
// 1) Src string decoderen met de private key.
// 2) Base64 string aanmaken.
std::string AuthHelper::encode(const char* src, const char* strSecretKey) {
	if (0 == src || 0 == strSecretKey) {
		return std::string();
	}

	const int length = strlen(src);
	if (length <= 1) {
		return std::string();
	}
	unsigned char* arr = new unsigned char[length+1];
	if (NULL == arr) {
		return std::string();
	}
	memset(arr, '\0', length);

	const int size = encrypt(reinterpret_cast<const unsigned char*>(src), length, strSecretKey, arr, length);
	if (size <= 0) {
		delete[] arr;
		return std::string();
	}

	std::string strBase64 = base64_encode(arr,size);
	delete[] arr;
	return strBase64;
}

// Data ingeladen.
// 1) Base64 decoderen.
// 2) Data encoderen met de private key.
std::string AuthHelper::decode(const char* src, const char* strSecretKey) {
	if (0 == src || 0 == strSecretKey) {
		return std::string();
	}

	std::string strBase64Decoded = base64_decode(src);
	const int length = strBase64Decoded.length();
	if (length <= 1) {
		return std::string();
	}
	unsigned char* arr = new unsigned char[length + 1];
	if (NULL == arr) {
		return std::string();
	}
	memset(arr, '\0', length);

	// Oppassen:
	// Moeten std::string::data() gebruiken en niet c_str().
	// data() is niet null termenated, c_str() is dit wel.
	const int size = encrypt(reinterpret_cast<const unsigned char*>(strBase64Decoded.data()), length, strSecretKey, arr, length);
	if (size <= 0) {
		delete[] arr;
		return std::string();
	}

	const std::string strRetval = std::string((const char*)arr, size);
	delete[] arr;
	return strRetval;
}

int AuthHelper::encrypt(const unsigned char* src,int srcLength,const char* key,unsigned char* arr,int maxLength) {
	if (NULL == src || NULL == key || NULL == arr) {		
		return 0;
	}
	if (maxLength<srcLength) {
		ASSERT(maxLength>= srcLength);
		return 0;
	}   

	int i;
	const int privateKeySize = strlen(key);
    for (i = 0; i < srcLength; i++) {
        arr[i] = src[i] ^ key[i % privateKeySize];
    }    
	ENSURE(i == srcLength);
	return srcLength;
}
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/

// UTF-8 string omzetten naar een ANSI C-string.
// Ontvangen data is standaard altijd UTF-8 data.
std::string CharSetDecoderHelper::change_encoding_from_UTF8_to_ANSI(const std::string& strUtf8) 
{
	wchar_t* wszString = 0;
	char* szAnsi = 0;
	std::string retVal;

	const char* const szU8 = strUtf8.data();
	if (0 == szU8) {
		return retVal;
	}

	const int wcsLen = ::MultiByteToWideChar(CP_UTF8, NULL, szU8, strlen(szU8), NULL, 0);
	if (wcsLen <= 0) {
		goto done;
	}
	wszString = new wchar_t[wcsLen + 1];
	if (0 == wszString) {
		goto done;
	}

	::MultiByteToWideChar(CP_UTF8, NULL, szU8, strlen(szU8), wszString, wcsLen);
	wszString[wcsLen] = '\0';

	int ansiLen = ::WideCharToMultiByte(CP_ACP, NULL, wszString, wcslen(wszString), NULL, 0, NULL, NULL);
	if (ansiLen <= 0) {
		goto done;
	}
	szAnsi = new char[ansiLen + 1];
	if (0 == szAnsi) {
		goto done;
	}
	::WideCharToMultiByte(CP_ACP, NULL, wszString, wcslen(wszString), szAnsi, ansiLen, NULL, NULL);
	szAnsi[ansiLen] = '\0';

	retVal = std::string(szAnsi);

done:
	if (0 != wszString) {
		delete[] wszString;
	}
	if (0 != szAnsi) {
		delete szAnsi;
	}
	return retVal;
}

/**************************************************************/

// Private...
static struct curl_slist* addHeaders(struct curl_slist *headerlist,const HeaderList& srcHeaders) {
	if (srcHeaders.empty()) {
		return headerlist;
	}
	
	HeaderList::const_iterator cit;
	for (cit = srcHeaders.begin(); cit != srcHeaders.end(); ++cit){
		char strTemp[256];
		sprintf(strTemp,"%s: %s",cit->first.c_str(),cit->second.c_str());
		headerlist = curl_slist_append(headerlist, strTemp);
		ASSERT(0 != headerlist);
	}					
	return headerlist;
}

static curl_mime* createMultiFormData(CURL *curl, const char* fieldName, const char* fileName,const FormParametersList& params) {

	if (params.empty()) {
		REQUIRE(!params.empty());
		return 0;
	}
	if (0 == fileName) {
		REQUIRE(0 != fileName);
		return 0;
	}
	
	curl_mime* form = curl_mime_init(curl);
	if (0 == form) {
		ASSERT(0 != form);
		return 0;
	}

	curl_mimepart* field = curl_mime_addpart(form);
	if (0 == field) {
		curl_mime_free(form);
		ASSERT(0 == field);
		return 0;
	}
	
	curl_mime_name(field, "file");
	curl_mime_filedata(field, fileName);
			
	FormParametersList::const_iterator cit;
	for (cit = params.begin(); cit != params.end(); ++cit){
		const char* const name = cit->first.c_str();
		const char* const value = cit->second.c_str();		
		if (0 != name && 0 != value) {
			curl_mimepart* const field = curl_mime_addpart(form);
			if (0 == field) {
				curl_mime_free(form);
				ASSERT(0 == field);
				return 0;
			}
			curl_mime_name(field, name);
			curl_mime_data(field, value, CURL_ZERO_TERMINATED);
		}
		else { 
			ASSERT(0 != name);
			ASSERT(0 != value);
		}
	}	
	return form;
}

/**************************************************************/
/**************************************************************/
/**************************************************************/
void httprcfg_httpInit( ) {
	curl_global_init(CURL_GLOBAL_ALL);
}
void httprcfg_httpShutdown( ) {
	curl_global_cleanup();
}

// Sedert V2.0.
// Wanneer er een fileName meegegeven wordt in de HTTPRequest dan wordt de ontvangen response body 
// in dit bestand opgeslagen.
HttpRCfgResult_t httprcfg_GET(const HttpRequest& request,HttpResponse *pHttpResponse) {
	
	std::string url;
	if (!request.format(&url)) {
		return RESULT_EC_HTTP_URL;
	}
	
	HttpRCfgResult_t result = RESULT_EC_HTTP_FAILED;
	CURL *curl = curl_easy_init();
	if (curl)
	{						
		static const char buf[] = "Expect:";
		std::string responseBody;

		struct curl_slist *headerlist = NULL;

		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

		FILE* fp = NULL;
		const char* fileName = request.getFileName();
		if ((NULL != fileName) && (strlen(fileName) > 0)) {
			fp = fopen(fileName, "wb");
			if (NULL == fp) {
				curl_easy_cleanup(curl);
				return RESULT_EC_HTTP_FAILED;			// ANDERE FOUT ???? <<---- TODO !!!!
			}

			// Opties voor opslaan van de ontvangen body in een bestand.
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc_fp);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
		}
		else {
			// Data opslaan in een string en later meegeven in de responseBody.
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBody);
		}


		if (HttpRequest::PROTOCOL_HTTPS == request.getProtocol()) {
#ifdef SKIP_PEER_VERIFICATION
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
#endif 
#ifdef SKIP_HOSTNAME_VERIFICATION
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
#endif
		}

		/* initialize custom header list (stating that Expect: 100-continue is not wanted */
		headerlist = curl_slist_append(headerlist, buf);
		headerlist = addHeaders(headerlist,request.headers());
		
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
		
		if (request.getVerboseOnOff()) {
			curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
		}

		const CURLcode res = curl_easy_perform(curl);
		if(res == CURLE_OK)  {			
			long http_code = 0;
			curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

			pHttpResponse->setHttpStatusCode(http_code);
		
			if (http_code>=200 && http_code<300) {	
				if (responseBody.length() > 0) {			/* sedert V2.0: Controle op de lengte.*/
					if (request.getDumpResonse()) {
						std::cout << responseBody << std::endl;
					}
					pHttpResponse->setDataBody(responseBody);
				}
			}
			result = convertHTTPStatusCode(http_code);		
		}
		else {
#if(0)
			if (CURLE_GOT_NOTHING == res) {
				long http_code = 0;
				curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
				pHttpResponse->setHttpStatusCode(http_code);
			}

			// CURL_COULDNT_CONNECT
#endif

			if (request.getVerboseOnOff()) {
				fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
			}
			result = RESULT_EC_HTTP_CURL;	// TODO: EXTRA STRING MEEGEVEN VOOR DE FOUT ???
		}	

		if (NULL != fp) {
			fclose(fp);
		}
		
		/* always cleanup */
		curl_easy_cleanup(curl);
		/* free slist */
		curl_slist_free_all(headerlist);
	}
	return result;
}

HttpRCfgResult_t httprcfg_POST(const HttpRequest& request,HttpResponse *pHttpResponse) {
	std::string url;
	if (!request.format(&url)) {
		return RESULT_EC_HTTP_URL;
	}
	
	HttpRCfgResult_t result = RESULT_EC_HTTP_FAILED;
	CURL *curl = curl_easy_init();
	if (curl)
	{						
		static const char buf[] = "Expect:";
		std::string responseBody;

		struct curl_slist *headerlist = NULL;

		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBody);

		if (HttpRequest::PROTOCOL_HTTPS == request.getProtocol()) {
#ifdef SKIP_PEER_VERIFICATION
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
#endif 
#ifdef SKIP_HOSTNAME_VERIFICATION
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
#endif
		}

		/* initialize custom header list (stating that Expect: 100-continue is not wanted */
		headerlist = curl_slist_append(headerlist, buf);
		headerlist = addHeaders(headerlist,request.headers());

		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
		
		if (request.getVerboseOnOff()) {
			curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
		}

		const char* strDataBody = request.getDataBody();
		if (0 != strDataBody) {
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, strDataBody);
		}

		const CURLcode res = curl_easy_perform(curl);
		if(res == CURLE_OK)  {			
			long http_code = 0;
			curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

			pHttpResponse->setHttpStatusCode(http_code);
		
			if (http_code>=200 && http_code<300) {
				if (request.getDumpResonse()) {
					std::cout << responseBody << std::endl;
				}
				pHttpResponse->setDataBody(responseBody);			
			}
			result = convertHTTPStatusCode(http_code);	
		}
		else {
			if (request.getVerboseOnOff()) {
				fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
			}
			result = RESULT_EC_HTTP_CURL;	// TODO: EXTRA STRING MEEGEVEN VOOR DE FOUT ???
		}	
		
		/* always cleanup */
		curl_easy_cleanup(curl);
		/* free slist */
		curl_slist_free_all(headerlist);
	}
	return result;
}

HttpRCfgResult_t httprcfg_POST(const HttpRequestMultiForm& request,HttpResponse *pHttpResponse) {

	std::string url;
	if (!request.format(&url)) {
		return RESULT_EC_HTTP_URL;
	}
	
	HttpRCfgResult_t result = RESULT_EC_HTTP_FAILED;
	CURL *curl = curl_easy_init();
	if (curl)
	{						
		curl_mime *form = NULL;
		curl_mimepart *field = NULL;
		struct curl_slist *headerlist = NULL;
		static const char buf[] = "Expect:";

		std::string responseBody;
	
		/* Create the form */
		form = createMultiFormData(curl,request.fieldName(),request.fileName(),request.params());
		if (0 == form) {		
			ASSERT(0 != form);
			return RESULT_EC_HTTP_MULTIFORMDATA;
		}
		curl_easy_setopt(curl, CURLOPT_MIMEPOST, form);
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBody);

		if (HttpRequest::PROTOCOL_HTTPS == request.getProtocol()) {
#ifdef SKIP_PEER_VERIFICATION
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
#endif 
#ifdef SKIP_HOSTNAME_VERIFICATION
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
#endif
		}

		/* initialize custom header list (stating that Expect: 100-continue is not wanted */
		headerlist = curl_slist_append(headerlist, buf);
		headerlist = addHeaders(headerlist,request.headers());
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
		
		if (request.getVerboseOnOff()) {
			curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
		}
						
		const CURLcode res = curl_easy_perform(curl);
		if(res == CURLE_OK) 
		{			
			long http_code = 0;
			curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

			pHttpResponse->setHttpStatusCode(http_code);
		
			if (http_code>=200 && http_code<300) {
				if (request.getDumpResonse()) {
					std::cout << responseBody << std::endl;
				}
				pHttpResponse->setDataBody(responseBody);				
			}
			result = convertHTTPStatusCode(http_code);	
		}
		else {
			if (request.getVerboseOnOff()) {
				fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
			}
			result = RESULT_EC_HTTP_CURL;	// TODO: EXTRA STRING MEEGEVEN VOOR DE FOUT ???
		}			
		/* always cleanup */
		curl_easy_cleanup(curl);
		/* then cleanup the form */
		curl_mime_free(form);
		/* free slist */
		curl_slist_free_all(headerlist);
	}
	return result;
}

HttpRCfgResult_t httprcfg_DELETE(const HttpRequest& request,HttpResponse *pHttpResponse) {

	std::string url;
	if (!request.format(&url)) {
		return RESULT_EC_HTTP_URL;
	}
	
	HttpRCfgResult_t result = RESULT_EC_HTTP_FAILED;
	CURL *curl = curl_easy_init();
	if (curl)
	{						
		static const char buf[] = "Expect:";
		std::string responseBody;

		struct curl_slist *headerlist = NULL;
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBody);
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");

		if (HttpRequest::PROTOCOL_HTTPS == request.getProtocol()) {
#ifdef SKIP_PEER_VERIFICATION
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
#endif 
#ifdef SKIP_HOSTNAME_VERIFICATION
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
#endif
		}

		/* initialize custom header list (stating that Expect: 100-continue is not wanted */
		headerlist = curl_slist_append(headerlist, buf);
		headerlist = addHeaders(headerlist,request.headers());
		
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
		
		if (request.getVerboseOnOff()) {
			curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
		}

		const CURLcode res = curl_easy_perform(curl);
		if(res == CURLE_OK)  {			
			long http_code = 0;
			curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

			pHttpResponse->setHttpStatusCode(http_code);
		
			if (http_code>=200 && http_code<300) {	
				if (request.getDumpResonse()) {
					std::cout << responseBody << std::endl;
				}
				pHttpResponse->setDataBody(responseBody);
			}
			result = convertHTTPStatusCode(http_code);		
		}
		else {
			if (request.getVerboseOnOff()) {
				fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
			}
			result = RESULT_EC_HTTP_CURL;	// TODO: EXTRA STRING MEEGEVEN VOOR DE FOUT ???
		}	
		
		/* always cleanup */
		curl_easy_cleanup(curl);
		/* free slist */
		curl_slist_free_all(headerlist);
	}
	return result;
}

/**************************** private *************************/

// Dit is de callback functie die de data ontvangt...
static size_t writefunc(void *ptr, size_t size, size_t nmemb, std::string *s) {
	s->append(static_cast<char *>(ptr), size*nmemb);
	return size*nmemb;
}

// Dit is de callback die binary data ontvangt om in een bestand te schrijven.
static size_t writefunc_fp(void *ptr, size_t size, size_t nmemb, void *stream) {
	size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
	return written;
}

/**************************************************************/

/**************************************************************/
}; /*namespace DUOTECNO */
/**************************************************************/