/***************************************************************************//**
 * @file 	APDU_def.h
 * @brief	header bestand met de APDU header definities.
 *
 * @author	Mitchell Tom
 * @date		20081201
 ******************************************************************************/
#ifndef INCLUDED_COMMUNICATION_APDU_DEF_H		// Niet veranderen
#define INCLUDED_COMMUNICATION_APDU_DEF_H		// Niet veranderen
/******************************************************************************/
namespace DUOTECNO {
namespace COM {
/***************************************************************************//**
 * @class 	APDUHeader
 * @brief   application protocol data unit header
 *				Communicatie header die beschrijft welk applicatie bericht er
 *				verstuurd wordt.
 *			   - Beschrijving van de opbouw van het bericht.
 *			   - Acties die de bestemming moet doen als het bericht ontvangt.
 ******************************************************************************/
struct APDUHeader
{
   /************************************************************************//**
    * Packet
    * @brief	application protocol data unit vlaggen
    *				Deze vlaggen bepalen de opbouw van het CAN-frame.\n
    *				- Basis of gesegmenteerd bericht.
    *				- Broadcast of adresgericht bericht.
    ***************************************************************************/
	enum Packet
	{
   	/**
       * APDU_BASIC_PACKET
       * @brief	  	Normaal/basis bericht.
       *             Indien deze vlag aktief is, is het een normaal/basis bericht.
       *					Indien deze vlag niet aktief is, is het een gesegmenteerd
       *				   bericht.
       */
 		APDU_BASIC_PACKET 				= 0x40,			// vlag

     	/**
       * APDU_BROADCAST_PACKET
       * @brief	  	Niet adresgericht bericht.
       *             Indien deze vlag aktief is, is het een niet adres
       *				   gericht bericht (BL,BA of BP).
       *					Indien deze vlag niet aktief is, is het een adres gericht
       *					bericht. (HE,ME,MACK of NOFREEMTC)
       */
		APDU_BROADCAST_PACKET        	= 0x02, 			// vlag
	};

   /************************************************************************//**
    * Services
    * @brief	application protocol data unit service
    ***************************************************************************/
	enum Services
	{
   	/**
       * APDU_IMPLICIT_INVOKE
       * @brief	application protocol data unit service
       *          Onvoorwaardelijke aanroep.
       */
		APDU_IMPLICIT_INVOKE  			= 0x20,
      /**
       * APDU_EXPLICIT_INVOKE
       * @brief	application protocol data unit service
       *          voorwaardelijke aanroep, moet een resultaat terugkrijgen.
       */
		APDU_EXPLICIT_INVOKE  			= 0x28,
      /**
       * APDU_EXPLICIT_RETRY
       * @brief	application protocol data unit service
       *          voorwaardelijke herkansing, moet een resultaat terugkrijgen
       */
		APDU_EXPLICIT_RETRY   			= 0x38,
      /**
       * APDU_RESULT
       * @brief	application protocol data unit service
       *          resultaat op een voorwaardelijke aanroep of herkansing
       */
		APDU_RESULT           			= 0x10,
  		/**
       * APDU_BINDING_RESULT
       * @brief	application protocol data unit service
       *          binding resultaat op een voorwaardelijke herkansing
       */
		APDU_BINDING_RESULT   			= 0x18,

      /**
       * APDU_SERVICE_MASK
       * @brief	application protocol data unit service Mask
       *          Mask om de APDU service uit de APDU header te halen
       */
		APDU_SERVICE_MASK     			= 0x38,			// Mask !!!!
	};
};
/******************************************************************************/
};
};
/******************************************************************************/
#endif
/******************************************************************************/

