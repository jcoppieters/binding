#ifndef TMCONVENIENCE
#define TMCONVENIENCE



namespace TMConvenience
{
	// Bestaande installatie: De offline databse updaten met de runtime properties.
	void TMUpdateDocumentNodeDatabase(BOOL copyNames);

	// Bestaande installatie: Refresh single node: De offline node updaten met de runtime properties.
	void TMUpdateDocumentNode(BYTE bNodeAddress);

	// Bestaande installatie: Refresh single node: Operatie mislukt - Herstellen van de runtime node dmv de offline node.
	void TMRestoreRuntimeNode(BYTE bNodeAddress);

	void TMCreateNewBindingFiles();
	void TMInitialiseDatabases();
	void TMLoadDatabases(CStringArray* pBindingFileErrors=0);
	void TMSaveDatabases();		
	void TMCopyRuntimeToDocDatabase();
	void TMAddNewNodes();	
	void TMCreateExtraPBindings();				
	void TMCreateExtraBindings();				
	unsigned short TMDeleteNodefromDatabases(BYTE bNodeAddress);
	void TMUpgradeIncompatibleNodes(BYTE bNodeAddress);
	void TMCreateUnitListDelegation(void);
	void TMCreateUnitListRequestMeasuredValues(void);			// sedert versie 0x0C03
	void TMCreateUnitDimListPIDControl(void);					// Sedert V16.00: USE_SENSOR_PIDCONTROL	

	// Sedert V16.42:
	// Bijhouden van extra info voor de units.
	// - Extra tekst label:
	//	 - Smartbox ultra: Weergave zoals op de sticker.
	//	 - DALI bridge config.
	void TMSetDatabaseUnitExtraInfo();	
	void TMSetDatabaseUnitExtraInfo(int nodeAddress);	

	// Sedert V16.46:
	// Dali bridge config inladen en info koppelen aan de Duotecno units.
	void TMLoadDaliBridgeConfig( );
	void TMLoadDaliBridgeConfig(int nodeAddress);

	// Binding Unit Types upgraden ifv het unitType en bConfig & DALI units	
	// Alle binding units van de bestaande binding entries worden overlopen. 
	// Er wordt nagekeken in de node database aan welke unit type deze gekoppeld zijn.
	// Ifv het unit type + configvlaggen wordt het type van de binding unit ge-upgrade zodat er meer mogelijkheden zijn.
	// Bij de DALI bridge config worden de bestaande binding entries aangepast zodat deze de juiste iconen hebben.
	void TMUpgradeBindingUnitTypes(int nodeAddress = -1);
};
#endif
