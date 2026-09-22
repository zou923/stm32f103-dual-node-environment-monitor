$ErrorActionPreference = 'Stop'
$source = Get-Content -Raw (Join-Path $PSScriptRoot '..\Hardware\OLED\OLED_Port.c')
$required = @('OLED_ADDRESS 0x3CU', 'I2C1.h', 'I2C1_Init', 'I2C1_Write', 'OLED_Port_Init', 'OLED_Port_WriteCommand', 'OLED_Port_WriteData')
foreach ($fragment in $required) {
    if (-not $source.Contains($fragment)) { throw "OLED port is missing: $fragment" }
}
Write-Host 'OLED port source checks passed.'
