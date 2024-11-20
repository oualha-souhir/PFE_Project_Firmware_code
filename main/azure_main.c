#include <stdio.h>
#include <stdbool.h>
#include <inttypes.h>
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "rc522.h" // Assuming you have the RC522 library
#include "iothub_client_ll.h"
#include "iothub_device_client_ll.h"
#include "iothub_message.h"
#include "iothubtransportmqtt.h"
#include "azure_c_shared_utility/platform.h"
#include "iothub.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#include "iothub_client_options.h"
#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/shared_util_options.h"
#include "azure_c_shared_utility/crt_abstractions.h"

// WiFi credentials
#define EXAMPLE_WIFI_SSID "*****"
#define EXAMPLE_WIFI_PASS "*****"


// Azure IoT Hub connection string
#define connectionString "HostName=evcsHub.azure-devices.net;DeviceId=evcsDevice;SharedAccessKey=CGPPVhTlK0ml/EjrbfXDO1CZR2X021e1NAIoTL5VFfQ="
#define CONFIG_MESSAGE_INTERVAL_TIME 1000

EventGroupHandle_t wifi_event_group;
#define CONNECTED_BIT BIT0
static const char *TAG = "azure";
rc522_event_data_t event_data;
static rc522_handle_t scanner;

esp_event_handler_instance_t instance_any_id;
#ifndef BIT0
#define BIT0 (0x1 << 0)
#endif




static void event_handler(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        /* This is a workaround as ESP platform WiFi libs don't currently
           auto-reassociate. */
        esp_wifi_connect();
        xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
    }
}


static void initialise_wifi(void)
{

    ESP_ERROR_CHECK( esp_netif_init() );
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK( esp_event_loop_create_default() );
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);


    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_WIFI_SSID,
            .password = EXAMPLE_WIFI_PASS,
        },
    };
    ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);

#ifdef CONFIG_IDF_TARGET_ESP8266 || (ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(4, 0, 0))
#else
    ESP_ERROR_CHECK( esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL) );
    ESP_ERROR_CHECK( esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL) );
#endif

    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
    ESP_ERROR_CHECK( esp_wifi_start() );
}

#define MESSAGE_COUNT        1
static bool g_continueRunning = true;
static bool g_continueRunning1 = true;
static IOTHUB_DEVICE_CLIENT_LL_HANDLE device_ll_handle;

static size_t g_message_recv_count = 0;

