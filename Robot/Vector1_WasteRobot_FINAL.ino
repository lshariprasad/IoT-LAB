// ================================================================
//  VECTOR1 SMART AI WASTE COLLECTOR ROBOT — FINAL PRODUCTION BUILD
//  One file. Upload and run. No config needed.
//
//  FEATURES:
//   - WiFi Access Point with customer-changeable SSID + Password
//   - Credentials saved to NVS flash (Preferences) — survives reboot
//   - Mobile-first dark dashboard — works on any phone / laptop
//   - Live sensor data via /status JSON (auto-refreshes every 1s)
//   - Non-blocking ultrasonic state machine (no connection timeouts)
//   - Smooth servo sweep for JCB arm
//   - OLED: title bar, dual sensor readout, alert bar
//   - Settings page: customer can update SSID/Password from browser
//
//  DEFAULT WiFi:  SSID: Vector1  |  Password: 87654321
//  Open browser:  http://192.168.4.1
// ================================================================

#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
#include <ESP32Servo.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ── OLED ───────────────────────────────────────────────────────
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT  64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// ── NVS (persistent credential storage) ────────────────────────
Preferences prefs;

// ── DEFAULT WiFi credentials (used only on first boot) ─────────
const char* DEFAULT_SSID = "Vector1";
const char* DEFAULT_PASS = "87654321";

// Runtime credentials (loaded from flash or defaults) ──────────
String apSSID;
String apPASS;

WebServer server(80);

// ── MOTOR PINS (L298N) ─────────────────────────────────────────
#define IN1 32   // Right motor A
#define IN2 33   // Right motor B
#define IN3 25   // Left  motor A
#define IN4 23   // Left  motor B

// ── SERVO PINS ─────────────────────────────────────────────────
#define SERVO_L 18
#define SERVO_R 19
const int ARM_UP   = 120;
const int ARM_DOWN =  20;
Servo leftServo, rightServo;

// ── ULTRASONIC PINS ────────────────────────────────────────────
#define FRONT_TRIG 27
#define FRONT_ECHO 26
#define REAR_TRIG  13
#define REAR_ECHO  14

// ── ALERTS ─────────────────────────────────────────────────────
#define BUZZER  15
#define RED_LED 16

// ── THRESHOLDS ─────────────────────────────────────────────────
const int WASTE_CM = 20;
const int REAR_CM  = 10;

// ── ROBOT STATE ────────────────────────────────────────────────
enum State { IDLE, FWD, BWD, LTURN, RTURN, BLOCKED };
State robotState = IDLE;

bool   wasteDetected = false;
bool   rearBlocked   = false;
bool   armUp_state   = false;
long   frontDist     = 999;
long   rearDist      = 999;
String statusMsg     = "Ready";

// ── TIMING ─────────────────────────────────────────────────────
unsigned long tFront = 0, tRear = 0, tOled = 0;
const long POLL_MS = 80;

// ================================================================
//  UTILITY: Read ultrasonic distance
// ================================================================
long readCM(int trig, int echo) {
  digitalWrite(trig, LOW);  delayMicroseconds(2);
  digitalWrite(trig, HIGH); delayMicroseconds(10);
  digitalWrite(trig, LOW);
  long d = pulseIn(echo, HIGH, 25000);
  return (d == 0) ? 999 : d * 0.034 / 2;
}

