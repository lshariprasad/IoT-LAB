<?php
header('Content-Type: application/json; charset=utf-8');
header('Cache-Control: no-store');
require_once __DIR__ . '/config.php';
$token = $_SERVER['HTTP_X_API_TOKEN'] ?? '';
if (!hash_equals(API_TOKEN, $token)) {
    http_response_code(401);
    echo json_encode(['ok' => false, 'error' => 'Unauthorized']);
    exit;
}
if (!is_file(COMMAND_FILE)) {
    echo json_encode(['ok' => true, 'id' => 'none', 'action' => 'none']);
    exit;
}
$command = json_decode(file_get_contents(COMMAND_FILE) ?: '{}', true) ?: [];
// Commands older than 10 minutes are treated as stale.
if (time() - intval($command['created_at'] ?? 0) > 600) {
    echo json_encode(['ok' => true, 'id' => 'none', 'action' => 'none']);
    exit;
}
echo json_encode($command);
