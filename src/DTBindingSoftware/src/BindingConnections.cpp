// BindingConnections.cpp: implementation of the CBindingConnections class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "BindingConnections.h"

#include "BindingSoftware.h"
#include "BindingLinks.h"
#include "BindingEntry.h"
#include "BindingGuiComponent.h"


// CAddFlow1
#include "addflow\Node1.h"
#include "addflow\Link1.h"
#include "addflow\Links1.h"
#include "addflow\nodes1.h"
#include "addflow\AddFlow1.h"
#include "addflow\AddflowConst.h"

#include "BindingUnitsOperations.h"
#include "BindingLinkFactory.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CBindingConnections::CBindingConnections(CBindingGuiComponent *pGui)
{
	ASSERT(pGui);
	m_pParent = pGui;
	m_pAddFlow = pGui->GetAddFlowComponent();
}

CBindingConnections::~CBindingConnections()
{
	// Delete Collections !!
}

//////////////////////////////////////////////////////////////////////
BYTE CBindingConnections::GetTypeProperties(unsigned short nID,ControlType Type)
{
	BYTE bType = 0xFF;		// CHANGES_MVS_2008_WARNINGS
	ASSERT (nID);

	CBindingOperators *const pBindingOperators = 
		m_pParent->GetCurrBindingEntry()->GetColOperator();

	CBindingUnits *const pBindingUnits = 
		m_pParent->GetCurrBindingEntry()->GetColUnit();

	ASSERT(pBindingOperators);
	ASSERT(pBindingUnits);

	switch (Type) 
	{
		case PM_UNIT: 
		{
			CBindingUnitsOperations BindingUnitsOperations(pBindingUnits);
			bType = BindingUnitsOperations.GetUnitType(nID);

		} break;
		
		case PM_OPERATOR: 
		{
			bType = (BYTE) pBindingOperators->GetType(nID); 

		}break;

		default: 
		{
			ASSERT(0); 

		} break;
	}
	ASSERT( bType != 0xFF);	// CHANGES_MVS_2008_WARNINGS

	return bType;			
}


/***********************************************************************************
* PRIVATE Function : CBIndingControls::CheckLinkControlsType()
* 
* Task ; Checks type of units/operators involved in current link creation.
*        This function doesn't check units that can be both in/out: 
*		 Virtuals + Sens + BoseRs
*
* returns : True if current link creation is OK!
*		    False if current link creation is forbidden + 
*				displays Messagebox + IDS_ERROR_LNK_XXX
*
* arguments : 	inID / outID : node Id : Binding Collections f(inType)
*				inType / outType : PM_UNIT / PM_OPERATOR
*
* Invoked by : CBindingControls::CheckLink() 
*
* TODO : Add All BindingUnits !!
*
************************************************************************************/


