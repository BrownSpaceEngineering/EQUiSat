/*
 * sat_data_tests.c
 *
 * Created: 1/18/2018 23:13:08
 *  Author: mcken
 */ 
#include "sat_data_tests.h"

void fill_random_data(uint8_t* data, int num) {
	for (int i = 0; i < num; i++) {
		data[i] = rand();
	}
}

void populate_equistack(equistack* stack) {
	uint8_t* data = equistack_Initial_Stage(stack);
	for (int i = 0; i < stack->max_size; i++) {
		fill_random_data(data, stack->data_size);
		data = equistack_Stage(stack);
	}
}

// fills all data equistacks with random data
void populate_equistacks(void) {
	populate_equistack(&idle_readings_equistack);
	populate_equistack(&attitude_readings_equistack);
	populate_equistack(&flash_readings_equistack);
	populate_equistack(&flash_cmp_readings_equistack);
	populate_equistack(&low_power_readings_equistack);
}

// fills all data equistacks and then tests packaging that data
// using the transmission packaging code
void test_message_packaging(void) {
	populate_equistacks();
	
	uint8_t msg_buffer[MSG_BUFFER_SIZE];
	
	uint32_t current_timestamp = get_current_timestamp();
	write_packet(msg_buffer, IDLE_DATA, current_timestamp);
	write_packet(msg_buffer, ATTITUDE_DATA, current_timestamp);
	write_packet(msg_buffer, FLASH_DATA, current_timestamp);
	write_packet(msg_buffer, FLASH_CMP_DATA, current_timestamp);
	write_packet(msg_buffer, LOW_POWER_DATA, current_timestamp);
}