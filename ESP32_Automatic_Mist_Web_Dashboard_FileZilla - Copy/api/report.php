<?php
header('Content-Type: application/json; charset=utf-8');
header('Cache-Control: no-store');
require_once __DIR__ . '/config.php';
function report_fail($message, $code = 400) {
    http_response_code($code);
    echo json_encode(['ok' => false, 'error' => $message]);
    exit;
}
$token = $_SERVER['HTTP_X_API_TOKEN'] ?? '';
if (!hash_equals(API_TOKEN, $token)) report_fail('Unauthorized', 401);
$input = json_decode(file_get_contents('php://input'), true);
if (!is_array($input)) report_fail('Invalid JSON');
$state = [
    'spraying' => (bool)($input['spraying'] ?? false),
    'timer' => (bool)($input['timer'] ?? true),
    'interval' => max(10, min(86400, intval($input['interval'] ?? 600))),
    'duration' => max(1, min(60, intval($input['duration'] ?? 5))),
    'next_seconds' => max(0, intval($input['next_seconds'] ?? 0)),
    'elapsed_seconds' => max(0, intval($input['elapsed_seconds'] ?? 0)),
    'last_seen' => time()
];
$tmp = STATE_FILE . '.tmp';
if (file_put_contents($tmp, json_encode($state), LOCK_EX) === false || !rename($tmp, STATE_FILE)) {
    report_fail('Cannot write state file; check PHP folder permissions', 500);
}
echo json_encode(['ok' => true]);
