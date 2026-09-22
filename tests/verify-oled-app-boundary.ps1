$ErrorActionPreference = 'Stop'
$root = Join-Path $PSScriptRoot '..'
$main = Get-Content -Raw (Join-Path $root 'Core\Src\main.c')
$cmake = Get-Content -Raw (Join-Path $root 'CMakeLists.txt')
$required = @('#include "OLED.h"', 'OLED_Init();', 'OLED_ShowString')
foreach ($fragment in $required) {
    if (-not $main.Contains($fragment)) { throw "Application is missing: $fragment" }
}
if ($main.Contains('I2C1') -or $main.Contains('ssd1306')) { throw 'Application must not access OLED transport directly.' }
if (-not $cmake.Contains('Hardware/OLED/OLED.c') -or -not $cmake.Contains('Hardware/OLED/OLED_Port.c')) { throw 'CMake does not compile the OLED modules.' }
Write-Host 'OLED application boundary checks passed.'
