#ifndef CHARGING_H
#define CHARGING_H

#include "sensors.h"
#include "iothub_receive.h"
#include "iothub_send.h"
#include "driver/ledc.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <driver/gpio.h>
#include <driver/adc.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include <esp_log.h>
#include <string.h>
#include "lcd_display.h"

#define DC_RELAY_CONTACTOR 4
#define CP_READ ADC1_CHANNEL_0 //36
#define PWM_CP 25
#define PP ADC1_CHANNEL_3 //39

#define CP_PWM_PIN 18

#define CURRENT_MODULE ADC1_CHANNEL_6 //34
#define VOLTAGE_MODULE  ADC1_CHANNEL_7 //35

 char *start_charging();
 void stop_charging(void);
 void set_cp_pwm_duty(uint32_t duty);
 void guiTask(void *pvParameter);
 float read_pp(adc1_channel_t channel);

#endif // CHARGING_H
