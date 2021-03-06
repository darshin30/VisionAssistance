#ifndef SONIC_DIST_H_
#define SONIC_DIST_H_

// Sonic globals
#define SONIC_ADDR 0x40050100;


// Returns a decimal value representation of the distance in centimeters
float data_to_cm(uint32_t in_data){

	// Convert 10nS count to 1mS count
	float cm_data = (float)in_data / 100;

	// Divide by 56 to get data in cm according to datasheet
	return cm_data/56;
}

#endif /* SONIC_DIST_H_ */
