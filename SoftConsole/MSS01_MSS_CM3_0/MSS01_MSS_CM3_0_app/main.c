#include <stdio.h>
#include <inttypes.h>
#include "drivers/mss_uart/mss_uart.h"
#include "drivers/mss_i2c/mss_i2c.h"
#include "gridEYE.h"
#include "LED.h"
#include "sonic_dist.h"

/*
    Authored by:
    Darshin Patel
    Matthew Stachowiak
    Michael Manceor
    Nathan Vollbrecht
*/

//--- Global Variables ---
volatile uint32_t* LED = (uint32_t*)LED_ADDR;

volatile uint32_t* SONIC_READ = (uint32_t*) SONIC_ADDR;


// Function reverses bits in the byte
uint8_t byte_reverse(uint8_t x)
{
	return  0xFF & (((x & 0b10000000) >> 7) | ((x & 0b01000000) >> 5) | ((x & 0b00100000) >> 3) | ((x & 0b00010000) >> 1) |
		((x & 0b00001000) << 1) | ((x & 0b000000100) << 3) | ((x & 0b000000010) << 5) | ((x & 0b00000001) << 7));
}


// TESTING FUNCTION
// This is a function for testing color interaction with distance
uint32_t dist_to_color(float in_dist){

	uint8_t green_amount = 0;

	if(in_dist < 100.0){
		green_amount = 0xFF - (int)(2.55*in_dist);
	}


	uint32_t result = byte_reverse(green_amount);
	//uint32_t result = (blue_amount << 16) | (red_amount << 8) | green_amount;
	return result;

}


// TESTING FUNCTION
// Function for testing distance controlling LED number
// Each LED is 2.5 cm of distance
int dist_to_LED(float in_dist){

	return (int)in_dist / 3;
}



int main()
{
	// Setup
	LED_reset(LED);
	// Initialize with a clock frequency of ~ 400kHz
	MSS_I2C_init(&g_mss_i2c1 , gridEYE_ADDR, MSS_I2C_PCLK_DIV_256 );
	gridEYE_init();

	// Base pixel register is 0x80
	uint8_t pixel_addr[] = {0x80};
	uint8_t pixel_data[128] = {0};

	float temps[8][8];


	// testing
	float cm_dist = 0;
	uint32_t color = 0;
	int LED_num = 0;
	int i = 0;

	while( 1 ) {
		// gridEYE
		gridEYE_read(pixel_addr, pixel_data);
		get_temps_reversed(pixel_data, temps);
		// Sonic
		uint32_t DATA = *SONIC_READ;
		cm_dist = data_to_cm(DATA);
		LED_num = dist_to_LED(cm_dist);
		//color = dist_to_color(cm_dist);

		if(temps[3][3] > 24.00 || temps[4][4] > 24.0)
			color = red;
		else
			color = blue;

		// Write to LEDs

		for(i = 0; i < NUMLEDS; ++i){
			if(i== LED_num)
				LED[i] = color;
			else
				LED[i] = off;
		}

		/*for (i = 0; i < NUMLEDS; ++i) {
			LED[i] = color;
		}*/
		gridEYE_print(temps);

	}//while(1)
	return 0;
}
