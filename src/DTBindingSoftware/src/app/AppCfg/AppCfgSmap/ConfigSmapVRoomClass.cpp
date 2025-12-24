#include "stdafx.h"
#include "ConfigSmapVRoomClass.h"
/****************************************************/
CSerSmapVRoom::CSerSmapVRoom( ) { }
CSerSmapVRoom::~CSerSmapVRoom( ) {
	// Pure virtuele functie.
}
/****************************************************/

/*****************************************************/
CSerSmapAVRoom::CSerSmapAVRoom(void) :
m_cfg( )
{
	init( );
}

CSerSmapAVRoom::~CSerSmapAVRoom(void)
{
}

int CSerSmapAVRoom::getClassID( ) const
{
	return m_cfg.hdr.m_bTypeItemData;
}

void CSerSmapAVRoom::setIndex(BYTE bIndex)
{
	m_cfg.hdr.m_bIndex = bIndex;
}
BYTE CSerSmapAVRoom::getIndex( ) const
{
	return m_cfg.hdr.m_bIndex;
}

const char* CSerSmapAVRoom::getName( ) const
{
	return m_cfg.hdr.m_szName;
}
void CSerSmapAVRoom::setName(const char* szName)
{
	using namespace DUOTECNO::DATA;

	if (szName)
	{
		memcpy(&m_cfg.hdr.m_szName[0],
   	  			szName,
				SMAP_VROOM_MAX_NAME_LENGTH - 1);

   		m_cfg.hdr.m_szName[SMAP_VROOM_MAX_NAME_LENGTH - 1] = '\0';
	}
}
/*
BYTE CSerSmapAVRoom::getFunction( ) const
{
	return m_cfg.hdr.m_bFunction;
}
void CSerSmapAVRoom::setFunction(BYTE func)
{
	m_cfg.hdr.m_bFunction = func;	
}
*/

BYTE CSerSmapAVRoom::getAudioRoomNodeAddress(int idx) const
{
	REQUIRE(idx < DUOTECNO::DATA::SMAP_AUDIOROOM_MAX_DATA_ITEMS);
	return m_cfg.data[idx].m_bAudioRoomID[DUOTECNO::DATA::SMAP_NODEADDRESS_IDX];
}
void CSerSmapAVRoom::setAudioRoomNodeAddress(int idx,BYTE bNodeAddress)
{
	REQUIRE(idx < DUOTECNO::DATA::SMAP_AUDIOROOM_MAX_DATA_ITEMS);
	m_cfg.data[idx].m_bAudioRoomID[DUOTECNO::DATA::SMAP_NODEADDRESS_IDX] = bNodeAddress;
}
BYTE CSerSmapAVRoom::getAudioRoomUnitAddress(int idx) const
{
	REQUIRE(idx < DUOTECNO::DATA::SMAP_AUDIOROOM_MAX_DATA_ITEMS);
	return m_cfg.data[idx].m_bAudioRoomID[DUOTECNO::DATA::SMAP_UNITADDRESS_IDX];
}
void CSerSmapAVRoom::setAudioRoomUnitAddress(int idx,BYTE bUnitAddress)
{
	REQUIRE(idx < DUOTECNO::DATA::SMAP_AUDIOROOM_MAX_DATA_ITEMS);
	m_cfg.data[idx].m_bAudioRoomID[DUOTECNO::DATA::SMAP_UNITADDRESS_IDX] = bUnitAddress;
}
BYTE CSerSmapAVRoom::getAudioRoomDest(int idx) const
{
	REQUIRE(idx < DUOTECNO::DATA::SMAP_AUDIOROOM_MAX_DATA_ITEMS);
	return m_cfg.data[idx].m_bAudioRoomDest;
}
void CSerSmapAVRoom::setAudioRoomDest(int idx,BYTE bDest)
{
	REQUIRE(idx < DUOTECNO::DATA::SMAP_AUDIOROOM_MAX_DATA_ITEMS);
	m_cfg.data[idx].m_bAudioRoomDest = bDest;
}
BYTE CSerSmapAVRoom::getAudioRoomDestLink(int idx) const
{
	REQUIRE(idx < DUOTECNO::DATA::SMAP_AUDIOROOM_MAX_DATA_ITEMS);
	return m_cfg.data[idx].m_bAudioRoomDestLink;
}
void CSerSmapAVRoom::setAudioRoomDestLink(int idx,BYTE bDest)
{
	REQUIRE(idx < DUOTECNO::DATA::SMAP_AUDIOROOM_MAX_DATA_ITEMS);
	m_cfg.data[idx].m_bAudioRoomDestLink = bDest;
}

