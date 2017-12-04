#include "fft.h"

const unsigned char Length = 255;

static const char PROGMEM WindowHann[256] =
{
	0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 8, 8, 9, 10, 11, 11, 12, 13, 14, 15, 16, 17, 17, 18, 19, 20, 21, 22, 23, 25, 26, 27, 28, 29, 30, 31, 32, 33, 35, 36, 37, 38, 39, 41, 42, 43, 44, 46, 47, 48, 49, 50, 52, 53, 54, 55, 57, 58, 59, 60, 61, 63, 64, 65, 66, 67, 68, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 86, 87, 88, 89, 90, 90, 91, 92, 92, 93, 94, 94, 95, 95, 96, 96, 97, 97, 97, 98, 98, 98, 99, 99, 99, 99, 99, 99, 99, 99, 100, 99, 99, 99, 99, 99, 99, 99, 99, 98, 98, 98, 97, 97, 97, 96, 96, 95, 95, 94, 94, 93, 92, 92, 91, 90, 90, 89, 88, 87, 86, 86, 85, 84, 83, 82, 81, 80, 79, 78, 77, 76, 75, 74, 73, 72, 71, 70, 68, 67, 66, 65, 64, 63, 61, 60, 59, 58, 57, 55, 54, 53, 52, 50, 49, 48, 47, 46, 44, 43, 42, 41, 39, 38, 37, 36, 35, 33, 32, 31, 30, 29, 28, 27, 26, 25, 23, 22, 21, 20, 19, 18, 17, 17, 16, 15, 14, 13, 12, 11, 11, 10, 9, 8, 8, 7, 6, 6, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0
};
static const char PROGMEM WindowHamming[256] =
{
	8, 8, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 16, 16, 17, 18, 18, 19, 20, 21, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 31, 32, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 48, 49, 50, 51, 52, 53, 54, 55, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 83, 84, 85, 86, 87, 87, 88, 89, 90, 90, 91, 92, 92, 93, 93, 94, 95, 95, 96, 96, 96, 97, 97, 98, 98, 98, 98, 99, 99, 99, 99, 99, 99, 99, 99, 100, 99, 99, 99, 99, 99, 99, 99, 99, 98, 98, 98, 98, 97, 97, 96, 96, 96, 95, 95, 94, 93, 93, 92, 92, 91, 90, 90, 89, 88, 87, 87, 86, 85, 84, 83, 83, 82, 81, 80, 79, 78, 77, 76, 75, 74, 73, 72, 71, 70, 69, 68, 67, 66, 64, 63, 62, 61, 60, 59, 58, 57, 55, 54, 53, 52, 51, 50, 49, 48, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 32, 31, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 21, 20, 19, 18, 18, 17, 16, 16, 15, 14, 14, 13, 13, 12, 12, 11, 11, 10, 10, 10, 9, 9, 9, 9, 8, 8, 8, 8, 8, 8, 8, 8
};
static const char PROGMEM WindowTriangle[256] =
{
	0, 1, 1, 2, 3, 4, 5, 5, 6, 7, 8, 9, 9, 10, 11, 12, 12, 13, 14, 15, 16, 16, 17, 18, 19, 20, 20, 21, 22, 23, 23, 24, 25, 26, 27, 27, 28, 29, 30, 30, 31, 32, 33, 34, 34, 35, 36, 37, 38, 38, 39, 40, 41, 41, 42, 43, 44, 45, 45, 46, 47, 48, 49, 49, 50, 51, 52, 52, 53, 54, 55, 56, 56, 57, 58, 59, 60, 60, 61, 62, 63, 63, 64, 65, 66, 67, 67, 68, 69, 70, 70, 71, 72, 73, 74, 74, 75, 76, 77, 78, 78, 79, 80, 81, 81, 82, 83, 84, 85, 85, 86, 87, 88, 89, 89, 90, 91, 92, 92, 93, 94, 95, 96, 96, 97, 98, 99, 100, 99, 98, 97, 96, 96, 95, 94, 93, 92, 92, 91, 90, 89, 89, 88, 87, 86, 85, 85, 84, 83, 82, 81, 81, 80, 79, 78, 78, 77, 76, 75, 74, 74, 73, 72, 71, 70, 70, 69, 68, 67, 67, 66, 65, 64, 63, 63, 62, 61, 60, 60, 59, 58, 57, 56, 56, 55, 54, 53, 52, 52, 51, 50, 49, 49, 48, 47, 46, 45, 45, 44, 43, 42, 41, 41, 40, 39, 38, 38, 37, 36, 35, 34, 34, 33, 32, 31, 30, 30, 29, 28, 27, 27, 26, 25, 24, 23, 23, 22, 21, 20, 20, 19, 18, 17, 16, 16, 15, 14, 13, 12, 12, 11, 10, 9, 9, 8, 7, 6, 5, 5, 4, 3, 2, 1, 1, 0
};

PGM_P Window;	// pointer to windows.....

