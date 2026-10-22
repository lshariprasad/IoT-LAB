<?php
declare(strict_types=1);
require_once __DIR__ . '/config.php';
$status=read_json_file(STATUS_FILE);
if (!isset($status['last_seen']) || !is_numeric($status['last_seen'])) json_response(['ok'=>true,'online'=>false,'last_seen'=>null,'age_seconds'=>null,'gasDetected'=>false,'fanOn'=>false,'mode'=>'AUTO']);
$age=max(0,time()-(int)$status['last_seen']);
json_response([
 'ok'=>true,'online'=>$age<=ONLINE_TIMEOUT_SECONDS,
 'last_seen'=>(int)$status['last_seen'],'age_seconds'=>$age,
 'gasDetected'=>($status['gasDetected'] ?? false)===true,
 'fanOn'=>($status['fanOn'] ?? false)===true,
 'mode'=>$status['mode'] ?? 'AUTO'
]);
