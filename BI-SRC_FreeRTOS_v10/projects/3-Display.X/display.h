#ifndef DISPLAY_H
#define	DISPLAY_H

#define displayQueueSize    5

void vDisplayInit ( void );
void vDisplayPrintTask ( void * );
void vDisplayGatekeeperTask ( void * );
void vDisplayPutString ( const char * );

#endif	/* DISPLAY_H */

