/*
 * sensor_read_commands.h
 *
 * This file essentially serves as an interface between sensor drivers and RTOS.
 * Sensor drivers are (optimistically) standalone, while this file adds complexity
 * such as time delays and mutexes.
 *
 * Created: 11/1/2016 8:17:44 PM
 *  Author: mckenna
 */

#ifndef SENSOR_READ_COMMANDS_H
#define	SENSOR_READ_COMMANDS_H

#include "../global.h"
#include "../testing_functions/equisim_simulated_data.h"
#include "sensor_def.h"

/************************************************************************/
/* CRITICAL HARDWARE TIMINGS                                            */
/************************************************************************/
#define EN_5V_POWER_UP_DELAY_MS			10  // note we hold a mutex for this time
#define IR_WAKE_DELAY_MS				300

/* PDIODE BOUNDS */
#define PDIODE_00_01					460
#define PDIODE_01_10					505
#define PDIODE_10_11					550

typedef enum {
	BCDS_LF_B1_BT,
	BCDS_LF_B1_TT,
	BCDS_LF_B2_TT,
	BCDS_LF_B2_BT,
	BCDS_LF_B2_CHGN,
	BCDS_LF_B2_FAULTN,
	BCDS_LF_B1_CHGN,
	BCDS_LF_B1_FAULTN,
	BCDS_L2_ST,
	BCDS_L1_ST,
	BCDS_SPF_ST,
	BCDS_DUD,
	BCDS_L1_CHGN,
	BCDS_L1_FAULTN,
	BCDS_L2_CHGN,
	BCDS_L2_FAULTN
} bcds_conversions_t;

/************************************************************************/
/* HARDWARE MUTEXs - see https://www.draw.io/#G1bt9XDgZvyObssMtjbUi8nUNwu0kcQpVI */
/* **ONLY** can be used outside this task in the flash_activate_task (for speed purposes) */
/************************************************************************/
#define HARDWARE_MUTEX_WAIT_TIME_TICKS	(1000 / portTICK_PERIOD_MS)
StaticSemaphore_t _i2c_mutex_d;
SemaphoreHandle_t i2c_mutex;
StaticSemaphore_t _processor_adc_mutex_d;
SemaphoreHandle_t processor_adc_mutex;
// NOTE: this mutex is only used in LOW_POWER, otherwise IR power is kept on constantly
// (anyone can enable it, no one can disable it). Tasks using it in low power:
// LOW_POWER_DATA_TASK, TRANSMIT_TASK, BATTERY_CHARGING_TASK, 
StaticSemaphore_t _irpow_mutex_d;
SemaphoreHandle_t irpow_mutex;

/************************************************************************/
/* FUNCTIONS                                                            */
/************************************************************************/
uint8_t truncate_16t(uint16_t src, sig_id_t sig);
void init_sensor_read_commands(void);

/* primary */
void read_lion_volts_batch(			lion_volts_batch batch);
void read_ad7991_batbrd(			lion_current_batch batch1, panelref_lref_batch batch2);
void en_and_read_lion_temps_batch(	lion_temps_batch batch);
void read_lifepo_volts_batch(		lifepo_volts_batch batch);
void read_ir_object_temps_batch(	ir_object_temps_batch batch);
void read_imu_temp_batch(			imu_temp_batch* batch); // pointer to single value
void read_pdiode_batch(				pdiode_batch* batch); // pointer to single value
void read_ir_ambient_temps_batch(	ir_ambient_temps_batch batch);
void read_accel_batch(				accelerometer_batch accel_batch);
void read_gyro_batch(				gyro_batch gyr_batch);
void read_magnetometer_batch(		magnetometer_batch batch);
void read_radio_temp_batch(			radio_temp_batch* batch);			// pointer to single value
void read_ad7991_ctrlbrd(			ad7991_ctrlbrd_batch batch); //TODO: UNCOMMENT IF YOU NEED THIS; otherwise it goes
void read_bat_charge_dig_sigs_batch(bat_charge_dig_sigs_batch* batch);	// pointer to single value
bool read_field_from_bcds(			bat_charge_dig_sigs_batch batch, bcds_conversions_t shift);
void read_lifepo_current_batch(		lifepo_current_batch batch, bool flashing_now);

/* non-thread safe functions that should ONLY be called from FLASH TASK */
void _read_led_temps_batch_unsafe(			led_temps_batch batch);
void _read_lifepo_temps_batch_unsafe(		lifepo_bank_temps_batch batch);
void _read_lifepo_current_batch_unsafe(		lifepo_current_batch batch, bool flashing_now);
void _read_lifepo_volts_batch_unsafe(		lifepo_volts_batch batch);
void _read_led_current_batch_unsafe(		led_current_batch batch, bool flashing_now);
void _read_gyro_batch_unsafe(				gyro_batch gyr_batch);

/* utility */
void read_lion_volts_precise(uint16_t* val_1, uint16_t* val_2);
void read_lifepo_volts_precise(uint16_t* val_1, uint16_t* val_2, uint16_t* val_3, uint16_t* val_4);
void read_lion_current_precise(uint16_t* val_1, uint16_t* val_2);
void read_lifepo_current_precise(uint16_t* val_1, uint16_t* val_2, uint16_t* val_3, uint16_t* val_4);

bool enable_ir_pow_if_necessary(void); // ONLY used in flash task
void disable_ir_pow_if_necessary(bool got_mutex_on_en);
void disable_ir_pow_if_should_be_off(bool expected_on);
bool _set_5v_enable_unsafe(bool on);
void verify_regulators(void);
void verify_flash_readings(bool flashing);

#endif
