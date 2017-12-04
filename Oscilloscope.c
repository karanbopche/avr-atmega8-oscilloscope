#include "Oscilloscope.h"

Button BtnEdge, BtnLevel;		// buffered button struct.......
char VoltageConvert = 20;		// multiplier to get mV value from sample....
unsigned char  ScaleY = 5;		// scale multiplier........ 5 = 5V
unsigned char StartPosition=0;	// start of DataBuffer to show on display 
char GroundLevel = DISPLAY_HEIGHT/2;	// position of zero level in display.......
char Buffer[8];		// temporary Buffer...........
unsigned char Trigger = 0;		// 0 = manual, 1 = auto trigger, 2 = external trigger
Prescalers Prescaler = ps_2;	// default prescaler......
unsigned char FftMode = 0;		// calculate fft if set to one.........
unsigned int MaxBufferSize = MAX_BUFFER_SIZE;
unsigned char WindowType = 0;		// default rectangle window........
int main(void)
{
	InitOscilloscope();
	wdt_enable(WDTO_1S);		// watch dog timer of 1 second........
	SampleButtons();
	IntroScreen();
	AdcStartBuffer(ch_0, DataBuffer, MaxBufferSize);
    while(1)
    {
		SampleButtons();
		
		if(Trigger == 1)	// auto trigger.......
		{
			// start with lower threshold 
			unsigned char Threshold = 20;
			MaxBufferSize = 100;
			//hold to align samples ........
			while (Threshold)	// avoid infinite loop to trigger.....
			{
				AdcStartBuffer(ch_0, Buffer, sizeof(Buffer));
				if((Buffer[4] < 0) && (Buffer[5] > 0))	// detect level of samples.	
						break;
				Threshold--;	// increase threshold range to detect high frequency signals.
				wdt_reset();
			}
			AdcStartBuffer(ch_0, DataBuffer, MaxBufferSize);
		}
		else if((Trigger == 2) && BtnEdge.ExtTrigger)		// external trigger.........
		{
			MaxBufferSize = MAX_BUFFER_SIZE;
			AdcStartBuffer(ch_0, DataBuffer, MaxBufferSize);
			
		}			
		else if(BtnEdge.Trigger)						/// manual trigger......
		{
			BUZZER_ON;
			MaxBufferSize = MAX_BUFFER_SIZE;
			AdcStartBuffer(ch_0, DataBuffer, MaxBufferSize);
		}
		
		if(BtnLevel.ShiftRight)
			if(StartPosition< (MaxBufferSize - DISPLAY_WIDTH))
				StartPosition += 5;
		if(BtnEdge.ShiftRight)
			BUZZER_ON;
		
		if(BtnLevel.ShiftLeft)
			if(StartPosition)
				StartPosition -= 5;
		if(BtnEdge.ShiftLeft)
			BUZZER_ON;
		
		if(BtnEdge.Select)
		{
			BUZZER_ON;
			OptionMenu();
		}
		
		if(FftMode)
		{
			AdcStartBuffer(ch_0, DataBuffer, MaxBufferSize);	// get samples.....
			fftWindow(DataBuffer, WindowType);	// apply window functions.....
			GroundLevel = DISPLAY_HEIGHT;		// zero line to bottom of display.....
			MaxBufferSize = MAX_BUFFER_SIZE/2;	// only up to Nyquist rate.....
			fft(DataBuffer, DataBuffer2);	// real and imaginary values.....
			Magnitude(DataBuffer, DataBuffer2);		// convert real imaginary into magnitude values..
		}
		else
		{
			GroundLevel = (DISPLAY_HEIGHT/2);
			MaxBufferSize  = MAX_BUFFER_SIZE;
		}			

		UpdateDisplay();
		_delay_ms(100);
		wdt_reset();
		LED_BLINK;
		BUZZER_OFF;
	}
}

void UpdateDisplay()		// display the BufferData on LCD Screen........
{
	unsigned char  PixelPositionX = 0;
	char PixelPositionY = 0, PixelPositionYOld;	
	unsigned char Sample= StartPosition;	// check if require int......
	unsigned char BankSelect=0, YPosition=0;
	PixelPositionYOld  = GroundLevel - (DataBuffer[Sample]/ScaleY);	// to avoid sharp line on display at start
	while((PixelPositionX < DISPLAY_WIDTH) && (Sample < MaxBufferSize))
	{
		unsigned char Column[] ={0, 0, 0, 0, 0, 0};
		Sample = PixelPositionX + StartPosition;
		PixelPositionY  = GroundLevel - (DataBuffer[Sample]/ScaleY);
		if(PixelPositionY < 0)		// for negative numbers avoid clipping above display......
			PixelPositionY = 0;
		else if(PixelPositionY > DISPLAY_HEIGHT)	// avoid clipping below display
			PixelPositionY = DISPLAY_HEIGHT;

		do{
			BankSelect = PixelPositionYOld/8;
			YPosition = PixelPositionYOld%8;
			
			if(PixelPositionY > PixelPositionYOld)
				PixelPositionYOld++;
			else if(PixelPositionY < PixelPositionYOld)
				PixelPositionYOld--;
			else
				PixelPositionYOld = PixelPositionY;
				
			Column[BankSelect] |= (1<<YPosition);
				
		}while(PixelPositionY != PixelPositionYOld);
		
		for(unsigned char i=0;i<6;i++)
		{
			LcdGotoXY(PixelPositionX, i);
			LcdWriteData(Column[i]);
		}
		PixelPositionY = PixelPositionYOld;
		PixelPositionX++;
	}
}

