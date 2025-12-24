#include "stdafx.h"
#include "bindingpropertyunitsens_utils.h"
#include "bindingpropertyunit.h"

#include "NodeDatabase.h"
#include "nodedatabaseoperations.h"
#include "bindingfiles.h"

#include "App\Appbindings\AppBindingConv2Nodes\BindingConversionToNodesProperty.h"


// prototypes:

static 
BOOL bindingPropertyUnitSens_ListAdd(
	CPtrList* pList,
	const CBindingPropertyUnit* pUnit);

static
BOOL bindingPropertyUnitSens_delegationConstructBindings(
	CBindingPropertySensUnit* pRef,
	int delegationType,
	BYTE bNodeAddress,
	BYTE bUnitAddress);

static 
BOOL bindingPropertyUnitSens_delegationBuddyRemove(
	CPtrList& refList,
	const CBindingPropertySensUnit& ref);

static 
BOOL bindingPropertyUnitSens_delegationBuddyAdd(
	CPtrList& refList,
	const CBindingPropertySensUnit& ref);

static
BOOL bindingPropertyUnitSens_delegationIsBuddy(
	const CBindingPropertySensUnit& ref,
	const CBindingPropertySensUnit& ref2);

static
int bindingPropertyUnitSens_delegationListAdd(
	CCanNode& refNode,
	CPtrList* pList);

static
BOOL bindingPropertyUnitSens_RemovePropertyStrings(
    CBindingFiles* pBindingFiles,
	BYTE bNodeAddress,
	BYTE bUnitAddress,
	BYTE bFunctionData,
	BYTE bMethodData,
	int data =-1);

//////////////////////////////////////////////////////////////////

// Voor de Modbus Request values.
static
int bindingPropertyUnitSens_delegationRequestMeasureValNodeRemove(
	CNodeDatabase* pNodeDatabase,
	CBindingFiles* pBindingFiles,
	BYTE bNodeAddress);


// Sedert V16.00: USE_SENSOR_PIDCONTROL	
// Indien er een node uit de nodedatabase verdwijnt.
// Controleren of er geen P-Bindings aangemaakt zijn voor de PID control die verwijzen naar een node met dit nodeAddress...
// Indien er P-Bindings voor dit node address aanwezig zijn dan verwijderen.
static int bindingPropertyUnitSens_pidControlNodeRemoved(CNodeDatabase* pNodeDatabase,CBindingFiles* pBindingFiles,BYTE bNodeAddress);

///////////////////////////////////////////////////////////////////
// Public:
int bindingPropertyUnitSens_delegationListCreate(const CNodeDatabase& refNodeDatabase,CPtrList* pList)
{
	int retVal=0;
	CNodeIterator nodeIt(const_cast<CNodeDatabase&>(refNodeDatabase).CreateNodeIterator());
	for(nodeIt.Begin();!nodeIt.IsDone();nodeIt.Next()) {
		retVal+= bindingPropertyUnitSens_delegationListAdd(*nodeIt.CurrentItem(),pList);
	}
	return retVal;
}

int bindingPropertyUnitSens_delegationListCreate(const CNodeDatabase& refNodeDatabase,BYTE bNodeAddress,CPtrList* pList)
{
	CCanNode* pNode=0;
	int retVal=0;
	int iEntryNR=0;
	if (TRUE==const_cast<CNodeDatabase&>(refNodeDatabase).FindNode(bNodeAddress,&iEntryNR,&pNode)) {
		retVal+= bindingPropertyUnitSens_delegationListAdd(*pNode,pList);
	}
	return retVal;
}

void bindingPropertyUnitSens_delegationListDestroy(CPtrList* pList) {
	if (0!=pList) {
		pList->RemoveAll( );
	}
}

BOOL bindingPropertyUnitSens_delegationListFind(
	CPtrList& refList,
	BYTE bNodeAddress,
	BYTE bUnitAddress,
	CBindingPropertySensUnit** p)
{
	BOOL fResult=FALSE;

	POSITION pos = refList.GetHeadPosition();
	while(pos!=NULL) 
	{
		CBindingPropertyUnit* pEntry = 0;

		if ((pEntry=(CBindingPropertyUnit*) refList.GetNext(pos)) != NULL)
		{		
			if (pEntry->GetNodeAddress()==bNodeAddress &&
				pEntry->GetUnitAddress()==bUnitAddress)
			{
				*p=dynamic_cast<CBindingPropertySensUnit*>(pEntry);
				if (0!=*p)
				{
					fResult=TRUE;
					break;
				}
			}
		}
	}
	return fResult;
}

// Zoeken naar een buddy in de refList...
BOOL bindingPropertyUnitSens_delegationListFindBuddy(
	CPtrList& refList,
	const CBindingPropertySensUnit& ref,
	CBindingPropertySensUnit** p)
{
	BOOL fResult=FALSE;
	POSITION pos = refList.GetHeadPosition();

	while(pos!=NULL) 
	{
		CBindingPropertySensUnit* pUnit = 
			dynamic_cast<CBindingPropertySensUnit*>(
				(CBindingPropertyUnit*)refList.GetNext(pos));

		if (0!=pUnit)
		{					
			if (pUnit->GetDelegationType()!=
				CBindingPropertySensUnit::DELEGATION_TYPE_NONE)
			{
				/// TODO: Hier een extra controle doen ????

				if (pUnit->GetDelegationNodeAddress()==ref.GetNodeAddress()&&
					pUnit->GetDelegationUnitAddress()==ref.GetUnitAddress())
				{				
					*p=pUnit;
					fResult=TRUE;
					break;
				}			
			} 
		} /* if (0!=pUnit) */
	}
	return fResult;
}