/*********************** public **********************/
int CSerSmapAVRoom::getData(BYTE* pbData)
{
	using DUOTECNO::DATA::SerSmapAVRoom;

	if (pbData)
	{
		memcpy(pbData,&this->m_cfg,sizeof(SerSmapAVRoom));
		return sizeof(SerSmapAVRoom);
	}

	return 0;
}
void CSerSmapAVRoom::dump( )
{
	char szBuffer[1024];
	int length;

	if (getString(szBuffer, 255, &length))
	{
		TRACE( szBuffer );
	}
}
BOOL CSerSmapAVRoom::save(FILE* fs)
{	
	if (fs)
	{
		int length;
		char szBuffer[1024];

		if (getString(szBuffer, 1024, &length))
		{
			fputs(szBuffer,fs);		// CHANGES_0X0A01_USE_FPUTS
			return TRUE;
		}	
	}
	return FALSE;
}
/************************ protected *****************/
void CSerSmapAVRoom::init( )
{
	using namespace DUOTECNO::DATA;

	this->m_cfg.hdr.m_bIndex = SMAP_AUDIOROOM_ASSIGNMENT_NONE;	
	memset(&this->m_cfg.hdr.m_szName[0],
		   '\0',
		   SMAP_VROOM_MAX_NAME_LENGTH);

/*
    this->m_cfg.hdr.m_bFunction = SMAP_AUDIOROOM_ASSIGNMENT_NONE;
*/
	// Oppassen: Dit wordt nu enum waarde maar moet ook nog de 0xFF ondersteunen.
	this->m_cfg.hdr.m_bTypeItemData = CSerSmapAVRoom::ID;

	for (int i=0;i<SMAP_AUDIOROOM_MAX_DATA_ITEMS;i++)
	{
		this->m_cfg.data[i].m_bAudioRoomID[SMAP_NODEADDRESS_IDX] = SMAP_AUDIOROOM_ASSIGNMENT_NONE;
		this->m_cfg.data[i].m_bAudioRoomID[SMAP_UNITADDRESS_IDX] = SMAP_AUDIOROOM_ASSIGNMENT_NONE;
		this->m_cfg.data[i].m_bAudioRoomDest = SMAP_AUDIOROOM_ASSIGNMENT_NONE;
   		this->m_cfg.data[i].m_bAudioRoomDestLink = SMAP_AUDIOROOM_ASSIGNMENT_NONE;
	}
}
BOOL CSerSmapAVRoom::getString(
	char* pbData,
	int maxLength,
	int* length)
{
	CString szTemp;

	// CHANGES_PRINTF_PARAM_TYPOS
	
	szTemp.Format("SMAPAUDIOROOM=%d;\n"				 
				  "szName=%s\n"			
				  "SMAPAUDIOROOMDEST=0;\n"
				  "bAddress=0x%02x;0x%02x\n"
				  "bDest=0x%02x;\n"
				  "bDestLink=0x%02x\n"
				  "SMAPAUDIOROOMDEST=1;\n"
				  "bAddress=0x%02x;0x%02x\n"
				  "bDest=0x%02x;\n"
				  "bDestLink=0x%02x\n"
				  "SMAPAUDIOROOMDEST=2;\n"
				  "bAddress=0x%02x;0x%02x\n"
				  "bDest=0x%02x;\n"
				  "bDestLink=0x%02x\n"
				  "SMAPAUDIOROOMDEST=3;\n"
				  "bAddress=0x%02x;0x%02x\n"
				  "bDest=0x%02x;\n"
				  "bDestLink=0x%02x\n",
				  this->getIndex(),					// "SMAPAUDIOROOM=%d;\n"				
				  this->getName(),					// "szName=%s\n"				
				  this->getAudioRoomNodeAddress(0),	// dest 0
				  this->getAudioRoomUnitAddress(0),
				  this->getAudioRoomDest(0),
				  this->getAudioRoomDestLink(0),
				  this->getAudioRoomNodeAddress(1),	// dest 1
				  this->getAudioRoomUnitAddress(1),
				  this->getAudioRoomDest(1),
				  this->getAudioRoomDestLink(1),
				  this->getAudioRoomNodeAddress(2),	// dest 2
				  this->getAudioRoomUnitAddress(2),
				  this->getAudioRoomDest(2),
				  this->getAudioRoomDestLink(2),
				  this->getAudioRoomNodeAddress(3),	// dest 3
				  this->getAudioRoomUnitAddress(3),
				  this->getAudioRoomDest(3),
				  this->getAudioRoomDestLink(3));

	const int stringLength = szTemp.GetLength() + 1;
	if (maxLength >= stringLength)
	{
		strcpy(pbData,szTemp.GetBuffer());
		*length = stringLength;
		return TRUE;
	}

	return FALSE;
}

/* Speciale functies */
CSerSmapAVRoom* CSerSmapAVRoom::doCast(CSerSmapVRoom* p)
{
	CSerSmapAVRoom* pReturn = 0;

	if (p && p->getClassID( ) == CSerSmapAVRoom::ID)
	{
		pReturn = static_cast<CSerSmapAVRoom*>(p);
	}
	return pReturn;
}

const CSerSmapAVRoom* CSerSmapAVRoom::doCast( const CSerSmapVRoom* p)
{
	const CSerSmapAVRoom* pReturn = 0;

	if (p && p->getClassID( ) == CSerSmapAVRoom::ID)
	{
		pReturn = static_cast<const CSerSmapAVRoom*>(p);
	}
	return pReturn;
}

/*****************************************************/

/* CSerSmapTempRoom */
/*****************************************************/
CSerSmapTempRoom::CSerSmapTempRoom(void) :
m_cfg( ) {
	init( );
}

CSerSmapTempRoom::~CSerSmapTempRoom(void) { }

int CSerSmapTempRoom::getClassID( ) const {
	return m_cfg.hdr.m_bTypeItemData;
}

void CSerSmapTempRoom::setIndex(BYTE bIndex) {
	m_cfg.hdr.m_bIndex = bIndex;
}
BYTE CSerSmapTempRoom::getIndex( ) const {
	return m_cfg.hdr.m_bIndex;
}

