#include "stdafx.h"
#include "AppRemoteCfgUtils.h"

#include "apprcfg/apprcfg.h"
#include "httprcfg/httprcfg_classes.h"
/**************************************************************/
#define REQUIRE				ASSERT
/**************************************************************/
namespace DUOTECNO {
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
bool AppRemoteCfgUtils::loadCredentials(AppCfgClientCredentials* dest,const char* path,const char* hostName, int port)
{
	CString fileName;
	if (!formatFileName(&fileName,path,hostName,port)) {
		return false;
	}

	CFile f;
	if (!f.Open(fileName, CFile::modeRead)) {
		return false;
	}

	std::string strData;
	DWORD dwRead = 0;
	do
	{
		BYTE buffer[4096];
		memset(buffer,0,sizeof(buffer));
		dwRead = f.Read(buffer, 4096);
		if (dwRead>0) {
			strData.append((const char*)buffer,(int)dwRead);
		}		
	} while (dwRead > 0);
	f.Close();

	bool result=false;
	if (apprcfg_jsonParse(dest, strData)) {
		result = true;
	}
	return result;
}


bool AppRemoteCfgUtils::saveCredentials(const char* path,const char* hostName, int port,const AppCfgClientCredentials& src) {
	CString fileName;
	if (!formatFileName(&fileName, path,hostName, port)) {
		return false;
	}

	CFile f;
	if (!f.Open(fileName, CFile::modeWrite | CFile::modeCreate)) {
		return false;
	}

	bool result = false;
	std::string strData;
	if (apprcfg_jsonFormat(&strData,src)) {
		f.Write(strData.c_str(),strData.length());
		f.Flush();
		result=true;
	}
	f.Close();
	return result;
};

bool AppRemoteCfgUtils::deleteCredentials(const char* path, const char* hostName, int port) 
{
	CString fileName;
	if (!formatFileName(&fileName, path, hostName, port)) {
		return false;
	}

	CFile f;
	if (!f.Open(fileName, CFile::modeWrite | CFile::modeCreate)) {
		return false;	
	}
	f.Close();
	return true;
}

// [PATH]/server_[hostname_port].json
bool AppRemoteCfgUtils::formatFileName(CString* dest, const char* path, const char* hostName, int port) {
	if (0 == dest || 0 == hostName || port<0) {		
		return false;
	}
	dest->Format("%s/server_%s_%d.json", path, hostName, port);
	return true;
}
/**************************************************************/
}; /* end namespace*/
/**************************************************************/