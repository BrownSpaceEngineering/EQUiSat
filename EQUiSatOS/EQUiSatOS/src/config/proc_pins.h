/*
 * proc_pins.h
 *
 * Created: 3/8/2017 8:10:30 PM
 *  Author: Tyler
 */ 


#ifndef PROC_PINS_H_
#define PROC_PINS_H_

#define CNTRL_BRD_V3

#ifdef CNTRL_BRD_V3

	//ADC
	#define P_AI_LED1SNS	ADC_POSITIVE_INPUT_PIN11
	#define P_AI_LED2SNS	ADC_POSITIVE_INPUT_PIN10
	#define P_AI_LED3SNS	ADC_POSITIVE_INPUT_PIN9
	#define P_AI_LED4SNS	ADC_POSITIVE_INPUT_PIN8
	#define P_AI_TEMP_OUT	ADC_POSITIVE_INPUT_PIN2
	#define P_AI_PD_OUT		ADC_POSITIVE_INPUT_PIN0
	#define LFB1OSNS		ADC_POSITIVE_INPUT_PIN19
	#define LFB1SNS			ADC_POSITIVE_INPUT_PIN18
	#define LFB2OSNS		ADC_POSITIVE_INPUT_PIN17
	#define LFB2SNS			ADC_POSITIVE_INPUT_PIN16
	#define LF1REF			ADC_POSITIVE_INPUT_PIN6
	#define LF2REF			ADC_POSITIVE_INPUT_PIN5
	#define LF3REF			ADC_POSITIVE_INPUT_PIN3
	#define LF4REF			ADC_POSITIVE_INPUT_PIN4
	#define L1_REF			ADC_POSITIVE_INPUT_PIN0
	#define L2_REF			ADC_POSITIVE_INPUT_PIN7
	
	//GPIO Output
	#define P_TX_EN			PIN_PA23 //active low
	#define P_RX_EN			PIN_PA22 //active low
	#define P_RAD_SHDN		PIN_PA21 //active low (to shutdown)
	#define P_RAD_PWR_RUN	PIN_PA20
	#define P_IR_PWR_CMD	PIN_PB17
	#define P_LED_CMD		PIN_PA12 //active low
	#define P_5V_EN			PIN_PB11
	#define L1_RUN_CHG		PIN_PA01
	#define L2_RUN_CHG		PIN_PB04
	#define LF_B1_RUNCHG	PIN_PB05
	#define LF_B2_RUNCHG	PIN_PB10
	#define L1_DISG			PIN_PB31 //active low
	#define L2_DISG			PIN_PA00 //active low
	#define LF_B1_OUTEN		PIN_PB06
	#define LF_B2_OUTEN		PIN_PB07
	
	//GPIO Input
	#define P_DET_RTN		PIN_PB14
	
	//PWM/TCC
	#define P_ANT_DRV1		PIN_PA13
	#define P_ANT_DRV2		PIN_PA14
	#define P_ANT_DRV3		PIN_PA15
	
	//I2C
	//#define P_SDA			PIN_PB12
	//#define P_SCL			PIN_PB13
	
	//Radio
	//#define P_TX_DATA		PIN_PA24
	//#define P_RX_DATA		PIN_PA25
		
	//SPI
	//#define P_SO			PIN_PA16
	//#define P_CS			PIN_PA17 //active low
	//#define P_SI			PIN_PA18
	//#define P_SCK			PIN_PA19
	//#define P_CS2			PIN_PA16 //active low			

#endif

#endif /* PROC_PINS_H_ */