#ifndef SHELLFUNCTIONS
#define SHELLFUNCTIONS

#ifndef EXTERN
	#ifdef MAIN
		#define EXTERN 
	#else
		#define EXTERN extern
	#endif

#endif

EXTERN BOOL CreateSubDirectory(CString szPathName,CString szNewSubDir);
EXTERN DWORD CreateSubDirectory(CString szNewPathName);
EXTERN BOOL FindFile(CString szPathName,CString szFileName);
EXTERN BOOL FindFile(CString szFulPathName,WIN32_FIND_DATA *FileInfo);
EXTERN BOOL FindFile(CString szFulPathName);
EXTERN BOOL FindFileDate(CString szFulPathName,CString *szFileDate);


#endif