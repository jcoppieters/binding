// BindingFileEntryParserGN.h: interface for the CBindingFileEntryParserGN class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGFILEENTRYPARSERGN_H__4FF3D44F_BC7A_4F09_ABD0_74042C27773A__INCLUDED_)
#define AFX_BINDINGFILEENTRYPARSERGN_H__4FF3D44F_BC7A_4F09_ABD0_74042C27773A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BindingFileEntryParserG.h"

class CBindingFileEntryParserGN : public CBindingFileEntryParserG  
{
	friend class CBindingFileEntry;

public:
	CBindingFileEntryParserGN();

public:
	virtual ~CBindingFileEntryParserGN();

protected:
	virtual BOOL IsTypeOf();

public:	
	virtual BOOL IsTypeG() const{
		return FALSE;
	}

	virtual BOOL IsTypeGN() const {
		return TRUE;
	}
};

#endif // !defined(AFX_BINDINGFILEENTRYPARSERGN_H__4FF3D44F_BC7A_4F09_ABD0_74042C27773A__INCLUDED_)
