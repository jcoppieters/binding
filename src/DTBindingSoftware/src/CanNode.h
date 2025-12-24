// CanNode.h: interface for the CCanNode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CANNODE_H__5D3067C1_3D24_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_CANNODE_H__5D3067C1_3D24_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/****************************************************************/

#include "nodemess.h"
#include "nodegenestructs.h"

class CBindingFile;
class CNodeRuntimeStatus;

struct IAppUnitData;

/**************************************************************/
#define LCD_TSLOCKED 0x02
#define LCD_LOCKED   0x01
#define LCD_UNLOCKED 0x00

#define PM_FATAL_ERROR 0
#define PM_WARNING 1
#define PM_ERROR 2

/* name length */
#define UNIT_NAME_LENGTH 16
#define NODE_NAME_LENGTH 16

/* ******************** Reset Modes ***************************/
#define P0WER_ON_RESET1 0x02
#define P0WER_ON_RESET2 0x03
#define WATCHDOG_RESET  0x00
#define EXTERNAL_RESET  0x01
#define NODERESET       0x04
#define UNIT_SOFTWARE_RESET 0x05


//////////////////////////////////////////////////////////////////////

/* ************************ EV_NODEDLLSTATUS ***************************/
#define DLL_STATUS_BLOCK0_LENGTH 5
#define DLL_STATUS_BLOCK1_LENGTH 5

/* ************************ EV_NODERESOURCES ***************************/
#define RESOURCES_BLOCK0_LENGTH 4
#define RESOURCES_BLOCK1_LENGTH 5
#define RESOURCES_BLOCK2_LENGTH 4		// versie 0x0700 - was 3, verhoogd naar 4
#define RESOURCES_BLOCK3_LENGTH	5		// sedert versie 0x0700
#define RESOURCES_BLOCK4_LENGTH 5		// sedert versie 0x0700

#define NODE_NAME_LENGTH 16
//////////////////////////////////////////////////////////////////////
//typedef struct STRUCT_UNIT STRUCT_UNIT;

/////////////////////////////// UNION_UNITSPECS //////////////////////////////
#define BOSE_4ROOMS_VOLUME_INDEX 0

// TM_CHANGES_ALARM_ARITECH:
// AlarmUnit toegevoegd aan de unitspecs: 
// Doordat het een union is en de UNIT_ALARM_STRUCT niet de grootste is blijft deze 
// union compatible.

// TM_AVMATRIX_INPUT
// AVMatrix Unit toegevoegd aan de unitspecs.
// Union Blijft compatible

typedef union
{
   UNIT_DIM_STRUCT DimmerUnit;
   UNIT_SWITCH_STRUCT RelayUnit;
   UNIT_DUOSWITCH_STRUCT DuoSwitchUnit;
   UNIT_CONTROL_STRUCT ControlUnit;			
   UNIT_SENS_STRUCT SensorUnit;
   UNIT_AUDIO_STRUCT AudioUnit;
   UNIT_RC5RX_STRUCT RC5RxUnit;
   UNIT_ALARM_STRUCT AlarmUnit;				// TM_CHANGES_ALARM_ARITECH
   UNIT_AVMATRIX_STRUCT AVMatrixUnit;		// TM_AVMATRIX_INPUT
   
} UNION_UNITSPECS;


/////////////////////////////// STRUCT_UNIT ////////////////////////////////
class CBindingPropertyUnit;

#define DEFAULT_NAME_OVERRIDDEN		0x01


// Deze unit wordt aangemaakt door de binding software om forward bindings te maken.
// Dit is nodig bij de Bose audio nodes.
enum {
	UNIT_VIRTUAL_BROADCAST_ADDRESS = 0xff
};


