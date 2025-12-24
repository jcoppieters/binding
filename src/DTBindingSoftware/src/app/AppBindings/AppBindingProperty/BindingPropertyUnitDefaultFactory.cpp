// BindingPropertyUnitDefaultFactory.cpp: implementation of the CBindingPropertyUnitDefaultFactory class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingPropertyUnitDefaultFactory.h"
#include "BindingPropertyUnitDefault.h"
#include "NodeDataBaseoperations.h"
#include "app\appCfg\AppCfginfo\cfginfoproductID_def.h"	
// ---------------------------------------
#include "TMGlobals.h"
using namespace TMNodeDatabase;
// ---------------------------------------

/***********************************************************/
#define DEBUG_LOCAL_ON		0
/***********************************************************/
#if (DEBUG_LOCAL_ON == 1)
	#define DEBUG_TRACE(x)			TRACE(x)
	#define DEBUG_TRACE_2(x,y)		TRACE(x,y)
#else
	#define	DEBUG_TRACE(x)
	#define DEBUG_TRACE_2(x,y)
#endif
/***********************************************************/


//////////////////////////////////////////////////////////////////////

namespace
{
	// Oppassen:
	// Kunnen wel de node opvragen maar kunnen niet de units opvragen via de 
	// node database.
	// Deze factory wordt ook aangeroepen tijdens het inladen van de units.
	// In dit geval is de unit nog niet toegevoegd aan zijn node.

	BOOL get_Node(BYTE bNodeAddress,BYTE bUnitAddress,CCanNode** ppNode)
	{
		BOOL result=FALSE;
		if ((bNodeAddress != 0xFF)&&(bUnitAddress !=0xFF))
		{
			CNodeDatabaseOperations Algoritme(TMGetDocumentNodes());
			CCanNode* const pNode = Algoritme.Find(bNodeAddress);
			if (pNode != 0)
			{
				*ppNode=pNode;
				result=TRUE;
			}
		}
		return result;
	}

	BYTE get_SoftwareVersion(BYTE bNodeAddress, BYTE bUnitAddress)
	{
		BYTE bSoftwareVersion = 0xFF;
		CCanNode* pNode=0;
		if (TRUE==get_Node(bNodeAddress,bUnitAddress,&pNode))
		{
			bSoftwareVersion = pNode->GetSoftwareVersion( );
		}
		return bSoftwareVersion;		
	}

	BYTE get_SoftwareVersionRevision(BYTE bNodeAddress, BYTE bUnitAddress)
	{
		BYTE bSoftwareVersion = 0xFF;
		CCanNode* pNode=0;
		if (TRUE==get_Node(bNodeAddress,bUnitAddress,&pNode))
		{
			bSoftwareVersion = pNode->GetSoftwareVersionRev( );
		}
		return bSoftwareVersion;	
	}

	// Sedert versie 0x0B00
	BYTE get_ProductID(BYTE bNodeAddress,BYTE bUnitAddress)
	{
		BYTE bProductID = PRODUCT_ID_UNKNOWN;
		CCanNode* pNode=0;
		if (TRUE==get_Node(bNodeAddress,bUnitAddress,&pNode))
		{
			bProductID=pNode->GetProductID();			
		}
		return bProductID;
	}

	// Sedert versie 0641
	int get_NrOfDimUnits(BYTE bNodeAddress)
	{
		int retVal = 0;

		CNodeDatabaseOperations Algoritme(TMGetDocumentNodes());
		CCanNode* const pNode = Algoritme.Find(bNodeAddress);
		if (pNode != 0)
		{
			CUnitIterator it(TMGetDocumentNodes()->CreateUnitIterator(pNode));
			for (it.Begin();!it.IsDone();it.Next())
			{
				STRUCT_UNIT* const pUnit = it.CurrentItem();
				if (pUnit->bUnitType == UNIT_TYPE_DIM)
				{
					++retVal;
				}
			}
		}
		return retVal;
	}

	// Sedert versie 0x0701
	// Zoeken naar de idx van de temperatuur sensor.
	// Als er meerdere sensor units aanwezig zijn in een module dan de sample intervals
	// niet dezelfde nemen maar een offset waarde ifv zijn idx.
	int get_SensorUnitIdx(BYTE bNodeAddress, BYTE bUnitAddress)
	{
		int retVal = 0;

		CNodeDatabaseOperations Algoritme(TMGetDocumentNodes());
		CCanNode* const pNode = Algoritme.Find(bNodeAddress);
		if (pNode != 0)
		{
			CUnitIterator it(TMGetDocumentNodes()->CreateUnitIterator(pNode));
			for (it.Begin();!it.IsDone();it.Next())
			{
				STRUCT_UNIT* const pUnit = it.CurrentItem();
				if (pUnit->bUnitType == UNIT_TYPE_SENS)
				{
					if (bUnitAddress > pUnit->bUnitAddress)
						retVal++;
				}
			}
		}
		return retVal;
	}

};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CBindingPropertyUnitDefaultFactory::CBindingPropertyUnitDefaultFactory() { }
CBindingPropertyUnitDefaultFactory::~CBindingPropertyUnitDefaultFactory() { }