BOOL bindingPropertyUnitSens_delegationIsSupported(
	const CPtrList& refList)
{
	BOOL fResult=FALSE;	
	const int count = refList.GetCount();

	if (count>=2)
	{
		BYTE* pbNodeAddress = new BYTE[count];
		int idx=0;

		if (0!=pbNodeAddress)
		{
			POSITION pos = refList.GetHeadPosition();

			memset(pbNodeAddress,0xFF,count);
			
			while(pos!=NULL) 
			{
				CBindingPropertyUnit* pEntry = 0;

				if ((pEntry=(CBindingPropertyUnit*)refList.GetNext(pos))!=NULL)
				{	
					int i=0;
					BYTE bNodeAddress = pEntry->GetNodeAddress();

					ASSERT(bNodeAddress != 0xFF);

					for(i=0;i<count;++i)
					{
						if(pbNodeAddress[i]!=bNodeAddress)
						{							
							break;
						}
					}	

					if(i==count)
					{
						pbNodeAddress[idx]=bNodeAddress;
						idx++;
					}
				}
			}

			if (idx>=2)
			{
				fResult=TRUE;
			}

			if (0!=pbNodeAddress)
			{
				delete [] pbNodeAddress;
			}
		}
	}
	return fResult;
}


// Aanpassingen versie 0x0C03:
// Functie aangepast zodat alle types van delegation ondersteund worden.
BOOL bindingPropertyUnitSens_delegationBindingsRemove(CBindingFiles* pBindingFiles,CBindingPropertySensUnit* p)
{
	BOOL fResult=FALSE;
	const BYTE delegationType=p->GetDelegationType();
	if (delegationType!=CBindingPropertySensUnit::DELEGATION_TYPE_NONE) {				
		BYTE bMethod=SENS_OBJECT_ATTR_DELEGATE;		
		if (delegationType==CBindingPropertySensUnit::DELEGATION_TYPE_DELEGATOR) {
			bMethod=SENS_OBJECT_ATTR_DELEGATOR;
		}
		else if (delegationType==CBindingPropertySensUnit::DELEGATION_TYPE_DELEGATE) {
			bMethod=SENS_OBJECT_ATTR_DELEGATE;
		}
		else if (delegationType==CBindingPropertySensUnit::DELEGATION_TYPE_REQUESTVALUE) {
			bMethod=SENS_OBJECT_ATTR_CFG_MEASURESENSOR;
		}
							
		if (TRUE==bindingPropertyUnitSens_RemovePropertyStrings(
				pBindingFiles,
				p->GetNodeAddress(),
				p->GetUnitAddress(),
				FC_UNITSENSSET,
				bMethod))
		{
			TRACE("RemovePropertyStrings() success\r\n");
		}
		else
		{
			TRACE("RemovePropertyStrings() failed\r\n");
		}
														
		// In de database.
		p->SetDelegationType(CBindingPropertySensUnit::DELEGATION_TYPE_NONE);
		p->SetDelegationNodeAddress(0xFF);
		p->SetDelegationUnitAddress(0xFF);

		fResult=TRUE;
	}
	return fResult;
}

#ifdef SKIP

	static 
	BOOL bindingPropertyUnitSens_delegationBuddyFind(
		CPtrList& refList,
		const CBindingPropertySensUnit& ref,
		CBindingPropertySensUnit** ppBuddy)
	{
		BOOL fResult=FALSE;

		if (TRUE==bindingPropertyUnitSens_delegationListFind(
				refList,
				ref.GetDelegationNodeAddress(),	//.GetNodeAddress(),		// Eigen node address
				ref.GetDelegationUnitAddress(),	//.GetUnitAddress(),		// Eigen unit address
				ppBuddy))
		{
			ASSERT(*ppBuddy!=0);
			fResult=TRUE;
		}
		return fResult;
	}

#endif


static 
BOOL bindingPropertyUnitSens_delegationBuddyFind(
	CPtrList& refList,
	const CBindingPropertySensUnit& ref,
	CBindingPropertySensUnit** ppBuddy)
{
	return bindingPropertyUnitSens_delegationListFindBuddy(
				refList,
				ref,
				ppBuddy);
}

static 
BOOL bindingPropertyUnitSens_delegationBuddyRemove(
	CPtrList& refList,
	const CBindingPropertySensUnit& ref)
{
	BOOL fResult=FALSE;
	CBindingPropertySensUnit* pBuddy=0;

	// Zoeken naar de buddy
	if (TRUE==bindingPropertyUnitSens_delegationBuddyFind(
				refList,
				ref,
				&pBuddy))
	{
		// Is niet meer dezelfde buddy.
		if (FALSE==bindingPropertyUnitSens_delegationIsBuddy(
				ref,
				*pBuddy))
		{
			TRACE("Remove binding for buddy\r\n");

			// Verwijderen van de oude buddy.
			fResult=bindingPropertyUnitSens_delegationConstructBindings(
				pBuddy,
				CBindingPropertySensUnit::DELEGATION_TYPE_NONE,
				0xFF,
				0xFF);	
		}	
	}
	return fResult;
}

static 
BOOL bindingPropertyUnitSens_delegationBuddyAdd(
	CPtrList& refList,
	const CBindingPropertySensUnit& ref)
{
	BOOL fResult=FALSE;
	CBindingPropertySensUnit* pBuddy=0;
	BOOL fAddBudy=FALSE;

	if (ref.GetDelegationType()!=
		CBindingPropertySensUnit::DELEGATION_TYPE_NONE)
	{
		fAddBudy=TRUE;

		// Zoeken naar de buddy
		if (TRUE==bindingPropertyUnitSens_delegationBuddyFind(
					refList,
					ref,
					&pBuddy))
		{
			if (TRUE==bindingPropertyUnitSens_delegationIsBuddy(
					ref,
					*pBuddy))
			{	
				fAddBudy=FALSE;
			}
		}

		if(TRUE==fAddBudy)
		{
			int type = CBindingPropertySensUnit::DELEGATION_TYPE_DELEGATOR;

			if (ref.GetDelegationType()==
				CBindingPropertySensUnit::DELEGATION_TYPE_DELEGATOR)
			{
				type=CBindingPropertySensUnit::DELEGATION_TYPE_DELEGATE;
			}

			TRACE("Add binding for buddy\r\n");

			if (0==pBuddy)
			{		
				bindingPropertyUnitSens_delegationListFind(
					refList,
					ref.GetDelegationNodeAddress(),	//bNodeAddress,
					ref.GetDelegationUnitAddress(),	//bUnitAddress,
					&pBuddy);	
			}

			fResult=bindingPropertyUnitSens_delegationConstructBindings(
					pBuddy,
					type,
					ref.GetNodeAddress(),
					ref.GetUnitAddress());
		}
	}
	return fResult;
}


