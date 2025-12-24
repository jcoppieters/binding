/******************************************************//**
 * @file	win32\dtfilefindwin32.h
 * @brief	Header bestand voor win32 file functies.
 *
 * @author	Mitchell Tom
 * @date	20100420
 *
 * @since	0x0562
 *
 *********************************************************/

#ifndef DUOTECNO_WIN32_FILEFIND_H
#define DUOTECNO_WIN32_FILEFIND_H
/**********************************************************/
namespace DUOTECNO {
/**********************************************************/

/**********************************************************/
/********************* CFileFindData **********************/
/**********************************************************/
class CFileFindData
{
public:
	CFileFindData( );
	~CFileFindData( );
	operator LPWIN32_FIND_DATA( );
	const char* getFileName( ) const;
	bool dir() const;

private:
	WIN32_FIND_DATA m_data;
};

/**************** CFileFindData inline functies **********/
inline const char* CFileFindData::getFileName( ) const {
	return m_data.cFileName;
}

inline bool CFileFindData::dir() const {
	return (FILE_ATTRIBUTE_DIRECTORY == (m_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY));
}
/*********************************************************/

/**********************************************************/
/******************** IFileFindIterator *******************/
/**********************************************************/
struct IFileFindIterator
{
	virtual ~IFileFindIterator( )						= 0;
	virtual void begin( )								= 0;
	virtual void next( )								= 0;
	virtual bool isDone( ) const						= 0;
	virtual CFileFindData* currentItem( )				= 0;
	virtual const CFileFindData* currentItem( ) const	= 0;
};

/**********************************************************/
/******************** CFileFindFactory ********************/
/**********************************************************/
struct CFileFindFactory
{
	static IFileFindIterator* createIterator(const char* szFileName);
	static void destroyIterator(IFileFindIterator* it);
};
/***********************************************************/

/**********************************************************/
/******************* CFileFindFunctions *******************/
/**********************************************************/
struct CFileFindFunctions
{
	static int getCount(const char* szFileName);
	static bool find(const char* szFileName);
};

/**********************************************************/
}; /* namespace DUOTECNO */
/************************************************************/
#endif
/************************************************************/