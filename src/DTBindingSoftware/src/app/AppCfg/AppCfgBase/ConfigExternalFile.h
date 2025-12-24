// ConfigExternalFile.h: interface for the CConfigExternalFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONFIGEXTERNALFILE_H__5F86390D_4540_4FEB_8FE2_80378E86B61D__INCLUDED_)
#define AFX_CONFIGEXTERNALFILE_H__5F86390D_4540_4FEB_8FE2_80378E86B61D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////
struct CConfigExternalClass;
/////////////////////////////////////////////////////////////////////

class CConfigExternalFile  
{
protected:
	const int m_maxClass;
	int m_current;
	CConfigExternalClass** const m_pClasses;

public:
	CConfigExternalFile(int MaxClass, CConfigExternalClass** pClasses);
	virtual ~CConfigExternalFile();
	BOOL load(const char* szFileName);
	BOOL save(const char* szFileName);		// since 0x0C05
	void initialise( );

protected:
	BOOL CheckAttribute(char *szAttribute,char *szData);
	BOOL ParseHeader(char *szAttribute,char *szData);
	BOOL ParseData(char *szAttribute,char *szData);

};

#endif // !defined(AFX_CONFIGEXTERNALFILE_H__5F86390D_4540_4FEB_8FE2_80378E86B61D__INCLUDED_)