BYTE CBindingConnections::CheckLinkControlsType(CBindingLink *pNewLink)
{  
  const BYTE bSrcType = GetTypeProperties(pNewLink->GetInLinkID(),
										  pNewLink->GetInLinkType());	

  const BYTE bDestType = GetTypeProperties(pNewLink->GetOutLinkID(),
										   pNewLink->GetOutLinkType());
		

/* inputs are UNITS */
	if ((pNewLink->GetInLinkType()) == PM_UNIT)	
	{
		switch(bSrcType)
		{

/* inputs = Dimmers + Switches */
			case BINDING_UNIT_TYPE_DIM:						/* OutPuts --> dest type must be output */
			case BINDING_UNIT_TYPE_SWITCH:
			case BINDING_UNIT_TYPE_AUDIO:
			case BINDING_UNIT_TYPE_AUDIO_HC4:
			// case BINDING_UNIT_TYPE_AUDIO_HC4_BNO:		/* TM_AUDIOHC4_BNO */
				
				if ((pNewLink->GetOutLinkType()) == PM_OPERATOR)
				{
					CString szError;
					szError.LoadString(IDS_ERROR_LNK_OUTOPERATOR);
					AfxMessageBox(szError);
					return FALSE;
				}

				switch (bDestType)
				{
					case BINDING_UNIT_TYPE_DIM:						/* outputs */
					case BINDING_UNIT_TYPE_DIM_FWD:
					case BINDING_UNIT_TYPE_SWITCH:
					case BINDING_UNIT_TYPE_SWITCH_FWD:
					case BINDING_UNIT_TYPE_AUDIO:
					case BINDING_UNIT_TYPE_AUDIO_HC4:
					case BINDING_UNIT_TYPE_AUDIO_HC4_BNO:			/* TM_AUDIOHC4_BNO */
						return TRUE;
					break;

					case BINDING_UNIT_TYPE_DUOSWITCH:				/* special outputs */
					case BINDING_UNIT_TYPE_DUOSWITCH_FWD:
					case BINDING_UNIT_TYPE_IRTX:
						return TRUE;
					break;

					case BINDING_UNIT_TYPE_VIDEOMUX:				/* ??? outputs */
					case BINDING_UNIT_TYPE_AVMATRIX:
					case BINDING_UNIT_TYPE_AVMATRIX_INPUT:			/* TM_AVMATRIX_INPUT */
						return TRUE;
					break;

					case BINDING_UNIT_TYPE_CONTROL:
					case BINDING_UNIT_TYPE_RC5RX:
					{
						CString szError;
						szError.LoadString(IDS_ERROR_LNK_OUTIN);
						AfxMessageBox(szError);						
						return FALSE;

					} break;

					case BINDING_UNIT_TYPE_ALARM_GALAXY:
					case BINDING_UNIT_TYPE_ALARM_ARITECH:
					case BINDING_UNIT_TYPE_ALARM_ANB:		/* TM_CHANGES_ALARM_ANB */
					case BINDING_UNIT_TYPE_ALARM_TEXECOM:	/* TM_CHANGES_ALARM_TEXECOM */
					case BINDING_UNIT_TYPE_ALARM_INTEGRA:	/* TM_CHANGES_ALARM_INTEGRA */
					case BINDING_UNIT_TYPE_BOSERS:			/* In + Outputs */
					case BINDING_UNIT_TYPE_BOSELF48:
					case BINDING_UNIT_TYPE_SENS:
					case BINDING_UNIT_TYPE_SENSUNIV:	/* Sedert versie 0x0D00 */
					case BINDING_UNIT_TYPE_LCD_VIRTUAL:
					case BINDING_UNIT_TYPE_BOSEV35:
					{						
						return TRUE;		/* what to do - Leave for the moment */

					} break;
				}
			break;

/* inputs = duoswitch + IrTx */
			case BINDING_UNIT_TYPE_DUOSWITCH:			/* special outputs */
			case BINDING_UNIT_TYPE_IRTX:
			case BINDING_UNIT_TYPE_VIDEOMUX:
			case BINDING_UNIT_TYPE_AVMATRIX:
			// case BINDING_UNIT_TYPE_AVMATRIX_INPUT:	/* TM_AVMATRIX_INPUT */
	
				if ((pNewLink->GetOutLinkType()) == PM_OPERATOR)
				{
					CString szError;
					szError.LoadString(IDS_ERROR_LNK_OUTOPERATOR);
					AfxMessageBox(szError);
					return FALSE;
				}

				switch (bDestType)
				{
					case BINDING_UNIT_TYPE_DIM:				/* outputs */
					case BINDING_UNIT_TYPE_DIM_FWD:
					case BINDING_UNIT_TYPE_SWITCH:
					case BINDING_UNIT_TYPE_SWITCH_FWD:
					case BINDING_UNIT_TYPE_AUDIO:
					case BINDING_UNIT_TYPE_AUDIO_HC4:
					case BINDING_UNIT_TYPE_AUDIO_HC4_BNO:		/* TM_AUDIOHC4_BNO */
						return TRUE;
					break;

					case BINDING_UNIT_TYPE_DUOSWITCH:	/* special outputs */
					case BINDING_UNIT_TYPE_DUOSWITCH_FWD:
					case BINDING_UNIT_TYPE_IRTX:
						return TRUE;
					break;

					case BINDING_UNIT_TYPE_VIDEOMUX:	/* ??? outputs */
					case BINDING_UNIT_TYPE_AVMATRIX:
					case BINDING_UNIT_TYPE_AVMATRIX_INPUT:	/* TM_AVMATRIX_INPUT */
						return TRUE;
					break;

					case BINDING_UNIT_TYPE_CONTROL:
					case BINDING_UNIT_TYPE_RC5RX:
					{
						CString szError;
						szError.LoadString(IDS_ERROR_LNK_OUTIN);
						AfxMessageBox(szError);						
						return FALSE;
					
					} break;

					case BINDING_UNIT_TYPE_BOSERS:			/* In + Outputs */
					case BINDING_UNIT_TYPE_SENS:
					case BINDING_UNIT_TYPE_SENSUNIV:	/* Sedert versie 0x0D00 */
					case BINDING_UNIT_TYPE_LCD_VIRTUAL:
					case BINDING_UNIT_TYPE_ALARM_GALAXY:
					case BINDING_UNIT_TYPE_ALARM_ARITECH:
					case BINDING_UNIT_TYPE_ALARM_ANB:		/* TM_CHANGES_ALARM_ANB */
					case BINDING_UNIT_TYPE_ALARM_TEXECOM:	/* TM_CHANGES_ALARM_TEXECOM */
					case BINDING_UNIT_TYPE_ALARM_INTEGRA:	/* TM_CHANGES_ALARM_INTEGRA */
					case BINDING_UNIT_TYPE_BOSELF48:
					case BINDING_UNIT_TYPE_BOSEV35:
					{						
						return TRUE;	/* what to do - Leave for the moment */

					} break;
				}
			break;

/* inputs = Control + RC5RX */
			case BINDING_UNIT_TYPE_CONTROL:				/* inputs */
			case BINDING_UNIT_TYPE_RC5RX:
							
				if ((pNewLink->GetOutLinkType()) == PM_OPERATOR) 
					return TRUE;

				switch (bDestType)
				{
					case BINDING_UNIT_TYPE_DIM:			/* outputs */
					case BINDING_UNIT_TYPE_DIM_FWD:
					case BINDING_UNIT_TYPE_SWITCH:
					case BINDING_UNIT_TYPE_SWITCH_FWD:
					case BINDING_UNIT_TYPE_AUDIO:
					case BINDING_UNIT_TYPE_AUDIO_HC4:
					case BINDING_UNIT_TYPE_AUDIO_HC4_BNO:		/* TM_AUDIOHC4_BNO */
						return TRUE;
					break;

					case BINDING_UNIT_TYPE_DUOSWITCH:	/* special outputs */
					case BINDING_UNIT_TYPE_DUOSWITCH_FWD:
					case BINDING_UNIT_TYPE_IRTX:
						return TRUE;
					break;

					case BINDING_UNIT_TYPE_VIDEOMUX:
					case BINDING_UNIT_TYPE_AVMATRIX:
					case BINDING_UNIT_TYPE_AVMATRIX_INPUT:	/* TM_AVMATRIX_INPUT */
						return TRUE;
					break;

					case BINDING_UNIT_TYPE_RC5RX:
					case BINDING_UNIT_TYPE_CONTROL:
					{
						CString szError;
						szError.LoadString(IDS_ERROR_LNK_2IN);
						AfxMessageBox(szError);
						return FALSE;

					} break;

					case BINDING_UNIT_TYPE_ALARM_GALAXY:	/* ?? */
					case BINDING_UNIT_TYPE_ALARM_ARITECH:
					case BINDING_UNIT_TYPE_ALARM_ANB:		/* TM_CHANGES_ALARM_ANB */	
					case BINDING_UNIT_TYPE_ALARM_TEXECOM:	/* TM_CHANGES_ALARM_TEXECOM */
					case BINDING_UNIT_TYPE_ALARM_INTEGRA:	/* TM_CHANGES_ALARM_INTEGRA */
					case BINDING_UNIT_TYPE_SENS:
					case BINDING_UNIT_TYPE_SENSUNIV:	/* Sedert versie 0x0D00 */
					case BINDING_UNIT_TYPE_BOSERS:
					case BINDING_UNIT_TYPE_BOSELF48:
					case BINDING_UNIT_TYPE_LCD_VIRTUAL:
					case BINDING_UNIT_TYPE_BOSEV35:
					{						
						return TRUE;	/* what to do - Leave for the moment */

					} break;
				}
			break;

/* input = virtual */
			case BINDING_UNIT_TYPE_LCD_VIRTUAL:			/* IN + Output */
			case BINDING_UNIT_TYPE_DIM_FWD:	
			case BINDING_UNIT_TYPE_SWITCH_FWD:
			case BINDING_UNIT_TYPE_DUOSWITCH_FWD:
			case BINDING_UNIT_TYPE_SENS:
			case BINDING_UNIT_TYPE_SENSUNIV:	/* Sedert versie 0x0D00 */
			case BINDING_UNIT_TYPE_BOSERS:
			case BINDING_UNIT_TYPE_ALARM_GALAXY:				/* ?? */
			case BINDING_UNIT_TYPE_ALARM_ARITECH:
			case BINDING_UNIT_TYPE_ALARM_ANB:			/* TM_CHANGES_ALARM_ANB */	
			case BINDING_UNIT_TYPE_ALARM_TEXECOM:		/* TM_CHANGES_ALARM_TEXECOM */
			case BINDING_UNIT_TYPE_ALARM_INTEGRA:		/* TM_CHANGES_ALARM_INTEGRA */
			case BINDING_UNIT_TYPE_BOSELF48:
			case BINDING_UNIT_TYPE_BOSEV35:
			case BINDING_UNIT_TYPE_AVMATRIX_INPUT:		/* TM_AVMATRIX_INPUT */
			case BINDING_UNIT_TYPE_AUDIO_HC4_BNO:		/* TM_AUDIOHC4_BNO */
			{
				if ((pNewLink->GetOutLinkType()) == PM_OPERATOR)
				{
					/* only if virtual is used as input */
					return TRUE;
				}

				switch (bDestType)
				{
					case BINDING_UNIT_TYPE_DIM:			/* outputs */
					case BINDING_UNIT_TYPE_DIM_FWD:	
					case BINDING_UNIT_TYPE_SWITCH:
					case BINDING_UNIT_TYPE_SWITCH_FWD:
					case BINDING_UNIT_TYPE_AUDIO:
					case BINDING_UNIT_TYPE_AUDIO_HC4:
					case BINDING_UNIT_TYPE_AUDIO_HC4_BNO:		/* TM_AUDIOHC4_BNO */
						return TRUE;
					break;

					case BINDING_UNIT_TYPE_DUOSWITCH:	/* special outputs */
					case BINDING_UNIT_TYPE_DUOSWITCH_FWD:
					case BINDING_UNIT_TYPE_IRTX:
						return TRUE;
					break;

					case BINDING_UNIT_TYPE_VIDEOMUX:	/* ?? */
					case BINDING_UNIT_TYPE_AVMATRIX:
					case BINDING_UNIT_TYPE_AVMATRIX_INPUT:	/* TM_AVMATRIX_INPUT */
						return TRUE;
					break;

					case BINDING_UNIT_TYPE_CONTROL:
					case BINDING_UNIT_TYPE_RC5RX:
					{
						CString szError;
						szError.LoadString(IDS_ERROR_LNK_2IN);
						AfxMessageBox(szError);
						return FALSE;

					} break;

					case BINDING_UNIT_TYPE_ALARM_GALAXY:		/* ?? */
					case BINDING_UNIT_TYPE_ALARM_ARITECH:
					case BINDING_UNIT_TYPE_ALARM_ANB:		/* TM_CHANGES_ALARM_ANB */	
					case BINDING_UNIT_TYPE_ALARM_TEXECOM:	/* TM_CHANGES_ALARM_TEXECOM */
					case BINDING_UNIT_TYPE_ALARM_INTEGRA:	/* TM_CHANGES_ALARM_INTEGRA */
					case BINDING_UNIT_TYPE_LCD_VIRTUAL:
					case BINDING_UNIT_TYPE_BOSERS:
					case BINDING_UNIT_TYPE_BOSELF48:
					case BINDING_UNIT_TYPE_SENS:
					case BINDING_UNIT_TYPE_SENSUNIV:	/* Sedert versie 0x0D00 */
					case BINDING_UNIT_TYPE_BOSEV35:
					{					
						return TRUE;	/* what to do - Leave for the moment */

					} break;
				}

			} break;
		}
	}
	//else if (inType == PM_OPERATOR)
	else if ((pNewLink->GetInLinkType()) == PM_OPERATOR)
	{
		switch ((OperatorType) bSrcType)
		{
			case PM_OPERATOR_AND:
			case PM_OPERATOR_OR:
			case PM_OPERATOR_XOR:
				
				//if (outType == PM_OPERATOR) {
				if ((pNewLink->GetOutLinkType()) == PM_OPERATOR)
				{
					// CHANDED 2004/03/15
					return TRUE;							
				}
				//else if (outType == PM_UNIT)
				if ((pNewLink->GetOutLinkType()) == PM_UNIT)
				{
					switch(bDestType)
					{
						case BINDING_UNIT_TYPE_DIM:			/* outputs */
						case BINDING_UNIT_TYPE_DIM_FWD:
						case BINDING_UNIT_TYPE_SWITCH:
						case BINDING_UNIT_TYPE_SWITCH_FWD:
						case BINDING_UNIT_TYPE_AUDIO:
						case BINDING_UNIT_TYPE_AUDIO_HC4:
						case BINDING_UNIT_TYPE_AUDIO_HC4_BNO:		/* TM_AUDIOHC4_BNO */
							return TRUE;
						break;

						case BINDING_UNIT_TYPE_DUOSWITCH:	/* special outputs */
						case BINDING_UNIT_TYPE_DUOSWITCH_FWD:
						case BINDING_UNIT_TYPE_IRTX:
							return TRUE;
						break;

						case BINDING_UNIT_TYPE_AVMATRIX:
						case BINDING_UNIT_TYPE_AVMATRIX_INPUT:	/* TM_AVMATRIX_INPUT */
						case BINDING_UNIT_TYPE_VIDEOMUX:
							return TRUE;
						break;

						case BINDING_UNIT_TYPE_CONTROL:
						case BINDING_UNIT_TYPE_RC5RX:
						{
							CString szError;
							szError.LoadString(IDS_ERROR_LNK_OPERATORIN);
							AfxMessageBox(szError);
							return FALSE;

						} break;

						case BINDING_UNIT_TYPE_ALARM_GALAXY:
						case BINDING_UNIT_TYPE_ALARM_ARITECH:
						case BINDING_UNIT_TYPE_ALARM_ANB:		/* TM_CHANGES_ALARM_ANB */	
						case BINDING_UNIT_TYPE_ALARM_TEXECOM:	/* TM_CHANGES_ALARM_TEXECOM */
						case BINDING_UNIT_TYPE_ALARM_INTEGRA:	/* TM_CHANGES_ALARM_INTEGRA */
						case BINDING_UNIT_TYPE_SENS:
						case BINDING_UNIT_TYPE_SENSUNIV:	/* Sedert versie 0x0D00 */
						case BINDING_UNIT_TYPE_BOSERS:
						case BINDING_UNIT_TYPE_BOSELF48:
						case BINDING_UNIT_TYPE_LCD_VIRTUAL:
						case BINDING_UNIT_TYPE_BOSEV35:
						{								
							return TRUE;	/* what to do - Leave for the moment */

						} break;
					}
				}
				else 
				{
					CString szError;
					szError.LoadString(IDS_ERROR_LNK_UNDEFINED);
					AfxMessageBox(szError);
					return FALSE;
				}
			break;

			case PM_OPERATOR_TIMER:
			
				if ((pNewLink->GetOutLinkType()) == PM_UNIT)
				{
					switch(bDestType)
					{
						case BINDING_UNIT_TYPE_DIM:					/* outputs */
						case BINDING_UNIT_TYPE_DIM_FWD:
						case BINDING_UNIT_TYPE_SWITCH:
						case BINDING_UNIT_TYPE_SWITCH_FWD:
						case BINDING_UNIT_TYPE_AUDIO:
						case BINDING_UNIT_TYPE_AUDIO_HC4:
						case BINDING_UNIT_TYPE_AUDIO_HC4_BNO:		/* TM_AUDIOHC4_BNO */
							return TRUE;
						break;

						case BINDING_UNIT_TYPE_DUOSWITCH:			/* special outputs */
						case BINDING_UNIT_TYPE_DUOSWITCH_FWD:
						case BINDING_UNIT_TYPE_IRTX:
							return TRUE;
						break;

						case BINDING_UNIT_TYPE_AVMATRIX:			/* ??? */
						case BINDING_UNIT_TYPE_AVMATRIX_INPUT:		/* TM_AVMATRIX_INPUT */
						case BINDING_UNIT_TYPE_VIDEOMUX:
							return TRUE;
						break;

						case BINDING_UNIT_TYPE_CONTROL:
						case BINDING_UNIT_TYPE_RC5RX:
						{
							CString szError;
							szError.LoadString(IDS_ERROR_LNK_OPERATORIN);
							AfxMessageBox(szError);
							return FALSE;

						} break;

						case BINDING_UNIT_TYPE_ALARM_GALAXY:	/* ??*/	
						case BINDING_UNIT_TYPE_ALARM_ARITECH:
						case BINDING_UNIT_TYPE_ALARM_ANB:		/* TM_CHANGES_ALARM_ANB */
						case BINDING_UNIT_TYPE_ALARM_TEXECOM:	/* TM_CHANGES_ALARM_TEXECOM */
						case BINDING_UNIT_TYPE_ALARM_INTEGRA:	/* TM_CHANGES_ALARM_INTEGRA */
						case BINDING_UNIT_TYPE_LCD_VIRTUAL:
						case BINDING_UNIT_TYPE_SENS:
						case BINDING_UNIT_TYPE_SENSUNIV:	/* Sedert versie 0x0D00 */
						case BINDING_UNIT_TYPE_BOSERS:
						case BINDING_UNIT_TYPE_BOSELF48:
						case BINDING_UNIT_TYPE_BOSEV35:
						{						
							return TRUE;	/* what to do - Leave for the moment */

						} break;
					}
				}			
				//else if (outType == PM_OPERATOR)
				else if ((pNewLink->GetOutLinkType()) == PM_OPERATOR)
				{
					return TRUE;
				}
			break;
		}
	}
	 
	if (1)
	{
		CString szError;
		szError.LoadString(IDS_ERROR_LNK_UNDEFINED);
		AfxMessageBox(szError);
	}

	return FALSE;
}



