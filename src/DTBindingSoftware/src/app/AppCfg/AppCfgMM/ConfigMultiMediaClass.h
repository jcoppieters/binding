// ConfigMultiMediaClass.h: interface for the CConfigMultiMediaClass class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONFIGMULTIMEDIACLASS_H__D10A5D3D_2D7F_41F4_8C94_AEC4A17F4C7F__INCLUDED_)
#define AFX_CONFIGMULTIMEDIACLASS_H__D10A5D3D_2D7F_41F4_8C94_AEC4A17F4C7F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

///////////////////////////////////////////////////////////////////////////
#include "ConfigMultiMediaStructs.h"
#include "app\appcfg\appcfgbase\ConfigExternalClass.h"
#include "utils/mfc/IReportError.h"
///////////////////////////////////////////////////////////////////////////

class CConfigMultiMediaClass : public CConfigExternalClass
{
protected:	
	BYTE bCurrentIndex;	
	DUOTECNO::MFC_HELPER::IReportError* const m_pErrorReport;

private:
	const int m_nMaxStructures;
	const char* const m_szHeaderID;

protected:
	void ReportError(CString s);
	unsigned short SwapUnsignedShort(unsigned short usValue);	
	virtual BOOL AssignPointer(BYTE bIndex) = 0;
	inline int GetMaxStructures( ) const { return m_nMaxStructures; }
	inline const char* GetHeaderID( ) const { return m_szHeaderID; };

public:
	CConfigMultiMediaClass(
		DUOTECNO::MFC_HELPER::IReportError *ptr, 
		const char* szHeaderID,
		int MaxStructures);
	virtual ~CConfigMultiMediaClass();
	virtual void dump( ); 
	virtual int GetMax(void) const { return m_nMaxStructures; }
};


////////////////////////////////////////////////////////////////////////////
class CConfigAudioRoom : public CConfigMultiMediaClass
{
protected:
	UNIT_AUDIOROOM_STRUCT AudioRoom[NR_OF_ROOM_UNITS];
	UNIT_AUDIOROOM_STRUCT *pCurrentRoom;
	BYTE bAudioRoomUsed[NR_OF_ROOM_UNITS];

protected:
	BOOL AssignPointer(BYTE bIndex);

public:
	CConfigAudioRoom(DUOTECNO::MFC_HELPER::IReportError *ptr);
	virtual ~CConfigAudioRoom();
public:
	virtual BOOL ParseHeader(char *szAttribute,char *szData);
	virtual BOOL ParseData(char *szAttribute,char *szData);
	virtual void Initialise(void);

	virtual int GetData(int index,BYTE *pbData);
	virtual BOOL GetData(int index,void **pbData);
	virtual BOOL IsUsed(int index);
};

////////////////////////////////////////////////////////////////////////////
class CConfigAudioDest_Base : public CConfigMultiMediaClass
{
protected:
	STRUCT_MATRIX_AUDIO_UNIT_DESTINATION* m_pMatrixDest;
	STRUCT_MATRIX_AUDIO_UNIT_DESTINATION* m_pCurrentDest;
	BYTE* m_pbDestinationUsed;

protected:
	virtual BOOL AssignPointer(BYTE bIndex);

public:
	CConfigAudioDest_Base(DUOTECNO::MFC_HELPER::IReportError *ptr,int maxStructures);
	virtual ~CConfigAudioDest_Base();
public:
	virtual BOOL ParseHeader(char *szAttribute,char *szData);
	virtual BOOL ParseData(char *szAttribute,char *szData);
	virtual void Initialise(void);

	virtual int GetData(int index,BYTE *pbData);
	virtual BOOL GetData(int index,void **pbData);
	virtual BOOL IsUsed(int index);
};


class CConfigAudioDest : public CConfigAudioDest_Base
{
public:
	CConfigAudioDest(DUOTECNO::MFC_HELPER::IReportError *ptr);
	virtual ~CConfigAudioDest( );
};

class CConfigAudioDest_V65 : public CConfigAudioDest_Base
{
public:
	CConfigAudioDest_V65(DUOTECNO::MFC_HELPER::IReportError *ptr);
	virtual ~CConfigAudioDest_V65( );
};

