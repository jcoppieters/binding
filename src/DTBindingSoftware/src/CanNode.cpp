// CanNode.cpp: implementation of the CCanNode class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "CanNode.h"

#include "NodeRuntimeStatus.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CCanNode::CCanNode( ) : 
	ulPhysicalAddress(0xFFFFFF), bLogicalAddress(0xFF)
{
	this->init();
}

CCanNode::CCanNode(unsigned long physicalAddress) : 
	ulPhysicalAddress(physicalAddress), bLogicalAddress(0xFF)
{
	this->init();
}

// BUG-0173: Oplossing.
// Het node address meegegeven in de ctor werd niet toegekend aan de var bLogicalAddress
CCanNode::CCanNode(BYTE bNodeAddress) : 
	ulPhysicalAddress(0xffffff), bLogicalAddress(bNodeAddress)
{
	this->init();
}

void CCanNode::init()
{
	// Logical & physical address in de ctor.
	memset(szNodeName,0,sizeof(szNodeName));
	bNodeType = NODE_TYPE_UNDEFINED;
	bNodeFlags = NODE_FLAGS_UNDEFINED;
	bDLLAccessControl = 0xff;
	bNROfUnits = 0;
	pvData = 0;
	pNextNode = 0;
	pFirstUnit = 0;

	ptrBindingFile = 0;
	pRuntimeStatus = 0;
	iNROfUnits = 0;
	m_nrOfBindingEntries = 0;
	m_bindingXORChecksum = 0;

	ulApplicationFlags = 0;

	ulReserved[0]=0;
	ulReserved[1]=0;
	ulReserved[2]=0;
	ulReserved[3]=0;
	ulReserved[4]=0;
	ulReserved[5]=0;

	// sedert versie 0x0700:
	memset(pbResourcesBlock0,0,sizeof(pbResourcesBlock0));
	memset(pbResourcesBlock1,0,sizeof(pbResourcesBlock1));
	memset(pbResourcesBlock2,0,sizeof(pbResourcesBlock2));
	memset(pbResourcesBlock3,0,sizeof(pbResourcesBlock3));
	memset(pbResourcesBlock4,0,sizeof(pbResourcesBlock4));
}

CCanNode::~CCanNode()
{
	if (pvData != 0) {
		delete pvData;
		pvData = 0;
	}

	if (pRuntimeStatus != 0) {
		delete pRuntimeStatus;
		pRuntimeStatus = 0;
	}
}

void CCanNode::AllocateRuntimeStatus() {
	if (NULL == pRuntimeStatus) {
		if (bNodeType == NODE_PIN_GATEWAY) {
			pRuntimeStatus = new CNodeGatewayRuntimeStatus();
		}
		else {
			pRuntimeStatus = new CNodeRuntimeStatus();
		}
	}
	ENSURE(NULL != pRuntimeStatus);
}

void CCanNode::CopyRuntimeStatus(CCanNode* pSourceNode)
{
	ASSERT( NULL != pSourceNode );
	ASSERT( NULL != this->pRuntimeStatus);
	if (pRuntimeStatus != 0 && pSourceNode->pRuntimeStatus) {
		pRuntimeStatus->Copy(pSourceNode->pRuntimeStatus);
	}
}

//////////////////////////////////////////////////////////////////////

// Sedert V17.00: Uitbreiding van de bindings.
// pbResourcesBlock4 bevat de MSB waarden.
unsigned short CCanNode::MaxReferences() const {
	return (pbResourcesBlock4[RESOURCE_NR_OF_REFERENCES_MSB-1]<<8) + pbResourcesBlock0[RESOURCE_NR_OF_REFERENCES_LSB-1]; 
}

BYTE CCanNode::MaxTimerOperators(void) const {
	return pbResourcesBlock0[RESOURCE_NR_OF_TIMER_OPERATORS-1]; 
}

BYTE CCanNode::MaxPacketLength(void) const {
	return pbResourcesBlock0[RESOURCE_MAX_PACKET_DATA_LENGTH-1]; 
}

