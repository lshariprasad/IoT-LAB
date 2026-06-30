/*
  ESP8266 WiFi Range Extender (Repeater) with NAT
  -------------------------------------------------
  This connects to your existing WiFi (e.g., mobile hotspot)
  as a STATION, and creates a NEW WiFi network (AP) that
  rebroadcasts/extends that internet connection using NAT
  (Network Address Translation), so devices connecting to the
  ESP8266's AP get real internet access through your mobile hotspot.

  IMPORTANT - BOARD SETTINGS (Arduino IDE):
  1. Install "esp8266" board package (version 2.5.0 or higher) via
     Boards Manager.
  2. Tools > lwIP Variant: select a variant that supports IPv4 NAPT.
     Look for "v2 Higher Bandwidth" or any option explicitly listing
     "IPv4 NAPT" support (varies by core version). If NAPT isn't
     available in your core version, update the esp8266 board package.
  3. Tools > Flash Size: at least 4MB recommended.
  4. Select your correct ESP8266 board (NodeMCU 1.0, Wemos D1 Mini, etc.)

  Replace ssid_home / password_home with YOUR mobile hotspot's
  credentials below.
*/

#include <ESP8266WiFi.h>
extern "C" {
  #include "lwip/napt.h"
  #include "lwip/dns.h"
}
#include <LwipDhcpServer.h>

#define NAPT 1000      // max NAT entries
#define NAPT_PORT 10   // max port forwarding entries

// ---- Your MOBILE HOTSPOT credentials (the network being extended) ----
const char* ssid_home     = "YOUR_MOBILE_HOTSPOT_NAME";
const char* password_home = "YOUR_MOBILE_HOTSPOT_PASSWORD";

// ---- New extended WiFi network created by the ESP8266 ----
const char* ssid_ap     = "ESP8266_Extender";
const char* password_ap = "12345678";   // min 8 characters

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\nStarting ESP8266 WiFi Extender...");

  WiFi.mode(WIFI_AP_STA);

  // 1) Connect to your mobile hotspot as a client (STA)
  WiFi.begin(ssid_home, password_home);
  Serial.print("Connecting to home WiFi");
  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 40) {
    delay(500);
    Serial.print(".");
    retries++;
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nFailed to connect to home WiFi. Check credentials and restart.");
    return;
  }

  Serial.println("\nConnected to home WiFi!");
  Serial.print("STA IP address: ");
  Serial.println(WiFi.localIP());

  // 2) Start the new extended Access Point
  IPAddress apIP(192, 168, 4, 1);
  IPAddress apGateway(192, 168, 4, 1);
  IPAddress apSubnet(255, 255, 255, 0);
  WiFi.softAPConfig(apIP, apGateway, apSubnet);
  WiFi.softAP(ssid_ap, password_ap);

  Serial.print("Extender AP IP address: ");
  Serial.println(WiFi.softAPIP());
  Serial.print("Connect your devices to WiFi network: ");
  Serial.println(ssid_ap);

  // 3) Enable NAT so AP-side devices get real internet access
  err_t ret = ip_napt_init(NAPT, NAPT_PORT);
  Serial.printf("ip_napt_init result: %d (0 = OK)\n", ret);

  if (ret == ERR_OK) {
    ret = ip_napt_enable_no(SOFTAP_IF, 1);
    Serial.printf("ip_napt_enable_no result: %d (0 = OK)\n", ret);
  }

  if (ret == ERR_OK) {
    Serial.println("NAT enabled. WiFi extender is LIVE!");
  } else {
    Serial.println("NAT failed to enable. Check your lwIP variant settings.");
  }

  // 4) Forward DNS so connected devices can resolve websites
  dhcpSoftAP.dns_setserver(0, WiFi.dnsIP());
}

void loop() {
  // Auto-reconnect if connection to home WiFi drops
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Lost connection to home WiFi. Reconnecting...");
    WiFi.reconnect();
    delay(5000);
  }
  delay(1000);
}