class CConfigAudioDest_V66 : public CConfigAudioDest_Base
{
public:
	CConfigAudioDest_V66(DUOTECNO::MFC_HELPER::IReportError *ptr);
	virtual ~CConfigAudioDest_V66( );
};

////////////////////////////////////////////////////////////////////////////
class CConfigAudioSource : public CConfigMultiMediaClass
{
protected:	
	STRUCT_MATRIX_AUDIO_UNIT_SOURCE* m_pMatrixSrc;
	STRUCT_MATRIX_AUDIO_UNIT_SOURCE* m_pCurrentSource;
	BYTE* m_pbSourceUsed;

protected:
	virtual BOOL AssignPointer(BYTE bIndex);
	int ParseExtraIR(char *szAttribute);

public:
	CConfigAudioSource(DUOTECNO::MFC_HELPER::IReportError *ptr, int maxStructures);
	virtual ~CConfigAudioSource();
public:
	virtual BOOL ParseHeader(char *szAttribute,char *szData);
	virtual BOOL ParseData(char *szAttribute,char *szData);
	virtual void Initialise(void);

	virtual int GetData(int index,BYTE *pbData);
	virtual BOOL GetData(int index,void **pbData);
	virtual BOOL IsUsed(int index);
};

/////////////////////////////////////////////////////////////////
// CConfigAudioSource_V64
class CConfigAudioSource_V64 : public CConfigAudioSource
{
public:
	CConfigAudioSource_V64(DUOTECNO::MFC_HELPER::IReportError *ptr);
	virtual ~CConfigAudioSource_V64();
};

/////////////////////////////////////////////////////////////////
// CConfigAudioSource_V65
class CConfigAudioSource_V65 : public CConfigAudioSource
{
public:
	CConfigAudioSource_V65(DUOTECNO::MFC_HELPER::IReportError *ptr);
	virtual ~CConfigAudioSource_V65();
};

/////////////////////////////////////////////////////////////////
// CConfigAudioSource_V66
class CConfigAudioSource_V66 : public CConfigAudioSource
{
public:
	CConfigAudioSource_V66(DUOTECNO::MFC_HELPER::IReportError *ptr);
	virtual ~CConfigAudioSource_V66();
};



////////////////////////////////////////////////////////////////////////////
class CConfigVideoMuxSource : public CConfigMultiMediaClass
{
protected:
	STRUCT_VIDEOMUXSOURCE VideoMuxSrc[NR_OF_VIDEOMUXSOURCES];
	STRUCT_VIDEOMUXSOURCE *pCurrentVideoMuxSrc;
	BYTE bVideoMuxSrcUsed[NR_OF_VIDEOMUXSOURCES];

protected:
	virtual BOOL AssignPointer(BYTE bIndex);

public:
	CConfigVideoMuxSource(DUOTECNO::MFC_HELPER::IReportError *ptr);
	virtual ~CConfigVideoMuxSource();
public:
	virtual BOOL ParseHeader(char *szAttribute,char *szData);
	virtual BOOL ParseData(char *szAttribute,char *szData);
	virtual void Initialise(void);

	virtual int GetData(int index,BYTE *pbData);
	virtual BOOL GetData(int index,void **pbData);
	virtual BOOL IsUsed(int index);
};


////////////////////////////////////////////////////////////////////////////
class CConfigVideoMuxDest : public CConfigMultiMediaClass
{
protected:
	STRUCT_VIDEOMUXDESTINATION VideoMuxDest[NR_OF_VIDEOMUXDESTINATIONS];
	STRUCT_VIDEOMUXDESTINATION *pCurrentVideoMuxDest;
	BYTE bVideoMuxDestUsed[NR_OF_VIDEOMUXDESTINATIONS];

protected:
	virtual BOOL AssignPointer(BYTE bIndex);

public:
	CConfigVideoMuxDest(DUOTECNO::MFC_HELPER::IReportError *ptr);
	virtual ~CConfigVideoMuxDest();
public:
	virtual BOOL ParseHeader(char *szAttribute,char *szData);
	virtual BOOL ParseData(char *szAttribute,char *szData);
	virtual void Initialise(void);

