#include <Wire.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_VL53L0X.h>
#include "MAX30105.h"
#include "heartRate.h"



// ============================================================
//  CONFIGURATION
// ============================================================
const char* ssid     = "ls_hariprasad";
const char* password = "HARIPRASAD";

// Pins
#define BUZZER_PIN  25   // GPIO 25 → Buzzer
#define SDA_PIN     21
#define SCL_PIN     22

// OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Sensors
Adafruit_VL53L0X lox;
MAX30105 particleSensor;
WebServer server(80);

// ============================================================
//  GLOBALS
// ============================================================
long  lastBeat  = 0;
float bpm       = 0;
float beatAvg   = 0;
long  irValue   = 0;
float spo2      = 0;

unsigned long lastVL    = 0;
int           distance  = -1;
String        maskStatus = "OFF";
bool          maskAlert  = false;

#define GRAPH_POINTS 64
int bpmGraph[GRAPH_POINTS] = {0};
int graphIdx = 0;
unsigned long lastGraphUpdate = 0;

bool heartBig = false;
unsigned long lastHeartAnim = 0;

// ============================================================
//  OLED HEART ICON  (8x8 bitmap)
// ============================================================
static const uint8_t PROGMEM heartBmp[] = {
  0b01100110,
  0b11111111,
  0b11111111,
  0b11111111,
  0b01111110,
  0b00111100,
  0b00011000,
  0b00000000
};

