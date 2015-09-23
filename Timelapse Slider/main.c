#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdlib.h>
#include "LCD.h"

#define ENC_PIN		PINA
#define ENC_PIN1 	PA0
#define ENC_PIN2 	PA1
#define ENC_BTN		PA2

#define TLAPSE_INT_ADDR		46
#define MTR_SPD_ADDR		54
#define TLAPSE_DUR_ADDR		62

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
	
	uint8_t val = 0, val_tmp = 0;
	
	// Read saved value from eeprom
	uint8_t tlapse_interval, motor_speed, tlapse_duration;
	tlapse_interval = eeprom_read_byte((uint8_t*)TLAPSE_INT_ADDR);
	motor_speed = eeprom_read_byte((uint8_t*)MTR_SPD_ADDR);
	tlapse_duration = eeprom_read_byte((uint8_t*)TLAPSE_DUR_ADDR);
	
	initialize_rotary_encoder();
	
	char tlapse_intervalerString[4];

	
	Send_A_Command(0x0C); // Hide cursor
	itoa(tlapse_interval, tlapse_intervalerString, 10);
	Goto_LCD_Location(1,1);
	Send_A_String(tlapse_intervalerString);
	
	while(1)
	{
		val_tmp = read_gray_code_from_encoder();
		
		if(val != val_tmp)
		{
			if((val==3 && val_tmp == 1)) // || (val==0 && val_tmp==2))
			{
				if(tlapse_interval > 1) tlapse_interval--;
				Send_A_String_XY(16,1, "L");
				
			}
			else if((val==2 && val_tmp==0)) // || (val==1 && val_tmp==3))
			{
				if(tlapse_interval < 255) tlapse_interval++;
				Send_A_String_XY(16,1, "R");
				
			}
			val = val_tmp;
			
			itoa(tlapse_interval, tlapse_intervalerString, 10);
			Goto_LCD_Location(1,2);
			Send_A_String("           ");
			//Goto_LCD_Location(1,1);
			//Send_A_String_XY(1,1, tlapse_intervalerString);
			Send_An_Integer_XY(1,2, tlapse_interval, 3);
			Send_A_String("s");
			Send_An_Integer_XY(7,2, tlapse_duration, 3);
			Send_An_Integer_XY(13,2, motor_speed, 3);
			
			
			// Save current value to eeprom
			eeprom_update_byte (( uint8_t *) TLAPSE_INT_ADDR, tlapse_interval);
			
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