const char* CSerSmapTempRoom::getName( ) const {
	return m_cfg.hdr.m_szName;
}
void CSerSmapTempRoom::setName(const char* szName) {
	using namespace DUOTECNO::DATA;
	if (szName) {
		memcpy(&m_cfg.hdr.m_szName[0], szName, SMAP_VROOM_MAX_NAME_LENGTH - 1);
   		m_cfg.hdr.m_szName[SMAP_VROOM_MAX_NAME_LENGTH - 1] = '\0';
	}
}

/*********************** public **********************/
int CSerSmapTempRoom::getData(BYTE* pbData)
{
	using DUOTECNO::DATA::SerSmapTempRoom;
	if (pbData) {
		memcpy(pbData,&this->m_cfg,sizeof(SerSmapTempRoom));
		return sizeof(CSerSmapTempRoom);
	}

	return 0;
}
void CSerSmapTempRoom::dump( )
{
	char szBuffer[1024];
	int length;
	if (getString(szBuffer, 255, &length)) {
		TRACE( szBuffer );
	}
}

BOOL CSerSmapTempRoom::save(FILE* fs)
{	
	if (fs) {
		int length;
		char szBuffer[1024];
		if (getString(szBuffer, 1024, &length)) {
			fputs(szBuffer,fs);		// CHANGES_0X0A01_USE_FPUTS
			return TRUE;
		}	
	}
	return FALSE;
}

BYTE CSerSmapTempRoom::getNodeAddress() const
{
	return m_cfg.data.m_bRoomID[DUOTECNO::DATA::SMAP_NODEADDRESS_IDX];
}
void CSerSmapTempRoom::setNodeAddress(BYTE bNodeAddress)
{
	m_cfg.data.m_bRoomID[DUOTECNO::DATA::SMAP_NODEADDRESS_IDX] = bNodeAddress;
}

BYTE CSerSmapTempRoom::getUnitAddress() const
{
	return m_cfg.data.m_bRoomID[DUOTECNO::DATA::SMAP_UNITADDRESS_IDX];
}
void CSerSmapTempRoom::setUnitAddress(BYTE bUnitAddress)
{
	 m_cfg.data.m_bRoomID[DUOTECNO::DATA::SMAP_UNITADDRESS_IDX] = bUnitAddress;
}

BYTE CSerSmapTempRoom::getType() const
{
	return m_cfg.data.m_bType;
}
void CSerSmapTempRoom::setType(BYTE bType)
{
	m_cfg.data.m_bType = bType;
}

/************************ protected *****************/
void CSerSmapTempRoom::init( )
{
	using namespace DUOTECNO::DATA;

	this->m_cfg.hdr.m_bIndex = SMAP_AUDIOROOM_ASSIGNMENT_NONE;	
	memset(&this->m_cfg.hdr.m_szName[0],
		   '\0',
		   SMAP_VROOM_MAX_NAME_LENGTH);
	
	this->m_cfg.hdr.m_bTypeItemData = CSerSmapTempRoom::ID;
	this->m_cfg.data.m_bRoomID[SMAP_NODEADDRESS_IDX] = SMAP_AUDIOROOM_ASSIGNMENT_NONE; 
	this->m_cfg.data.m_bRoomID[SMAP_UNITADDRESS_IDX] = SMAP_AUDIOROOM_ASSIGNMENT_NONE; 
	this->m_cfg.data.m_bType = SMAP_AUDIOROOM_ASSIGNMENT_NONE;
}

BOOL CSerSmapTempRoom::getString(
	char* pbData,
	int maxLength,
	int* length)
{
	CString szTemp;

	szTemp.Format("SMAPTEMPROOM=%d;\n"				 
				  "szName=%s\n"		
				  "SMAPTEMPROOMUNIT=0;\n"
				  "bAddress=0x%02x;0x%02x\n"
				  "bType=0x%02x;\n",				 
				  this->getIndex(),					// "SMAPAUDIOROOM=%d;\n"				
				  this->getName(),					// "szName=%s\n"				
				  this->getNodeAddress(),
				  this->getUnitAddress(),
				  this->getType());

	const int stringLength = szTemp.GetLength() + 1;
	if (maxLength >= stringLength)
	{
		strcpy(pbData,szTemp.GetBuffer());
		*length = stringLength;
		return TRUE;
	}

	return FALSE;
}

/* Speciale functies */
CSerSmapTempRoom* CSerSmapTempRoom::doCast(CSerSmapVRoom* p)
{
	CSerSmapTempRoom* pReturn = 0;
	if (p && p->getClassID( ) == CSerSmapTempRoom::ID) {
		pReturn = static_cast<CSerSmapTempRoom*>(p);
	}
	return pReturn;
}

const CSerSmapTempRoom* CSerSmapTempRoom::doCast( const CSerSmapVRoom* p)
{
	const CSerSmapTempRoom* pReturn = 0;
	if (p && p->getClassID( ) == CSerSmapTempRoom::ID) {
		pReturn = static_cast<const CSerSmapTempRoom*>(p);
	}
	return pReturn;
}


/*****************************************************/
/********** CSerSmapSerSmapUnitMeasureItem  **********/
/*****************************************************/
CSerSmapSerSmapUnitMeasureItem::CSerSmapSerSmapUnitMeasureItem(void) :
m_cfg( ) {
	init( );
}

CSerSmapSerSmapUnitMeasureItem::~CSerSmapSerSmapUnitMeasureItem(void) { }