static
BOOL bindingPropertyUnitSens_delegationIsBuddy(
	const CBindingPropertySensUnit& ref,
	const CBindingPropertySensUnit& refBuddy)
{	
	BOOL fResult=FALSE;

	if ((refBuddy.GetNodeAddress()==ref.GetDelegationNodeAddress())&&
		(refBuddy.GetUnitAddress()==ref.GetDelegationUnitAddress()))
	{
		const int type = ref.GetDelegationType();
		const int typeBuddy =refBuddy.GetDelegationType();

		if ((type==CBindingPropertySensUnit::DELEGATION_TYPE_DELEGATE)&&
			(typeBuddy==CBindingPropertySensUnit::DELEGATION_TYPE_DELEGATOR))
		{
			fResult=TRUE;	
		}
		else if ((type==CBindingPropertySensUnit::DELEGATION_TYPE_DELEGATOR)&&
				 (typeBuddy==CBindingPropertySensUnit::DELEGATION_TYPE_DELEGATE))
		{
			fResult=TRUE;
		}
	}
	return fResult;
}

BOOL bindingPropertyUnitSens_delegationBuddyAdjust(
	CPtrList& refList,
	const CBindingPropertySensUnit& ref)
{
	BOOL fResult=FALSE;

	TRACE("bindingPropertyUnitSens_delegationBuddyAdjust()-Enter\r\n");
	TRACE("Address:0x%02x,0x%02x\r\n",
		  ref.GetNodeAddress(),ref.GetUnitAddress());
	TRACE("Delegation:type=%d,address=0x%02x,0x%02x\r\n",
		  ref.GetDelegationType(),
		  ref.GetDelegationNodeAddress(),
		  ref.GetDelegationUnitAddress());

	if (TRUE==bindingPropertyUnitSens_delegationBuddyRemove(
			refList,
			ref))
	{
		TRACE("bindingPropertyUnitSens_delegationBuddyRemove() return TRUE\r\n");

		// Er is een buddy verwijderd.
		fResult=TRUE;		
	}

	if(TRUE==bindingPropertyUnitSens_delegationBuddyAdd(
			refList,
			ref))
	{
		TRACE("bindingPropertyUnitSens_delegationBuddyAdd() return TRUE\r\n");

		// Er is een nieuwe budy toegevoegd.
		fResult=TRUE;
	}

	TRACE("bindingPropertyUnitSens_delegationBuddyAdjust()-Exit\r\n");

	return fResult;
}

/*
static BOOL bindingPropertyUnitSens_RemovePropertyStrings(
    CBindingFiles* pBindingFiles,
	BYTE bNodeAddress,
	BYTE bUnitAddress,
	BYTE bFunctionData,
	BYTE bMethodData)
{
	BOOL fResult=FALSE;
	if (0!=pBindingFiles) {
		int i=0;
		const int count=pBindingFiles->Count();		
		CBindingFile* const pBindingFile=pBindingFiles->Get(bNodeAddress);
		if (0!=pBindingFile) {
			fResult=pBindingFile->RemovePropertyStrings(bUnitAddress,bFunctionData,bMethodData);
		}
	}
	return fResult;
}
*/
static BOOL bindingPropertyUnitSens_RemovePropertyStrings(
    CBindingFiles* pBindingFiles,
	BYTE bNodeAddress,
	BYTE bUnitAddress,
	BYTE bFunctionData,
	BYTE bMethodData,
	int data /* =-1 */)
{
	BOOL fResult=FALSE;
	if (0!=pBindingFiles) {
		// int i=0;
		// const int count=pBindingFiles->Count();		
		CBindingFile* const pBindingFile=pBindingFiles->Get(bNodeAddress);
		if (0!=pBindingFile) {
			if (-1 == data) {
				fResult=pBindingFile->RemovePropertyStrings(bUnitAddress,bFunctionData,bMethodData);
			}
			else { 
				fResult=pBindingFile->RemovePropertyStrings(bUnitAddress,bFunctionData,bMethodData,data);
			}
		}
	}
	return fResult;
}


