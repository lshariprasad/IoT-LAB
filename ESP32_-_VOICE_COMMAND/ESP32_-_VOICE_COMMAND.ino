#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


// ---------------- PIN DEFINITIONS ----------------
#define LED_PIN    2
#define RELAY_PIN  26   // NO mode relay

// ---------------- WIFI DETAILS -------------------
const char* ssid = "HARIDEVAN2006";
const char* password = "HARIDEVAN@1410";




// ---------------- OLED ---------------------------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// ---------------- SERVER -------------------------
WiFiServer server(80);

// ---------------- OLED FUNCTION ------------------
void showStatus(String l1, String l2) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("SMART CONTROL");
  display.println(l1);
  display.println(l2);
  display.display();
}

// ---------------- URL DECODER --------------------
String urlDecode(String input) {
  input.replace("+", " ");
  return input;
}

// ---------------- SETUP --------------------------
void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);

  // NO MODE: Motor OFF by default
  digitalWrite(RELAY_PIN, HIGH);   // relay OFF (LOW-level trigger relay)
  digitalWrite(LED_PIN, LOW);      // light OFF by default

  Wire.begin(21, 22);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  showStatus("WiFi", "Connecting...");

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  server.begin();

  Serial.print("OPEN THIS URL: http://");
  Serial.println(WiFi.localIP());

  showStatus("READY", WiFi.localIP().toString());
}

// ---------------- LOOP ---------------------------
void loop() {
  WiFiClient client = server.accept();
  if (!client) return;

  String req = client.readStringUntil('\r');
  client.clear();

  // -------- DIRECT BUTTON COMMANDS --------
  if (req.indexOf("GET /light/on") != -1) {
    digitalWrite(LED_PIN, HIGH);
    showStatus("LIGHT", "ON");
  }

  if (req.indexOf("GET /light/off") != -1) {
    digitalWrite(LED_PIN, LOW);
    showStatus("LIGHT", "OFF");
  }

  if (req.indexOf("GET /motor/on") != -1) {
    // Motor ON = Relay ON (NO mode)
    digitalWrite(RELAY_PIN, LOW);     // relay ON
    showStatus("MOTOR", "ON");
  }

  if (req.indexOf("GET /motor/off") != -1) {
    // Motor OFF = Relay OFF
    digitalWrite(RELAY_PIN, HIGH);    // relay OFF
    showStatus("MOTOR", "OFF");
  }

  // -------- TEXT / VOICE COMMAND FROM FORM --------
  if (req.indexOf("GET /?cmd=") != -1) {

    int start = req.indexOf("cmd=") + 4;
    int end = req.indexOf(" HTTP");

    if (end > start) {
      String cmd = req.substring(start, end);
      cmd = urlDecode(cmd);
      cmd.toLowerCase();

      Serial.println("CMD: " + cmd);

      if (cmd == "light on") {
        digitalWrite(LED_PIN, HIGH);
        showStatus("LIGHT", "ON");
      }
      else if (cmd == "light off") {
        digitalWrite(LED_PIN, LOW);
        showStatus("LIGHT", "OFF");
      }
      else if (cmd == "motor on" || cmd == "fan on") {
        digitalWrite(RELAY_PIN, LOW);   // relay ON
        showStatus("MOTOR", "ON");
      }
      else if (cmd == "motor off" || cmd == "fan off") {
        digitalWrite(RELAY_PIN, HIGH);  // relay OFF
        showStatus("MOTOR", "OFF");
      }
      else {
        showStatus("UNKNOWN CMD", cmd);
      }
    }
  }

  // -------- SIMPLE NORMAL WEB PAGE --------
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html\n");

  client.println(R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>ESP32 Smart Control</title>
<style>
body{font-family:Arial;text-align:center;background:#f2f2f2;}
h2{color:#333;}
button{width:200px;padding:12px;margin:8px;font-size:16px;}
input{width:200px;padding:10px;font-size:16px;}
</style>
</head>

<body>
<h2>ESP32 SMART CONTROL</h2>

<h3>Light Control</h3>
<a href="/light/on"><button>LIGHT ON</button></a><br>
<a href="/light/off"><button>LIGHT OFF</button></a><br>

<h3>Motor / Fan Control</h3>
<a href="/motor/on"><button>MOTOR ON</button></a><br>
<a href="/motor/off"><button>MOTOR OFF</button></a><br>

<h3>Text / Voice Command</h3>

<form action="/" method="get">
<input name="cmd" placeholder="Type or say: light on"><br><br>
<input type="submit" value="SEND COMMAND">
</form>

<p>On mobile: tap keyboard 🎤 and speak</p>

</body>
</html>
)rawliteral");

  client.stop();
}
  

