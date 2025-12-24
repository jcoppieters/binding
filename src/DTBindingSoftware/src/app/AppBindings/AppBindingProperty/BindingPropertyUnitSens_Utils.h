#pragma once

/**********************************************************************************************/
class CBindingPropertySensUnit;
class CBindingPropertyUnit;
class CNodeDatabase;
class CCanNode;
class CBindingFiles;
/**********************************************************************************************/

// Een lijst aanmaken van alle unit's die delegation ondersteunen.
extern int bindingPropertyUnitSens_delegationListCreate(const CNodeDatabase& refNodeDatabase,CPtrList* pList);	
extern int bindingPropertyUnitSens_delegationListCreate(const CNodeDatabase& refNodeDatabase,BYTE bNodeAddress,CPtrList* pList);	

// Alle items wissen uit de lijst.
extern void bindingPropertyUnitSens_delegationListDestroy(CPtrList* pList);

// Zoeken naar een Binding sensor unit die delegation ondersteund.
extern BOOL bindingPropertyUnitSens_delegationListFind(CPtrList& refList,BYTE bNodeAddress,BYTE bUnitAddress,CBindingPropertySensUnit** p);
extern BOOL bindingPropertyUnitSens_delegationListFindBuddy(CPtrList& refList,const CBindingPropertySensUnit& ref,CBindingPropertySensUnit** p);

// Zoeken of delegation ondesrteund wordt in dit project.
// Minimum 2 verschillende nodes die een sensor unit hebben die
// delegation ondersteund.
extern BOOL bindingPropertyUnitSens_delegationIsSupported(const CPtrList& refList);	

// Bindings aanmaken/verwijderen voor de buddy...
extern BOOL bindingPropertyUnitSens_delegationBuddyAdjust(CPtrList& refList,const CBindingPropertySensUnit& ref);


// Verwijderen van alle Bindings voor de node die verwijderd werd.
// - Delegation buddy's.
// - Modbus request unit.
// - PID Control Units.
// Aanroepen wanneer een node verwijderd wordt uit de nodedatabase.
extern int bindingPropertyUnitSens_checkBindingsNodeRemoved(CNodeDatabase* pNodeDatabase,CBindingFiles* pBindingFiles,BYTE bNodeAddress);

extern BOOL bindingPropertyUnitSens_delegationVerify(CNodeDatabase* pNodeDatabase);


// Verify:
// Controleren of iedere sensor unit een buddy heeft.
// Telkens een delegator / delegate.
extern BOOL bindingPropertyUnitSens_delegationBuddyVerify(CPtrList& refList,const CBindingPropertySensUnit& ref);
extern BOOL bindingPropertyUnitSens_delegationBuddyVerify(CPtrList& refList);

// Indien er voor een sensor unit die verbonden is via delegation
// geen buddy kan gevonden worden dan de bindings hiervan
// verwijderen.
extern BOOL bindingPropertyUnitSens_delegationBuddyNormalize(CPtrList& refList,CBindingFiles* pBindingFiles);


// 
extern BOOL bindingPropertyUnitSens_delegationBindingsRemove(CBindingFiles* pBindingFiles,CBindingPropertySensUnit* p);

// TODO ??
// Opvragen of Delegation supported is ifv software versie en product code.



// sedert versie 0x0C03
extern int bindingPropertyUnitSens_delegationRequestMeasureValListCreate(const CNodeDatabase& refNodeDatabase,CPtrList* pList);	

// Sedert V16.43: Mogelijkheid om de nieuwe units van een node toe te voegen.
extern int bindingPropertyUnitSens_delegationRequestMeasureValListAdd(CCanNode& refNode,CPtrList* pList);

// Alle items wissen uit de lijst.
extern void bindingPropertyUnitSens_delegationRequestMeasureValListDestroy(CPtrList* pList);

// Verify uitvoeren
extern BOOL bindingPropertyUnitSens_delegationRequestMeasureValVerify(CNodeDatabase* pNodeDatabase);


// Indien er P-bindings bestaan voor delegation waarbij een sensor gekoppeld werd aan
// een sensor die zich niet in de nodedatabase bevindt dan wordt deze P-binding 
// gewist...
// return waarde: Het aantal sensoren voor wie een P-binding gewist werd.
extern int bindingPropertyUnitSens_delegationRequestMeasureValNormalize(CNodeDatabase* pNodeDatabase,CBindingFiles* pBindingFiles);


/**********************************************************************************************/
// Sedert V16.00: USE_SENSOR_PIDCONTROL	

// Controleren of alle dimmer units die gekoppeld zijn in de PID control van de sensor units aanwezig zijn in de nodeDatabase...
// Controle op het juiste type.
extern BOOL bindingPropertyUnitSens_pidControlDimUnitsVerify(CNodeDatabase* pNodeDatabase);

// Sedert V16.00
// Een lijst aanmaken van alle dimmer units die kunnen gebruikt worden door de sensor unit voor PID control.
extern int bindingPropertyUnitSens_pidControlDimUnitListAdd(const CNodeDatabase& refNodeDatabase,CPtrList* pList);

// Sedert V16.43: Mogelijkheid om de nieuwe units van een node toe te voegen.
extern int bindingPropertyUnitSens_pidControlDimUnitListAdd(CCanNode& refNode,CPtrList* pList);

// Wissen van de P-Bindings voor de Sensor unit.
// return waarde: TRUE indien de P-Binding gewist werd...
extern BOOL bindingPropertyUnitSens_pidControlUnitBindingsRemove(CBindingFiles* pBindingFiles,int workingsmode,CBindingPropertySensUnit* p);

// Indien er P-Bindings aangemaakt werden voor een sensor unit en deze unit is 
// niet meer in de nodedatabase of de unit is niet meer van het type UNIT_TYPE_DIM 
// dan wordt de P-Binding gewist.
// return waarde: Het aantal P-bindings die gewist werden.
extern int bindingPropertyUnitSens_pidControlDimUnitsNormalize(CNodeDatabase* pNodeDatabase,CBindingFiles* pBindingFiles);

/**********************************************************************************************/