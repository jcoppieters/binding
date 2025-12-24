// appnodedatabase.h: interface for the CAppNodeDatabase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NODEDATABASEAPPLICATION_H__D6BC9E94_23FC_41D5_ACB3_0857A3F8CD6C__INCLUDED_)
#define AFX_NODEDATABASEAPPLICATION_H__D6BC9E94_23FC_41D5_ACB3_0857A3F8CD6C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NodeDatabase.h"
#include "app\appstate\appstate.h"


/****************************************************************//**
 * @class	CAppNodeDatabase
 * @brief	Wrapper voor nodeDatabase application operations.
 * 
 * @author	Mitchell Tom.
 *
 * @todo	Format a list of:\n
 *			Incompatible nodes, Name differences,...\n
 *			Handle all the work of the CPropertyWizard dialogs !!
 *
 * @changes	TM,v0562: CreateBindingUnitProperties
 * @changes TM,v0562: findIncompatibleUnits
 * @changes	TM,v0562: possibleToUpgradeIncompatibleUnits
 *******************************************************************/
class CAppNodeDatabase : public CNodeDatabase  
{
public:
	static CAppNodeDatabase* GetInstance();
	virtual ~CAppNodeDatabase();

	CNodeDatabase* GetNodesDocument();
	CNodeDatabase* GetNodesRuntime();


	/**
	 * @brief	Er zijn 0xFE addressen gevonden.
	 *			Na een Physical browse controleren of er een master aanwezig is.	
	 *			Wanneer er een master aanwezig is dan gaan we de fout afhandelen als 0xFE addressen.			
	 *
	 *			Wanneer er nog geen master aanwezig is en er is een touchscreen aanwezig dan voorstellen 
	 *			om deze als master in te stellen.
	 *
	 *			Wanneer er nog geen master aanwezig is en er worden sbox ultra modules gevonden worden
	 *			Mogelijkheid geven om deze als master te plaatsen.
	 *		    OPPASSEN: Smartbox ultra wordt gedetecteerd door Node type Gateway + fysisch adres.
	 *
	 *			Controle gebeurd in de runtime node database.
	 *
	 * @since	V16.44	 
	 */
	BOOL runtimeNodeDatabase_isMasterPresent( );
	BOOL runtimeNodeDatabase_foundTouchscreenNodesWithFactoryAddress(CList<int>& list);
	BOOL runtimeNodeDatabase_foundSmartboxUltraNodesWithFactoryAddresses(CList<int>& list);

	/**************************************************************************************************/


	/**
	 * @brief	Enkel mogelijk voor LCD nodes zonder audio units.
	 * @since	v0562
	 */
	BOOL possibleToUpgradeIncompatibleUnits(BYTE bNodeAddress);


	/**
	 * @brief	Mogelijkheid om incompactibiliteit van de nodes op te lossen via de makeCompatible
	 *			Als er 1 probleem tussen zit die niet kan opgelost worden dan is de returnwaarde FALSE !
	 *			return waarde:
	 *			 TRUE: Alle problemen kunnen opgelost worden door de makeCompatible optie.
	 *			 FALSE: Niet alle problemen " " " "" " 
	 *			 
	 * @since	V16.16
	 */ 
	BOOL possibleToMakeDbaseNodesCompatible( );


	/**
	 * @brief	Mogelijkheid om het probleem op te lossen door modules te vervangen ?
	 * @since V16.16.
	 */
	BOOL possibleToReplaceNodes( );


	/**
	 * @brief	Mogelijkheid om het probleem op te lossen door de address van de database 
	 *			naar de netwerk nodes door te sturen.
	 * @since	V16.16
	 */
	BOOL possibleToResolveAddressFromDbase( );


	/**
	 * @brief	Bij programmatie verschillen bepalen of het laatste bestand hebben.
	 *			Controle doen aan de hand van de naam van het master address.
	 * @since	V16.16
	 */
	typedef enum {
		RESULT_UNKNOWN = 0,					// geen idee - Namen gelijk of geen datum.
		RESULT_DATABASE_RECENT,				// Naam van de master in de database (=datum) is recenter.
		RESULT_NETWORK_RECENT				// Naam van de master in het netwerk (=datum) is recenter.

	} ProgramFileDiffResult_t;

	ProgramFileDiffResult_t findTypeProgramFileDifference(CString& dateDatabase, CString& dateNetwork);


	/**
	 * @brief	Copy names from network to pcDatabase
	 */
	void copyNamesFromNetwork( );
	void copyNamesFromNetwork(CCanNode *pDocNode,CCanNode *pRTNode);


	/**
	 * @since	v0562
	 */
	BOOL replaceIncompatibleNodes(CCanNode* pNodeDoc, CCanNode* pNodeNetwork);

	
	/**************************************************************//**
     * @brief	Incompatible units zoeken.
	 * @param	pNodeDoc: [IN] pointer naar doc node.
	 * @param	pNodeNetwork: [IN] pointer naar network node.
	 * @param	pArrUnits: [OUT] pointer naar array	
	 * @param	maxLength: [IN] max elementen in array
	 * @return	Aantal gevonden elementen = aantal in het array
	 * @since	v0562
	 *****************************************************************/
	int findIncompatibleUnits(CCanNode* pNodeDoc,CCanNode* pNodeNetwork,
		STRUCT_UNIT** pArrUnits, int maxLength);

	static BOOL AddBroadcastUnit(CCanNode* pNode,STRUCT_UNIT **ReturnUnitSpecsPtr);
	
	/**
	 *	add New nodes from runtime to document node database ...
	 */
	void AddNewNodesToRTDatabase(CList<int>* pListNodes);

