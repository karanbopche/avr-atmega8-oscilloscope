#include "adc.h"

unsigned char ZeroLevel = 95;		// calibrated for zero line on display...

void AdcInit()
{
	ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1)| (1<<ADPS0);	// prescaler 128;
}

void AdcSetZeroLevel(char Offset)
{
	ZeroLevel = ZeroLevel - Offset;	// calibrate for offset........
}

int AdcStart(Channels Channel)
{
	int Value;
	ADMUX &= 0xF0;	// Clear first four bits Multiplexer.
	ADMUX += Channel;
	ADCSRA |= (1<<ADSC);
	while(ADCSRA & (1<<ADSC));
	Value = ADC;
	return Value;
}

void AdcStartBuffer(Channels Channel, char* Data, unsigned int DataLenth)
{
	unsigned int DataSize = 0;
	ADMUX &= 0xF0;	// Clear first four bits Multiplexer.
	ADMUX |= Channel | (1<<ADLAR);
	ADCSRA |= (1<<ADFR) | (1<<ADSC);
	while(DataSize < DataLenth)
	{
		while(!(ADCSRA & (1<<ADIF)));
		ADCSRA |= (1<<ADIF);
		Data[DataSize] = ADCH - ZeroLevel;
		DataSize++;
	}
	ADCSRA &= ~(1<<ADFR);
}

void AdcPrescaler(Prescalers Prescaler)
{
	ADCSRA &= 0xF8;		// Clear Prescaler.....
	ADCSRA |= Prescaler;	//Load new value......
}

void AdcSetRef(unsigned char InternalRef)
{
	ADCSRA &= ~((1<<REFS1) | (1<<REFS0));
	if(InternalRef)
	ADCSRA |= (1<<REFS1) | (1<<REFS0);
}

void AdcStop()
{
	ADCSRA &= ~((1<<ADEN) | (1<<ADIE) | (1<<ADSC));
}