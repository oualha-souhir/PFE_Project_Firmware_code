
#ifndef IOTHUB_SEND_H
#define IOTHUB_SEND_H

#include <stdio.h>
#include "iothub_message.h"
#include "iothub_device_client_ll.h"
#include "azure_c_shared_utility/threadapi.h"
#include "iothub_client_options.h"

#include "iothub_client_ll.h"
#include "iothubtransportmqtt.h"
#include "iothub.h"


// Function prototypes
static void send_confirm_callback(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* userContextCallback);
void iothub_send(const char *msgText);

// External connection status callback function
extern void connection_status_callback(IOTHUB_CLIENT_CONNECTION_STATUS result, IOTHUB_CLIENT_CONNECTION_STATUS_REASON reason, void* user_context);


#endif // IOTHUB_SEND_H
