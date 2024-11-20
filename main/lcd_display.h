#ifndef EXAMPLE_H
#define EXAMPLE_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_freertos_hooks.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "esp_rom_gpio.h"


#include "lvgl.h"
#include "lvgl/lvgl.h"
#include "lv_demo_widgets/lv_demo_widgets.h"
#include "lvgl_helpers.h"
#include <driver/adc.h>
#include "sensors.h"
#define LV_TICK_PERIOD_MS 1

#ifdef __cplusplus
extern "C" {
#endif

#define CURRENT_MODULE ADC1_CHANNEL_6 //34
#define VOLTAGE_MODULE  ADC1_CHANNEL_7 //35
extern SemaphoreHandle_t xSemaphore;
void guiTask(void *pvParameter);

void create_demo_application();
void lv_tick_task(void *arg);
// void my_task(lv_task_t *task);
void update_display();
void update_text();
#ifdef __cplusplus
}
#endif

#endif 