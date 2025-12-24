// testZipConsole.cpp : Defines the entry point for the console application.
//


// ref: https://github.com/kuba--/zip

#include "stdafx.h"
#include "zip.h"
#include <iostream>
#include <conio.h>
#include <string>
#include <list>

using namespace std;

#define PATH "D:/developer/test installations/binding software installations/0x0F00/20210824 - ts master met emb tcpip slave & pc tcpip/"


static int zipAddEntry(zip_t* archive,const char* path,const char* fileName) {
	int res = zip_entry_open(archive,fileName);
	if (res<0) return res;
	
	char fullPathName[MAX_PATH];
	sprintf(fullPathName,"%s%s",path,fileName);
	res = zip_entry_fwrite(archive, fullPathName);
	if (res<0) return res;

	res = zip_entry_close(archive);
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


//////////////////

static void createZip( ) {
	const char* const zipFileName = "testZip.zip";
	const char* files[] = 
	{
		PATH "bind0a.txt",
		PATH "bindfc.txt"
	};

	zip_create(zipFileName,files,2);
}



// Moeten deze gebruiken zodat ook de subdirectories gezipt worden !!
static void createZip2( ) {
	const char* const zipFileName = "testZip2.zip";

	zip_t* newArchive = zip_open(zipFileName, ZIP_DEFAULT_COMPRESSION_LEVEL, 'w');
	if (0 != newArchive) {

		list<string> files;
		files.push_back("bind0a.txt");
		files.push_back("bindfc.txt");
		files.push_back("bind03.txt");
		files.push_back("bind04.txt");
		files.push_back("Screenmaps/Scrmap_05_0.bin");
		files.push_back("Screenmaps/Scrmap_05_1.bin");
		files.push_back("Screenmaps/Scrmap_05_2.bin");

		zipAddEntries(newArchive,PATH,files);

		
#if(0)
		zipAddEntry(newArchive, PATH, "bind0a.txt");
		zipAddEntry(newArchive, PATH, "bindfc.txt");
		zipAddEntry(newArchive, PATH, "bind03.txt");
		zipAddEntry(newArchive, PATH, "bind04.txt");
		zipAddEntry(newArchive, PATH, "Screenmaps/Scrmap_05_0.bin");
#endif 

#if(0)

		zip_entry_open(newArchive,"bind0a.txt");
		{
			zip_entry_fwrite(newArchive, PATH "bind0a.txt");
		}
		zip_entry_close(newArchive);

		zip_entry_open(newArchive,"bindfc.txt");
		{
			zip_entry_fwrite(newArchive, PATH "bindfc.txt");
		}
		zip_entry_close(newArchive);

		zip_entry_open(newArchive,"bind03.txt");
		{
			zip_entry_fwrite(newArchive, PATH "bind03.txt");
		}
		zip_entry_close(newArchive);

		zip_entry_open(newArchive,"bind04.txt");
		{
			zip_entry_fwrite(newArchive, PATH "bind04.txt");
		}
		zip_entry_close(newArchive);

		zip_entry_open(newArchive,"Screenmaps/Scrmap_05_0.bin");					// << !!
		{
			zip_entry_fwrite(newArchive, PATH "Screenmaps/Scrmap_05_0.bin");
		}
		zip_entry_close(newArchive);
		
#endif 
		zip_close(newArchive);
	}	
}

static void createZip3( ) {
	const char* const zipFileName = "testZip3.zip";

	std::string str1 = PATH "bind0a.txt";
	std::string str2 = PATH "bindfc.txt";

	const char* files[2];
	files[0] = str1.c_str();
	files[1] = str2.c_str();
	zip_create(zipFileName,files,2);
}

static void createZip4( ) {

	const char* const zipFileName = "testZip4.zip";

	list<std::string> strings;
	strings.push_back(PATH "bind0a.txt");
	strings.push_back(PATH "bindfc.txt");
	strings.push_back(PATH "bind03.txt");
	strings.push_back(PATH "bind04.txt");
	strings.push_back(PATH "Screenmaps/Scrmap_05_0.bin");

	int i=0;
	const char** files = new const char*[strings.size()];
	for (list<std::string>::iterator it = strings.begin(); it != strings.end(); ++it,++i) {
		files[i] = it->c_str();
	}
	zip_create(zipFileName,files,strings.size());
	delete [] files;
}



int _tmain(int argc, _TCHAR* argv[])
{
	createZip();
	createZip2();
	createZip3();
	createZip4();

	cout << "press return to exit" << endl;
	getch();
	return 0;
}

