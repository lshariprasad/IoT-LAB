<?php
declare(strict_types=1);
require_once __DIR__ . '/config.php';
if (($_SERVER['REQUEST_METHOD'] ?? '') !== 'POST') { header('Allow: POST'); json_response(['ok'=>false,'error'=>'POST required'],405); }
$body=json_decode(file_get_contents('php://input') ?: '',true);
$command=is_array($body) ? ($body['command'] ?? '') : '';
if (!in_array($command,['auto','on','off'],true)) json_response(['ok'=>false,'error'=>'Use auto, on, or off'],400);
$current=read_json_file(STATUS_FILE);
if (!isset($current['last_seen']) || time()-(int)$current['last_seen']>ONLINE_TIMEOUT_SECONDS) json_response(['ok'=>false,'error'=>'Device is offline; command not sent'],409);
$old=read_json_file(COMMAND_FILE);
$id=(int)($old['id'] ?? 0)+1;
if (!write_json_file(COMMAND_FILE,['id'=>$id,'command'=>$command,'created_at'=>time()])) json_response(['ok'=>false,'error'=>'Cannot save command; check hosting permissions'],500);
json_response(['ok'=>true,'message'=>'Command queued. Device will apply it on its next heartbeat.','command_id'=>$id]);
