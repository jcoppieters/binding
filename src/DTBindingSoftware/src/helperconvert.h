/**********************************************************************
 * @file	helperconvert.h
 * @brief	header bestand met helper convert functies.
 * @author	Mitchell Tom
 * @date	20120403
 *
 *********************************************************************/

#ifndef HELPERCONVERT_H
#define HELPERCONVERT_H

/**********************************************************************
 * signed value converteren van 2Complement.
 * Integers worden standaard als two's complement weergegeven.
 *
 * param sValue [IN]: signed waarde in 2Complement.
 *		 1=0x0001, 0=0x0000, -1=0xFFFF, -2=0xFFFE
 *
 * return unsigned waarde.
 *		  1= 0x0001, 0=0x000, -1=0x8001, -2=0x8002
 *
 * note Deze functie kan gebruikt worden om een short weer te geven 
 *		als 2 bytes.
 *********************************************************************/
extern unsigned short convertFrom2Complement(short sValue);


/**********************************************************************
 * unsigned value converteren naar 2Complement.
 * 
 * param usValue: unsigned waarde.
 *		  1= 0x0001, 0=0x000, -1=0x8001, -2=0x8002
 *		 
 * return signed waarde in 2Complement.
 *		  1=0x0001, 0=0x0000, -1=0xFFFF, -2=0xFFFE
 *
 *********************************************************************/
extern short convertTo2Complement(unsigned short usValue);


#endif