static int bindingPropertyUnitSens_delegationNodeRemoveBuddys(CNodeDatabase* pNodeDatabase,CBindingFiles* pBindingFiles,BYTE bNodeAddress)
{
	int count=0;
	CPtrList listDelegates;		
	CPtrList listDelegatesNode;	
	
	if (bindingPropertyUnitSens_delegationListCreate(*pNodeDatabase,bNodeAddress,&listDelegatesNode)>0) {
		if (bindingPropertyUnitSens_delegationListCreate(*pNodeDatabase,&listDelegates)>0) {
			// Zoeken naar alle delegates uit de lijst of ze een buddy hebben...			
			POSITION pos = listDelegatesNode.GetHeadPosition();
			while(pos!=NULL)  {
				CBindingPropertySensUnit* pUnit = dynamic_cast<CBindingPropertySensUnit*>((CBindingPropertyUnit*)listDelegatesNode.GetNext(pos));
				if (0!=pUnit) {										
					TRACE("Iterate 0x%02x,0x%02x\r\n",pUnit->GetNodeAddress(),pUnit->GetUnitAddress());
					if (pUnit->GetDelegationType()!=CBindingPropertySensUnit::DELEGATION_TYPE_NONE) {
						CBindingPropertySensUnit* pBuddy=0;
						TRACE("Search buddy for 0x%02x,0x%02x\r\n",pUnit->GetNodeAddress(),pUnit->GetUnitAddress());												
						if (TRUE==bindingPropertyUnitSens_delegationListFindBuddy(listDelegates,*pUnit,&pBuddy)) {								
							// Buddy verwijderen uit de database & de binding files.
							if (TRUE==bindingPropertyUnitSens_delegationBindingsRemove(pBindingFiles,pBuddy)) {
								++count;
							}
							else {
								ASSERT_FAILED;
							}
						} /* if (TRUE==bindingPropertyUnitSens_delegationListFindBuddy( ) */
					} /* if (pUnit->GetDelegationType()!= */
				} /* if (0!=pUnit) */
			} /* while(pos!=NULL) */	

			bindingPropertyUnitSens_delegationListDestroy(&listDelegates);

		} /* if (bindingPropertyUnitSens_delegationListCreate( ) */

		bindingPropertyUnitSens_delegationListDestroy(&listDelegatesNode);

	} /* if (bindingPropertyUnitSens_delegationListCreate( ) */
	return count;
}

//	 Om te debuggen wordt er nog een verify gedaan van de delegation unit (buddys)
//	 deze verify( ) moet gebeuren nadat de node (en zijn bindings) uit de node 
//   database verwijderd werden.
static
BOOL bindingPropertyUnitSens_delegationBuddysVerify(
	CNodeDatabase* pNodeDatabase)
{
	BOOL fRetVal=TRUE;

	// Gaan hier een verify uitvoeren om te controleren... (Enkel debug code.)				
	CPtrList listDelegates;

	if (bindingPropertyUnitSens_delegationListCreate(
		*pNodeDatabase,
		&listDelegates)>=0)
	{
		if (listDelegates.GetCount()>0)
		{
			const BOOL fResult=
				bindingPropertyUnitSens_delegationBuddyVerify(
				listDelegates);

			ASSERT(TRUE==fResult);

			if(FALSE==fResult)
			{
				fRetVal=FALSE;
			}

			bindingPropertyUnitSens_delegationListDestroy(
				&listDelegates);
		}
	}

	return fRetVal;
}


// Implementatie:
// listDelegates: bevat een lijst van alle senor units die delegation ondersteunen.
// listDelegatesNode: bevat een lijst van deze node (die verwijderd wordt)  van alle sensor units die delegation ondersteunen.
//
// Alle sensor (bindingpropertysens units) overlopen en controleren of deze gekoppeld zijn aan een buddy.
// Indien er een buddy gevonden werd dan de delegation bindings verwijderen voor deze buddy.
//
// Opmerking: Eerst wordt deze functie aangeroepen, nadien worden de bindings van deze node verwijderd.
//
int bindingPropertyUnitSens_checkBindingsNodeRemoved(CNodeDatabase* pNodeDatabase,CBindingFiles* pBindingFiles,BYTE bNodeAddress)
{
	int retVal=0;	
	
	// Verwijderen van bindings die verwijden naar een buddy van de node die verwijdert is.
	retVal+=bindingPropertyUnitSens_delegationNodeRemoveBuddys(pNodeDatabase,pBindingFiles,bNodeAddress);
	
	// Verwijderen van bindings naar de unit van de node die verwijderd is.
	retVal+=bindingPropertyUnitSens_delegationRequestMeasureValNodeRemove(pNodeDatabase,pBindingFiles,bNodeAddress);

	// Sedert V16.00: USE_SENSOR_PIDCONTROL	
	// Verwijderen van bindings waarbij een PID control unit gekoppeld is van deze node.	
	retVal+=bindingPropertyUnitSens_pidControlNodeRemoved(pNodeDatabase,pBindingFiles,bNodeAddress);;
	
	return retVal;
}

BOOL bindingPropertyUnitSens_delegationVerify(CNodeDatabase* pNodeDatabase)
{
	BOOL fRetVal=FALSE;
	if (TRUE==bindingPropertyUnitSens_delegationBuddysVerify(pNodeDatabase)) {
		if(TRUE==bindingPropertyUnitSens_delegationRequestMeasureValVerify(pNodeDatabase)) {
			fRetVal=TRUE;
		}
		else {
			ASSERT(fRetVal==TRUE);
		}
	}
	else {
		ASSERT(fRetVal==TRUE);
	}
	return fRetVal;
}

// Verify:
// Controleren of iedere sensor unit een buddy heeft.
// Telkens een delegator / delegate.
BOOL bindingPropertyUnitSens_delegationBuddyVerify(CPtrList& refList,const CBindingPropertySensUnit& ref)
{
	BOOL fResult=FALSE;
	CBindingPropertySensUnit* pBuddy=0;
	if (TRUE==bindingPropertyUnitSens_delegationBuddyFind(refList,ref,&pBuddy)) {
		ASSERT(0!=pBuddy);
		fResult=TRUE;
	}	
	return fResult;
}

BOOL bindingPropertyUnitSens_delegationBuddyVerify(
	CPtrList& refList)
{
	BOOL fResult=TRUE;

	POSITION pos = refList.GetHeadPosition();
	while(pos!=NULL) 
	{
		CBindingPropertySensUnit* pUnit = 
			dynamic_cast<CBindingPropertySensUnit*>(
				(CBindingPropertyUnit*)refList.GetNext(pos));

		if ((0!=pUnit)&&
			(CBindingPropertySensUnit::DELEGATION_TYPE_NONE!=pUnit->GetDelegationType()))	
		{		
			// CBindingPropertySensUnit* pBuddy=0;	

			// sedert versie 0x0C03:
			// Units in deze lijst moeten allemaal delegation ondersteunen. 
			// (anders mogen deze niet in de lijst staan...)

			ASSERT(pUnit->GetSupportedFlags()&
				   CBindingPropertySensUnit::SENS_SUPPORT_FLAGS_DELEGATION);

			if (FALSE==bindingPropertyUnitSens_delegationBuddyVerify(
					refList,
					*pUnit))
			{
				fResult=FALSE;
				break;
			}
		}
	}

	return fResult;
}

