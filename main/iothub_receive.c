#include "iothub_receive.h"
extern IOTHUB_DEVICE_CLIENT_LL_HANDLE device_ll_handle;
int g_message_recv_count;
double balance = 0.0;
int initialized  = 0;
bool web;
extern int i;
extern uint64_t rfid;
extern float n;
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
                            cJSON *web_req = cJSON_GetObjectItemCaseSensitive(item, "start");
                            cJSON *balance_web = cJSON_GetObjectItemCaseSensitive(item, "balance");

                             
                            if (cJSON_IsString(rfid_tag) && (rfid_tag->valuestring != NULL)) {
                                const char *rfid_tag_value = rfid_tag->valuestring;
                                if (strcmp(rfid_tag_value, "0") == 0) {
                                    printf("Not a client\n");
                                } else {
                                    printf("RFID_tag: %s\n", rfid_tag_value);
                                }
                            }
                            
                            if (cJSON_IsNumber(account_balance)) {
                                balance = account_balance->valuedouble;
                                if (balance == 0) {
                                    printf("RFID_tag: %lld\n", rfid);
                                    printf("you don't have enough balance in your account\n");
                                } else {
                                    //rfid scan && client has balance
                                   printf("RFID_tag: %lld\n", rfid);
                                    printf("your account balance is: %f\n", balance);
                                        // int balance = 20;
                                        n = 1.3;
                                        initialized = 1;
                                        web = false;
                                        i = 0;
                                        xTaskCreate(charging_task, "Charging Task", 4096, NULL, 7, NULL);

                                        // 
                                        // 
                                        // initialized = 0;
                                        // balance = 0.0;
                                        // printf("Charging session ended\n");
                                        // send stopped to iothub and new balance                            
                                }
                            }

                           if (cJSON_IsNumber(web_req)) {
                                int start = web_req->valueint;
                                if (start == 1) {
                                    n = 1.3;
                                    balance = balance_web->valuedouble;
                                     printf("Balance: %f\n", balance_web->valuedouble);
                                    web = true;
                                    initialized = 1;
                                    i = 0;
                                    xTaskCreate(charging_task, "Charging Task", 4096, NULL, 7, NULL);

                                    // while (balance > 0)
                                    // {
                                    //     strcpy(msg, start_charging(balance));
                                    //     iothub_send(msg);
                                    //     vTaskDelay(1000 / portTICK_PERIOD_MS); // Delay for stability

                                    // }
                                } else if (start == 0) {
                                    printf("Charging session ended\n");
                                    gpio_set_level(DC_RELAY_CONTACTOR, 0);
                                    initialized = 0;
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



void iothub_receive()
{

    IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol;
    size_t messages_count = 0;

    
        // Set any option that are necessary.
        // For available options please see the iothub_sdk_options.md documentation
        bool traceOn = true;
        IoTHubDeviceClient_LL_SetOption(device_ll_handle, OPTION_LOG_TRACE, &traceOn);
        bool urlDecodeOn = true;
        (void)IoTHubDeviceClient_LL_SetOption(device_ll_handle, OPTION_AUTO_URL_ENCODE_DECODE, &urlDecodeOn);
        if (IoTHubDeviceClient_LL_SetMessageCallback(device_ll_handle, receive_msg_callback, &messages_count) != IOTHUB_CLIENT_OK)
        {

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

        // Clean up the iothub sdk Sending message %d
        // IoTHubDeviceClient_LL_Destroy(device_ll_handle);
}

