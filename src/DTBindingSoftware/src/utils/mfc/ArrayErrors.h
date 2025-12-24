#ifndef INCLUDED_DUOTECNO_MFC_HELPER_ARRAYERRORS_H
#define INCLUDED_DUOTECNO_MFC_HELPER_ARRAYERRORS_H
/*************************************************************/
#include "IReportError.h"
/*************************************************************/
namespace DUOTECNO {
namespace MFC_HELPER {
/*************************************************************/
class CArrayErrors : public IReportError
{
protected:
	int m_errorCnt;
	CStringArray m_strArrayErrors;
	
public:
	CArrayErrors( ) : 
		m_errorCnt( 0 ), 
		m_strArrayErrors( ) 
		{ };
	~CArrayErrors( ) { };

public:
	BOOL GetErrors(int i, const char** szString); 

	void removeAll( );

	int GetNrErrors(void) const
	{
		return m_errorCnt;
	}
	BOOL Empty( ) const 
	{
		return m_errorCnt == 0;
	}

public:		// IReportError
	virtual void reportError(const char* szError);
};
/**************************************************************/
};
};
/**************************************************************/
#endif
/**************************************************************/