BOOL bindingPropertyUnitSens_delegationBuddyNormalize(
	CPtrList& refList,
	CBindingFiles* pBindingFiles)
{
	BOOL fResult=TRUE;
	POSITION pos = refList.GetHeadPosition();

	while(pos!=NULL) 
	{
		CBindingPropertySensUnit* pUnit = 
			dynamic_cast<CBindingPropertySensUnit*>(
				(CBindingPropertyUnit*)refList.GetNext(pos));

		if ((0!=pUnit)&&
			(CBindingPropertySensUnit::DELEGATION_TYPE_NONE!=pUnit->GetDelegationType()))	
		{		
			if (FALSE==bindingPropertyUnitSens_delegationBuddyVerify(
					refList,
					*pUnit))
			{
				if (FALSE==bindingPropertyUnitSens_delegationBindingsRemove(
						pBindingFiles,
						pUnit))
				{
					ASSERT_FAILED;
					fResult=FALSE;
				}
			}
		}
	}
	return fResult;
}

// private:
static BOOL bindingPropertyUnitSens_ListAdd(CPtrList* pList,const CBindingPropertyUnit* pUnit) {
	BOOL fResult=FALSE;	
	if (0!=pList) {
		pList->AddTail((void*) pUnit);
		fResult=TRUE;
	}
	return fResult;
}

static BOOL bindingPropertyUnitSens_ListFind(CPtrList* pList,const CBindingPropertyUnit* pUnit) {
	REQUIRE(NULL != pList);
	return (NULL != pList->Find((void*) pUnit) ? TRUE : FALSE);
	
}


static BOOL bindingPropertyUnitSens_delegationConstructBindings(CBindingPropertySensUnit* pRef,int delegationType,
																BYTE bNodeAddress,BYTE bUnitAddress) {
	BOOL fResult=FALSE;
	if(0!=pRef) {
		CBindingConversionToNodesProperty ConvertBindingToNodes(pRef);
		TRACE("Binding:NodeAddress=0x%02x,0x%02x,delegationtype=%d\r\n",pRef->GetNodeAddress( ),pRef->GetUnitAddress( ),delegationType);
		pRef->SetDelegationType(delegationType);
		pRef->SetDelegationNodeAddress(bNodeAddress);
		pRef->SetDelegationUnitAddress(bUnitAddress);							
		if (ConvertBindingToNodes.ProcessPropertyBindings()>=0) {
			fResult=TRUE;
		}
	}
	return fResult;
}

// Voor deze node alle sensor units die Delegation ondersteunen Ptr naar BindingPropertyUnit 
// toevoegen aan de lijst.
static int bindingPropertyUnitSens_delegationListAdd(CCanNode& refNode,CPtrList* pList) {
	int retVal=0;
	CUnitIterator unitIt(&refNode);
	for(unitIt.Begin();!unitIt.IsDone();unitIt.Next()) {
		STRUCT_UNIT* const pUnit = unitIt.CurrentItem();
		if (0!=pUnit) {
			if ((pUnit->bUnitType==UNIT_TYPE_SENS)&&(pUnit->m_pBindingUnit!=0)) {
				CBindingPropertySensUnit* const pBindingPropSensUnit = dynamic_cast<CBindingPropertySensUnit*>(pUnit->m_pBindingUnit);
				if (0!=pBindingPropSensUnit) {
					if (pBindingPropSensUnit->GetSupportedFlags()& CBindingPropertySensUnit::SENS_SUPPORT_FLAGS_DELEGATION) {
						if (0!=pList) {
							if (TRUE==bindingPropertyUnitSens_ListAdd(pList,pUnit->m_pBindingUnit)) {
								retVal++;
							}
						}
					}
				}
			}
		}
	}	
	return retVal;
}


// private:

// Als er een node uit de database verdwijnt controleren of er geen sensor units van 
// deze node gekoppeld zijn om de gemeten temp waarde op te vragen...
//
// Iteratie van alle sensor units die de gemeten waarde kunnen opvragen bij een andere sensor.
// Indien er een sensor gekoppeld is aan een sensor van de node die verwijderd wordt dan deze
// binding verwijderen.
//
//

static int bindingPropertyUnitSens_delegationRequestMeasureValNodeRemove(CNodeDatabase* pNodeDatabase,CBindingFiles* pBindingFiles,BYTE bNodeAddress) {
	int retVal=0;
	CNodeIterator nodeIt(const_cast<CNodeDatabase&>(*pNodeDatabase).CreateNodeIterator());
	for(nodeIt.Begin();!nodeIt.IsDone();nodeIt.Next()) {
		CCanNode* const pNodeCurrent=nodeIt.CurrentItem();
		if ((0!=pNodeCurrent)&& (pNodeCurrent->getNodeAddress()!=bNodeAddress)) {
			CUnitIterator unitIt(nodeIt.CurrentItem());
			STRUCT_UNIT* const pUnit=unitIt.CurrentItem();
			if ((0!=pUnit)&&(UNIT_TYPE_SENS==pUnit->bUnitType)) {
				CBindingPropertySensUnit* const pBindingPropSensUnit = dynamic_cast<CBindingPropertySensUnit*>(pUnit->m_pBindingUnit);
				if (0!=pBindingPropSensUnit) {
					if (pBindingPropSensUnit->GetSupportedFlags()& CBindingPropertySensUnit::SENS_SUPPORT_FLAGS_REQUESTMEASUREDVALUE) {	
						if ((pBindingPropSensUnit->GetDelegationType()==CBindingPropertySensUnit::DELEGATION_TYPE_REQUESTVALUE)&&
							(pBindingPropSensUnit->GetDelegationNodeAddress()==bNodeAddress)) {
							if (TRUE==bindingPropertyUnitSens_delegationBindingsRemove(pBindingFiles,pBindingPropSensUnit)) {
								retVal++;
							}
						}
					}
				}			
			}				
		}
	}
	return retVal;
}