// ================================================================
//  MOTOR CONTROL
// ================================================================
void motorsOff() {
  digitalWrite(IN1,LOW); digitalWrite(IN2,LOW);
  digitalWrite(IN3,LOW); digitalWrite(IN4,LOW);
  robotState = IDLE; statusMsg = "Stopped";
}
void goForward() {
  digitalWrite(IN1,HIGH); digitalWrite(IN2,LOW);
  digitalWrite(IN3,HIGH); digitalWrite(IN4,LOW);
  robotState = FWD; statusMsg = "Forward";
}
void goBackward() {
  long d = readCM(REAR_TRIG, REAR_ECHO);
  if (d <= REAR_CM) {
    motorsOff(); digitalWrite(RED_LED, HIGH);
    rearBlocked = true; robotState = BLOCKED;
    statusMsg = "Rear Blocked!"; return;
  }
  digitalWrite(RED_LED, LOW); rearBlocked = false;
  digitalWrite(IN1,LOW);  digitalWrite(IN2,HIGH);
  digitalWrite(IN3,LOW);  digitalWrite(IN4,HIGH);
  robotState = BWD; statusMsg = "Backward";
}
void goLeft() {
  digitalWrite(IN1,HIGH); digitalWrite(IN2,LOW);
  digitalWrite(IN3,LOW);  digitalWrite(IN4,HIGH);
  robotState = LTURN; statusMsg = "Left";
}
void goRight() {
  digitalWrite(IN1,LOW);  digitalWrite(IN2,HIGH);
  digitalWrite(IN3,HIGH); digitalWrite(IN4,LOW);
  robotState = RTURN; statusMsg = "Right";
}

// ================================================================
//  SERVO — smooth sweep both together
// ================================================================
void moveArm(bool up) {
  int from = up ? ARM_DOWN : ARM_UP;
  int to   = up ? ARM_UP   : ARM_DOWN;
  int step = up ? 2 : -2;
  for (int a = from; up ? (a <= to) : (a >= to); a += step) {
    leftServo.write(a); rightServo.write(a); delay(10);
  }
  armUp_state = up;
  statusMsg = up ? "Arm Up" : "Arm Down";
}