int CSerSmapSerSmapUnitMeasureItem::getClassID( ) const {
	return m_cfg.hdr.m_bTypeItemData;
}

void CSerSmapSerSmapUnitMeasureItem::setIndex(BYTE bIndex) {
	m_cfg.hdr.m_bIndex = bIndex;
}
BYTE CSerSmapSerSmapUnitMeasureItem::getIndex( ) const {
	return m_cfg.hdr.m_bIndex;
}

const char* CSerSmapSerSmapUnitMeasureItem::getName( ) const {
	return m_cfg.hdr.m_szName;
}
void CSerSmapSerSmapUnitMeasureItem::setName(const char* szName)
{
	using namespace DUOTECNO::DATA;
	if (szName) {
		memcpy(&m_cfg.hdr.m_szName[0], szName, SMAP_VROOM_MAX_NAME_LENGTH - 1);
   		m_cfg.hdr.m_szName[SMAP_VROOM_MAX_NAME_LENGTH - 1] = '\0';
	}
}
/*********************** public **********************/
int CSerSmapSerSmapUnitMeasureItem::getData(BYTE* pbData)
{
	using DUOTECNO::DATA::SerSmapUnitMeasureItem;

	if (pbData) {
		memcpy(pbData,&this->m_cfg,sizeof(SerSmapUnitMeasureItem));
		return sizeof(SerSmapUnitMeasureItem);
	}
	return 0;
}
void CSerSmapSerSmapUnitMeasureItem::dump( )
{
	char szBuffer[1024];
	int length;
	if (getString(szBuffer, 255, &length)) {
		TRACE( szBuffer );
	}
}
BOOL CSerSmapSerSmapUnitMeasureItem::save(FILE* fs)
{	
	if (fs) {
		int length;
		char szBuffer[1024];
		if (getString(szBuffer, 1024, &length)) {
			fputs(szBuffer,fs);		// CHANGES_0X0A01_USE_FPUTS
			return TRUE;
		}	
	}
	return FALSE;
}

BYTE CSerSmapSerSmapUnitMeasureItem::getNodeAddress() const {
	return m_cfg.data.m_bRoomID[DUOTECNO::DATA::SMAP_NODEADDRESS_IDX];
}
void CSerSmapSerSmapUnitMeasureItem::setNodeAddress(BYTE bNodeAddress) {
	m_cfg.data.m_bRoomID[DUOTECNO::DATA::SMAP_NODEADDRESS_IDX] = bNodeAddress;
}
BYTE CSerSmapSerSmapUnitMeasureItem::getUnitAddress() const {
	return m_cfg.data.m_bRoomID[DUOTECNO::DATA::SMAP_UNITADDRESS_IDX];
}
void CSerSmapSerSmapUnitMeasureItem::setUnitAddress(BYTE bUnitAddress) {
	 m_cfg.data.m_bRoomID[DUOTECNO::DATA::SMAP_UNITADDRESS_IDX] = bUnitAddress;
}
BYTE CSerSmapSerSmapUnitMeasureItem::getType() const {
	return m_cfg.data.m_bType;
}
void CSerSmapSerSmapUnitMeasureItem::setType(BYTE bType) {
	m_cfg.data.m_bType = bType;
}

int CSerSmapSerSmapUnitMeasureItem::getRegisterAddress() const {
	return m_cfg.data.m_usRegisterAddress;
}
void CSerSmapSerSmapUnitMeasureItem::setRegisterAddress(int address) {
	int registerAddress = 0xffff;
	if (address>=0 && address <=1024) {
		registerAddress = address;
	}
	m_cfg.data.m_usRegisterAddress = registerAddress;
}
int CSerSmapSerSmapUnitMeasureItem::getRefIcon() const {
	return m_cfg.data.m_bRefIcon;
}
void CSerSmapSerSmapUnitMeasureItem::setRefIcon(int refIcon) {
	int r = 0xff;
	if (refIcon>=0 && refIcon <=0xff) {
		r = refIcon;
	}
	m_cfg.data.m_bRefIcon = refIcon;
}

const char* CSerSmapSerSmapUnitMeasureItem::getStrMeasurementUnit( ) const {
	return &m_cfg.data.m_strMeasureUnit[0];
}
void CSerSmapSerSmapUnitMeasureItem::setStrMeasurementUnit(const char* s) {
	if (s) {
		memcpy(&m_cfg.data.m_strMeasureUnit[0], s, sizeof(m_cfg.data.m_strMeasureUnit)-1);
   		m_cfg.data.m_strMeasureUnit[sizeof(m_cfg.data.m_strMeasureUnit)-1] = '\0';
	}
}

int CSerSmapSerSmapUnitMeasureItem::getDivider() const {
	return m_cfg.data.m_bDivider;
}			
void CSerSmapSerSmapUnitMeasureItem::setDivider(int d) {
	int divider = 1;
	if (d>=0 && d <=0xff) {
		divider = d;
	}
	m_cfg.data.m_bDivider = divider;
}

int CSerSmapSerSmapUnitMeasureItem::getMultiplier() const {
	return m_cfg.data.m_bMultiplier;
}
void CSerSmapSerSmapUnitMeasureItem::setMultiplier(int v) {
	int multiplier = 1;
	if (v>=0 && v <=0xff) {
		multiplier = v;
	}
	m_cfg.data.m_bMultiplier = multiplier;
}

