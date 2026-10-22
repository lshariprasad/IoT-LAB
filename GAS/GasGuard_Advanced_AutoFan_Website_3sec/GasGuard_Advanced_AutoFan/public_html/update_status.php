<?php
declare(strict_types=1);
require_once __DIR__ . '/config.php';
if (($_SERVER['REQUEST_METHOD'] ?? '') !== 'POST') { header('Allow: POST'); json_response(['ok'=>false,'error'=>'POST required'],405); }
$provided = $_SERVER['HTTP_X_DEVICE_TOKEN'] ?? '';
if (!hash_equals(DEVICE_TOKEN, $provided)) json_response(['ok'=>false,'error'=>'Unauthorized token'],401);
$input = json_decode(file_get_contents('php://input') ?: '', true);
if (!is_array($input)) json_response(['ok'=>false,'error'=>'Invalid JSON'],400);

$now=time();
$status=[
 'last_seen'=>$now,
 'gasDetected'=>($input['gasDetected'] ?? false) === true,
 'fanOn'=>($input['fanOn'] ?? false) === true,
 'wifi'=>true,
 'mode'=>in_array(($input['mode'] ?? 'AUTO'),['AUTO','MANUAL'],true) ? $input['mode'] : 'AUTO',
 'firmware'=>substr((string)($input['firmware'] ?? '1.0'),0,30)
];
if (!write_json_file(STATUS_FILE,$status)) json_response(['ok'=>false,'error'=>'Cannot write status; check hosting permissions'],500);
$command=read_json_file(COMMAND_FILE);
json_response(['ok'=>true,'server_time'=>gmdate('c',$now),'command'=>$command['command'] ?? 'auto','command_id'=>$command['id'] ?? 0]);