// Sedert V17.00: Uitbreiding van de bindings.
// pbResourcesBlock4 bevat de MSB waarden.
unsigned short CCanNode::MaxBindingEntries(void) const {
	return (pbResourcesBlock4[RESOURCE_NR_OF_BINDING_ENTRIES_MSB-1]<<8) + pbResourcesBlock1[RESOURCE_NR_OF_BINDING_ENTRIES_LSB-1]; 
}


// Sedert V17.00: Uitbreiding van de bindings.
// pbResourcesBlock4 bevat de MSB waarden.
unsigned short CCanNode::MaxConditions(void) const {
	return (pbResourcesBlock4[RESOURCE_MAX_CONDITIONS_MSB-1]<<8) + pbResourcesBlock1[RESOURCE_MAX_CONDITIONS_LSB-1]; 
}

unsigned short CCanNode::MaxTempBuffer(void) const
{
	const unsigned short usTemp = 
		(pbResourcesBlock1[RESOURCE_BINDING_BUFFER_LENGTH_MSB-1] * 256) +
		 pbResourcesBlock1[RESOURCE_BINDING_BUFFER_LENGTH_LSB-1];

	return usTemp;
}

BYTE CCanNode::MaxConditionLength(void) const
{
	return pbResourcesBlock1[RESOURCE_MAX_CONDITION_LENGTH-1]; 
}

BYTE CCanNode::MaxNameLength(void) const
{
	return pbResourcesBlock2[RESOURCE_NR_OBJECT_NAME_LENGTH-1];
}

BYTE CCanNode::GetSoftwareVersion(void) const
{
	return pbResourcesBlock2[RESOURCE_NR_SOFTWARE_VERSION-1];
}

BYTE CCanNode::GetNICMedium(void) const
{
	return pbResourcesBlock2[RESOURCE_NR_MEDIUM_TYPE-1];
}

// Since v0700
BYTE CCanNode::GetExtraResourceBlocks(void) const
{
	return pbResourcesBlock2[RESOURCE_NR_EXTRA_BLOCKS-1];
}

BYTE CCanNode::GetProductID(void) const
{
	return pbResourcesBlock3[RESOURCE_PRODUCT_ID-1];
}

// Sedert V16.29: Extra vlaggen per product.
BYTE CCanNode::GetProductExtraFlags(void) const {
	return pbResourcesBlock3[RESOURCE_PRODUCT_FLAGS-1];
}		

BYTE CCanNode::GetSoftwareVersionRev(void) const
{
	return pbResourcesBlock3[RESOURCE_SOFTWARE_VERSION_REV-1];
}

BYTE CCanNode::GetProtocolVersion(void) const
{
	return pbResourcesBlock3[RESOURCE_PROTOCOL_VERSION-1];
}

BYTE CCanNode::GetProtocolVersionRev(void) const
{
	return pbResourcesBlock3[RESOURCE_PROTOCOL_VERSION_REV-1];
}

/********************************************************************************/
/********************************************************************************/
/********************************************************************************/
IAppUnitData::~IAppUnitData() { }


/********************************************************************************/
CAppUnitDataDali::CAppUnitDataDali() : m_daliAddress(-1), m_daliType(DALI_UNKNOWN) { }
CAppUnitDataDali::~CAppUnitDataDali( ) { }

CAppUnitDataDali::UnitDataType_t CAppUnitDataDali::getUnitDataType( ) const {
	return TYPE_UNIT_DATA_DALI;
}

void CAppUnitDataDali::setDaliAddress(int value) {
	this->m_daliAddress = value;
}

int CAppUnitDataDali::getDaliAddress( ) const {
	return this->m_daliAddress;
}

void CAppUnitDataDali::setDaliType(DaliType_t type) {
	this->m_daliType = type;
}

CAppUnitDataDali::DaliType_t CAppUnitDataDali::getDaliType( ) const {
	return this->m_daliType;
}

IAppUnitData* CAppUnitDataDali::clone( ) {
	CAppUnitDataDali* const p = new CAppUnitDataDali( );
	if (NULL == p) {
		ASSERT(0);
		return NULL;
	}
	p->m_daliAddress = this->m_daliAddress;
	p->m_daliType = this->m_daliType;
	return p;
}
/********************************************************************************/