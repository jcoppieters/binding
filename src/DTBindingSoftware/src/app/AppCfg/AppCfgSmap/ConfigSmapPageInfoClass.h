// ConfigSmapPageInfoClass.h: interface for the CConfigSmapPageInfoClass class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONFIGSMAPPAGEINFOCLASS_H__5B64974F_CED9_41F2_8A61_1FAE7AB0F641__INCLUDED_)
#define AFX_CONFIGSMAPPAGEINFOCLASS_H__5B64974F_CED9_41F2_8A61_1FAE7AB0F641__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/************************************************************/
#include "app\appcfg\appcfgbase\ConfigExternalClass.h"

#include "utils/mfc/IReportError.h"
#include "utils/mfc/TPtrList.h"
/************************************************************/


/*************************************************************/
namespace DUOTECNO {
namespace DATA {
/*************************************************************/

	enum
	{
		SMAP_PAGE_MAX_NAME_LENGTH = 16
	};

	#pragma pack(1)			/* Oppassen: #pragma pack() toevoegen aan einde */
	struct SerSmapPageInfo 
	{
		BYTE m_pageNR;
		char m_name[ SMAP_PAGE_MAX_NAME_LENGTH ];
	};
	#pragma pack()

	#pragma pack(1)			/* Oppassen: #pragma pack() toevoegen aan einde */
	struct SerSmapPageInfoHeader
	{
		BYTE m_screenmapID;	
		short m_count;
	};
	#pragma pack()

/*************************************************************/
};	// end namespace
};	// end namespace
/*************************************************************/


/*************************************************************/

class CSmapPageInfoData
{
public:
	CSmapPageInfoData( );
	~CSmapPageInfoData( );


	void init( );
	BOOL parse(
		const char* szAttribute,
		const char* szData);
	void dump( );
	int GetData(BYTE *pbData);	// Serializable data opvragen.

protected:
	void removeAll( );

private:
	DUOTECNO::DATA::SerSmapPageInfo m_data;
};


/*************************************************************/

class CSmapPageInfoHeader
{
private:
	
	DUOTECNO::DATA::SerSmapPageInfoHeader m_header;

	/**
	 * @brief	Lijst van pageInfo data.
	 */
	DUOTECNO::MFC_HELPER::TPtrList<CSmapPageInfoData> m_list;

public:
	CSmapPageInfoHeader( );
	~CSmapPageInfoHeader( );
	int GetData(BYTE *pbData);	// Serializable data opvragen.

	void init( );
	void add(CSmapPageInfoData* pEntry);
	CSmapPageInfoData* get(int index);
	void dump( );

	void setIdentifier(BYTE identifier) {
		m_header.m_screenmapID = identifier;
	}
	BYTE getIdentifier() const {
		return m_header.m_screenmapID;
	}
	short getCount( ) const {
		return m_header.m_count;
	}

protected:
	void removeAll( );

};

/*************************************************************/

class CConfigSmapPageInfoClass : public CConfigExternalClass
{
protected:
	/**
	 * @brief	presentatie in bestand	= "SCREENMAP"
	 */
	static const char* const _szHeader;

	/**
	 * @brief	referentie naar error display
	 */
	DUOTECNO::MFC_HELPER::IReportError* const m_pErrorReport;

	/**
	 * @brief	Referentie naar de huidige ingelezen page info header
	 */
	CSmapPageInfoHeader* m_pCurrHeader;


	/**
	 * @brief	Referentie naar de huidig ingelezen page info data.
	 */
	CSmapPageInfoData* m_pCurrData;

	/**
	 * @brief	De huidige index
	 */
	int m_currentIndex;

private:
	/**
	 * @brief	Een lijst van moodHeaders
	 */
	DUOTECNO::MFC_HELPER::TPtrList<CSmapPageInfoHeader> m_list;

public:
	CConfigSmapPageInfoClass(DUOTECNO::MFC_HELPER::IReportError *ptr);
	virtual ~CConfigSmapPageInfoClass();

	CSmapPageInfoHeader* getHeader(int index);

public:
	virtual BOOL ParseHeader(char *szAttribute,char *szData);
	virtual BOOL ParseData(char *szAttribute,char *szData);
	virtual void Initialise(void);
	virtual int GetMax(void) const;
	virtual void dump( );
	virtual int GetData(int index,BYTE *pbData);
	virtual BOOL IsUsed(int index);
	int getStartIndex( ) const { return 0; };

protected:
	void removeAll( );
	void ReportError(const char* szMessage);

};


/***********************************************************//**
 * @class	CConfigPageInfoHeader
 * @brief	Deze klasse wordt gebruikt door de FSM.
 * 
 * @note	deze klasse wordt niet gebruikt om de config file
 *			in te lezen.
 *
 * @author	Mitchell Tom
 * @date	20110216
 **************************************************************/
class CConfigPageInfoHeader : public CConfigExternalClass
{
protected:
	CSmapPageInfoHeader* m_pHeader;

public:
	CConfigPageInfoHeader( );
	~CConfigPageInfoHeader( );

	void set(CSmapPageInfoHeader* pHeader);

public:
	virtual BOOL ParseHeader(char* /*szAttribute*/ ,char* /*szData*/)	// CHANGES_MVS_2008_WARNINGS
	{
		return FALSE;
	}
	virtual BOOL ParseData(char* /*szAttribute*/ ,char* /*szData*/)		// CHANGES_MVS_2008_WARNINGS
	{
		return FALSE;
	}
	virtual void Initialise(void) {
	
	}	
public:
	virtual int GetData(int index,BYTE *pbData);
	virtual BOOL IsUsed(int index);
	virtual int GetMax(void) const;
	virtual void dump( );
	int getStartIndex( ) const;
};

/*************************************************************/


/***********************************************************//**
 * @class	CConfigPageInfoData
 * @brief	Deze klasse wordt gebruikt door de FSM.
 * 
 * @note	deze klasse wordt niet gebruikt om de config file
 *			in te lezen.
 *
 * @author	Mitchell Tom
 * @date	20110216
 ***************************************************************/

class CConfigPageInfoData : public CConfigExternalClass
{
protected:
	/**
	 * @brief	Referentie naar de header.
	 *
	 *			Via de header wordt de data van de entries
	 *			opgehaald.
	 */
	CSmapPageInfoHeader* m_pHeader;

public:
	CConfigPageInfoData( );
	~CConfigPageInfoData( );

	void set(CSmapPageInfoHeader* pHeader);

public:
	virtual BOOL ParseHeader(char *szAttribute,char *szData) {
		return FALSE;
	}
	virtual BOOL ParseData(char *szAttribute,char *szData) {
		return FALSE;
	}
	virtual void Initialise(void) {
	
	}	
	virtual int GetData(int index,BYTE *pbData);
	virtual BOOL IsUsed(int index);
	virtual int GetMax(void) const;
	virtual void dump( );
	int getStartIndex( ) const { return 0; };
};

/***************************************************************/


/*************************************************************/

#endif // !defined(AFX_CONFIGSMAPPAGEINFOCLASS_H__5B64974F_CED9_41F2_8A61_1FAE7AB0F641__INCLUDED_)