// public

// sedert versie 0x0C03
int bindingPropertyUnitSens_delegationRequestMeasureValListCreate(const CNodeDatabase& refNodeDatabase,CPtrList* pList) {
	int retVal=0;
	CNodeIterator nodeIt(const_cast<CNodeDatabase&>(refNodeDatabase).CreateNodeIterator());
	for(nodeIt.Begin();!nodeIt.IsDone();nodeIt.Next()) {
		retVal+= bindingPropertyUnitSens_delegationRequestMeasureValListAdd(*nodeIt.CurrentItem(),pList);
	}
	return retVal;
}


int bindingPropertyUnitSens_delegationRequestMeasureValListAdd(CCanNode& refNode,CPtrList* pList) {
	int retVal=0;
	CUnitIterator unitIt(&refNode);
	for(unitIt.Begin();!unitIt.IsDone();unitIt.Next()) {
		STRUCT_UNIT* const pUnit = unitIt.CurrentItem();
		if (0!=pUnit) {
			if ((pUnit->bUnitType==UNIT_TYPE_SENS)&&(pUnit->m_pBindingUnit!=0)) {
				CBindingPropertySensUnit* const pBindingPropSensUnit =dynamic_cast<CBindingPropertySensUnit*>(pUnit->m_pBindingUnit);
				if (0!=pBindingPropSensUnit) {
					if (!(pBindingPropSensUnit->GetSupportedFlags()&CBindingPropertySensUnit::SENS_SUPPORT_FLAGS_REQUESTMEASUREDVALUE)) {
						if (0!=pList) {
							// Sedert V16.43: Controleren of deze nog niet aanwezig is in de lijst.
							// Aanpassing nodig voor het browsen single node (+ toevoegen van nieuwe units...)
							if (FALSE == bindingPropertyUnitSens_ListFind(pList,pUnit->m_pBindingUnit)) {								
								if (TRUE==bindingPropertyUnitSens_ListAdd(pList,pUnit->m_pBindingUnit)) {
									retVal++;
								}
							}
						}
					}
				}
			}
		}
	}	
	return retVal;
}

// sedert versie 0x0C03
void bindingPropertyUnitSens_delegationRequestMeasureValListDestroy(CPtrList* pList) {
	if (0!=pList) {
		pList->RemoveAll( );
	}
}

//
// public functie.
//
// Voor iedere sensor unit die gekoppeld is via delegation controleren of
// deze aanwezig is in de nodedatabase
//
BOOL bindingPropertyUnitSens_delegationRequestMeasureValVerify(CNodeDatabase* pNodeDatabase) {
	BOOL fRetVal=TRUE;
	CNodeIterator nodeIt(const_cast<CNodeDatabase&>(*pNodeDatabase).CreateNodeIterator());
	for(nodeIt.Begin();!nodeIt.IsDone();nodeIt.Next()) {
		CUnitIterator unitIt(nodeIt.CurrentItem());
		for(unitIt.Begin();!unitIt.IsDone();unitIt.Next()) {
			STRUCT_UNIT* const pUnit=unitIt.CurrentItem();
			if ((0!=pUnit)&&(UNIT_TYPE_SENS==pUnit->bUnitType)) {
				CBindingPropertySensUnit* const pBindingPropSensUnit = dynamic_cast<CBindingPropertySensUnit*>(pUnit->m_pBindingUnit);
				if (0!=pBindingPropSensUnit) {
					if (pBindingPropSensUnit->GetSupportedFlags()&CBindingPropertySensUnit::SENS_SUPPORT_FLAGS_REQUESTMEASUREDVALUE) {	
						if (pBindingPropSensUnit->GetDelegationType()==CBindingPropertySensUnit::DELEGATION_TYPE_REQUESTVALUE) {
							// Zoeken of de gekoppeld unit in de nodedatabase aanwezig is...
							const BYTE bNodeAddress=pBindingPropSensUnit->GetDelegationNodeAddress();
							const BYTE bUnitAddress=pBindingPropSensUnit->GetDelegationUnitAddress();
							STRUCT_UNIT* pUnitVerify = 0;
							CNodeDatabaseOperations nodeDatabaseHelper(pNodeDatabase);
							if(TRUE==nodeDatabaseHelper.Find(bNodeAddress,bUnitAddress,&pUnitVerify)) {
								if (pUnitVerify->bUnitType!=UNIT_TYPE_SENS) {						
									ASSERT(pUnitVerify->bUnitType==UNIT_TYPE_SENS);
									fRetVal=FALSE;
								}
							}
							else {
								ASSERT(0==pUnitVerify);
								fRetVal=FALSE;
							}
						}
					}			
				}
			}
		}
	}
	return fRetVal;
}

