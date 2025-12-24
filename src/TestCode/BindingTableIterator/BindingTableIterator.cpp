// BindingTableIterator.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "BindingTableIterator.h"
#include <conio.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// The one and only application object

CWinApp theApp;

using namespace std;


#define BOSEV35_SOURCE_FUNCTION_KEY1         0
#define BOSEV35_SOURCE_FUNCTION_KEY2         1
#define BOSEV35_SOURCE_FUNCTION_KEY3         2
#define BOSEV35_SOURCE_FUNCTION_KEY4         3
#define BOSEV35_SOURCE_FUNCTION_KEY5         4
#define BOSEV35_SOURCE_FUNCTION_KEY6         5
#define BOSEV35_SOURCE_FUNCTION_KEY7         6
#define BOSEV35_SOURCE_FUNCTION_KEY8         7
#define BOSEV35_SOURCE_FUNCTION_KEY9         8
#define BOSEV35_SOURCE_FUNCTION_KEY0         9	
#define BOSEV35_SOURCE_FUNCTION_KPUP     	10
#define BOSEV35_SOURCE_FUNCTION_KPDOWN   	11
#define BOSEV35_SOURCE_FUNCTION_KPLEFT   	12
#define BOSEV35_SOURCE_FUNCTION_KPRIGHT  	13
#define BOSEV35_SOURCE_FUNCTION_KPENTER  	14
#define BOSEV35_SOURCE_FUNCTION_TIENEEN  	15
#define BOSEV35_SOURCE_FUNCTION_FREE1_MENU 	16		/* MENU */
#define BOSEV35_SOURCE_FUNCTION_FREE2_GUIDE	17		/* GUIDE */
#define BOSEV35_SOURCE_FUNCTION_FREE3_INFO 	18		/* INFO */
#define BOSEV35_SOURCE_FUNCTION_FREE4_EXIT 	19		/* EXIT */
#define BOSEV35_SOURCE_FUNCTION_FREE5    	20
#define BOSEV35_SOURCE_FUNCTION_UP1      	21
#define BOSEV35_SOURCE_FUNCTION_DOWN1    	22 
#define BOSEV35_SOURCE_FUNCTION_UP2      	23
#define BOSEV35_SOURCE_FUNCTION_DOWN2    	24	
#define BOSEV35_SOURCE_FUNCTION_PLAY		25
#define BOSEV35_SOURCE_FUNCTION_PAUSE		26
#define BOSEV35_SOURCE_FUNCTION_DEC			27
#define BOSEV35_SOURCE_FUNCTION_INC			28
#define BOSEV35_SOURCE_FUNCTION_PREV		29
#define BOSEV35_SOURCE_FUNCTION_NEXT		30
#define BOSEV35_SOURCE_FUNCTION_STOP		31		
#define BOSEV35_SOURCE_FUNCTION_FREV		32
#define BOSEV35_SOURCE_FUNCTION_FFWD		33
#define BOSEV35_SOURCE_FUNCTION_PAGEUP		34
#define BOSEV35_SOURCE_FUNCTION_PAGEDOWN	35
#define BOSEV35_SOURCE_FUNCTION_RECORD		36
#define BOSEV35_SOURCE_FUNCTION_SHUFFLE		37
#define BOSEV35_SOURCE_FUNCTION_ON			38
#define BOSEV35_SOURCE_FUNCTION_OFF			39

//#define	BOSEV35_MAX_KEYPAD_FUNCTIONS		29			
#define BOSEV35_MAX_SOURCES						8
#define BOSEV35_MAX_BINDINGFLAG_FUNCTIONS		40		

#define BOSEV35_ADDITIONAL_DESTINATION_ON   	0x7E		/* Binding vlaggen */
#define BOSEV35_ADDITIONAL_DESTINATION_OFF  	0x7D		/* Binding vlaggen */

