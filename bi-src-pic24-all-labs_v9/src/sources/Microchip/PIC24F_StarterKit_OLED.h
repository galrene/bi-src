// DRN adaptation to drive PIC24F Starter Kit's SH1101A OLED display
// Dave.Nadler@Nadler.com 15-August-2012

/*
 * Warning !  This code uses some Microchip-distributed graphics
 * support. Microchip's code works but beware of horrendous global
 * names like "DeviceRead" - there's no intelligent name prefixing to
 * separate code for numerous different kind of devices.
 *
 * The code below provides a few primitives to support FreeRTOS
 * PIC24F demonstration code access to the OLED display on
 * Microchip's PIC24F Starter Kit board.
 *
*/

#ifndef DEFINED_PIC24F_StarterKit_OLED_h
#define DEFINED_PIC24F_StarterKit_OLED_h


#include "GenericTypeDefs.h" // Microchip generic type definitions

void OLED_Init(void);
void OLED_Clear(void);

void OLED_PutStringInvertOption(const char* ptr, BYTE row, BYTE column, BOOL invert);
#if 0
  #define ROM_STRING const /*far rom*/ char * // better ROM attribute handling available for PIC24F ?
  void OLED_PutROMStringInvertOption(ROM_STRING ptr, BYTE page, BYTE column, BOOL invert);
#endif
// void OLED_Scroll_Down(void);
void OLED_Scroll_Up(void);   // make room for new bottom row

// Draw operations seem to be cut-and-pasted from earlier Microchip code...
void OLED_Set_Color(BYTE color); // kludgy global color setting for Draw operations
WORD OLED_Draw_Box(SHORT left, SHORT top, SHORT right, SHORT bottom);
WORD OLED_Draw_Line(SHORT x1, SHORT y1, SHORT x2, SHORT y2);

#endif // #ifndef DEFINED_PIC24F_StarterKit_OLED_h