int bindingPropertyUnitSens_delegationRequestMeasureValNormalize(
	CNodeDatabase* pNodeDatabase,
	CBindingFiles* pBindingFiles)
{
	int retVal=0;
	CNodeIterator nodeIt(const_cast<CNodeDatabase&>(*pNodeDatabase).CreateNodeIterator());
	for(nodeIt.Begin();!nodeIt.IsDone();nodeIt.Next()) {
		CUnitIterator unitIt(nodeIt.CurrentItem());
		for(unitIt.Begin();!unitIt.IsDone();unitIt.Next()) {	
			STRUCT_UNIT* const pUnit=unitIt.CurrentItem();
			if ((0!=pUnit)&& (UNIT_TYPE_SENS==pUnit->bUnitType)) {
				CBindingPropertySensUnit* const pBindingPropSensUnit = dynamic_cast<CBindingPropertySensUnit*>(pUnit->m_pBindingUnit);
				if (0!=pBindingPropSensUnit) {
					BOOL fRemoveBinding = FALSE;

					if (pBindingPropSensUnit->GetSupportedFlags()&
						CBindingPropertySensUnit::SENS_SUPPORT_FLAGS_REQUESTMEASUREDVALUE)
					{	
						if (pBindingPropSensUnit->GetDelegationType()==
							CBindingPropertySensUnit::DELEGATION_TYPE_REQUESTVALUE)
						{
							// Zoeken of de gekoppeld unit in de nodedatabase aanwezig is...
							const BYTE bNodeAddress=pBindingPropSensUnit->GetDelegationNodeAddress();
							const BYTE bUnitAddress=pBindingPropSensUnit->GetDelegationUnitAddress();						

							STRUCT_UNIT* pUnitVerify = 0;
							CNodeDatabaseOperations nodeDatabaseHelper(pNodeDatabase);

							if(TRUE==nodeDatabaseHelper.Find(bNodeAddress,bUnitAddress,&pUnitVerify))
							{
								if (pUnitVerify->bUnitType!=UNIT_TYPE_SENS)
								{
									fRemoveBinding=TRUE;
								}
							}
							else
							{
								fRemoveBinding=TRUE;
							}
						}
					}

					if(TRUE==fRemoveBinding)
					{
						if(TRUE==bindingPropertyUnitSens_delegationBindingsRemove(
							pBindingFiles,
							pBindingPropSensUnit))
						{
							retVal++;
						}
					}
				}
			}
		}
	}
	return retVal;
}

/**********************************************************************************************/
// Sedert V16.00: USE_SENSOR_PIDCONTROL	

// Controle of de unit gevonden wordt in de nodeDatabase + of deze van het juiste type is...
static BOOL bindingPropertyUnitSens_pidControlDimUnitVerify(CNodeDatabase* pNodeDatabase,BYTE bNodeAddress,BYTE bUnitAddress,BYTE bUnitType) 
{
	STRUCT_UNIT* pUnit = 0;
	CNodeDatabaseOperations nodeDatabaseHelper(pNodeDatabase);
	if(FALSE == nodeDatabaseHelper.Find(bNodeAddress,bUnitAddress,&pUnit)) {
		return FALSE;	// unit niet gevonden...
	}
	return ((0!=pUnit) && (bUnitType == pUnit->bUnitType));								
}

// Indien er een node uit de nodedatabase verdwijnt.
// Controleren of er geen P-Bindings aangemaakt zijn voor de PID control die verwijzen naar een node met dit nodeAddress...
static int bindingPropertyUnitSens_pidControlNodeRemoved(CNodeDatabase* pNodeDatabase,CBindingFiles* pBindingFiles,BYTE bRemovedNodeAddress) {
	int count = 0;
	CNodeIterator nodeIt(const_cast<CNodeDatabase&>(*pNodeDatabase).CreateNodeIterator());
	for(nodeIt.Begin();!nodeIt.IsDone();nodeIt.Next()) {
		CCanNode* const pNodeCurrent=nodeIt.CurrentItem();
		if ((0!=pNodeCurrent)&& (pNodeCurrent->getNodeAddress()!=bRemovedNodeAddress)) {
			CUnitIterator unitIt(nodeIt.CurrentItem());
			STRUCT_UNIT* const pUnit=unitIt.CurrentItem();
			if ((0!=pUnit)&&(UNIT_TYPE_SENS==pUnit->bUnitType)) {
				CBindingPropertySensUnit* const pBindingPropSensUnit = dynamic_cast<CBindingPropertySensUnit*>(pUnit->m_pBindingUnit);			
				if ((0!=pBindingPropSensUnit) && (pBindingPropSensUnit->GetSupportedFlags()&CBindingPropertySensUnit::SENS_SUPPORT_FLAGS_PIDCONTROL)) {
					for (int mode = 0;mode < SENS_PIDCONTROL_WORKINGMODE_MAX; ++mode) {
						BYTE bNodeAddress = 0xff;
						BYTE bUnitAddress = 0xff;
						if (pBindingPropSensUnit->getPropertyPIDControlUnit(mode,&bNodeAddress,&bUnitAddress)) {							
							if ((bNodeAddress != 0xff) && (bRemovedNodeAddress == bNodeAddress)) {
								if (bindingPropertyUnitSens_pidControlUnitBindingsRemove(pBindingFiles,mode,pBindingPropSensUnit)) {
									++count;
								}
							}
						}
					}
				}				
			}
		}
	}
	return count;
}

// Controleren of alle dimmer units die gekoppeld zijn in de PID control van de sensor units aanwezig zijn in de nodeDatabase...
// Controle op het juiste type.
BOOL bindingPropertyUnitSens_pidControlDimUnitsVerify(CNodeDatabase* pNodeDatabase) {
	BOOL fRetVal = TRUE;
	CNodeIterator nodeIt(const_cast<CNodeDatabase&>(*pNodeDatabase).CreateNodeIterator());
	for(nodeIt.Begin();!nodeIt.IsDone();nodeIt.Next()) {
		CUnitIterator unitIt(nodeIt.CurrentItem());
		for(unitIt.Begin();!unitIt.IsDone();unitIt.Next()) {
			STRUCT_UNIT* const pUnit=unitIt.CurrentItem();
			if ((0!=pUnit)&&(UNIT_TYPE_SENS==pUnit->bUnitType)) {
				CBindingPropertySensUnit* const pBindingPropSensUnit = dynamic_cast<CBindingPropertySensUnit*>(pUnit->m_pBindingUnit);
				if ((0!=pBindingPropSensUnit) && (pBindingPropSensUnit->GetSupportedFlags()&CBindingPropertySensUnit::SENS_SUPPORT_FLAGS_PIDCONTROL)) {
					for (int mode = 0;mode < SENS_PIDCONTROL_WORKINGMODE_MAX; ++mode) {
						BYTE bNodeAddress = 0xff;
						BYTE bUnitAddress = 0xff;
						if (pBindingPropSensUnit->getPropertyPIDControlUnit(mode,&bNodeAddress,&bUnitAddress)) {
							if ((0xff != bNodeAddress) && (0xff != bUnitAddress)) {
								if (!bindingPropertyUnitSens_pidControlDimUnitVerify(pNodeDatabase,bNodeAddress,bUnitAddress,UNIT_TYPE_DIM)) {
									fRetVal=FALSE;		// verify niet ok - break & return false.
									break;
								}								
							}
						}
					}
				}
			}
		}
	}
	return fRetVal;
}