CBindingPropertyUnitDefaultFactory* CBindingPropertyUnitDefaultFactory::
Instance()
{
	static CBindingPropertyUnitDefaultFactory _instance;
	return &_instance;
}

const CBindingPropertyUnitDefault* CBindingPropertyUnitDefaultFactory::
Create(BYTE bUnitType) const
{
	const CBindingPropertyUnitDefault* pReturn = 0;
	DEBUG_TRACE("CBindingPropertyUnitDefaultFactory::Create(BYTE bUnitType)-Enter\n");
	switch (bUnitType)
	{
		case UNIT_TYPE_DIM:
		{
			pReturn = CBindingPropertyUnitDimDefault::Instance();
		} break;
		case UNIT_TYPE_SWITCH:
		{
			pReturn = CBindingPropertyUnitSwitchDefault::Instance();				
		} break;
		case UNIT_TYPE_DUOSWITCH:
		{
			pReturn = CBindingPropertyUnitDuoswitchDefault::Instance();			
		} break;
		case UNIT_TYPE_ALARM:
		{
			DEBUG_TRACE("CBindingPropertyUnitAlarmDefault::Instance()\n");
			pReturn = CBindingPropertyUnitAlarmDefault::Instance();				
		} break;		
		case UNIT_TYPE_SENS:
		{
			pReturn = CBindingPropertyUnitSensDefault::Instance();				
		} break;
	}
	DEBUG_TRACE("CBindingPropertyUnitDefaultFactory::Create(BYTE bUnitType)-Exit\n");
	return pReturn;
}