int CSerSmapSerSmapUnitMeasureItem::getNrDecimals() const {
	return m_cfg.data.m_bDecimals;
}
void CSerSmapSerSmapUnitMeasureItem::setNrDecimals(int d) {
	int decimals = 1;
	if (d>=0 && d <=0xff) {
		decimals = d;
	}
	m_cfg.data.m_bDecimals = decimals;
}

/************************ protected *****************/
void CSerSmapSerSmapUnitMeasureItem::init( )
{
	using namespace DUOTECNO::DATA;

	this->m_cfg.hdr.m_bIndex = SMAP_AUDIOROOM_ASSIGNMENT_NONE;	
	memset(&this->m_cfg.hdr.m_szName[0],'\0',SMAP_VROOM_MAX_NAME_LENGTH);	
	this->m_cfg.hdr.m_bTypeItemData = CSerSmapSerSmapUnitMeasureItem::ID;
	this->m_cfg.data.m_bRoomID[SMAP_NODEADDRESS_IDX] = SMAP_AUDIOROOM_ASSIGNMENT_NONE; 
	this->m_cfg.data.m_bRoomID[SMAP_UNITADDRESS_IDX] = SMAP_AUDIOROOM_ASSIGNMENT_NONE; 
	this->m_cfg.data.m_bType = SMAP_AUDIOROOM_ASSIGNMENT_NONE;
	memset(this->m_cfg.data.m_strMeasureUnit,'\0',sizeof(this->m_cfg.data.m_strMeasureUnit));
	memset(this->m_cfg.data.m_strReserved,'\0',sizeof(this->m_cfg.data.m_strReserved));
	this->m_cfg.data.m_usRegisterAddress = 0xffff;
	this->m_cfg.data.m_bRefIcon = 255;
	this->m_cfg.data.m_bDivider = 1;
	this->m_cfg.data.m_bMultiplier = 1;
	this->m_cfg.data.m_bDecimals = 0;
	memset(this->m_cfg.data.m_bReserved,0,sizeof(this->m_cfg.data.m_bReserved));
}

BOOL CSerSmapSerSmapUnitMeasureItem::getString(char* pbData,int maxLength,int* length)
{
	CString szTemp;
	const char* szMeasureUnit = "";
	const char* s = this->getStrMeasurementUnit();
	if ((0 != s) && (s[0] != '\0')) {
		szMeasureUnit = s;
	}
	szTemp.Format("SMAPUNITMEASUREROOM=%d;\n"				 
				  "szName=%s\n"		
				  "SMAPUNITMEASURE=0;\n"
				  "bAddress=0x%02x;0x%02x\n"
				  "usRegisterAddress=%d;\n"
				  "bRefIcon=%d;\n"
				  "szMeasureUnit=%s;\n"
				  "bDividerValue=%d;\n"
				  "bNrDecimals=%d;\n",
				  this->getIndex(),					// "SMAPAUDIOROOM=%d;\n"				
				  this->getName(),					// "szName=%s\n"				
				  this->getNodeAddress(),
				  this->getUnitAddress(),
				  this->getRegisterAddress(),
				  this->getRefIcon(),
				  szMeasureUnit,
				  this->getDivider(),
				  this->getNrDecimals());

	const int stringLength = szTemp.GetLength() + 1;
	if (maxLength >= stringLength) {
		strcpy(pbData,szTemp.GetBuffer());
		*length = stringLength;
		return TRUE;
	}
	return FALSE;
}

/* Speciale functies */
CSerSmapSerSmapUnitMeasureItem* CSerSmapSerSmapUnitMeasureItem::doCast(CSerSmapVRoom* p)
{
	CSerSmapSerSmapUnitMeasureItem* pReturn = 0;
	if (p && p->getClassID( ) == CSerSmapSerSmapUnitMeasureItem::ID) {
		pReturn = static_cast<CSerSmapSerSmapUnitMeasureItem*>(p);
	}
	return pReturn;
}

const CSerSmapSerSmapUnitMeasureItem* CSerSmapSerSmapUnitMeasureItem::doCast( const CSerSmapVRoom* p)
{
	const CSerSmapSerSmapUnitMeasureItem* pReturn = 0;
	if (p && p->getClassID( ) == CSerSmapSerSmapUnitMeasureItem::ID) {
		pReturn = static_cast<const CSerSmapSerSmapUnitMeasureItem*>(p);
	}
	return pReturn;
}
/*****************************************************/


/*****************************************************
 * SMAPAUDIOROOM=0;
 * szName=Audioroom 1
 * SMAPAUDIOROOMDEST=0;
 * bAddress=0xfc;0x20;
 * bDest=0x00;
 * bDestLink=0xff;
 * SMAPAUDIOROOMDEST=1;
 * bAddress=0xfc;0x21;
 * bDest=0x00;
 * bDestLink=0xff;
 * SMAPAUDIOROOMDEST=2;
 * bAddress=0xfc;0x21;
 * bDest=0x01;
 * bDestLink=0xff;
 * SMAPAUDIOROOMDEST=3;
 * bAddress=0xfc;0x21;
 * bDest=0x02;
 * bDestLink=0xff;
 * SMAPUNITMEASUREROOM=1;
 * szName="verbruik elek"
 * SMAPUNITMEASURE=0;
 * szMeasureUnit="kwh";
 * usRegisterAddress=0;
 * bRefIcon=255;
 * bDividerValue=1;										
 * bNrDecimals=1;
 *****************************************************/

