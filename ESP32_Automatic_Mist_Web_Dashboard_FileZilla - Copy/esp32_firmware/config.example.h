// Copy this file to config.h and fill in your details.
// Do not publish config.h: it contains Wi-Fi credentials and API token.
#pragma once

#define WIFI_SSID "YOUR_2_4_GHZ_WIFI_NAME"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

// Include trailing slash, use your real HTTPS domain.
#define API_BASE_URL "https://YOUR-DOMAIN.example/api/"

// Must exactly match API_TOKEN in api/config.php.
#define API_TOKEN "REPLACE_WITH_THE_SAME_LONG_RANDOM_SECRET"

// Relay module logic varies. Common 1-channel modules are active LOW.
#define RELAY_PIN 26
#define RELAY_ON LOW
#define RELAY_OFF HIGH