BYTE CBindingConnections::CheckLinkNLinks(CNode1 afSrcNode,CNode1 afDestNode,CBindingLink *pNewLink)
{
	//CLinks1 links;

	const BYTE bSrcType = GetTypeProperties(pNewLink->GetInLinkID(),
											pNewLink->GetInLinkType());	
	const BYTE bDestType = GetTypeProperties(pNewLink->GetOutLinkID(),
											 pNewLink->GetOutLinkType());
	

	if ((pNewLink->GetOutLinkType()) == PM_OPERATOR)
	{
		if (((OperatorType) bDestType) == PM_OPERATOR_TIMER) 
		{
			/* Check NR of Inlinks > 1 ? */
			CLinks1 links = afDestNode.GetInLinks();
			if (links.GetCount() > 1) 
			{	
				CString szError;
				szError.LoadString(IDS_ERROR_LNK_NINLINKS);
				AfxMessageBox(szError);
				return FALSE;
			}
		}
	}
		
	/* Check Src Control */
	switch (pNewLink->GetInLinkType())		//(inType)
	{
		case PM_UNIT:
		{
			/* only 1 outlink allowed */
			CLinks1 links = afSrcNode.GetOutLinks();

			if (links.GetCount() > 1) 
			{
				CString szError;
				szError.LoadString(IDS_ERROR_LNK_NOUTLINKS);
				AfxMessageBox(szError);
				return FALSE;
			}

			//switch (outType)
			switch(pNewLink->GetOutLinkType())
			{
				case PM_UNIT:
				{
					CLinks1 links = afDestNode.GetInLinks();
					if (links.GetCount() > 1) 
					{
						CString szError;
						szError.LoadString(IDS_ERROR_LNK_NINLINKS);
						AfxMessageBox(szError);
						return FALSE;
					}					
				} break;

				case PM_OPERATOR:
					/* Dest or src must be timer */
				break;
			}
		} break;

		case PM_OPERATOR:
		{
			/* only 1 outlink allowed */
			CLinks1 links = afSrcNode.GetOutLinks();
			if (links.GetCount() > 1) 
			{
				CString szError;
				szError.LoadString(IDS_ERROR_LNK_NOUTLINKS);
				AfxMessageBox(szError);
				return FALSE;
			}
			switch (pNewLink->GetOutLinkType())//		(outType)
			{
				case PM_UNIT:
				{
					CLinks1 links = afDestNode.GetInLinks();
					
					if (links.GetCount() > 1) 
					{
						CString szError;
						szError.LoadString(IDS_ERROR_LNK_NINLINKS);
						AfxMessageBox(szError);
						return FALSE;
					}
					
					/* dest unit must be operator or
					   output unit 
					*/
					
				} break;

				case PM_OPERATOR:
					/* Dest or src must be timer */
				break;
			}
		} break;
	}
	return (TRUE);
}


