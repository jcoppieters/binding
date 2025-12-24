#ifndef INCLUDED_SCREENMAPS_H
#define INCLUDED_SCREENMAPS_H
///////////////////////////////////////////////////////////////////////
#include "screenmapid.h"
///////////////////////////////////////////////////////////////////////
class CNodeDatabase;
struct STRUCT_UNIT;
class CCanNode;
///////////////////////////////////////////////////////////////////////
/**
 * @class	IScreenmap
 * @since	0x0571
 */
class IScreenmap
{
public:
	typedef struct 
	{
		BYTE bNodeAddress;
		BYTE bUnitAddress;
		BYTE bPageNr;
		BYTE bButtonNr;
		char szUnitName[16];

	} ENTRY;

	virtual ~IScreenmap( )					= 0;

	virtual int getClassID( ) const			= 0;
	virtual BOOL Save(CString szFilename)	= 0;
	virtual BOOL Load(CString szFilename)	= 0;
	virtual void FillStandard(CCanNode* pNode)	= 0;
	virtual int getNrOfEntries( ) const		= 0;
	virtual ENTRY& getEntry(int entryNR)	= 0;
	virtual const ENTRY& getEntry(int entryNR) const = 0;
	virtual const CScreenmapID getScreenmapID( ) const			= 0;
	virtual void Initialize(void)				= 0;
	virtual void SetMaxButtons(int x)			= 0;		
	virtual void SetTypes(LONG type)			= 0;	
	virtual void setStartPage(BYTE page)		= 0;
	virtual BYTE getStartPage( ) const			= 0;

	virtual int GetButtonsPerPage(void)			= 0;
	virtual void SetModified(BOOL flag)			= 0;
	virtual BOOL IsModified(void) const			= 0;

	virtual BOOL UpdateNames(void)				= 0;
	virtual BOOL ConvertToString(CString *pstr)		= 0;

	virtual BOOL CheckUnitType(STRUCT_UNIT *pUnit) = 0;
	virtual BOOL CheckUnitType(BYTE bNodeAddress,BYTE bUnitAddress) = 0;

	//virtual BOOL ConvertArrayToStructs(CDWordArray *ptrArray)	= 0;

	virtual BOOL createEntries(int count)				= 0;
};



/**************************************************************************//**
 * @class	CScreenmap
 * @brief	Screenmap klasse.
 * @author	Mitchell Tom
 *
 * @changes	TM,v0564: NormalizeString( ) : 0x0D en 0x0A toevoegen.
 * @changes	TM,v0571: Afleiden van IScreenmap
 ******************************************************************************/
class CScreenmap : public IScreenmap
{	
protected:
	CNodeDatabase* const m_pNodeDatabase;	
	int m_nMaxButtonsOnPage;
	int m_nNrOfEntries;
	BYTE m_StartPage;
	LONG m_ulScreenmapFlags;
	BOOL m_fModified;
	ENTRY *m_Screenmap;	
	const CScreenmapID m_ScreenmapID;

public:
	enum
	{
		CLASS_ID = 1001
	};

	CScreenmap(CNodeDatabase *pDatabase, const CScreenmapID& ID);
	virtual ~CScreenmap( );
		
	const CScreenmap& operator=(const CScreenmap& item);

	// IScreenmap
	virtual int getClassID( ) const { return CLASS_ID; };

	virtual ENTRY& getEntry(int entryNR);
	virtual const ENTRY& getEntry(int entryNR) const;
	virtual	void FillStandard(CCanNode* pNode);	
	virtual BOOL Save(CString szFilename);
	virtual BOOL Load(CString szFilename);
	virtual void Initialize(void);

	virtual void setStartPage(BYTE page) {
		m_StartPage = page;
	}
	virtual BYTE getStartPage( ) const {
		return m_StartPage;
	}

	virtual int getNrOfEntries( ) const {
		return m_nNrOfEntries;
	}

