#pragma once

#ifdef __cplusplus
#   define EXTERN_C     extern "C"
#else
#   define EXTERN_C
#endif // __cplusplus


#ifdef __cplusplus
namespace DUOTECNO {
#endif


struct IDTDevice
{
	virtual ~IDTDevice( )							= 0;
	virtual const char* getDeviceName(void)	const	= 0;
	virtual const BYTE* getMACAddress(void)	const	= 0;
	virtual int getDHCPOnOff(void) const			= 0;
	virtual const char* getIPAddress(void) const	= 0;
	virtual const char* getNetmask(void) const		= 0;
	virtual const char* getGateway(void) const		= 0;
	virtual int getPhysicalNodeAddress( ) const		= 0;		// sedert V4.0 
    virtual int getNodeLogicalAddress() const		= 0;		// sedert V4.0 
    virtual int getSoftwareVersion() const			= 0;		// sedert V4.0 
    virtual int getProductID( ) const				= 0;		// sedert V4.0 
};

struct IDTDeviceIterator
{
	virtual ~IDTDeviceIterator( )				= 0;
	virtual void begin( )						= 0;
	virtual BOOL end( )							= 0;
	virtual void next( )						= 0;
	virtual IDTDevice* currentItem( )			= 0;
};

struct IDTDeviceList
{
	virtual ~IDTDeviceList( )							= 0;	
	virtual int count(void) const						= 0;
	virtual IDTDeviceIterator* createIterator(void)		= 0;
	virtual void destroyIterator(IDTDeviceIterator* p)	= 0;
};


#ifdef __cplusplus
};	// end namespace DUOTECNO
#endif