#ifndef _ADC_H_
#define _ADC_H_

#include <avr/io.h>
#include <avr/interrupt.h>


#define REF_INTERNAL	1
#define REF_EXTERNAL	0

typedef enum { ps_2=1, ps_4, ps_8, ps_16, ps_32, ps_64, ps_128} Prescalers;
typedef enum {ch_0,ch_1,ch_2,ch_3,ch_4,ch_5,ch_6,ch_7} Channels;

// Function prototypes..........
void AdcInit();
int AdcStart(unsigned char Channel);
void AdcStartBuffer(Channels Channel, char* Data, unsigned int DataLenth);
void AdcSetRef(unsigned char InternalRef);
void AdcPrescaler(Prescalers Prescaler);
void AdcSetZeroLevel(char Offset);
void AdcStop();
#endif