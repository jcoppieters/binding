#pragma once

#include "app\appcfg\appcfgbase\configexternalclass.h"
#include "utils/mfc/IReportError.h"
#include "utils/mfc/TPtrList.h"

/***************************************************/
namespace DUOTECNO {
namespace DATA {
/***************************************************/

	enum
	{
		SMAP_VROOM_MAX_NAME_LENGTH 		= 16,	/* Max lengte van de kamer naam */

		SMAP_NODEADDRESS_IDX            = 0,	/* Index m_bAudioRoomID array */
		SMAP_UNITADDRESS_IDX            = 1,	/* Index m_bAudioRoomID array */
		SMAP_NODEUNITADDRESS_SIZE		= 2,	/* Max elementen m_bAudioRoomID array */
	};

	enum
	{		
		SMAP_AUDIOROOM_MAX_DATA_ITEMS   = 4,	/* Moet dit hier ? */
		SMAP_AUDIOROOM_ASSIGNMENT_NONE  = 0xFF,		
	};


	/**
	 * Virtuele kamer Header.
	 */
	#pragma pack(1)				/* Oppassen: #pragma pack() toevoegen aan einde */
	struct SerSmapVRoomHdr
	{
		BYTE m_bIndex;			// Index van de kamer - referentie in de smap	
		char m_szName[SMAP_VROOM_MAX_NAME_LENGTH];
		BYTE m_bTypeItemData;	// reserve - later		
	};
	#pragma pack()


	/**
	 * Virtuele unit measurement (kamer)
	 */
	#pragma pack(1)
	struct SerSmapUnitMeasureItemData {
		uint8_t m_bRoomID[SMAP_NODEUNITADDRESS_SIZE];		// address - reserve.
		uint8_t m_bType;									// reserve.
		char m_strMeasureUnit[8];							// kwH, m3 , L.
		char m_strReserved[8];								// Reserve
		uint16_t m_usRegisterAddress;						// 16bit waarde.
		uint8_t m_bRefIcon;									// referentie naar een icoon: begint bij 0
		uint8_t m_bDivider;									// standaard 1
		uint8_t m_bMultiplier;								// standaard 1
		uint8_t m_bDecimals;								// Aantal cijders na de comma ???
		uint8_t m_bReserved[8];								// Aantal reserve bytes.
	};
	#pragma pack()

	#pragma pack(1)
	struct SerSmapUnitMeasureItem {
		struct SerSmapVRoomHdr hdr;
		struct SerSmapUnitMeasureItemData data;
	};
	#pragma pack()

	/**
	 * Virtuele temperatuur kamer
	 */
	#pragma pack(1)
	struct SerSmapTempRoomData
	{
		BYTE m_bRoomID[SMAP_NODEUNITADDRESS_SIZE];	
		BYTE m_bType;	/* bepaalt type kamer */
	};
	#pragma pack()

	#pragma pack(1)
	struct SerSmapTempRoom
	{
		struct SerSmapVRoomHdr hdr;
		struct SerSmapTempRoomData data;
	};
	#pragma pack()
	
	/**
	 * Virtuele AV kamer
	 */
	#pragma pack(1)				/* Oppassen: #pragma pack() toevoegen aan einde */
	struct SerSmapAVRoomData
	{
		BYTE m_bAudioRoomID[SMAP_NODEUNITADDRESS_SIZE]; // address van de audio room.
		BYTE m_bAudioRoomDest;	// destination index van de audio room.
		BYTE m_bAudioRoomDestLink;	// Link nr een andere dest in deze config.
	};
	#pragma pack()

	#pragma pack(1)	/* Oppassen: #pragma pack() toevoegen aan einde */
	struct SerSmapAVRoom
	{
		struct SerSmapVRoomHdr hdr;
		struct SerSmapAVRoomData data[SMAP_AUDIOROOM_MAX_DATA_ITEMS];
	};
	#pragma pack()

/***************************************************/
};
};
/***************************************************/

