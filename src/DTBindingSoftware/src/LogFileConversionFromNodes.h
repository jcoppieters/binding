// LogFileConversionFromNodes.h: interface for the CLogFileConversionFromNodes class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGFILECONVERSIONFROMNODES_H__31B77E5A_CA73_43A8_B904_15E92116AC80__INCLUDED_)
#define AFX_LOGFILECONVERSIONFROMNODES_H__31B77E5A_CA73_43A8_B904_15E92116AC80__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "LogFile.h"

class CBindingUnit;
class CBindingEntry;
class CBindingPropertyUnit;
class CBindingFile;
class CLogFileConversionFromNodesErrors;
class CLogFileConversionConstruct;


/****************************************************************************************/
// Changes V16.16:
// - Functie getErrors( ) aangemaakt.
//   Vroeger was er een variabele m_pErrors die in de ctor geinitialisseerd werd door
//	 waarde aan toe te kennen via de singleton CLogFile::GetConversionFromNodesErrors()
//   nu wordt deze functie rechtsreeks gebruikt in de functie getErrors( )
class CLogFileConversionFromNodes : public CLogFile  
{
protected:	
	static unsigned short m_usCurrentBindingNr;
	static CStringArray* m_pCurrentStrings;

	CLogFileConversionFromNodesErrors* getErrors();		// Aanpassing V16.16 

protected:
	CLogFileConversionConstruct* m_pConstruct;

public:
	CLogFileConversionFromNodes();
	virtual ~CLogFileConversionFromNodes();

protected:
	BOOL _CreateFile(CString szFileName);
	void Report(CStringArray* pStrings,BOOL fDisplayIndex = TRUE);
	//void Report(unsigned usBindingNr,CStringArray* pStrings);

public:
	BOOL Search(unsigned short usBindingNr);
	BOOL Process(CStringArray *pStringArray);
	BOOL Success(const CBindingEntry* const pBindingEntry);
	BOOL Success(CString szMessage,const CBindingPropertyUnit* const pBindingPropertyUnit = 0);
	//BOOL Failed(const CBindingPropertyUnit* const pBindingPropertyUnit = 0);
	BOOL Failed(const CBindingUnit* const pBindingUnit,CString szPropertyBinding);
	BOOL Failed(CStringArray *pStringArray);
	BOOL Found(CString string,CBindingFile* const pBindingFile,CStringArray* pStringArray);
	BOOL Replaced(CStringArray* const pStringArray,CStringArray* const pStringsReplaced);
	BOOL ReportErrors(CString szError,CBindingFile* const pBindingFile,CStringArray* pStringArray);
	BOOL AddHeader(CString szMessage);

	//BOOL Replaced(CString string,unsigned short usStart,unsigned short usStop);
	BOOL AddWarning(CString s);
	

protected:
	virtual BOOL _Start();
	virtual BOOL _Stop();
};

#endif // !defined(AFX_LOGFILECONVERSIONFROMNODES_H__31B77E5A_CA73_43A8_B904_15E92116AC80__INCLUDED_)