	virtual int GetData(int index,BYTE *pbData);
	virtual BOOL GetData(int index,void **pbData);
	virtual BOOL IsUsed(int index);
};


////////////////////////////////////////////////////////////////////////////
class CConfigSpeechDevIndoor : public CConfigMultiMediaClass
{
protected:
	STRUCT_SPEECHDEVINDOOR SpeechDevIndoor[NR_OF_SPEECHDEVINDOORS];
	STRUCT_SPEECHDEVINDOOR *pCurrentSpeechDevIndoor;
    BYTE bSpeechDevIndoorUsed[NR_OF_SPEECHDEVINDOORS];

protected:
	virtual BOOL AssignPointer(BYTE bIndex);

public:
	CConfigSpeechDevIndoor(DUOTECNO::MFC_HELPER::IReportError *ptr);
	virtual ~CConfigSpeechDevIndoor();
public:
	virtual BOOL ParseHeader(char *szAttribute,char *szData);
	virtual BOOL ParseData(char *szAttribute,char *szData);
	virtual void Initialise(void);

	virtual int GetData(int index,BYTE *pbData);
	virtual BOOL GetData(int index,void **pbData);
	virtual BOOL IsUsed(int index);
};


////////////////////////////////////////////////////////////////////////////

// 20181003: Uitbreiding voor nieuwe touchscreen 2018

class CConfigSpeechDevOutdoor_Base : public CConfigMultiMediaClass
{
protected:	
	STRUCT_SPEECHDEVOUTDOOR* m_pSpeechDevOutdoor;
	STRUCT_SPEECHDEVOUTDOOR* m_pCurrentSpeechDevOutdoor;
	BYTE* m_pbSpeechDevOutdoorUsed;

protected:
	virtual BOOL AssignPointer(BYTE bIndex);

public:
	CConfigSpeechDevOutdoor_Base(DUOTECNO::MFC_HELPER::IReportError *ptr,int maxStructures);
	virtual ~CConfigSpeechDevOutdoor_Base();

public:
	virtual BOOL ParseHeader(char *szAttribute,char *szData);
	virtual BOOL ParseData(char *szAttribute,char *szData);
	virtual void Initialise(void);

	virtual int GetData(int index,BYTE *pbData);
	virtual BOOL GetData(int index,void **pbData);
	virtual BOOL IsUsed(int index);
};

class CConfigSpeechDevOutdoor : public CConfigSpeechDevOutdoor_Base
{
public:
	CConfigSpeechDevOutdoor(DUOTECNO::MFC_HELPER::IReportError *ptr);
	virtual ~CConfigSpeechDevOutdoor();

private:
	typedef CConfigSpeechDevOutdoor_Base Base;
};

// TM,V14.09
class CConfigSpeechDevOutdoor_V65 : public CConfigSpeechDevOutdoor_Base
{
public:
	CConfigSpeechDevOutdoor_V65(DUOTECNO::MFC_HELPER::IReportError *ptr);
	virtual ~CConfigSpeechDevOutdoor_V65();

private:
	typedef CConfigSpeechDevOutdoor_Base Base;
};

class CConfigSpeechDevOutdoor_V66 : public CConfigSpeechDevOutdoor_Base
{
public:
	CConfigSpeechDevOutdoor_V66(DUOTECNO::MFC_HELPER::IReportError *ptr);
	virtual ~CConfigSpeechDevOutdoor_V66();

	virtual BOOL ParseData(char *szAttribute,char *szData);

private:
	typedef CConfigSpeechDevOutdoor_Base Base;
};

////////////////////////////////////////////////////////////////////////////

class CConfigAlarmZoneNameUtil
{
public:
	enum TypeAlarmCentrale
	{
		TYPE_ALARM_GALAXY		= 0,
		TYPE_ALARM_ARITECH		= 1,
		TYPE_ALARM_ANB			= 2,		/* TM_CHANGES_ALARM_ANB */
		TYPE_ALARM_TEXECOM		= 3,		/* TM_CHANGES_ALARM_TEXECOM */
		TYPE_ALARM_INTEGRA		= 4			/* TM_CHANGES_ALARM_INTEGRA */
	};

