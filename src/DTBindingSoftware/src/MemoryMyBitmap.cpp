// MemoryMyBitmap.cpp: implementation of the CMemoryMyBitmap class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "MemoryMyBitmap.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMemoryMyBitmap::CMemoryMyBitmap()
{
	pbBitmapFile = NULL;
	pbPixelsLine = NULL;
	pbDummyBytes = NULL;

	Clear();
}

CMemoryMyBitmap::~CMemoryMyBitmap()
{

	Clear();
}


//////////////////////////////////////////////////////////////////////
BOOL CMemoryMyBitmap::Clear(void)
{
	try {
	
		if (pbBitmapFile != NULL) {	
			delete [] pbBitmapFile;
		}
		if (pbPixelsLine != NULL) {
			delete [] pbPixelsLine;
		}
		if (pbDummyBytes != NULL) {
			delete [] pbDummyBytes;
		}

		pbBitmapFile = NULL;
		pbPixelsLine = NULL;
		pbDummyBytes = NULL;

		return TRUE;
	}
	catch (CMemoryException *e) {
	
		e->ReportError();
		e->Delete();
		return FALSE;
	}
}

BOOL CMemoryMyBitmap::Load(CString szFileName)
{
	FILE *fp;
	int numRead;
	CString szError;
	int iBitmapMemOffset;
	int t;

	Clear();

	if ((fp = fopen(szFileName,"rb")) != NULL)
	{
  	   // Convert bitmap
		numRead = fread((void*)&BitmapFileHeader,sizeof(_BITMAPFILEHEADER),1,fp );
		if (numRead == 0) {
			Clear();
			return FALSE;
		}
	  
		numRead = fread((void*)&BitmapInfo,sizeof(_BITMAPINFO),1,fp );
		if (numRead == 0) {
			Clear();
			return FALSE;
		}

		/********************************* MONOCHROME *****************************/
		if (BitmapInfo.bmiHeader.biBitCount == 1) // MONOCHROME
		{
			fseek(fp,BitmapFileHeader.bfOffBits,SEEK_SET);

			if (BitmapInfo.bmiHeader.biWidth == 0) {
				AfxMessageBox("Bitmap file is possible corrupt\nCannot be loaded");
				fclose(fp);
				return FALSE;
			}
     
			iAlign = (BitmapInfo.bmiHeader.biWidth/8);

			if ((BitmapInfo.bmiHeader.biWidth%8)!=0) iAlign++;

			if (iAlign < 4) iDummyBytes = 4 - iAlign;
			else  {
				if ((iAlign%4) != 0)  iDummyBytes = 4-(iAlign%4);
				else iDummyBytes = 0;
			}
     
			iBitmapSize = BitmapInfo.bmiHeader.biSizeImage-iDummyBytes*BitmapInfo.bmiHeader.biHeight;

			try {

				pbBitmapFile = new BYTE[iBitmapSize];
				pbPixelsLine = new BYTE[iAlign];
				pbDummyBytes = new BYTE[iDummyBytes];
     
				iBitmapMemOffset=(iBitmapSize-1); 

				for(DWORD i=0;i<BitmapInfo.bmiHeader.biHeight;i++) {  
					fread((void*)pbPixelsLine,sizeof(BYTE),iAlign,fp);
					fread((void*)pbDummyBytes,sizeof(BYTE),iDummyBytes,fp);

					for (int t=iAlign-1;t>=0;t--){ 
						if (iBitmapMemOffset >= 0) {
							pbBitmapFile[iBitmapMemOffset] = (pbPixelsLine[t]^0xff);
							iBitmapMemOffset--;
						}
					}
				}
				fclose(fp);	  
				return TRUE;
			}
			catch (CMemoryException *e) {
				e->ReportError();
				fclose(fp);
				return FALSE;
			}	
		}
		 /********************************* 24-BIT *****************************/
		  else if (BitmapInfo.bmiHeader.biBitCount == 24)
		  {
			 fseek(fp,BitmapFileHeader.bfOffBits,SEEK_SET);
         
			 if (BitmapInfo.bmiHeader.biSizeImage != 0) {
				 			
				if (BitmapInfo.bmiHeader.biHeight == 0) {
					AfxMessageBox("Bitmap file is possible corrupt\nCannot be loaded");
					fclose(fp);
					return FALSE;
				}
					
				iDummyBytes = (BitmapInfo.bmiHeader.biSizeImage/BitmapInfo.bmiHeader.biHeight)-
						      (BitmapInfo.bmiHeader.biWidth * 3);

				iBitmapSize = BitmapInfo.bmiHeader.biSizeImage-iDummyBytes*BitmapInfo.bmiHeader.biHeight+
							   sizeof(STRUCT_PMBITMAP_HEADER);

				try {

					/* 18122000 */
					pbBitmapFile = new BYTE[((iBitmapSize-sizeof(STRUCT_PMBITMAP_HEADER))/3)+sizeof(STRUCT_PMBITMAP_HEADER)];
					pbPixelsLine = new BYTE[BitmapInfo.bmiHeader.biWidth*3];
					pbDummyBytes = new BYTE[iDummyBytes];
         
					iBitmapMemOffset=sizeof(STRUCT_PMBITMAP_HEADER); 

					/* 18122000 */
					pbBitmapFile[0] = (BYTE)(BitmapInfo.bmiHeader.biWidth/256);
					pbBitmapFile[1] = (BYTE)(BitmapInfo.bmiHeader.biWidth%256);
					pbBitmapFile[2] = (BYTE)(BitmapInfo.bmiHeader.biHeight/256);
					pbBitmapFile[3] = (BYTE)(BitmapInfo.bmiHeader.biHeight%256);
					pbBitmapFile[4] = COLOR8COLOR_BITMAP;

					for(int i=(BitmapInfo.bmiHeader.biHeight-1);i>=0;i--) {  
						fread((void*)pbPixelsLine,sizeof(BYTE),BitmapInfo.bmiHeader.biWidth*3,fp);
						fread((void*)pbDummyBytes,sizeof(BYTE),iDummyBytes,fp);

						iBitmapMemOffset=sizeof(STRUCT_PMBITMAP_HEADER)+i*BitmapInfo.bmiHeader.biWidth; 
						for (t=0;t<(int)(BitmapInfo.bmiHeader.biWidth*3);t+=3) { 
							BYTE bVal=0;
							if (pbPixelsLine[t] != 0) bVal = 0x01;
							if (pbPixelsLine[t+1] != 0) bVal |= 0x02;
							if (pbPixelsLine[t+2] != 0) bVal |= 0x04;
							pbBitmapFile[iBitmapMemOffset] = bVal;
							iBitmapMemOffset++;
						}
					}

					// ??????????????????????????????
					//iBitmapSize= (iBitmapSize-sizeof(STRUCT_PMBITMAP_HEADER))/3;
					iBitmapSize= (iBitmapSize-sizeof(STRUCT_PMBITMAP_HEADER))/3 + sizeof(STRUCT_PMBITMAP_HEADER);			
					fclose(fp);
					return TRUE;
				}
				catch (CMemoryException *e)
				{
					e->ReportError();
					fclose(fp);
					return FALSE;
				}
			 }
			 else 
			 {
				AfxMessageBox("Wrong Format of bitmap file.\n"
							  "Possible problem with resolution: dots per inch ...");
				fclose(fp);
				return FALSE;			 
			 }
		  }
		  else
		  {
			 szError.Format(_T("Bitmap format is not supported and cannot be loaded in memory"));
			 AfxMessageBox(szError);		
			 fclose(fp);
			 return FALSE;
		  }
	}
	else {
		szError.Format(_T("Error opening file :\n%s"),szFileName);
		AfxMessageBox(szError);
		return FALSE;
	}
	return FALSE;
}
