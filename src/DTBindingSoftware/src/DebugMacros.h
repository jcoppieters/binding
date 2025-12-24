#ifndef DEBUGMACROS
#define DEBUGMACROS

#ifdef _DEBUG
	#define _DEBUG_BINDINGFILE_CONSTRUCT_PARSER		1
	//#define _DEBUG_BINDINGUNIT_IS_EQUAL				1
#endif

// --------------------------------------------------------
//#define PNP_DEBUG
//---------------------------------------------------------
#ifdef PNP_DEBUG
	#define PNP_DEBUG_TRACE(x)				TRACE(x)
	#define PNP_DEBUG_TRACE1(x,y)			TRACE1(x,y)
	#define PNP_DEBUG_TRACE2(x,y,z)			TRACE2(y,z,z)
	#define PNP_DEBUG_TRACE3(x,y,z,w)		TRACE3(x,y,z,w)
#else
	#define PNP_DEBUG_TRACE(x)
	#define PNP_DEBUG_TRACE1(x,y)
	#define PNP_DEBUG_TRACE2(x,y,z)
	#define PNP_DEBUG_TRACE3(x,y,z,w)
#endif
//----------------------------------------------------------
#endif
