<?php
declare(strict_types=1);
const DEVICE_TOKEN = 'BGZYwwBdHzxe6x-7F4D__s0RvN2jHEmf0Zq0VEV-qPA';
const ONLINE_TIMEOUT_SECONDS = 60;
const STATUS_FILE = __DIR__ . '/device_status.json';
const COMMAND_FILE = __DIR__ . '/device_command.json';
function json_response(array $data, int $status = 200): void {
  http_response_code($status);
  header('Content-Type: application/json; charset=utf-8');
  header('Cache-Control: no-store, no-cache, must-revalidate, max-age=0');
  header('X-Content-Type-Options: nosniff');
  echo json_encode($data, JSON_UNESCAPED_SLASHES);
  exit;
}
function read_json_file(string $path): array {
  if (!is_file($path)) return [];
  $data = json_decode(file_get_contents($path) ?: '', true);
  return is_array($data) ? $data : [];
}
function write_json_file(string $path, array $data): bool {
  $json = json_encode($data, JSON_UNESCAPED_SLASHES);
  return $json !== false && file_put_contents($path, $json, LOCK_EX) !== false;
}
