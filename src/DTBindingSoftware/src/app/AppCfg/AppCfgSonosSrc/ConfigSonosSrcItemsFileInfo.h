#pragma once

////////////////////////////////////////////////

// Oppassen:
// In deze implementatie wordt het bestand niet ingelezen 
// en wordt er geen verify() uitgevoerd.
//
// Het cfg bestand is een ASCII bestand die als *.tmp verstuurd
// wordt. Het embedded device copieert het *.tmp naar *.dat 
// bestand.

class CConfigSonosSrcItemsFileInfo
{
public:
	static CConfigSonosSrcItemsFileInfo& getInstance( );

protected:
	CConfigSonosSrcItemsFileInfo();

public:
	virtual ~CConfigSonosSrcItemsFileInfo();
	CString GetFullPathName(BYTE bNodeAddress) const;
	BOOL Initialise(void);
	BOOL load( );
	BOOL isLoaded( ) const;
};

////////////////////////////////////////////////