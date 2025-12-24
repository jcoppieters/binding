/**********************************************************//**
 * @file	ConfigBoseStRenderClass.h
 * @brief	header bestand voor de Bose soundtouch cfg klassen
 *
 * @author	Mitchell Tom
 * @date	20150324
 *
 * @since	0x0802
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

	/***********************************************************************************/
	/***************************** CSerBoseSTCfgRender *********************************/
	/***********************************************************************************/
	enum 
	{ 
		SERCLASS_BOSESTCFG_NAME_LENGTH = 32 
	};

	struct CSerBoseSTCfgRender 
	{
		char szName[SERCLASS_BOSESTCFG_NAME_LENGTH];	/* 0 -31 */
		BYTE pbMACAddress[6];							/* 32-37 */
		BYTE bDestNR;									/* 38-39 */
		BYTE bReserved[3];								/* 40-42 */
		unsigned long ipAddress;						/* 43-47 */
	};
	/***********************************************************************************/	

#pragma	pack()

/*************************************************************/
};	/* namespace DATA */
};	/* namespace DUOTECNO */
/*************************************************************/


/*************************************************************/
/********************CConfigRoomCtrlAVDest *******************/
/*************************************************************/
class CConfigBoseSTCfgRender : public CConfigExternalClass
{
public:
	CConfigBoseSTCfgRender(DUOTECNO::MFC_HELPER::IReportError* ptr);
	~CConfigBoseSTCfgRender(void);

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
	void ParseDataReportError(int currentIndex,const char* szAttribute);
	DUOTECNO::DATA::CSerBoseSTCfgRender* GetEntry(int entry);

private:
	static const char* const _szHeader;
	DUOTECNO::DATA::CSerBoseSTCfgRender* m_pCurrentEntry;
	DUOTECNO::MFC_HELPER::IReportError* const m_pErrorReport;
	int m_currentIndex;

	DUOTECNO::MFC_HELPER::TPtrList
	<
		DUOTECNO::DATA::CSerBoseSTCfgRender

	> m_list;

}; /* class CConfigBoseSTCfgRender */
/*************************************************************/

/*********************** END OF FILE *************************/