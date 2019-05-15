#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdlib.h>
#include <stdbool.h>
#include "LCD.h"

//			 A  		B
// NEMA 17 (röd-grön | gul-blå)
//			röd-svart| brun-blå

#define ENC_PIN		PINA
#define ENC_PIN1 	PA0
#define ENC_PIN2 	PA1
#define ENC_BTN		PA2

#define TLAPSE_INT_ADDR		32
#define MTR_SPD_ADDR		48
#define TLAPSE_DUR_ADDR		63

#define MTR_STEP_PIN	PA3
#define MTR_DIR_PIN		PA4

#define setbit(port, bit) (port) |= (1 << (bit))
#define clearbit(port, bit) (port) &= ~(1 << (bit))
#define readbit(port, bit) ~(port) & (1<<(bit));

#define CHECK_MSEC	5	// Read hardware every 5 msec
#define PRESS_MSEC	10	// Stable time before registering pressed
#define RELEASE_MSEC	100	// Stable time before registering released


typedef enum {FALSE, TRUE} bool_t;

uint16_t tlapse_interval, motor_speed, tlapse_duration;

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

void initialize_encoder_button(void) 
{
	DDRA &= ~(1 << ENC_BTN);	// Set pin as input
	PORTA |= (1 << ENC_BTN);	// Set pin pullup enabled
}

void initialize_motor_control(void) {
	DDRA |= (1<< MTR_STEP_PIN);			// Set pin as output ???
	DDRA |= (1<< MTR_DIR_PIN);
	clearbit(PORTA, MTR_DIR_PIN);
	clearbit(PORTA, MTR_STEP_PIN);

}

void LCD_Splash(void)
{
	
	Send_A_String("Laszlo Space");
	_delay_ms(2000);
	Send_A_Command(0x01); // Clear screen
	
}


// This holds the debounced state of the key.
bool_t DebouncedKeyPress = false;

bool_t RawKeyPressed() {
	//return ~ENC_PIN & (1<<ENC_BTN);
	return readbit(ENC_PIN, ENC_BTN);
}

// Service routine called by a timer interrupt
bool_t DebounceSwitch()
{
    static uint16_t State = 0; // Current debounce status
    State=(State<<1) | !RawKeyPressed() | 0xe000;
    if(State==0xf000)return TRUE;
    return FALSE; 
}

void Reset_Device(void)
{
    WDTCR=0x18; 
    WDTCR=0x08; 
    //#asm("wdr") 
    while(1); 
}

void RunTimeLapseProgram()
{
	uint16_t mtr_steps = motor_speed;
	Goto_LCD_Location(1,1);
	Send_A_String("Running program");
	uint8_t mtr_dir = 0;
	unsigned char ElapsedSeconds = 0;

	TCCR1B |= ((1 << CS10) | (1 << CS11)); // Set up timer at Fcpu/64



	
	//Calculate how many steps are needed to move at selected speed
	while(1)
	{
		DebouncedKeyPress=DebounceSwitch();

		if(DebouncedKeyPress) //If switch is pressed switch direction
    	{
			mtr_dir = !mtr_dir;
		}

		if(mtr_dir != 0) setbit(PORTA, MTR_DIR_PIN);
		if(mtr_dir == 0) clearbit(PORTA, MTR_DIR_PIN); 

  		// If timer has reached one second
		if(TCNT1 >= 15625) {
			TCNT1 = 0; // Set timer back to zero
			ElapsedSeconds++; // Increment ElapsedSeconds
			
		}
		if(ElapsedSeconds >= tlapse_interval) { // If elapsed seconds matches the previously set interval
			// Pulse motor controller for as many steps as are set
			uint16_t i;
			for (i = 0; i < mtr_steps ; ++i)
			{
				setbit(PORTA, MTR_STEP_PIN);
				_delay_us(1000);
				clearbit(PORTA, MTR_STEP_PIN);
				_delay_us(1000);
			}
			Send_An_Integer_XY(1,2, ElapsedSeconds, 3);
			ElapsedSeconds = 0; // Set elapsed seconds back to zero
		}
		if(TCNT1 >= 10000) {
			Goto_LCD_Location(1,2);
			Send_A_String(" ");
			//Goto_LCD_Location(8, 2);
			Send_An_Integer_XY(8, 2, mtr_dir, 10);
		}


	}
	//Send required amount of pulses to motor controller
	//Calculate delay (the time the motor movement takes)
	//Delay for amount of time needed between shots so that the motor has stopped when picture is taken
	//Send pulse to camera IR
}



