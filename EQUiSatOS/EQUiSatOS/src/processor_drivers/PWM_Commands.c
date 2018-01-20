/*
 * PWM_COMMANDS.c
 *
 * Created: 12/4/2016 3:55:13 PM
 *  Author: Daniel
 */ 

#include "PWM_Commands.h"

void configure_pwm(int pwm_pin, int pwm_mux, int p_ant) {
	struct tcc_config config_tcc;
	tcc_get_config_defaults(&config_tcc, CONF_PWM_MODULE);
	
	config_tcc.counter.clock_prescaler = TCC_CLOCK_PRESCALER_DIV1; //clock prescaler
	config_tcc.counter.period = PWM_PERIOD; //maximum number of counter
	config_tcc.compare.wave_generation = TCC_WAVE_GENERATION_SINGLE_SLOPE_PWM;
	int wo;
	if (p_ant == 1) {
		channel = 3;
		wo = 7;
	} else if (p_ant == 2) {
		channel = 0;
		wo = 4;
	} else if (p_ant == 3) {
		channel = 1;
		wo = 5;
	}
	config_tcc.compare.match[channel] = PWM_ON_PERIOD; //when to change output
	
	config_tcc.pins.enable_wave_out_pin[wo] = true;
	config_tcc.pins.wave_out_pin[wo]        = pwm_pin;
	config_tcc.pins.wave_out_pin_mux[wo]    = pwm_mux;

	tcc_init(&tcc_instance, CONF_PWM_MODULE, &config_tcc);

	tcc_enable(&tcc_instance);
}

bool set_pulse_width_fraction(int numerator, int denominator) {
	if (numerator > denominator) {
		return false;
	} else {
		int toSet = (PWM_PERIOD * numerator) / denominator;
		tcc_set_compare_value(&tcc_instance, channel, toSet);
		return true;
	}
}

void disable_pwm(void) {
	tcc_disable(&tcc_instance);
	tcc_reset(&tcc_instance);
}