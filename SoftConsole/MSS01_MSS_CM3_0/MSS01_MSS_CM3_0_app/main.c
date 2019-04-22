#include <stdio.h>
#include <inttypes.h>
#include <assert.h>
#include <math.h>
#include "drivers/mss_uart/mss_uart.h"
#include "drivers/mss_i2c/mss_i2c.h"
#include "drivers/mss_gpio/mss_gpio.h"
#include "gridEYE.h"
#include "LED.h"
#include "sonic_dist.h"
#include "imu_interface.h"

#define WAND_IMU_ADDR 		0x28
#define GLASSES_IMU_ADDR 	0x29

#define DISPLAY_FOV 72 // degrees
#define DISPLAY_POINT_WIDTH 2 // num of LEDs to light up at a time
#define NO_LED -1000

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

int int_count;
// Function reverses bits in the byte
uint8_t byte_reverse(uint8_t x)
{
	return  0xFF & (((x & 0b10000000) >> 7) | ((x & 0b01000000) >> 5) | ((x & 0b00100000) >> 3) | ((x & 0b00010000) >> 1) |
		((x & 0b00001000) << 1) | ((x & 0b000000100) << 3) | ((x & 0b000000010) << 5) | ((x & 0b00000001) << 7));
}


// TESTING FUNCTION
// This is a function for testing color interaction with distance
uint32_t scale_brightness(int32_t color, float in_dist){
	uint i;
	uint32_t result = 0;
	double scale_factor;

	for (i = 0; i<3; ++i)
	{
		uint8_t ind_value = byte_reverse((color >> 8*(i)) & 0xFF);
		scale_factor = 2.17 - 0.332*log(in_dist);
		if (scale_factor > 1) scale_factor = 1;
		if (scale_factor < 0) scale_factor = 0;
		//printf("Brightness scaling: %f\r\n", scale_factor);
		result = result | (byte_reverse(ind_value*scale_factor) << 8*(i));
	}
	//	green_amount = 0xFF - (int)(2.55*in_dist);

	//	uint32_t result = byte_reverse(green_amount);
	//uint32_t result = (blue_amount << 16) | (red_amount << 8) | green_amount;
	return result;
}


// TESTING FUNCTION
// Function for testing distance controlling LED number
// Each LED is 2.5 cm of distance
int dist_to_LED(float in_dist){

	return (int)in_dist / 3;
}

void GPIO0_IRQHandler (void){
	printf("Interrupt Happened: %d \r\n", int_count);
	MSS_GPIO_clear_irq(MSS_GPIO_0);
	int_count++;
}

int main()
{
	int_count = 0;
	// Setup
	LED_reset(LED);
	// Initialize GPIO for interrupts
	MSS_GPIO_init();
	MSS_GPIO_config( MSS_GPIO_0, MSS_GPIO_INPUT_MODE | MSS_GPIO_IRQ_EDGE_POSITIVE );
	MSS_GPIO_config( MSS_GPIO_1, MSS_GPIO_INPUT_MODE | MSS_GPIO_IRQ_EDGE_POSITIVE );
	MSS_GPIO_enable_irq( MSS_GPIO_0 );
	MSS_GPIO_enable_irq( MSS_GPIO_1 );
	// Initialize with a clock frequency of ~ 400kHz
	MSS_I2C_init(&g_mss_i2c1 , 0x0, MSS_I2C_PCLK_DIV_256 );
	gridEYE_init();
	if (init_BNO055(WAND_IMU_ADDR)) assert("IMU init error");
	if (init_BNO055(GLASSES_IMU_ADDR)) assert("IMU init error");

	// Base pixel register is 0x80
	uint8_t pixel_addr[] = {0x80};
	uint8_t pixel_data[128] = {0};

	float temps[8][8];


	// Loop variables
	float cm_dist = 0;
	uint32_t color = 0;
	int LED_num = 0;
	int i = 0;
	double display_angle;
	int8_t set_heading_baseline = 1;

	while( 1 ) {
		// gridEYE
		gridEYE_read(pixel_addr, pixel_data);
		get_temps_reversed(pixel_data, temps);

		if(temps[3][3] > 24.00 || temps[4][4] > 24.0)
			color = yellow;
		else
			color = green;

		// Sonic: calculate dist and set brightness
		uint32_t DATA = *SONIC_READ;
		cm_dist = data_to_cm(DATA);
//		LED_num = dist_to_LED(cm_dist);
		//color = dist_to_color(cm_dist);

		// IMUs: read and calculate angle at which to display
		display_angle = calc_display_angle(GLASSES_IMU_ADDR, WAND_IMU_ADDR, set_heading_baseline);
		set_heading_baseline = 0;
		if (display_angle < -DISPLAY_FOV/2 || display_angle >= DISPLAY_FOV/2){
			LED_num = display_angle < 0 ? NUMLEDS-1 : 0 - (DISPLAY_POINT_WIDTH-1);
			color = red;
		}
		else
			LED_num = (NUMLEDS-1) - (int)(display_angle/(DISPLAY_FOV/NUMLEDS) + NUMLEDS/2);



		// Write to LEDs
		for(i = 0; i < NUMLEDS; ++i){
			if (i >= LED_num && i < LED_num + DISPLAY_POINT_WIDTH)
				LED[i] = scale_brightness(color, cm_dist);
			else
				LED[i] = off;
		}
//		for (i = 0; i < NUMLEDS; ++i) {
//			LED[i] = color;
//		}

		//gridEYE_print(temps);
		//printf("%x\r\n", color);

	}//while(1)
	return 0;
}