static const BYTE bindingFlagTabel[ BOSEV35_MAX_BINDINGFLAG_FUNCTIONS ][ BOSEV35_MAX_SOURCES ] = 
{
	/* 			  Tuner, HDMI1,	HDMI2,	HDMI3,	IN1,	IN2, 	IPOD,	Local */
	/* KP1 */  	{ 0xFF,	0x36,	0xFF,	0xFF,	0x4a,	0xFF,	0x2B,	0x10 	},	/*00=BOSEV35_SOURCE_FUNCTION_KEY1*/
	/* KP2 */	{ 0xFF,	0x37,	0xFF,	0xFF,	0x4b,	0xFF,	0x2C,	0x11	},	/*01=BOSEV35_SOURCE_FUNCTION_KEY2*/
	/* KP3 */	{ 0xFF,	0x38,	0xFF,	0xFF,	0x4c,	0xFF,	0x2D,	0x12	},	/*02=BOSEV35_SOURCE_FUNCTION_KEY3*/
	/* KP4 */	{ 0xFF,	0x39,	0xFF,	0xFF,	0x4d,	0xFF,	0x2E,	0x13  	},	/*03=BOSEV35_SOURCE_FUNCTION_KEY4*/
	/* KP5 */	{ 0xFF,	0x3a,	0xFF,	0xFF,	0x4e,	0xFF,	0x2F,	0x14	},	/*04=BOSEV35_SOURCE_FUNCTION_KEY5*/
	/* KP6 */	{ 0xFF,	0x3b,	0xFF,	0xFF,	0x4f,	0xFF,	0x30,	0x15	},	/*05=BOSEV35_SOURCE_FUNCTION_KEY6*/
	/* KP7 */	{ 0xFF,	0x3c,	0xFF,	0xFF,	0x50,	0xFF,	0x31,	0x16	},	/*06=BOSEV35_SOURCE_FUNCTION_KEY7*/
	/* KP8 */	{ 0xFF,	0x3d,	0xFF,	0xFF,	0x51,	0xFF,	0x32,	0x17	},	/*07=BOSEV35_SOURCE_FUNCTION_KEY8*/
	/* KP9 */	{ 0xFF,	0x3e,	0xFF,	0xFF,	0x52,	0xFF,	0x33,	0x18	},	/*08=BOSEV35_SOURCE_FUNCTION_KEY9*/
	/* KP0 */	{ 0xFF,	0x3f,	0xFF,	0xFF,	0x53,	0xFF,	0x34,	0x19	},	/*09=BOSEV35_SOURCE_FUNCTION_KEY0*/	
	/* OP  */	{ 0xFF,	0x55,	0xFF,	0xFF,	0x5c,	0xFF,	0xFF,	0x1B	},	/*10=BOSEV35_SOURCE_FUNCTION_KPUP*/
	/* Down*/	{ 0xFF,	0x56,	0xFF,	0xFF,	0x5d,	0xFF,	0xFF,	0x1C	},	/*11=BOSEV35_SOURCE_FUNCTION_KPDOWN*/
	/* Left*/	{ 0xFF,	0x57,	0xFF,	0xFF,	0x5e,	0xFF,	0xFF,	0x1D  	},	/*12=BOSEV35_SOURCE_FUNCTION_KPLEFT*/
	/* Right*/	{ 0xFF,	0x58,	0xFF,	0xFF,	0x5f,	0xFF,	0xFF,	0x1E	},	/*13=BOSEV35_SOURCE_FUNCTION_KPRIGHT/
	/* Enter*/	{ 0xFF,	0x59,	0xFF,	0xFF,	0x60,	0xFF,	0xFF,	0x1F	},	/*14=BOSEV35_SOURCE_FUNCTION_KPENTER*/
	/* 		*/ 	{ 0xFF,	0x40,	0xFF,	0xFF,	0x54,	0xFF,	0x35,	0x1A 	},	/*15=BOSEV35_SOURCE_FUNCTION_TIENEEN*/
	/* Menu */	{ 0xFF,	0x75,	0xFF,	0xFF,	0x6B,	0xFF,	0xFF,	0x71	},	/*38=BOSEV35_SOURCE_FUNCTION_FREE1_MENU*/
	/* Guide */	{ 0xFF,	0x6E,	0xFF,	0xFF,	0x6F,	0xFF,	0xFF,	0x74	},	/*39=BOSEV35_SOURCE_FUNCTION_FREE2_GUIDE*/
	/* Info */	{ 0xFF,	0x76,	0xFF,	0xFF,	0x6C,	0xFF,	0xFF,	0x72	},	/*40=BOSEV35_SOURCE_FUNCTION_FREE3_INFO*/
	/* Exit */	{ 0xFF,	0x77,	0xFF,	0xFF,	0x6D,	0xFF,	0xFF,	0x73	}, 	/*41=BOSEV35_SOURCE_FUNCTION_FREE4_EXIT*/
	/* 		*/	{ 0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF	},	/*20=BOSEV35_SOURCE_FUNCTION_FREE5*/
	/* 		*/	{ 0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF	},	/*21=BOSEV35_SOURCE_FUNCTION_UP1*/
	/* 		*/	{ 0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF	},	/*22=BOSEV35_SOURCE_FUNCTION_DOWN1*/
	/* 		*/	{ 0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF	},	/*23=BOSEV35_SOURCE_FUNCTION_UP2*/
	/* 		*/	{ 0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF	},	/*24=BOSEV35_SOURCE_FUNCTION_DOWN2*/
	/* Play */	{ 0xFF,	0x41,	0xFF,	0xFF,	0x62,	0xFF,	0xFF,	0x22 	},	/*25=BOSEV35_SOURCE_FUNCTION_PLAY*/
	/* Pause */ { 0xFF,	0x43,	0xFF,	0xFF,	0x64,	0xFF,	0xFF,	0x24 	},	/*26=BOSEV35_SOURCE_FUNCTION_PAUSE*/
	/* CH- */	{ 0xFF,	0x48,	0xFF,	0xFF,	0x69,	0xFF,	0xFF,	0x29 	},	/*27=BOSEV35_SOURCE_FUNCTION_DEC*/
	/* CH+ */	{ 0xFF,	0x49,	0xFF,	0xFF,	0x6A,	0xFF,	0xFF,	0x2A 	},	/*28=BOSEV35_SOURCE_FUNCTION_INC*/
	/* Prev */	{ 0xFF,	0x44,	0xFF,	0xFF,	0x65,	0xFF,	0xFF,	0x25 	},	/*29=BOSEV35_SOURCE_FUNCTION_PREV*/
	/* Next */	{ 0xFF,	0x45,	0xFF,	0xFF,	0x66,	0xFF,	0xFF,	0x26 	},	/*30=BOSEV35_SOURCE_FUNCTION_NEXT*/
	/* Stop */	{ 0xFF,	0x42,	0xFF,	0xFF,	0x63,	0xFF,	0xFF,	0x23 	},	/*31=BOSEV35_SOURCE_FUNCTION_STOP*/	
	/* FREV */  { 0xFF, 0x46,	0xFF,	0xFF,	0x67,	0xFF,	0xFF,	0x27	},	/*32=BOSEV35_SOURCE_FUNCTION_FREV*/
	/* FFWD */	{ 0xFF,	0x47,	0xFF,	0xFF,	0x68,	0xFF,	0xFF,	0x28	},	/*33=BOSEV35_SOURCE_FUNCTION_FFWD*/
	/* Page+ */	{ 0xFF,	0x5a,	0xFF,	0xFF,	0x61,	0xFF,	0xFF,	0x20	},	/*34=BOSEV35_SOURCE_FUNCTION_PAGEUP*/
	/* Page- */ { 0xFF,	0x5b,	0xFF,	0xFF,	0x62,	0xFF,	0xFF,	0x21	},	/*35=BOSEV35_SOURCE_FUNCTION_PAGEDOWN*/	
	/* Record*/ { 0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF	},	/*36=BOSEV35_SOURCE_FUNCTION_RECORD*/
	/* Shuffle*/{ 0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF	},	/*37=BOSEV35_SOURCE_FUNCTION_SHUFFLE*/				
	/* On */	{ 0x00,	0x02,	0x04,	0x06,	0x08,	0x0A,	0x0C,	0x0E	},	/*38=BOSEV35_SOURCE_FUNCTION_ON*/
	/* Off */	{ 0x01,	0x03,	0x05,	0x07,	0x09,	0x0B,	0x0D,	0x0F 	},	/*39=BOSEV35_SOURCE_FUNCTION_OFF*/
};

