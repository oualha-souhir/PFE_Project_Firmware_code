#ifndef IOTHUB_RECEIVE_H
#define IOTHUB_RECEIVE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "iothub_message.h"
#include "iothub_device_client_ll.h"
#include "cJSON.h"
#include "azure_c_shared_utility/threadapi.h"
#include "iothub_client_options.h"
#include "azure_c_shared_utility/shared_util_options.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <driver/gpio.h>

// Function prototypes
static IOTHUBMESSAGE_DISPOSITION_RESULT receive_msg_callback(IOTHUB_MESSAGE_HANDLE message, void* user_context);
void iothub_receive();

// External task creation function
extern void charging_task(void *param);

// Constants
#define MESSAGE_COUNT 1
#define DC_RELAY_CONTACTOR 4

#endif // IOTHUB_RECEIVE_H
