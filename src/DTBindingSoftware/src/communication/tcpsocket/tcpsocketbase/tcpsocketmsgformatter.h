#ifndef INCLUDED_TCPSOCKETMSGFORMATTER_H
#define INCLUDED_TCPSOCKETMSGFORMATTER_H

// ---------------------------------------------------
#include <string>		// Stl Class ....
// ---------------------------------------------------

// ---------------------------------------------------
#define ARRAY_LENGTH						0
#define ARRAY_INDEX_MESSAGECODE				1
#define ARRAY_INDEX_METHOD					2
#define ARRAY_INDEX_DATA0					3
#define ARRAY_INDEX_DATA1					4
#define ARRAY_INDEX_DATA2					5
#define ARRAY_INDEX_DATA3					6
#define ARRAY_INDEX_DATA4					7
#define ARRAY_INDEX_DATA5					8
#define ARRAY_INDEX_DATA6					9
// ---------------------------------------------------
//
//
// ---------------------------------------------------
// htn : Host To Network order ...
// nth : Network to Host order ...
// ---------------------------------------------------
class CTCpSocketMsgFormatter  
{
public:
	CTCpSocketMsgFormatter();
	virtual ~CTCpSocketMsgFormatter();

	BOOL Build(const BYTE *pbData,char *out,int maxLen);
	BOOL Build(const char *in,BYTE *pbData,int maxLen);
	BOOL htn(const char *name,char *out,int maxLen);
	BOOL nth(const char *in,char *out,int maxLen);
	BOOL ParseString(const char* in,char *out,BOOL fBegin=FALSE);

	// int parseSubString(BYTE* pbIn,int length,int pos,char* out);

protected:
	BOOL htn(BYTE bVal,char *out,int maxlen);	

private:
	static const char header;
	static const char footer;
	static const char delimiter;
	static const char endofline;
	static const int radix;
	static const std::string substring;
};
/*****************************************************************/
#endif
/*****************************************************************/
