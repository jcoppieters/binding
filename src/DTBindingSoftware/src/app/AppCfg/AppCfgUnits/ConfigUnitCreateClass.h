/**********************************************************//**
 * @file	ConfigExternalClass
 * @brief	header bestand voor de CConfigExternalClass klasse.
 *
 * @author	Mitchell Tom
 * @date	20090423
 *
 * @since	0x0560
 *************************************************************/
#ifndef DUOTECNO_CONFIG_UNITCREATECLASS
#define DUOTECNO_CONFIG_UNITCREATECLASS
/*************************************************************/
#include "app\appcfg\appcfgbase\ConfigExternalClass.h"
#include "utils/mfc/IReportError.h"
#include "utils/mfc/TPtrList.h"
/*************************************************************/
class ostream;
/*************************************************************/
namespace DUOTECNO {
namespace DATA {

	enum { UNIT_NAME_LENGTH = 16 };

	#pragma pack(1)			/* Oppassen: #pragma pack() toevoegen aan einde */
	struct CSerUnit
	{
		char m_szName[UNIT_NAME_LENGTH];    			// 0  - 15
		BYTE m_bNodeLogicalAddress;						// 16 - 17
		BYTE m_bUnitAddress;							// 17 - 18
		BYTE m_bUnitType;								// 18 - 19
		BYTE m_bUnitFlags;								// 19 - 20
	};
	#pragma	pack()

	
/***************************************************************/
};
};
/***************************************************************/

/***************************************************************/
class CConfigUnitCreateClass : public CConfigExternalClass
{
protected:
	static const char* _szHeader;
	DUOTECNO::DATA::CSerUnit* m_pCurrentUnit;
	DUOTECNO::MFC_HELPER::IReportError* const m_pErrorReport;
	int m_currentIndex;

private:
	DUOTECNO::MFC_HELPER::TPtrList<DUOTECNO::DATA::CSerUnit> m_list;

public:
	CConfigUnitCreateClass(DUOTECNO::MFC_HELPER::IReportError *ptr);
	virtual ~CConfigUnitCreateClass();

public:
	virtual BOOL ParseHeader(char *szAttribute,char *szData);
	virtual BOOL ParseData(char *szAttribute,char *szData);
	virtual void Initialise(void);
	virtual int GetMax(void) const;
	virtual void dump( );

public:
	virtual int GetData(int index,BYTE *pbData);
	virtual BOOL IsUsed(int index);
	int getStartIndex( );

public:
	DUOTECNO::DATA::CSerUnit* CConfigUnitCreateClass::GetEntry(int entry);

protected:
	void removeAll( );
	void ReportError(const char* szMessage);
	

};

/*************************************************************/
#endif
/*************************************************************/
