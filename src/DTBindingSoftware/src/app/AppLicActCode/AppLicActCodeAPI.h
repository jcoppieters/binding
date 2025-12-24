#pragma once

class CNodeDatabase;
class CCanNode;

class CAppLicActCodeAPI
{
public:
	/**
	 * @brief	Opvragen of een licentie activatie code nodig is.
	 * @return	TRUE = ja.
	 **/
	static BOOL IsLicenseNeeded(void);

	/**
	 * @brief	Licentie kan geactiveerd worden.
	 * @return	None
	 **/
	static void SetLicenseActivated(void);

	/**
	 * @brief	Opvragen of licentie geactiveerd is.
 	 * @return	TRUE = ja.
	 */
	static BOOL GetLicenseActivated(void);

	/**
	 * @brief	Controle of dit een geldige licentie code is.
	 * @param	ulCode [IN] licentie code.
 	 * @return	TRUE = ja.
	 */
	static BOOL CheckLicenseCodeValid(
		unsigned long ulCode);

	/**
	 * @brief	Nodes zoeken waarvoor een licentie code nodig is.
	 * @param	ppNodes [OUT] array van pointers naar CCanNode.
	 * @param	maxElements [IN] Max aantal elementen die array kan bevatten.
 	 * @return	aantal gevonden nodes 
	 */
	static int FindNodesLicActCodeNeeded(
		CCanNode** ppNodes,
		int maxElements);
};
