#pragma once


class CNodeDatabase;
class CCanNode;


// Upgraden van de binding property units dmv de node properties.
// Deze functie wordt aangeroepen na het browsen op een bestaande installatie.
extern void bindingpropertyunitalg_upgrade(CNodeDatabase* pNodeDatabase);
extern void bindingpropertyunitalg_upgrade(CCanNode* pNode);


// zie USE_DIMMER_PBINDINGS_MINVALUE
// Extra Property bindings aanmaken voor de modules 
// Dimmer module V7x & V8x: Minimum range 20%
// Deze functie wordt aangeroepen wanneer er nieuwe modules toegevoegd werden.
// Lijst nodeAddresses bevat een lijst van de addressen van de nieuwe modules
extern void bindingpropertyunitalg_createExtraPBindings(CNodeDatabase* pNodeDatabase,const CList<int>& nodeAddresses);