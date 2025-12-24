#pragma once

enum
{
	APPFILEIOSER_ID_LICACTCODE = 100,
};


// Class CAppFileIoSer

class CAppFileIoSer
{
public:
	CAppFileIoSer( );
	virtual ~CAppFileIoSer( );

	virtual BOOL Save(CFile* fp) = 0;
	virtual BOOL Load(CFile* fp) = 0;
	virtual BOOL Peek(CFile* fp) = 0;

	virtual unsigned int getSerID( ) const = 0;
};
