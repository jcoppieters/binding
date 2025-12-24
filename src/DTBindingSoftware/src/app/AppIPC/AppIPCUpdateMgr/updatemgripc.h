#pragma once

/*****************************************************************************
 * Bij de start van de applicatie moet een "Update" string verstuurd worden
 * naar de update manager via een UDP socket. 
 * IP-Address = 127.0.0.1
 * poort = 45301
 * 
 * @since 0x0639
 *****************************************************************************/
extern int updatemgripc_SendUpdateString(void);