/*****************************************************/
const char* const CConfigSmapVRoomClass::_szHeaderAVRoom = "SMAPAUDIOROOM";
const char* const CConfigSmapVRoomClass::_szHeaderTempRoom = "SMAPTEMPROOM";
const char* const CConfigSmapVRoomClass::_szHeaderMeasureUnitItem = "SMAPUNITMEASUREROOM";
/*****************************************************/
CConfigSmapVRoomClass::CConfigSmapVRoomClass(
	DUOTECNO::MFC_HELPER::IReportError *ptr) :
	m_pErrorReport( ptr ), m_list( ), m_pCurrent( 0 ), m_CurrentState( STATE_IDLE ), m_bCurrentDest( 0 )
{
	for (int i=0;i<MAX_INDEX;i++) {
		m_bUsed[i] = 0;
	}
}

CConfigSmapVRoomClass::~CConfigSmapVRoomClass(void)
{
	removeAll( );
}

CSerSmapVRoom* CConfigSmapVRoomClass::getEntry(int index)
{
	ASSERT (index < m_list.getCount( ));
	return (m_list.getEntry( index ));
}

BOOL CConfigSmapVRoomClass::helper_addEntry(int index,CSerSmapVRoom* p) {
	if (0 == p) {
		return FALSE;
	}	
	if (!addEntry(p)) {	
		delete p;
		p = 0;
		return FALSE;
	}

	m_pCurrent = p;	
	m_bUsed[index] = 1;
	m_pCurrent->setIndex(static_cast<BYTE>(index));
	m_CurrentState = STATE_HEADER;	
	return TRUE;
}

BOOL CConfigSmapVRoomClass::ParseHeader(char *szAttribute,char *szData)
{
	if (strcmp(_szHeaderAVRoom,szAttribute) == 0) {				

		m_pCurrent = 0;

		int tempValue = 0;
		if (sscanf(szData,"%d",&tempValue) == 1) {				
			if ((tempValue >= 0) && (tempValue < MAX_INDEX)) {
				return helper_addEntry(tempValue,new CSerSmapAVRoom( ));				
			}
		}
		
		char szBuffer[255];
		sprintf(szBuffer,"Error:Screenmap AV Room field could not be parsed");
		ReportError(szBuffer);		
	}
	else if (strcmp(_szHeaderTempRoom,szAttribute) == 0) 
	{				
		m_pCurrent = 0;

		int tempValue = 0;
		if (sscanf(szData,"%d",&tempValue) == 1) {				
			if ((tempValue >= 0) && (tempValue < MAX_INDEX)) {
				return helper_addEntry(tempValue,new CSerSmapTempRoom( ));						
			}
		}
	
		char szBuffer[255];
		sprintf(szBuffer,"Error:Screenmap Temp Room field could not be parsed");
		ReportError(szBuffer);
		
	}
	else if (strcmp(_szHeaderMeasureUnitItem,szAttribute) == 0) 
	{
		m_pCurrent = 0;

		int tempValue = 0;
		if (sscanf(szData,"%d",&tempValue) == 1) {				
			if ((tempValue >= 0) && (tempValue < MAX_INDEX)) {
				return helper_addEntry(tempValue,new CSerSmapSerSmapUnitMeasureItem( ));				
			}
		}
		
		char szBuffer[255];
		sprintf(szBuffer,"Error:Screenmap Virtual unit measure room field could not be parsed");
		ReportError(szBuffer);		
	}
	return FALSE;
}

/*************************************************************/
/* Parsen van een audio avroom item --> class CSerSmapAVRoom 
 * bAddress=<hex>;<hex>;
 * bDest=<hex> 
 * bDestLink=<hex>
/*************************************************************/
BOOL CConfigSmapVRoomClass::ParseData_itemAvroom(char *szAttribute,char *szData) {
	static const char* const ITEM_ADDRESS = "bAddress";					
	static const char* const ITEM_AVROOM_DEST = "bDest";				/* enkel audio */	
	static const char* const ITEM_AVROOM_DESTLINK = "bDestLink";		/* enkel audio */

	using namespace DUOTECNO::DATA;
	if (m_bCurrentDest < SMAP_AUDIOROOM_MAX_DATA_ITEMS) {
		if (strcmp(ITEM_ADDRESS,szAttribute) == 0) {											
			int values[2];
			if (sscanf(szData,"%x;%x;",&values[0],&values[1]) == 2) {
				CSerSmapAVRoom* const pSerSmapAVRoom = CSerSmapAVRoom::doCast(m_pCurrent);
				if (pSerSmapAVRoom) {
					pSerSmapAVRoom->setAudioRoomNodeAddress(m_bCurrentDest,values[0]);
					pSerSmapAVRoom->setAudioRoomUnitAddress(m_bCurrentDest,values[1]);
					return TRUE;
				}								
			}
		}		
		else if (strcmp(ITEM_AVROOM_DEST,szAttribute) == 0) {
			int value;
			if (sscanf(szData,"%x",&value) == 1) {
				CSerSmapAVRoom* const pSerSmapAVRoom = CSerSmapAVRoom::doCast(m_pCurrent);
				if (pSerSmapAVRoom) {
					pSerSmapAVRoom->setAudioRoomDest(m_bCurrentDest,value);	
					return TRUE;
				}
			}				
		}	
		else if (strcmp(ITEM_AVROOM_DESTLINK,szAttribute) == 0) {
			int value;
			if (sscanf(szData,"%x",&value) == 1) {
				CSerSmapAVRoom* const pSerSmapAVRoom = CSerSmapAVRoom::doCast(m_pCurrent);
				if (pSerSmapAVRoom) {
					pSerSmapAVRoom->setAudioRoomDestLink(m_bCurrentDest,value);	
					return TRUE;
				}
			}	
		}					
	}
	return FALSE;
}

