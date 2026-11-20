#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>

#define DHT_PIN 4
#define DHT_TYPE DHT11
#define TRIG_PIN 5
#define ECHO_PIN 18
#define LDR_PIN 34
#define FAN_RELAY_PIN 26
#define LIGHT_RELAY_PIN 27

const char* AP_SSID = "SmartHome-ESP32";
const char* AP_PASSWORD = "Home12345";
const int LDR_DARK_THRESHOLD = 1800;
const float FAN_ON_TEMPERATURE = 30.0;
const float FAN_OFF_TEMPERATURE = 28.0;
const float OBJECT_DISTANCE_CM = 30.0;
const bool RELAY_ACTIVE_LOW = true;

WebServer server(80);
DHT dht(DHT_PIN, DHT_TYPE);

bool automaticMode = true;
bool fanOn = false;
bool lightOn = false;
float temperatureC = NAN;
float humidity = NAN;
float distanceCm = NAN;
int lightLevel = 0;
bool objectDetected = false;
unsigned long lastSensorRead = 0;

const char DASHBOARD[] PROGMEM = R"HTML(
<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<meta name="theme-color" content="#101827">
<title>SmartHome | Control Center</title>
<style>
:root{color-scheme:dark;--bg:#0b1220;--panel:#121d2e;--panel2:#17243a;--line:#263750;--text:#edf4ff;--muted:#91a2ba;--cyan:#54d6e8;--green:#62dda7;--amber:#ffc66d;--red:#ff7885}*{box-sizing:border-box}body{margin:0;background:radial-gradient(ellipse at 15% 0%,#1b3550 0,transparent 38%),var(--bg);color:var(--text);font-family:Inter,ui-sans-serif,system-ui,-apple-system,Segoe UI,sans-serif;min-height:100vh}.shell{max-width:1180px;margin:auto;padding:28px 22px 42px}.top{display:flex;align-items:center;justify-content:space-between;gap:18px;margin-bottom:26px}.brand{display:flex;align-items:center;gap:13px}.logo{width:46px;height:46px;border-radius:15px;background:linear-gradient(145deg,#54d6e8,#3976c8);display:grid;place-items:center;color:#071321;font-size:23px;font-weight:900;box-shadow:0 10px 28px #54d6e833}.eyebrow{font-size:11px;letter-spacing:2px;text-transform:uppercase;color:var(--cyan);font-weight:800}.brand h1{font-size:21px;margin:3px 0 0;letter-spacing:-.5px}.connection{display:flex;align-items:center;gap:9px;border:1px solid var(--line);background:#111d2d;padding:10px 14px;border-radius:30px;color:var(--muted);font-size:12px}.dot{width:8px;height:8px;border-radius:50%;background:var(--green);box-shadow:0 0 12px var(--green)}.hero{display:flex;justify-content:space-between;align-items:end;gap:20px;margin:25px 0}.hero h2{font-size:clamp(25px,4vw,35px);letter-spacing:-1px;margin:0 0 7px}.hero p{color:var(--muted);margin:0;font-size:14px}.mode{background:var(--panel);border:1px solid var(--line);padding:5px;border-radius:13px;display:flex;gap:5px}.mode button{border:0;background:transparent;color:var(--muted);padding:10px 16px;border-radius:9px;font-weight:700;cursor:pointer}.mode button.active{background:#24435a;color:#8deafa}.grid{display:grid;grid-template-columns:repeat(4,minmax(0,1fr));gap:15px}.card{background:linear-gradient(145deg,#17253a,#111b2b);border:1px solid var(--line);border-radius:18px;padding:19px;min-width:0;box-shadow:0 12px 30px #00000018}.metric{min-height:150px}.cardhead{display:flex;justify-content:space-between;align-items:center;color:var(--muted);font-size:12px;font-weight:700}.symbol{width:34px;height:34px;border-radius:11px;background:#23364e;display:grid;place-items:center;color:var(--cyan);font-size:17px}.value{font-size:34px;font-weight:750;letter-spacing:-1.5px;margin:18px 0 3px}.unit{font-size:14px;color:var(--muted);font-weight:500;letter-spacing:0}.hint{font-size:11px;color:var(--muted)}.section-title{display:flex;align-items:center;justify-content:space-between;margin:27px 0 13px}.section-title h3{margin:0;font-size:16px}.section-title span{color:var(--muted);font-size:11px}.controls{grid-column:span 2}.device{display:flex;align-items:center;justify-content:space-between;gap:16px;padding:14px 0;border-bottom:1px solid var(--line)}.device:last-child{border:0;padding-bottom:0}.device-info{display:flex;align-items:center;gap:12px}.device-icon{width:42px;height:42px;border-radius:13px;background:#23364e;display:grid;place-items:center;font-size:19px}.device-name{font-weight:700;font-size:14px}.device-state{font-size:11px;color:var(--muted);margin-top:4px}.toggle{width:52px;height:29px;border:0;border-radius:30px;background:#35445a;padding:4px;cursor:pointer;transition:.2s}.toggle:after{content:"";display:block;width:21px;height:21px;border-radius:50%;background:#dce7f5;transition:.2s}.toggle.on{background:#168e87}.toggle.on:after{transform:translateX(23px);background:white}.toggle:disabled{opacity:.4;cursor:not-allowed}.wide{grid-column:span 2}.sensor-row{display:flex;justify-content:space-between;gap:12px;padding:13px 0;border-bottom:1px solid var(--line);font-size:13px}.sensor-row:last-child{border:0}.sensor-row span:first-child{color:var(--muted)}.status{color:var(--green);font-weight:700}.status.warn{color:var(--amber)}.footer{margin-top:25px;text-align:center;color:#657891;font-size:11px}.notice{color:var(--muted);font-size:11px;margin-top:13px;line-height:1.5}.error{color:var(--red)}@media(max-width:850px){.grid{grid-template-columns:repeat(2,minmax(0,1fr))}.controls,.wide{grid-column:span 2}}@media(max-width:520px){.shell{padding:19px 14px 30px}.top{align-items:flex-start}.connection{font-size:0;padding:11px}.hero{align-items:flex-start;flex-direction:column}.mode{width:100%}.mode button{flex:1}.grid{gap:10px}.card{padding:15px;border-radius:15px}.metric{min-height:132px}.value{font-size:29px}.section-title{margin-top:22px}}
</style>
</head>
<body>
<div class="shell">
<header class="top"><div class="brand"><div class="logo">⌂</div><div><div class="eyebrow">ESP32 Connected Living</div><h1>SmartHome Control</h1></div></div><div class="connection"><i class="dot"></i><span id="connectionText">Connected to local network</span></div></header>
<section class="hero"><div><h2>Your home, in control.</h2><p>Live environment readings and device automation.</p></div><div class="mode"><button id="autoBtn" class="active" onclick="setMode('auto')">Auto mode</button><button id="manualBtn" onclick="setMode('manual')">Manual mode</button></div></section>
<div class="grid">
<div class="card metric"><div class="cardhead"><span>TEMPERATURE</span><span class="symbol">℃</span></div><div class="value" id="temperature">--<span class="unit"> °C</span></div><div class="hint">Indoor ambient temperature</div></div>
<div class="card metric"><div class="cardhead"><span>HUMIDITY</span><span class="symbol">◌</span></div><div class="value" id="humidity">--<span class="unit"> %</span></div><div class="hint">Relative humidity</div></div>
<div class="card metric"><div class="cardhead"><span>DISTANCE</span><span class="symbol">⌁</span></div><div class="value" id="distance">--<span class="unit"> cm</span></div><div class="hint">Ultrasonic sensor reading</div></div>
<div class="card metric"><div class="cardhead"><span>LIGHT LEVEL</span><span class="symbol">☼</span></div><div class="value" id="lightLevel">--</div><div class="hint">LDR analog value · 0–4095</div></div>
<div class="card controls"><div class="cardhead"><span>DEVICE CONTROL</span><span id="controlMode">AUTOMATIC</span></div>
<div class="device"><div class="device-info"><div class="device-icon">▤</div><div><div class="device-name">Smart fan</div><div class="device-state" id="fanState">Waiting for data</div></div></div><button class="toggle" id="fanToggle" onclick="toggleDevice('fan')" aria-label="Toggle fan"></button></div>
<div class="device"><div class="device-info"><div class="device-icon">☼</div><div><div class="device-name">Smart light</div><div class="device-state" id="lightState">Waiting for data</div></div></div><button class="toggle" id="lightToggle" onclick="toggleDevice('light')" aria-label="Toggle light"></button></div>
<div class="notice">Manual controls are enabled only in Manual mode. Automatic mode uses temperature and ambient light thresholds.</div></div>
<div class="card wide"><div class="cardhead"><span>HOME SENSOR STATUS</span><span>LIVE</span></div>
<div class="sensor-row"><span>Object detection</span><strong id="objectStatus" class="status">--</strong></div>
<div class="sensor-row"><span>Ambient condition</span><strong id="ambientStatus">--</strong></div>
<div class="sensor-row"><span>Automation state</span><strong id="automationStatus">--</strong></div>
<div class="sensor-row"><span>Last update</span><strong id="lastUpdate">--</strong></div></div>
</div><div class="footer">SMART HOME AUTOMATION · LOCAL ESP32 NETWORK · NO INTERNET REQUIRED</div>
</div>
<script>
let state={mode:'auto',fan:false,light:false};
async function request(path){const r=await fetch(path,{cache:'no-store'});if(!r.ok)throw new Error('Request failed');return r.json()}
function updateToggle(id,on,disabled){const el=document.getElementById(id);el.classList.toggle('on',!!on);el.disabled=!!disabled}
function render(d){state=d;document.getElementById('temperature').innerHTML=(d.temperature===null?'--':d.temperature.toFixed(1))+'<span class="unit"> °C</span>';document.getElementById('humidity').innerHTML=(d.humidity===null?'--':d.humidity.toFixed(0))+'<span class="unit"> %</span>';document.getElementById('distance').innerHTML=(d.distance===null?'--':d.distance.toFixed(1))+'<span class="unit"> cm</span>';document.getElementById('lightLevel').textContent=d.lightLevel;document.getElementById('fanState').textContent=d.fan?'ON':'OFF';document.getElementById('lightState').textContent=d.light?'ON':'OFF';updateToggle('fanToggle',d.fan,d.mode==='auto');updateToggle('lightToggle',d.light,d.mode==='auto');document.getElementById('controlMode').textContent=d.mode==='auto'?'AUTOMATIC':'MANUAL';document.getElementById('autoBtn').classList.toggle('active',d.mode==='auto');document.getElementById('manualBtn').classList.toggle('active',d.mode==='manual');document.getElementById('objectStatus').textContent=d.objectDetected?'Object detected':'No object nearby';document.getElementById('objectStatus').classList.toggle('warn',d.objectDetected);document.getElementById('ambientStatus').textContent=d.isDark?'Low light':'Sufficient light';document.getElementById('automationStatus').textContent=d.mode==='auto'?'Active':'Manual control';document.getElementById('lastUpdate').textContent=new Date().toLocaleTimeString();document.getElementById('connectionText').textContent='Connected to local network'}
async function refresh(){try{render(await request('/api/status'))}catch(e){document.getElementById('connectionText').textContent='Connection interrupted';document.getElementById('connectionText').classList.add('error')}}
async function setMode(mode){try{await request('/api/control?mode='+mode);await refresh()}catch(e){}}
async function toggleDevice(device){if(state.mode!=='manual')return;const next=!state[device];try{await request('/api/control?'+device+'='+(next?'1':'0'));await refresh()}catch(e){}}
refresh();setInterval(refresh,2000);
</script>
</body>
</html>
)HTML";

