#ifndef GRIDEYE_H_
#define GRIDEYE_H_

#include <inttypes.h>
#include "drivers/mss_i2c/mss_i2c.h"


// GridEYE address is 0x69
#define gridEYE_ADDR 0b1101001


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


// Prints visual interpretation of grideye data to the console
void gridEYE_print(float temps[8][8]){

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
}//gridEYE_print()


#endif /* GRIDEYE_H_ */