/***********************************************************************************
* PRIVATE Function : CBIndingControls::CheckLinkinoutType()
* 
* Task ; Checks if in current link creation virtual,sens or BoseRs units are involved,
*        via inlinks + outlinks tries to figure out if these units are used
*        as input / output.
*
* returns : True if current link creation is OK!
*		    False if current link creation is forbidden +
*				displays Messagebox + IDS_ERROR_LNK_XXX
*
* arguments : ->Source node + destination node
*				afSrcNode / afdestNode : addflow CNode1
*				inID / outID : node Id : Binding Collections f(inType)
*				inType / outType : PM_UNIT / PM_OPERATOR
*
* Invoked by : CBindingControls::CheckLink() 
*
* See documentation !
*
*
* TODO : Add All BindingUnits !!
*
************************************************************************************/

BYTE CBindingConnections::CheckLinkinoutType(CNode1 afSrcNode,CNode1 afDestNode,CBindingLink *pLink)
{  
  CBindingLink *pOldLink = NULL; 
  int id = -1;		// CHANGES_MVS_2008_SSCANF

  CBindingLinks *const pBindingLinks = 
	  m_pParent->GetCurrBindingEntry()->GetColLink();

  ASSERT(pBindingLinks);
  CBindingUnits *const pBindingUnits = 
	  m_pParent->GetCurrBindingEntry()->GetColUnit();

  ASSERT(pBindingUnits);

  VARIANT vaIdx;
  VariantInit(&vaIdx);
  vaIdx.vt = VT_I4;

  const BYTE bSrcType = GetTypeProperties(pLink->GetInLinkID(),pLink->GetInLinkType());	
  const BYTE bDestType = GetTypeProperties(pLink->GetOutLinkID(),pLink->GetOutLinkType());		

  if ((bSrcType == BINDING_UNIT_TYPE_LCD_VIRTUAL) ||
	  (bSrcType == BINDING_UNIT_TYPE_SENS) ||
	  (bSrcType == BINDING_UNIT_TYPE_SENSUNIV) ||
	  (bSrcType == BINDING_UNIT_TYPE_BOSERS) ||
	  (bSrcType == BINDING_UNIT_TYPE_BOSELF48) ||
	  (bSrcType == BINDING_UNIT_TYPE_BOSEV35) ||		
	  (bSrcType == BINDING_UNIT_TYPE_ALARM_GALAXY) ||
	  (bSrcType == BINDING_UNIT_TYPE_ALARM_ARITECH) ||
	  (bSrcType == BINDING_UNIT_TYPE_ALARM_ANB) ||		/* TM_CHANGES_ALARM_ANB */
	  (bSrcType == BINDING_UNIT_TYPE_ALARM_TEXECOM)||	/* TM_CHANGES_ALARM_TEXECOM */
	  (bSrcType == BINDING_UNIT_TYPE_ALARM_INTEGRA))	/* TM_CHANGES_ALARM_INTEGRA */
  {	  	  
	/* 1) Check if virtual already has inlinks */	  
	CLinks1 links = afSrcNode.GetInLinks();
	if (links.GetCount() != 0) 
	{			
		/* get link property out of collection */
		vaIdx.lVal = 1;
		CLink1 link = links.Item(vaIdx);

		CString szTag(link.GetTag());	

		if(!sscanf((LPCTSTR) szTag,"%d",&id))		// CHANGES_MVS_2008_SSCANF, CHANGES_MVS_2008_CSTRING
		{
			CString szError;
			szError.LoadString(IDS_ERROR_LINKS_TAG);
			AfxMessageBox(szError);
			return FALSE;
		}	

		ASSERT (id);	

		if ((pOldLink = pBindingLinks->Find(id)) == 0)
		{
			ASSERT(pOldLink);		
			return FALSE;
		}
		
		switch (pOldLink->GetInLinkType())		//(In.Type)
		{
			case PM_OPERATOR:	/* handle Situation1 : lnk Operator -> virtual -> input */
			{
				/* must not link to input ! */
				//if (outType == PM_UNIT) 
				if (pLink->GetOutLinkType() == PM_UNIT)
				{
					if ((bDestType == BINDING_UNIT_TYPE_CONTROL) ||
						(bDestType == BINDING_UNIT_TYPE_RC5RX))
					{
						CString szError;
						szError.LoadString(IDS_ERROR_LNK_OUTIN);
						AfxMessageBox(szError);
						return FALSE;
					}				
				}
			}break;
		
			case PM_UNIT:		/* lnk Unit -> Virtual */
			{							
				if (pLink->GetOutLinkType() == PM_OPERATOR)	//(outType == PM_OPERATOR)
				{
					/* handle situation3 : lnk input -> virtual -> operator */
					CString szError;
					szError.LoadString(IDS_ERROR_LNK_OUTOPERATOR);
					AfxMessageBox(szError);
					return FALSE;
				}
				
				if (pLink->GetOutLinkType() == PM_UNIT)	//(outType == PM_UNIT)
				{
					/* situation2 : already handled by previous checks  */
					/* input -> virtual -> input */
				
					CBindingUnitsOperations BindingUnitsOperations(pBindingUnits);
					const BYTE bUnitType = BindingUnitsOperations.GetUnitType(pLink->GetOutLinkID());

					if ((bUnitType == BINDING_UNIT_TYPE_CONTROL) ||
						(bUnitType == BINDING_UNIT_TYPE_RC5RX))						
					{
						CString szError;
						szError.LoadString(IDS_ERROR_LNK_OUTIN);
						AfxMessageBox(szError);
						return FALSE;
					}				
				}
			} break;
		} /*end switch */
	}/* if inlinks */
  }

  if ((bDestType == BINDING_UNIT_TYPE_LCD_VIRTUAL) ||
	  (bDestType == BINDING_UNIT_TYPE_SENS) ||
	  (bDestType == BINDING_UNIT_TYPE_SENSUNIV) ||
	  (bDestType == BINDING_UNIT_TYPE_BOSERS) ||
	  (bDestType == BINDING_UNIT_TYPE_BOSELF48) ||
	  (bDestType == BINDING_UNIT_TYPE_BOSEV35) ||
	  (bDestType == BINDING_UNIT_TYPE_ALARM_GALAXY) ||
	  (bDestType == BINDING_UNIT_TYPE_ALARM_ARITECH) ||
	  (bDestType == BINDING_UNIT_TYPE_ALARM_ANB) ||		/* TM_CHANGES_ALARM_ANB */
	  (bDestType == BINDING_UNIT_TYPE_ALARM_TEXECOM) ||	/* TM_CHANGES_ALARM_TEXECOM */
	  (bDestType == BINDING_UNIT_TYPE_ALARM_INTEGRA))	/* TM_CHANGES_ALARM_INTEGRA */
  {
    /* Get existing out Link */
	CLinks1 links = afDestNode.GetOutLinks();
	if (links.GetCount() != 0) 
	{					
		vaIdx.lVal = 1;
		CLink1 link = links.Item(vaIdx);
		
		CString szTag(link.GetTag());	
		if(!sscanf((LPCTSTR) szTag,"%d",&id))	// CHANGES_MVS_2008_SSCANF, CHANGES_MVS_2008_CSTRING
		{
			CString szError;
			szError.LoadString(IDS_ERROR_LINKS_TAG);
			AfxMessageBox(szError);
			return FALSE;
		}	

		ASSERT (id);
		//if (!pLinks->Get(nID,&In,&Out,&bInverse)) return FALSE;
		
		if ((pOldLink = pBindingLinks->Find(id)) == 0)
		{
			ASSERT(pOldLink);
			return FALSE;
		}

		switch (pOldLink->GetOutLinkType())	//(Out.Type)
		{
			case PM_OPERATOR:
				if (pLink->GetInLinkType() == PM_UNIT)	//(inType == PM_UNIT)
				{				
					CString szError;
					szError.LoadString(IDS_ERROR_LNK_2IN);
					AfxMessageBox(szError);
					return FALSE;				
				}
			break;
		}
	}
  }
  return TRUE;
}


