#pragma once

// CHANGES_ACTIVATION_LICENSE_CODE
// Deze lijst aanpassen wanneer er andere product id's bijkomen 

enum { NO_LICENSE_NEEDED = -1 };

// Maximum aantal producten die toegelaten is.
// dwz vanaf 3 HC-Boxen is er een licentie code nodig...

// Sedert V15.04: Er is geen licentie code meer nodig voor de smartbox'en.
enum 
{	
	PRODUCT_HC_BOX_ALLOWED_ITEMS		= NO_LICENSE_NEEDED,	// 2,	// OPPPASSEN: Om te debuggen de waarde aanpassen 
	PRODUCT_DMX_MASTER_ALLOWED_ITEMS	= NO_LICENSE_NEEDED,
	PRODUCT_DALI_MASTER_ALLOWED_ITEMS	= NO_LICENSE_NEEDED,
	PRODUCT_TFT_ALLOWED_ITEMS			= NO_LICENSE_NEEDED,	// OPPPASSEN: Om te debuggen de waarde aanpassen 
	PRODUCT_OLED_ALLOWED_ITEMS			= NO_LICENSE_NEEDED,	// OPPPASSEN: Om te debuggen de waarde aanpassen 
};