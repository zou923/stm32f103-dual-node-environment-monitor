$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot
$source = Get-Content (Join-Path $root 'Core\Src\main_master.c') -Raw
if ($source -notmatch '#define LIGHT_ALARM_LX 500U') { throw 'Alarm threshold is not 500 lx.' }
if ($source -notmatch 'remote_valid.*remote_l.*LIGHT_ALARM_LX') { throw 'Remote light is not part of the alarm condition.' }
Write-Output 'Dual-light alarm checks passed.'