//////////////////////////////////////////////////////////////////////
BOOL CBindingConnections::Addlink(CBindingLink *pLink)
{	
	char ctype = '\0';			// CHANGES_MVS_2008_SSCANF
	int id2 = -1;			// CHANGES_MVS_2008_SSCANF
	CNode1 afSrcNode = 0;
	CNode1 afDestNode = 0;
	
	VARIANT vaIdx;
	VariantInit(&vaIdx);
	vaIdx.vt = VT_I4;

	for (int i = 1, nCount = m_pAddFlow->GetNodes().GetCount(); 
		 i <= nCount; 
		 i++)
	{
		vaIdx.lVal = i;
		CNode1 node = m_pAddFlow->GetNodes().Item(vaIdx);
		
		CString szTag(node.GetTag());

		if(!sscanf((LPCTSTR) szTag,"%c,%d",&ctype,&id2))			// CHANGES_MVS_2008_SSCANF, CHANGES_MVS_2008_CSTRING
		{
			CString szError;
			szError.LoadString(IDS_ERROR_LINKS_TAG);
			AfxMessageBox(szError);
			return FALSE;
		}

		switch(ctype)
		{
			case 'U':
			{
				if ((pLink->GetInLinkType() == PM_UNIT) && 
					(pLink->GetInLinkID() == id2))
				{
					afSrcNode = node;
				}
				else if ((pLink->GetOutLinkType() == PM_UNIT) && 
					     (pLink->GetOutLinkID() == id2))
				{
					afDestNode = node;
				}
			} break;

			case 'O':
			case 'T':
			{
				if ((pLink->GetInLinkType() == PM_OPERATOR) && 
					(pLink->GetInLinkID() == id2))
				{
					afSrcNode = node;
				}
				else if ((pLink->GetOutLinkType() == PM_OPERATOR) && 
						 (pLink->GetOutLinkID() == id2))
				{
					afDestNode = node;
				}
			} break;
		}
	}

	if ((afDestNode ==NULL) || (afSrcNode == NULL)) 
	{
		CString szError;
		szError.LoadString(IDS_ERROR_LINKS_NODESNOTFOUND);
		AfxMessageBox(szError);	
		return FALSE;
	}

	if (CheckLink(afSrcNode,afDestNode,pLink))
	{
		CLink1 Lnk = afSrcNode.GetOutLinks().Add(afDestNode);
	
		CString szTag;
		szTag.Format("%d",pLink->GetID());

		Lnk.SetTag(szTag);

		if (pLink->isInverted() == TRUE) 
			Lnk.SetDrawColor(RGB(255,0,0));
		else
			Lnk.SetDrawColor(RGB(0,0,0));

		return TRUE;
	}
	
	return FALSE;
}

