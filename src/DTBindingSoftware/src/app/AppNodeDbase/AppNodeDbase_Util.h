#pragma once

// Voorwaartse declaratie.
class CNodeDatabase;
class CCanNode;

// Klasse.
class CAppNodeDbase_Util
{
public:
	/**
	 * Opzoeken van nodes van een bepaald product code.
	 * 
	 * @param	pNodeDatabase [IN] ptr naar nodedatabase
	 * @param	productID [IN] Product id die gezocht wordt.
	 * @param	ppNodes [OUT] ptr naar een array van nodes.
	 * @param	maxElements [IN] max aantal elementen in ppNodes array
	 *
	 * @return	Aantal nodes in array.
	 */
	static int FindNodesWithProductID(
		CNodeDatabase *pNodeDatabase,
		int productID,
		CCanNode** ppNodes,
		int maxElements);


	static int getCountNodesWithProductID(
		CNodeDatabase *pNodeDatabase,
		int productID);
};
