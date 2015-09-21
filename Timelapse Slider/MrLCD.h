#ifndef MrLCD
#define MrLCD

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

#define LCDDataLines 				PORTB
#define DataDir_LCDDataLines 		DDRB
#define LCDControl 					PORTD
#define DataDir_LCDControl 			DDRD
#define LCDEnable 					5
#define LCDReadWrite 				7
#define LCDRegisterSelect 			2

char firstColumnPositionsForMrLCD[4] = {0, 64, 20, 84};

void Check_If_LCD_isBusy(void);
void Peek_A_Boo(void);
void Send_A_Command(unsigned char command);
void Send_A_Character(unsigned char character);
void Send_A_String(char *StringOfCharacters);
void Goto_LCD_Location(uint8_t x, uint8_t y);
void Initialize_LCD(void);

void Check_If_LCD_isBusy()
{
	DataDir_LCDDataLines = 0;
	LCDControl |= 1<<LCDReadWrite;
	LCDControl &= ~1<<LCDRegisterSelect;

	while (LCDDataLines >= 0x80)
	{
		Peek_A_Boo();
	}
	DataDir_LCDDataLines = 0xFF; //0xFF means 0b11111111
}

void Peek_A_Boo()
{
	LCDControl |= 1<<LCDEnable;
	asm volatile ("nop");
	asm volatile ("nop");
	LCDControl &= ~1<<LCDEnable;
}

void Send_A_Command(unsigned char command)
{
	Check_If_LCD_isBusy();
	LCDDataLines = command;
	LCDControl &= ~ ((1<<LCDReadWrite)|(1<<LCDRegisterSelect));
	Peek_A_Boo();
	LCDDataLines = 0;
}

void Send_A_Character(unsigned char character)
{
	Check_If_LCD_isBusy();
	LCDDataLines = character;
	LCDControl &= ~ (1<<LCDReadWrite);
	LCDControl |= 1<<LCDRegisterSelect;
	Peek_A_Boo();
	LCDDataLines = 0;
}

void Send_A_String(char *StringOfCharacters)
{
	while(*StringOfCharacters > 0)
	{
		Send_A_Character(*StringOfCharacters++);
	}
}

void Goto_LCD_Location(uint8_t x, uint8_t y)
{
	Send_A_Command(0x80 + firstColumnPositionsForMrLCD[y-1] + (x-1));
}

void Initialize_LCD()
{
	DataDir_LCDControl |= 1<<LCDEnable | 1<<LCDReadWrite | 1<<LCDRegisterSelect;
	_delay_ms(15);

	Send_A_Command(0x01); //Clear screen 0x01 = 00000001
	_delay_ms(2);
	Send_A_Command(0x38); // Set 8 bit mode
	_delay_us(50);
	Send_A_Command(0b00001110);
	_delay_us(50);
}

#endif