	/**
	 *	Update Document Nodedatabase with runtime properties.
	 *
	 *  @param copyNames [IN] bepaalt of de namen van de RT nodes 
	 *	naar de doc nodes moeten gekopieerd worden (zie BUG-0130)
	 *
	 *  Maakt enkel een copy van de bestaande units. Maakt geen nieuwe units aan.
	 */
	void UpdateNodesWithRTSpecs(BOOL copyNames);	
	BOOL UpdateNodesWithRTSpecs(BYTE bNodeAddress, BOOL copyNames, BOOL addNewUnits);
	BOOL UpdateNodesWithRTSpecs(CCanNode* pDocNode, CCanNode* pRTNode, BOOL copyNames, BOOL addNewUnits);

	/**
	 * Update Document node with rt specs
	 *
	 * sedert V16.32: 
	 * param pStatusChanged [OUT]: Wanneer TRUE dan werd de document node veranderd.
	 * 
	 */
	BOOL UpdateNodeWithRTSpecs(BYTE bNodeAddress,BOOL* pStatusChanged);

	/**
	 * Compare Document and runtime node Database ...
	 */
	CAppDbasePostBrowseError::Result CompareNodeDatabases() const;

	// Sedert V16.43
	// Vergelijken van de document en runtime node.	
	CAppDbasePostBrowseError::Result CompareNode(int nodeAddress) const;

	/**
	 * Copy RT Nodedatabase to document database ...
	 */
	void CopyRuntimeToDocDatabase();

	/**
	 * Copy document NodeDatabase to Runtime database
	 */
	void CopyDocumentToRuntimeDatabase();

	// Copieren van een document node naar een runtime node.
	//
	// Deze functie kan gebruikt worden wanneer een document node en een runtime node incompatible zijn.
	// De Document node wordt gecopieerd naar de runtime node.
	//
	// Gebruik: Browse single node.
	// Wanneer we een incompactible node hebben omdat de units verschillend zijn (of de runtime node 
	// heeft meer units) dan wordt de runtime node hersteld met de properties van de document node.
	// 
	// De pointers naar de units van de runtime node blijven bestaan.
	// 
	// - Alle properties van de node worden gecopieerd.
	// - De units worden gecopieerd + hetzelfde aantal.
	// - De runtime unit ptr pBindingUnit wordt op NULL geplaats.
	void CopyDocumentNodeToRuntimeNode(int nodeAddress);
	void CopyDocumentNodeToRuntimeNode(CCanNode* pDestRtNode,CCanNode& refDocumentNode);


	/**********************************************************************
	 * Sedert V16.41: m_mapUnitInfoLabels
     * Bijhouden van extra unit info in een map.	
	 **********************************************************************/
	void clearDatabaseExtraUnitInfo( );
	CString getUnitTextLabel(int nodeAddress, int unitAddress);
	void setUnitTextLabel(int nodeAddress, int unitAddress, const CString& str);
	void removeUnitTextLabel(int nodeAddress, int unitAddress);

protected:
	CAppNodeDatabase();

	// Sedert V16.16: Compare functies opgesplitst.
	// Helperfunctie voor de compare node databases...
	CAppDbasePostBrowseError::Result helper_CompareNodeDatabases_NodesRemoved( ) const;	
	CAppDbasePostBrowseError::Result helper_CompareNodeDatabases_NodesIncompatible( ) const;	
	CAppDbasePostBrowseError::Result helper_CompareNodeDatabases_NodesIncompatible(CCanNode* pDocNode, CCanNode* pRTNode) const;
	CAppDbasePostBrowseError::Result helper_CompareNodeDatabases_NodesAdded( ) const;	
	CAppDbasePostBrowseError::Result helper_CompareNodeDatabases_ConfigDifferences( ) const;
	CAppDbasePostBrowseError::Result helper_CompareNodeDatabases_NameDifferences( ) const;	


	static BOOL CreateBindingUnitProperties(CNodeDatabase *pNodes);
	static BOOL CreateBindingUnitProperties(CNodeDatabase *pNodes,CCanNode *pNode);	
	// static BOOL CreateBindingUnitProperties_whenNonAssigned(CCanNode *pNode,STRUCT_UNIT* pUnit);


	// Bij de runtime units moeten we de pointer naar de binding unit op NULL zetten.
	// Deze wordt specifiek gealloceerd voor de database node.
	// Door deze op NULL te zetten zorgen we ervoor dat deze nergens gedalloceerd wordt bij het vernietigen.
	//
	// TODO_LOW_PRIO: Bij een copy de referentie van de pBindingUnit nooit copieren.
	static BOOL DeleteRuntimeNodesBindingUnitPropPtr(CNodeDatabase* pNodes);
	static BOOL DeleteRuntimeNodeBindingUnitPropPtr(CNodeDatabase *pNodes, CCanNode* pNode);


	BOOL CreateBroadcastBindingUnits(CNodeDatabase *pNodes);
	BOOL CreateBroadcastBindingUnits(CNodeDatabase *pNodes,CCanNode *pNode);

private:
	CNodeDatabase *m_pRuntime;
	CNodeDatabase *m_pDocument;	

	// V16.42: Map met extra info labels voor de units. 
	// Key:		(NodeAddress<<8)+UnitAddress
	// Value:	String label.
	CMap<int,int,CString,CString> m_mapUnitInfoLabels;		
};
/********************************* inline functions ********************************/
inline CNodeDatabase* CAppNodeDatabase::GetNodesDocument() 
{
	return m_pDocument;
}
inline CNodeDatabase* CAppNodeDatabase::GetNodesRuntime() 
{
	return m_pRuntime;
}
/***********************************************************************************/

#endif // !defined(AFX_NODEDATABASEAPPLICATION_H__D6BC9E94_23FC_41D5_ACB3_0857A3F8CD6C__INCLUDED_)
