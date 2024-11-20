#include "iothub_send.h"
extern IOTHUB_DEVICE_CLIENT_LL_HANDLE device_ll_handle;
static void send_confirm_callback(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* userContextCallback)
{
    (void)userContextCallback;
    // When a message is sent this callback will get invoked
   
    // (void)printf("Confirmation callback received for message with result \r\n");
}

void iothub_send(const char *msgText) {

            
            // Send message to IoT Hub
            // IOTHUB_MESSAGE_HANDLE messageHandle = IoTHubMessage_CreateFromString(msgText);
            IOTHUB_MESSAGE_HANDLE message_handle;
           
            bool urlEncodeOn = true;
            (void)IoTHubDeviceClient_LL_SetOption(device_ll_handle, OPTION_AUTO_URL_ENCODE_DECODE, &urlEncodeOn);

            // Setting connection status callback to get indication of connection to iothub
            (void)IoTHubDeviceClient_LL_SetConnectionStatusCallback(device_ll_handle, connection_status_callback, NULL);
        
        
            // Construct the iothub message from a string or a byte array
            message_handle = IoTHubMessage_CreateFromString(msgText);
            // Add custom properties to message
            (void)IoTHubMessage_SetProperty(message_handle, "property_key", "property_value");
            (void)printf("Sending message  to IoTHub\r\n");
            IoTHubDeviceClient_LL_SendEventAsync(device_ll_handle, message_handle, send_confirm_callback, NULL);
            // The message is copied to the sdk so the we can destroy it
            IoTHubMessage_Destroy(message_handle);
            
            // IoTHubDeviceClient_LL_DoWork(device_ll_handle);
            ThreadAPI_Sleep(1);
        

        // Clean up the iothub sdk handle
        // IoTHubDeviceClient_LL_Destroy(device_ll_handle);
          
} 
