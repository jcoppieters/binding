/**********************************************************//**
 * @file	configRoomCtrlAVClass.h
 * @brief	header bestand voor de RoomCtrl audiocfg klassen
 *
 * @author	Mitchell Tom
 * @date	20130116
 *
 * @since	0x0625
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
	/***************************** CSerRoomCtrlAVDest **********************************/
	/***********************************************************************************/
	enum
	{
		AUDIOCFG_DESTNAME_MAX 				= 16,
		AUDIOCFG_NODEUNITADDRESS_SIZE 		= 2
	};

	struct CSerRoomCtrlAVDest 
	{
		char m_szName[AUDIOCFG_DESTNAME_MAX];				/* De naam */
		BYTE m_bAudioUnit[AUDIOCFG_NODEUNITADDRESS_SIZE];	/* Adres van de audiounit */
		BYTE m_bDestNR;										/* Dest nr in de audio kamer */
		BYTE m_bMaxVolume;									/* Het maximum volume */
		BYTE m_bType;										/* Bose lf50,v35, ext audio,... */
		BYTE m_bConfig;										/* Extra config veld */
	};
	/***********************************************************************************/
	
	/***********************************************************************************/
	/**************************** CSerRoomCtrlAVSource *********************************/
	/***********************************************************************************/
	enum 
	{ 
		AUDIOCFG_SOURCENAME_MAX 	= 16,
		AUDIOCFG_SOURCE_MAX_FUNC	= 32
	};

	struct CSerRoomCtrlAVSource
	{
		char m_szName[AUDIOCFG_SOURCENAME_MAX];			/* De naam */
		BYTE m_bSourceNR;								/* Src nr in de audio kamer */
		BYTE m_bConfig;									/* extra config veld */
		BYTE m_bFunctionIdx[AUDIOCFG_SOURCE_MAX_FUNC]; /* Screenmapping van de functies */
	};
	/***********************************************************************************/

	/***********************************************************************************/
	/************************** CSerRoomCtrlAVTunerPreset ******************************/
	/***********************************************************************************/
	enum 
	{ 
		AUDIOCFG_TUNERPRESETNAME_MAX 	= 16 
	};

	struct CSerRoomCtrlAVTunerPreset
	{
		char m_szName[AUDIOCFG_TUNERPRESETNAME_MAX];			/* De naam */
		BYTE m_bTunerPreset;
		BYTE m_bSourceNr;
		BYTE m_bConfig;
		unsigned long m_ulTunerFrequency;
	};
	/***********************************************************************************/

	/***********************************************************************************/
	/**************************** CSerRoomCtrlAVFunction *******************************/
	/***********************************************************************************/
	enum 
	{ 
		AUDIOCFG_FUNCTIONNAME_MAX = 16 
	};

	struct CSerRoomCtrlAVFunction
	{
		char m_szName[AUDIOCFG_FUNCTIONNAME_MAX];	/* De naam */
		BYTE m_bFunction;							/* Duotecno functie - Zie tabel */
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
class CConfigRoomCtrlAVDest : public CConfigExternalClass
{
public:
	CConfigRoomCtrlAVDest(DUOTECNO::MFC_HELPER::IReportError* ptr);
	~CConfigRoomCtrlAVDest(void);

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
	DUOTECNO::DATA::CSerRoomCtrlAVDest* GetEntry(int entry);

private:
	static const char* const _szHeader;
	DUOTECNO::DATA::CSerRoomCtrlAVDest* m_pCurrentEntry;
	DUOTECNO::MFC_HELPER::IReportError* const m_pErrorReport;
	int m_currentIndex;

	DUOTECNO::MFC_HELPER::TPtrList
	<
		DUOTECNO::DATA::CSerRoomCtrlAVDest

	> m_list;

}; /* class CConfigRoomCtrlAVDest */
/*************************************************************/

/*************************************************************/
/******************* CConfigRoomCtrlAVSource *****************/
/*************************************************************/
class CConfigRoomCtrlAVSource : public CConfigExternalClass
{
public:
	CConfigRoomCtrlAVSource(DUOTECNO::MFC_HELPER::IReportError* ptr);
	~CConfigRoomCtrlAVSource(void);

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
	DUOTECNO::DATA::CSerRoomCtrlAVSource* GetEntry(int entry);

private:
	static const char* const _szHeader;
	DUOTECNO::DATA::CSerRoomCtrlAVSource* m_pCurrentEntry;
	DUOTECNO::MFC_HELPER::IReportError* const m_pErrorReport;
	int m_currentIndex;
	int m_currentFunctionIdx;

	DUOTECNO::MFC_HELPER::TPtrList
	<
		DUOTECNO::DATA::CSerRoomCtrlAVSource

	> m_list;

}; /* class CConfigRoomCtrlAVSource */
/*************************************************************/


/*************************************************************/
/***************** CConfigRoomCtrlTunerPreset ****************/
/*************************************************************/
class CConfigRoomCtrlTunerPreset : public CConfigExternalClass
{
public:
	CConfigRoomCtrlTunerPreset(DUOTECNO::MFC_HELPER::IReportError* ptr);
	~CConfigRoomCtrlTunerPreset(void);

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
	DUOTECNO::DATA::CSerRoomCtrlAVTunerPreset* GetEntry(int entry);

private:
	static const char* const _szHeader;
	DUOTECNO::DATA::CSerRoomCtrlAVTunerPreset* m_pCurrentEntry;
	DUOTECNO::MFC_HELPER::IReportError* const m_pErrorReport;
	int m_currentIndex;

	DUOTECNO::MFC_HELPER::TPtrList
	<
		DUOTECNO::DATA::CSerRoomCtrlAVTunerPreset

	> m_list;

}; /* class CConfigRoomCtrlTunerPreset */
/*************************************************************/


/*************************************************************/
/***************** CConfigRoomCtrlAVFunction *****************/
/*************************************************************/
class CConfigRoomCtrlAVFunction : public CConfigExternalClass
{
public:
	CConfigRoomCtrlAVFunction(DUOTECNO::MFC_HELPER::IReportError* ptr);
	~CConfigRoomCtrlAVFunction(void);

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
	DUOTECNO::DATA::CSerRoomCtrlAVFunction* GetEntry(int entry);

private:
	static const char* const _szHeader;
	DUOTECNO::DATA::CSerRoomCtrlAVFunction* m_pCurrentEntry;
	DUOTECNO::MFC_HELPER::IReportError* const m_pErrorReport;
	int m_currentIndex;

	DUOTECNO::MFC_HELPER::TPtrList
	<
		DUOTECNO::DATA::CSerRoomCtrlAVFunction

	> m_list;

}; /* class CConfigRoomCtrlTunerPreset */
/*************************************************************/

/*********************** END OF FILE *************************/