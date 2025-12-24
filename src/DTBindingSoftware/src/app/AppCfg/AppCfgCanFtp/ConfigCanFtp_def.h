#ifndef DUOTECNO_CONFIG_CANFTP_DEF_H
#define	DUOTECNO_CONFIG_CANFTP_DEF_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

///////////////////////////////////////////////////////////////////
/////////////////////////// FtpErrorCode  /////////////////////////
///////////////////////////////////////////////////////////////////
enum FtpErrorCode
{
	FTP_SUCCESS					= 0,   	/** Ok - geen error */
	FTP_ERROR_WRONG_STATE 		= -1,	/** Fsm in de verkeerde toestand */
	FTP_ERROR_WRITE_DATA  		= -2,	/** Fout bij het schrijven van data */
	FTP_ERROR_READ_DATA   		= -3,	/** Fout bij het lezen van data */
	FTP_ERROR_FILE_OPEN			= -4,	/** Fout bij het openen van een bestand */
	FTP_ERROR_FILE_CLOSE		= -5,
	FTP_ERROR_NOT_IMPLEMENTED	= -6,	/** Nog niet geimplementeerd */
	FTP_ERROR_GENERAL			= -7,
	FTP_ERROR_GET_INFO			= -8,
};
///////////////////////////////////////////////////////////////////

#endif