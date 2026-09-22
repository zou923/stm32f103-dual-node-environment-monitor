$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot
$header = Get-Content (Join-Path $root 'Hardware\NRF24L01\NRF24L01.h') -Raw
$source = Get-Content (Join-Path $root 'Hardware\NRF24L01\NRF24L01.c') -Raw
if ($header -notmatch 'NRF24L01_Init\s*\(\s*const uint8_t local_address\[5\]\s*,\s*const uint8_t peer_address\[5\]\s*\)') { throw 'Missing role-aware NRF24L01_Init signature.' }
if ($header -notmatch 'NRF24L01_ReadTelemetry') { throw 'Missing NRF24L01_ReadTelemetry declaration.' }
if ($source -notmatch 'NRF24L01_ReadTelemetry') { throw 'Missing NRF24L01_ReadTelemetry implementation.' }
if ($source -notmatch 'p\[0\]\s*!=\s*0x01U') { throw 'Telemetry packet type 0x01U is not validated.' }
Write-Output 'NRF master/slave protocol checks passed.'
