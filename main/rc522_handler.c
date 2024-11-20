#include "rc522_handler.h"

extern rc522_handle_t scanner;
rc522_event_data_t event_data;
extern const char *TAG;
uint64_t rfid;
extern char msgText[256];
 void rc522_handler(void* arg, esp_event_base_t base, int32_t event_id, void* event_data)
{
    printf("******** EXECUTING rc522_handler ********\r\n");
    rc522_event_data_t* data = (rc522_event_data_t*) event_data;

    switch(event_id) {
        // triggered when a tag is scanned by the RFID reader
        case RC522_EVENT_TAG_SCANNED: {
                rc522_tag_t* tag = (rc522_tag_t*) data->ptr;
                ESP_LOGI(TAG, "Tag scanned (sn: %" PRIu64 ")", tag->serial_number);
                rfid = tag->serial_number; 
                sprintf(msgText, "{\"deviceId\":\"RFID\",\"RFID_tag\":%" PRIu64 "}",tag->serial_number);
                iothub_send(msgText);

            }
            break;
    }

    
}

