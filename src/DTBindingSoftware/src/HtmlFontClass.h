// HtmlFontClass.h: interface for the CHtmlFontClass class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HTMLFONTCLASS_H__CD10128B_28EF_48D2_A36D_B913607688A0__INCLUDED_)
#define AFX_HTMLFONTCLASS_H__CD10128B_28EF_48D2_A36D_B913607688A0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/**
 *
 */
class CHtmlFontClass  
{
protected:
	LPCSTR m_szStyle;
	LPCSTR m_szName;

public:
	CHtmlFontClass(LPCSTR szName,LPCSTR szStyle);
	virtual ~CHtmlFontClass();

	CString GetName() const{
		return m_szName;
	}
	
	CString GetStyle() const{
		return m_szStyle;
	}
};

#endif // !defined(AFX_HTMLFONTCLASS_H__CD10128B_28EF_48D2_A36D_B913607688A0__INCLUDED_)
