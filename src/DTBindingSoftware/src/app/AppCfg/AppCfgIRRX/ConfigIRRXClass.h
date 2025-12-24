/**********************************************************//**
 * @file	configIRRXClass
 * @brief	header bestand voor de CConfigExternalClass klasse.
 *
 * @author	Mitchell Tom
 * @date	20090930
 *
 * @since	0x0506
 *************************************************************/
#ifndef DUOTECNO_CONFIG_IRRXCONFIGCLASS
#define DUOTECNO_CONFIG_IRRXCONFIGCLASS
/*************************************************************/
#include "app\appcfg\appcfgbase\ConfigExternalClass.h"
#include "utils/mfc/IReportError.h"
#include "utils/mfc/TPtrList.h"
/*************************************************************/
class ostream;
/*************************************************************/
namespace DUOTECNO {
namespace DATA {

	/** @brief Serializable klasse */

	#pragma pack(1)	/* Oppassen: #pragma pack() toevoegen aan einde */
	struct CSerIRRXCodeEntry
	{
		unsigned short m_usIndex;		// 0 - 1
		unsigned short m_usIrCode;		// 2 - 3
	};
	#pragma	pack()


/***************************************************************/
};
};
/***************************************************************/

/***************************************************************/
class CConfigIRRXClass : public CConfigExternalClass
{
protected:
	static const char* _szHeader;
	DUOTECNO::DATA::CSerIRRXCodeEntry* m_pCurrentEntry;
	DUOTECNO::MFC_HELPER::IReportError* const m_pErrorReport;
	int m_currentIndex;

private:
	DUOTECNO::MFC_HELPER::TPtrList<DUOTECNO::DATA::CSerIRRXCodeEntry> m_list;

public:
	CConfigIRRXClass(DUOTECNO::MFC_HELPER::IReportError *ptr);
	virtual ~CConfigIRRXClass();

	virtual BOOL ParseHeader(char *szAttribute,char *szData);
	virtual BOOL ParseData(char *szAttribute,char *szData);
	virtual void Initialise(void);
	virtual int GetMax(void) const;
	virtual void dump( );
	virtual int GetData(int index,BYTE *pbData);
	virtual BOOL IsUsed(int index);
	int getStartIndex( );

protected:
	void removeAll( );
	void ReportError(const char* szMessage);
	DUOTECNO::DATA::CSerIRRXCodeEntry* GetEntry(int entry);
};

/*************************************************************/
#endif
/*************************************************************/
