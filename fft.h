/*
 * fft.h
 *
 * Created: 24-Sep-17 10:19:13 AM
 *  Author: KARAN
 */ 


#ifndef FFT_H_
#define FFT_H_
#include <avr/pgmspace.h>
#include <math.h>

// window types define.........
#define Window_Rectangle	0
#define Window_Triangle		1
#define Window_Hamming		2
#define Window_Hann			3


// function prototypes..........
void fft(char* Real, char* Imag);
void Magnitude(char* real, char* img);
void GenerateSine(char* Real, char* Imag);
void fftWindow(char* Real, char Windowtype);
#endif /* FFT_H_ */