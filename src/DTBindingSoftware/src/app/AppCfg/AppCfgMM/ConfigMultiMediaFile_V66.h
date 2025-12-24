#pragma once
#include "configmultimediafile.h"

class CConfigMultiMediaFile_V66 :
	public CConfigMultiMediaFile
{
public:
	CConfigMultiMediaFile_V66(void);
	virtual ~CConfigMultiMediaFile_V66(void);
	
	virtual BOOL Load(CString szFilename);
	virtual BOOL Initialise(void);		
	virtual CConfigExternalClass* GetClass(int type);

protected:
	CConfigExternalFile* m_pConfigFile;
	CConfigExternalClass **m_ppConfigBaseClass;	

private:
	void allocateMemory( );
	void destroyMemory( );
	void initData( );

	typedef CConfigMultiMediaFile Base;
};
