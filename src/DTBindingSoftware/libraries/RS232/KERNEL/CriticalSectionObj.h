// CriticalSectionObj.h: interface for the CCriticalSectionObj class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CRITICALSECTIONOBJ_H__BF769B7D_3E8A_4CEE_886F_89AF81982D73__INCLUDED_)
#define AFX_CRITICALSECTIONOBJ_H__BF769B7D_3E8A_4CEE_886F_89AF81982D73__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//////////////////////////////////////////////////////////////////////
#if defined(INCLUDE_DUOTECNO_RS232)
//////////////////////////////////////////////////////////////////////


/*******************************************************/

namespace DUOTECNO {
namespace KERNEL {
namespace WIN_32 {

/*******************************************************/


class CCriticalSectionObj  
{
protected:
	CRITICAL_SECTION m_hCriticalSection;

public:
	CCriticalSectionObj();
	virtual ~CCriticalSectionObj();

	//BOOL TryEnter( );
	void Enter( );
	void Leave( );
};


/*******************************************************/
};
};
};
/*******************************************************/


#endif



#endif // !defined(AFX_CRITICALSECTIONOBJ_H__BF769B7D_3E8A_4CEE_886F_89AF81982D73__INCLUDED_)