void writeRelay(uint8_t pin, bool on) {
  digitalWrite(pin, RELAY_ACTIVE_LOW ? (on ? LOW : HIGH) : (on ? HIGH : LOW));
}

float readDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(3);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  unsigned long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0) return NAN;
  return duration * 0.0343f / 2.0f;
}

void readSensors() {
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  if (!isnan(t)) temperatureC = t;
  if (!isnan(h)) humidity = h;
  distanceCm = readDistance();
  lightLevel = analogRead(LDR_PIN);
  objectDetected = !isnan(distanceCm) && distanceCm <= OBJECT_DISTANCE_CM;
}

void applyAutomation() {
  if (automaticMode) {
    if (!isnan(temperatureC)) {
      if (temperatureC >= FAN_ON_TEMPERATURE) fanOn = true;
      if (temperatureC <= FAN_OFF_TEMPERATURE) fanOn = false;
    }
    lightOn = lightLevel < LDR_DARK_THRESHOLD;
  }
  writeRelay(FAN_RELAY_PIN, fanOn);
  writeRelay(LIGHT_RELAY_PIN, lightOn);
}

void sendStatus() {
  String json = "{";
  json += "\"mode\":\"" + String(automaticMode ? "auto" : "manual") + "\",";
  json += "\"fan\":" + String(fanOn ? "true" : "false") + ",";
  json += "\"light\":" + String(lightOn ? "true" : "false") + ",";
  json += "\"temperature\":" + (isnan(temperatureC) ? String("null") : String(temperatureC, 1)) + ",";
  json += "\"humidity\":" + (isnan(humidity) ? String("null") : String(humidity, 1)) + ",";
  json += "\"distance\":" + (isnan(distanceCm) ? String("null") : String(distanceCm, 1)) + ",";
  json += "\"lightLevel\":" + String(lightLevel) + ",";
  json += "\"objectDetected\":" + String(objectDetected ? "true" : "false") + ",";
  json += "\"isDark\":" + String(lightLevel < LDR_DARK_THRESHOLD ? "true" : "false");
  json += "}";
  server.send(200, "application/json", json);
}