const CBindingPropertyUnitDefault* CBindingPropertyUnitDefaultFactory::
Create(BYTE bUnitType, const CUnitInfo& refUnitInfo) const
{
	// Oppassen:
	// Kunnen wel de node opvragen maar kunnen niet de units opvragen via de 
	// node database.

	const CBindingPropertyUnitDefault* pReturn = 0;

	const BYTE bNodeAddress = refUnitInfo.bNodeAddress;
	const BYTE bUnitAddress = refUnitInfo.bUnitAddress;
	const BYTE bBindingUnitType = refUnitInfo.bBindingUnitType;
	const BYTE bSoftwareVersion = get_SoftwareVersion(bNodeAddress,bUnitAddress);
	const BYTE bProductID = get_ProductID(bNodeAddress,bUnitAddress);

	DEBUG_TRACE("CBindingPropertyUnitDefaultFactory::Create(BYTE bUnitType,const CUnitInfo& refUnitInfo)-Enter\n");

	switch (bUnitType)
	{
		case UNIT_TYPE_DIM:
		{		
			// Sedert versie 0641
			// DALI & DMX dimmer vanaf versie V90 heeft altijd meer dan 64 dimmer units.

			// Tijdens het inladen is nrOfDimUnits een verkeerde waarde.
			// Daarom is het beter om de node productID te gebruiken.
			// const BYTE bProductID=
			const int nrOfDimUnits=get_NrOfDimUnits(bNodeAddress);

			// Sedert V16.33: Integratie Smartbox ultra
			if (PRODUCT_ID_HC_BOX_ULTRA == bProductID) {				
				if (bUnitAddress <= PRODUCT_SBOXULTRA_UNITADDRESS_INTERNAL_LAST) {
					pReturn = VERSION_V82::CBindingPropertyUnitDimDefault::Instance();
				}
				else{
					pReturn = CBindingPropertyUnitDimDefault::Instance();
				}
			}
			else if ((PRODUCT_ID_STD_DALI_MASTER_DIM==bProductID)||(PRODUCT_ID_STD_DMX_MASTER_64DIM==bProductID)||
				     ((bSoftwareVersion!=0xFF)&&(bSoftwareVersion>=0x90)&&(nrOfDimUnits>=64))) {
				pReturn = VERSION_V90_DALIDMX::CBindingPropertyUnitDimDefault::Instance();	
			}
			else {
				if ((bSoftwareVersion!=0xFF && bSoftwareVersion>=0x82) ||
					(bSoftwareVersion!=0xFF && bSoftwareVersion>=0x73 && bSoftwareVersion<0x80)) {
					pReturn = VERSION_V82::CBindingPropertyUnitDimDefault::Instance();
				}
				else {
					pReturn = CBindingPropertyUnitDimDefault::Instance();
				}
			}
		} break;

		case UNIT_TYPE_SWITCH:
		{
			pReturn = CBindingPropertyUnitSwitchDefault::Instance();				
		} break;

		case UNIT_TYPE_DUOSWITCH:
		{
			const BYTE bSoftwareVersionRev = get_SoftwareVersionRevision(bNodeAddress,bUnitAddress);
			if ((bSoftwareVersion != 0xFF) && (bSoftwareVersion>=0x90) && (bSoftwareVersionRev>=0x25)) {
				pReturn = VERSION_V9025::CBindingPropertyUnitDuoswitchDefault::Instance();		
			}
			else {
				pReturn = CBindingPropertyUnitDuoswitchDefault::Instance();			
			}
		} break;

		case UNIT_TYPE_ALARM:
		{
			// TM_CHANGES_ALARM_TEXECOM
			// Ifv van het Binding unit type de standaard waarden aanmaken.
			// Kunnen niet in de nodedatabase de unit opvragen omdat hij tijdens
			// het laden van de units nog niet aan zijn node is toegevoegd.

			// TM_CHANGES_ALARM_INTEGRA
			// Mogen het standaard type gebruiken want er is geen configuratie 
			// nodig.

			switch(bBindingUnitType)
			{
				case BINDING_UNIT_TYPE_ALARM_TEXECOM:
				{
					// Andere standaard remote code.
					DEBUG_TRACE("VERSION_V2::CBindingPropertyUnitAlarmDefault::Instance()\n");
					pReturn = VERSION_V2::CBindingPropertyUnitAlarmDefault::Instance();
				} break;
			
				default:
				{
					DEBUG_TRACE("CBindingPropertyUnitAlarmDefault::Instance()\n");
					pReturn = CBindingPropertyUnitAlarmDefault::Instance();
				} break;
			}						
		} break;
		
		case UNIT_TYPE_SENS:
		{
			// Sedert versie 0x0D00
			// Onderscheid maken tussen de verschillende types sensor.
			switch(bBindingUnitType)
			{
				case BINDING_UNIT_TYPE_SENS:
				{	
					// Nxp Modules: Versie V90.
					// Beck-ipc: Versie V65
					// Atmel 103: Versie V70
					// Atmel 128:Versie V80
					// RPI: V66 (Smartbox ultra product).

					// Sedert V16.48: Firmware NXP modules V90.31 aanpassing aan de hysteris waarden.
					// Verlaagd van 1 naar 0.5C
					const BYTE bSoftwareVersionRev = get_SoftwareVersionRevision(bNodeAddress,bUnitAddress);						
					if ((bSoftwareVersion == 0x90) && (bSoftwareVersionRev < 0xFF) && (bSoftwareVersionRev >= 0x31)) {
						int idx = 0;
						if (bNodeAddress != 0xFF && bUnitAddress != 0xFF) {
							idx = get_SensorUnitIdx(bNodeAddress, bUnitAddress);
						}
						static const int REVISION_NUMBER = 1;
						pReturn = VERSION_V4::CBindingPropertyUnitSensDefault::Instance(REVISION_NUMBER, idx);
					}
					// Sedert V16.33: Integratie Smartbox ultra 	
					else if ((bSoftwareVersion==0x66) && (bSoftwareVersionRev>=0x13)) {						
						int idx = 0;
						if (bNodeAddress != 0xFF && bUnitAddress != 0xFF) {
							idx=get_SensorUnitIdx(bNodeAddress,bUnitAddress);
						}			
						pReturn = VERSION_V5::CBindingPropertyUnitSensDefault::Instance(idx);		
					}
					else if ((bSoftwareVersion<0xFF)&&(bSoftwareVersion>=0x90)&&
					         (bSoftwareVersionRev<0xFF)&&(bSoftwareVersionRev>=0x21))
					{					
						int idx = 0;
						if (bNodeAddress != 0xFF && bUnitAddress != 0xFF) {
							idx=get_SensorUnitIdx(bNodeAddress,bUnitAddress);
						}
						static const int REVISION_NUMBER = 0;
						pReturn = VERSION_V4::CBindingPropertyUnitSensDefault::Instance(REVISION_NUMBER,idx);
					}
					else if (bSoftwareVersion != 0xFF && bSoftwareVersion >= 0x90)
					{					
						int idx = 0;
						if (bNodeAddress != 0xFF && bUnitAddress != 0xFF) {
							idx=get_SensorUnitIdx(bNodeAddress,bUnitAddress);
						}
						pReturn = VERSION_V3::CBindingPropertyUnitSensDefault::Instance(idx);	
					}
					else if (((bSoftwareVersion>=0x84)&&(bSoftwareVersion<0x90))||
							 ((bSoftwareVersion>=0x75)&&(bSoftwareVersion<0x80)))
					{
						pReturn = VERSION_V2::CBindingPropertyUnitSensDefault::Instance();			
					}
					else
					{
						pReturn = CBindingPropertyUnitSensDefault::Instance();
					}		
				} break;

				case BINDING_UNIT_TYPE_SENSUNIV:
				{
					const int idx = get_SensorUnitIdx(bNodeAddress,bUnitAddress);
					pReturn = CBindingPropertyUnitSensUnivDefault::Instance(idx);

				} break;
			} /* switch(bBindingUnitType) */			 
		} break; /* UNIT_TYPE_SENS */
	}

	DEBUG_TRACE("CBindingPropertyUnitDefaultFactory::Create(BYTE bUnitType,const CUnitInfo& refUnitInfo)-Exit\n");
	return pReturn;
}
