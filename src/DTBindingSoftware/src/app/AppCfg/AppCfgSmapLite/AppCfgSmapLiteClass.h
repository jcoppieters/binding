/**********************************************************//**
 * @file	AppCfgSmapLiteClass
 * @brief	header bestand voor de CAppCfgSmapLiteClass klasse.
 *
 * @author	Mitchell Tom
 * @date	20161128
 *
 * @since	0x0C05
 *************************************************************/
#ifndef DUOTECNO_CONFIG_APPCFGSMAPLITECLASS_H
#define DUOTECNO_CONFIG_APPCFGSMAPLITECLASS_H
/*************************************************************/
#include "app\appcfg\appcfgbase\ConfigExternalClass.h"

#include "utils/mfc/IReportError.h"
#include "utils/mfc/TPtrList.h"

/*************************************************************/
namespace DUOTECNO {
namespace DATA {
/*************************************************************/
	enum { SMAP_UNIT_NAME_LENGTH = 16 };

	// Oppassen:
	// Onderstaande struct bevat veel velden die momenteel niet
	// gebruikt worden...

	#pragma pack(1)			/* Oppassen: #pragma pack() toevoegen aan einde */
	struct CSerSmapLiteEntry
	{	
		uint8_t m_bType;		// Momenteel niet gebruikt.
		uint8_t m_bNodeAddress;					
		uint8_t m_bUnitAddress;	
		uint8_t m_bUnitType;	// Momenteel niet gebruikt.
		char m_szName[SMAP_UNIT_NAME_LENGTH];   
		uint16_t m_pageNR;		// Momenteel altijd 0
		uint16_t m_buttonNR;	// Momenteel altijd 0
		uint16_t m_reserved[6]; // Reserved kan gebruikt worden voor: xPos, yPos, ref naar een resourceID, ....
	};
	#pragma	pack()	
/***************************************************************/
};
};
/***************************************************************/


/*************************************************************/
/********************CConfigRoomCtrlAVDest *******************/
/*************************************************************/
class CAppCfgSmapLiteClass : public CConfigExternalClass
{
public:
	CAppCfgSmapLiteClass(DUOTECNO::MFC_HELPER::IReportError* ptr);
	~CAppCfgSmapLiteClass(void);

	virtual BOOL ParseHeader(char *szAttribute,char *szData);
	virtual BOOL ParseData(char *szAttribute,char *szData);
	virtual void Initialise(void);
	virtual int GetMax(void) const;
	virtual void dump( );
	virtual int GetData(int index,BYTE *pbData);
	virtual BOOL IsUsed(int index);
	virtual BOOL GetEntryStrCfg(
			int index,
			char* pszTempBuffer,
			int maxBufferLength);
	int getStartIndex( );

protected:
	void removeAll( );
	void ReportError(const char* szMessage);
	void ParseDataReportError(int currentIndex,const char* szAttribute);
	DUOTECNO::DATA::CSerSmapLiteEntry* GetEntry(int entry);
	BOOL addEntry(DUOTECNO::DATA::CSerSmapLiteEntry* pEntry);

private:
	static const char* const _szHeader;
	DUOTECNO::DATA::CSerSmapLiteEntry* m_pCurrentEntry;
	DUOTECNO::MFC_HELPER::IReportError* const m_pErrorReport;
	int m_currentIndex;

	DUOTECNO::MFC_HELPER::TPtrList
	<
		DUOTECNO::DATA::CSerSmapLiteEntry

	> m_list;

	friend struct CAppCfgSmapLiteClass_Util;

}; /* class CConfigRoomCtrlAVDest */
/*************************************************************/

struct CAppCfgSmapLiteClass_Util
{
	static 
	BOOL createEntry(
		DUOTECNO::DATA::CSerSmapLiteEntry** ppEntry);

	static 
	BOOL findEntry(
		CAppCfgSmapLiteClass& ref,
		int pageNR,
		int buttonNR,
		DUOTECNO::DATA::CSerSmapLiteEntry** ppEntry);

	static
	BOOL addEntry(
		CAppCfgSmapLiteClass& ref,
		DUOTECNO::DATA::CSerSmapLiteEntry* pEntry);
};

/*************************************************************/
#endif /* DUOTECNO_CONFIG_APPCFGSMAPLITECLASS_H */
/*************************************************************/