BYTE CBindingConnections::Add(CLink1 lnk)
{
	int id = -1;				// CHANGES_MVS_2008_SSCANF
	char ctype = '\0';			// CHANGES_MVS_2008_SSCANF
	BINDING_CONNECTION In,Out;
	CLink1 Lnk,srclnk;

	CBindingLinks* const pBindingLinks = 
		m_pParent->GetCurrBindingEntry()->GetColLink();
    ASSERT(pBindingLinks);

	CNode1 afDestNode = NULL;
	CNode1 afSrcNode = NULL;

	VARIANT vaIdx;
	VariantInit(&vaIdx);			
	vaIdx.vt = VT_I4;

	CNodes1 nodes = m_pAddFlow->GetNodes();
	for (int i = 1,n = nodes.GetCount(); 
		 i <= n; 
		 i++)
	{
		vaIdx.lVal = i;	
		CNode1 node = nodes.Item(vaIdx);
		
		if (1)
		{
			CLinks1 links = node.GetOutLinks();
			for (int k = 1,nCount = links.GetCount(); 
				 k <= nCount; 
				 k++)
			{
				vaIdx.lVal = k;
				srclnk = links.Item(vaIdx);
			
				if (lnk == srclnk)
					afSrcNode = node;
			}
		}

		if (1)
		{
			CLinks1 links = node.GetInLinks();		
			for (int k = 1,nCount = links.GetCount(); 
				 k <= nCount; 
				 k++)
			{
				vaIdx.lVal = k;
				srclnk = links.Item(vaIdx);
			
				if (lnk == srclnk)
					afDestNode = node;
			}
		}
	}

	if ((afSrcNode == NULL) || (afDestNode == NULL)) 
	{
		CString szError;
		szError.LoadString(IDS_ERROR_LINKS_NODESNOTFOUND);
		AfxMessageBox(szError);	
		return FALSE;
	}

	if (1)
	{
		/* Tag Src Node */
		CString szTag(afSrcNode.GetTag());
		if(!sscanf((LPCTSTR) szTag,"%c,%d",&ctype,&id))		// CHANGES_MVS_2008_SSCANF, CHANGES_MVS_2008_CSTRING
		{	
			CString szError;
			szError.LoadString(IDS_ERROR_LINKS_TAG);
			AfxMessageBox(szError);
			return FALSE;
		}

		ASSERT(id>=0);		// ????
		In.m_nID = id;
	
		if (ctype == 'U')
		{
			In.m_Type = PM_UNIT;
		}
		else if ((ctype =='O') || (ctype =='T'))
		{
			In.m_Type = PM_OPERATOR;
		}
		else 
		{
			CString szError;
			szError.LoadString(IDS_ERROR_LINKS_TAG);
			AfxMessageBox(szError);
			return FALSE;
		}
	}

	if (1)
	{
		/* Tag Dest Node */
		CString szTag(afDestNode.GetTag());
		if(!sscanf((LPCTSTR) szTag,"%c,%d",&ctype,&id))		// CHANGES_MVS_2008_SSCANF, CHANGES_MVS_2008_CSTRING
		{
			CString szError;
			szError.LoadString(IDS_ERROR_LINKS_TAG);
			AfxMessageBox(szError);
			return FALSE;
		}
	
		ASSERT(id>=0);

		Out.m_nID = id;
		if (ctype == 'U')
		{
			Out.m_Type = PM_UNIT;
		}
		else if ((ctype =='O') || (ctype =='T'))
		{
			Out.m_Type = PM_OPERATOR;
		}
		else 
		{
			CString szError;
			szError.LoadString(IDS_ERROR_LINKS_TAG);
			AfxMessageBox(szError);
			return FALSE;
		}
	}


	/* TODO : check if link is possible 
	   yes : add Link to dbase
	   no : delete lnk AddFlow
	*/
	
	CBindingLink* pLink;
	if ((pLink = CBindingLinkFactory::Create()) == 0)
	{
		ASSERT(pLink);
		return 0;
	}

	pLink->SetInLink(In.m_nID,In.m_Type);
	pLink->SetOutLink(Out.m_nID,Out.m_Type);
	pLink->SetInverse(FALSE);

	if (CheckLink(afSrcNode,afDestNode,pLink)) 
	{
		pLink->SetID(0);	// !!!

		id = pBindingLinks->Add(pLink);		

		ASSERT(id);
		ASSERT(id == pLink->GetID());
		
		if (id == 0) 
		{
			CString szError;
			szError.LoadString(IDS_ERROR_LINKS_ADDLINK);
			AfxMessageBox(szError);	
			return FALSE;
		}

		CString szTag;
		szTag.Format("%d",id);
		lnk.SetTag(szTag);
	}
	else 
	{
		if (pLink != NULL) 
		{
			delete pLink;
			pLink = NULL;
		}

		lnk.SetMarked(TRUE);
		m_pAddFlow->DeleteMarked();

		return FALSE;
	}
	return TRUE;
}

