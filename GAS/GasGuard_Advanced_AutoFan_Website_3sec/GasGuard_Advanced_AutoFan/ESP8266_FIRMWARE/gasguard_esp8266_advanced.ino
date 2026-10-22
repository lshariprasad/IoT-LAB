#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>

// Wi-Fi settings: enter the network used by your NodeMCU.
const char* WIFI_SSID = "YOUR_WIFI_NAME";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

// Keep this token identical to DEVICE_TOKEN in public_html/config.php.
const char* CLOUD_URL = "https://gas.generai.in/update_status.php";
const char* DEVICE_TOKEN = "BGZYwwBdHzxe6x-7F4D__s0RvN2jHEmf0Zq0VEV-qPA";

constexpr uint8_t GAS_PIN = D1;
constexpr uint8_t ENA_PIN = D0;
constexpr uint8_t IN1_PIN = D5;
constexpr uint8_t IN2_PIN = D6;
constexpr bool GAS_ACTIVE_LOW = true;

const unsigned long HEARTBEAT_MS = 5000UL;
const unsigned long SENSOR_READ_MS = 200UL;
const unsigned long FAN_COOLDOWN_MS = 3000UL; // Fan stays on 3 sec after gas clears.
const unsigned long HTTP_TIMEOUT_MS = 7000UL;

ESP8266WebServer server(80);
bool gasDetected=false, fanOn=false, manualMode=false, manualFanRequest=false;
unsigned long bootMillis=0, lastSensorRead=0, lastHeartbeat=0, gasClearSince=0;
unsigned long lastCommandId=0;
int lastCloudHttpCode=0;
bool cloudEverSucceeded=false;

const char PAGE[] PROGMEM = R"HTML(
<!doctype html><html lang="en"><head><meta charset="UTF-8"><meta name="viewport" content="width=device-width,initial-scale=1"><title>GasGuard Local</title>
<style>
:root{color-scheme:dark;--bg:#0b1020;--card:#151d32;--text:#edf2ff;--muted:#9aa9c7;--blue:#60a5fa;--green:#34d399;--red:#fb7185;--border:#293650}*{box-sizing:border-box}body{margin:0;background:var(--bg);color:var(--text);font-family:system-ui,Arial,sans-serif}header{padding:22px 18px;border-bottom:1px solid var(--border)}main{max-width:900px;margin:auto;padding:20px}h1{margin:0}.sub,.note{color:var(--muted);font-size:13px;line-height:1.5}.grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(220px,1fr));gap:14px}.card{background:var(--card);border:1px solid var(--border);border-radius:16px;padding:19px}.label{font-size:12px;color:var(--muted)}.value{font-size:27px;font-weight:800;margin:12px 0}.green{color:var(--green)}.red{color:var(--red)}button{border:0;border-radius:10px;padding:12px 14px;font-weight:800;margin:5px 4px 5px 0;cursor:pointer}.on{background:var(--green);color:#062a20}.off{background:#344156;color:white}.auto{background:#263957;color:white}
</style></head><body><header><h1>GasGuard <span style="color:#60a5fa">/ Local</span></h1><div class="sub">Gas sensor and fan controller</div></header><main><div class="grid">
<section class="card"><div class="label">GAS SENSOR</div><div id="gas" class="value">--</div><div id="alert" class="note">Reading...</div></section>
<section class="card"><div class="label">FAN</div><div id="fan" class="value">--</div><div id="mode" class="note">AUTO</div><p><button class="auto" onclick="cmd('auto')">AUTO</button><button class="on" onclick="cmd('on')">FAN ON</button><button class="off" onclick="cmd('off')">FAN OFF</button></p></section>
<section class="card"><div class="label">CONNECTION</div><div id="wifi" class="value">--</div><div id="cloud" class="note">Cloud: --</div><div id="code" class="note">HTTP: --</div></section></div></main>
<script>
async function refresh(){try{let r=await fetch('/api/status?t='+Date.now(),{cache:'no-store'});let s=await r.json();document.getElementById('gas').textContent=s.gasDetected?'GAS DETECTED':'CLEAR';document.getElementById('gas').className='value '+(s.gasDetected?'red':'green');document.getElementById('alert').textContent=s.gasDetected?'Sensor triggered':'No sensor alert';document.getElementById('fan').textContent=s.fanOn?'ON':'OFF';document.getElementById('fan').className='value '+(s.fanOn?'green':'');document.getElementById('mode').textContent=s.mode+' mode';document.getElementById('wifi').textContent=s.wifi?'CONNECTED':'OFFLINE';document.getElementById('cloud').textContent='Cloud: '+(s.cloudLinked?'CONNECTED':'NOT CONNECTED');document.getElementById('code').textContent='HTTP: '+s.cloudHttpCode;}catch(e){document.getElementById('wifi').textContent='UNREACHABLE';}}
async function cmd(c){try{let r=await fetch('/api/fan?state='+c,{method:'POST'});let d=await r.json();if(!r.ok||!d.ok)alert(d.message||'Command failed');refresh();}catch(e){alert('Local device unavailable');}}refresh();setInterval(refresh,1500);
</script></body></html>
)HTML";