enum
{
	CLASSID_SMAP_AV_ROOM = 0x00,
	CLASSID_SMAP_TEMP_ROOM = 0x01,
	CLASSID_VIRTUALUNIT_MEASURE_ITEM = 0x02		// zie SMAP_CLASSID_VIRTUALUNIT_MEASURE_ITEM 
};
/***************************************************/

/**
 *
 */
class CSerSmapVRoom
{
public:
	CSerSmapVRoom( );
	virtual ~CSerSmapVRoom( )					= 0;

	// Getters & setters
	virtual int getClassID( ) const				= 0;
	virtual void setIndex(BYTE bIndex)			= 0;
	virtual void setName(const char* szName)	= 0;	
	virtual BYTE getIndex( ) const				= 0;

	// Functions
	virtual int getData(BYTE* pbData)			= 0;
	virtual void dump( )						= 0;
	virtual BOOL save(FILE* fs)					= 0;
};
/****************************************************/

/**
 *
 */
class CSerSmapAVRoom : public CSerSmapVRoom
{
private:
	enum { ID = CLASSID_SMAP_AV_ROOM };

public:
	CSerSmapAVRoom( );
	virtual ~CSerSmapAVRoom( );

	virtual int getClassID( ) const;		/* m_bFunction bevat classID */
	virtual void setIndex(BYTE bIndex);	
	virtual void setName(const char* szName);
	virtual BYTE getIndex( ) const;	

	/* Getter en Setter */		
	const char* getName( ) const;
	
/*
	BYTE getFunction( ) const;
	void setFunction(BYTE func);	
*/
	BYTE getAudioRoomNodeAddress(int idx) const;
	void setAudioRoomNodeAddress(int idx,BYTE bNodeAddress);
	BYTE getAudioRoomUnitAddress(int idx) const;
	void setAudioRoomUnitAddress(int idx,BYTE bUnitAddress);
	BYTE getAudioRoomDest(int idx) const;
	void setAudioRoomDest(int idx,BYTE bDest);
	BYTE getAudioRoomDestLink(int idx) const;
	void setAudioRoomDestLink(int idx,BYTE bDest);

	/* Functions */
	virtual int getData(BYTE* pbData);
	virtual void dump( );
	virtual BOOL save(FILE* fs);

	/* Speciale functies */
	static CSerSmapAVRoom* doCast(CSerSmapVRoom* p);
	static const CSerSmapAVRoom* doCast( const CSerSmapVRoom* p);

protected:
	void init( );
	BOOL getString(char* pbData,int maxLength,int* length);

private:
	DUOTECNO::DATA::SerSmapAVRoom m_cfg;
};
/***************************************************/

/**
 *
 */
class CSerSmapTempRoom : public CSerSmapVRoom
{
private:
	enum { ID = CLASSID_SMAP_TEMP_ROOM };

public:
	CSerSmapTempRoom( );
	virtual ~CSerSmapTempRoom( );

	virtual int getClassID( ) const;		/* m_bFunction bevat classID */
	virtual void setIndex(BYTE bIndex);	
	virtual void setName(const char* szName);
	virtual BYTE getIndex( ) const;	

	/* Getter en Setter */		
	const char* getName( ) const;
	
	/* Data */
	BYTE getNodeAddress() const;
	void setNodeAddress(BYTE bNodeAddress);
	BYTE getUnitAddress() const;
	void setUnitAddress(BYTE bUnitAddress);
	BYTE getType() const;
	void setType(BYTE bType);
	
	/* Functions */
	virtual int getData(BYTE* pbData);
	virtual void dump( );
	virtual BOOL save(FILE* fs);

	/* Speciale functies */
	static CSerSmapTempRoom* doCast(CSerSmapVRoom* p);
	static const CSerSmapTempRoom* doCast( const CSerSmapVRoom* p);

protected:
	void init( );
	BOOL getString(char* pbData,int maxLength,int* length);

private:
	DUOTECNO::DATA::SerSmapTempRoom m_cfg;
};
/***************************************************/

/**
 * Sedert V16.12
 */