void handleControl() {
  if (server.hasArg("mode")) {
    String mode = server.arg("mode");
    if (mode == "auto") automaticMode = true;
    if (mode == "manual") automaticMode = false;
  }
  if (!automaticMode) {
    if (server.hasArg("fan")) fanOn = server.arg("fan") == "1";
    if (server.hasArg("light")) lightOn = server.arg("light") == "1";
  }
  applyAutomation();
  sendStatus();
}

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(FAN_RELAY_PIN, OUTPUT);
  pinMode(LIGHT_RELAY_PIN, OUTPUT);
  pinMode(LDR_PIN, INPUT);
  writeRelay(FAN_RELAY_PIN, false);
  writeRelay(LIGHT_RELAY_PIN, false);
  analogReadResolution(12);
  dht.begin();
  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PASSWORD);
  server.on("/", HTTP_GET, []() { server.send_P(200, "text/html", DASHBOARD); });
  server.on("/api/status", HTTP_GET, sendStatus);
  server.on("/api/control", HTTP_GET, handleControl);
  server.onNotFound([]() { server.send(404, "text/plain", "Not found"); });
  server.begin();
  Serial.print("Dashboard: http://");
  Serial.println(WiFi.softAPIP());
}

void loop() {
  server.handleClient();
  if (millis() - lastSensorRead >= 2000) {
    lastSensorRead = millis();
    readSensors();
    applyAutomation();
  }
}