// Sedert V16.00
// Een lijst aanmaken van alle dimmer units die kunnen gebruikt worden door de sensor unit voor PID control.
int bindingPropertyUnitSens_pidControlDimUnitListAdd(const CNodeDatabase& refNodeDatabase,CPtrList* pList) {
	int retVal=0;
	CNodeIterator nodeIt(const_cast<CNodeDatabase&>(refNodeDatabase).CreateNodeIterator());
	for(nodeIt.Begin();!nodeIt.IsDone();nodeIt.Next()) {
		retVal+= bindingPropertyUnitSens_pidControlDimUnitListAdd(*nodeIt.CurrentItem(),pList);
	}
	return retVal;
}

// Sedert V16.00: USE_SENSOR_PIDCONTROL	
// Toevoegen van alle dimmer units die in aanmerking komen om als PID pwm unit gebruikt te worden.
// Momenteel zijn dit alle units van het type UNIT_TYPE_DIM.
//
// Sedert V16.43: Implementatie browse single node.
//	- Functie publiek geplaatst.
//	- Controle doen of de unit nog niet aanwezig is.
int bindingPropertyUnitSens_pidControlDimUnitListAdd(CCanNode& refNode,CPtrList* pList) {
	int retVal=0;
	CUnitIterator unitIt(&refNode);
	for(unitIt.Begin();!unitIt.IsDone();unitIt.Next()) {
		STRUCT_UNIT* const pUnit = unitIt.CurrentItem();
		if ((0!=pUnit) && (UNIT_TYPE_DIM == pUnit->bUnitType)) {
			if (NULL == pList->Find((void*) pUnit)) {
				pList->AddTail((void*) pUnit);
				retVal++;
			}
		}
	}
	return retVal;
}

// Een P-Binding verwijderen voor een workingsmode (workingsmode = heating of cooling).
BOOL bindingPropertyUnitSens_pidControlUnitBindingsRemove(CBindingFiles* pBindingFiles,int workingsmode,CBindingPropertySensUnit* p) {
	BOOL retVal=FALSE;
	if (0!=p) {					
		if (TRUE==bindingPropertyUnitSens_RemovePropertyStrings(pBindingFiles,p->GetNodeAddress(),p->GetUnitAddress(),
			FC_UNITSENSSET, SENS_OBJECT_ATTR_PIDCONTROLUNIT, workingsmode))  {
			TRACE3("Sensor PID Control unit removePropertyStrings() success - ,mode=%d,0x%02x,0x%02x\r\n",
				workingsmode, p->GetNodeAddress(), p->GetUnitAddress());
			retVal = TRUE;
		}
		else {
			TRACE("Sensor PID Control unit removePropertyStrings() failed\r\n");
		}
		p->setPropertyPIDControlUnit(workingsmode,0xff,0xff);
	}
	return retVal;
}


// Indien er P-Bindings aangemaakt werden voor een sensor unit en deze unit is 
// niet meer in de nodedatabase of de unit is niet meer van het type UNIT_TYPE_DIM 
// dan wordt de P-Binding gewist.
// return waarde: Het aantal P-bindings die gewist werden.
int bindingPropertyUnitSens_pidControlDimUnitsNormalize(CNodeDatabase* pNodeDatabase,CBindingFiles* pBindingFiles) {	
	int retVal = 0;
	BOOL fRetVal = TRUE;
	CNodeIterator nodeIt(const_cast<CNodeDatabase&>(*pNodeDatabase).CreateNodeIterator());
	for(nodeIt.Begin();!nodeIt.IsDone();nodeIt.Next()) {
		CUnitIterator unitIt(nodeIt.CurrentItem());
		for(unitIt.Begin();!unitIt.IsDone();unitIt.Next()) {
			STRUCT_UNIT* const pUnit=unitIt.CurrentItem();
			if ((0!=pUnit)&&(UNIT_TYPE_SENS==pUnit->bUnitType)) {
				CBindingPropertySensUnit* const pBindingPropSensUnit = dynamic_cast<CBindingPropertySensUnit*>(pUnit->m_pBindingUnit);
				if ((0!=pBindingPropSensUnit) && (pBindingPropSensUnit->GetSupportedFlags()&CBindingPropertySensUnit::SENS_SUPPORT_FLAGS_PIDCONTROL)) {
					for (int mode = 0;mode < SENS_PIDCONTROL_WORKINGMODE_MAX; ++mode) {
						BYTE bNodeAddress = 0xff;
						BYTE bUnitAddress = 0xff;
						if (pBindingPropSensUnit->getPropertyPIDControlUnit(mode,&bNodeAddress,&bUnitAddress)) {
							if ((0xff != bNodeAddress) && (0xff != bUnitAddress)) {
								if (!bindingPropertyUnitSens_pidControlDimUnitVerify(pNodeDatabase,bNodeAddress,bUnitAddress,UNIT_TYPE_DIM)) {
									if (bindingPropertyUnitSens_pidControlUnitBindingsRemove(pBindingFiles,mode,pBindingPropSensUnit)) {
										retVal++;
									}
								}							
							}
						}
					}
				}
			}
		}
	}
	return retVal;
}

/**********************************************************************************************/