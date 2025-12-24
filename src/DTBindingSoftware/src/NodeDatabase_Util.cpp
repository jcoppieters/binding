#include "stdafx.h"
#include "NodeDatabase_Util.h"
#include "NodeDatabase.h"


BOOL CNodeDatabase_Util::isValid(CNodeDatabase& refNodeDatabase,CCanNode* pNode)
{
	BOOL result = FALSE;
	int count = 0;
	if (refNodeDatabase.NROfNodes(&count) == TRUE) {
		if (count > 0) {
			for (CNodeIterator it(refNodeDatabase.CreateNodeIterator());!it.IsDone();it.Next()) {
				if (it.CurrentItem() == pNode) {
					result = TRUE;
					break;
				}
			}
		}
	}
	return result;
}


// Een twilight module:
//	1) Software versie >= V84 && <V90.
//  2) Standaard node.
//  2) 1 sensor unit & 16 inputs.
//		Nr of units = 17
//		unit 0x00 tem 0x0f	=> UNIT_TYPE_CONTROL.
//		unit 0x10			=> UNIT_TYPE_SENS.
//  3) Controle op de namen:
//	   Momenteel nog via een macro: enable/disable.

#define USE_TWILIGHT_CHECK_NAMES			1

int CNodeDatabase_Util::findNodeTwilight(CNodeDatabase& refNodeDatabase,CList<int>* listAddresses) {
	int nrOfNodes;
	refNodeDatabase.NROfNodes(&nrOfNodes);
	if (nrOfNodes <= 0) {
		ASSERT(nrOfNodes>0);
		return 0;
	}
			
	int count = 0;
	for (CNodeIterator it(refNodeDatabase.CreateNodeIterator());!it.IsDone();it.Next()) {
		CCanNode* const pNode = it.CurrentItem();
		if (NULL != pNode) {
			const int softwareVersion = pNode->GetSoftwareVersion();
			if (softwareVersion>=0x84 && softwareVersion<0x90) {
				if (NODE_PIN_STD_UNITS == pNode->bNodeType) {
					const int nrOfUnits = pNode->getNROfUnits();				
					if (17 == nrOfUnits) {			
						BOOL found = TRUE;
						for (int i=0;i<nrOfUnits;++i) {
							STRUCT_UNIT* pUnit = 0;
							if (!refNodeDatabase.GetUnit(pNode,i,&pUnit)) {
								found = FALSE;
								break;
							}

							// Controle doen op het unit type...
							if ((i>=0 && i<=0x0f) && (UNIT_TYPE_CONTROL != pUnit->bUnitType)) {
								found = FALSE;
								break;
							}							
							else if ((i==0x10) && (UNIT_TYPE_SENS != pUnit->bUnitType)) {
								found = FALSE;							
								break;						
							}
								
	#if(USE_TWILIGHT_CHECK_NAMES == 1)	
							// Controle doen op de namen 
							// Momenteel enkel controle doen op de eerste twilight input 0x08												
							if (0x08 == 0) {
								CString unitName(pUnit->szUnitName);
								if (unitName.Find("Active")<0) {
									found = FALSE;
									break;
								}
							}														
	#endif														
						}			
						if (found) {
							if (NULL != listAddresses) {
								listAddresses->AddTail(pNode->getNodeAddress());
							}
							count++;
						}
					}					
				}			
			}				
		}
	}	
	return count;
}