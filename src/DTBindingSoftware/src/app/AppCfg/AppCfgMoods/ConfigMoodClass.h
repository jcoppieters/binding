/**********************************************************//**
 * @file	ConfigMoodClass.h
 * @brief	header bestand voor de CConfigMoodClass klasse.
 *
 * @author	Mitchell Tom
 * @date	20090427
 *
 * @since	0x0506
 *************************************************************/
#ifndef DUOTECNO_CONFIG_UNITCREATECLASS
#define DUOTECNO_CONFIG_UNITCREATECLASS
/*************************************************************/
#include "app\appcfg\appcfgbase\ConfigExternalClass.h"

#include "utils/mfc/IReportError.h"
#include "utils/mfc/TPtrList.h"
/*************************************************************/
namespace DUOTECNO {
namespace DATA {
/*************************************************************/
	enum
	{
		MAX_PACKET_DATA_BUFFER 	= 6
	};

	#pragma pack(1)		/* Oppassen: #pragma pack() toevoegen aan einde */
	struct CSerMessagePacket
	{
		BYTE bRemoteAddress;
		BYTE bAPDUExplicitMessageID;
		BYTE bAPDUHeader;
		BYTE bDestinationUnitNR;
		BYTE bSourceUnitNR;
		BYTE bMessageCode;
		BYTE bLength;
		BYTE pbData[ MAX_PACKET_DATA_BUFFER ];
	};
	#pragma pack()

	#pragma pack(1)		/* Oppassen: #pragma pack() toevoegen aan einde */
	struct CSerMoodHeader
	{
		short m_identifier;
		short m_count;
	};
	#pragma pack()

	#pragma pack(1)		/* Oppassen: #pragma pack() toevoegen aan einde */
	struct CSerMoodEntry
	{
		BYTE 					m_delay;
		CSerMessagePacket		m_packet;	
	};
	#pragma pack()
	
/***************************************************************/
};
};
/***************************************************************/


/*************************************************************/
class CMoodEntry;	// voorwaartse Declaratie
/*************************************************************/

/***********************************************************//**
 * @class	CMoodHeader
 * @brief	Bevat serializable data van een mood.
 *
 * @author	Mitchell Tom
 * @date	20090429
 **************************************************************/
class CMoodHeader
{
protected:
	/**
	 * @brief	Serializable data
	 */
	DUOTECNO::DATA::CSerMoodHeader	m_header;

	/**
	 * @brief	Lijst van moodEntries.
	 */
	DUOTECNO::MFC_HELPER::TPtrList<CMoodEntry> m_list;
	//CPtrList m_list;		

protected:
	void removeAll( );

public:
	CMoodHeader( );
	~CMoodHeader( );

	void init( );

	void setIdentifier(short identifier) {
		m_header.m_identifier = identifier;
	}
	int getIdentifier() const {
		return m_header.m_identifier;
	}
	short getCount( ) const {
		return m_header.m_count;
	}

	void add(CMoodEntry* pEntry);
	CMoodEntry* get(int index);

	void dump( );
};
/**************************************************************/

/***********************************************************//**
 * @class	CMoodEntry
 * @brief	Bevat serializable data van een moodEntry.
 *
 * @author	Mitchell Tom
 * @date	20090429
 **************************************************************/
class CMoodEntry
{
protected:
	/** 
	 * @brief	Serializable data
	 */
	DUOTECNO::DATA::CSerMoodEntry	m_entry;

protected:
	void removeAll( );

public:
	CMoodEntry( );
	~CMoodEntry( );

	void init( );
	bool parse(const char* szData);

	void dump( );
};
/**************************************************************/

/***********************************************************//**
 * @class	CConfigMoodClass
 * @brief	Bevat serializable data van de sferen.
 *
 *			Deze klasse wordt gebruikt om de configuratie file
 *			in te lezen. De data wordt in deze klasse opgeslaan.
 *
 * @author	Mitchell Tom
 * @date	20090429
 **************************************************************/
class CConfigMoodClass : public CConfigExternalClass
{
protected:
	/**
	 * @brief	presentatie in bestand	= "MOOD"
	 */
	static const char* _szHeader;

	/**
	 * @brief	referentie naar error display
	 */
	DUOTECNO::MFC_HELPER::IReportError* const m_pErrorReport;

	/**
	 * @brief	Referentie naar de huidige ingelezen moodheader
	 */
	CMoodHeader* m_pCurrent;

	/**
	 * @brief	De huidige index
	 */
	int m_currentIndex;

private:
	/**
	 * @brief	Een lijst van moodHeaders
	 */
	//CPtrList m_list;
	DUOTECNO::MFC_HELPER::TPtrList<CMoodHeader> m_list;

public:
	CConfigMoodClass(DUOTECNO::MFC_HELPER::IReportError *ptr);
	virtual ~CConfigMoodClass();

	CMoodHeader* getMoodHeader(int index);

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
 * @class	CConfigMoodHeaderClass
 * @brief	Deze klasse wordt gebruikt door de FSM.
 * 
 * @note	deze klasse wordt niet gebruikt om de config file
 *			in te lezen.
 *
 * @author	Mitchell Tom
 * @date	20090429
 **************************************************************/
class CConfigMoodHeaderClass : public CConfigExternalClass
{
protected:
	/**
	 * @brief	Referentie naar de moodheader
	 */
	CMoodHeader* m_pMoodHeader;

public:
	CConfigMoodHeaderClass( );
	~CConfigMoodHeaderClass( );

	void set(CMoodHeader* pHeader);

public:
	virtual BOOL ParseHeader(char* /*szAttribute*/,char* /*szData*/ )	// CHANGES_MVS_2008_WARNINGS
	{
		return FALSE;
	}
	virtual BOOL ParseData(char* /*szAttribute*/,char* /*szData*/ )		// CHANGES_MVS_2008_WARNINGS
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

/***********************************************************//**
 * @class	CConfigMoodEntryClass
 * @brief	Deze klasse wordt gebruikt door de FSM.
 * 
 * @note	deze klasse wordt niet gebruikt om de config file
 *			in te lezen.
 *
 * @author	Mitchell Tom
 * @date	20090429
 ***************************************************************/

class CConfigMoodEntryClass : public CConfigExternalClass
{
protected:
	/**
	 * @brief	Referentie naar de moodheader.
	 *
	 *			Via de moodheader wordt de data van de moodentries
	 *			opgehaald.
	 */
	CMoodHeader* m_pMoodHeader;

public:
	CConfigMoodEntryClass( );
	~CConfigMoodEntryClass( );

	void set(CMoodHeader* pHeader);

public:
	virtual BOOL ParseHeader(char* /*szAttribute*/,char* /*szData*/)	// CHANGES_MVS_2008_WARNINGS
	{
		return FALSE;
	}
	virtual BOOL ParseData(char* /*szAttribute*/,char* /*szData*/)		// CHANGES_MVS_2008_WARNINGS
	{
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

/*************************************************************/
#endif
/*************************************************************/