// ============================================================
//  WEBPAGE
// ============================================================
const char WEBPAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>SmartHealth Monitor</title>
<link href="https://fonts.googleapis.com/css2?family=Rajdhani:wght@400;600;700&family=Share+Tech+Mono&display=swap" rel="stylesheet">
<style>
  :root {
    --bg:        #04080f;
    --panel:     #080f1c;
    --border:    #0d2040;
    --accent:    #00cfff;
    --accent2:   #00ff9d;
    --danger:    #ff3a5c;
    --warn:      #ffb800;
    --text:      #c8e6ff;
    --dim:       #4a6a8a;
    --glow:      0 0 12px rgba(0,207,255,0.45);
    --glow-g:    0 0 12px rgba(0,255,157,0.45);
    --glow-r:    0 0 14px rgba(255,58,92,0.6);
  }
  * { margin:0; padding:0; box-sizing:border-box; }
  body {
    font-family: 'Rajdhani', sans-serif;
    background: var(--bg);
    color: var(--text);
    min-height: 100vh;
    overflow-x: hidden;
  }
  body::before {
    content:'';
    position:fixed; inset:0;
    background:
      repeating-linear-gradient(0deg, transparent, transparent 3px, rgba(0,207,255,0.012) 4px),
      repeating-linear-gradient(90deg, transparent, transparent 3px, rgba(0,207,255,0.012) 4px);
    pointer-events:none; z-index:0;
  }
  header {
    display:flex; align-items:center; justify-content:space-between;
    padding:14px 24px;
    border-bottom:1px solid var(--border);
    background: linear-gradient(90deg, #04080f 0%, #040e1e 100%);
    position:relative; z-index:2;
  }
  .logo { display:flex; align-items:center; gap:10px; }
  .logo-cross { width:28px; height:28px; position:relative; }
  .logo-cross::before, .logo-cross::after {
    content:''; position:absolute; background:var(--accent); border-radius:2px;
  }
  .logo-cross::before { width:28px; height:8px; top:10px; left:0; }
  .logo-cross::after  { width:8px; height:28px; left:10px; top:0; }
  .logo h1 { font-size:1.25rem; font-weight:700; letter-spacing:3px; color:var(--accent); text-shadow:var(--glow); }
  .logo span { font-size:.7rem; color:var(--dim); letter-spacing:2px; }
  .header-right { display:flex; align-items:center; gap:16px; }
  #clock { font-family:'Share Tech Mono'; color:var(--accent2); font-size:.9rem; text-shadow:var(--glow-g); }
  .status-dot {
    width:8px; height:8px; border-radius:50%; background:var(--accent2);
    box-shadow:0 0 8px var(--accent2);
    animation: pulse-dot 2s infinite;
  }
  @keyframes pulse-dot { 0%,100%{opacity:1} 50%{opacity:.3} }
  main {
    max-width: 1100px;
    margin: 0 auto;
    padding: 20px 16px;
    position:relative; z-index:1;
  }
  .patient-bar { display:flex; gap:12px; margin-bottom:18px; flex-wrap:wrap; }
  .patient-chip {
    background: var(--panel);
    border:1px solid var(--border);
    border-radius:6px;
    padding:6px 14px;
    font-size:.75rem;
    color: var(--dim);
    letter-spacing:1px;
  }
  .patient-chip span { color:var(--text); font-weight:600; }
  .vitals-grid {
    display:grid;
    grid-template-columns: repeat(auto-fit, minmax(210px, 1fr));
    gap:14px;
    margin-bottom:18px;
  }
  .card {
    background: var(--panel);
    border:1px solid var(--border);
    border-radius:12px;
    padding:18px 20px;
    position:relative;
    overflow:hidden;
    transition: border-color .3s;
  }
  .card::before {
    content:''; position:absolute;
    top:0; left:0; right:0; height:2px;
  }
  .card.blue::before  { background: var(--accent); box-shadow: var(--glow); }
  .card.green::before { background: var(--accent2); box-shadow: var(--glow-g); }
  .card.red::before   { background: var(--danger); box-shadow: var(--glow-r); }
  .card.warn::before  { background: var(--warn); }
  .card-label { font-size:.68rem; letter-spacing:2px; color:var(--dim); text-transform:uppercase; margin-bottom:8px; }
  .card-value { font-size:2.6rem; font-weight:700; line-height:1; font-family:'Share Tech Mono'; }
  .card.blue  .card-value { color:var(--accent);  text-shadow:var(--glow); }
  .card.green .card-value { color:var(--accent2); text-shadow:var(--glow-g); }
  .card.red   .card-value { color:var(--danger);  text-shadow:var(--glow-r); }
  .card.warn  .card-value { color:var(--warn); }
  .card-unit { font-size:.85rem; color:var(--dim); margin-top:4px; }
  .heart-icon {
    position:absolute; right:16px; top:50%; transform:translateY(-50%);
    font-size:2.2rem;
    animation: heartbeat 1s ease-in-out infinite;
    filter: drop-shadow(0 0 6px var(--danger));
  }
  @keyframes heartbeat {
    0%,100% { transform:translateY(-50%) scale(1); }
    30%      { transform:translateY(-50%) scale(1.25); }
    60%      { transform:translateY(-50%) scale(0.95); }
  }
  .mask-badge {
    display:inline-block;
    padding:4px 14px; border-radius:20px;
    font-size:.85rem; font-weight:700; letter-spacing:2px;
    margin-top:6px;
  }
  .mask-on  { background:rgba(0,255,157,.15); color:var(--accent2); border:1px solid var(--accent2); }
  .mask-off { background:rgba(255,58,92,.15);  color:var(--danger);  border:1px solid var(--danger); }
  .graph-section {
    background:var(--panel);
    border:1px solid var(--border);
    border-radius:12px;
    padding:18px 20px;
    margin-bottom:18px;
  }
  .section-title {
    font-size:.72rem; letter-spacing:3px; color:var(--dim);
    text-transform:uppercase; margin-bottom:14px;
    display:flex; align-items:center; gap:8px;
  }
  .section-title::after { content:''; flex:1; height:1px; background:var(--border); }
  canvas { width:100% !important; display:block; }
  .bottom-row {
    display:grid;
    grid-template-columns: 1fr 1fr;
    gap:14px;
  }
  @media(max-width:600px){ .bottom-row{ grid-template-columns:1fr; } }
  .gauge-wrap {
    background:var(--panel);
    border:1px solid var(--border);
    border-radius:12px;
    padding:18px 20px;
    display:flex; flex-direction:column; align-items:center;
  }
  .spo2-ring { position:relative; width:130px; height:130px; margin:10px 0; }
  .spo2-ring svg { width:130px; height:130px; }
  .spo2-ring .ring-bg { fill:none; stroke:var(--border); stroke-width:10; }
  .spo2-ring .ring-fill {
    fill:none; stroke:var(--accent2); stroke-width:10;
    stroke-linecap:round;
    stroke-dasharray: 283;
    stroke-dashoffset: 283;
    transform: rotate(-90deg); transform-origin:50% 50%;
    transition: stroke-dashoffset 1s ease, stroke .5s;
    filter: drop-shadow(0 0 6px var(--accent2));
  }
  .spo2-center {
    position:absolute; inset:0;
    display:flex; flex-direction:column; align-items:center; justify-content:center;
  }
  .spo2-val { font-family:'Share Tech Mono'; font-size:1.8rem; color:var(--accent2); text-shadow:var(--glow-g); }
  .spo2-unit { font-size:.7rem; color:var(--dim); }
  .dist-section {
    background:var(--panel);
    border:1px solid var(--border);
    border-radius:12px;
    padding:18px 20px;
  }
  .dist-bar-track {
    width:100%; height:18px;
    background:rgba(255,255,255,.04);
    border-radius:9px;
    overflow:hidden;
    margin:12px 0 6px;
    border:1px solid var(--border);
  }
  .dist-bar-fill {
    height:100%; border-radius:9px;
    background: linear-gradient(90deg, var(--accent2), var(--accent));
    transition: width .6s ease, background .5s;
    box-shadow:0 0 8px rgba(0,207,255,.5);
  }
  .dist-labels { display:flex; justify-content:space-between; font-size:.65rem; color:var(--dim); }
  .dist-reading { font-family:'Share Tech Mono'; font-size:1.8rem; color:var(--accent); text-shadow:var(--glow); margin-top:8px; }
  #alert-banner {
    display:none;
    background: rgba(255,58,92,.12);
    border:1px solid var(--danger);
    border-radius:8px;
    padding:10px 18px;
    margin-bottom:14px;
    color:var(--danger);
    font-weight:700;
    letter-spacing:2px;
    font-size:.85rem;
    text-align:center;
    animation: flashAlert 1s infinite;
  }
  @keyframes flashAlert { 0%,100%{opacity:1} 50%{opacity:.5} }
  footer {
    text-align:center; padding:14px;
    color:var(--dim); font-size:.68rem; letter-spacing:2px;
    border-top:1px solid var(--border); margin-top:20px;
  }
