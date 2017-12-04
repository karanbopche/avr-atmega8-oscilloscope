#ifndef OSCILLOSCOPE_H_
#define OSCILLOSCOPE_H_

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <avr/wdt.h>
#include "3310_routines.h"
#include "adc.h"
#include "fft.h"

#define MAX_BUFFER_SIZE		256

#define SW_DDR				DDRD
#define SW_PORT				PORTD
#define SW_PIN				PIND
#define SW_TRIGGER			7
#define SW_LEFT				4
#define SW_RIGHT			6
#define SW_SELECT			2
#define EXT_TRIGGER			0

#define LED_PIN				5
#define BUZZER_PIN			3
#define LED_BLINK			PORTD^=(1<<PD5)
#define BUZZER_ON			PORTD|=(1<<BUZZER_PIN)
#define BUZZER_OFF			PORTD&=~(1<<BUZZER_PIN)

#define SW_X2				2
#define X2_PIN				(PINC&(1<<PC2))

typedef struct {  unsigned char Select, ShiftLeft, ShiftRight,  Trigger, ExtTrigger;  }Button;
// data Buffer used for storing ADC sample values...
char DataBuffer[MAX_BUFFER_SIZE];
// data buffer2 used for storing imaginary values
char DataBuffer2[MAX_BUFFER_SIZE];
//// Menu Options......

char* MenuOptions[] = {"Measurement", "Sample Freq", "Volt-Scale", "Trigger", "FFT", "Window", "Calibrate" };
// sub options of each menu options........
char* MeasurementOptions[]	=	   {"Start"};
char* TimeSubOptions[] = {"305Khz", "152Khz", "76Khz", "38Khz", "19Khz", "9Khz", "5Khz"};
char* VoltSubOptions[] = {"5 Volt", "2 Volt", "1 Volt"};
char* TriggerOptions[] = {"Manual", "Auto", "External"};
char* FftOption[] = {"off", "On"};
char* WindowOption[] = {"Rectangle", "Triangle", "Hamming", "Hann"};
// pointer to each sub options.......
char** SubOptions[] = {MeasurementOptions, TimeSubOptions, VoltSubOptions, TriggerOptions, FftOption, WindowOption, MeasurementOptions};

const unsigned char MaxMenuOptions = 7;
const unsigned char MaxMenuSubOptions[] =	{0, 7, 3, 3, 2, 4, 0};
// default selected sub options.......
unsigned char SelectedSubOption[] =			{0, 0, 0, 0, 0, 0, 0};

void SampleButtons();
void InitOscilloscope();
void UpdateDisplay();
void IntroScreen();
void OptionMenu();
void Measurements();
void Calibrate();
void ToString(int32_t Number, char* data);

#endif /* OSCILLOSCOPE_H_ */