#ifndef INCLUDED_APPRCFG_H
#define INCLUDED_APPRCFG_H
/**************************************************************/
#include <list>		// std::list
#include <string>
/**************************************************************/
namespace DUOTECNO {
/**************************************************************/
//class AppCfgClientCredentials;
/**************************************************************/

// Geeft een lijst van de node addressen terug van de binding bestanden die
// gevonden werden in deze directory...
// return waarde: false wanneer er geen gevonden werden.
extern bool apprcfg_findNodeBindingFiles(const char* path,std::list<int>* list);

// 
extern bool apprcfg_findProjectCfgFiles(const char* path,std::list<std::string>* list);

// zip bestand aanmaken.
extern bool apprcfg_createProjectCfgZipFile(const char* path,const char* name);

/**************************************************************/
}; /* namespace DUOTECNO */
/**************************************************************/
#endif
/**************************************************************/