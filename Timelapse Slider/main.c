#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdlib.h>
#include "MrLCD.h"

#define ENC_PIN		PINA
#define ENC_PIN1 	PA0
#define ENC_PIN2 	PA1
#define ENC_BTN		PA2

#define INTERVAL_VAL_ADDR	46

uint8_t read_gray_code_from_encoder(void)
{
	uint8_t val = 0;
	if(!bit_is_clear(ENC_PIN, ENC_PIN1))
	{
		val |= (1<<1);
	}
	if(!bit_is_clear(ENC_PIN, ENC_PIN2))
	{
		val |= (1<<0);
	}
	return val;
}

void initialize_rotary_encoder(void)
{
	DDRA &=~ (1 << ENC_PIN1);				/* Set pins as input */
	DDRA &=~ (1 << ENC_PIN2);        
	PORTA |= (1 << ENC_PIN1)|(1 << ENC_PIN2); /* Set pins pullup enabled */
}

void LCD_Splash(void)
{
	
	Send_A_String("Hello!");
	_delay_ms(1000);
	Send_A_Command(0x01); // Clear screen
	
}

int main(void)
{
	Initialize_LCD();
	LCD_Splash();
	
	uint8_t val = 0, val_tmp = 0, enc_dir = 0;
	
	// Read saved value from eeprom
	uint8_t count;
	count = eeprom_read_byte((uint8_t*)INTERVAL_VAL_ADDR);
	
	initialize_rotary_encoder();
	
	char counterString[4];

	
	Send_A_Command(0x0C); // Hide cursor
	itoa(count, counterString, 10);
	Goto_LCD_Location(1,1);
	Send_A_String(counterString);
	
	while(1)
	{
		val_tmp = read_gray_code_from_encoder();
		
		if(val != val_tmp)
		{
			if((val==3 && val_tmp == 1)) // || (val==0 && val_tmp==2))
			{
				enc_dir = 1;
				if(count > 1) count--;
				Goto_LCD_Location(1,2);
				Send_A_String("L");
				
			}
			else if((val==2 && val_tmp==0)) // || (val==1 && val_tmp==3))
			{
				enc_dir = 2;
				if(count < 255) count++;
				Goto_LCD_Location(1,2);
				Send_A_String("R");
				
			}
			val = val_tmp;
			
			itoa(count, counterString, 10);
			Goto_LCD_Location(1,1);
			Send_A_String("           ");
			Goto_LCD_Location(1,1);
			Send_A_String(counterString);
			Send_A_String("s");
			
			// Save current value to eeprom
			eeprom_update_byte (( uint8_t *) INTERVAL_VAL_ADDR, count);
			
		}
		
		//_delay_ms(1);
	}
	return 0;
}

void RunTimeLapseProgram()
	{
		//Calculate how many steps are needed to move at selected speed
		//Send required amount of pulses to motor controller
		//Calculate delay (the time the motor movement takes)
		//Delay for amount of time needed between shots so that the motor has stopped when picture is taken
		//Send pulse to camera IR
	}
	
	// Reset motor
	void InitializeMotor()
	{
		//Make motor go to left limit switch
		//While loop that moves motor until left limit is reached
		//Set flag for motor ready
	}