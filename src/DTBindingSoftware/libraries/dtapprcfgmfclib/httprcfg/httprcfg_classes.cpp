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
		dest->AddTail(new T(*pTemp));
	}	
}

template <class T>
bool list_getItem(CList<T*>& src,int idx,T** ppFile) {
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
/**************************************************************/
/**************************************************************/
AppCfgResult::AppCfgResult( ) : m_resultCode(RESULT_SUCCESS), 
	m_errorMsg( ), m_errDetailMsg()  { }


AppCfgResult::ResultCode_t AppCfgResult::getResultCode() const {
	return m_resultCode;
}
void AppCfgResult::setResultCode(ResultCode_t code) {
	m_resultCode = code;
}

const CString& AppCfgResult::getErrorMsg( ) const {
	return m_errorMsg;
}

void AppCfgResult::setErrorMsg(const char* msg) {
	m_errorMsg = CString(msg);
}

const CString& AppCfgResult::getErrorDetailMsg( ) const {
	return m_errDetailMsg;
}
void AppCfgResult::setErrorDaetailMsg(const char* msg) {
	m_errDetailMsg = CString(msg);
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
	m_softwareVersion(), m_nicSoftwareVersion( ) { }


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
}; /* namespace DUOTECNO*/
/**************************************************************/