bool readGasSensor(){int raw=digitalRead(GAS_PIN);return GAS_ACTIVE_LOW?(raw==LOW):(raw==HIGH);}

void setFan(bool enabled){
  if(enabled){digitalWrite(IN1_PIN,HIGH);digitalWrite(IN2_PIN,LOW);digitalWrite(ENA_PIN,HIGH);}
  else{digitalWrite(ENA_PIN,LOW);digitalWrite(IN1_PIN,LOW);digitalWrite(IN2_PIN,LOW);}
  fanOn=enabled;
}

// AUTO behavior: sensor triggers fan ON; after sensor clears, fan runs for cooldown then OFF.
// Gas detection overrides a manual OFF request in this prototype.
void applyFanPolicy(){
  if(gasDetected){gasClearSince=0;setFan(true);return;}
  if(!manualMode){
    if(fanOn){
      if(gasClearSince==0)gasClearSince=millis();
      if(millis()-gasClearSince>=FAN_COOLDOWN_MS)setFan(false);
    }else gasClearSince=0;
  }else{
    gasClearSince=0;
    setFan(manualFanRequest);
  }
}

void sendCloudHeartbeat(){
  if(WiFi.status()!=WL_CONNECTED){lastCloudHttpCode=-1000;return;}
  BearSSL::WiFiClientSecure client;
  // For initial test only. Configure CA certificate verification for production.
  client.setInsecure();
  HTTPClient http;http.setTimeout(HTTP_TIMEOUT_MS);
  if(!http.begin(client,CLOUD_URL)){lastCloudHttpCode=-1001;return;}
  http.addHeader("Content-Type","application/json");
  http.addHeader("X-Device-Token",DEVICE_TOKEN);
  String payload="{\"device\":\"GasGuard\",\"firmware\":\"2.0\",\"gasDetected\":";
  payload+=gasDetected?"true":"false";
  payload+=",\"fanOn\":";payload+=fanOn?"true":"false";
  payload+=",\"mode\":\"";payload+=manualMode?"MANUAL":"AUTO";payload+="\"}";
  int code=http.POST(payload);lastCloudHttpCode=code;
  Serial.printf("[CLOUD] HTTP code: %d\n",code);
  if(code>0){
    String response=http.getString();Serial.print("[CLOUD] Response: ");Serial.println(response);
    if(code>=200&&code<300){
      cloudEverSucceeded=true;
      // Read command returned by heartbeat response and apply newer command.
      int pos=response.indexOf("\"command\":\"");
      if(pos>=0){
        int start=pos+11;int end=response.indexOf('"',start);
        String cmd=end>start?response.substring(start,end):"auto";
        int idPos=response.indexOf("\"command_id\":");
        unsigned long id=0;
        if(idPos>=0){int p=idPos+13;while(p<(int)response.length()&&response[p]==' ')p++;while(p<(int)response.length()&&isDigit(response[p])){id=id*10+(response[p]-'0');p++;}}
        if(id>lastCommandId){
          lastCommandId=id;
          if(cmd=="auto"){manualMode=false;manualFanRequest=false;Serial.println("[CMD] AUTO mode");}
          else if(cmd=="on"){manualMode=true;manualFanRequest=true;Serial.println("[CMD] Manual FAN ON");}
          else if(cmd=="off"){manualMode=true;manualFanRequest=false;Serial.println("[CMD] Manual FAN OFF");}
        }
      }
    }
  }else Serial.printf("[CLOUD] Network/TLS error: %s\n",http.errorToString(code).c_str());
  http.end();
}