void OptionMenu()
{
	unsigned char CurrentOption =0, i, optionSelected =0;
	unsigned char Offset=0;
	char** OptionTemp;
	char* str;
	LcdClear();
	while(1)
	{
		SampleButtons();
		for(i=0;i<3;i++)
		{
			/// Display Menus.............
			LcdGotoXY(0, i*2);
			if((CurrentOption == (i+Offset)) && !optionSelected )
				LcdWriteStringInvertedln(MenuOptions[i+Offset]);
			else
				LcdWriteStringln(MenuOptions[i+Offset]);
			
			/// display subMenus..............
			LcdGotoXY(0, (i*2)+1);
			OptionTemp = SubOptions[i+Offset];
			str = OptionTemp[SelectedSubOption[i+Offset]];
			if((optionSelected) && (CurrentOption == (i+Offset)))
				LcdWriteStringInvertedln(str);
			else
				LcdWriteStringln(str);
		}
		
		if(BtnEdge.Select)
		{
			BUZZER_ON;
			if(optionSelected)
			{
				optionSelected = 0;
				if(CurrentOption == 0)		// Measurements........
					Measurements();
				else if(CurrentOption == 1)	// Sampling Frequency...........
				{
					Prescaler = SelectedSubOption[CurrentOption] + 1;
					AdcPrescaler(Prescaler);
				}				
				else if(CurrentOption == 2)	// Voltage Scale.........
				{
					if(SelectedSubOption[CurrentOption] == 0)
						ScaleY = 5;
					else if(SelectedSubOption[CurrentOption] == 1)
						ScaleY = 2;
					else
						ScaleY = 1;
				}
				else if(CurrentOption == 3)	// Trigger.......
					Trigger = SelectedSubOption[CurrentOption];
				else if(CurrentOption == 4)	// fft mode......
					FftMode = SelectedSubOption[CurrentOption];	// set FFT mode on and off.......
				else if(CurrentOption == 5) // 	fft Window type........
					WindowType = SelectedSubOption[CurrentOption];	// set WindowType....		
				else if(CurrentOption == 6)	// calibrate.....
					Calibrate();
			}
			else
			{
				optionSelected = 1;
			}
		}
		
		if(BtnEdge.ShiftLeft)			// up..........
		{
			BUZZER_ON;
			if(optionSelected)		// if any option is selected then shift sub options............ 
			{
				if(SelectedSubOption[CurrentOption])
				SelectedSubOption[CurrentOption]--;
				else
					SelectedSubOption[CurrentOption] = MaxMenuSubOptions[CurrentOption] - 1;
			}	
			else					// shift main menu options......
			{
				if(CurrentOption)
				{
					CurrentOption--;
					if(Offset > CurrentOption)
						Offset--;
				}
				else
				{
					CurrentOption = MaxMenuOptions - 1;
					Offset = MaxMenuOptions - 3;
				}
			}				
		}				
		
		if(BtnEdge.ShiftRight)			// down..........
		{
			BUZZER_ON;
			if(optionSelected)
			{
				SelectedSubOption[CurrentOption]++;
				if(SelectedSubOption[CurrentOption] >= MaxMenuSubOptions[CurrentOption])
					SelectedSubOption[CurrentOption] = 0;
			}
			else
			{
				CurrentOption++;			
				if((Offset+2) < CurrentOption)
					Offset++;
				if(CurrentOption >= MaxMenuOptions)
				{
					CurrentOption = 0;
					Offset = 0;
				}	
			}
		}
		
		if(BtnEdge.Trigger)
		{
			BUZZER_ON;
			break;
		}			
		_delay_ms(100);
		LED_BLINK;
		BUZZER_OFF;
		wdt_reset();
	}
	LcdClear();
	AdcStartBuffer(ch_0, DataBuffer , MaxBufferSize);
}

