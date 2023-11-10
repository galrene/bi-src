#ifndef DISPLAY_H
#define	DISPLAY_H

void vDisplayInit ( void );
void vDisplayGatekeeperTask ( void * pvParameters );
void vDisplayPutString ( const char *pcStringToSend, size_t xStringLength );

#endif	/* DISPLAY_H */

