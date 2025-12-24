#pragma once

class CNodeDatabase;
class CCanNode;

class CAppLicActCodeNodeDbase
{
public:
	/**
	 * @brief	Controle of er een licentie code nodig is.
	 * @param	pNodeDatabase [IN] ptr naar nodedatabase.
	 * @return	TRUE = Ja nodig.
	 */
	static BOOL CheckLicActCodeNeeded(
		CNodeDatabase *pNodeDatabase);

	/**
	 * @brief	Zoeken naar de nodes die een licentie code nodig hebben.
 	 * @param	pNodeDatabase [IN] ptr naar nodedatabase.
	 * @param	ppNodes [OUT] Array van ptr naar nodes die een code nodig hebben.
 	 * @param	maxElements [IN] Max aantal die ppNodes array kan bijhouden.
	 * @param	Aantal nodes die een code nodig hebben.
	 */
	static int FindNodesLicActCodeNeeded(
		CNodeDatabase *pNodeDatabase,
		CCanNode** ppNodes,
		int maxElements);

	/**
	 * @brief	Controle of de licentie code correct is.
  	 * @param	pNodeDatabase [IN] ptr naar nodedatabase.
  	 * @param	pNodeDatabase [IN] ptr naar nodedatabase.
	 * @param	ulCode [IN] de code die moet gecontroleerd worden.
	 * @return	TRUE = Code ok.
	 */
	static BOOL CheckLicenseCodeValid(
		CNodeDatabase *pNodeDatabase,
		unsigned long ulCode);
};