bool vehiculeDetected= false;
void vehicule_detection(void* pvParameters) {
    while (1) {
        // vehiculeDetected = !(Read(CP));
        // vTaskDelay(pdMS_TO_TICKS(1000)); // Adjust delay as needed
    }
}
static IOTHUBMESSAGE_DISPOSITION_RESULT receive_msg_callback(IOTHUB_MESSAGE_HANDLE message, void* user_context)
{
    (void)user_context;
    const char* messageId;
    const char* correlationId;

    // Message properties
    if ((messageId = IoTHubMessage_GetMessageId(message)) == NULL)
    {
        messageId = "<unavailable>";
    }

    if ((correlationId = IoTHubMessage_GetCorrelationId(message)) == NULL)
    {
        correlationId = "<unavailable>";
    }

    IOTHUBMESSAGE_CONTENT_TYPE content_type = IoTHubMessage_GetContentType(message);
    if (content_type == IOTHUBMESSAGE_BYTEARRAY)
    {
        const unsigned char* buff_msg;
        size_t buff_len;

        if (IoTHubMessage_GetByteArray(message, &buff_msg, &buff_len) != IOTHUB_MESSAGE_OK)
        {
            (void)printf("Failure retrieving byte array message\r\n");
        }
        else
        {
            (void)printf("Received Binary message\r\nMessage ID: %s\r\n Correlation ID: %s\r\n Data: <<<%.*s>>> & Size=%d\r\n", messageId, correlationId, (int)buff_len, buff_msg, (int)buff_len);
            cJSON *json = cJSON_Parse((const char *)buff_msg);
                if (json != NULL) {
                        cJSON *item = cJSON_GetArrayItem(json, 0);
                        if (item != NULL) {
                            cJSON *rfid_tag = cJSON_GetObjectItemCaseSensitive(item, "RFID_tag");
                            cJSON *account_balance = cJSON_GetObjectItemCaseSensitive(item, "account_balance");

                            if (cJSON_IsString(rfid_tag) && (rfid_tag->valuestring != NULL)) {
                                // Assuming rfid_tag is of the form "%.*s"
                                //int length = strlen(rfid_tag->valuestring);
                                //printf("RFID_tag: %.*s\n", length, rfid_tag->valuestring);
                                const char *rfid_tag_value = rfid_tag->valuestring;
                                if (strcmp(rfid_tag_value, "0") == 0) {
                                    printf("Not a client\n");
                                } else {
                                    printf("RFID_tag: %s\n", rfid_tag_value);
                                }
                            }
                            if (cJSON_IsNumber(account_balance)) {
                                int balance = account_balance->valueint;
                                if (balance == 0) {
                                    printf("you don't have enough balance in your account\n");
                                } else {
                                    printf("your account balance is: %d\n", balance);
                                    while (balance){
                                        // DC_Relay_contactor = HIGH
                                            while (vehiculeDetected == true && CP== LOW ){
                                            // READ currrent
                                            // read voltage 
                                            // calcultate energy consumed
                                            // oprn websocket over MQTT_Protocol
                                            // balance = balance - energy_consume * charging_rate
                                        }
                                        // send stopped to iothub and new balance
                                    }
                                }
                            }
                        }
                        cJSON_Delete(json);
                    }
        }
    }
    else
    {
        const char* string_msg = IoTHubMessage_GetString(message);
        if (string_msg == NULL)
        {
            (void)printf("Failure retrieving byte array message\r\n");
        }
        else
        {
            (void)printf("Received String Message\r\nMessage ID: %s\r\n Correlation ID: %s\r\n Data: <<<%s>>>\r\n", messageId, correlationId, string_msg);
        }
    }
    const char* property_value = "property_value";
    const char* property_key = IoTHubMessage_GetProperty(message, property_value);
    if (property_key != NULL)
    {
        printf("\r\nMessage Properties:\r\n");
        printf("\tKey: %s Value: %s\r\n", property_value, property_key);
    }
    g_message_recv_count++;


    // Returning IOTHUBMESSAGE_ACCEPTED causes the SDK to acknowledge receipt of the message to
    // the service.  The application does not need to take further action to ACK at this point.
    return IOTHUBMESSAGE_ACCEPTED;

}
static void send_confirm_callback(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* userContextCallback)
{
    (void)userContextCallback;
    // When a message is sent this callback will get invoked
   
    (void)printf("Confirmation callback received for message with result \r\n");
}
void receive(void* pvParameters) {
    while (1) {
        IoTHubDeviceClient_LL_DoWork(device_ll_handle);
        vTaskDelay(pdMS_TO_TICKS(1000)); // Adjust delay as needed
            g_continueRunning = false;
        
    }
}
void send_task(void* pvParameters) {
    while (1) {
        // IoTHubClient_LL_DoWork(handle);
        vTaskDelay(pdMS_TO_TICKS(1000)); // Adjust delay as needed
    }
}
static void connection_status_callback(IOTHUB_CLIENT_CONNECTION_STATUS result, IOTHUB_CLIENT_CONNECTION_STATUS_REASON reason, void* user_context)
{printf("2000\n");
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

void iothub_send(uint64_t serial_number) {
            // Construct message
            char msgText[256];
            sprintf(msgText, "{\"deviceId\":\"RFID\",\"RFID_tag\":%" PRIu64 "}", serial_number);
            printf("12\n");
            // Send message to IoT Hub
            // IOTHUB_MESSAGE_HANDLE messageHandle = IoTHubMessage_CreateFromString(msgText);
            printf("13\n");
            IOTHUB_MESSAGE_HANDLE message_handle;
           
            bool urlEncodeOn = true;
            (void)IoTHubDeviceClient_LL_SetOption(device_ll_handle, OPTION_AUTO_URL_ENCODE_DECODE, &urlEncodeOn);

            // Setting connection status callback to get indication of connection to iothub
            (void)IoTHubDeviceClient_LL_SetConnectionStatusCallback(device_ll_handle, connection_status_callback, NULL);
printf("14\n");
        
        
            // Construct the iothub message from a string or a byte array
            message_handle = IoTHubMessage_CreateFromString(msgText);
            // Add custom properties to message
            (void)IoTHubMessage_SetProperty(message_handle, "property_key", "property_value");
printf("15\n");
            (void)printf("Sending message  to IoTHub\r\n");
            IoTHubDeviceClient_LL_SendEventAsync(device_ll_handle, message_handle, send_confirm_callback, NULL);
printf("16\n");
            // The message is copied to the sdk so the we can destroy it
            IoTHubMessage_Destroy(message_handle);
            // IoTHubDeviceClient_LL_DoWork(device_ll_handle);
            ThreadAPI_Sleep(1);
printf("17\n");
        

        // Clean up the iothub sdk handle
        // IoTHubDeviceClient_LL_Destroy(device_ll_handle);
          
} 

static void rc522_handler(void* arg, esp_event_base_t base, int32_t event_id, void* event_data)
{
    printf("******** EXECUTING rc522_handler ********\r\n");
    rc522_event_data_t* data = (rc522_event_data_t*) event_data;

    switch(event_id) {
        // triggered when a tag is scanned by the RFID reader
        printf("11\n");
        case RC522_EVENT_TAG_SCANNED: {
                rc522_tag_t* tag = (rc522_tag_t*) data->ptr;
                ESP_LOGI(TAG, "Tag scanned (sn: %" PRIu64 ")", tag->serial_number);
                iothub_send(tag->serial_number);

            }
            break;
    }

    
}

void iothub_receive()
{
    printf("21\n");

    IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol;
    size_t messages_count = 0;

    
        printf("23\n");
        // Set any option that are necessary.
        // For available options please see the iothub_sdk_options.md documentation
        bool traceOn = true;
        IoTHubDeviceClient_LL_SetOption(device_ll_handle, OPTION_LOG_TRACE, &traceOn);
        printf("24\n");
        bool urlDecodeOn = true;
        (void)IoTHubDeviceClient_LL_SetOption(device_ll_handle, OPTION_AUTO_URL_ENCODE_DECODE, &urlDecodeOn);
        printf("25\n");
        if (IoTHubDeviceClient_LL_SetMessageCallback(device_ll_handle, receive_msg_callback, &messages_count) != IOTHUB_CLIENT_OK)
        {
                    printf("26\n");

            (void)printf("ERROR: IoTHubClient_LL_SetMessageCallback..........FAILED!\r\n");
        }
        else
        {
            (void)printf("Waiting for message to be sent to device (will quit after %d messages)\r\n", MESSAGE_COUNT);
            // while (1){
            //     // IoTHubDeviceClient_LL_DoWork(device_ll_handle);
            //     ThreadAPI_Sleep(10);
            // } 
        }
        printf("34\n");

        // Clean up the iothub sdk Sending message %d
        // IoTHubDeviceClient_LL_Destroy(device_ll_handle);
}

void app_main(){
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    printf("1\n");

    initialise_wifi();
    EventBits_t bits = xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT, pdFALSE, pdTRUE, portMAX_DELAY);
    if ((bits & CONNECTED_BIT) == 0) {
        printf("Failed to connect to WiFi. Aborting.\n");
        return;
    }
    printf("2\n");
    printf("WiFi connected!\n");
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    /*RFID Module*/
    // Initialize RC522
        rc522_config_t config = {
        .spi.host = VSPI_HOST,
        .spi.miso_gpio = 25,
        .spi.mosi_gpio = 23,
        .spi.sck_gpio = 19,
        .spi.sda_gpio = 22,
    };

    rc522_create(&config, &scanner);
    rc522_start(scanner);

    /*IoT Hub*/
    printf("3\n");
    // Used to initialize IoTHub SDK subsystem
    (void)IoTHub_Init();
    (void)printf("Creating IoTHub Device handle\r\n");

    // Create the iothub handle here
    device_ll_handle = IoTHubDeviceClient_LL_CreateFromConnectionString(connectionString, MQTT_Protocol);
    if (device_ll_handle == NULL)
    {
        printf("22\n");
        (void)printf("Failure creating IotHub device. Hint: Check your connection string.\r\n");
    }
    else
    {
        rc522_register_events(scanner, RC522_EVENT_ANY, rc522_handler, NULL);
        printf("7\n");
        iothub_receive();
vTaskDelay(2000 / portTICK_PERIOD_MS);
        xTaskCreate(receive, "listen_task", 4096, NULL, 6, NULL);
        
    }
        printf("36\n");
        // IoTHub_Deinit();
         xTaskCreate(send_task, "send_task", 4096, NULL, 5, NULL);
        printf("4\n");
        // vTaskDelay(2000 / portTICK_PERIOD_MS);
}