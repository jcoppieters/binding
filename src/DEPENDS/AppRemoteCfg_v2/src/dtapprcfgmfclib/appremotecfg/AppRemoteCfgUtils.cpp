#include "stdafx.h"
#include "AppRemoteCfgUtils.h"
#include "AppRemoteCfgcmn.h"		// Private key

// #include "apprcfg/apprcfg.h"
#include "httprcfg/httprcfg_classes.h"
#include "httprcfg/httprcfg_json.h"
#include "json/jsonserverscfg.h"
/**************************************************************/
#define REQUIRE				ASSERT
/**************************************************************/
#define USE_CREDENTIALS_SECURED		1
/**************************************************************/
#if (USE_CREDENTIALS_SECURED == 1)
	#include "httprcfg/httprcfg_http.h"		//AuthHelper
#endif
/**************************************************************/
namespace DUOTECNO {
/**************************************************************/

/**************************************************************/
/****************** Json format & parse. **********************/
/**************************************************************/
template <class T_JSONCLASS, class T_DATA>
static bool thelper_JsonFormat(std::string* dest, const T_DATA& src)
{
	T_JSONCLASS jsonFormatter;
	return jsonFormatter.format(dest, src);
}

template <class T_JSONCLASS, class T_DATA>
static bool thelper_JsonParse(T_DATA* dest, const std::string& str)
{
	T_JSONCLASS jsonParser;
	return (RESULT_SUCCESS == jsonParser.parse(dest, str) ? true : false);
}

static bool jsonFormat(std::string* dest, const AppCfgClientCredentials& src) {
	return thelper_JsonFormat<JsonCfgClientCredentials, AppCfgClientCredentials>(dest, src);
}
static bool jsonParse(AppCfgClientCredentials*dest, const std::string& str) {
	return thelper_JsonParse<JsonCfgClientCredentials, AppCfgClientCredentials>(dest, str);
}

static bool jsonFormat(std::string* dest, const AppRemoteCfgServers& src) {
	return thelper_JsonFormat<JsonServersCfg, AppRemoteCfgServers>(dest, src);
}
static bool jsonParse(AppRemoteCfgServers*dest, const std::string& str) {
	return thelper_JsonParse<JsonServersCfg, AppRemoteCfgServers>(dest, str);
}
/**************************************************************/

/**************************************************************/
/***************************** File IO ************************/
/**************************************************************/
static bool deleteFile(const CString& fileName) {
	CFile f;
	if (!f.Open(fileName, CFile::modeWrite | CFile::modeCreate)) {
		return false;
	}
	f.Close();
	return true;
}

static bool helper_saveEncryptedData(CFile* dest, const std::string& strData) {
	const char* const strPrivateKey = json_getAuthPrivateKey();
	const std::string strDataEncode = AuthHelper::encode(strData.c_str(), strPrivateKey);
	dest->Write(strDataEncode.c_str(), strDataEncode.length());
	dest->Flush();
	return true;
}
static bool helper_saveData(CFile* dest, const std::string& strData) {
	dest->Write(strData.c_str(), strData.length());
	dest->Flush();
	return true;
}

// Templates...
template <class T_DATA>
static bool thelper_save(const CString& fileName, const T_DATA& src) {
	CFile f;
	if (!f.Open(fileName, CFile::modeWrite | CFile::modeCreate)) {
		return false;
	}
	bool result = false;
	std::string strData;
	if (jsonFormat(&strData, src)) {
#if (USE_CREDENTIALS_SECURED == 1)
		result = helper_saveEncryptedData(&f,strData);
#else 
		result = helper_saveData(&f, strData);
#endif 
	}
	f.Close();
	return result;
}

// Templates...
template <class T_DATA>
static bool thelper_load(const CString& fileName, T_DATA* dest) {
	CFile f;
	if (!f.Open(fileName, CFile::modeRead)) {
		return false;
	}

	std::string strData;
	DWORD dwRead = 0;
	do {
		BYTE buffer[4096];
		memset(buffer, 0, sizeof(buffer));
		dwRead = f.Read(buffer, 4096);
		if (dwRead>0) {
			strData.append((const char*)buffer, (int)dwRead);
		}
	} while (dwRead > 0);
	f.Close();

	bool result = false;

#if (USE_CREDENTIALS_SECURED == 1)
	const char* const strPrivateKey = json_getAuthPrivateKey();
	const std::string strDataDecoded = AuthHelper::decode(strData.c_str(), strPrivateKey);
	if (jsonParse(dest, strDataDecoded)) {
		result = true;
	}
#else
	if (apprcfg_jsonParse(dest, strData)) {
		result = true;
	}
#endif 
	return result;
}
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
bool AppRemoteCfgUtils::loadCredentials(AppCfgClientCredentials* dest,const char* path,const char* hostName, int port) {
	CString fileName;
	if (!formatServerCredentialsFileName(&fileName,path,hostName,port)) {
		return false;
	}
	return thelper_load<AppCfgClientCredentials>(fileName,dest);
}

bool AppRemoteCfgUtils::saveCredentials(const char* path,const char* hostName, int port,const AppCfgClientCredentials& src) {
	CString fileName;
	if (!formatServerCredentialsFileName(&fileName, path,hostName, port)) {
		return false;
	}
	return thelper_save<AppCfgClientCredentials>(fileName, src);
}

bool AppRemoteCfgUtils::deleteCredentials(const char* path, const char* hostName, int port) 
{
	CString fileName;
	if (!formatServerCredentialsFileName(&fileName, path, hostName, port)) {
		return false;
	}
	return deleteFile(fileName);	
}

// inladen & opslaan van de servers info: serversrcfg.dat
bool AppRemoteCfgUtils::loadServers(AppRemoteCfgServers* dest, const char* path) {
	CString fileName;
	if (!formatServersFileName(&fileName, path)) {
		return false;
	}
	return thelper_load<AppRemoteCfgServers>(fileName, dest);
}

bool AppRemoteCfgUtils::saveServers(const char* path, const AppRemoteCfgServers& src) {
	CString fileName;
	if (!formatServersFileName(&fileName, path)) {
		return false;
	}
	return thelper_save<AppRemoteCfgServers>(fileName,src);
}

bool AppRemoteCfgUtils::deleteServers(const char* path) {
	CString fileName;
	if (!formatServersFileName(&fileName, path)) {
		return false;
	}
	return deleteFile(fileName);
}


// [PATH]/server_[hostname_port].dat
bool AppRemoteCfgUtils::formatServerCredentialsFileName(CString* dest, const char* path, const char* hostName, int port) {
	if (0 == dest || 0 == hostName || port<0) {		
		return false;
	}
	dest->Format("%s/server_%s_%d.dat", path, hostName, port);
	return true;
}

// [PATH]/serversremotecfg.dat
bool AppRemoteCfgUtils::formatServersFileName(CString* dest, const char* path) {
	static const char* const fileName = "serversremotecfg.dat";
	if (0 == dest || 0 == path) {
		return false;
	}
	dest->Format("%s/%s", path, fileName);
	return true;
}

/**************************************************************/
}; /* end namespace*/
/**************************************************************/