	static BOOL GetAlarmZoneName(
		TypeAlarmCentrale type,
		int zoneNR,
		size_t maxLength,
		char* szName);

private:
	static const char AlarmZoneNr[GALAXYCENTRALEMAXALARMZONES][5];
};


////////////////////////////////////////////////////////////////////////////

class CConfigAlarmZone : public CConfigMultiMediaClass
{
protected:
	STRUCT_ALARMZONE_CONFIG AlarmConfigZone[GALAXYCENTRALEMAXALARMZONES];
	STRUCT_ALARMZONE_CONFIG *pCurrentAlarmZone;
	BYTE bAlarmConfigZoneUsed[GALAXYCENTRALEMAXALARMZONES];

protected:
	virtual BOOL AssignPointer(BYTE bIndex);

public:
	CConfigAlarmZone(DUOTECNO::MFC_HELPER::IReportError *ptr);
	virtual ~CConfigAlarmZone();

	virtual BOOL ParseHeader(char *szAttribute,char *szData);
	virtual BOOL ParseData(char *szAttribute,char *szData);
	virtual void Initialise(void);
	virtual int GetData(int index,BYTE *pbData);
	virtual BOOL GetData(int index,void **pbData);
	virtual BOOL IsUsed(int index);
};


////////////////////////////////////////////////////////////////////////////
class CConfigAVMatrix : public CConfigMultiMediaClass
{
protected:
	/* TM_MULTIPLE_AVMATRIX */
	STRUCT_CONFIGURATION_AVMATRIX ConfigAVMatrix[NR_OF_AVMATRIX_SYSTEMS];		/* TM_MULTIPLE_AVMATRIX */
	STRUCT_CONFIGURATION_AVMATRIX *pCurrentAVMatrix;							/* TM_MULTIPLE_AVMATRIX */

protected:
	virtual BOOL AssignPointer(BYTE bIndex);

public:
	CConfigAVMatrix(DUOTECNO::MFC_HELPER::IReportError *ptr);
	virtual ~CConfigAVMatrix();
public:
	virtual BOOL ParseHeader(char *szAttribute,char *szData);
	virtual BOOL ParseData(char *szAttribute,char *szData);
	virtual void Initialise(void);
	virtual int GetData(int index,BYTE *pbData);
	virtual BOOL GetData(int index,void **pbData);
	virtual BOOL IsUsed(int index);
};


////////////////////////////////////////////////////////////////////////////
class CConfigVideomux : public CConfigMultiMediaClass
{
protected:
 
	STRUCT_CONFIGURATION_VIDEOMUX ConfigVideoMux;
	STRUCT_CONFIGURATION_VIDEOMUX *pCurrentVideoMux;

protected:
	virtual BOOL AssignPointer(BYTE bIndex);

public:
	CConfigVideomux(DUOTECNO::MFC_HELPER::IReportError *ptr);
	virtual ~CConfigVideomux();
public:
	virtual BOOL ParseHeader(char *szAttribute,char *szData);
	virtual BOOL ParseData(char *szAttribute,char *szData);
	virtual void Initialise(void);

	virtual int GetData(int index,BYTE *pbData);
	virtual BOOL GetData(int index,void **pbData);
	virtual BOOL IsUsed(int index);
};



////////////////////////////////////////////////////////////////////////////
class CConfigVideophone : public CConfigMultiMediaClass
{
protected:
 
	STRUCT_VIDEOPHONE_CONFIG ConfigVideoPhone;
	STRUCT_VIDEOPHONE_CONFIG *pCurrentVideophone;

protected:
	virtual BOOL AssignPointer(BYTE bIndex);

public:
	CConfigVideophone(DUOTECNO::MFC_HELPER::IReportError *ptr);
	virtual ~CConfigVideophone();
public:
	virtual BOOL ParseHeader(char *szAttribute,char *szData);
	virtual BOOL ParseData(char *szAttribute,char *szData);
	virtual void Initialise(void);

	virtual int GetData(int index,BYTE *pbData);
	virtual BOOL GetData(int index,void **pbData);
	virtual BOOL IsUsed(int index);
};


#endif // !defined(AFX_CONFIGMULTIMEDIACLASS_H__D10A5D3D_2D7F_41F4_8C94_AEC4A17F4C7F__INCLUDED_)