///////////////////////////////////////////////////////////////////////////////

class CBindingBoseV35Unit_Helper
{
public:
	class CIterator
	{		
	public:
		void incSource( );
		void decSource( );
		void incFunction( );
		void decFunction( );	
		void beginFunction( );

		CString getString( );

	protected:
		CIterator(int function, int source) :
			 m_function(function),
			 m_source(source) { }
		~CIterator( ) {	}
	
	private:
		int m_source;
		int m_function;

		friend class CBindingBoseV35Unit_Helper;
	};

public:

	static bool create_Iterator(int source,int function,CIterator** pIterator);
	static void delete_Iterator(CIterator* pIterator);
};

/////////////////////////////////////////////////////////////////////////////
void CBindingBoseV35Unit_Helper::CIterator::
incSource( )
{
	if (++m_source >= BOSEV35_MAX_SOURCES-1)
		m_source = BOSEV35_MAX_SOURCES-1;


	if (bindingFlagTabel[m_function][m_source] == 0xFF)
	{
		for (int i=0;i<BOSEV35_MAX_BINDINGFLAG_FUNCTIONS;i++)
		{
			if (bindingFlagTabel[i][m_source] != 0xFF)
			{
				m_function = i;
				break;
			}
		}
	}	
}

void CBindingBoseV35Unit_Helper::CIterator::
decSource( )
{
	if (--m_source <= 0)
		m_source = 0;


	if (bindingFlagTabel[m_function][m_source] == 0xFF)
	{
		for (int i=0;i<BOSEV35_MAX_BINDINGFLAG_FUNCTIONS;i++)
		{
			if (bindingFlagTabel[i][m_source] != 0xFF)
			{
				m_function = i;
				break;
			}
		}
	}	
}