void sendStatus(){
  bool cloudLinked=cloudEverSucceeded&&millis()-lastHeartbeat<=15000UL;
  String j="{\"gasDetected\":";j+=gasDetected?"true":"false";
  j+=",\"fanOn\":";j+=fanOn?"true":"false";
  j+=",\"wifi\":";j+=WiFi.status()==WL_CONNECTED?"true":"false";
  j+=",\"cloudLinked\":";j+=cloudLinked?"true":"false";
  j+=",\"cloudHttpCode\":";j+=String(lastCloudHttpCode);
  j+=",\"mode\":\"";j+=manualMode?"MANUAL":"AUTO";j+="\"";
  j+=",\"uptime\":";j+=String((millis()-bootMillis)/1000UL);j+="}";
  server.sendHeader("Cache-Control","no-store");server.send(200,"application/json",j);
}

void handleFan(){
  if(!server.hasArg("state")){server.send(400,"application/json","{\"ok\":false,\"message\":\"Missing state\"}");return;}
  String s=server.arg("state");s.toLowerCase();
  if(s=="auto"){manualMode=false;manualFanRequest=false;server.send(200,"application/json","{\"ok\":true,\"message\":\"AUTO mode enabled\"}");return;}
  if(s=="on"){manualMode=true;manualFanRequest=true;applyFanPolicy();server.send(200,"application/json","{\"ok\":true,\"message\":\"Manual ON requested; gas detection always takes priority\"}");return;}
  if(s=="off"){
    if(gasDetected){setFan(true);server.send(409,"application/json","{\"ok\":false,\"message\":\"Gas detected: fan OFF is blocked\"}");return;}
    manualMode=true;manualFanRequest=false;setFan(false);server.send(200,"application/json","{\"ok\":true,\"message\":\"Manual OFF requested\"}");return;
  }
  server.send(400,"application/json","{\"ok\":false,\"message\":\"Use auto, on, or off\"}");
}

void setup(){
  Serial.begin(115200);delay(100);bootMillis=millis();
  pinMode(GAS_PIN,INPUT);pinMode(ENA_PIN,OUTPUT);pinMode(IN1_PIN,OUTPUT);pinMode(IN2_PIN,OUTPUT);
  digitalWrite(ENA_PIN,LOW);digitalWrite(IN1_PIN,LOW);digitalWrite(IN2_PIN,LOW);
  gasDetected=readGasSensor();
  WiFi.mode(WIFI_STA);WiFi.begin(WIFI_SSID,WIFI_PASSWORD);Serial.print("\nConnecting to Wi-Fi");
  unsigned long start=millis();while(WiFi.status()!=WL_CONNECTED&&millis()-start<20000UL){delay(300);Serial.print(".");}
  if(WiFi.status()==WL_CONNECTED){Serial.println("\nWi-Fi connected!");Serial.print("Local dashboard: http://");Serial.println(WiFi.localIP());}
  else Serial.println("\nWi-Fi connection failed; will retry.");
  server.on("/",HTTP_GET,[]{server.send_P(200,"text/html",PAGE);});
  server.on("/api/status",HTTP_GET,sendStatus);server.on("/api/fan",HTTP_POST,handleFan);
  server.onNotFound([](){server.send(404,"text/plain","Not found");});server.begin();Serial.println("Local server started.");
  applyFanPolicy();sendCloudHeartbeat();lastHeartbeat=millis();
}

void loop(){
  server.handleClient();unsigned long now=millis();
  if(now-lastSensorRead>=SENSOR_READ_MS){lastSensorRead=now;gasDetected=readGasSensor();applyFanPolicy();}
  if(now-lastHeartbeat>=HEARTBEAT_MS){lastHeartbeat=now;sendCloudHeartbeat();}
  if(WiFi.status()!=WL_CONNECTED){WiFi.reconnect();}
  yield();
}