/***************************************************************/
/* Parsen van een sensor room item --> class CSerSmapTempRoom  
 * bAddress=<hex>;<hex>;
 * bType=<hex>;
/***************************************************************/
BOOL CConfigSmapVRoomClass::ParseData_itemSensroom(char *szAttribute,char *szData) {
	static const char* const ITEM_ADDRESS = "bAddress";				
	static const char* const ITEM_SENSOR_TYPE = "bType";		

	// using namespace DUOTECNO::DATA;				
	// 
	if (strcmp(ITEM_ADDRESS,szAttribute) == 0) {											
		int values[2];
		if (sscanf(szData,"%x;%x;",&values[0],&values[1]) == 2) {
			CSerSmapTempRoom* const pSerSmapTempRoom = CSerSmapTempRoom::doCast(m_pCurrent);
			if (pSerSmapTempRoom) {
				pSerSmapTempRoom->setNodeAddress(values[0]);
				pSerSmapTempRoom->setUnitAddress(values[1]);
				return TRUE;
			}								
		}
	}
	else if (strcmp(ITEM_SENSOR_TYPE,szAttribute) == 0) {
		int value;
		if (sscanf(szData,"%x;",&value) == 1) {
			CSerSmapTempRoom* const pSerSmapTempRoom = CSerSmapTempRoom::doCast(m_pCurrent);
			if (pSerSmapTempRoom) {
				pSerSmapTempRoom->setType(value);
				return TRUE;
			}
		}
	}
	return FALSE;
}

/***********************************************************************************/
/* Parsen van een unit measure room item --> class CSerSmapSerSmapUnitMeasureItem  
 * bAddress=<hex>;<hex>;		// reserve.
 * bType=<hex>;
 * szMeasureUnit="kwh";			
 * usRegisterAddress=0;
 * bRefIcon=255;
 * bDividerValue=1;
 * bMultiplierValue=1;
 * bNrDecimals=1;
/***********************************************************************************/
BOOL CConfigSmapVRoomClass::ParseData_itemMeasureItem(char *szAttribute,char *szData) {
	// Data velden - algemeen.
	static const char* const ITEM_ADDRESS = "bAddress";							/* Algemeen */
	static const char* const ITEM_TYPE = "bType";		
	static const char* const ITEM_STR_MEASURE_UNIT = "szMeasureUnit";
	static const char* const ITEM_REGISTERADDRESS = "usRegisterAddress";
	static const char* const ITEM_ICON = "bRefIcon";
	static const char* const ITEM_DIVIDER_VALUE = "bDividerValue";
	static const char* const ITEM_MULTIPLIER_VALUE = "bMultiplierValue";		// Reserve...
	static const char* const ITEM_NR_DECIMALS = "bNrDecimals";
			
	CSerSmapSerSmapUnitMeasureItem* const pSerClass = CSerSmapSerSmapUnitMeasureItem::doCast(m_pCurrent);
	if (0 == pSerClass) {
		ASSERT(0 != pSerClass);
		return FALSE;
	}

	if (0 == strcmp(ITEM_ADDRESS,szAttribute)) {											
		int values[2];
		if (sscanf(szData,"%x;%x;",&values[0],&values[1]) == 2) {			
			pSerClass->setNodeAddress(values[0]);
			pSerClass->setUnitAddress(values[1]);
			return TRUE;											
		}
	}
	else if (0 == strcmp(ITEM_TYPE,szAttribute)) { 
		int value;
		if (1 == sscanf(szData,"%x",&value)) {
			if (value>=0 && value<=0xff) {				
				pSerClass->setType(value);				
			}
		}
	}
	else if (0 == strcmp(ITEM_STR_MEASURE_UNIT,szAttribute)) { 							
		if (szData) {
			pSerClass->setStrMeasurementUnit(szData);
		}
		return TRUE;
	}
	else if (0 == strcmp(ITEM_REGISTERADDRESS,szAttribute)) {
		int value;
		if (1 == sscanf(szData,"%d",&value)) {
			if (value>=0 && value<=0xffff) {				
				pSerClass->setRegisterAddress(value);	
			}
		}
		return TRUE;
	}
	else if (0 == strcmp(ITEM_ICON,szAttribute)) {
		int value;
		if (1 == sscanf(szData,"%d",&value)) {
			if (value>=0 && value<=0xff) {				
				pSerClass->setRefIcon(value);
			}
		}
		return TRUE;
	}
	else if (0 == strcmp(ITEM_DIVIDER_VALUE,szAttribute)) {
		int value;
		if (1 == sscanf(szData,"%d",&value)) {
			if (value>=0 && value<=0xff) {				
				pSerClass->setDivider(value);
			}
		}
		return TRUE;
	}
	else if (0 == strcmp(ITEM_NR_DECIMALS,szAttribute)) {
		int value;
		if (1 == sscanf(szData,"%d",&value)) {
			if (value>=0 && value<=0xff) {				
				pSerClass->setNrDecimals(value);
			}
		}
		return TRUE;
	}
	else if (0 == strcmp(ITEM_MULTIPLIER_VALUE,szAttribute)) {
		int value;
		if (1 == sscanf(szData,"%d",&value)) {
			if (value>=0 && value<=0xff) {				
				pSerClass->setMultiplier(value);
			}
		}
		return TRUE;
	}
	return FALSE;
}


