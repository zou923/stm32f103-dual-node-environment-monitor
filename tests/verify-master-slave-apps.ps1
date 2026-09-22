$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot
$masterPath = Join-Path $root 'Core\Src\main_master.c'
$slavePath = Join-Path $root 'Core\Src\main_slave.c'
if (!(Test-Path $masterPath) -or !(Test-Path $slavePath)) { throw 'Missing master or slave application source.' }
$master = Get-Content $masterPath -Raw
$slave = Get-Content $slavePath -Raw
if ($master -notmatch 'OLED_Init' -or $master -notmatch 'Buzzer_Init' -or $master -notmatch 'NRF24L01_ReadTelemetry') { throw 'Master application boundary is incomplete.' }
if ($slave -notmatch 'NRF24L01_SendTelemetry') { throw 'Slave does not transmit telemetry.' }
if ($slave -match 'OLED_|Buzzer_') { throw 'Slave must not access OLED or Buzzer.' }
if ($master -notmatch 'S OFFLINE') { throw 'Master lacks remote offline display.' }
Write-Output 'Master/slave application checks passed.'