class CSerSmapSerSmapUnitMeasureItem : public CSerSmapVRoom
{
private:
	enum { ID = CLASSID_VIRTUALUNIT_MEASURE_ITEM };

public:
	CSerSmapSerSmapUnitMeasureItem( );
	virtual ~CSerSmapSerSmapUnitMeasureItem( );

	virtual int getClassID( ) const;		/* m_bFunction bevat classID */
	virtual void setIndex(BYTE bIndex);	
	virtual void setName(const char* szName);
	virtual BYTE getIndex( ) const;	

	/* Getter en Setter */		
	const char* getName( ) const;
	
	/* Data */
	BYTE getNodeAddress() const;
	void setNodeAddress(BYTE bNodeAddress);
	BYTE getUnitAddress() const;
	void setUnitAddress(BYTE bUnitAddress);
	BYTE getType() const;
	void setType(BYTE bType);

	// Getters & setters...
	int getRegisterAddress() const;
	void setRegisterAddress(int address);
	int getRefIcon() const;
	void setRefIcon(int refIcon);
	const char* getStrMeasurementUnit( ) const;
	void setStrMeasurementUnit(const char* s);
	int getDivider() const;			// rseerve:
	void setDivider(int v);			// reserve.
	int getMultiplier() const;		// rseerve: 
	void setMultiplier(int v);		// reserve.
	int getNrDecimals() const;		// reserve: Aantal cijfers na de komma na het delen.
	void setNrDecimals(int d);		// reserve.
	
	/* Functions */
	virtual int getData(BYTE* pbData);
	virtual void dump( );
	virtual BOOL save(FILE* fs);

	/* Speciale functies */
	static CSerSmapSerSmapUnitMeasureItem* doCast(CSerSmapVRoom* p);
	static const CSerSmapSerSmapUnitMeasureItem* doCast( const CSerSmapVRoom* p);

protected:
	void init( );
	BOOL getString(char* pbData,int maxLength,int* length);

private:
	DUOTECNO::DATA::SerSmapUnitMeasureItem m_cfg;
};
/***************************************************/

class CConfigSmapVRoomClass : public CConfigExternalClass
{
public:
	CConfigSmapVRoomClass(DUOTECNO::MFC_HELPER::IReportError *ptr);
	virtual ~CConfigSmapVRoomClass(void);	

	CSerSmapVRoom* getEntry(int index);
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
	BOOL helper_addEntry(int index,CSerSmapVRoom* p);	// Add entry - deallocate when the function fails.
	BOOL addEntry(CSerSmapVRoom* p);

	BOOL parseData_headerItem(char *szAttribute,char *szData);
	BOOL ParseData_itemAvroom(char *szAttribute,char *szData);
	BOOL ParseData_itemSensroom(char *szAttribute,char *szData);
	BOOL ParseData_itemMeasureItem(char *szAttribute,char *szData);


private:
	/**
	 * @brief	presentatie in bestand	= "SMAPAUDIOROOM"
	 */
	static const char* const _szHeaderAVRoom;
	static const char* const _szHeaderTempRoom;
	static const char* const _szHeaderMeasureUnitItem;

	/**
	 * @brief	referentie naar error display
	 */
	DUOTECNO::MFC_HELPER::IReportError* const m_pErrorReport;

	
	/**
	 * @brief	Een lijst van moodHeaders
	 */
	DUOTECNO::MFC_HELPER::TPtrList<CSerSmapVRoom> m_list;

	/**
	 * @brief	De huidige 
	 */
	CSerSmapVRoom* m_pCurrent;

	/**
	 * @brief	Proxy om bij te houden of een index gebruikt werd.
	 */
	enum { MAX_INDEX = 255 };
	BYTE m_bUsed[MAX_INDEX];

	/**
	 * @brief	Toestand van de parser.
	 */
	enum State
	{
		STATE_IDLE = 0,
		STATE_HEADER = 1,
		STATE_AVROOM_ITEM = 2,
		STATE_TEMP_ITEM = 3,
		STATE_UNITMEASURE_ITEM = 4
	} m_CurrentState;

	/**
	 * @brief	Index van de destination.
	 */
	BYTE m_bCurrentDest;
};