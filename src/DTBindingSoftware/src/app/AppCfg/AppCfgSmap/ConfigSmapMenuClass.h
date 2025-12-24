// ConfigSmapMenuClass.h: interface for the CConfigSmapMenuClass class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONFIGSMAPMENUCLASS_H__A38B40E2_8C46_4BBE_9AEE_EDE76CECF61B__INCLUDED_)
#define AFX_CONFIGSMAPMENUCLASS_H__A38B40E2_8C46_4BBE_9AEE_EDE76CECF61B__INCLUDED_

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

	#pragma pack(1)			/* Oppassen: #pragma pack() toevoegen aan einde */
	struct SerSmapMenu
	{
		BYTE m_position;					// Icon positie nr.
		BYTE m_function;					// Screenmap functie.
		BYTE m_state;						// 0 of 1		
	};
	#pragma pack()

/***************************************************/
};
};
/***************************************************/


class CSerSmapMenu
{
public:
	CSerSmapMenu( );
	~CSerSmapMenu( );

	BYTE getPosition( ) const { return m_data.m_position; };
	BYTE getFunction( ) const { return m_data.m_function; };
	BYTE getState( ) const { return m_data.m_state; };
	void setPosition(BYTE position) { m_data.m_position = position; }
	void setFunction(BYTE function) { m_data.m_function = function; }
	void setState(BYTE state) { m_data.m_state = state; }

	int getData(BYTE* pbData);
	void dump( );
	BOOL save(FILE* fs);

protected:
	void init( );
	BOOL getString(char* pbData,int maxLength,int* length);

private:
	DUOTECNO::DATA::SerSmapMenu m_data;
};


/*****************************************************/

class CConfigSmapMenuClass : public CConfigExternalClass 
{
public:
	CConfigSmapMenuClass(DUOTECNO::MFC_HELPER::IReportError *ptr);
	virtual ~CConfigSmapMenuClass();

	CSerSmapMenu* getEntry(int index);
	virtual BOOL ParseHeader(char *szAttribute,char *szData);
	virtual BOOL ParseData(char *szAttribute,char *szData);
	virtual void Initialise(void);
	virtual int GetMax(void) const;
	virtual void dump( );
	virtual int GetData(int index,BYTE *pbData);
	virtual BOOL IsUsed(int index);
	int getStartIndex( ) const { return 0; };
	BOOL save(FILE* fs);

protected:
	void removeAll( );
	void ReportError(const char* szMessage);
	BOOL addEntry(CSerSmapMenu* p);

private:
	/**
	 * @brief	presentatie in bestand	= "ICON"
	 */
	static const char* const _szHeader;

	/**
	 * @brief	referentie naar error display
	 */
	DUOTECNO::MFC_HELPER::IReportError* const m_pErrorReport;

	
	/**
	 * @brief	Een lijst van moodHeaders
	 */
	DUOTECNO::MFC_HELPER::TPtrList<CSerSmapMenu> m_list;

	/**
	 * @brief	De huidige 
	 */
	CSerSmapMenu* m_pCurrent;
};



#endif // !defined(AFX_CONFIGSMAPMENUCLASS_H__A38B40E2_8C46_4BBE_9AEE_EDE76CECF61B__INCLUDED_)
