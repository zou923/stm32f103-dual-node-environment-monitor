$ErrorActionPreference = 'Stop'
$root = Join-Path $PSScriptRoot '..'
$main = Get-Content -Raw (Join-Path $root 'Core\Src\main.c')
$cmake = Get-Content -Raw (Join-Path $root 'CMakeLists.txt')
foreach ($part in @('#include "SHT30.h"', 'SHT30_Init();', 'SHT30_Read', 'Temp:', 'Humi:', 'SHT30 ERROR', '1000U', '500U')) {
    if (-not $main.Contains($part)) { throw "SHT30 application missing: $part" }
}
if (-not $cmake.Contains('Hardware/SHT30/SHT30.c')) { throw 'CMake does not compile SHT30.' }
Write-Host 'SHT30 OLED application checks passed.'