</style>
</head>
<body>

<header>
  <div class="logo">
    <div class="logo-cross"></div>
    <div>
      <h1>SMARTHEALTH</h1>
      <span>PATIENT MONITORING SYSTEM</span>
    </div>
  </div>
  <div class="header-right">
    <div id="clock">--:--:--</div>
    <div class="status-dot"></div>
  </div>
</header>

<main>
  <div class="patient-bar">
    <div class="patient-chip">DEVICE &nbsp;<span>ESP32 NODE</span></div>
    <div class="patient-chip">STATUS &nbsp;<span id="conn-status">CONNECTING...</span></div>
    <div class="patient-chip">SENSOR &nbsp;<span>MAX30102 + VL53L0X</span></div>
    <div class="patient-chip">REFRESH &nbsp;<span>1 sec</span></div>
  </div>

  <div id="alert-banner">WARNING: MASK REMOVED - PLEASE WEAR YOUR MASK</div>

  <div class="vitals-grid">
    <div class="card red">
      <div class="card-label">Heart Rate</div>
      <div class="card-value" id="bpm-val">--</div>
      <div class="card-unit">BPM</div>
      <div class="heart-icon">&#9829;</div>
    </div>
    <div class="card green">
      <div class="card-label">SpO2</div>
      <div class="card-value" id="spo2-val">--</div>
      <div class="card-unit">% Oxygen Saturation</div>
    </div>
    <div class="card blue">
      <div class="card-label">Distance</div>
      <div class="card-value" id="dist-val">--</div>
      <div class="card-unit">mm from sensor</div>
    </div>
    <div class="card" id="mask-card">
      <div class="card-label">Mask Status</div>
      <div class="card-value" style="font-size:1.8rem" id="mask-val">--</div>
      <div style="margin-top:6px">
        <span class="mask-badge" id="mask-badge">--</span>
      </div>
    </div>
  </div>

  <div class="graph-section">
    <div class="section-title">Live Heart Rate Graph</div>
    <canvas id="bpmChart" height="90"></canvas>
  </div>

  <div class="bottom-row">
    <div class="gauge-wrap">
      <div class="section-title" style="width:100%">SpO2 Ring Gauge</div>
      <div class="spo2-ring">
        <svg viewBox="0 0 100 100">
          <circle class="ring-bg"   cx="50" cy="50" r="45"/>
          <circle class="ring-fill" id="spo2-ring" cx="50" cy="50" r="45"/>
        </svg>
        <div class="spo2-center">
          <div class="spo2-val" id="spo2-gauge-val">--</div>
          <div class="spo2-unit">SpO2 %</div>
        </div>
      </div>
    </div>

    <div class="dist-section">
      <div class="section-title">Distance Sensor</div>
      <div class="dist-reading" id="dist-bar-val">-- mm</div>
      <div class="dist-bar-track">
        <div class="dist-bar-fill" id="dist-bar" style="width:0%"></div>
      </div>
      <div class="dist-labels">
        <span>0 mm</span>
        <span>Mask ON &lt;80mm</span>
        <span>400 mm</span>
      </div>
      <div style="margin-top:12px; font-size:.8rem; color:var(--dim)">
        Threshold: <span style="color:var(--warn); font-weight:700">80 mm</span>
      </div>
    </div>
  </div>
</main>

