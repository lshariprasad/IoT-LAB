<?php
header('Content-Type: application/json; charset=utf-8');
header('Cache-Control: no-store');
require_once __DIR__ . '/config.php';
function fail_request($message, $code = 400) {
    http_response_code($code);
    echo json_encode(['ok' => false, 'error' => $message]);
    exit;
}
$token = $_SERVER['HTTP_X_API_TOKEN'] ?? '';
// Browser commands are allowed only after a simple same-origin check.
// For real public deployment, add user authentication and CSRF protection.
$origin = $_SERVER['HTTP_ORIGIN'] ?? '';
$host = $_SERVER['HTTP_HOST'] ?? '';
if ($origin && parse_url($origin, PHP_URL_HOST) !== preg_replace('/:\d+$/', '', $host)) {
    fail_request('Cross-origin request rejected', 403);
}
$input = json_decode(file_get_contents('php://input'), true);
if (!is_array($input)) fail_request('Invalid JSON');
$action = $input['action'] ?? '';
$command = ['action' => $action, 'created_at' => time(), 'id' => bin2hex(random_bytes(8))];
if ($action === 'start' || $action === 'stop') {
    // accepted
} elseif ($action === 'settings') {
    $interval = filter_var($input['interval'] ?? null, FILTER_VALIDATE_INT);
    $duration = filter_var($input['duration'] ?? null, FILTER_VALIDATE_INT);
    if ($interval === false || $interval < 10 || $interval > 86400) fail_request('Interval must be 10–86400 seconds');
    if ($duration === false || $duration < 1 || $duration > 60) fail_request('Duration must be 1–60 seconds');
    $command['interval'] = $interval;
    $command['duration'] = $duration;
    $command['timer'] = (bool)($input['timer'] ?? true);
} else {
    fail_request('Unknown action');
}
// Atomic write reduces the chance of a partially written command file.
$tmp = COMMAND_FILE . '.tmp';
if (file_put_contents($tmp, json_encode($command), LOCK_EX) === false || !rename($tmp, COMMAND_FILE)) {
    fail_request('Cannot write command file. Check PHP folder permissions.', 500);
}
echo json_encode(['ok' => true, 'message' => 'Command queued', 'id' => $command['id']]);
