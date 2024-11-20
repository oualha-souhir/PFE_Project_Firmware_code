#ifndef RC522_HANDLER_H
#define RC522_HANDLER_H
#include <inttypes.h>
#include <stdint.h>
#include "esp_event.h"
#include "rc522.h"
#include "esp_log.h"
#include <string.h>
#include "iothub_send.h"

 void rc522_handler(void* arg, esp_event_base_t base, int32_t event_id, void* event_data);

#endif // RC522_HANDLER_H
