#pragma once

class CCanNode;
struct STRUCT_UNIT;


struct CCfgInfoUnit
{
public:
	typedef enum
	{
		FLAG_SUPPORTED_BINDINGS_FORWARD		= (1<<0),

	} CfgInfoUnitFlags_t;

public:
	static BOOL isSupported(const CCanNode* pNode,const STRUCT_UNIT* pUnit,CfgInfoUnitFlags_t flag);

}; /* CCfgInfoUnit */


struct CfgInfoUnitDim
{
public:
	static unsigned short getSupportedFlags(
		const CCanNode* pNode,
		const STRUCT_UNIT* pUnit);

}; /* CfgInfoUnitDim */



// Oppassen: bindingpropertyunitalg_upgrade
// In de bindingpropertyunitalg_upgrade( ) worden de supported vlaggen upgedate na 
// opnieuw te browsen. Hierdoor worden onderstaande mogelijks overschreven.
struct CfgInfoUnitSens
{
public:
	typedef enum
	{
		TYPE_SENSOR_NORMAL = 0,
		TYPE_SENSOR_UNIVERSAL

	} Type_t;

public:
	static unsigned short getSupportedFlags(const CCanNode* pNode,const STRUCT_UNIT* pUnit);	
	static Type_t getType(const CCanNode* pNode,const STRUCT_UNIT* pUnit);	

protected:

	// Oppassen: Onderstaande functies worden aangeroepen nadat het product type & 
	// sensor unit type: De gewone of de universele reeds gecontroleerd werden.
	static BOOL getSoftwareVersionSupportSetpointRange(BYTE bSoftwareVersion,BYTE bSoftwareVersionRev);
	static BOOL getSoftwareVersionSupportPIDControl(BYTE bSoftwareVersion,BYTE bSoftwareVersionRev);
}; /* CfgInfoUnitSens */


struct CfgInfoUnitDuoswitch
{
public:
	typedef enum
	{
		FLAG_SUPPORTED_STOPTIME_NONE			= (1<<0),
		FLAG_SUPPORTED_STOPTIME_LONGERVALUES	= (1<<1),

	} CfgInfoUnitFlags_t;

	static unsigned short getSupportedFlags(
		const CCanNode* pNode,
		const STRUCT_UNIT* pUnit);	
	
}; /* CfgInfoUnitDuoswitch */


struct CfgInfoUnitAVMatrix
{
	typedef enum {
		// Config: Subtype: bvb audac mtx, audac m2, ...
		FLAG_SUPPORTED_CONFIG_SUBTYPE	= (1<<0)

	} CfgInfoUnitFlags_t;

	typedef enum {
		TYPE_AVMATRIX_UNKNOWN	= 0x00,
		TYPE_AVMATRIX_AUDAC		= 0x01,
		TYPE_AVMATRIX_INTEGRA	= 0x02
	} Type_t;

public:
	static unsigned short getSupportedFlags(const CCanNode* pNode,const STRUCT_UNIT* pUnit);	
	static Type_t getType(const CCanNode* pNode,const STRUCT_UNIT* pUnit);
	
}; /* CfgInfoUnitAVMatrix */