// ================================================================
//  OLED — advanced layout
// ================================================================
void drawOLED() {
  display.clearDisplay();
  display.fillRect(0,0,128,11,SSD1306_WHITE);
  display.setTextColor(SSD1306_BLACK); display.setTextSize(1);
  display.setCursor(2,2); display.print("Vector1 Waste Robot");
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,14); display.print("IP: "); display.print(WiFi.softAPIP());
  display.setCursor(0,14+10); display.print(apSSID);
  display.drawLine(0,26,128,26,SSD1306_WHITE);
  display.setCursor(0,29); display.print("St: "); display.print(statusMsg);
  display.setCursor(0,40);
  display.print("F:"); display.print(frontDist); display.print("cm");
  display.setCursor(65,40);
  display.print("R:"); display.print(rearDist); display.print("cm");
  if (wasteDetected) {
    display.fillRect(0,52,128,12,SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
    display.setCursor(8,54); display.print("!! WASTE DETECTED !!");
    display.setTextColor(SSD1306_WHITE);
  } else if (rearBlocked) {
    display.fillRect(0,52,128,12,SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
    display.setCursor(8,54); display.print("!! REAR BLOCKED  !!");
    display.setTextColor(SSD1306_WHITE);
  } else {
    display.setCursor(0,54);
    display.print("Arm:"); display.print(armUp_state?"UP":"DOWN");
    display.print("  No Waste");
  }
  display.display();
}

// ================================================================
//  WEB DASHBOARD HTML  (complete mobile-first dark UI)
// ================================================================
String dashboardPage() {
  return String(F(R"HTML(
<!DOCTYPE html><html lang="en"><head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1,maximum-scale=1">
<title>Vector1 Control</title>
<style>
:root{--g:#00ff88;--r:#ff6b35;--b:#58a6ff;--bg:#0d1117;--card:#161b22;
      --brd:#30363d;--tx:#e6edf3;--mx:#8b949e;}
*{box-sizing:border-box;margin:0;padding:0;-webkit-tap-highlight-color:transparent;}
body{background:var(--bg);color:var(--tx);font-family:'Segoe UI',Arial,sans-serif;
     padding:12px;min-height:100vh;}
h1{text-align:center;font-size:18px;font-weight:700;color:var(--g);
   letter-spacing:2px;text-transform:uppercase;margin-bottom:2px;}
.sub{text-align:center;font-size:11px;color:var(--mx);margin-bottom:12px;}
#alert{display:none;text-align:center;padding:9px 12px;border-radius:8px;
       font-weight:700;font-size:13px;margin-bottom:10px;
       animation:blink 1s infinite;}
@keyframes blink{0%,100%{opacity:1}50%{opacity:.4}}
.alert-w{background:rgba(0,255,136,.09);border:1px solid var(--g);color:var(--g);}
.alert-r{background:rgba(255,107,53,.09);border:1px solid var(--r);color:var(--r);}
.sens{display:grid;grid-template-columns:1fr 1fr;gap:8px;margin-bottom:10px;}
.sc{background:var(--card);border:1px solid var(--brd);border-radius:10px;
    padding:10px 14px;text-align:center;}
.sl{font-size:10px;color:var(--mx);text-transform:uppercase;letter-spacing:1px;}
.sv{font-size:26px;font-weight:700;font-family:'Courier New',monospace;margin-top:3px;}
.sg{color:var(--g);} .sd{color:var(--r);}
.live{text-align:center;font-size:12px;background:var(--card);
      border:1px solid var(--brd);border-radius:8px;
      padding:7px 10px;margin-bottom:10px;color:var(--mx);}
.live span{color:var(--g);font-weight:600;}
.card{background:var(--card);border:1px solid var(--brd);
      border-radius:10px;padding:12px;margin-bottom:10px;}
.ct{font-size:10px;color:var(--mx);text-transform:uppercase;
    letter-spacing:1.5px;margin-bottom:10px;}
.dpad{display:grid;grid-template-columns:repeat(3,1fr);
      gap:7px;max-width:220px;margin:0 auto;}
.btn{display:flex;align-items:center;justify-content:center;
     height:58px;border:none;border-radius:9px;font-size:22px;
     font-weight:700;cursor:pointer;text-decoration:none;
     transition:transform .1s,opacity .15s;user-select:none;}
.btn:active{transform:scale(.93);}
.bf{background:#182d18;color:var(--g);border:1px solid #204020;}
.bb{background:#182d18;color:var(--g);border:1px solid #204020;}
.bl{background:#181e2d;color:var(--b);border:1px solid #1e284a;}
.br{background:#181e2d;color:var(--b);border:1px solid #1e284a;}
.bs{background:#2d1818;color:var(--r);border:1px solid #4a1e1e;font-size:14px;}
.bx{visibility:hidden;}
.arm{display:grid;grid-template-columns:1fr 1fr;gap:8px;}
.ab{display:flex;align-items:center;justify-content:center;gap:6px;
    height:50px;border:none;border-radius:9px;font-size:13px;
    font-weight:700;cursor:pointer;text-decoration:none;
    transition:transform .1s;}
.ab:active{transform:scale(.95);}
.au{background:#182d1e;color:var(--g);border:1px solid #204a2a;}
.ad{background:#2d241a;color:#e3a227;border:1px solid #4a381e;}
.nav{display:flex;justify-content:center;margin-top:4px;}
.nl{font-size:12px;color:var(--b);cursor:pointer;
    background:none;border:none;text-decoration:underline;padding:4px 8px;}
footer{text-align:center;font-size:10px;color:var(--mx);margin-top:10px;}
</style></head><body>
<h1>🤖 Vector1</h1>
<p class="sub">Smart Waste Collector &bull; 192.168.4.1</p>
<div id="alert"></div>
<div class="sens">
  <div class="sc"><div class="sl">Front</div><div class="sv sg" id="fv">--</div></div>
  <div class="sc"><div class="sl">Rear</div><div class="sv sg" id="rv">--</div></div>
</div>
<div class="live">Status: <span id="st">Loading...</span></div>
<div class="card">
  <div class="ct">▲ Movement</div>
  <div class="dpad">
    <div class="bx"></div>
    <a class="btn bf" href="/fwd">↑</a>
    <div class="bx"></div>
    <a class="btn bl" href="/left">←</a>
    <a class="btn bs" href="/stop">STOP</a>
    <a class="btn br" href="/right">→</a>
    <div class="bx"></div>
    <a class="btn bb" href="/bwd">↓</a>
    <div class="bx"></div>
  </div>
</div>
<div class="card">
  <div class="ct">🦴 JCB Arm</div>
  <div class="arm">
    <a class="ab au" href="/armup">⬆ ARM UP</a>
    <a class="ab ad" href="/armdown">⬇ ARM DOWN</a>
  </div>
</div>
<div class="nav">
  <button class="nl" onclick="location.href='/settings'">⚙ WiFi Settings</button>
</div>
<footer>Vector1 Waste Robot &bull; Auto-refresh 1s</footer>
<script>
function poll(){
  fetch('/status').then(r=>r.json()).then(d=>{
    var fv=document.getElementById('fv'),rv=document.getElementById('rv'),
        al=document.getElementById('alert'),st=document.getElementById('st');
    fv.textContent=d.front+' cm'; rv.textContent=d.rear+' cm';
    fv.className='sv '+(d.waste?'sd':'sg');
    rv.className='sv '+(d.blocked?'sd':'sg');
    st.textContent=d.status;
    if(d.waste){al.textContent='⚠ WASTE DETECTED — Collect now!';
      al.className='alert-w';al.style.display='block';}
    else if(d.blocked){al.textContent='🛑 REAR BLOCKED — Cannot reverse!';
      al.className='alert-r';al.style.display='block';}
    else al.style.display='none';
  }).catch(()=>{});
}
poll(); setInterval(poll,1000);
</script></body></html>
)HTML"));
}

// ================================================================
//  SETTINGS PAGE — customer changes WiFi SSID + Password
// ================================================================
String settingsPage(String msg = "") {
  String html = F(R"HTML(
<!DOCTYPE html><html lang="en"><head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>WiFi Settings</title>
<style>
*{box-sizing:border-box;margin:0;padding:0;}
body{background:#0d1117;color:#e6edf3;font-family:'Segoe UI',Arial,sans-serif;
     padding:16px;min-height:100vh;}
.wrap{max-width:360px;margin:0 auto;}
h2{color:#58a6ff;font-size:16px;text-transform:uppercase;
   letter-spacing:2px;margin-bottom:4px;}
.sub{font-size:11px;color:#8b949e;margin-bottom:20px;}
.card{background:#161b22;border:1px solid #30363d;border-radius:10px;padding:16px;}
label{display:block;font-size:11px;color:#8b949e;text-transform:uppercase;
      letter-spacing:1px;margin-bottom:6px;}
input[type=text],input[type=password]{
  width:100%;background:#0d1117;border:1px solid #30363d;
  border-radius:7px;padding:10px 12px;color:#e6edf3;
  font-size:14px;margin-bottom:14px;outline:none;}
input:focus{border-color:#58a6ff;}
.note{font-size:11px;color:#8b949e;margin-bottom:14px;line-height:1.5;}
.btn-save{width:100%;padding:12px;background:#182d18;
  color:#00ff88;border:1px solid #204020;border-radius:8px;
  font-size:14px;font-weight:700;cursor:pointer;}
.btn-save:active{transform:scale(.97);}
.msg{padding:8px 12px;border-radius:7px;font-size:13px;
     font-weight:600;text-align:center;margin-bottom:14px;}
.msg-ok{background:rgba(0,255,136,.1);border:1px solid #00ff88;color:#00ff88;}
.msg-er{background:rgba(255,107,53,.1);border:1px solid #ff6b35;color:#ff6b35;}
.back{display:block;text-align:center;margin-top:12px;font-size:12px;
      color:#58a6ff;text-decoration:none;}
</style></head><body><div class="wrap">
<h2>⚙ WiFi Settings</h2>
<p class="sub">Change robot hotspot name and password</p>
)HTML");
  if (msg.length() > 0) html += msg;
  html += "<div class='card'>";
  html += "<form method='POST' action='/savewifi'>";
  html += "<label>Hotspot Name (SSID)</label>";
  html += "<input type='text' name='ssid' placeholder='e.g. MyRobot' maxlength='31' value='" + apSSID + "'>";
  html += "<label>Password (min 8 chars)</label>";
  html += "<input type='password' name='pass' placeholder='min 8 characters' maxlength='63'>";
  html += "<p class='note'>⚠ After saving, robot restarts with the new name. Reconnect to the new WiFi hotspot, then open http://192.168.4.1</p>";
  html += "<button type='submit' class='btn-save'>Save & Restart</button>";
  html += "</form></div>";
  html += "<a class='back' href='/'>← Back to Control</a>";
  html += "</div></body></html>";
  return html;
}

// ================================================================
//  SETUP
// ================================================================
void setup() {
  Serial.begin(115200);

  // Load saved credentials from NVS (or use defaults first boot)
  prefs.begin("wifi", false);
  apSSID = prefs.getString("ssid", DEFAULT_SSID);
  apPASS = prefs.getString("pass", DEFAULT_PASS);
  prefs.end();
  Serial.println("SSID: " + apSSID);

  // Motor pins
  pinMode(IN1,OUTPUT); pinMode(IN2,OUTPUT);
  pinMode(IN3,OUTPUT); pinMode(IN4,OUTPUT);
  motorsOff();

  // Ultrasonic pins
  pinMode(FRONT_TRIG,OUTPUT); pinMode(FRONT_ECHO,INPUT);
  pinMode(REAR_TRIG, OUTPUT); pinMode(REAR_ECHO, INPUT);

  // Alert pins
  pinMode(BUZZER, OUTPUT);  digitalWrite(BUZZER,  LOW);
  pinMode(RED_LED,OUTPUT);  digitalWrite(RED_LED, LOW);

  // Servos
  leftServo.attach(SERVO_L);  rightServo.attach(SERVO_R);
  moveArm(false);

  // OLED
  Wire.begin(21, 22);
  if (display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    display.clearDisplay(); display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1); display.setCursor(18,20);
    display.print("Vector1 Starting..."); display.display();
    delay(1000);
  }

  // Start WiFi AP
  WiFi.softAP(apSSID.c_str(), apPASS.c_str());
  Serial.print("AP IP: "); Serial.println(WiFi.softAPIP());

  // ── HTTP ROUTES ───────────────────────────────────────────────
  server.on("/", HTTP_GET, [](){
    server.send(200, "text/html", dashboardPage());
  });
  server.on("/fwd", HTTP_GET, [](){
    goForward();
    server.sendHeader("Location","/"); server.send(303);
  });
  server.on("/bwd", HTTP_GET, [](){
    goBackward();
    server.sendHeader("Location","/"); server.send(303);
  });
  server.on("/left", HTTP_GET, [](){
    goLeft();
    server.sendHeader("Location","/"); server.send(303);
  });
  server.on("/right", HTTP_GET, [](){
    goRight();
    server.sendHeader("Location","/"); server.send(303);
  });
  server.on("/stop", HTTP_GET, [](){
    motorsOff();
    server.sendHeader("Location","/"); server.send(303);
  });
  server.on("/armup", HTTP_GET, [](){
    moveArm(true);
    server.sendHeader("Location","/"); server.send(303);
  });
  server.on("/armdown", HTTP_GET, [](){
    moveArm(false);
    server.sendHeader("Location","/"); server.send(303);
  });

  // Live JSON status
  server.on("/status", HTTP_GET, [](){
    String j = "{";
    j += "\"front\":"  + String(frontDist) + ",";
    j += "\"rear\":"   + String(rearDist)  + ",";
    j += "\"waste\":"  + (wasteDetected ? "true" : "false") + ",";
    j += "\"blocked\":"+ (rearBlocked   ? "true" : "false") + ",";
    j += "\"arm\":"    + (armUp_state    ? "true" : "false") + ",";
    j += "\"status\":\""+ statusMsg + "\"";
    j += "}";
    server.send(200, "application/json", j);
  });

  // Settings page GET
  server.on("/settings", HTTP_GET, [](){
    server.send(200, "text/html", settingsPage());
  });

  // Settings save POST — validates, saves to NVS, restarts
  server.on("/savewifi", HTTP_POST, [](){
    String newSSID = server.arg("ssid");
    String newPASS = server.arg("pass");
    newSSID.trim(); newPASS.trim();

    if (newSSID.length() < 1 || newSSID.length() > 31) {
      server.send(200, "text/html",
        settingsPage("<div class='msg msg-er'>SSID must be 1–31 characters.</div>"));
      return;
    }
    if (newPASS.length() < 8 || newPASS.length() > 63) {
      server.send(200, "text/html",
        settingsPage("<div class='msg msg-er'>Password must be 8–63 characters.</div>"));
      return;
    }

    // Save to NVS flash
    prefs.begin("wifi", false);
    prefs.putString("ssid", newSSID);
    prefs.putString("pass", newPASS);
    prefs.end();

    // Confirm page then restart
    String ok = "<!DOCTYPE html><html><head>";
    ok += "<meta charset='UTF-8'>";
    ok += "<meta name='viewport' content='width=device-width,initial-scale=1'>";
    ok += "<style>body{background:#0d1117;color:#e6edf3;";
    ok += "font-family:Segoe UI,Arial,sans-serif;padding:24px;text-align:center;}";
    ok += "h2{color:#00ff88;margin-bottom:12px;} p{color:#8b949e;font-size:13px;";
    ok += "line-height:1.7;}</style></head><body>";
    ok += "<h2>✅ Saved! Restarting...</h2>";
    ok += "<p>New hotspot: <strong style='color:#58a6ff'>" + newSSID + "</strong><br>";
    ok += "Connect your phone/laptop to that WiFi,<br>then open <strong>http://192.168.4.1</strong></p>";
    ok += "</body></html>";
    server.send(200, "text/html", ok);

    delay(1800);
    ESP.restart();
  });

  // 404 catch-all — redirect to home
  server.onNotFound([](){
    server.sendHeader("Location", "/");
    server.send(303);
  });

  server.begin();
  statusMsg = "AP Ready";
  drawOLED();
  Serial.println("Server up — http://192.168.4.1");
}

// ================================================================
//  LOOP — non-blocking sensor polling
// ================================================================
void loop() {
  server.handleClient();
  unsigned long now = millis();

  // Front sensor poll
  if (now - tFront >= POLL_MS) {
    tFront = now;
    frontDist = readCM(FRONT_TRIG, FRONT_ECHO);
    if (frontDist <= WASTE_CM) {
      wasteDetected = true;  digitalWrite(BUZZER, HIGH);
    } else {
      wasteDetected = false; digitalWrite(BUZZER, LOW);
    }
  }

  // Rear sensor poll
  if (now - tRear >= POLL_MS + 40) {
    tRear = now;
    rearDist = readCM(REAR_TRIG, REAR_ECHO);
    if (rearDist <= REAR_CM) {
      rearBlocked = true; digitalWrite(RED_LED, HIGH);
      if (robotState == BWD) {
        motorsOff(); statusMsg = "Rear Blocked!";
      }
    } else {
      rearBlocked = false;
      if (robotState != BLOCKED) digitalWrite(RED_LED, LOW);
    }
  }

  // OLED update every 200ms
  if (now - tOled >= 200) {
    tOled = now;
    drawOLED();
  }
}
// ================================================================
//  END OF FILE — Vector1_WasteRobot_FINAL.ino
// ================================================================
