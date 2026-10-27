ESP32 AUTOMATIC WATER MIST SPRAYER — HOSTINGER / FILEZILLA PACKAGE
=================================================================

CONTENTS
--------
public/
  index.php       Responsive web dashboard
  style.css       Dashboard styling
  app.js          Browser-side dashboard logic
  .htaccess       Basic directory protection / security headers where supported
api/
  config.example.php  Example shared API token configuration
  state.php           Read current status
  command.php         Send a requested command/settings to ESP32
  report.php          ESP32 reports status here
  .htaccess           Prevent direct listing
esp32_firmware/
  esp32_mist_cloud.ino  ESP32 Arduino firmware that polls the hosting API
  config.example.h      Firmware configuration template

IMPORTANT ARCHITECTURE NOTE
---------------------------
The original ESP32-hosted web page only works on the same local Wi-Fi network.
A website uploaded through FileZilla to Hostinger is a separate server. For
remote dashboard control, this package uses a cloud relay pattern:

Browser dashboard -> PHP API on Hostinger -> ESP32 polls API -> relay/pump
ESP32 -> reports status back to PHP API -> dashboard refreshes status.

The ESP32 must have outbound internet access. The PHP hosting must support PHP
8.x and writable JSON state files. HTTPS is recommended and normally provided
by Hostinger SSL.

INSTALLATION
------------
1. In Hostinger, create a website/subdomain and enable HTTPS/SSL.
2. Using FileZilla, upload the CONTENTS of the "public" folder into public_html/
   (or the document root of the subdomain). Do not upload the entire package
   folder as the document root unless you adjust paths.
3. Upload the "api" folder beside public_html, OR upload it as public_html/api.
   Easiest option: upload api/ into public_html/api so URLs are:
      https://YOUR-DOMAIN.example/api/state.php
      https://YOUR-DOMAIN.example/api/command.php
      https://YOUR-DOMAIN.example/api/report.php
   If your hosting exposes the API folder publicly, its PHP scripts are intended
   to be called over HTTPS; do not place config.example.php online as config.php
   until you replace the example token with a strong random secret.
4. Copy api/config.example.php to api/config.php and set a long random token.
   Use the SAME token in esp32_firmware/config.example.h (copy to config.h).
   Never share the token publicly or commit it to a public repository.
5. If PHP cannot write files, create api/data/ and grant the minimum permissions
   needed by your hosting, then update the paths in PHP files if necessary.
6. Copy esp32_firmware/config.example.h to config.h, set Wi-Fi credentials,
   API base URL, and the same token. Open the .ino in Arduino IDE and upload it.
7. Visit https://YOUR-DOMAIN.example/ in a browser.

API PATHS
---------
GET  /api/state.php
POST /api/command.php
POST /api/report.php

COMMANDS
--------
{"action":"start"}       Start a manual spray cycle
{"action":"stop"}        Stop spraying immediately
{"action":"settings","interval":600,"duration":5,"timer":true}
                          Set interval in seconds, duration in seconds,
                          and automatic timer enabled/disabled.

SAFETY
------
- First test with the pump disconnected and confirm the relay OFF state.
- Relay modules may be active-LOW or active-HIGH; set RELAY_ON / RELAY_OFF
  correctly in the firmware.
- Do not power a pump from an ESP32 GPIO. Use a correctly rated external supply.
- For AC mains equipment, use a properly enclosed, correctly rated installation
  handled by a qualified person.
- Do not expose the dashboard/API without HTTPS and a strong token.
- This is a starter implementation. For public production deployment, add user
  login, rate limiting, audit logging, and stronger authorization.
