#ifndef INCLUDED_SCREENMAPTYPES_H
#define INCLUDED_SCREENMAPTYPES_H
/********************************************************************************/

/********************************************************************************/
/********************************************************************************/
/********************************************************************************/
enum
{
	SCREENMAP_UNITTYPE_DIM			= 0x01,		// (1 << 0)
	SCREENMAP_UNITTYPE_SWITCH		= 0x02,		// (1 << 1)
	SCREENMAP_UNITTYPE_SENS			= 0x04,		// (1 << 2)
	SCREENMAP_UNITTYPE_AUDIO		= 0x08,		// (1 << 3)
	SCREENMAP_UNITTYPE_BOSE			= 0x10,		// (1 << 4)
	SCREENMAP_UNITTYPE_DUOSWITCH	= 0x20,		// (1 << 5)
	SCREENMAP_UNITTYPE_CONTROL		= 0x40,		// (1 << 6)
	SCREENMAP_UNITTYPE_VIRTUAL		= 0x80,		// (1 << 7)

#if (USE_SMAP_ENERGYMEASURE == 1)
	SCREENMAP_UNITTYPE_ENERGYMEASURE = (1 << 8)		// Sedert V16.21 (firmware V66.11.00)
#endif 
};
/********************************************************************************/


/********************************************************************************/
/********************************************************************************/
/********************************************************************************/
enum screenmaptype 
{
	SCREENMAP_DIMMER	= 0,		// oppassen : NIET MEER AANPASSEN
	SCREENMAP_SWITCH	= 1,		// oppassen : NIET MEER AANPASSEN
	SCREENMAP_CONTROLS	= 2,		// oppassen : NIET MEER AANPASSEN
	SCREENMAP_SENS		= 3,		// oppassen : NIET MEER AANPASSEN
	SCREENMAP_DUOSWITCH = 4,		// oppassen : NIET MEER AANPASSEN
	SCREENMAP_AUDIO		= 5,		// oppassen : NIET MEER AANPASSEN			
	SCREENMAP_V65_MAIN_MOODS	= 6,	// nieuw v0571
	SCREENMAP_V65_MAIN_SENSOR	= 7,	// nieuw v0571
	SCREENMAP_V65_MAIN_AUDIO	= 8,	// nieuw v0571
	SCREENMAP_V65_MYHOME		= 9,	// nieuw v0571
	SCREENMAP_V65_MYHOME_SENS	= 10,	// nieuw v0571

#if (USE_SMAP_ENERGYMEASURE == 1)
	SCREENMAP_ENERGYMEASURE		= 11	// Sedert V16.21 (firmware V66.11.00)
#endif
};
/********************************************************************************/


/********************************************************************************/
/********************************************************************************/
/********************************************************************************/
enum 
{ 
	SCREENMAP_MAX_V64 = (SCREENMAP_AUDIO + 1),			// 6

#if (USE_SMAP_ENERGYMEASURE == 1) 
	SCREENMAP_MAX_V661100 = (SCREENMAP_MAX_V64 + 1)
#endif
};				

/********************************************************************************/
#endif
/********************************************************************************/