void CBindingBoseV35Unit_Helper::CIterator::
beginFunction( )
{	
	for (m_function=0;m_function<BOSEV35_MAX_BINDINGFLAG_FUNCTIONS;m_function++)
	{
		if (bindingFlagTabel[m_function][m_source] != 0xFF)
		{		
			break;
		}
	}
}


void CBindingBoseV35Unit_Helper::CIterator::
incFunction( )
{
	if (++m_function >= BOSEV35_MAX_BINDINGFLAG_FUNCTIONS-1)
		m_function = BOSEV35_MAX_BINDINGFLAG_FUNCTIONS-1;
	
	if (bindingFlagTabel[m_function][m_source] == 0xFF)
	{		
		int temp;

		for (temp = m_function;temp<BOSEV35_MAX_BINDINGFLAG_FUNCTIONS;temp++)
		{
			if (bindingFlagTabel[temp][m_source] != 0xFF)
			{
				m_function = temp;
				break;
			}
		}
		
		for (temp = m_function;temp>=0;temp--)
		{
			if (bindingFlagTabel[temp][m_source] != 0xFF)
			{
				m_function = temp;
				break;
			}
		}
	}	
}

void CBindingBoseV35Unit_Helper::CIterator::
decFunction( )
{
	if (--m_function <= 0)
		m_function = 0;
	
	if (bindingFlagTabel[m_function][m_source] == 0xFF)
	{		
		int temp;

		for (temp = m_function;temp>=0;temp--)
		{
			if (bindingFlagTabel[temp][m_source] != 0xFF)
			{
				m_function = temp;
				break;
			}
		}

		for (temp = m_function;temp<BOSEV35_MAX_BINDINGFLAG_FUNCTIONS;temp++)
		{
			if (bindingFlagTabel[temp][m_source] != 0xFF)
			{
				m_function = temp;
				break;
			}
		}			
	}	
}

