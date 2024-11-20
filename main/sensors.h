#ifndef SENSORS_H
#define SENSORS_H



#include <stdio.h>
#include <driver/gpio.h>
#include <driver/adc.h>
#include <esp_log.h>
#include <math.h>
#include "driver/ledc.h"
#include "esp_adc_cal.h"
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"

#define DC_RELAY_CONTACTOR 4

#define CP_READ ADC1_CHANNEL_0 //36
#define PWM_CP 25
#define PP ADC1_CHANNEL_3 //39

#define CP_PWM_PIN 18
#define PP_PIN ADC1_CHANNEL_9

#define CURRENT_MODULE ADC1_CHANNEL_6 //34
#define VOLTAGE_MODULE  ADC1_CHANNEL_7 //35
#define READ_PWD_SIGNAL ADC1_CHANNEL_5 
#include <time.h>
void configure_adc(void);
float read_volt(adc1_channel_t channel);
float read_cp(adc1_channel_t channel);
float read_Current(adc1_channel_t channel);
float energy_consumed(float Voltage, float Current, time_t time_seconds);
void cp_pwm(float current);
float read_pp(adc1_channel_t channel);
extern void set_cp_pwm_duty(duty_cycle_res);

#endif // SENSORS_H