int main(void)
{
	Initialize_LCD();
	LCD_Splash();
	
	uint8_t enc_val = 0, enc_val_tmp = 0;
	
	// Read saved value from eeprom
	

	
	tlapse_interval = eeprom_read_word((uint16_t*)TLAPSE_INT_ADDR);
	motor_speed = eeprom_read_word((uint16_t*)MTR_SPD_ADDR);
	tlapse_duration = eeprom_read_word((uint16_t*)TLAPSE_DUR_ADDR);

		


	initialize_rotary_encoder();
	initialize_encoder_button();
	initialize_motor_control();
	sei(); // Enable global interrupts
	
	int enc_direction = 0;

	
	Send_A_Command(0x0C); // Hide cursor
	//itoa(tlapse_interval, tlapse_intervalerString, 10);
	//Goto_LCD_Location(1,1);
	//Send_A_String(tlapse_intervalerString);

	int lcd_draw_counter_ms = 0;
	int lcd_draw_delay_ms = 100;

	int selected_value = 0;

	if(tlapse_interval == -1) {
		tlapse_interval = 1;
	}
	if(motor_speed == -1) motor_speed = 100;

	while(1)
	{
		lcd_draw_counter_ms++;

		DebouncedKeyPress = DebounceSwitch();

		if(DebouncedKeyPress) //If switch is pressed
    	{
	     	selected_value++;
	     	if(selected_value >= 2) {
	     		break;
	     	}
  		}

		enc_val_tmp = read_gray_code_from_encoder();

		if(enc_val != enc_val_tmp)
		{
			// If encoder was turned left
			if((enc_val==3 && enc_val_tmp == 1)) // || (val==0 && val_tmp==2))
			{
				enc_direction = 0;
				if(selected_value == 0) {
					tlapse_interval--;
					if(tlapse_interval < 1) tlapse_interval = 255;
				}
				if(selected_value == 1) {
					motor_speed--;
					if(motor_speed < 1) motor_speed = 1024;
				}
				
				
			}
			else if((enc_val==2 && enc_val_tmp==0)) // || (val==1 && val_tmp==3))
			{
				enc_direction = 1;
				if(selected_value == 0) {
					tlapse_interval++;
					if(tlapse_interval >= 255) tlapse_interval = 1;
				}
				if(selected_value == 1) {
					motor_speed++;
					if(motor_speed >= 1024) motor_speed = 1;
				}
			}
			enc_val = enc_val_tmp;
		}

		// Draw to LCD
		if(lcd_draw_counter_ms > lcd_draw_delay_ms) {
			if(selected_value==0) {
				Send_A_String_XY(2,1,"v");
				Send_A_String_XY(14,1," ");	
			}
			if(selected_value==1) {
				Send_A_String_XY(14,1,"v");
				Send_A_String_XY(2,1," ");
			}
			Send_A_String_XY(1, 2, "     ");
			Send_An_Integer_XY(1,2, tlapse_interval, 5);
			Send_A_String("s");
			Send_A_String_XY(11, 2, "     ");
			Send_An_Integer_XY(11,2, motor_speed, 5);
			Send_An_Integer_XY(16,1, enc_direction, 1);
			lcd_draw_counter_ms = 0;
		}
		
		_delay_ms(1);
  		
	}

	// Save current value to eeprom
	eeprom_update_word ((uint16_t*) TLAPSE_INT_ADDR, tlapse_interval);
	eeprom_update_word ((uint16_t*) MTR_SPD_ADDR, motor_speed);
	eeprom_update_word ((uint16_t*) TLAPSE_DUR_ADDR, tlapse_duration);

	Send_A_Command(0x01); // Clear screen

	_delay_ms(100);

	Goto_LCD_Location(1,1);
	Send_A_String("Data saved");

	_delay_ms(1000);

	Send_A_Command(0x01); // Clear screen

	_delay_ms(100);


	RunTimeLapseProgram();

	return 0;
}



	
// Reset motor
void InitializeMotor()
{
		//Make motor go to left limit switch
		//While loop that moves motor until left limit is reached
		//Set flag for motor ready
}