unsigned short CBindingConnections::Add(unsigned short InID, ControlType InType, unsigned short OutID, ControlType OutType, BYTE bInvers)
{	
	char ctype = '\0';
	CBindingLinks* const pBindingLinks = m_pParent->GetCurrBindingEntry()->GetColLink();
    ASSERT(pBindingLinks);
	
	CBindingLink* const pLink = CBindingLinkFactory::Create();

	if (pLink == 0)
	{
		ASSERT(pLink);
		return 0;
	}

	ASSERT(bInvers==FALSE || bInvers==TRUE);	// USE_IMPLEMENTATION_SOLVE_BUG_0157

	pLink->SetInLink(InID,InType);
	pLink->SetOutLink(OutID,OutType);	
	pLink->SetInverse(bInvers);

	pLink->SetID(0); 
	int id = pBindingLinks->Add(pLink);			// CHANGES_MVS_2008_SSCANF
	
	ASSERT(id);
	ASSERT(id == pLink->GetID());

	if (id == 0) 
	{
		CString szError;
		szError.LoadString(IDS_ERROR_LINKS_ADDLINK);
		AfxMessageBox(szError);
		return 0;
	}

	CNode1 afSrcNode = NULL;
	CNode1 afDestNode = NULL;
	VARIANT vaIdx;

	VariantInit(&vaIdx);
	vaIdx.vt = VT_I4;

	for (int i = 1,nCount = m_pAddFlow->GetNodes().GetCount(); 
		 i <= nCount; 
		 i++)
	{
		vaIdx.lVal = i;

		CNode1 node = m_pAddFlow->GetNodes().Item(vaIdx);
		CString szTag(node.GetTag());

		unsigned short id2;
		int tempValue = 0;
		
		if(!sscanf((LPCTSTR) szTag,"%c,%d",&ctype,&tempValue))		// CHANGES_MVS_2008_SSCANF, CHANGES_MVS_2008_CSTRING
		{
			CString szError;
			szError.LoadString(IDS_ERROR_LINKS_TAG);
			AfxMessageBox(szError);
			return 0;
		}
		else
		{
			id2 = tempValue;
		}

		switch(ctype)
		{
			case 'U':
				if ((InType == PM_UNIT) && (InID == id2))
				{
					afSrcNode = node;
				}
				else if ((OutType == PM_UNIT) && (id2 == OutID))
				{
					afDestNode = node;
				}
			break;

			case 'O':
			case 'T':
				if ((InType == PM_OPERATOR) && (InID == id2))
				{
					afSrcNode = node;
				}
				else if ((OutType == PM_OPERATOR) && (id2 == OutID))
				{
					afDestNode = node;
				}
			break;
		}
	}

	if ((afDestNode ==NULL) || (afSrcNode == NULL)) 
	{
		CString szError;
		szError.LoadString(IDS_ERROR_LINKS_NODESNOTFOUND);
		AfxMessageBox(szError);	
		return NULL;
	}

	/* Check if Lnk is possible */
	if (CheckLink(afSrcNode,afDestNode,pLink)) 
	{
		CString szTag;
		CLink1 Lnk = afSrcNode.GetOutLinks().Add(afDestNode);

		szTag.Format("%d",id);
		Lnk.SetTag(szTag);

		if (bInvers == TRUE) 
			Lnk.SetDrawColor(RGB(255,0,0));
		else 
			Lnk.SetDrawColor(RGB(0,0,0));

		return (id);
	}
	else return NULL;
}




BYTE CBindingConnections::Remove(CLink1 lnk)
{
	int id = -1;									// CHANGES_MVS_2008_SSCANF
	CBindingLinks* const pBindingLinks = 
		m_pParent->GetCurrBindingEntry()->GetColLink();

    ASSERT(pBindingLinks);

	CString szTag(lnk.GetTag());
	if(!sscanf((LPCTSTR) szTag,"%d",&id))		// CHANGES_MVS_2008_SSCANF, CHANGES_MVS_2008_CSTRING
	{
		CString szError;
		szError.LoadString(IDS_ERROR_LINKS_TAG);
		AfxMessageBox(szError);
		return FALSE;
	}

	if (!(pBindingLinks->Remove(id)))	
		return FALSE;	

	lnk.SetMarked(TRUE);
	m_pAddFlow->DeleteMarked();
	return TRUE;
}


BYTE CBindingConnections::Set(CLink1 lnk, BYTE bInvert)
{
	int id = -1;			// CHANGES_MVS_2008_SSCANF
	CBindingLinks* const pBindingLinks = 
		m_pParent->GetCurrBindingEntry()->GetColLink();

    ASSERT(pBindingLinks);
	
	CString szTag(lnk.GetTag());
	if(!sscanf((LPCTSTR) szTag,"%d",&id))	// CHANGES_MVS_2008_SSCANF, CHANGES_MVS_2008_CSTRING
	{
		CString szError;
		szError.LoadString(IDS_ERROR_LINKS_TAG);
		AfxMessageBox(szError);
		return FALSE;
	}

	CBindingLink* const pLink = pBindingLinks->Find(id);
	if (pLink == NULL) 
	{	
		ASSERT(pLink);
		return FALSE;
	}

	BOOL fInverse = pLink->isInverted();

	/* if Mode == TOGGLE */
	if (bInvert == TOGGLE) 
	{
		if (fInverse == FALSE) 
			fInverse = TRUE;
		else 
			fInverse = FALSE;
	}
	else 
	{ 
		ASSERT(bInvert==FALSE || bInvert==TRUE);	// USE_IMPLEMENTATION_SOLVE_BUG_0157
		fInverse = (BOOL) bInvert;
	}

	pLink->SetInverse(fInverse);

	/* AddFlow */
	if (fInverse == FALSE) 
		lnk.SetDrawColor(RGB(0,0,0));
	else 
		lnk.SetDrawColor(RGB(255,0,0));

	return (TRUE);
}