CString CBindingBoseV35Unit_Helper::CIterator::
getString( )
{
	CString szReturn;
	szReturn.Format("Source=%d, function=%d", m_source, m_function);
	return szReturn;
}

/////////////////////////////////////////////////////////////////////////////

bool CBindingBoseV35Unit_Helper::create_Iterator(
	int source,
	int function,
	CIterator** pIterator)
{
	CIterator* pReturn = 0;

	if (source < 0)
	{
		source = 0;
	}
	else if (source >= BOSEV35_MAX_SOURCES)
	{
		source = BOSEV35_MAX_SOURCES-1;
	}

	if (function < 0)
	{
		function = 0;
	}
	else if (function >= BOSEV35_MAX_BINDINGFLAG_FUNCTIONS)
	{
		function = BOSEV35_MAX_BINDINGFLAG_FUNCTIONS-1;
	}

	if (bindingFlagTabel[function][source] != 0xFF)
	{
		*pIterator = new CIterator(function, source);
		return true;
	}
	else
	{
		int i = 0;

		for (i=0;i<BOSEV35_MAX_BINDINGFLAG_FUNCTIONS;i++)
		{
			if (bindingFlagTabel[i][source] != 0xFF)
			{
				*pIterator = new CIterator(i, function);	
				return true;
			}
		}	

		for (i=0;i<BOSEV35_MAX_SOURCES;i++)
		{
			if (bindingFlagTabel[function][i] != 0xFF)
			{
				*pIterator = new CIterator(function, i);	
				return true;
			}
		}		

		for (function=0;function<BOSEV35_MAX_BINDINGFLAG_FUNCTIONS;function++)
		{
			for (source=0;source<BOSEV35_MAX_SOURCES;source++)
			{
				if (bindingFlagTabel[function][source] != 0xFF)
				{
					*pIterator = new CIterator(source, function);	
					return true;
				}	
			}
		}
	}

	return false;
}

void CBindingBoseV35Unit_Helper::
delete_Iterator(CIterator* pIterator)
{
	if (pIterator)
	{
		delete pIterator;
		pIterator = 0;
	}
}


/////////////////////////////////////////////////////////////////////////////




int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		cerr << _T("Fatal Error: MFC initialization failed") << endl;
		nRetCode = 1;
	}
	else
	{
		// TODO: code your application's behavior here.
		// CString strHello;
		// strHello.LoadString(IDS_HELLO);
		// cout << (LPCTSTR)strHello << endl;


		if (1)
		{		
			CBindingBoseV35Unit_Helper::CIterator* pIterator = 0;

			if (CBindingBoseV35Unit_Helper::create_Iterator(0,0,&pIterator))
			{
				ASSERT( pIterator );								
				
				for (int i=0;i<BOSEV35_MAX_SOURCES;i++,pIterator->incSource( ))
				{		
					int j;
					for (j=0,pIterator->beginFunction();j<BOSEV35_MAX_BINDINGFLAG_FUNCTIONS;j++,pIterator->incFunction( ))
					{						
						cout << "Iterator=" << (const char*) pIterator->getString() << "\n";
					}				
				

					getch( );
				}
			}
		
			CBindingBoseV35Unit_Helper::delete_Iterator( pIterator );
		}			

		getch( );

	}

	return nRetCode;
}


