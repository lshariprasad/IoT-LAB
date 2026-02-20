#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "HARIDEVAN2006";
const char* password = "HARIDEVAN@1410";

WebServer server(80);

#define SWITCH_PIN 5

// ================== FULL WEBSITE ==================
const char index_html[] PROGMEM = R"rawliteral(

<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Dr. Anand - ESP32 Smart Switch</title>
  <script src="https://cdn.jsdelivr.net/npm/@tailwindcss/browser@4"></script>
  <link href="https://fonts.googleapis.com/css2?family=Playfair+Display:wght@700;900&family=Inter:wght@300;400;500;600;700&family=JetBrains+Mono:wght@400;500&display=swap" rel="stylesheet">
  <style>
    body {
      font-family: 'Inter', sans-serif;
      margin: 0;
      min-height: 100vh;
      overflow-x: hidden;
    }

    .bg-gradient {
      background: linear-gradient(135deg, #0f0c29 0%, #1a1a3e 30%, #24243e 60%, #0f0c29 100%);
      min-height: 100vh;
      position: relative;
    }

    .bg-gradient::before {
      content: '';
      position: absolute;
      top: 0; left: 0; right: 0; bottom: 0;
      background: radial-gradient(circle at 20% 80%, rgba(79, 70, 229, 0.15) 0%, transparent 50%),
                  radial-gradient(circle at 80% 20%, rgba(16, 185, 129, 0.1) 0%, transparent 50%),
                  radial-gradient(circle at 50% 50%, rgba(139, 92, 246, 0.05) 0%, transparent 70%);
      pointer-events: none;
    }

    .playfair { font-family: 'Playfair Display', serif; }
    .mono { font-family: 'JetBrains Mono', monospace; }

    /* Floating particles */
    .particle {
      position: absolute;
      border-radius: 50%;
      background: rgba(255,255,255,0.05);
      animation: float-particle linear infinite;
      pointer-events: none;
    }

    @keyframes float-particle {
      0% { transform: translateY(100vh) scale(0); opacity: 0; }
      10% { opacity: 1; }
      90% { opacity: 1; }
      100% { transform: translateY(-10vh) scale(1); opacity: 0; }
    }

    /* Glowing ring around switch */
    .switch-container {
      position: relative;
      width: 280px;
      height: 280px;
      display: flex;
      align-items: center;
      justify-content: center;
    }

    .glow-ring {
      position: absolute;
      width: 100%;
      height: 100%;
      border-radius: 50%;
      border: 3px solid transparent;
      transition: all 1s ease;
    }

    .glow-ring.on {
      border-color: rgba(16, 185, 129, 0.6);
      box-shadow: 0 0 40px rgba(16, 185, 129, 0.3), 0 0 80px rgba(16, 185, 129, 0.15), inset 0 0 40px rgba(16, 185, 129, 0.1);
      animation: pulse-green 2s ease-in-out infinite;
    }

    .glow-ring.off {
      border-color: rgba(239, 68, 68, 0.6);
      box-shadow: 0 0 40px rgba(239, 68, 68, 0.3), 0 0 80px rgba(239, 68, 68, 0.15), inset 0 0 40px rgba(239, 68, 68, 0.1);
      animation: pulse-red 2s ease-in-out infinite;
    }

    @keyframes pulse-green {
      0%, 100% { box-shadow: 0 0 40px rgba(16, 185, 129, 0.3), 0 0 80px rgba(16, 185, 129, 0.15); }
      50% { box-shadow: 0 0 60px rgba(16, 185, 129, 0.5), 0 0 120px rgba(16, 185, 129, 0.25); }
    }

    @keyframes pulse-red {
      0%, 100% { box-shadow: 0 0 40px rgba(239, 68, 68, 0.3), 0 0 80px rgba(239, 68, 68, 0.15); }
      50% { box-shadow: 0 0 60px rgba(239, 68, 68, 0.5), 0 0 120px rgba(239, 68, 68, 0.25); }
    }

    .switch-circle {
      width: 220px;
      height: 220px;
      border-radius: 50%;
      display: flex;
      flex-direction: column;
      align-items: center;
      justify-content: center;
      transition: all 0.8s cubic-bezier(0.4, 0, 0.2, 1);
      position: relative;
      z-index: 2;
      cursor: default;
    }

    .switch-circle.on {
      background: linear-gradient(145deg, #065f46, #10b981, #34d399);
      box-shadow: 0 20px 60px rgba(16, 185, 129, 0.4);
    }

    .switch-circle.off {
      background: linear-gradient(145deg, #7f1d1d, #ef4444, #f87171);
      box-shadow: 0 20px 60px rgba(239, 68, 68, 0.4);
    }

    .switch-circle.loading {
      background: linear-gradient(145deg, #374151, #6b7280, #9ca3af);
      box-shadow: 0 20px 60px rgba(107, 114, 128, 0.4);
    }

    /* Power icon */
    .power-icon {
      width: 60px;
      height: 60px;
      margin-bottom: 8px;
      transition: all 0.5s ease;
    }

    .power-icon.on { filter: drop-shadow(0 0 10px rgba(255,255,255,0.8)); }

    /* Status bar animation */
    .status-bar {
      height: 4px;
      border-radius: 2px;
      overflow: hidden;
      width: 200px;
      background: rgba(255,255,255,0.1);
      margin: 0 auto;
    }

    .status-bar-fill {
      height: 100%;
      border-radius: 2px;
      transition: all 0.8s ease;
      width: 100%;
    }

    .status-bar-fill.on { background: linear-gradient(90deg, #10b981, #34d399); }
    .status-bar-fill.off { background: linear-gradient(90deg, #ef4444, #f87171); }
    .status-bar-fill.loading { background: linear-gradient(90deg, #6b7280, #9ca3af); animation: shimmer 1.5s infinite; }

    @keyframes shimmer {
      0% { transform: translateX(-100%); }
      100% { transform: translateX(100%); }
    }

    /* Card glass effect */
    .glass-card {
      background: rgba(255, 255, 255, 0.05);
      backdrop-filter: blur(20px);
      -webkit-backdrop-filter: blur(20px);
      border: 1px solid rgba(255, 255, 255, 0.1);
      border-radius: 24px;
      transition: all 0.3s ease;
    }

    .glass-card:hover {
      background: rgba(255, 255, 255, 0.08);
      border-color: rgba(255, 255, 255, 0.15);
    }

    /* Rotating border for the outer ring */
    .rotating-border {
      position: absolute;
      width: 290px;
      height: 290px;
      border-radius: 50%;
      border: 2px dashed rgba(255,255,255,0.08);
      animation: rotate-slow 20s linear infinite;
    }

    @keyframes rotate-slow {
      from { transform: rotate(0deg); }
      to { transform: rotate(360deg); }
    }

    /* Signal dots */
    .signal-dot {
      width: 8px;
      height: 8px;
      border-radius: 50%;
      display: inline-block;
      margin: 0 3px;
      animation: blink 1.5s ease-in-out infinite;
    }

    .signal-dot:nth-child(2) { animation-delay: 0.3s; }
    .signal-dot:nth-child(3) { animation-delay: 0.6s; }

    @keyframes blink {
      0%, 100% { opacity: 0.3; }
      50% { opacity: 1; }
    }

    /* Last updated pulse */
    .live-dot {
      width: 10px;
      height: 10px;
      border-radius: 50%;
      display: inline-block;
      position: relative;
    }

    .live-dot::after {
      content: '';
      position: absolute;
      top: -3px; left: -3px;
      width: 16px;
      height: 16px;
      border-radius: 50%;
      animation: live-pulse 2s ease-in-out infinite;
    }

    .live-dot.on { background: #10b981; }
    .live-dot.on::after { border: 2px solid rgba(16, 185, 129, 0.5); }
    .live-dot.off { background: #ef4444; }
    .live-dot.off::after { border: 2px solid rgba(239, 68, 68, 0.5); }

    @keyframes live-pulse {
      0%, 100% { transform: scale(1); opacity: 1; }
      50% { transform: scale(1.5); opacity: 0; }
    }

    /* Avatar glow */
    .avatar-ring {
      background: linear-gradient(135deg, #8b5cf6, #6366f1, #4f46e5);
      padding: 3px;
      border-radius: 50%;
      display: inline-block;
      box-shadow: 0 0 30px rgba(99, 102, 241, 0.3);
    }

    /* Fade in animation */
    .fade-in {
      animation: fadeInUp 0.8s ease-out forwards;
      opacity: 0;
    }

    .fade-in-delay-1 { animation-delay: 0.2s; }
    .fade-in-delay-2 { animation-delay: 0.4s; }
    .fade-in-delay-3 { animation-delay: 0.6s; }
    .fade-in-delay-4 { animation-delay: 0.8s; }
    .fade-in-delay-5 { animation-delay: 1.0s; }

    @keyframes fadeInUp {
      from { opacity: 0; transform: translateY(30px); }
      to { opacity: 1; transform: translateY(0); }
    }

    /* Info cards */
    .info-card {
      background: rgba(255,255,255,0.03);
      border: 1px solid rgba(255,255,255,0.06);
      border-radius: 16px;
      padding: 20px;
      text-align: center;
      transition: all 0.3s ease;
    }

    .info-card:hover {
      background: rgba(255,255,255,0.06);
      transform: translateY(-2px);
    }

    /* History log */
    .log-entry {
      padding: 10px 16px;
      border-radius: 10px;
      background: rgba(255,255,255,0.03);
      margin-bottom: 8px;
      border-left: 3px solid;
      animation: slideIn 0.3s ease-out;
    }

    @keyframes slideIn {
      from { opacity: 0; transform: translateX(-20px); }
      to { opacity: 1; transform: translateX(0); }
    }

    .log-entry.on { border-left-color: #10b981; }
    .log-entry.off { border-left-color: #ef4444; }

    /* Code block styling */
    .code-block {
      background: rgba(0, 0, 0, 0.4);
      border: 1px solid rgba(255,255,255,0.08);
      border-radius: 16px;
      overflow: hidden;
    }

    .code-block-header {
      background: rgba(255,255,255,0.05);
      border-bottom: 1px solid rgba(255,255,255,0.08);
      padding: 12px 20px;
      display: flex;
      align-items: center;
      justify-content: space-between;
    }

    .code-block pre {
      padding: 20px;
      margin: 0;
      overflow-x: auto;
      font-size: 12px;
      line-height: 1.6;
      color: #e2e8f0;
      scrollbar-width: thin;
      scrollbar-color: rgba(255,255,255,0.1) transparent;
    }

    .code-block code {
      font-family: 'JetBrains Mono', monospace;
    }

    /* Syntax highlighting */
    .code-block .kw { color: #c792ea; }
    .code-block .str { color: #c3e88d; }
    .code-block .num { color: #f78c6c; }
    .code-block .cmt { color: #546e7a; font-style: italic; }
    .code-block .fn { color: #82aaff; }
    .code-block .type { color: #ffcb6b; }
    .code-block .prep { color: #89ddff; }
    .code-block .def { color: #f07178; }

    /* Copy button */
    .copy-btn {
      background: rgba(99, 102, 241, 0.2);
      border: 1px solid rgba(99, 102, 241, 0.3);
      color: #a5b4fc;
      padding: 6px 16px;
      border-radius: 8px;
      font-size: 12px;
      cursor: pointer;
      transition: all 0.3s ease;
      font-family: 'Inter', sans-serif;
    }

    .copy-btn:hover {
      background: rgba(99, 102, 241, 0.4);
      border-color: rgba(99, 102, 241, 0.5);
    }

    .copy-btn.copied {
      background: rgba(16, 185, 129, 0.2);
      border-color: rgba(16, 185, 129, 0.3);
      color: #6ee7b7;
    }

    /* Steps styling */
    .step-number {
      width: 28px;
      height: 28px;
      border-radius: 50%;
      display: flex;
      align-items: center;
      justify-content: center;
      font-size: 13px;
      font-weight: 700;
      flex-shrink: 0;
    }

    /* Tab styling */
    .tab-btn {
      padding: 8px 20px;
      border-radius: 10px;
      font-size: 13px;
      font-weight: 500;
      cursor: pointer;
      transition: all 0.3s ease;
      border: 1px solid transparent;
      background: transparent;
      color: rgba(255,255,255,0.4);
    }

    .tab-btn.active {
      background: rgba(99, 102, 241, 0.2);
      border-color: rgba(99, 102, 241, 0.3);
      color: #a5b4fc;
    }

    .tab-btn:hover:not(.active) {
      color: rgba(255,255,255,0.7);
      background: rgba(255,255,255,0.05);
    }

    .tab-content {
      display: none;
    }

    .tab-content.active {
      display: block;
    }
  </style>
</head>
<body class="bg-gradient text-white">
  <!-- Floating Particles -->
  <div id="particles"></div>

  <div class="relative z-10 min-h-screen flex flex-col">
    <!-- Header -->
    <header class="fade-in pt-6 pb-4 px-4">
      <div class="max-w-2xl mx-auto flex items-center justify-between">
        <div class="flex items-center gap-3">
          <div class="w-10 h-10 rounded-xl bg-gradient-to-br from-indigo-500 to-purple-600 flex items-center justify-center shadow-lg shadow-indigo-500/20">
            <svg class="w-5 h-5" fill="none" stroke="currentColor" viewBox="0 0 24 24">
              <path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M13 10V3L4 14h7v7l9-11h-7z"/>
            </svg>
          </div>
          <div>
            <h1 class="text-sm font-bold tracking-wider text-white/90">ESP32 SMART SWITCH</h1>
            <p class="text-xs text-white/40">IoT Control Panel v2.0</p>
          </div>
        </div>
        <div class="flex items-center gap-2 text-xs text-white/40">
          <div class="signal-dot bg-indigo-400"></div>
          <div class="signal-dot bg-indigo-400"></div>
          <div class="signal-dot bg-indigo-400"></div>
          <span class="ml-1">LIVE</span>
        </div>
      </div>
    </header>

    <!-- Main Content -->
    <main class="flex-1 flex flex-col items-center justify-center px-4 pb-8">
      <!-- Profile Section -->
      <div class="fade-in fade-in-delay-1 text-center mb-8">
        <div class="avatar-ring mb-4 mx-auto">
          <div class="w-20 h-20 rounded-full bg-gradient-to-br from-slate-700 to-slate-800 flex items-center justify-center text-3xl font-bold playfair">
            A
          </div>
        </div>
        <h2 class="playfair text-3xl md:text-4xl font-black bg-gradient-to-r from-white via-purple-200 to-indigo-200 bg-clip-text text-transparent">
          Dr. Anand
        </h2>
        <p class="text-white/50 text-sm mt-1 tracking-wide">Dean of the College</p>
        <div class="flex items-center justify-center gap-2 mt-2">
          <span class="px-3 py-1 rounded-full text-xs bg-indigo-500/20 text-indigo-300 border border-indigo-500/20">🎓 Administration</span>
          <span class="px-3 py-1 rounded-full text-xs bg-purple-500/20 text-purple-300 border border-purple-500/20">⚡ IoT Lab</span>
        </div>
      </div>

      <!-- Switch Display -->
      <div class="fade-in fade-in-delay-2 mb-8">
        <div class="switch-container mx-auto">
          <div class="rotating-border"></div>
          <div id="glowRing" class="glow-ring"></div>
          <div id="switchCircle" class="switch-circle loading">
            <svg id="powerIcon" class="power-icon" viewBox="0 0 24 24" fill="none" stroke="white" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
              <path d="M18.36 6.64a9 9 0 1 1-12.73 0"/>
              <line x1="12" y1="2" x2="12" y2="12"/>
            </svg>
            <span id="statusText" class="text-white font-bold text-xl tracking-wider">LOADING</span>
            <span id="statusSub" class="text-white/60 text-xs mt-1">Connecting...</span>
          </div>
        </div>
      </div>

      <!-- Status Bar -->
      <div class="fade-in fade-in-delay-3 w-full max-w-md mx-auto mb-8">
        <div class="glass-card p-6">
          <div class="flex items-center justify-between mb-4">
            <div class="flex items-center gap-2">
              <div id="liveDot" class="live-dot off"></div>
              <span class="text-sm text-white/70">Switch Status</span>
            </div>
            <span id="statusBadge" class="px-3 py-1 rounded-full text-xs font-semibold bg-gray-500/20 text-gray-300">--</span>
          </div>

          <div class="status-bar mb-4">
            <div id="statusBarFill" class="status-bar-fill loading"></div>
          </div>

          <!-- Info Grid -->
          <div class="grid grid-cols-3 gap-3 mt-4">
            <div class="info-card">
              <div class="text-lg font-bold text-white/90" id="toggleCount">0</div>
              <div class="text-xs text-white/40 mt-1">Toggles</div>
            </div>
            <div class="info-card">
              <div class="text-lg font-bold text-white/90" id="uptime">0s</div>
              <div class="text-xs text-white/40 mt-1">Uptime</div>
            </div>
            <div class="info-card">
              <div class="text-lg font-bold text-white/90" id="lastUpdate">--</div>
              <div class="text-xs text-white/40 mt-1">Last Check</div>
            </div>
          </div>
        </div>
      </div>

      <!-- Activity Log -->
      <div class="fade-in fade-in-delay-4 w-full max-w-md mx-auto mb-8">
        <div class="glass-card p-6">
          <div class="flex items-center justify-between mb-4">
            <h3 class="text-sm font-semibold text-white/70 flex items-center gap-2">
              <svg class="w-4 h-4" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                <path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M12 8v4l3 3m6-3a9 9 0 11-18 0 9 9 0 0118 0z"/>
              </svg>
              Activity Log
            </h3>
            <button onclick="clearLog()" class="text-xs text-white/30 hover:text-white/60 transition-colors">Clear</button>
          </div>
          <div id="activityLog" class="max-h-48 overflow-y-auto space-y-0 pr-1" style="scrollbar-width: thin; scrollbar-color: rgba(255,255,255,0.1) transparent;">
            <div class="text-center text-white/20 text-sm py-4">Waiting for data...</div>
          </div>
        </div>
      </div>

      <!-- ESP32 Arduino Code Section -->
      <div class="fade-in fade-in-delay-5 w-full max-w-2xl mx-auto mb-8">
        <div class="glass-card p-6">
          <!-- Section Header -->
          <div class="flex items-center gap-3 mb-5">
            <div class="w-10 h-10 rounded-xl bg-gradient-to-br from-amber-500 to-orange-600 flex items-center justify-center shadow-lg shadow-amber-500/20">
              <svg class="w-5 h-5" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                <path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M10 20l4-16m4 4l4 4-4 4M6 16l-4-4 4-4"/>
              </svg>
            </div>
            <div>
              <h3 class="text-base font-bold text-white/90">ESP32 Arduino Code</h3>
              <p class="text-xs text-white/40">Upload this to your ESP32 board</p>
            </div>
          </div>

          <!-- Setup Instructions -->
          <div class="mb-5 space-y-3">
            <div class="flex items-start gap-3">
              <div class="step-number bg-indigo-500/20 text-indigo-300 border border-indigo-500/30 mt-0.5">1</div>
              <div>
                <p class="text-sm text-white/80 font-medium">Change WiFi Credentials</p>
                <p class="text-xs text-white/40 mt-0.5">Replace <code class="mono text-amber-300/80 bg-amber-500/10 px-1.5 py-0.5 rounded">YOUR_WIFI_NAME</code> and <code class="mono text-amber-300/80 bg-amber-500/10 px-1.5 py-0.5 rounded">YOUR_WIFI_PASSWORD</code></p>
              </div>
            </div>
            <div class="flex items-start gap-3">
              <div class="step-number bg-emerald-500/20 text-emerald-300 border border-emerald-500/30 mt-0.5">2</div>
              <div>
                <p class="text-sm text-white/80 font-medium">Upload to ESP32</p>
                <p class="text-xs text-white/40 mt-0.5">Open Arduino IDE → Select ESP32 Board → Upload</p>
              </div>
            </div>
            <div class="flex items-start gap-3">
              <div class="step-number bg-purple-500/20 text-purple-300 border border-purple-500/30 mt-0.5">3</div>
              <div>
                <p class="text-sm text-white/80 font-medium">Open Serial Monitor</p>
                <p class="text-xs text-white/40 mt-0.5">Check the IP address printed → Open in browser</p>
              </div>
            </div>
          </div>

          <!-- Wiring Info -->
          <div class="mb-5 p-4 rounded-xl bg-amber-500/5 border border-amber-500/15">
            <div class="flex items-center gap-2 mb-2">
              <span class="text-amber-400">⚡</span>
              <span class="text-sm font-semibold text-amber-300/90">Wiring</span>
            </div>
            <p class="text-xs text-white/50 leading-relaxed">
              Connect your switch between <code class="mono text-amber-300/80 bg-amber-500/10 px-1.5 py-0.5 rounded">GPIO 5</code> and <code class="mono text-amber-300/80 bg-amber-500/10 px-1.5 py-0.5 rounded">GND</code>. 
              The code uses <code class="mono text-amber-300/80 bg-amber-500/10 px-1.5 py-0.5 rounded">INPUT_PULLUP</code>, so no external resistor is needed.
              When switch is pressed (GPIO reads 0) → Website shows <span class="text-emerald-400 font-semibold">ON</span>. 
              When switch is released (GPIO reads 1) → Website shows <span class="text-red-400 font-semibold">OFF</span>.
            </p>
          </div>

          <!-- Code Block -->
          <div class="code-block">
            <div class="code-block-header flex items-center justify-between">
              <div class="flex items-center gap-2">
                <div class="flex gap-1.5">
                  <div class="w-3 h-3 rounded-full bg-red-500/60"></div>
                  <div class="w-3 h-3 rounded-full bg-yellow-500/60"></div>
                  <div class="w-3 h-3 rounded-full bg-green-500/60"></div>
                </div>
                <span class="text-xs text-white/40 ml-2 mono">esp32_smart_switch.ino</span>
              </div>
              <button onclick="copyCode()" id="copyBtn" class="copy-btn">
                <span id="copyBtnText">📋 Copy Code</span>
              </button>
            </div>
            <pre><code id="arduinoCode"></code></pre>
          </div>
        </div>
      </div>
    </main>

    <!-- Footer -->
    <footer class="text-center py-4 text-white/20 text-xs">
      <p>Designed for <span class="text-indigo-400/60">Dr. Anand</span> • ESP32 IoT Project</p>
      <p class="mt-1">Polling every 500ms • Built with ❤️</p>
    </footer>
  </div>

  <!-- JavaScript to read switch state from ESP32 and update UI -->
  <script>
    document.addEventListener('DOMContentLoaded', function () {
      // Existing elements for switch + status bar
      var switchCircle   = document.getElementById('switchCircle');
      var glowRing       = document.getElementById('glowRing');
      var statusText     = document.getElementById('statusText');
      var statusSub      = document.getElementById('statusSub');
      var liveDot        = document.getElementById('liveDot');
      var statusBadge    = document.getElementById('statusBadge');
      var statusBarFill  = document.getElementById('statusBarFill');

      // New elements for toggles, uptime, last check, activity log
      var toggleCountEl  = document.getElementById('toggleCount');
      var uptimeEl       = document.getElementById('uptime');
      var lastUpdateEl   = document.getElementById('lastUpdate');
      var activityLog    = document.getElementById('activityLog');

      var lastState   = null;
      var toggleCount = 0;
      var startTime   = Date.now();

      function formatUptime(ms) {
        var total = Math.floor(ms / 1000);
        var h = Math.floor(total / 3600);
        var m = Math.floor((total % 3600) / 60);
        var s = total % 60;
        if (h > 0) return h + 'h ' + m + 'm';
        if (m > 0) return m + 'm ' + s + 's';
        return s + 's';
      }

      function updateUptime() {
        if (uptimeEl) {
          uptimeEl.textContent = formatUptime(Date.now() - startTime);
        }
      }
      // Start uptime timer
      updateUptime();
      setInterval(updateUptime, 1000);

      function applyState(isOn) {
        // ===== Existing visual behaviour (unchanged) =====
        switchCircle.classList.remove('loading', 'on', 'off');
        glowRing.classList.remove('on', 'off');
        statusBarFill.classList.remove('loading', 'on', 'off');
        liveDot.classList.remove('on', 'off');

        switchCircle.classList.add(isOn ? 'on' : 'off');
        glowRing.classList.add(isOn ? 'on' : 'off');
        statusBarFill.classList.add(isOn ? 'on' : 'off');
        liveDot.classList.add(isOn ? 'on' : 'off');

        statusText.textContent = isOn ? 'ON' : 'OFF';
        statusSub.textContent  = isOn ? 'Switch is ACTIVE' : 'Switch is IDLE';

        statusBadge.textContent = isOn ? 'ON' : 'OFF';
        statusBadge.className =
          'px-3 py-1 rounded-full text-xs font-semibold ' +
          (isOn ? 'bg-emerald-500/20 text-emerald-300'
                : 'bg-red-500/20 text-red-300');

        // ===== New: last check time =====
        var now = new Date();
        var timeStr = now.toLocaleTimeString();
        if (lastUpdateEl) {
          lastUpdateEl.textContent = timeStr;
        }

        // ===== New: toggles + activity log =====
        if (lastState !== null && lastState !== isOn) {
          // Only count when state actually changes
          toggleCount++;
          if (toggleCountEl) {
            toggleCountEl.textContent = String(toggleCount);
          }

          if (activityLog) {
            // Remove "Waiting for data..." placeholder the first time
            if (activityLog.firstElementChild &&
                activityLog.firstElementChild.classList.contains('text-center')) {
              activityLog.innerHTML = '';
            }

            var entry = document.createElement('div');
            entry.className = 'log-entry ' + (isOn ? 'on' : 'off');
            entry.innerHTML =
              '<div class="flex justify-between text-xs text-white/70">' +
                '<span>' + (isOn ? 'Switched ON' : 'Switched OFF') + '</span>' +
                '<span class="text-white/40">' + timeStr + '</span>' +
              '</div>';

            // Add new entry to top (compatible with older browsers)
            if (activityLog.firstChild) {
              activityLog.insertBefore(entry, activityLog.firstChild);
            } else {
              activityLog.appendChild(entry);
            }

            // Limit log length
            while (activityLog.children.length > 30) {
              activityLog.removeChild(activityLog.lastChild);
            }
          }
        }

        lastState = isOn;
      }

      function pollStatus() {
        fetch('/status')
          .then(function (res) { return res.text(); })
          .then(function (text) {
            var value = text.trim();
            // INPUT_PULLUP: 0 = pressed (to GND) -> ON, 1 = released -> OFF
            var isOn = (value === '0');
            applyState(isOn);
          })
          .catch(function (err) {
            console.log('Error reading /status:', err);
          });
      }

      // First read immediately, then poll every 500 ms
      pollStatus();
      setInterval(pollStatus, 500);

      // Make "Clear" button work (onclick="clearLog()")
      window.clearLog = function () {
        if (activityLog) {
          activityLog.innerHTML =
            '<div class="text-center text-white/20 text-sm py-4">' +
              'Log cleared. Waiting for new events...' +
            '</div>';
        }
        toggleCount = 0;
        if (toggleCountEl) {
          toggleCountEl.textContent = '0';
        }
      };
    });
  </script>
</body>
</html>
)rawliteral";
// ========================================================

void handleRoot() {
  server.send_P(200, "text/html", index_html);
}

void handleStatus() {
  int switchState = digitalRead(SWITCH_PIN);
  server.send(200, "text/plain", String(switchState));
}

void setup() {
  Serial.begin(115200);

  pinMode(SWITCH_PIN, INPUT_PULLUP);

  WiFi.begin(ssid, password);
  Serial.print("Connecting");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/status", handleStatus);

  server.begin();
}

void loop() {
  server.handleClient();
}
