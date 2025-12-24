/**********************************************************//**
 * @file	configGatewayUClass
 * @brief	header bestand voor de CConfigGatewayUClass klasse.
 *
 * @author	Mitchell Tom
 * @date	20120820
 *
 * @since	0x0620
 *************************************************************/

#pragma once

#include "app\appcfg\appcfgbase\ConfigExternalClass.h"

#include "utils/mfc/IReportError.h"
#include "utils/mfc/TPtrList.h"

/*************************************************************/
namespace DUOTECNO {
namespace DATA {
/*************************************************************/

#pragma pack(1)	/* Oppassen: #pragma pack() toevoegen aan einde */
	struct CSerGatewayUnivCfgEntry
	{
		BYTE bNodeAddress;					// Node address
		BYTE bUnitAddress;					// Unit address
		BYTE bUnitType;						// Unit Type
		BYTE bFunction;						// De functie
		BYTE bSystemInfo;					// info ivm het aan te sturen systeem.
		unsigned short systemAddress[4];	// het systeem adres 16 bits.
		unsigned short systemAction[4];		// Actie aan te sturen systeem.
		unsigned short systemFunction[4];	// Functie aan te sturen systeem.
		unsigned short systemParams[4];		// Extra systeem params.
	};
#pragma	pack()

/*************************************************************/
};
};
/*************************************************************/


/*************************************************************/
class CConfigGatewayUClass :
	public CConfigExternalClass
{
public:
	CConfigGatewayUClass(DUOTECNO::MFC_HELPER::IReportError* ptr);
	~CConfigGatewayUClass(void);

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
	DUOTECNO::DATA::CSerGatewayUnivCfgEntry* GetEntry(int entry);

private:
	static const char* _szHeader;
	DUOTECNO::DATA::CSerGatewayUnivCfgEntry* m_pCurrentEntry;
	DUOTECNO::MFC_HELPER::IReportError* const m_pErrorReport;
	int m_currentIndex;

	DUOTECNO::MFC_HELPER::TPtrList
	<
		DUOTECNO::DATA::CSerGatewayUnivCfgEntry

	> m_list;
};
/*************************************************************/

// Speciale klasse die ervoor zorgt dat bij het parsen van het configuratie bestand
// "GATEWAYSYSTEM" header mag ingevoegd worden.
// Deze kan nodig zijn voor de configurator tool.

class CConfigGatewayUSystemClass :
	public CConfigExternalClass
{
public:
	CConfigGatewayUSystemClass(DUOTECNO::MFC_HELPER::IReportError* ptr);
	~CConfigGatewayUSystemClass(void);

	virtual BOOL ParseHeader(char *szAttribute,char *szData);
	virtual BOOL ParseData(char *szAttribute,char *szData)		{return TRUE; };
	virtual void Initialise(void)								{/* absorbe */ };
	virtual int GetMax(void) const								{return 0; };
	virtual void dump( )										{/* absorbe */ };
	virtual int GetData(int index,BYTE *pbData)					{return 0; };
	virtual BOOL IsUsed(int index)								{return FALSE; };

private:
	static const char* _szHeader;
};