void Measurements()
{
	unsigned int Peak=0;	// signal peak and its frequency.......
	uint32_t Frequency=0;
	int Average=0;		// dc value of signal......
	unsigned char i=0, index=0;	// index to find fundamental frequency index in fft.....
	LcdClear();	
	SampleButtons();
	BtnEdge.Trigger = 1;	// first calculation.......
	while(1)
	{
		if(BtnEdge.Select)
		{
			BUZZER_ON;
			break;
		}		
			
		else if(BtnEdge.Trigger)
		{
			AdcStartBuffer(ch_0, DataBuffer, MaxBufferSize);
			fft(DataBuffer, DataBuffer2);
			Magnitude(DataBuffer, DataBuffer2);
			Peak = 0;
			Average = 0;
			Frequency = 0;
			index = 0;
					// find peak amplitude........
			for(i=0;i<128;i++)
			{
				if(Peak < DataBuffer[i])	// find peak magnitude of signal....
				{
					Peak = DataBuffer[i];
					index = i;		// current peak......
				}				
			}		
			Average = DataBuffer[0]*VoltageConvert;	// dc voltage value from fft.....
			Peak = Peak*VoltageConvert;	// peak voltage value from fft.......
			Frequency = (((unsigned)index*2383)>>Prescaler);	// calculate frequency from Fs, sample length,  Prescaler, Index.....
			BUZZER_ON;
		}	
		
		if(X2_PIN)
			VoltageConvert = 40;	// double voltage multiplier......
		else
			VoltageConvert = 20;			
		
		LcdGotoXY(0, 0);
		LcdWriteStringln("Peak Voltage");
		ToString(Peak, Buffer);
		LcdGotoXY(0, 1);
		LcdWriteString(Buffer);
		LcdWriteString("mV");
		
		LcdGotoXY(0, 2);
		LcdWriteStringln("Avg Voltage");
		LcdGotoXY(0, 3);	
		ToString(Average, Buffer);
		LcdWriteString(Buffer);
		LcdWriteString("mV");
		
		LcdGotoXY(0, 4);
		LcdWriteStringln("Frequency");
		ToString(Frequency, Buffer);
		LcdGotoXY(0, 5);
		LcdWriteString(Buffer);
		LcdWriteString("Hz");
		
		SampleButtons();
		_delay_ms(100);
		wdt_reset();
		BUZZER_OFF;
	}
	LcdClear();
}


void Calibrate()
{
	int Avg = 0;
	AdcStartBuffer(ch_0, Buffer, sizeof(Buffer));	// temporary samples for ground adjustment
	for(unsigned char i=0;i<sizeof(Buffer);i++)
		Avg = Buffer[i];
	// average value for zero level.........
	AdcSetZeroLevel(Avg/sizeof(Buffer));
}

void InitOscilloscope()
{
	SW_DDR &= ~((1<<SW_SELECT) | (1<<SW_TRIGGER) | (1<<SW_LEFT) | (1<<SW_RIGHT) | (1<<EXT_TRIGGER));
	SW_DDR |= (1<<LED_PIN) | (1<<BUZZER_PIN);
	SW_PORT |= (1<<SW_SELECT) | (1<<SW_TRIGGER) | (1<<SW_LEFT) | (1<<SW_RIGHT);
	SW_PORT &= ~((1<<LED_PIN) | (1<<BUZZER_PIN) | (1<<EXT_TRIGGER));
	
	DDRC &= ~(1<<X2_PIN);		// 2X pin............
	PORTC &= ~(1<<X2_PIN);
	
	AdcInit();
	AdcPrescaler(Prescaler);
	LcdInit();
}

void SampleButtons()
{
	unsigned char ShiftLeft = ((SW_PIN & (1<<SW_LEFT)) == 0);
	unsigned char ShiftRight = ((SW_PIN & (1<<SW_RIGHT)) == 0);
	unsigned char Trigger = ((SW_PIN & (1<<SW_TRIGGER)) == 0);
	unsigned char Select = ((SW_PIN & (1<<SW_SELECT)) == 0);
	unsigned char ExtTrigger = ((SW_PIN & (1<<EXT_TRIGGER)) != 0);	// active high Trigger input..........
	
	BtnEdge.ShiftLeft = (!BtnLevel.ShiftLeft) && (ShiftLeft);
	BtnEdge.ShiftRight = (!BtnLevel.ShiftRight) && (ShiftRight);
	BtnEdge.Trigger = (!BtnLevel.Trigger) && (Trigger);
	BtnEdge.Select = (!BtnLevel.Select) && (Select);
	BtnEdge.ExtTrigger = (!BtnLevel.ExtTrigger) && (ExtTrigger);
	
	BtnLevel.Select = Select;
	BtnLevel.ShiftLeft = ShiftLeft;
	BtnLevel.ShiftRight = ShiftRight;
	BtnLevel.Trigger = Trigger;
	BtnLevel.ExtTrigger = ExtTrigger;
}

void IntroScreen()
{
	LcdGotoXY(0, 2);
	LcdWriteStringInverted("Oscilloscope");
	while(1)
	{
		SampleButtons();
		
		if(BtnEdge.Select)
		{
			BUZZER_ON;
			break;
		}		
		_delay_ms(100);
		BUZZER_OFF;
		wdt_reset();
	}	
	LcdClear();
}


void ToString(int32_t num, char* data)
{
	unsigned char i,rem, len = 0;
	int32_t n;

	if(num < 0)
    {
        data[0] = '-';		// for negative sign..........
        num = -num;
    }
	else
		data[0] = ' ';
    n = num;
	while(n)	// length of number....
	{
		len++;
		n /= 10;
	}
	for(i=0;i<len;i++)	// convert each number into ASCII......
	{
		rem = num % 10;
		num = num / 10;
		data[len-i] = rem + '0';
	}
	len++;
	for(i=len;i<sizeof(Buffer);i++)
        data[i] = ' ';	// blank spaces ...........
    data[sizeof(Buffer)-1] = '\0';
}