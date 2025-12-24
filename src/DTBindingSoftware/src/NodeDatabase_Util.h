#pragma once

class CNodeDatabase;
class CCanNode;

struct CNodeDatabase_Util
{
	// Return waarde TRUE wanneer een node in de database aanwezig is.
	static BOOL isValid(CNodeDatabase& refNodeDatabase,CCanNode* pNode);

	// Zoeken in de node database naar een twilight module.
	// return waarde: aantal gevonden.
	// listAddresses is een lijst met de gevonden node addresses.
	static int findNodeTwilight(CNodeDatabase& refNodeDatabase,CList<int>* listAddresses);
};
