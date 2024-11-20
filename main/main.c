#include <stdio.h>
#include <stdbool.h>
#include <inttypes.h>
#include "wifi_config.h"
#include "iothub_receive.h"
#include "iothub_send.h"
#include "rc522_handler.h"
#include "sensors.h"
#include "charging.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "freertos/event_groups.h"
#include "lcd_display.h"

#include "freertos/timers.h"
#define connectionString "HostName=evcss.azure-devices.net;DeviceId=evcs;SharedAccessKey=EqWmrpBG9F2ux2TFOstLxyWeNdOBFA54/AIoTGgtcYc="

IOTHUB_DEVICE_CLIENT_LL_HANDLE device_ll_handle;
static bool g_continueRunning = true;
static bool g_continueRunning1 = true;
EventGroupHandle_t wifi_event_group;
char msgText[256];
char msg[256];
const char *TAG = "azure";

rc522_handle_t scanner;

int station_id = 6;

void receive(void* pvParameters) {
    while (1) {
        IoTHubDeviceClient_LL_DoWork(device_ll_handle);
        vTaskDelay(pdMS_TO_TICKS(1000)); 
            g_continueRunning = false;
        
    }
}
void send_task(void* pvParameters) {
    while (1) {
        // IoTHubClient_LL_DoWork(handle);
        vTaskDelay(pdMS_TO_TICKS(1000)); 
    }
}

void connection_status_callback(IOTHUB_CLIENT_CONNECTION_STATUS result, IOTHUB_CLIENT_CONNECTION_STATUS_REASON reason, void* user_context)
{
    (void)reason;
    (void)user_context;
    // This sample DOES NOT take into consideration network outages.
    if (result == IOTHUB_CLIENT_CONNECTION_AUTHENTICATED)
    {
        (void)printf("The device client is connected to iothub\r\n");
    }
    else
    {
        (void)printf("The device client has been disconnected\r\n");
    }
}


void app_main(){
    
    
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    ESP_ERROR_CHECK(ret);

    initialise_wifi();
    EventBits_t bits = xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT, pdFALSE, pdTRUE, portMAX_DELAY);
    if ((bits & CONNECTED_BIT) == 0) {
        printf("Failed to connect to WiFi. Aborting.\n");
        return;
    }
    printf("WiFi connected!\n");
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    configure_adc();
    /******* RFID Module*/
   // Initialize RC522
    rc522_config_t config = {
        .spi.host = VSPI_HOST,
        .spi.miso_gpio = 25,
        .spi.mosi_gpio = 23,
        .spi.sck_gpio = 19,
        .spi.sda_gpio = 22,
    };
// rc522_config_t config = {
//     .transport = RC522_TRANSPORT_I2C,
//     .i2c.sda_gpio = 26,
//     .i2c.scl_gpio = 27,
// };
    rc522_create(&config, &scanner);
    rc522_start(scanner);       
    /*IoT Hub*/
    // Used to initialize IoTHub SDK subsystem
    (void)IoTHub_Init();
    (void)printf("Creating IoTHub Device handle\r\n");

    // Create the iothub handle here
    device_ll_handle = IoTHubDeviceClient_LL_CreateFromConnectionString(connectionString, MQTT_Protocol);
    if (device_ll_handle == NULL)
    {
        (void)printf("Failure creating IotHub device. Hint: Check your connection string.\r\n");
    }
    else
    {
        
        // rc522_register_events(scanner, RC522_EVENT_ANY, rc522_handler, NULL);
        iothub_receive();
vTaskDelay(2000 / portTICK_PERIOD_MS);
        xTaskCreate(receive, "listen_task", 4096, NULL, 6, NULL);
        
    }
        // IoTHub_Deinit();
         xTaskCreate(send_task, "send_task", 4096, NULL, 5, NULL);
        // vTaskDelay(2000 / portTICK_PERIOD_MS);
}