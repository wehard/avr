#define F_CPU 1000000 // 1Mhz

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "MrLCD.h"

uint8_t read_gray_code_from_encoder(void)
{
	uint8_t val = 0;
	if(!bit_is_clear(PIND, PD2))
	{
		val |= (1<<1);
	}
	if(!bit_is_clear(PIND, PD3))
	{
		val |= (1<<0);
	}
	return val;
}

int main(void)
{
	Initialize_LCD();
	Send_A_String("Hello Susani!");
	_delay_ms(2000);
	Goto_LCD_Location(1,1);
	Send_A_String("Tralallalalala!");
	
	uint8_t val = 0, val_tmp = 0;
	
	while(1)
	{
		Goto_LCD_Location(1,2);
		Send_A_String("I love you!");
	}
	
	void RunTimeLapseProgram()
	{
		//Calculate how many steps are needed to move at selected speed
		//Send required amount of pulses to motor controller
		//Calculate delay (the time the motor movement takes)
		//Delay for amount of time needed between shots so that the motor has stopped when picture is taken
		//Send pulse to camera
	}
	
	// Reset motor
	void InitializeMotor()
	{
		//Make motor go to left limit switch
		//While loop that moves motor until left limit is reached
		//Set flag for motor ready
	}
}