struct STRUCT_UNIT
{
   // unit specific stuff 
   char szUnitName[UNIT_NAME_LENGTH];
   BYTE bNodeLogicalAddress;
   BYTE bUnitAddress;
   BYTE bUnitType;
   BYTE bUnitFlags;
   STRUCT_UNIT_STATUS UnitStatus;   
   UNION_UNITSPECS UnitSpecs;		
   CBindingPropertyUnit *m_pBindingUnit;   
   unsigned long ulApplicationFlags;	// database specific stuff 
   IAppUnitData* m_pAppUnitData;
   STRUCT_UNIT *pNextUnit;
};   




/*******************************************************************//**
 * @class	CCanNode
 * @brief	Een can Node.
 * @author	Mitchell Tom
 * @changes	TM,v0562: Inline functies.
 * @changes	TM,v0562: NODE_BINDINGFILE_MODIFIED_FLAG als enum ipv macro.
 *
 * @todo	geen publieke variabelen, toegang via inline functies.
 **********************************************************************/
class CCanNode  
{
public:
	enum { NODE_BINDINGFILE_MODIFIED_FLAG = 0x01 }; // TM 20041115: UlApplicationFlags

	enum {
		NODE_TYPE_UNDEFINED = 0xff,
		NODE_FLAGS_UNDEFINED = 0xff,
	};

	CCanNode();
	CCanNode(unsigned long ulPhysicalAddress);
	CCanNode(BYTE bNodeAddress);
	virtual ~CCanNode();

	unsigned short MaxReferences(void) const;								
	BYTE MaxTimerOperators(void) const;
	BYTE MaxPacketLength(void) const;
	unsigned short MaxBindingEntries(void) const;							
	unsigned short MaxConditions(void) const;								
	unsigned short MaxTempBuffer(void) const;
	BYTE MaxConditionLength(void) const;
	BYTE MaxNameLength(void) const;
	BYTE GetSoftwareVersion(void) const;
	BYTE GetNICMedium(void) const;

	BYTE getNodeBindingsXORChecksum() const;
	void setNodeBindingsXORChecksum(BYTE i);
	unsigned short getNodeBindingsNrEntries() const;
	void setNodeBindingsNrEntries(unsigned short NEntries);


	CBindingFile* GetBindingFilePtr();
	void SetBindingFilePtr(CBindingFile* ptr);
	CNodeRuntimeStatus* GetRuntimeStatus();		
	void AllocateRuntimeStatus();
	void CopyRuntimeStatus(CCanNode* pSourceNode);

	// Since v0700
	BYTE GetExtraResourceBlocks(void) const;
	BYTE GetProductID(void) const;
	BYTE GetProductExtraFlags(void) const;			// Sedert V16.29
	BYTE GetSoftwareVersionRev(void) const;
	BYTE GetProtocolVersion(void) const;
	BYTE GetProtocolVersionRev(void) const;

	// since v0562
	BYTE getNROfUnits( ) const;
	void setNROfUnits(BYTE bNrOfUnits);
	BYTE getNodeAddress( ) const;
	void setNodeAddress(BYTE bNodeAddress);
	unsigned long getPhysicalAddress( ) const;
	void setPhysicalAddress(unsigned long nodeAddress);
	const char* getNodeName( ) const { return szNodeName; }
	BYTE getNodeType( ) const { return bNodeType; };

