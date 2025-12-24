#include "stdafx.h"
#include "httprcfg_classes.h"

#include <time.h>			// random string aanmaken
#include <stdlib.h>			// random string aanmaken
/**************************************************************/
#define REQUIRE			ASSERT
#define ALLEGE			ASSERT
/**************************************************************/


/**************************************************************/
namespace DUOTECNO {
/**************************************************************/

template <class T>
static void list_deleteAll(CList<T*>* dest) {
	if (0 == dest) 
		return;

	while (!dest->IsEmpty()) {
		T* const p =  reinterpret_cast<T*>(dest->RemoveHead());
		ASSERT( p );
		delete p;
	}
	ASSERT( dest->GetCount( ) == 0 );
}

template <class T>
static void list_append(CList<T*>* dest,const CList<T*>& src) {
	if (0 == dest) 
		return;

	POSITION pos = src.GetHeadPosition();
	while (pos) {
		T* const pTemp = src.GetNext(pos);
		ALLEGE(0 != pTemp);
		dest->AddTail(pTemp->clone());
		//dest->AddTail(new T(*pTemp));
	}	
}

template <class T>
bool list_getItem(const CList<T*>& src,int idx,T** ppFile) {
	const int count = src.GetCount();
	if (idx >=count) {
		REQUIRE(idx<count);
		return false;
	}

	POSITION pos = src.GetHeadPosition();
	for (int i = 0; i < count; i++)
	{
		T* pItem = src.GetNext(pos);
		if (idx==i) {
			*ppFile = pItem;
			return true;
		}		
	}
	return false;
}


/**************************************************************/
/**************************************************************/
/**************************************************************/
AppCfgBindAction::AppCfgBindAction(Action_t action) : m_action(action) { 
	clear( );
}

void AppCfgBindAction::clear( ) {
	memset(m_nodes,0,sizeof(m_nodes));
	m_numberOfNodes = 0;
}

void AppCfgBindAction::add(int nodeAddress) {
	const int length = sizeof(m_nodes)/sizeof(int);
	if (m_numberOfNodes<length-1) {
		m_nodes[m_numberOfNodes] = nodeAddress;
		m_numberOfNodes++;
	}
}

int AppCfgBindAction::getCount() const {
	return m_numberOfNodes;
}

int AppCfgBindAction::get(int index) const {
	if (index>=m_numberOfNodes) {
		return -1;
	}
	return m_nodes[index];
}

AppCfgBindAction::Action_t AppCfgBindAction::getAction() const {
	return m_action;
}
/**************************************************************/




/**************************************************************/
AppCfgLogInfo::AppCfgLogInfo( ) : m_stringList( ) { }


void AppCfgLogInfo::add(const char* s) {
	m_stringList.AddTail(s);
}
const CStringList& AppCfgLogInfo::getStrings() const {
	return m_stringList;
}
/**************************************************************/


/**************************************************************/
/**************************************************************/
/**************************************************************/
AppCfgBindInfoNode::AppCfgBindInfoNode() : m_nodeAddress(-1),
	m_bindFileNrOfEntries(-1), m_bindFileChecksum(-1), m_nrOfEntries(0), m_countBindDone(0), 
	m_status(BINDINGFILE_FSM_STATUS_IDLE)/*,m_error(BINDINGFG_EC_NONE)*/  {	 }

AppCfgBindInfoNode::AppCfgBindInfoNode(const AppCfgBindInfoNode& l) : m_nodeAddress(l.m_nodeAddress),
	m_bindFileNrOfEntries(l.m_bindFileNrOfEntries), m_bindFileChecksum(l.m_bindFileChecksum), m_nrOfEntries(l.m_nrOfEntries), 
	m_countBindDone(l.m_countBindDone), m_status(l.m_status) 
{ 
}

AppCfgBindInfoNode::~AppCfgBindInfoNode() { }

AppCfgBindInfoNode& AppCfgBindInfoNode::operator=(const AppCfgBindInfoNode& l) {
	if (this != &l) {
		this->m_nodeAddress = l.m_nodeAddress;
		this->m_bindFileNrOfEntries = l.m_bindFileNrOfEntries;
		this->m_bindFileChecksum = l.m_bindFileChecksum;
		this->m_nrOfEntries = l.m_nrOfEntries;
		this->m_countBindDone = l.m_countBindDone;
		this->m_status = l.m_status;
	}
	return *this;
}

AppCfgBindInfoNode* AppCfgBindInfoNode::clone() {
	return new AppCfgBindInfoNode(*this);
}

void AppCfgBindInfoNode::setNodeAddress(int nodeAddress) {
	REQUIRE(nodeAddress>=0x03 && nodeAddress<=0xFC);
	m_nodeAddress = nodeAddress;
}
void AppCfgBindInfoNode::setBindFileNrOfEntries(int v) {
	m_bindFileNrOfEntries = v;
}
void AppCfgBindInfoNode::setBindFileChecksum(int v) {
	m_bindFileChecksum = v;
}
void AppCfgBindInfoNode::setBindingsProgressCurrent(int v){
	m_countBindDone = v;
}
void AppCfgBindInfoNode::setBindingsProgressMax(int v) {
	m_nrOfEntries = v;
}
int AppCfgBindInfoNode::getBindingsProgressCurrent() const {	
	return m_countBindDone;
}
int AppCfgBindInfoNode::getBindingsProgressMax() const {
	return m_nrOfEntries;
}

void AppCfgBindInfoNode::formatSummary(CString* dest) {
	if (0 == dest)
		return;

	if (m_bindFileNrOfEntries >=0 && m_bindFileChecksum>=0) {
		dest->Format(_T("Node 0x%02x: Binding file entries=%d,Checksum=0x%02x"),
			m_nodeAddress, m_bindFileNrOfEntries, m_bindFileChecksum);
	}
	else {
		dest->Format(_T("Node 0x%02x: Binding file entries=%d,Checksum=%d - NOT WRITTEN"),
			m_nodeAddress, m_bindFileNrOfEntries, m_bindFileChecksum);
	}
}
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
#if(0)
static void list_deleteAll(CList<AppCfgBindInfoNode*>* dest) {
	if (0 == dest) 
		return;

	while (!dest->IsEmpty()) {
		AppCfgBindInfoNode* const p =  reinterpret_cast<AppCfgBindInfoNode*>(dest->RemoveHead());
		ASSERT( p );
		delete p;
	}
	ASSERT( dest->GetCount( ) == 0 );
}

static void list_append(CList<AppCfgBindInfoNode*>* dest,const CList<AppCfgBindInfoNode*>& src) {
	if (0 == dest) 
		return;

	POSITION pos = src.GetHeadPosition();
	while (pos) {
		AppCfgBindInfoNode* const pTemp = src.GetNext(pos);
		ALLEGE(0 != pTemp);
		dest->AddTail(new AppCfgBindInfoNode(*pTemp));
	}	
}
#endif 

AppCfgBindInfoNodes::AppCfgBindInfoNodes() : m_status( ), m_listNodes( ), 
	m_currentNodeIndex(0), m_maxNodes(0) { }

AppCfgBindInfoNodes& AppCfgBindInfoNodes::operator=(const AppCfgBindInfoNodes& l) {
	if (this != &l) {
		this->m_status = l.m_status;
		this->m_currentNodeIndex = l.m_currentNodeIndex;
		this->m_maxNodes = m_maxNodes;

		list_deleteAll(&this->m_listNodes);	
		list_append(&this->m_listNodes,l.m_listNodes);	
	}
	return *this;
}

AppCfgBindInfoNodes::~AppCfgBindInfoNodes( ) {
	list_deleteAll(&m_listNodes);	
}

void AppCfgBindInfoNodes::addNode(AppCfgBindInfoNode* newNode) {
	REQUIRE(0 != newNode);
	m_listNodes.AddTail(newNode);
}

void AppCfgBindInfoNodes::formatSummary(CStringList* dest) {
	if (0 == dest) 
		return;
	
	POSITION pos = m_listNodes.GetHeadPosition();
	while (pos) {
		CString string;
		AppCfgBindInfoNode* const pTemp = m_listNodes.GetNext(pos);
		ALLEGE( 0!= pTemp);
		pTemp->formatSummary(&string);
		dest->AddTail(string);
	}
}


int AppCfgBindInfoNodes::getCount() const {
	return m_listNodes.GetSize();
}
bool AppCfgBindInfoNodes::getNodeInfo(int index,AppCfgBindInfoNode** ppInfo) const {
	if (index>=m_listNodes.GetSize()) {
		return false;
	}
	int count = 0;
	POSITION pos = m_listNodes.GetHeadPosition();
	while (pos) {
		CString string;
		AppCfgBindInfoNode* const pTemp = m_listNodes.GetNext(pos);
		ALLEGE( 0!= pTemp);
		if (count == index) {
			*ppInfo = pTemp;
			return true;
		}
		count++;
	}
	return false;
}
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
AppCfgNodeInfo::AppCfgNodeInfo( ) : 
	m_logicalNodeAddress(0xff), m_masterAddress(0xff), m_numberOfUnits(0),
	m_physicalAddress(0xffffff), m_name(), m_productName( ), 
	m_softwareVersion(), m_nicSoftwareVersion( ), m_qtVersion( ), m_hardware( ) { }


void AppCfgNodeInfo::setLogicalNodeAddress(int v) {
	m_logicalNodeAddress = v;
}

void AppCfgNodeInfo::setMasterAddress(int v) {
	m_masterAddress = v;
}

void AppCfgNodeInfo::setNumberOfUnits(int v) {
	m_numberOfUnits = v;
}

void AppCfgNodeInfo::setPhysicalNodeAddress(int v) {
	m_physicalAddress = v;
}

void AppCfgNodeInfo::setName(const CString& s) {
	m_name = s;	
}

void AppCfgNodeInfo::setProductName(const CString& s) {
	m_productName = s;
}

void AppCfgNodeInfo::setSoftwareVersion(const CString& s) {
	m_softwareVersion = s;
}

void AppCfgNodeInfo::setNicSoftwareVersion(const CString& s) {
	m_nicSoftwareVersion = s;
}

void AppCfgNodeInfo::setQtVersion(const CString& version) {
	m_qtVersion = version;
}

void AppCfgNodeInfo::setHardware(const CString& v) {
	m_hardware = v;
}

void AppCfgNodeInfo::format(CStringList* dest) {	
	{
		CString str;
		str.Format("name = %s",m_name);
		dest->AddTail(str);
	}
	{
		CString str;
		str.Format("productName = %s",m_productName);
		dest->AddTail(str);
	}
	{
		CString str;
		str.Format("logicalAddress = 0x%02x",m_logicalNodeAddress);
		dest->AddTail(str);
	}
	{
		CString str;
		str.Format("physicalAddress = 0x%06x",m_physicalAddress);
		dest->AddTail(str);
	}
	{
		CString str;
		str.Format("masterAddress = 0x%02x",m_masterAddress);
		dest->AddTail(str);
	}
	{
		CString str;
		str.Format("numberOfUnits = %d",m_numberOfUnits);
		dest->AddTail(str);
	}
	{
		CString str;
		str.Format("softwareVersion = %s",m_softwareVersion);
		dest->AddTail(str);
	}
	{
		CString str;
		str.Format("softwareVersionNic = %s",m_nicSoftwareVersion);
		dest->AddTail(str);
	}
	{
		CString str;
		str.Format("hardware = %s", m_hardware);
		dest->AddTail(str);
	}
	{
		CString str;
		str.Format("qtversion = %s", m_qtVersion);
		dest->AddTail(str);
	}
}
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
AppCfgFileServerFile::AppCfgFileServerFile( ) : 
	m_type(TYPE_UNKNOWN), m_fileName(), m_size(0) { }

/**************************************************************/
/**************************************************************/
/**************************************************************/
AppCfgFileServerDir::AppCfgFileServerDir() : m_listFiles() {

}
AppCfgFileServerDir::~AppCfgFileServerDir( ) {
	list_deleteAll<AppCfgFileServerFile>(&m_listFiles);
}

void AppCfgFileServerDir::addFile(AppCfgFileServerFile* newNode) {
	REQUIRE(0 != newNode);
	m_listFiles.AddTail(newNode);
}
int AppCfgFileServerDir::getNrOfFiles( ) const {
	return m_listFiles.GetCount();
}
bool AppCfgFileServerDir::getFile(int idx,AppCfgFileServerFile** ppFile) {
	return list_getItem<AppCfgFileServerFile>(m_listFiles,idx,ppFile);
}


/**************************************************************/
/**************************************************************/
/**************************************************************/
static const char* createRandomString(char* strOut,int size) {      
	static const char* const charset = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
	const int lengthcharSet = strlen(charset);
	srand(static_cast<unsigned int>(time(NULL)));
    for(int i=0; i<size; ++i) {		   	
        strOut[i] = charset[rand() % lengthcharSet];
	}    
	return strOut;
}


AppCfgClientCredentials::AppCfgClientCredentials() :
	m_accessToken(), m_clientID( ), m_expiryTimeSec(0), m_refreshToken(),
	m_type( )
{
	char strRandomClient[20];	
	memset(strRandomClient,0,sizeof(strRandomClient));
	m_clientID = createRandomString(strRandomClient, sizeof(strRandomClient)-1);
}

void AppCfgClientCredentials::clear( ) {
	m_accessToken.Empty();
	m_expiryTimeSec = 0;
	m_refreshToken.Empty();
	m_type.Empty();
	m_clientID.Empty();

	char strRandomClient[20];	
	memset(strRandomClient,0,sizeof(strRandomClient));
	m_clientID = createRandomString(strRandomClient, sizeof(strRandomClient)-1);
}

void AppCfgClientCredentials::update(const AppCfgClientCredentials& r) {
	this->m_accessToken = r.m_accessToken;
	this->m_clientID = r.m_clientID;
	this->m_expiryTimeSec = r.m_expiryTimeSec;
	this->m_refreshToken = r.m_refreshToken;
	this->m_type = r.m_type;
}
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
AppCfgRemoteAction::~AppCfgRemoteAction() { }
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
AppCfgRemoteActionList::AppCfgRemoteActionList() : m_list() { }

AppCfgRemoteActionList::AppCfgRemoteActionList(const AppCfgRemoteActionList& refList) : m_list() {
	list_append(&this->m_list, refList.m_list);
}

AppCfgRemoteActionList::~AppCfgRemoteActionList() {
	deallocate();
}

void AppCfgRemoteActionList::deallocate() {
	list_deleteAll(&this->m_list);
}

void AppCfgRemoteActionList::add(AppCfgRemoteAction* item) {	
	m_list.AddTail(item);
}

int AppCfgRemoteActionList::getCount() const {
	return m_list.GetCount();
}

bool AppCfgRemoteActionList::get(int index, AppCfgRemoteAction** ppItem) const {
	return list_getItem(this->m_list, index, ppItem);
}

CList<AppCfgRemoteAction*>& AppCfgRemoteActionList::getList() {
	return m_list;
}
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
AppCfgRemoteActionWriteName::AppCfgRemoteActionWriteName() : Base(),
m_nodeAddress(-1), m_unitAddress(-1), m_szName(0) {	}

AppCfgRemoteActionWriteName::AppCfgRemoteActionWriteName(int nodeAddress, int unitAddress, const char* szName) :
	m_nodeAddress(nodeAddress), m_unitAddress(unitAddress), m_szName(0)
{
	setName(szName);
}

AppCfgRemoteActionWriteName::AppCfgRemoteActionWriteName(const AppCfgRemoteActionWriteName& r) : m_szName(0) {
	this->m_nodeAddress = r.m_nodeAddress;
	this->m_unitAddress = r.m_unitAddress;
	setName(r.getName());
}

AppCfgRemoteActionWriteName::~AppCfgRemoteActionWriteName() {
	deallocName();
}

AppCfgRemoteActionWriteName& AppCfgRemoteActionWriteName::operator=(const AppCfgRemoteActionWriteName& entry) {
	if (this != &entry) {
		this->m_nodeAddress = entry.m_nodeAddress;
		this->m_unitAddress = entry.m_unitAddress;
		this->setName(entry.m_szName);
	}
	return *this;
}

AppCfgRemoteAction* AppCfgRemoteActionWriteName::clone() const {

	AppCfgRemoteActionWriteName* pNewInstance = new AppCfgRemoteActionWriteName(*this);
	ALLEGE(0 != pNewInstance);
	return pNewInstance;
}

void AppCfgRemoteActionWriteName::clear() {
	m_nodeAddress = -1;
	m_unitAddress = -1;
	deallocName();
}

int AppCfgRemoteActionWriteName::getNodeAddress() const {
	return m_nodeAddress;
}
int AppCfgRemoteActionWriteName::getUnitAddress() const {
	return m_unitAddress;
}

const char* AppCfgRemoteActionWriteName::getName() const {
	return m_szName;
}

void AppCfgRemoteActionWriteName::deallocName() {
	if (0 != m_szName) {
		delete[] m_szName;
		m_szName = 0;
	}
}

void AppCfgRemoteActionWriteName::setName(const char* szName) {
	deallocName();
	if (0 != szName) {
		const int length = strlen(szName);
		if (length>0) {
			m_szName = new char[length + 1];
			strcpy(m_szName, szName);
		}
		else if (length == 0) {
			m_szName = new char[2];
			m_szName[0] = ' ';
			m_szName[1] = '\0';
		}
	}
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
AppCfgCmnProgressInfo::AppCfgCmnProgressInfo() : m_state(STATE_IDLE) { }
AppCfgCmnProgressInfo::~AppCfgCmnProgressInfo() { }

void AppCfgCmnProgressInfo::setState(State_t state) {
	m_state = state;
}
AppCfgCmnProgressInfo::State_t AppCfgCmnProgressInfo::getState() const {
	return m_state;
}

/**************************************************************/
/**************************************************************/
/**************************************************************/
AppRemoteCfgFirmwareVersion::AppRemoteCfgFirmwareVersion() :
	m_version(), m_refIpk(), m_productName(), m_platform(), m_release(false) { }

AppRemoteCfgFirmwareVersion::~AppRemoteCfgFirmwareVersion() { }

AppRemoteCfgFirmwareVersion* AppRemoteCfgFirmwareVersion::clone() {
	return new AppRemoteCfgFirmwareVersion(*this);
}
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
AppRemoteCfgFirmwareVersionInfo::AppRemoteCfgFirmwareVersionInfo() : m_list() {
}
AppRemoteCfgFirmwareVersionInfo::~AppRemoteCfgFirmwareVersionInfo() {
	deallocate();
}
void AppRemoteCfgFirmwareVersionInfo::deallocate() {
	list_deleteAll<AppRemoteCfgFirmwareVersion>(&m_list);
}

void AppRemoteCfgFirmwareVersionInfo::clear() {
	deallocate();
}
void AppRemoteCfgFirmwareVersionInfo::add(AppRemoteCfgFirmwareVersion* item) {
	m_list.AddTail(item);
}
int AppRemoteCfgFirmwareVersionInfo::getCount() const {
	return m_list.GetCount();
}
bool AppRemoteCfgFirmwareVersionInfo::get(int index, AppRemoteCfgFirmwareVersion** ppItem) const {
	return list_getItem<AppRemoteCfgFirmwareVersion>(this->m_list, index, ppItem);
}

AppRemoteCfgFirmwareVersionInfo& AppRemoteCfgFirmwareVersionInfo::operator=(const AppRemoteCfgFirmwareVersionInfo& l) {
	if (this == &l) {
		return *this;
	}	
	list_deleteAll(&this->m_list);
	list_append(&this->m_list, l.m_list);
	return *this;
}


/**************************************************************/
/**************************************************************/
/**************************************************************/

bool AppRemoteCfgFirmwareVersionInfo_Utils::isValid(const AppRemoteCfgFirmwareVersionInfo& r) {
	return r.getCount() > 0 ? true : false;
}

bool AppRemoteCfgFirmwareVersionInfo_Utils::find(const AppRemoteCfgFirmwareVersionInfo& r, const CString& productName, const CString& platform,
		bool releaseVersion, const AppRemoteCfgFirmwareVersion** ppVersion)
{
	const int count = r.getCount();
	for (int i=0;i<count;++i) {
		AppRemoteCfgFirmwareVersion* pItem = 0;
		if (r.get(i,&pItem)) {
			if ((releaseVersion == pItem->getFlagReleaseVersion()) && (0 == pItem->getPlatform().Compare(platform)) && (0 == pItem->getProductName().Compare(productName))) {
				*ppVersion = pItem;
				return true;
			}
		}	
	}
	return false;
}
/**************************************************************/
}; /* namespace DUOTECNO*/
/**************************************************************/