BYTE CBindingConnections::Remove(CNode1 node, LinkType Type)
{
	CLinks1 links;
	int k;
	int id;

	CBindingLinks *const pBindingLinks = 
		m_pParent->GetCurrBindingEntry()->GetColLink();

    ASSERT(pBindingLinks);


	VARIANT vaIdx;
	VariantInit(&vaIdx);
	vaIdx.vt = VT_I4;

	switch(Type)
	{
		case PM_INLINKS: 
			links = node.GetInLinks(); 
		break;
		
		case PM_OUTLINKS: 
			links = node.GetOutLinks(); 
		break;
		
		case PM_ALLLINKS: 
			links = node.GetLinks(); 
		break;
	}

	// MUST DO FIRST LOOP FOR LINKS IN CBindingLinks Dbase
	const int nCount = links.GetCount();
	for (k = 1; k <= links.GetCount() ; k++)
	{
		vaIdx.lVal = k;

		CLink1 lnk = links.Item(vaIdx);

		/* Get Tag */
		CString szTag(lnk.GetTag());
		if(!sscanf((LPCTSTR) szTag,"%d",&id))		// CHECKED_SSCANF_OK, CHANGES_MVS_2008_CSTRING
		{
			CString szError;
			szError.LoadString(IDS_ERROR_LINKS_TAG);
			AfxMessageBox(szError);
			return FALSE;
		}	

		/* Delete out of dbase */
		if (!(pBindingLinks->Remove(id))) 
			return FALSE;
	}

	// MUST DO SECOND LOOP FOR ADDFLOW DATABASE
	for (k = 1; k <= links.GetCount() ; k++)
	{
		vaIdx.lVal = k;

		CLink1 lnk = links.Item(vaIdx);

		// delete addflow
		lnk.SetMarked(TRUE);
		m_pAddFlow->DeleteMarked();
	}
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BYTE CBindingConnections::CheckLink(CNode1 afSrcNode,CNode1 afDestNode,CBindingLink *pLink)
{	
	ASSERT(afSrcNode);
	ASSERT(afDestNode);
	ASSERT(pLink);

	if (afSrcNode == afDestNode) return FALSE;
	if (!CheckLinkNLinks(afSrcNode,afDestNode,pLink)) return FALSE;
	if (!CheckLinkControlsType(pLink)) return FALSE;
	if (!CheckLinkinoutType(afSrcNode,afDestNode,pLink)) return FALSE;

	// Sedert versie 0x0C01.
	// Bij het connecteren van een link of er een input aanwezig is met referentie naar dezelfde unit...
	if (!CheckLinkInOutUnits(afSrcNode,afDestNode,pLink)) return FALSE;

	return TRUE;
}


// Sedert versie 0x0C01: 
// Controle of er geen input/output combinatie is van dezelfde unit.
BYTE CBindingConnections::CheckLinkInOutUnits(
	CNode1 afSrcNode,
	CNode1 afDestNode,
	CBindingLink *pNewLink)
{
	BYTE retVal=TRUE;

	CBindingUnits *const pBindingUnits = 
	  m_pParent->GetCurrBindingEntry()->GetColUnit();

	if (0!=pBindingUnits)
	{		
		if (0!=m_pParent)
		{
			const ControlType inType = pNewLink->GetInLinkType();
			const ControlType outType= pNewLink->GetOutLinkType();

			CBindingLinks* const pBindingLinks = 
				m_pParent->GetCurrBindingEntry()->GetColLink();

			if (0!=pBindingLinks)
			{
				const int count=pBindingLinks->Count();

				if ((PM_UNIT==inType)&&(PM_UNIT==outType))
				{
					// Eenvoudige controle:
					// Link tussen dezelfde unit.

					CBindingUnit* pUnitDest=m_pParent->GetControls()->GetUnit(afDestNode);
					CBindingUnit* pUnitSrc=m_pParent->GetControls()->GetUnit(afSrcNode);

					if (0!=pUnitDest && 0!=pUnitSrc)
					{
						if (((pUnitDest->GetNodeAddress()==pUnitSrc->GetNodeAddress())&&
							 (pUnitDest->GetUnitAddress()==pUnitSrc->GetUnitAddress())))
						{
							CString szError;
							szError.LoadString(IDS_ERROR_LNK_EQUALUNITS);
							AfxMessageBox(szError);
							return FALSE;
						}					
					}
				}					

				if (PM_UNIT==outType)
				{												
					CBindingUnit* pUnitDest=m_pParent->GetControls()->GetUnit(afDestNode);

					// Controleert bij het connecteren van een output unit of er 
					// reeds eenzelfde unit is waarvan een output link vertrekt.

					if (0!=pUnitDest)
					{										
						for (int i=0;i<count;++i)
						{
							CBindingLink* const p=pBindingLinks->Get(i);
							if ((0!=p)&&(p!=pNewLink))
							{							
								if (p->GetInLinkType()==PM_UNIT)
								{
									const unsigned short id=p->GetInLinkID();
									CBindingUnit* const pUnit=pBindingUnits->Find(id);
									if (pUnit!=0)
									{
										if ((pUnitDest != pUnit) &&
											((pUnitDest->GetNodeAddress()==pUnit->GetNodeAddress())&&
											 (pUnitDest->GetUnitAddress()==pUnit->GetUnitAddress())))
										{
											//if (p->GetOutLinkType()==PM_UNIT)
											//{

											CString szError;
											szError.Format("Error cannot make a link to this unit.\r\n"
												           "This unit already has an output link in this binding");

											AfxMessageBox(szError);

											return FALSE;
											//}
										}
									}
								}
							}
						}
					} /* if (0!=pUnitDest) */
				} /* if (PM_UNIT==outType) */

				if (PM_UNIT==inType)
				{
					CBindingUnit* pUnitSrc=m_pParent->GetControls()->GetUnit(afSrcNode);

					// Controleert bij het aanmaken van een link of de source unit in deze binding
					// reeds eenzelfde unit is die een input link heeft.

					if (0!=pUnitSrc)
					{										
						for (int i=0;i<count;++i)
						{
							CBindingLink* const p=pBindingLinks->Get(i);
							if ((0!=p)&&(p!=pNewLink))
							{							
								if (p->GetOutLinkType()==PM_UNIT)
								{
									const unsigned short id=p->GetOutLinkID();
									CBindingUnit* const pUnit=pBindingUnits->Find(id);
									if (pUnit!=0)
									{
										if ((pUnitSrc != pUnit) &&
											((pUnitSrc->GetNodeAddress()==pUnit->GetNodeAddress())&&
											 (pUnitSrc->GetUnitAddress()==pUnit->GetUnitAddress())))
										{
											//if (p->GetOutLinkType()==PM_UNIT)
											//{
											CString szError;
											szError.Format("Error cannot create a new link for this unit.\r\n"
												           "This unit is already used as an output in this binding.");

											AfxMessageBox(szError);
											return FALSE;
											//}
										}
									}
								}
							}
						}
					} /* if (0!=pUnitDest) */
				}
			}				
		}		
	}

	return retVal;
}