	// todo	AANVULLEN

public:
	/* Node specific stuff */
	char szNodeName[NODE_NAME_LENGTH];
	unsigned long ulPhysicalAddress;
	BYTE bLogicalAddress;
	BYTE bNodeType;
	BYTE bNodeFlags;
	BYTE bDLLAccessControl;
	BYTE bNROfUnits; 
	BYTE pbResourcesBlock0[RESOURCES_BLOCK0_LENGTH]; 
	BYTE pbResourcesBlock1[RESOURCES_BLOCK1_LENGTH]; 
	BYTE pbResourcesBlock2[RESOURCES_BLOCK2_LENGTH]; 
	BYTE pbResourcesBlock3[RESOURCES_BLOCK3_LENGTH];	// Sedert v0x0700
	BYTE pbResourcesBlock4[RESOURCES_BLOCK4_LENGTH];	// Sedert v0x0700
	int iNROfUnits;							/* Gecreërde units in VC++ appl*/
	void *pvData;							/* to assign to specific data */
	CCanNode *pNextNode;
	STRUCT_UNIT *pFirstUnit;   
	unsigned long ulReserved[6];
	unsigned long ulApplicationFlags;		/* Database specific stuff */

private:
	void init();

private:
	CNodeRuntimeStatus* pRuntimeStatus;
	BYTE m_bindingXORChecksum;					//NEW !!!!!!!!!!!!!
	unsigned short m_nrOfBindingEntries;
	CBindingFile *ptrBindingFile;
};
/**************************** inline functions **********************************/
inline BYTE CCanNode::getNROfUnits( ) const
{
	return bNROfUnits;
}
inline void CCanNode::setNROfUnits(BYTE bNrOfUnits)
{
	this->bNROfUnits = bNrOfUnits;
}
inline BYTE CCanNode::getNodeAddress( ) const
{
	return bLogicalAddress;
}
inline void CCanNode::setNodeAddress(BYTE bNodeAddress)
{
	this->bLogicalAddress = bNodeAddress;
}
inline unsigned long CCanNode::getPhysicalAddress( ) const
{
	return ulPhysicalAddress;
}
inline void CCanNode::setPhysicalAddress(unsigned long nodeAddress)
{
	this->ulPhysicalAddress = nodeAddress;
}

inline BYTE CCanNode::getNodeBindingsXORChecksum() const { 
	return m_bindingXORChecksum; 
}	

inline void CCanNode::setNodeBindingsXORChecksum(BYTE i)  {
	m_bindingXORChecksum = i; 
}

inline unsigned short CCanNode::getNodeBindingsNrEntries() const { 
	return m_nrOfBindingEntries; 
}
inline void CCanNode::setNodeBindingsNrEntries(unsigned short NEntries)  { 
	m_nrOfBindingEntries = NEntries; 
}

inline CBindingFile* CCanNode::GetBindingFilePtr() 
{
	return ptrBindingFile; 
};	
inline void CCanNode::SetBindingFilePtr(CBindingFile* ptr) 
{
	ptrBindingFile = ptr; 
};
inline CNodeRuntimeStatus* CCanNode::GetRuntimeStatus() 
{
	return pRuntimeStatus;
};
/********************************************************************************/

/********************************************************************************/
/********************************************************************************/
/********************************************************************************/
struct IAppUnitData {
	typedef enum {
		TYPE_UNIT_DATA_UNKNOWN = -1,
		TYPE_UNIT_DATA_DALI = 0	
	} UnitDataType_t;

	virtual ~IAppUnitData() = 0;
	virtual UnitDataType_t getUnitDataType( ) const = 0;
	virtual IAppUnitData* clone( ) = 0;
};
/********************************************************************************/


/********************************************************************************/
/********************************************************************************/
/********************************************************************************/
class CAppUnitDataDali : public IAppUnitData {
public:
	typedef enum {
		DALI_UNKNOWN = -1,
		DALI_DEVICE_CONTROLGEAR = 0,
		DALI_DEVICE_INPUTDEVICE,
		DALI_GROUP_CONTROLGEAR,
		DALI_SCENE
	} DaliType_t;

	CAppUnitDataDali( );
	virtual ~CAppUnitDataDali( );
	virtual UnitDataType_t getUnitDataType( ) const;

	void setDaliAddress(int value);
	int getDaliAddress( ) const;

	void setDaliType(DaliType_t type);
	DaliType_t getDaliType( ) const;

	virtual IAppUnitData* clone( );

private:
	int m_daliAddress;
	DaliType_t m_daliType;
};
/********************************************************************************/


/********************************************************************************/
#endif // !defined(AFX_CANNODE_H__5D3067C1_3D24_11D8_B865_0050BAC412B1__INCLUDED_)
