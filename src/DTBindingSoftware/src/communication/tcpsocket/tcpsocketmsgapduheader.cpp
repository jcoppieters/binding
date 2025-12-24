#include "stdafx.h"
#include "tcpsocketmsgapduheader.h"
#include "apdu_def.h"						// Nodig voor het bepalen van de header...
#include "nodemess.h"
/*****************************************************************/
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
/*****************************************************************/
using namespace DUOTECNO::COM;
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
int getADPUHeader(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bMsgCode,
			BYTE bLength,const BYTE* pbData)
{	
	int header = 0;

	switch(bMsgCode)
	{
		case EV_UNITCONTROLTOGGLE:
	 	case EV_UNITCONTROLPULSTOGGLE:
	 	case EV_UNITCONTROLPULS:
	 	case EV_UNITSELECTOR:
		case EV_BINDINGRESPONSE:
		{
			ASSERT(0);		// TODO !!!
			return -1;
		} break;

		 case FC_NODEMANAGEAVIRDEVICES:		// Explicit invoke
	 	 case FC_NODEGETTEXTSTRING:			// Explicit invoke
	 	 case FC_UNITREQUESTDIMSTATUS:		// Explicit invoke
	 	 case FC_UNITREQUESTCONFIG:			// Explicit invoke
	 	 case FC_UNITREQUESTSWITCHSTATUS:	// Explicit invoke
	 	 case FC_UNITREQUESTCONTROLSTATUS: 	// Explicit invoke
	 	 case FC_UNITREQUESTSENSSTATUS:		// Explicit invoke
	 	 case FC_NODEREQUESTCONFIG:			// Explicit invoke
	 	 case FC_UNITREQUESTNAME:			// Explicit invoke
	 	 case FC_NODEREQUESTNAME:			// Explicit invoke
	 	 case FC_NODEREQUESTSTATUS:			// Explicit invoke
	 	 // case FC_UNITEMPCNTSET:				// Explicit invoke
	 	 case FC_NODELCDMASTERSET:			// Explicit invoke
	 	 case FC_NODELCDMASTERTIMEDATE:		// Explicit invoke
	 	 case FC_NODELCDMASTERREQUESTSTATUS: // Explicit invoke
	 	 // case FC_UNITREQUESTEMPCNTSTATUS:	 // Explicit invoke
	 	 case FC_EXTERNALCONFIG:	// Explicit invoke
	 	 case FC_LCDBUILDSCREENMAP: // Explicit invoke
	 	 case FC_UNITVIDEOMUXSET: // Explicit invoke
	 	 case FC_UNITREQUESTVIDEOMUXSTATUS: // Explicit invoke
	 	 case FC_UNITSPEECHCOMMSET: // Explicit invoke
	 	 case FC_UNITREQUESTSPEECHCOMMSTATUS: // Explicit invoke
	 	 case FC_NODEMASTERLCDTAPISET:		// Explicit invoke
	 	 case FC_UNITIRTXSET:	 // Explicit invoke
	 	 case FC_FTPFILECONTROL:	 // Explicit invoke
	 	 case FC_REQUESTFTPFILECONTROL:	 // Explicit invoke
	 	 case FC_NODEAVIRCONNECTIONMATRIXSET:	 // Explicit invoke
	 	 case FC_NODEREQUESTAVIRMATRIXSTATUS:	 // Explicit invoke
	 	 case FC_NODEAVIRDEVICESENDIRCODE:	 // Explicit invoke
	 	 case FC_UNITRC5IRRECEIVERSET:	 // Explicit invoke
	 	 case FC_UNITREQUESTRC5IRRECEIVERSTATUS:	 // Explicit invoke
	 	 case FC_UNITDIMSET:					// Explicit invoke
	 	 case FC_UNITDUOSWITCHSET: // Explicit invoke
	 	 case FC_UNITREQUESTDUOSWITCHSTATUS: // Explicit invoke
	 	 case FC_DATABASEMANAGEMENT:	// Explicit invoke
	 	 case FC_NODEBINDINGSFILECONTROL:	// Explicit invoke
	 	 case FC_NODEBINDINGENTRY:			// Explicit invoke
	 	 case FC_NODEGETRESOURCES:			// Explicit invoke
	 	 case FC_UNITAUDIOSET:				// Explicit invoke
	 	 case FC_LCDINFODIALOG:				// Explicit invoke
	 	 case FC_UNITREQUESTAUDIOSTATUS:	// Explicit invoke
	 	 case FC_UNITREQUESTIRTXSTATUS:		// Explicit invoke
	 	 case FC_UNITSWITCHSET:			// Explicit invoke
	 	 case FC_UNITCONTROLSET:		// Explicit invoke
	 	 case FC_NODELCDSET:			// Explicit invoke
	 	 case FC_UNITREQUESTLCDSTATUS:	// Explicit invoke
	 	 case FC_UNITLCDVIRTUALSET:		// Explicit invoke
	 	 case FC_UNITREQUESTLCDVIRTUALSTATUS: // Explicit invoke
	 	 case FC_NODEERASEEEPROM:		// Explicit invoke
	 	 case FC_NODEEEPROMREAD:		// Explicit invoke
	 	 case FC_NODEEEPROMWRITE:		// Explicit invoke
	 	 case FC_UNITALARMSET:			// Explicit invoke
	 	 case FC_UNITREQUESTALARMSTATUS:		// Explicit invoke
		 case FC_UNITAVMATRIXSET:		// Explicit invoke
		 case FC_UNITREQUESTAVMATRIXSTATUS: // Explicit invoke
	 	 // case FC_UNITEMSET:				// Explicit invoke
	 	 // case FC_UNITREQUESTEMSTATUS:	// Explicit invoke
	 	 case FC_NODEGATEWAYSET:		// Explicit invoke
	 	 case FC_NODEGATEWAYREQUESTSTATUS: // Explicit invoke
	 	 case FC_UNITPS2INTELLIMOUSESET: // Explicit invoke
	 	 case FC_UNITREQUESTPS2INTELLIMOUSESTATUS: // Explicit invoke
	 	 //case FC_RESERVED_TEMP1:		// Explicit invoke
	 	 //case FC_RESERVED_TEMP2:		// Explicit invoke
	 	 //case FC_RESERVED_TEMP3_V6553:	// Explicit invoke
	 	 //case FC_RESERVED_TEMP4_V6553:	// Explicit invoke
	 	 //case FC_RESERVED_TEMP5_V6553:	// Explicit invoke
	 	 //case FC_RESERVED_TEMP6_V6553:	// Explicit invoke
	 	 //case FC_RESERVED_TEMP7_V6553:	// Explicit invoke
	 	 //case FC_RESERVED_TEMP8_V6553:	// Explicit invoke
	 	 //case FC_RESERVED_TEMP9_V6553:	// Explicit invoke
	 	 //case FC_RESERVED_TEMP10_V6553:	// Explicit invoke
		 {
			if (bLength<=6) {
				header += APDUHeader::APDU_BASIC_PACKET;		
			}
			if (bNodeAddress==0xFF || bNodeAddress==0xFF) {
				header += APDUHeader::APDU_IMPLICIT_INVOKE;
			}
			else {
				header += APDUHeader::APDU_EXPLICIT_INVOKE;
			}			
		 } break;

	     case EV_FTPFILECONTROLDATA:		// implicit invoke
	 	 case EV_FTPFILECONTROLSTATUS:		// implicit invoke
	 	 case EV_PNPLOGICALADDRESSASSIGNED: // implicit invoke
	 	 case EV_MEMORYDATADUMP: 			// implicit invoke
	 	 case EV_WRITEMEMORYDATADONE: 		// implicit invoke
	 	 case EV_LCDINFODIALOG: 			// implicit invoke
	 	 case FC_PNPPHYSADRACK:				// implicit invoke
	 	 case FC_PNPASSIGNLOGICALADDRESS:	// implicit invoke
	 	 case FC_BROWSEFORPHYSICALNODES:	// implicit invoke
	 	 case FC_SLAVELCDINITIATEBROWSING:	// implicit invoke
	 	 case FC_NODEMASTERLCDSHUTDOWN:		// implicit invoke
	 	 case FC_NODELCDSHUTDOWN:			// implicit invoke
	 	 case FC_NODESETMASTERLCDNODELA:	// implicit invoke
	 	 case FC_BROWSEFORNODES:			// implicit invoke
	 	 case FC_NODERESET:					// implicit invoke
	 	 case FC_UNITRESET:					// implicit invoke
	 	 case FC_GETMEMORYDATA:				// implicit invoke
	 	 case FC_WRITEMEMORYDATA:			// implicit invoke
	 	 case FC_LOCKLCD:					// implicit invoke
	 	 case FC_UNLOCKLCD:					// implicit invoke
		 {
			if (bLength<=6) {
				header += APDUHeader::APDU_BASIC_PACKET;		
			}			
			header += APDUHeader::APDU_IMPLICIT_INVOKE;
		 } break;

		 case FC_UNITSENSSET:		// Explicit invoke
	 	 case FC_NODETEXTSTRING:	// Explicit invoke
	 	 case FC_UNITSETNAME:		// Explicit invoke - no broadcast
	 	 case FC_NODESETNAME:		// Explicit invoke - no broadcast
	 	 case FC_UNITCONFIG:		// Explicit invoke - no broadcast
	 	 case FC_NODECONFIG:		// Explicit invoke - no broadcast
	 	 case FC_NODEREQUESTDLLSTATUS:	// Explicit invoke - no broadcast
		 {
			if (bLength<=6) {
				header += APDUHeader::APDU_BASIC_PACKET;		
			}			
			header += APDUHeader::APDU_EXPLICIT_INVOKE;
		 } break;

		 default: {
			return -1; 
	     } break;
	}
	return header;
}
/*****************************************************************/