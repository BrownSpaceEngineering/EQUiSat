/*
* package_transmission.c
*
* Created: 12/6/2016 8:47:44 PM
* Author: jleiken
*/
#include "package_transmission.h"

void assert_transmission_constants(void) {
	// check things will fit in buffer (one space for \0)
	int top_length = MSG_PREAMBLE_LENGTH + MSG_HEADER_LENGTH + ERROR_PACKET_SIZE * ERROR_PACKETS;
	assert(top_length + ATTITUDE_DATA_PACKET_SIZE * ATTITUDE_DATA_PACKETS < MSG_BUFFER_SIZE - 1);
	assert(top_length + FLASH_DATA_PACKET_SIZE * FLASH_DATA_PACKETS < MSG_BUFFER_SIZE - 1);
	assert(top_length + FLASH_CMP_PACKET_SIZE * FLASH_DATA_PACKETS < MSG_BUFFER_SIZE - 1);
	
	// check that starts line up with data sizes
	assert(MSG_PREAMBLE_LENGTH == START_HEADER);
	assert(MSG_PREAMBLE_LENGTH + MSG_HEADER_LENGTH == START_ERRORS);
	assert(MSG_PREAMBLE_LENGTH + MSG_HEADER_LENGTH + ERROR_PACKET_SIZE * ERROR_PACKETS == START_DATA);
}

/**
 * Writes the given data to the preamble of the message buffer.
 */
void write_preamble(char* buffer, uint32_t timestamp, uint32_t states, uint8_t data_len) {
	uint8_t index = START_PREAMBLE;
	
	// write callsign
	buffer[index++] = 'K';
	buffer[index++] = '1';
	buffer[index++] = 'A';
	buffer[index++] = 'D';
	
	write_bytes_and_shift(buffer,	timestamp,		sizeof(timestamp),	&index); // 4 byte timestamp
	write_bytes_and_shift(buffer,	states,			sizeof(states),		&index); // 4 byte state string
	write_bytes_and_shift(buffer,	ERROR_PACKETS,	1,					&index); // 1 byte error packet #
	write_bytes_and_shift(buffer,	data_len,		sizeof(data_len),	&index); // 1 byte data packet #
	
	assert(index == START_HEADER);
	
	// just in case we miss a \0... it should normally be written at the end of data
	buffer[MSG_BUFFER_SIZE - 1] = '\0'; 
}

void write_header(char* buffer, idle_data_t *idle_data) {
	uint8_t index = START_HEADER;
	
	if (idle_data != NULL) {
		// TODO: drop two LSB (see write_bytes_and_shift_truncating)
		write_bytes_and_shift(buffer, idle_data->lion_volts_data,			sizeof(lion_volts_batch),			&index);
		write_bytes_and_shift(buffer, idle_data->lion_current_data,			sizeof(lion_current_batch),			&index);
		write_bytes_and_shift(buffer, idle_data->bat_charge_volts_data,		sizeof(bat_charge_volts_batch),		&index);
		write_bytes_and_shift(buffer, idle_data->bat_charge_dig_sigs_data,	sizeof(bat_charge_dig_sigs_batch),	&index);
		write_bytes_and_shift(buffer, idle_data->digital_out_data,			sizeof(digital_out_batch),			&index);
	} else {
		// write all 0s to buffer for idle data
		write_value_and_shift(buffer, 0, MSG_HEADER_LENGTH, &index);
		log_error(ELOC_PACKAGE_TRANS, ECODE_NULL_IDLE_DATA);
	}
		
	assert(index == START_ERRORS);
}

void write_errors(char* buffer, equistack* error_stack) {
	uint8_t index = START_ERRORS;	
	
	// note: _PACKETS should be less than _STACK_MAX, or we're wasting space!
	for (int i = 0; i < ERROR_PACKETS; i++) {
		sat_error_t *error = (sat_error_t*) equistack_Get(error_stack, i);
		
		// we have to fill up the error section, so either write the error or it's null equivalent
		if (error != NULL) {
			write_bytes_and_shift(buffer, *error,			sizeof(sat_error_t), &index);
		} else {
			write_bytes_and_shift(buffer, ECODE_NO_ERROR,	sizeof(sat_error_t), &index);
		}
	}
	
	assert(index == START_DATA);
}

void write_idle_data(char* buffer, equistack* idle_stack) {
	uint8_t index = START_DATA;
	
	// note: _PACKETS should be less than _STACK_MAX, or we're wasting space!
	for (int i = 0; i < IDLE_DATA_PACKETS; i++) {
		idle_data_t *idle_data = (idle_data_t*) equistack_Get(idle_stack, i);
		
		if (idle_data != NULL) {
			write_bytes_and_shift(buffer, idle_data->bat_temp_data,			sizeof(bat_temp_batch),				&index);
			write_bytes_and_shift(buffer, idle_data->radio_temp_data,		sizeof(radio_temp_batch),			&index);
			write_bytes_and_shift(buffer, idle_data->radio_volts_data,		sizeof(radio_volts_batch),			&index);
			write_bytes_and_shift(buffer, idle_data->imu_temp_data,			sizeof(imu_temp_batch),				&index);
			write_bytes_and_shift(buffer, idle_data->ir_temps_data,			sizeof(ir_temps_batch),				&index);
			write_bytes_and_shift(buffer, idle_data->rail_3v_data,			sizeof(rail_3v_batch),				&index);
			write_bytes_and_shift(buffer, idle_data->rail_5v_data,			sizeof(rail_5v_batch),				&index);
		} else {
			// write all 0s to buffer for idle data
			write_value_and_shift(buffer, 0, IDLE_DATA_PACKET_SIZE, &index);
			log_error(ELOC_PACKAGE_TRANS, ECODE_NULL_IDLE_DATA);
		}
	}
	
	assert(index == START_DATA + IDLE_DATA_PACKET_SIZE * IDLE_DATA_PACKETS);
	// TODO: Write padding
}

