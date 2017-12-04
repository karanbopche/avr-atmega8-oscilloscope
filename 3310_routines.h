#ifndef _3310_ROUTINES_H_
#define _3310_ROUTINES_H_

#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#define DISPLAY_WIDTH		84
#define DISPLAY_HEIGHT		47

#define CONTROL_PORT		PORTB
#define CONTROL_DDR			DDRB
#define DC					2
#define RST					0
#define SCE					1

#define DDR_SPI_PORT		DDRB
#define SPI_PORT			PORTB
#define SS					2
#define MISO				4
#define MOSI				3
#define SCK					5

void LcdInit ( void );
void LcdClear ( void );
void LcdGotoXY ( unsigned char x, unsigned char y );
void LcdWriteChar ( unsigned char character );
void LcdWriteCharInverted ( unsigned char character );
void LcdWriteData ( unsigned char data );
void LcdWriteCommand ( unsigned char command );
void LcdWriteString (char *string);
void LcdWriteStringInverted (char *string);
void LcdWriteStringln (char *string);
void LcdWriteStringInvertedln (char *string);
void LcdWriteString_P(PGM_P str);
void LcdWriteStringInverted_P(PGM_P str);
void LcdSetPixel(unsigned char x, unsigned char y);
void LcdClearPixel(unsigned char x, unsigned char y);
void LcdClearRow(unsigned char y);
void LcdClearColumn(unsigned char x);


#endif  //  _3310_ROUTINES_H_