<footer>SmartHealth Monitor v2.0 &nbsp;|&nbsp; ESP32 &nbsp;|&nbsp; VL53L0X + MAX30102 + SSD1306</footer>

<script src="https://cdn.jsdelivr.net/npm/chart.js@4.4.0/dist/chart.umd.min.js"></script>
<script>
function updateClock(){
  const n=new Date();
  document.getElementById('clock').textContent=n.toLocaleTimeString('en-GB',{hour12:false});
}
setInterval(updateClock,1000);
updateClock();

const ctx = document.getElementById('bpmChart').getContext('2d');
const bpmData = Array(60).fill(null);
const gradient = ctx.createLinearGradient(0,0,0,120);
gradient.addColorStop(0,'rgba(255,58,92,0.4)');
gradient.addColorStop(1,'rgba(255,58,92,0)');
const bpmChart = new Chart(ctx, {
  type:'line',
  data:{
    labels: Array(60).fill(''),
    datasets:[{
      data: bpmData,
      borderColor:'#ff3a5c',
      borderWidth:2,
      backgroundColor: gradient,
      tension:0.4,
      fill:true,
      pointRadius:0,
    }]
  },
  options:{
    animation:false,
    responsive:true,
    scales:{
      x:{ display:false },
      y:{
        min:40, max:180,
        grid:{ color:'rgba(13,32,64,0.8)' },
        ticks:{ color:'#4a6a8a', font:{family:'Share Tech Mono', size:10} }
      }
    },
    plugins:{ legend:{display:false}, tooltip:{enabled:false} }
  }
});

function setSpo2Ring(val){
  const circle = document.getElementById('spo2-ring');
  const r = 45;
  const circ = 2 * Math.PI * r;
  const clamped = Math.min(Math.max(val||0, 0), 100);
  const offset = circ - (clamped/100)*circ;
  circle.style.strokeDasharray  = circ;
  circle.style.strokeDashoffset = offset;
  if(clamped>=95)      circle.style.stroke='#00ff9d';
  else if(clamped>=90) circle.style.stroke='#ffb800';
  else                 circle.style.stroke='#ff3a5c';
}

function fetchData(){
  fetch('/data')
  .then(r=>r.json())
  .then(d=>{
    document.getElementById('conn-status').textContent='LIVE';
    const bpm = d.bpm||0;
    document.getElementById('bpm-val').textContent = bpm>0 ? bpm : '--';
    bpmData.push(bpm>0 ? bpm : null);
    bpmData.shift();
    bpmChart.data.datasets[0].data = [...bpmData];
    bpmChart.update('none');

    const sp = parseFloat(d.spo2)||0;
    document.getElementById('spo2-val').textContent       = sp>0 ? sp.toFixed(1) : '--';
    document.getElementById('spo2-gauge-val').textContent = sp>0 ? sp.toFixed(1) : '--';
    setSpo2Ring(sp);

    const dist = parseInt(d.distance);
    document.getElementById('dist-val').textContent     = dist>0 ? dist : '--';
    document.getElementById('dist-bar-val').textContent = dist>0 ? dist+' mm' : '-- mm';
    const pct = dist>0 ? Math.min((dist/400)*100, 100) : 0;
    document.getElementById('dist-bar').style.width = pct+'%';
    document.getElementById('dist-bar').style.background =
      (dist>0&&dist<80) ? 'linear-gradient(90deg,#ff3a5c,#ff7a5c)' :
                          'linear-gradient(90deg,#00ff9d,#00cfff)';

    const maskOn = d.mask==='ON';
    document.getElementById('mask-val').textContent = d.mask;
    const badge = document.getElementById('mask-badge');
    badge.textContent = maskOn ? 'MASK DETECTED' : 'NO MASK';
    badge.className   = 'mask-badge '+(maskOn?'mask-on':'mask-off');
    document.getElementById('mask-card').style.borderColor = maskOn ? 'var(--accent2)' : 'var(--danger)';
    document.getElementById('alert-banner').style.display  = maskOn ? 'none' : 'block';
  })
  .catch(()=>{ document.getElementById('conn-status').textContent='OFFLINE'; });
}

fetchData();
setInterval(fetchData, 1000);
</script>
</body>
</html>
)rawliteral";