void write_attitude_data(char* buffer, equistack* attitude_stack) {
	uint8_t index = START_DATA;
	
	// note: _PACKETS should be less than _STACK_MAX, or we're wasting space!
	for (int i = 0; i < ATTITUDE_DATA_PACKETS; i++) {
		attitude_data_t *attitude_data = (attitude_data_t*) equistack_Get(attitude_stack, i);
		
		// we have to fill up the entire section, so either write the data or its null equivalent
		if (attitude_data != NULL) {
			write_bytes_and_shift(buffer, attitude_data->ir_data,			sizeof(ir_batch)				/* [1] */,		&index);
			write_bytes_and_shift(buffer, attitude_data->diode_data,		sizeof(diode_batch)				/* [1] */,		&index);
			write_bytes_and_shift(buffer, attitude_data->accelerometer_data,sizeof(accelerometer_batch) * 2 /* [2] */,		&index);
			write_bytes_and_shift(buffer, attitude_data->gyro_data,			sizeof(gyro_batch)				/* [2] */,		&index);
			write_bytes_and_shift(buffer, attitude_data->magnetometer_data,	sizeof(magnetometer_batch)		/* [1] */,		&index); 
			write_bytes_and_shift(buffer, attitude_data->timestamp,			4 /* uint32_t */,								&index); 
		} else {
			// overwrite entire section with 0s
			write_value_and_shift(buffer, 0, ATTITUDE_DATA_PACKET_SIZE, &index);
		}
	}
	
	assert(index == START_DATA + ATTITUDE_DATA_PACKET_SIZE * ATTITUDE_DATA_PACKETS);
	// TODO: Write padding
}

void write_flash_data(char* buffer, equistack* flash_stack) {
	uint8_t index = START_DATA;
	
	// note: _PACKETS should be less than _STACK_MAX, or we're wasting space!
	for (int i = 0; i < FLASH_DATA_PACKETS; i++) {
		flash_data_t *flash_data = (flash_data_t*) equistack_Get(flash_stack, i);
		
		// we have to fill up the entire section, so either write the data or its null equivalent
		if (flash_data != NULL) {
			write_bytes_and_shift(buffer, flash_data->led_temps_data,		sizeof(led_temps_batch)		* 10 /* [10] */,			&index); 
			write_bytes_and_shift(buffer, flash_data->lifepo_volts_data,	sizeof(lifepo_volts_batch)	* 10 /* [10] */,			&index); 
			write_bytes_and_shift(buffer, flash_data->lifepo_current_data,	sizeof(lifepo_current_batch)* 10 /* [10] */,			&index); 
			write_bytes_and_shift(buffer, flash_data->led_current_data,		sizeof(led_current_batch)	* 10 /* [10] */,			&index); 
			write_bytes_and_shift(buffer, flash_data->timestamp,			4 /* uint_32_t */,									&index);
			
		} else {
			// overwrite entire section with 0s
			write_value_and_shift(buffer, 0, FLASH_DATA_PACKET_SIZE, &index);
		}
	}
	
	assert(index == START_DATA + FLASH_DATA_PACKET_SIZE * FLASH_DATA_PACKETS);
	// TODO: Write padding
}

void write_flash_cmp(char* buffer, equistack* flash_cmp_stack) {
	uint8_t index = START_DATA;
	
	// note: _PACKETS should be less than _STACK_MAX, or we're wasting space!
	for (int i = 0; i < FLASH_CMP_PACKETS; i++) {
		flash_cmp_t *flash_cmp = (flash_cmp_t*) equistack_Get(flash_cmp_stack, i);
		
		// we have to fill up the entire section, so either write the data or its null equivalent
		if (flash_cmp != NULL) {
			/* NOTE: Though these are the same types as in flash_data_t, they are those values AVERAGED */
			write_bytes_and_shift(buffer, flash_cmp->led_temps_avg_data,		sizeof(led_temps_batch)			/* [1] */,			&index);
			write_bytes_and_shift(buffer, flash_cmp->lifepo_volts_avg_data,		sizeof(lifepo_volts_batch)		/* [1] */,			&index);
			write_bytes_and_shift(buffer, flash_cmp->lifepo_current_avg_data,	sizeof(lifepo_current_batch)	/* [1] */,			&index);
			write_bytes_and_shift(buffer, flash_cmp->led_current_avg_data,		sizeof(led_current_batch)		/* [1] */,			&index);
			write_bytes_and_shift(buffer, flash_cmp->timestamp,					4 /* uint_32_t */,									&index);
			
			} else {
			// overwrite entire section with 0s
			write_value_and_shift(buffer, 0, FLASH_CMP_PACKET_SIZE, &index);
		}
	}
	
	assert(index == START_DATA + FLASH_CMP_PACKET_SIZE * FLASH_CMP_PACKETS);
	// TODO: Write padding
}

/* Writes num_bytes from input to data, and shifts the value at index up by num_bytes */
void write_bytes_and_shift(char *data, void *input, size_t num_bytes, uint8_t *index) {
	memcpy(data[*index], (char*) input, num_bytes);
	*index += num_bytes;
}

/* Writes num_bytes copies of value to data, and shifts the value at index up by num_bytes */
void write_value_and_shift(char *data, char value, size_t num_bytes, uint8_t *index) {
	for (int i = 0; i < num_bytes; i++) {
		data[*index] = value;
		(*index)++;
	}
}