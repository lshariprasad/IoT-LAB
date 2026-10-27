<?php
header('Content-Type: application/json; charset=utf-8');
header('Cache-Control: no-store, no-cache, must-revalidate');
require_once __DIR__ . '/config.php';
$state = [];
if (is_file(STATE_FILE)) {
    $raw = file_get_contents(STATE_FILE);
    $state = json_decode($raw ?: '{}', true) ?: [];
}
$last = intval($state['last_seen'] ?? 0);
$online = $last > 0 && (time() - $last) <= ONLINE_WINDOW_SECONDS;
echo json_encode([
    'ok' => true,
    'online' => $online,
    'spraying' => (bool)($state['spraying'] ?? false),
    'timer' => (bool)($state['timer'] ?? true),
    'interval' => intval($state['interval'] ?? 600),
    'duration' => intval($state['duration'] ?? 5),
    'next_seconds' => intval($state['next_seconds'] ?? 0),
    'elapsed_seconds' => intval($state['elapsed_seconds'] ?? 0),
    'last_seen' => $last
]);
