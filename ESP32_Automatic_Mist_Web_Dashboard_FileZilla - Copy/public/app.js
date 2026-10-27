const API = 'api';
const $ = (id) => document.getElementById(id);
let busy = false;

function showMessage(text, error = false) {
  $('message').textContent = text;
  $('message').style.color = error ? '#ff9aa8' : '#40d6a0';
}
async function apiPost(path, data = {}) {
  const response = await fetch(`${API}/${path}`, {
    method: 'POST',
    headers: {'Content-Type': 'application/json'},
    body: JSON.stringify(data),
    cache: 'no-store'
  });
  const payload = await response.json().catch(() => ({}));
  if (!response.ok || payload.ok === false) throw new Error(payload.error || 'Request failed');
  return payload;
}
async function command(action) {
  if (busy) return;
  busy = true;
  $('startBtn').disabled = true;
  $('stopBtn').disabled = true;
  try {
    await apiPost('command.php', {action});
    showMessage(action === 'start' ? 'Start command queued.' : 'Stop command queued.');
    await refresh();
  } catch (err) {
    showMessage(err.message, true);
  } finally {
    busy = false;
    $('startBtn').disabled = false;
    $('stopBtn').disabled = false;
  }
}
$('startBtn').addEventListener('click', () => command('start'));
$('stopBtn').addEventListener('click', () => command('stop'));

$('settingsForm').addEventListener('submit', async (event) => {
  event.preventDefault();
  const interval = Number($('interval').value);
  const duration = Number($('duration').value);
  if (!Number.isInteger(interval) || interval < 10 || interval > 86400) {
    showMessage('Interval must be 10–86400 seconds.', true); return;
  }
  if (!Number.isInteger(duration) || duration < 1 || duration > 60) {
    showMessage('Duration must be 1–60 seconds.', true); return;
  }
  try {
    await apiPost('command.php', {
      action: 'settings', interval, duration,
      timer: $('timerEnabled').checked
    });
    showMessage('Settings command queued for ESP32.');
  } catch (err) { showMessage(err.message, true); }
});

async function refresh() {
  try {
    const response = await fetch(`${API}/state.php`, {cache:'no-store'});
    if (!response.ok) throw new Error('API unavailable');
    const s = await response.json();
    $('connection').textContent = s.online ? 'ESP32 online' : 'ESP32 not reporting';
    $('connection').className = `pill ${s.online ? 'online' : 'offline'}`;
    $('status').textContent = s.spraying ? 'SPRAYING' : 'STOPPED';
    $('status').className = `status ${s.spraying ? 'running' : ''}`;
    $('statusDot').className = `dot ${s.spraying ? 'on' : ''}`;
    $('statusMessage').textContent = s.online ? 'Latest status received from ESP32' : 'Waiting for the ESP32 to report in';
    $('nextCycle').textContent = s.timer ? `${s.next_seconds ?? '—'} sec` : 'Timer off';
    $('elapsed').textContent = `${s.elapsed_seconds ?? 0} sec`;
    if (s.interval !== undefined) $('interval').value = s.interval;
    if (s.duration !== undefined) $('duration').value = s.duration;
    if (s.timer !== undefined) $('timerEnabled').checked = !!s.timer;
  } catch (_) {
    $('connection').textContent = 'API offline';
    $('connection').className = 'pill offline';
    $('statusMessage').textContent = 'Cannot reach the hosting API. Check upload and PHP configuration.';
  }
}
refresh();
setInterval(refresh, 3000);
