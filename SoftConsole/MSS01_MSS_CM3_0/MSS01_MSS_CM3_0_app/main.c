#include <stdio.h>
#include <inttypes.h>
#include "drivers/mss_uart/mss_uart.h"
#include "drivers/mss_i2c/mss_i2c.h"

/*
    Authored by:
    Darshin Patel
    Matthew Stachowiak
    Michael Manceor
    Nathan Vollbrecht
*/

// LED Globals
#define LED_ADDR 0x40050000
#define NUMLEDS 8
#define blue 0xff0000
#define red 0x00ff00
#define green 0x0000ff
#define yellow 0x00ffff
#define off 0x0

volatile uint32_t* LED = (uint32_t*)LED_ADDR;

// Sonic globals
#define SONIC_ADDR 0x40050100;
volatile uint32_t* SONIC_READ = (uint32_t*) SONIC_ADDR;

// Grideye globals

// GridEYE address is 0x69
#define gridEYE_ADDR 0b1101001

// Reset LED values
void LED_reset(){
	int i;
	for (i = 0; i < NUMLEDS; ++i){
		LED[i] = off;
	}
}

//--- Distance Functions ---

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
	// uint32_t blue_amount = 0;
	uint32_t green_amount = 0;

	if(in_dist < 20.0){
		green_amount = 0xFF;
	//} else if(in_dist > 20.0){
		//green_amount = 0xFF;
	}
	else if (in_dist < 40.0)
		green_amount = 0x0F;
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

//--- Grideye Functions ---

// Perform a write to a register on the gridEYE
// reg_addr: 1-byte array containing address of register to read
// recieved_data: 1-byte array containing input data
void gridEYE_write(uint8_t* reg_addr, uint8_t* data){

	uint8_t signal[] = {*reg_addr, *data};

	MSS_I2C_write
		(
				&g_mss_i2c1,
				gridEYE_ADDR,
				signal,
				sizeof(signal),
				MSS_I2C_RELEASE_BUS
		);

	MSS_I2C_wait_complete(&g_mss_i2c1, MSS_I2C_NO_TIMEOUT);
}//gridEYE_write()


// Read from a specific register on the gridEYE
// reg_addr: 1-byte array containing address of register to read
// recieved_data: 1-byte array where received data will be placed
void gridEYE_read(uint8_t* reg_addr, uint8_t* recieved_data ){

	MSS_I2C_write_read
		(
				&g_mss_i2c1,
				gridEYE_ADDR,
				reg_addr,
				1,
				recieved_data,
				128,
				MSS_I2C_RELEASE_BUS
		);

	MSS_I2C_wait_complete(&g_mss_i2c1, MSS_I2C_NO_TIMEOUT);
}//gridEYE_read()


// Reset gridEYE
void gridEYE_reset(){

	uint8_t resAddr[] = {0x1};
	uint8_t resVal[] = {0x3f};
	gridEYE_write(resAddr, resVal);
}//gridEYE_reset()


// Set normal power mode
void gridEYE_set_normal_power(){

	uint8_t powAddr[] = {0x0};
	uint8_t powVal[] = {0x0};
	gridEYE_write(powAddr, powVal);
}//gridEYE_set_normal()


// Reset device, set operation to normal power
void gridEYE_init(){
	gridEYE_reset();
	gridEYE_set_normal_power();
}//grideye_init()

// Function converts a 12 bit 2C number to
// a signed float
float raw_to_temp(uint8_t upper, uint8_t lower){

	// TEMPERATURE CONVERSION
	// Data is recieved as a 12 bit 2's complement number
	// Each value represents a 0.25 C temperature difference

	int full = (upper << 8) | lower;

	// If input is negative, perform sign extension
	if (upper &  (1 << 3)){
		full |= 0xFFFFF000;
	}
	return (float)full*0.25;
}//raw_to_temp()

// Function converts 128 byte raw data array
// to a set of 64 signed float values
// Data is oriented in the same positions
// as the vision of the grideye (looking outward)
void get_temps_forward(uint8_t* data_in, float temps[][8]){

	int i = 0;
	int j = 0;
	int k = 7;

	// Iterate through all data
	for (i = 0; i < 128; i += 2) {
		// Move one column to the left every 16 input bytes
		if(i && i%16 == 0){
			--k;
			j = 0;
		}

		// Read upper and lower bytes, convert to float, store
		temps[j][k] = raw_to_temp(data_in[i + 1], data_in[i]);

		++j;
	}
}//get_temps_forward()

// Function converts 128 byte raw data array
// to a set of 64 signed float values
// This function also reverses the values from left
// to right for easier debugging while printing
void get_temps_reversed(uint8_t* data_in, float temps[][8]){

	int i = 0;
	int j = 0;
	int k = 0;

	// Iterate through all data
	for (i = 0; i < 128; i += 2) {
		// Move one column to the right every 16 input bytes
		if(i && i%16 == 0){
			++k;
			j = 0;
		}

		// Read upper and lower bytes, convert to float, store
		temps[j][k] = raw_to_temp(data_in[i + 1], data_in[i]);

		++j;
	}
}//get_temps_reversed()

int main()
{
	// Setup
	LED_reset();
	// Initialize with a clock freq of ~ 400kHz
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

		// Write to LEDs
		for(i = 0; i < 8; ++i){
			if(i== LED_num)
				LED[i] = blue;
			else
				LED[i] = off;
		}
		/*for(i = 0; i < 8; ++i){
			LED[i] = color;
		}*/

		//printf("Distance = %5.2f cm; Raw Data = %ld\r\n",cm_dist ,DATA);

		// Print grideye data
		printf("\r\n\n\n");
		int i = 0;
		int j = 0;
		for (i = 0; i < 8; ++i){
			for(j = 0; j < 8; ++j){
				if(temps[i][j] > 24.00)
					printf("# ");
				else
					printf(". ");
			}
			printf("\r\n");
		}
	}//while(1)
	return 0;
}