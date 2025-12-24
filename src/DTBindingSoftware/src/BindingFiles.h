// BindingFiles.h: interface for the CBindingFiles class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINDINGFILES_H__E8C4E381_99D9_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_BINDINGFILES_H__E8C4E381_99D9_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/***************************************************************/
class CTimerDoc;
class CNodeDatabase;
/***************************************************************/
#include "BindingFile1.h"
#include "TPtrListIterator.h"
/***************************************************************/
class CBindingFiles  
{
	typedef TPtrListIterator<CBindingFile*> CBindingFiles_it;

public:
	typedef CBindingFiles_it Iterator;

	CBindingFiles();
	virtual ~CBindingFiles();
	BOOL Save(void);
	BOOL Load(CStringArray* pStrArrayError=0);
	BOOL Add(BYTE bNodeAddress);
	BOOL Delete(BYTE bNodeAddress);
	CBindingFile* Get(BYTE bNodeAddress);
	CBindingFile* GetEntry(BYTE i);	
	void ClearDatabase(void);
	unsigned short Count();
	Iterator CreateIterator( );

private:
	CPtrList List;
};
/************************** inline functions *******************/
inline CBindingFiles::Iterator CBindingFiles::CreateIterator( )
{
	return Iterator( List );
}
/***************************************************************/

#endif // !defined(AFX_BINDINGFILES_H__E8C4E381_99D9_11D8_B865_0050BAC412B1__INCLUDED_)
