#include "stdafx.h"
#include "httprcfg_api.h"
#include "httprcfg_json.h"
#include "httprcfg_http.h"

//#include "json_object.h"

#include <iostream>
#include <string>
/**************************************************************/
#define REQUIRE				ASSERT
/**************************************************************/
#define	USE_TESTCASES			1
/**************************************************************/
namespace DUOTECNO {
/**************************************************************/

/**************************************************************/
// Compileren: msvc2008 - x86 static library.
// --------------------------------------------
// Visual Studio 2008 Command Prompt --> Oppassen niet de win64 variant !!!
//  Naar de 'winbuild' dir gaan.
// 	nmake /f Makefile.vc mode=static VC=8 ENABLE_SSPI=no ENABLE_IDN=no ENABLE_WINSSL=no DEBUG=yes GEN_PDB=no ENABLE_IPV6=yes
//  nmake /f Makefile.vc mode=static VC=8 ENABLE_SSPI=no ENABLE_IDN=no ENABLE_WINSSL=no DEBUG=no GEN_PDB=no ENABLE_IPV6=yes
// --> Bestanden staan onder de D:\developer\sw\curl\curl-7.78.0\builds
//
//	TODO: openSSL toevoegen...
/**************************************************************/
// Invoegen in project:
//
// 1: Include directories zetten.
// 2: linker: library + dir zetten.
// 3: preprocessor: CURL_STATICLIB 
// 4: moeten deze aanpassen naar: Multi-threaded DLL (/MD)	(voor de debug.)
// 5: linker opties: Extra libraries: ws2_32.lib Wldap32.lib
/**************************************************************/

/***************************************************************
 * Json-c:
 *	Gedownload van https://github.com/winlibs
 *  Is een oude versie.
 *
 *	Zelf een project (static library) aangemaakt en deze gecompileerd.
 *
 **************************************************************/

#define USE_VERBOSE_ON					0
#define DUMP_HTTPRESPONSE_RESULT		0
/**************************************************************/


/*************************** public ***************************/

int httprcfg_init( ) {
	httprcfg_httpInit();
#if(USE_TESTCASES == 1)
	httprcfg_testJsonParser();
#endif 
	return 0;
}

int httprcfg_shutdown( ) {
	httprcfg_httpShutdown();
	return 0;
}

/**************************************************************/
/**************************** General *************************/
/**************************************************************/

// private
static HttpRCfgResult_t sendGetNodeInfo(const char* remoteAddress, int port, const char* clientID, const char* accessToken, 
	AppCfgNodeInfo* response) {
	if (0 == remoteAddress || 0 == response) {
		REQUIRE(0 != remoteAddress);
		REQUIRE(0 != response);
		return RESULT_EC_PARAMS;
	}

	static const char* const endpoint = "/nodeinfo.json";
	const HttpRequest::protocol_t protocolType = HttpRequest::PROTOCOL_HTTP;
	HttpResponse httpResponse;
	HttpRequest httpRequest(remoteAddress, port, endpoint, protocolType);
	httpRequest.addHeader(std::make_pair(std::string("Accept"), std::string("application/json")));

	if (0 != clientID && 0 != accessToken) {
		// header	Authorization: Base64{ str1:str2 }
		std::string authHeader = AuthHelper::CreateBasicBase64Header(clientID, accessToken);
		if (authHeader.length() <= 0) {
			return RESULT_EC_HTTP_FAILED;			// TODO EEN ANDERE ERROR CODE !!
		}
		httpRequest.addHeader(std::make_pair("Authorization", authHeader));
	}

#if(USE_VERBOSE_ON == 1)
	httpRequest.setVerboseOnOff(true);
#endif 
#if(DUMP_HTTPRESPONSE_RESULT==1)
	httpRequest.setDumpResponse(true);
#endif 

	// Uitvoeren van de Get
	HttpRCfgResult_t result = httprcfg_GET(httpRequest, &httpResponse);
	if (result != RESULT_SUCCESS) {
		return result;
	}

	// Json data parsen.
	JsonAppNodeInfo jsonParser;
	result = jsonParser.parse(response, httpResponse.getDataBody());
	if (result != RESULT_SUCCESS) {
		return result;
	}
	return RESULT_SUCCESS;
}

// public
HttpRCfgResult_t httprcfg_sendGetNodeInfo(const char* remoteAddress,int port,AppCfgNodeInfo* response) {
	return sendGetNodeInfo(remoteAddress, port, 0, 0, response);
}

HttpRCfgResult_t httpsrcfg_sendGetNodeInfo(const char* remoteAddress,
	int port, const char* clientID, const char* accessCode, AppCfgNodeInfo* response) 
{
	if  (0 == clientID || 0 == accessCode) {
		REQUIRE(0 != clientID);
		REQUIRE(0 != accessCode);
		return RESULT_EC_PARAMS;
	}
	return sendGetNodeInfo(remoteAddress, port, clientID, accessCode, response);
}

/**************************************************************/
/**************************** Bindings ************************/
/**************************************************************/

// private...
static HttpRCfgResult_t sendStartBindingsAction(const char* remoteAddress, int port, const char* clientID, const char* accessToken, 
	const AppCfgBindAction& action, AppCfgResult* response)
{
	if (0 == remoteAddress || 0 == response) {
		REQUIRE(0 != remoteAddress);
		REQUIRE(0 != response);
		return RESULT_EC_PARAMS;
	}

	const HttpRequest::protocol_t protocolType = HttpRequest::PROTOCOL_HTTP;
	static const char* const endpoint = "/rconfig/bindings/start";
	HttpResponse httpResponse;
	HttpRequest httpRequest(remoteAddress, port, endpoint, protocolType);
	httpRequest.addHeader(std::make_pair("Accept", "application/json"));

	if (0 != clientID && 0 != accessToken) {
		// header	Authorization: Base64{ str1:str2 }
		std::string authHeader = AuthHelper::CreateBasicBase64Header(clientID, accessToken);
		if (authHeader.length() <= 0) {
			return RESULT_EC_HTTP_FAILED;			// TODO EEN ANDERE ERROR CODE !!
		}
		httpRequest.addHeader(std::make_pair("Authorization", authHeader));
	}

#if(USE_VERBOSE_ON == 1)
	httpRequest.setVerboseOnOff(true);
#endif 
#if(DUMP_HTTPRESPONSE_RESULT==1)
	httpRequest.setDumpResponse(true);
#endif 

	// Json Data body invullen.
	std::string dataBody;
	JsonAppCfgBindAction jsonFormatter;
	if (!jsonFormatter.format(&dataBody, action)) {
		return RESULT_EC_PARAMS;
	}
	httpRequest.setDataBody(dataBody.c_str());

	// Uitvoeren van de post.
	HttpRCfgResult_t result = httprcfg_POST(httpRequest, &httpResponse);
	if (result != RESULT_SUCCESS) {
		return result;
	}

	// Parsen van Json Result.
	JsonAppCfgResult jsonParser;
	result = jsonParser.parse(response, httpResponse.getDataBody());
	if (RESULT_SUCCESS != result) {
		return result;
	}
	return RESULT_SUCCESS;
}

static HttpRCfgResult_t sendBindingsGetLog(const char* remoteAddress, int port, const char* clientID, const char* accessToken, AppCfgLogInfo* response)
{
	if (0 == remoteAddress || 0 == response) {
		REQUIRE(0 != remoteAddress);
		REQUIRE(0 != response);
		return RESULT_EC_PARAMS;
	}

	static const char* const endpoint = "/rconfig/bindings/log/info";
	const HttpRequest::protocol_t protocolType = HttpRequest::PROTOCOL_HTTP;
	HttpResponse httpResponse;
	HttpRequest httpRequest(remoteAddress, port, endpoint, protocolType);
	httpRequest.addHeader(std::make_pair("Accept", "application/json"));
	if (0 != clientID && 0 != accessToken) {
		// header	Authorization: Base64{ str1:str2 }
		std::string authHeader = AuthHelper::CreateBasicBase64Header(clientID, accessToken);
		if (authHeader.length() <= 0) {
			return RESULT_EC_HTTP_FAILED;			// TODO EEN ANDERE ERROR CODE !!
		}
		httpRequest.addHeader(std::make_pair("Authorization", authHeader));
	}
#if(USE_VERBOSE_ON == 1)
	httpRequest.setVerboseOnOff(true);
#endif 
#if(DUMP_HTTPRESPONSE_RESULT==1)
	httpRequest.setDumpResponse(true);
#endif 

	// Uitvoeren van de Get
	HttpRCfgResult_t result = httprcfg_GET(httpRequest, &httpResponse);
	if (result != RESULT_SUCCESS) {
		return result;
	}

	// Json Data parsen.
	JsonAppCfgLogInfo jsonParser;
	result = jsonParser.parse(response, httpResponse.getDataBody());
	if (RESULT_SUCCESS != result) {
		return result;
	}
	return RESULT_SUCCESS;
}

static HttpRCfgResult_t sendBindingsGetProgress(const char* remoteAddress, int port, const char* clientID, const char* accessToken, AppCfgBindInfoNodes* response)
{
	if (0 == remoteAddress || 0 == response) {
		REQUIRE(0 != remoteAddress);
		REQUIRE(0 != response);
		return RESULT_EC_PARAMS;
	}

	static const char* const endpoint = "/rconfig/bindings/progress";
	const HttpRequest::protocol_t protocolType = HttpRequest::PROTOCOL_HTTP;
	HttpResponse httpResponse;
	HttpRequest httpRequest(remoteAddress, port, endpoint, protocolType);
	httpRequest.addHeader(std::make_pair("Accept", "application/json"));
	if (0 != clientID && 0 != accessToken) {
		// header	Authorization: Base64{ str1:str2 }
		std::string authHeader = AuthHelper::CreateBasicBase64Header(clientID, accessToken);
		if (authHeader.length() <= 0) {
			return RESULT_EC_HTTP_FAILED;			// TODO EEN ANDERE ERROR CODE !!
		}
		httpRequest.addHeader(std::make_pair("Authorization", authHeader));
	}

#if(USE_VERBOSE_ON == 1)
	httpRequest.setVerboseOnOff(true);
#endif 
#if(DUMP_HTTPRESPONSE_RESULT==1)
	httpRequest.setDumpResponse(true);
#endif 

	// Uitvoeren van de Get
	HttpRCfgResult_t result = httprcfg_GET(httpRequest, &httpResponse);
	if (result != RESULT_SUCCESS) {
		return result;
	}

	// Json data parsen.
	JsonAppBindInfoNodes jsonParser;
	result = jsonParser.parse(response, httpResponse.getDataBody());
	if (RESULT_SUCCESS != result) {
		return result;
	}
	return RESULT_SUCCESS;
}

static HttpRCfgResult_t sendUploadBindFile(const char* remoteAddress,
	int port, const char* clientID, const char* accessToken, int nodeAddress, const char* fileName, AppCfgResult* response)
{
	if (0 == remoteAddress || 0 == fileName || 0 == response) {
		REQUIRE(0 != remoteAddress);
		REQUIRE(0 != fileName);
		REQUIRE(0 != response);
		return RESULT_EC_PARAMS;
	}

	static const char* const endpoint = "/rconfig/files";
	const HttpRequest::protocol_t protocolType = HttpRequest::PROTOCOL_HTTP;	
	HttpResponse httpResponse;
	HttpRequestMultiForm httpRequest(remoteAddress, port, endpoint, fileName, "file", protocolType);
	httpRequest.addHeader(std::make_pair("Accept", "application/json"));
	if (0 != clientID && 0 != accessToken) {
		// header	Authorization: Base64{ str1:str2 }
		std::string authHeader = AuthHelper::CreateBasicBase64Header(clientID, accessToken);
		if (authHeader.length() <= 0) {
			return RESULT_EC_HTTP_FAILED;			// TODO EEN ANDERE ERROR CODE !!
		}
		httpRequest.addHeader(std::make_pair("Authorization", authHeader));
	}

	// Extra multipart formdata...
	char strNodeAddress[8];
	sprintf(strNodeAddress, "%d", nodeAddress);
	httpRequest.addParam(std::make_pair("type", "bindings"));
	httpRequest.addParam(std::make_pair("nodeAddress", strNodeAddress));

#if(USE_VERBOSE_ON == 1)
	httpRequest.setVerboseOnOff(true);
#endif 
#if(DUMP_HTTPRESPONSE_RESULT==1)
	httpRequest.setDumpResponse(true);
#endif 

	// Uitvoeren van de Post
	HttpRCfgResult_t result = httprcfg_POST(httpRequest, &httpResponse);
	if (result != RESULT_SUCCESS) {
		return result;
	}

	// Parsen van de Josn data.
	JsonAppCfgResult jsonParser;
	result = jsonParser.parse(response, httpResponse.getDataBody());
	if (RESULT_SUCCESS != result) {
		return result;
	}
	return RESULT_SUCCESS;
}


// public...
HttpRCfgResult_t httprcfg_sendBindingsGetLog(const char* remoteAddress,int port, AppCfgLogInfo* response) {	
	return sendBindingsGetLog(remoteAddress, port, 0, 0, response);
}

HttpRCfgResult_t httprcfg_sendBindingsGetProgress(const char* remoteAddress,int port,AppCfgBindInfoNodes* response) {
	return sendBindingsGetProgress(remoteAddress, port, 0, 0, response);
}

HttpRCfgResult_t httprcfg_sendStartBindingsAction(const char* remoteAddress,int port,const AppCfgBindAction& action,AppCfgResult* response) {
	return sendStartBindingsAction(remoteAddress, port, 0, 0, action, response);
}

HttpRCfgResult_t httprcfg_sendUploadBindFile(const char* remoteAddress,int port, int nodeAddress, const char* fileName,AppCfgResult* response) {
	return sendUploadBindFile(remoteAddress, port, 0, 0, nodeAddress, fileName, response);	
}

// https 
HttpRCfgResult_t httpsrcfg_sendUploadBindFile(const char* remoteAddress,
	int port, const char* clientID, const char* accessToken, int nodeAddress, const char* fileName,
	AppCfgResult* response)
{
	if (0 == clientID || 0 == accessToken) {
		REQUIRE(0 != clientID);
		REQUIRE(0 != accessToken);
		return RESULT_EC_PARAMS;
	}
	return sendUploadBindFile(remoteAddress, port, clientID, accessToken, nodeAddress, fileName, response);
}

HttpRCfgResult_t httpsrcfg_sendStartBindingsAction(const char* remoteAddress,
	int port, const char* clientID, const char* accessToken, const AppCfgBindAction& action, AppCfgResult* response) 
{
	if (0 == clientID || 0 == accessToken) {
		REQUIRE(0 != clientID);
		REQUIRE(0 != accessToken);
		return RESULT_EC_PARAMS;
	}
	return sendStartBindingsAction(remoteAddress, port, clientID, accessToken, action, response);
}

HttpRCfgResult_t httpsrcfg_sendBindingsGetLog(const char* remoteAddress,
	int port, const char* clientID, const char* accessToken, AppCfgLogInfo* response)
{
	if (0 == clientID || 0 == accessToken) {
		REQUIRE(0 != clientID);
		REQUIRE(0 != accessToken);
		return RESULT_EC_PARAMS;
	}
	return sendBindingsGetLog(remoteAddress, port, clientID, accessToken, response);
}

HttpRCfgResult_t httpsrcfg_sendBindingsGetProgress(const char* remoteAddress,
	int port, const char* clientID, const char* accessToken, AppCfgBindInfoNodes* response)
{
	if (0 == clientID || 0 == accessToken) {
		REQUIRE(0 != clientID);
		REQUIRE(0 != accessToken);
		return RESULT_EC_PARAMS;
	}
	return sendBindingsGetProgress(remoteAddress, port, clientID, accessToken, response);
}

/**************************************************************/
/******************* Backup Binding project *******************/
/**************************************************************/
static HttpRCfgResult_t sendUploadFile(const char* remoteAddress,
	int port, const char* clientID, const char* accessToken, const char* fileName, const char* destinationPath, AppCfgResult* response)
{
	if (0 == remoteAddress || 0 == fileName || 0 == response) {
		REQUIRE(0 != remoteAddress);
		REQUIRE(0 != fileName);
		REQUIRE(0 != response);
		return RESULT_EC_PARAMS;
	}

	static const char* const endpoint = "/fs/upload";
	const HttpRequest::protocol_t protocolType = HttpRequest::PROTOCOL_HTTP;
	HttpResponse httpResponse;
	HttpRequestMultiForm httpRequest(remoteAddress, port, endpoint, fileName, "file", protocolType);
	httpRequest.addHeader(std::make_pair("Accept", "application/json"));
	if (0 != clientID && 0 != accessToken) {
		// header	Authorization: Base64{ str1:str2 }
		std::string authHeader = AuthHelper::CreateBasicBase64Header(clientID, accessToken);
		if (authHeader.length() <= 0) {
			return RESULT_EC_HTTP_FAILED;			// TODO EEN ANDERE ERROR CODE !!
		}
		httpRequest.addHeader(std::make_pair("Authorization", authHeader));
	}

	// Extra multipart formdata...	
	if (0 != destinationPath) {
		httpRequest.addParam(std::make_pair("destination", destinationPath));
	}

#if(USE_VERBOSE_ON == 1)
	httpRequest.setVerboseOnOff(true);
#endif 
#if(DUMP_HTTPRESPONSE_RESULT==1)
	httpRequest.setDumpResponse(true);
#endif 

	// Uitvoeren van de Post
	HttpRCfgResult_t result = httprcfg_POST(httpRequest, &httpResponse);
	if (result != RESULT_SUCCESS) {
		return result;
	}

	// Parsen van de Josn data.
	JsonAppCfgResult jsonParser;
	result = jsonParser.parse(response, httpResponse.getDataBody());
	if (RESULT_SUCCESS != result) {
		return result;
	}
	return RESULT_SUCCESS;
}

static HttpRCfgResult_t sendRequestDirInfo(const char* remoteAddress, int port, const char* clientID, const char* accessToken,
	const char* path, AppCfgFileServerDir* response)
{
	if (0 == remoteAddress || 0 == response) {
		REQUIRE(0 != remoteAddress);
		REQUIRE(0 != response);
		return RESULT_EC_PARAMS;
	}

	// Oppassen: Moeten de url decoderen !!!
	char endpoint[512];
	if (0 != path) {
		sprintf(endpoint, "/fs/dir%s", UriHelper::Encode(path).c_str());
	}
	else {
		strcpy(endpoint, "/fs/dir");
	}

	const HttpRequest::protocol_t protocolType = HttpRequest::PROTOCOL_HTTP;
	HttpResponse httpResponse;
	HttpRequest httpRequest(remoteAddress, port, endpoint, protocolType);
	httpRequest.addHeader(std::make_pair(std::string("Accept"), std::string("application/json")));
	if (0 != clientID && 0 != accessToken) {
		// header	Authorization: Base64{ str1:str2 }
		std::string authHeader = AuthHelper::CreateBasicBase64Header(clientID, accessToken);
		if (authHeader.length() <= 0) {
			return RESULT_EC_HTTP_FAILED;			// TODO EEN ANDERE ERROR CODE !!
		}
		httpRequest.addHeader(std::make_pair("Authorization", authHeader));
	}

#if(USE_VERBOSE_ON == 1)
	httpRequest.setVerboseOnOff(true);
#endif 
#if(DUMP_HTTPRESPONSE_RESULT==1)
	httpRequest.setDumpResponse(true);
#endif 

	// Uitvoeren van de Get
	HttpRCfgResult_t result = httprcfg_GET(httpRequest, &httpResponse);
	if (result != RESULT_SUCCESS) {
		return result;
	}

	// Json data parsen.
	JsonCfgFileServerDir jsonParser;
	result = jsonParser.parse(response, httpResponse.getDataBody());
	if (result != RESULT_SUCCESS) {
		return result;
	}
	return RESULT_SUCCESS;
}

static HttpRCfgResult_t sendDeleteFile(const char* remoteAddress, int port, 
	const char* clientID, const char* accessToken, const char* fileName, AppCfgResult* response)
{
	if (0 == remoteAddress || 0 == response || 0 == fileName) {
		REQUIRE(0 != remoteAddress);
		REQUIRE(0 != response);
		REQUIRE(0 != fileName);
		return RESULT_EC_PARAMS;
	}

	// Oppassen: Moeten de url decoderen !!!
	char endpoint[512];
	sprintf(endpoint, "/fs%s", UriHelper::Encode(fileName).c_str());

	const HttpRequest::protocol_t protocolType = HttpRequest::PROTOCOL_HTTP;
	HttpResponse httpResponse;
	HttpRequest httpRequest(remoteAddress, port, endpoint, protocolType);
	httpRequest.addHeader(std::make_pair("Accept", "application/json"));
	if (0 != clientID && 0 != accessToken) {
		// header	Authorization: Base64{ str1:str2 }
		std::string authHeader = AuthHelper::CreateBasicBase64Header(clientID, accessToken);
		if (authHeader.length() <= 0) {
			return RESULT_EC_HTTP_FAILED;			// TODO EEN ANDERE ERROR CODE !!
		}
		httpRequest.addHeader(std::make_pair("Authorization", authHeader));
	}

#if(USE_VERBOSE_ON == 1)
	httpRequest.setVerboseOnOff(true);
#endif 
#if(DUMP_HTTPRESPONSE_RESULT==1)
	httpRequest.setDumpResponse(true);
#endif 

	// Uitvoeren van de Post
	HttpRCfgResult_t result = httprcfg_DELETE(httpRequest, &httpResponse);
	if (result != RESULT_SUCCESS) {
		return result;
	}

	// Parsen van de Josn data.
	JsonAppCfgResult jsonParser;
	result = jsonParser.parse(response, httpResponse.getDataBody());
	if (RESULT_SUCCESS != result) {
		return result;
	}
	return RESULT_SUCCESS;
}

// public
HttpRCfgResult_t httprcfg_sendUploadFile(const char* remoteAddress,
	int port, const char* fileName, const char* destinationPath, AppCfgResult* response)
{	
	return sendUploadFile(remoteAddress, port, 0, 0, fileName, destinationPath, response);
}

HttpRCfgResult_t httprcfg_sendRequestDirInfo(const char* remoteAddress,
											int port, const char* path, AppCfgFileServerDir* response) 
{
	return sendRequestDirInfo(remoteAddress, port, 0, 0, path, response);
}

HttpRCfgResult_t httprcfg_sendDeleteFile(const char* remoteAddress,
										int port, const char* fileName, AppCfgResult* response) 
{
	return sendDeleteFile(remoteAddress,port,0,0,fileName,response);
}

// https
HttpRCfgResult_t httpsrcfg_sendUploadFile(const char* remoteAddress,
	int port, const char* clientID, const char* accessToken, const char* fileName, const char* destPath, AppCfgResult* response)
{
	if (0 == clientID || 0 == accessToken) {
		REQUIRE(0 != clientID);
		REQUIRE(0 != accessToken);
		return RESULT_EC_PARAMS;
	}
	return sendUploadFile(remoteAddress, port, clientID, accessToken, fileName, destPath, response);
}

HttpRCfgResult_t httpsrcfg_sendRequestDirInfo(const char* remoteAddress,
	int port, const char* clientID, const char* accessToken, const char* path, AppCfgFileServerDir* response) {
	if (0 == clientID || 0 == accessToken) {
		REQUIRE(0 != clientID);
		REQUIRE(0 != accessToken);
		return RESULT_EC_PARAMS;
	}
	return sendRequestDirInfo(remoteAddress, port, clientID, accessToken, path, response);
}

HttpRCfgResult_t httpsrcfg_sendDeleteFile(const char* remoteAddress,
	int port, const char* clientID, const char* accessToken, const char* fileName, AppCfgResult* response) {
	if (0 == clientID || 0 == accessToken) {
		REQUIRE(0 != clientID);
		REQUIRE(0 != accessToken);
		return RESULT_EC_PARAMS;
	}
	return sendDeleteFile(remoteAddress, port, clientID, accessToken, fileName, response);
}
/************************** Authentication ********************/

static HttpRCfgResult_t httprcfg_sendAuth(const char* remoteAddress, int port, const char* endpoint,
	const char* str1, const char* str2, const char* strEncrypt,AppCfgClientCredentials* response) 
{
	if (0 == remoteAddress || 0 == endpoint || 0 == str1 || 0 == str2) {
		REQUIRE(0 != remoteAddress);
		REQUIRE(0 != endpoint);
		REQUIRE(0 != str1);
		REQUIRE(0 != str2);
		return RESULT_EC_PARAMS;
	}

	HttpResponse httpResponse;
	HttpRequest httpRequest(remoteAddress, port, endpoint, HttpRequest::PROTOCOL_HTTP);
	httpRequest.addHeader(std::make_pair("Accept", "application/json"));

	// header	Authorization: Base64{ str1:str2 }
	std::string authHeader;
	if (NULL != strEncrypt) {
		authHeader = AuthHelper::CreateEncryptedBasicBase64Header(str1, str2, strEncrypt);
	}
	else {
		authHeader = AuthHelper::CreateBasicBase64Header(str1, str2);
	}
	if (authHeader.length() <= 0) {
		return RESULT_EC_HTTP_FAILED;			// TODO EEN ANDERE ERROR CODE !!
	}
	httpRequest.addHeader(std::make_pair("Authorization", authHeader));

#if(USE_VERBOSE_ON == 1)
	httpRequest.setVerboseOnOff(true);
#endif 
#if(DUMP_HTTPRESPONSE_RESULT==1)
	httpRequest.setDumpResponse(true);
#endif 

	// Uitvoeren van de GET
	HttpRCfgResult_t result = httprcfg_GET(httpRequest, &httpResponse);
	if (result != RESULT_SUCCESS) {
		return result;
	}

	JsonCfgClientCredentials jsonParser;
	result = jsonParser.parse(response, httpResponse.getDataBody());
	if (RESULT_SUCCESS != result) {
		return result;
	}
	return RESULT_SUCCESS;
}


HttpRCfgResult_t httprcfg_sendAuthCreateClient(const char* remoteAddress,
	int port, const char* clientID, const char* password, AppCfgClientCredentials* response)
{
	static const char* const endpoint = "/auth/create";
	static const char* const secretKey_encrypt = "TomAndJohan";
	return httprcfg_sendAuth(remoteAddress,port, endpoint, clientID, password, secretKey_encrypt, response);
}

HttpRCfgResult_t httprcfg_sendAuthRefreshAccessCode(const char* remoteAddress,
	int port, const char* clientID, const char* refreshToken, AppCfgClientCredentials* response) 
{
	static const char* const endpoint = "/auth/refresh";
	static const char* const secretKey_encrypt = NULL;
	return httprcfg_sendAuth(remoteAddress, port, endpoint, clientID, refreshToken, secretKey_encrypt, response);
}

/**************************************************************/
}; /* namespace DUOTECNO */
/**************************************************************/