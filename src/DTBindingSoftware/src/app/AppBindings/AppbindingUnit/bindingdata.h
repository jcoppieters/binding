#pragma once

class CBindingData
{
private:
	BYTE bMessageCode;
	BYTE bData[6];
	BYTE bLength;
	BYTE bBindingFlags;

public:
	void Set_MessageCode(BYTE bCode) {
		bMessageCode = bCode;		
	};
	BYTE Get_MessageCode(void) const {
		return bMessageCode;		
	};
	void Set_Length(BYTE bLen) {
		bLength = bLen;			
	};
	BYTE Get_Length(void) const {
		return bLength;			
	};
	void Set_BindingFlags(BYTE bFlags) {
		bBindingFlags = bFlags;	
	};
	BYTE Get_BindingFlags(void)	const {
		return bBindingFlags;		
	};
	void Set_MethodData(BYTE data) {
		bData[0] = data;			
	};
	BYTE Get_MethodData(void) const	{
		return bData[0];			
	};
	void Set_Data(BYTE Index,BYTE data)	{
		ASSERT(Index<6); 
		bData[Index] = data; 
	};	
	BYTE Get_Data(BYTE Index) const {
		ASSERT(Index<6);
		return (bData[Index]); 
	};

public:	

	/* Operations */
	void Clear(void);
	BOOL IsEqual(const CBindingData* const pSource) const;
};