	virtual const CScreenmapID getScreenmapID( ) const {
		return m_ScreenmapID;
	}

	virtual void SetMaxButtons(int x) {
		m_nMaxButtonsOnPage = x;
	}
		
	virtual void SetTypes(LONG type) {
		m_ulScreenmapFlags = type; 
	}

	virtual int GetButtonsPerPage(void)	{
		return m_nMaxButtonsOnPage; 
	}

	virtual void SetModified(BOOL flag)	{
		m_fModified = flag;	
	}
	virtual BOOL IsModified(void) const {
		return m_fModified;	
	}

	virtual BOOL UpdateNames(void);
	virtual BOOL ConvertToString(CString *pstr);
	virtual BOOL CheckUnitType(STRUCT_UNIT *pUnit);
	virtual BOOL CheckUnitType(BYTE bNodeAddress,BYTE bUnitAddress);
	//virtual BOOL ConvertArrayToStructs(CDWordArray *ptrArray);
	virtual BOOL createEntries(int count);
};

/******************************************************************************/


/**************************************************************************//**
 * @class	CScreenmapMain
 * @brief	CScreenmapMain klasse.
 * @author	Mitchell Tom
 *
 * @since	v0571
 ******************************************************************************/

class CScreenmapMain : public CScreenmap
{
public:
	enum
	{
		CLASS_ID = 1002
	};

	CScreenmapMain(CNodeDatabase *pDatabase, const CScreenmapID& ID);
	virtual ~CScreenmapMain( );

	virtual	void FillStandard(CCanNode* pNode);
	virtual int getClassID( ) const { return CLASS_ID; };

	const CScreenmapMain& operator=(const CScreenmapMain& item);
};

/******************************************************************************/

// Nieuw type screenmapping.
// page;button;nodeaddress;unitaddress;name;
//
// Wordt gebruikt voor de MyHome screenmapping V66.
class CScreenmap_V2 : public CScreenmap
{
public:
	enum
	{
		CLASS_ID = 1003
	};

	CScreenmap_V2(CNodeDatabase *pDatabase, const CScreenmapID& ID);
	virtual ~CScreenmap_V2( );

	virtual	void FillStandard(CCanNode* pNode);
	virtual BOOL Save(CString szFilename);
	virtual BOOL Load(CString szFilename);	
	virtual int getClassID( ) const { return CLASS_ID; };

	const CScreenmap_V2& operator=(const CScreenmap_V2& item);

protected:
	CScreenmap::ENTRY* create(int pageNR,int buttonNR,int nodeAddress,int unitAddress);
};

/////////////////////////////////////////////////////////////////////////////


/**
 * @class	CScreenmapUtil
 * @since	0x0571
 */

struct CScreenmapUtil
{
public:
	static BOOL Copy(
		IScreenmap *pDest,
		const IScreenmap* pSrc);

	static BOOL CheckStringForErrors(
		IScreenmap* pScreenmap,
		CString szBuffer,
		int iLength,
		CString *szRet);

	static BOOL ConvertStringToStructs(
		IScreenmap* pScreenmap,
		CString szBuffer,
		int iLength,
		CString *szRet);

	static BOOL NormalizeString(CString* pstr);

	static BOOL ConvertArrayToStructs(
		IScreenmap* pScreenmap,
		CDWordArray *ptrArray);

	static void format(const CScreenmapID& ref,CString* pStr);

protected:
	static BOOL CheckArrayForErrors(
		IScreenmap* pScreenmap,
		CDWordArray *ptrArray,
		CString *szRet);	

	static BOOL ConvertStringToArray(
		IScreenmap* pScreenmap,
		CString szBuffer,
		int iLength,
		CDWordArray *ptrArray);

	static BOOL CorrectArray(
		IScreenmap* pScreenmap,
		CDWordArray *ptrArray);

	static BOOL CorrectScreenmap(IScreenmap* pScreenmap);		
};

//////////////////////////////////////////////////////////////////////////////



#endif