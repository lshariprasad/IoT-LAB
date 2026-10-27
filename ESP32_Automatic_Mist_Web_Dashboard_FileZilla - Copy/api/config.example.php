<?php
// Copy this file to config.php and replace the token with a long random secret.
// Keep this secret identical to API_TOKEN in the ESP32 config.h.
define('API_TOKEN', 'REPLACE_WITH_A_LONG_RANDOM_SECRET_AT_LEAST_32_CHARS');
define('STATE_FILE', __DIR__ . '/state.json');
define('COMMAND_FILE', __DIR__ . '/command.json');
define('ONLINE_WINDOW_SECONDS', 45);
