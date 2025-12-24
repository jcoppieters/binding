#include "stdafx.h"
#include "apprcfg.h"

#include "httprcfg/httprcfg_json.h"

#include "win32/dtfilefindwin32.h"
#include "zip.h"
/**************************************************************/
namespace DUOTECNO {
/**************************************************************/
using namespace std;
/**************************************************************/
static bool helper_getDirFileNames(const char* path,const char* subPath,std::list<std::string>* list) {
	char strFilter[_MAX_PATH];
	if (0 == subPath) {
		sprintf(strFilter,"%s\\*.*",path);
	}
	else {
		sprintf(strFilter,"%s\\%s\\*.*",path,subPath);
	}
	IFileFindIterator* it = 0;
	for (it = CFileFindFactory::createIterator(strFilter); !it->isDone(); it->next()) {
		CFileFindData* const pFileFindData = it->currentItem( );
		if (pFileFindData) {
			if (!pFileFindData->dir()) {
				const char* fileName = pFileFindData->getFileName();
				if (0 != fileName) {
					if ((0 == strcmp(".",fileName)) || (0 == strcmp("..",fileName))) {
						continue;
					}

					char newName[_MAX_PATH];
					if (0 == subPath) {
						strcpy(newName,fileName);						
					}
					else {
						sprintf(newName,"%s\\%s",subPath,fileName);
					}

					list->push_back(newName);			
				}
			}
		}		
	}
	CFileFindFactory::destroyIterator(it);
	return (list->size()>0 ? true : false);
}

static int zipAddEntry(zip_t* archive,const char* path,const char* fileName) {
	int res = zip_entry_open(archive,fileName);
	if (res<0) { 
		return res;
	}
	
	char fullPathName[MAX_PATH];
	sprintf(fullPathName,"%s\\%s",path,fileName);
	res = zip_entry_fwrite(archive, fullPathName);
	if (res<0) {
		return res;
	}

	res = zip_entry_close(archive);
	if (res < 0) {
		return res;
	}
	return res;
}

static int zipAddEntries(zip_t* archive,const char* path,std::list<string>& files) {

	int count = 0;
	for (list<std::string>::iterator it = files.begin(); it != files.end(); ++it,++count) {
		const int res = zipAddEntry(archive,path,it->c_str());
		if (res<0) {
			break;
		}
	}
	return count;
}


/**************************************************************/

bool apprcfg_findNodeBindingFiles(const char* path,std::list<int>* list) {
	if (0 == path || 0 == list) {
		return false;
	}

	bool result=false;
	char strFilter[_MAX_PATH];
	sprintf(strFilter,"%s\\*.txt",path);
	IFileFindIterator* it = 0;
	for (it = CFileFindFactory::createIterator(strFilter); !it->isDone(); it->next()) {
		CFileFindData* const pFileFindData = it->currentItem( );
		if (pFileFindData) {
			const char* fileName = pFileFindData->getFileName();
			if (0!=fileName) {
				int temp = -1;
				if (sscanf(fileName,"bind%x.txt",&temp)>=1) {
					list->push_back(temp);
					result=true;
				}			
			}
		}
	}
	CFileFindFactory::destroyIterator(it);
	return result;
}

bool apprcfg_findProjectCfgFiles(const char* path,std::list<std::string>* list) {
	if (0 == path || 0 == list) {
		return false;
	}

	// Path van het project: *.duo & bindings...
	helper_getDirFileNames(path,0,list);	
	helper_getDirFileNames(path,"Audioconfig",list);	
	helper_getDirFileNames(path,"Config",list);	
	helper_getDirFileNames(path,"Screenmaps",list);

	return (list->size()>0 ? true : false);
}

bool apprcfg_createProjectCfgZipFile(const char* path,const char* name) {

	std::list<std::string> fileNames;
	bool res = apprcfg_findProjectCfgFiles(path,&fileNames);
	if (false == res) {
		return res;
	}

	zip_t* newArchive = zip_open(name, ZIP_DEFAULT_COMPRESSION_LEVEL, 'w');
	if (0 != newArchive) {
		if (zipAddEntries(newArchive,path,fileNames)>0) {
			res = true;
		}
		zip_close(newArchive);
	}
	return res;
}

// Json format & parse.
bool apprcfg_jsonFormat(std::string* dest, const AppCfgClientCredentials& src) {
	JsonCfgClientCredentials jsonFormatter;
	if (jsonFormatter.format(dest, src)) {
		return true;
	}
	return false;
}

bool apprcfg_jsonParse(AppCfgClientCredentials*dest, const std::string& str) {
	JsonCfgClientCredentials jsonParser;
	if (RESULT_SUCCESS == jsonParser.parse(dest, str)) {
		return true;
	}
	return false;
}

/**************************************************************/
}; /* namespace DUOTECNO */
/**************************************************************/