// ConfigSmapSmapInfoClass.h: interface for the CConfigSmapSmapInfoClass class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONFIGSMAPSMAPINFOCLASS_H__3722985E_598A_4B8C_B0DB_1FA9DB24E6FC__INCLUDED_)
#define AFX_CONFIGSMAPSMAPINFOCLASS_H__3722985E_598A_4B8C_B0DB_1FA9DB24E6FC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/***************************************************/
#include "app\appcfg\appcfgbase\ConfigExternalClass.h"

#include "utils/mfc/IReportError.h"
#include "utils/mfc/TPtrList.h"
/***************************************************/


/***************************************************/
namespace DUOTECNO {
namespace DATA {
/***************************************************/

	#pragma pack(1)				/* Oppassen: #pragma pack() toevoegen aan einde */
	struct SerSmapSmapInfo
	{
		BYTE m_screenmapID;
		BYTE m_startPageNR;		
	};
	#pragma pack()

/***************************************************/
};
};
/***************************************************/


class CSerSmapSmapInfo
{
public:
	CSerSmapSmapInfo( );
	~CSerSmapSmapInfo( );

	void setScreenmapID(int id) 
	{ 
		ASSERT(id >= 0 && id <= 0xFF);					// CHANGES_MVS_2008_WARNINGS		
		m_data.m_screenmapID = static_cast<BYTE>(id);	// CHANGES_MVS_2008_WARNINGS
	};

	void setStartPageNR(int page) 
	{ 
		ASSERT(page >= 0 && page <= 0xFF);				// CHANGES_MVS_2008_WARNINGS		
		m_data.m_startPageNR = static_cast<BYTE>(page);	// CHANGES_MVS_2008_WARNINGS		 
	};

	int getScreenmapID( ) const 
	{ 
		return m_data.m_screenmapID; 
	};
	int getStartPageNR( ) const 
	{ 
		return m_data.m_startPageNR; 
	};

	int getData(BYTE* pbData);
	void dump( );
	BOOL save(FILE* fs);

protected:
	void init( );

private:
	DUOTECNO::DATA::SerSmapSmapInfo m_data;
};


/***************************************************/
class CConfigSmapSmapInfoClass : public CConfigExternalClass 
{
public:
	CConfigSmapSmapInfoClass(DUOTECNO::MFC_HELPER::IReportError *ptr);
	virtual ~CConfigSmapSmapInfoClass();

	CSerSmapSmapInfo* getEntry(int index);
	virtual BOOL ParseHeader(char *szAttribute,char *szData);
	virtual BOOL ParseData(char *szAttribute,char *szData);
	virtual void Initialise(void);
	virtual int GetMax(void) const;
	virtual void dump( );
	virtual int GetData(int index,BYTE *pbData);
	virtual BOOL IsUsed(int index);
	int getStartIndex( ) const { return 0; };
	BOOL save(FILE* fs);
	BOOL addEntry(CSerSmapSmapInfo* p);

protected:
	void removeAll( );
	void ReportError(const char* szMessage);

private:
	/**
	 * @brief	presentatie in bestand	= "SCREENMAP"
	 */
	static const char* const _szHeader;

	/**
	 * @brief	referentie naar error display
	 */
	DUOTECNO::MFC_HELPER::IReportError* const m_pErrorReport;

	
	/**
	 * @brief	Een lijst van moodHeaders
	 */
	DUOTECNO::MFC_HELPER::TPtrList<CSerSmapSmapInfo> m_list;

	/**
	 * @brief	Huidige id van de screenmaps.
	 */
	int m_currScreenmapID;

};
/***************************************************/

#endif // !defined(AFX_CONFIGSMAPSMAPINFOCLASS_H__3722985E_598A_4B8C_B0DB_1FA9DB24E6FC__INCLUDED_)
