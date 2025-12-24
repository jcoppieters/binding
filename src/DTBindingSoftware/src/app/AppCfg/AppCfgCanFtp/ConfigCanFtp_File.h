#ifndef DUOTECNO_CONFIG_CANFTP_FILE_H
#define	DUOTECNO_CONFIG_CANFTP_FILE_H


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

///////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
////////////////////////// CConfigCanFTP_File ////////////////////
//////////////////////////////////////////////////////////////////
class CConfigCanFTP_File
{	
public:
	CConfigCanFTP_File( );
	~CConfigCanFTP_File( );
	
	bool openFileWrite(
		const char* szFileName);
	bool openFileRead(
		const char* szFileName);
	bool getData(
		BYTE* pbData, 
		int length,
		int* numRead);
	bool addData(
		const BYTE* pbData,
		int length);
	bool isDone( ) const;
	bool isBusy( ) const;
	bool close( );
	int getLength( );
	int getPosition( );

private:
	FILE* m_fp;
	int m_fileLength;

};
///////////////////////////////////////////////////////////////////
#endif