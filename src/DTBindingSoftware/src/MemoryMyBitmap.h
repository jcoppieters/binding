// MemoryMyBitmap.h: interface for the CMemoryMyBitmap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MEMORYMYBITMAP_H__72EA7772_6739_4BF5_B718_B5D46C80C113__INCLUDED_)
#define AFX_MEMORYMYBITMAP_H__72EA7772_6739_4BF5_B718_B5D46C80C113__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// -------------------------------------------------------------------

#define MONOCHROME_BITMAP      0
#define COLOR1BIT_BITMAP       1
#define COLOR6BIT_BITMAP       6
#define COLOR8BIT_BITMAP       8
#define COLOR8COLOR_BITMAP   254
#define COLORPALETTE_BITMAP  255


#pragma pack(1)		/* Oppassen: #pragma pack() toevoegen aan einde */

typedef struct
{
	unsigned short wXSize;
    unsigned short wYSize;
	BYTE bPixelDepth;

} STRUCT_PMBITMAP_HEADER;

// -------------------------------------------------------------------
typedef struct
{
   WORD bfType;
   DWORD bfSize;
   WORD bfReserved1;
   WORD bfReserved2;
   DWORD bfOffBits;

} _BITMAPFILEHEADER;

typedef struct
{
   DWORD biSize;
   DWORD biWidth;
   DWORD biHeight;
   WORD piPlanes;
   WORD biBitCount;
   DWORD biCompression;
   DWORD biSizeImage;
   DWORD biXPelsPerMeter;
   DWORD biYPelsPerMeter;
   DWORD biClrUsed;
   DWORD biClrImportant;

} _BITMAPINFOHEADER;

typedef struct
{
   BYTE rgbBlue;
   BYTE rgbGreen;
   BYTE rgbRed;
   BYTE rgbReserved;

} _RGBQUAD;

typedef struct
{
   _BITMAPINFOHEADER bmiHeader;
   _RGBQUAD bmiColors[1];

} _BITMAPINFO;

typedef struct
{
   DWORD bcSize;
   WORD bcWidth;
   WORD bcHeight;
   WORD bcPlanes;
   WORD bcBitCount;

} _BITMAPCOREHEADER;


typedef struct
{
   BYTE rgbtBlue;
   BYTE rgbtGreen;
   BYTE rgbtRed;

} _RGBTRIPLE;

typedef struct
{
   _BITMAPCOREHEADER bmciHeader;
   _RGBTRIPLE *bmciColors; // was [];

} _BITMAPCOREINFO;

#pragma pack()

// -------------------------------------------------------------------

class CMemoryMyBitmap  
{
protected:	
	int iBitmapSize;
	int iAlign;
	int iDummyBytes;
	_BITMAPFILEHEADER BitmapFileHeader;
	_BITMAPINFO BitmapInfo;
	BYTE *pbBitmapFile;
	BYTE *pbPixelsLine;
	BYTE *pbDummyBytes;

protected:
	BOOL Clear(void);

public:
	CMemoryMyBitmap();
	virtual ~CMemoryMyBitmap();

	BOOL Load(CString szFileName);
	int GetWidth(void)			{ return BitmapInfo.bmiHeader.biWidth;	};
	int GetHeight(void)			{ return BitmapInfo.bmiHeader.biHeight;	};
	int GetBitmapSize(void)		{ return iBitmapSize; };
	BYTE* GetBuffer(void)		{ return pbBitmapFile; };

};

#endif // !defined(AFX_MEMORYMYBITMAP_H__72EA7772_6739_4BF5_B718_B5D46C80C113__INCLUDED_)
