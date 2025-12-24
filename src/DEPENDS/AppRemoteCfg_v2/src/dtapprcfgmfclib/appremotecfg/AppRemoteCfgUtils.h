#ifndef INCLUDED_APPREMOTECFGUTILS_H
#define INCLUDED_APPREMOTECFGUTILS_H
/**************************************************************/
#include <string>
/**************************************************************/
namespace DUOTECNO {
/**************************************************************/
class AppCfgClientCredentials;
class AppRemoteCfgServers;
/**************************************************************/
/**************************************************************/
/**************************************************************/
struct AppRemoteCfgUtils 
{
public:
	// server_[hostname_port].dat
	static bool loadCredentials(AppCfgClientCredentials* dest,const char* path, const char* hostName,int port);
	static bool saveCredentials(const char* path, const char* hostName, int port, const AppCfgClientCredentials& src);
	static bool deleteCredentials(const char* path, const char* hostName, int port);

	// inladen & opslaan van de servers info: serversremotecfg.dat
	static bool loadServers(AppRemoteCfgServers* dest,const char* path);
	static bool saveServers(const char* path, const AppRemoteCfgServers& src);
	static bool deleteServers(const char* path);

protected:

	// Format file names.
	static bool formatServerCredentialsFileName(CString* dest, const char* path,const char* hostName, int port);
	static bool formatServersFileName(CString* dest, const char* path);
};
/**************************************************************/
}; /* end namespace*/
/**************************************************************/
#endif 
/**************************************************************/