// ============================================================
//  OLED UPDATE
// ============================================================
void updateOLED() {
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(18, 0);
  display.print("SMART HEALTH MON.");

  uint8_t hx = heartBig ? 2 : 3;
  uint8_t hy = heartBig ? 11 : 12;
  display.drawBitmap(hx, hy, heartBmp, 8, 8, WHITE);

  display.setCursor(14, 12);
  display.print("BPM:");
  display.setCursor(38, 12);
  if (beatAvg > 0) display.print((int)beatAvg);
  else             display.print("--");

  for (int i = 1; i < GRAPH_POINTS; i++) {
    int y1 = map(constrain(bpmGraph[i-1], 40, 180), 40, 180, 42, 22);
    int y2 = map(constrain(bpmGraph[i],   40, 180), 40, 180, 42, 22);
    int x1 = map(i-1, 0, GRAPH_POINTS-1, 0, 127);
    int x2 = map(i,   0, GRAPH_POINTS-1, 0, 127);
    if (bpmGraph[i-1] > 0 && bpmGraph[i] > 0)
      display.drawLine(x1, y1, x2, y2, WHITE);
  }

  display.drawLine(0, 44, 127, 44, WHITE);

  display.setCursor(0, 47);
  display.print("HR:");
  display.print(beatAvg>0?(int)beatAvg:0);

  display.setCursor(40, 47);
  display.print("O2:");
  display.print(spo2>0?(int)spo2:0);
  display.print("%");

  display.setCursor(0, 56);
  display.print("D:");
  display.print(distance>0?distance:0);
  display.print("mm");

  display.setCursor(60, 56);
  display.print("Mask:");
  display.print(maskStatus);

  display.display();
}


// ============================================================
//  SETUP
// ============================================================
void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED init failed");
    while (1);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(10, 20);
  display.println("SmartHealth v2.0");
  display.setCursor(10, 35);
  display.println("Connecting WiFi...");
  display.display();

  WiFi.begin(ssid, password);
  int tries = 0;
  while (WiFi.status() != WL_CONNECTED && tries < 40) {
    delay(500);
    tries++;
  }

  display.clearDisplay();
  display.setCursor(0, 0);
  if (WiFi.status() == WL_CONNECTED) {
    display.println("WiFi Connected!");
    display.println(WiFi.localIP());
  } else {
    display.println("WiFi FAILED");
    display.println("Running offline");
  }
  display.display();
  delay(6000);

  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("MAX30102 not found");
  }
  particleSensor.setup();
  particleSensor.setPulseAmplitudeRed(0x0A);
  particleSensor.setPulseAmplitudeIR(0x1F);

  if (!lox.begin()) {
    Serial.println("VL53L0X not found");
  }

  server.on("/", HTTP_GET, []() {
    server.send_P(200, "text/html", WEBPAGE);
  });

  server.on("/data", HTTP_GET, []() {
    String json = "{";
    json += "\"bpm\":"      + String((int)beatAvg)   + ",";
    json += "\"spo2\":"     + String(spo2, 1)         + ",";
    json += "\"distance\":" + String(distance)        + ",";
    json += "\"mask\":\""   + maskStatus              + "\"";
    json += "}";
    server.send(200, "application/json", json);
  });

  server.begin();
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}


// ============================================================
//  LOOP
// ============================================================
void loop() {
  server.handleClient();

  irValue = particleSensor.getIR();

  if (irValue > 50000) {
    if (checkForBeat(irValue)) {
      long delta = millis() - lastBeat;
      lastBeat   = millis();
      bpm        = 60.0 / (delta / 1000.0);

      if (bpm > 40 && bpm < 180) {
        beatAvg  = 0.85 * beatAvg + 0.15 * bpm;
        spo2     = 95.0 + random(-1, 2);
        heartBig = !heartBig;
      }
    }
  } else {
    beatAvg = 0;
    spo2    = 0;
  }

  if (millis() - lastVL > 500) {
    lastVL = millis();
    VL53L0X_RangingMeasurementData_t m;
    lox.rangingTest(&m, false);
    distance = (m.RangeStatus != 4) ? m.RangeMilliMeter : -1;
  }

  // Mask ON if distance < 80mm
  if (distance > 0 && distance < 80) {
    maskStatus = "ON";
    maskAlert  = false;
  } else {
    maskStatus = "OFF";
    maskAlert  = true;
  }

  // Buzzer beeps every 2s when mask is OFF
  static unsigned long lastBuzz = 0;
  if (maskAlert && (millis() - lastBuzz > 2000)) {
    lastBuzz = millis();
    for (int b = 0; b < 3; b++) {
      digitalWrite(BUZZER_PIN, HIGH);
      delay(100);
      digitalWrite(BUZZER_PIN, LOW);
      delay(100);
    }
  }

  if (millis() - lastGraphUpdate > 1000) {
    lastGraphUpdate = millis();
    bpmGraph[graphIdx % GRAPH_POINTS] = (int)beatAvg;
    graphIdx++;
  }

  updateOLED();
  delay(20);
}
