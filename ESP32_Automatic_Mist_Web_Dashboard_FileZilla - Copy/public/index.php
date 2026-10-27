<?php
?><!doctype html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width,initial-scale=1">
  <meta name="theme-color" content="#101827">
  <title>ESP32 Mist Control Dashboard</title>
  <link rel="stylesheet" href="style.css">
</head>
<body>
<main class="shell">
  <header class="top">
    <div class="logo">💧</div>
    <div>
      <p class="eyebrow">ESP32 · IOT CONTROL</p>
      <h1>Automatic Mist Sprayer</h1>
      <p class="sub">Remote control and timer scheduling</p>
    </div>
    <span id="connection" class="pill offline">Connecting…</span>
  </header>

  <section class="grid">
    <article class="card hero">
      <div class="card-heading">
        <div><p class="eyebrow">LIVE STATUS</p><h2>Sprayer</h2></div>
        <span id="statusDot" class="dot"></span>
      </div>
      <div id="status" class="status">Loading…</div>
      <p id="statusMessage" class="muted">Waiting for ESP32 status</p>
      <div class="actions">
        <button id="startBtn" class="btn primary">Start spray</button>
        <button id="stopBtn" class="btn danger">Stop spray</button>
      </div>
      <div class="stats">
        <div><span class="muted">Next cycle</span><strong id="nextCycle">—</strong></div>
        <div><span class="muted">Spray elapsed</span><strong id="elapsed">—</strong></div>
      </div>
    </article>

    <article class="card">
      <p class="eyebrow">AUTOMATION</p>
      <h2>Timer settings</h2>
      <form id="settingsForm">
        <label for="interval">Interval between sprays <span>(seconds)</span></label>
        <input id="interval" type="number" min="10" max="86400" value="600" required>
        <p class="hint">Example: 600 seconds = 10 minutes</p>

        <label for="duration">Spray duration <span>(seconds)</span></label>
        <input id="duration" type="number" min="1" max="60" value="5" required>
        <p class="hint">Maximum 60 seconds per cycle in this firmware</p>

        <label class="toggle-row" for="timerEnabled">
          <span><strong>Automatic timer</strong><small>Repeat sprays on schedule</small></span>
          <input id="timerEnabled" type="checkbox" checked>
        </label>
        <button class="btn secondary full" type="submit">Save settings</button>
      </form>
      <p id="message" class="message" role="status"></p>
    </article>
  </section>

  <section class="card how">
    <p class="eyebrow">SYSTEM FLOW</p>
    <div class="flow">
      <div><span>01</span><strong>Web dashboard</strong><small>Phone or computer</small></div>
      <b>→</b>
      <div><span>02</span><strong>Hostinger API</strong><small>HTTPS command queue</small></div>
      <b>→</b>
      <div><span>03</span><strong>ESP32 + relay</strong><small>Controls the pump</small></div>
    </div>
  </section>

  <footer>Keep electronics dry. Test relay logic with the pump disconnected first.</footer>
</main>
<script src="app.js"></script>
</body>
</html>