BOOL CConfigSmapVRoomClass::parseData_headerItem(char *szAttribute,char *szData) {	
	static const char* const ITEM_HEADER_AVROOM = "SMAPAUDIOROOMDEST";
	static const char* const ITEM_HEADER_TEMP = "SMAPSENSORUNIT";
	static const char* const ITEM_HEADER_UNITMEASURE = "SMAPUNITMEASURE";

	if (0 == strcmp(ITEM_HEADER_AVROOM,szAttribute)) {						
		int value = 0;
		if (sscanf(szData,"%d",&value) == 1) {												
			m_bCurrentDest = value;
			m_CurrentState = STATE_AVROOM_ITEM;
		}
		else {			
			m_CurrentState = STATE_AVROOM_ITEM;
		}		
		return TRUE;
	}
	else if (0 == strcmp(ITEM_HEADER_TEMP,szAttribute)) {
		m_CurrentState = STATE_TEMP_ITEM;	
		return TRUE;
	}
	else if (0 == strcmp(ITEM_HEADER_UNITMEASURE,szAttribute)) {
		m_CurrentState = STATE_UNITMEASURE_ITEM;
		return TRUE;
	}
	else {
		/*******************************************************************/
		/* Parsen van de algemene properties --> header (naam van de kamer
		/* szName=<string>
		/*******************************************************************/
		if (STATE_HEADER == m_CurrentState) {
			static const char* const HDR_NAME = "szName";	
			if (strcmp(HDR_NAME,szAttribute) == 0) {						
				if (szData) {
					m_pCurrent->setName(szData);			
				}
				return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL CConfigSmapVRoomClass::ParseData(char *szAttribute,char *szData)
{
	if (m_pCurrent == 0) {
		ASSERT( m_pCurrent != 0);
		return FALSE;
	}		
	if (!parseData_headerItem(szAttribute,szData)) {
		switch (m_CurrentState) {					
			case STATE_AVROOM_ITEM: {
				ParseData_itemAvroom(szAttribute,szData);				
			} break;	
			case STATE_TEMP_ITEM: {
				ParseData_itemSensroom(szAttribute,szData);			
			} break;			
			case STATE_UNITMEASURE_ITEM: {										 										
				ParseData_itemMeasureItem(szAttribute,szData);										 										 
			} break; 			
		}
	}
	return FALSE;
}

void CConfigSmapVRoomClass::Initialise(void)
{
	removeAll( );
	m_pCurrent = 0;
	for (int i=0;i<MAX_INDEX;i++) {
		m_bUsed[i] = 0;
	}
	m_CurrentState = STATE_IDLE;
	m_bCurrentDest = 0;
}

int CConfigSmapVRoomClass::GetMax(void) const
{
	int max = 0;
	for (int i=0;i<MAX_INDEX;i++) {
		if (m_bUsed[i] == 1) {
			max = i+1;
		}
	}
	return max;
}

void CConfigSmapVRoomClass::dump( )
{
	const int max = m_list.getCount( );

	for (int i=0;i<max;i++)
	{
		CSerSmapVRoom* const p = getEntry( i );
		if (p)
		{
			p->dump( );
		}
	}
}

// TM,V16.25 - Oplossing BUG-0171.
// Instantie van CSerSmapVRoom werd opgevraagd via 'index' ipv via 'i'.
// hierdoor was de data corrupt.
int CConfigSmapVRoomClass::GetData(int index,BYTE *pbData)
{
	int length = 0;
	for (int i=0;i<MAX_INDEX;i++) {		
		CSerSmapVRoom* const p = m_list.getEntry(i); // BUG-0171: Was 'index' ipv 'i'
		if (p && p->getIndex() == index) {
			length = p->getData(pbData);
			return length;
		}	
	}
	return length;
}

BOOL CConfigSmapVRoomClass::IsUsed(
	int index)
{
	if (index < MAX_INDEX)
	{
		return (m_bUsed[index] == 1 ? TRUE : FALSE);
	}
	return FALSE;
}	

BOOL CConfigSmapVRoomClass::save(
	FILE* fs)
{
	BOOL result = FALSE;

	if (fs)
	{
		const int max = m_list.getCount( );
		for (int i=0;i<max;i++)
		{
			CSerSmapVRoom* const p = getEntry( i );
			if (p)
			{
				p->save(fs);
			}
		}	
		result = TRUE;
	}
	return result;
}

/*********************** Protected *******************/

void CConfigSmapVRoomClass::removeAll( )
{
	while (!m_list.isEmpty())
	{
		CSerSmapVRoom* const p = m_list.removeHead( );
		ASSERT( p );
		delete p;
	}

	ASSERT( m_list.getCount( ) == 0 );
}

void CConfigSmapVRoomClass::ReportError(
	const char* szMessage)
{
	if (m_pErrorReport)
	{
		m_pErrorReport->reportError( szMessage );
	}
}

BOOL CConfigSmapVRoomClass::addEntry(CSerSmapVRoom* p)
{
	ASSERT( p );
	m_list.addTail(p);
	return TRUE;
}
