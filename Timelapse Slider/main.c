#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include "MrLCD.h"

int main(void)
{
	Initialize_LCD();

	Send_A_String("Hello World!");
	Goto_LCD_Location(5, 2);
	Send_A_String("Second row!");
	int num = 0;
	char positionString[3];
	while(1)
	{
		Goto_LCD_Locations(1,2);
		itoa(num, positionString, 10);
		Send_A_String(positionString);
		_delay_ms(100);
		num++;
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
		//Make it go to left limit switch
		//While loop that moves motor until left limit is reached
		//Set flag for motor ready
	}
}