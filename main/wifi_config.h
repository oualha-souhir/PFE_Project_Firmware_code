#ifndef WIFI_H
#define WIFI_H

#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "freertos/event_groups.h"

#define EXAMPLE_WIFI_SSID "Orange"
#define EXAMPLE_WIFI_PASS "20400424"


#define CONNECTED_BIT BIT0
#ifndef BIT0
#define BIT0 (0x1 << 0)
#endif

void initialise_wifi(void);

#endif // WIFI_H