static const char PROGMEM twiddle[256] =
{
	120, 0, 119, -2, 119, -5, 119, -8, 119, -11, 119, -14, 118, -17, 118, -20, 117, -23, 117, -26, 116, -29, 115, -32, 114, -34, 113, -37, 112, -40, 111, -43, 110, -45, 109, -48, 108, -51, 107, -53, 105, -56, 104, -59, 102, -61, 101, -64, 99, -66, 98, -69, 96, -71, 94, -73, 92, -76, 90, -78, 88, -80, 86, -82, 84, -84, 82, -86, 80, -88, 78, -90, 76, -92, 73, -94, 71, -96, 69, -98, 66, -99, 64, -101, 61, -102, 59, -104, 56, -105, 53, -107, 51, -108, 48, -109, 45, -110, 43, -111, 40, -112, 37, -113, 34, -114, 32, -115, 29, -116, 26, -117, 23, -117, 20, -118, 17, -118, 14, -119, 11, -119, 8, -119, 5, -119, 2, -119, 0, -120, -2, -119, -5, -119, -8, -119, -11, -119, -14, -119, -17, -118, -20, -118, -23, -117, -26, -117, -29, -116, -32, -115, -34, -114, -37, -113, -40, -112, -43, -111, -45, -110, -48, -109, -51, -108, -53, -107, -56, -105, -59, -104, -61, -102, -64, -101, -66, -99, -69, -98, -71, -96, -73, -94, -76, -92, -78, -90, -80, -88, -82, -86, -84, -84, -86, -82, -88, -80, -90, -78, -92, -76, -94, -73, -96, -71, -98, -69, -99, -66, -101, -64, -102, -61, -104, -59, -105, -56, -107, -53, -108, -51, -109, -48, -110, -45, -111, -43, -112, -40, -113, -37, -114, -34, -115, -32, -116, -29, -117, -26, -117, -23, -118, -20, -118, -17, -119, -14, -119, -11, -119, -8, -119, -5, -119, -2 
};

void fft(char* Real,char* Imag)
{

	unsigned char j=0, i; // variables for bit reversing.......
	unsigned char HalfLength;	// half length of sequence......
	char temp;		// for swapping variables
	unsigned int n1, n2, n3;	// index for FFT calculation
	int16_t tempImg, tempReal;
	char TwiddleReal, TwiddleImg; 
	unsigned char TwiddleIndex;			// for complex FFT calculations....
	
	// bit reversal part..........
	for(i=1;i<Length;i++)
	{
		HalfLength = 128;
		while( j >= HalfLength)
		{
			j -= HalfLength;
			HalfLength = HalfLength/2;
		}
		j += HalfLength;

		if(i<j)
		{
			temp = Real[i];
			Real[i] = Real[j];
			Real[j] = temp;
		}
	}
	// FFT calculations..........
	n1 = 0;		// index of buffer values for
	n2 = 1;		// first stage........
	n3 = 256;	// for twiddle index calculation..........
	for(i=0;i<8;i++)	// loop for  8 stages..........
	{
		n1 = n2;
		n2 += n2;
		n3 = (n3>>1);

		for(j=0;j<n1;j++)		// loop for number of twiddles.......
		{
			TwiddleIndex = 2*j*n3;
			// retrieve twiddle values from program memory.........

			TwiddleReal = pgm_read_byte(&twiddle[TwiddleIndex]);
			TwiddleImg =  pgm_read_byte(&twiddle[TwiddleIndex+1]);
			for(unsigned int k=j;k<=Length;k+=n2)			// loop for number of butterfly multiplication..........
			{
				// twiddle multiplication of data......
				tempReal = (Real[k+n1]*TwiddleReal - Imag[k+n1]*TwiddleImg);
				tempImg = (Real[k+n1]*TwiddleImg + Imag[k+n1]*TwiddleReal);
				tempReal = tempReal/120;	// unnormalizes  twiddle values.......
				tempImg = tempImg/120;
				// butterfly subtraction......
				Real[k+n1] = (Real[k] - tempReal)/2;    // scale values to avoid overflow....
				Imag[k+n1] = (Imag[k] - tempImg)/2;
				//butterfly addition.......
				Real[k] = (Real[k] + tempReal)/2;
				Imag[k] = (Imag[k] + tempImg)/2;
			}
		}
	}
}

void Magnitude(char* real, char* img)
{
	unsigned char i=0, j=255;
	unsigned int temp;
	do 
	{	// calculate magnitude from real and imaginary value
		temp = real[i]*real[i] + img[i]*img[i];
		real[i] = sqrt(temp);
	} while (--i);
	
	for(i=1;i<128;i++)  // fold frequency ..........
    {
        real[i] += real[j];
        j--;
    }
}

void fftWindow(char* Real, char Windowtype)
{
	int Temp;
	unsigned char i=0;
	if(Windowtype == Window_Rectangle)
		return;							// multiply by 1;
	else if(Windowtype == Window_Triangle)
		Window = WindowTriangle;
	else if(Windowtype == Window_Hann)
		Window = WindowHann;
	else if(Windowtype == Window_Hamming)
		Window = WindowHamming;
	do 
	{
		Temp = Real[i] * pgm_read_byte(&Window[i]);
		Real[i] = Temp/100;	// unnormalized window effect......
	} while (--i);
		
}