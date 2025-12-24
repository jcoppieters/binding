/******************************************************//**
 * @file	win32\win32filefind.cpp
 * @brief	Implementatie bestand voor win32 file functies.
 *
 * @author	Mitchell Tom
 * @date	20100420
 *
 * @since	0x0562
 *
 *********************************************************/

#include "stdafx.h"
#include "win32filefind.h"

/*********************************************************/
#define DEBUG_LOCAL_ON		0
/*********************************************************/
namespace
{
	template <class PARAM>
		inline void TRACER(PARAM x)
	{
		#if (DEBUG_LOCAL_ON == 1)
		TRACE(x);
		#endif
	};

	template <class PARAM, class PARAM2>
		inline void TRACER(PARAM x, PARAM2 y)
	{
		#if (DEBUG_LOCAL_ON == 1)
		TRACE( x, y );
		#endif
	};
};

/**********************************************************/
/********************* CFileFindData **********************/
/**********************************************************/
CFileFindData::CFileFindData( ) :
m_data( )
{
}

CFileFindData::~CFileFindData( )
{
}
CFileFindData::operator LPWIN32_FIND_DATA( )
{
	return &m_data;
}
/**********************************************************/


/**********************************************************/
/******************** IFileFindIterator *******************/
/**********************************************************/
IFileFindIterator::~IFileFindIterator( )
{

}	
/**********************************************************/				

/**********************************************************/
/******************** CFileFindIterator *******************/
/**********************************************************/
class CFileFindIterator : public IFileFindIterator
{
public:
	CFileFindIterator(const char* szFileName);
	virtual ~CFileFindIterator( );

	void begin( );
	void next( );
	bool isDone( ) const;
	CFileFindData* currentItem( );
	const CFileFindData* currentItem( ) const;

private:
	bool m_fDone;
	HANDLE m_handle;
	CFileFindData m_current;
};

/**********************************************************/
CFileFindIterator::CFileFindIterator(const char* szFileName) :
m_handle( INVALID_HANDLE_VALUE ),
m_fDone( false )
{
	m_handle = FindFirstFile(szFileName,m_current);		
	
	if (m_handle == INVALID_HANDLE_VALUE)
	{
		DWORD dwError = GetLastError( );
		TRACER("FindFirstFile Error %d\n", dwError);
	}
};

CFileFindIterator::~CFileFindIterator( )
{
	if (m_handle != INVALID_HANDLE_VALUE)
	{
		FindClose( m_handle );
	}
}

void CFileFindIterator::begin( )
{
	m_fDone = false;
}

void CFileFindIterator::next( )
{
	BOOL result = FindNextFile(m_handle, m_current);
	if (result == FALSE)
	{
		DWORD dwError = GetLastError( );
		if (dwError == ERROR_NO_MORE_FILES)
		{
			m_fDone = true;		
		}
		else
		{
			m_fDone = true;
			TRACER("FindFirstFile Error %d\n", dwError);
		}
	}
}

bool CFileFindIterator::isDone( ) const
{
	if (m_handle == INVALID_HANDLE_VALUE || 
		m_fDone == true)
	{
		return true;
	}
	return false;
}

CFileFindData* CFileFindIterator::currentItem( )
{
	return &m_current;
}

const CFileFindData* CFileFindIterator::currentItem( ) const
{
	return &m_current;
}
/**********************************************************/

/**********************************************************/
/******************** CFileFindFactory ********************/
/**********************************************************/
IFileFindIterator* CFileFindFactory::createIterator(
	const char* szFile)
{
	return new CFileFindIterator(szFile);
}

void CFileFindFactory::destroyIterator(IFileFindIterator* it)
{
	if (it)
	{
		delete it;
		it = 0;
	}
}
/**********************************************************/


/**********************************************************/
/******************* CFileFindFunctions *******************/
/**********************************************************/
int CFileFindFunctions::getCount(const char* szFileName)
{
	int retVal = 0;	
	IFileFindIterator* it = 0;
	for (it = CFileFindFactory::createIterator(szFileName);
	     !it->isDone();
		 it->next( ))
	{
		ASSERT (it->currentItem( ) != 0 && 
			    it->currentItem( )->getFileName( ) != 0);	
		
		retVal++;	 		
	}	
	CFileFindFactory::destroyIterator( it );
	return retVal;
}

bool CFileFindFunctions::find(const char* szFileName)
{
	bool retVal = false;

	IFileFindIterator* it = 0;
	for (it = CFileFindFactory::createIterator(szFileName);
	     !it->isDone();
		 it->next( ))
	{
		ASSERT (it->currentItem( ) != 0 && 
			    it->currentItem( )->getFileName( ) != 0);	
		
		retVal = true;
		break;
	}	

	CFileFindFactory::destroyIterator( it );
	return retVal;
}