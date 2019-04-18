#include <stdio.h>
#include <inttypes.h>
#include "drivers/mss_uart/mss_uart.h"
#define LED1_ADDR 0x40050000
#define LED2_ADDR 0x40050004
#define LED3_ADDR 0x40050008
#define LED4_ADDR 0x4005000c
#define LED5_ADDR 0x40050010
#define LED6_ADDR 0x40050014
#define LED7_ADDR 0x40050018
#define LED8_ADDR 0x4005001c
#define blue 0xff0000
#define red 0x00ff00
#define green 0x0000ff
#define yellow 0x00ffff
#define off 0x0

#define BASE_ADDR 0x40050100;
volatile uint32_t* DATA_READ = (uint32_t*) BASE_ADDR;

float data_to_cm(uint32_t in_data){

	// Convert 10nS count to 1mS count

	float cm_data = (float)in_data / 100;

	// Divide by 56 to get data in cm according to datasheet

	return cm_data/56;

}

// This is a function for testing color interaction with distance
uint32_t dist_to_color(float in_dist){

	// 10 cm and less is full blue, 20 cm and more is full red
	// in between is a combination of the two
	uint32_t blue_amount = 0;
	uint32_t green_amount = 0;

	if(in_dist < 20.0){
		green_amount = 0xFF;
	//} else if(in_dist > 20.0){
		//green_amount = 0xFF;
	}
	else if (in_dist < 40.0)
		green_amount = 0x01;
	/*}else{
		//blue_amount = 0xFF - (in_dist - 10.0) * 10;
		//green_amount = (in_dist - 10.0)* 25.5;
	}*/

	//uint32_t result = (blue_amount << 16) | green_amount;
	uint32_t result = green_amount;
	return result;

}

// Function for testing distance controlling LED number
// Each LED is 10 cm of distance
int dist_to_LED(float in_dist){
	return (int)in_dist / 10;

}

int main()
{
	uint32_t* LED1 = (uint32_t*)LED1_ADDR;
	uint32_t* LED2 = (uint32_t*)LED2_ADDR;
	uint32_t* LED3 = (uint32_t*)LED3_ADDR;
	uint32_t* LED4 = (uint32_t*)LED4_ADDR;
	uint32_t* LED5 = (uint32_t*)LED5_ADDR;
	uint32_t* LED6 = (uint32_t*)LED6_ADDR;
	uint32_t* LED7 = (uint32_t*)LED7_ADDR;
	uint32_t* LED8 = (uint32_t*)LED8_ADDR;
	*LED1 = off;
	*LED2 = off;
	*LED3 = off;
	*LED4 = off;
	*LED5 = off;
	*LED6 = off;
	*LED7 = off;
	*LED8 = off;

	// testing
	//int count = 0;
	float cm_dist = 0;
	uint32_t color = 0;
	int LED_num = 0;
	int i = 0;
	while( 1 ) {
		uint32_t DATA = *DATA_READ;
		cm_dist = data_to_cm(DATA);
		LED_num = dist_to_LED(cm_dist);
		color = dist_to_color(cm_dist);
		/*for(i = 0; i < 8; ++i){
			if(i== LED_num)
				LED1[i] = blue;
			else
				LED1[i] = off;
		}*/
		for(i = 0; i < 8; ++i){
			LED1[i] = color;
		}

		printf("Distance = %5.2f cm; Raw Data = %ld\r\n",cm_dist ,DATA);
	}
	return 0;
}
