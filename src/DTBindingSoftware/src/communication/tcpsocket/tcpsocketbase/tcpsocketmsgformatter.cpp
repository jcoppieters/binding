#include "stdafx.h"
#include "tcpsocketmsgformatter.h"
#include "tcpsocketmsgtokenizer.h"
/*****************************************************************/
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
/*****************************************************************/
using namespace std;
/*****************************************************************/


/*****************************************************************/
const char CTCpSocketMsgFormatter::header = '[';
const char CTCpSocketMsgFormatter::footer = ']';
const char CTCpSocketMsgFormatter::delimiter = ',';
const char CTCpSocketMsgFormatter::endofline = '\n';
const int CTCpSocketMsgFormatter::radix = 10;
const string CTCpSocketMsgFormatter::substring = "]\n";
/*****************************************************************/

CTCpSocketMsgFormatter::CTCpSocketMsgFormatter() { }
CTCpSocketMsgFormatter::~CTCpSocketMsgFormatter() { }

BOOL CTCpSocketMsgFormatter::Build(const BYTE *pbData,char *out,int maxLen)
{		
	if (pbData == NULL || out == NULL) {
		ASSERT(pbData);	
		ASSERT(out);
		return FALSE;
	}

	std::string szStringBuffer;
	szStringBuffer += header;

	for (int i=1;i<=pbData[0];i++) {		
		if (i != 1) {
			szStringBuffer += delimiter;
		}

		char szTemp[8] = { 0,0,0,0,0,0,0,0 };		
		_itoa(pbData[i],&szTemp[0],radix);
		szStringBuffer += &szTemp[0];	
	}

	szStringBuffer += footer;
	szStringBuffer += endofline;

	const int stringBufferLength = szStringBuffer.length();
	if (stringBufferLength > maxLen) {
		return FALSE;
	}	
	strcpy(out,szStringBuffer.c_str());
	return TRUE;
}

/*****************************************************************
 *
 * BOOL CFormatter::Build(const char *in,BYTE *pbData,int maxLen)
 * Description: Parses data in and converts to bytes
 *
 * Arguments : * const char *in: [In]
 *			   * BYTE *pbData: [out]
 *			   * int maxLen: [in] max. outputbuffer length.
 *****************************************************************/
BOOL CTCpSocketMsgFormatter::Build(const char *in,BYTE *pbData,int maxLen)
{	
	if (in == NULL || pbData == NULL) {
		return FALSE;
	}

	std::string szStringBuffer;
	szStringBuffer.assign( in );
	if (szStringBuffer.at(0) != header) 
		return FALSE;

	const int nLast = szStringBuffer.find_last_of(substring);
	if (nLast < 0) 
		return FALSE;

	string sub(szStringBuffer, 1 , nLast-2);

	// Now we must use the String tokenizer to split the tokens ...
	int i;
	vector<string> oResult;
	vector<string>::iterator theIterator;

	CTokenizer<CIsFromString>::Tokenize(oResult, sub,CIsFromString(","));		
	for (i=1,theIterator = oResult.begin(); theIterator != oResult.end() && i<maxLen ; i++,theIterator++) 
	{					
		int temp = -1;
		if (sscanf(theIterator->c_str(),"%i",&temp)<1) {
			return FALSE;
		}
		ASSERT(temp >= 0 && temp <= 0xFF);
		pbData[i] = (BYTE) temp;
	}
	pbData[0] = i-1;

	if (i == maxLen) {
		return FALSE;
	}
	return TRUE;
}


BOOL CTCpSocketMsgFormatter::htn(BYTE bVal,char *out,int maxlen)
{
	if (out == NULL) 
		return FALSE;

	_itoa(bVal,out,radix);
	return TRUE;
}

BOOL CTCpSocketMsgFormatter::htn(const char *name,char *out,int maxLen)
{
	if (name == NULL || out == NULL) {
		return FALSE;
	}
	
	string s;
	s.erase();

	int i=0;
	for (i=0;i<maxLen && name[i] != NULL;i++) 
	{		
		char szTemp[8] = { 0,0,0,0,0,0,0,0 };	
		if (i != 0) {
			s += delimiter;
		}
		htn(name[i],&szTemp[0],3);	
		s += szTemp;		
	}

	if (i<maxLen) {
		strcpy(out,s.c_str());
		return TRUE;
	}

	*out = NULL;
	return FALSE;
}


BOOL CTCpSocketMsgFormatter::nth(const char *in,char *out,int maxLen)
{
	if (in == NULL || out == NULL) {
		return FALSE;
	}

	string s;
	vector<string> oResult;
	vector<string>::iterator theIterator;
	BYTE pbData[4];

	s.assign(in);

	CTokenizer<CIsFromString>::Tokenize(oResult, s,CIsFromString(","));
	
	int i;
	for (i=0,theIterator = oResult.begin();theIterator != oResult.end() && i<maxLen;i++,theIterator++) 
	{				
		int temp = -1;
		if (sscanf(theIterator->c_str(),"%i",&temp) < 1) {
			return FALSE;
		}	
		ASSERT(temp >= 0 && temp <= 0xFF);
		pbData[0] = (BYTE) temp;
		out[i] = pbData[0];	
	}

	if (i<maxLen) return TRUE;
	
	*out = NULL;
	return FALSE;
}


BOOL CTCpSocketMsgFormatter::ParseString(const char *in,char *out,BOOL fBegin)
{
	static int iPos = 0;

	if (fBegin == TRUE) {
		iPos = 0;
	}

	if (in != NULL) {
		string strIn;
		strIn.erase();
		strIn.assign(in);

		const int iLen = strIn.length();
		int iNewPos = strIn.find_first_of("\n",iPos);	
		if ((iNewPos != -1) && (iNewPos < iLen)) {			
			strncpy(out,&in[iPos],(iNewPos-iPos)+1);		
			iPos = iNewPos+1;
			return TRUE;					
		}		
	}
	*out = '\0';
	return FALSE;
}
/*
int CTCpSocketMsgFormatter::parseSubString(BYTE* pbIn,int length,int pos,char* out) {

	while (pos < length) 
	{
		if (pbIn[pos] == '\0') {
			pos++;			
		}
		else if (pbIn[i] == '\n') {
			out[j++] = pbIn;
			pos++;
			return pos;
		}		
	}
}
*/
