$ErrorActionPreference = 'Stop'
$root = Join-Path $PSScriptRoot '..'
$transport = Get-Content -Raw (Join-Path $root 'Hardware\I2C\I2C1.c')
$oled = Get-Content -Raw (Join-Path $root 'Hardware\OLED\OLED_Port.c')
foreach ($part in @('I2C1', 'GPIO_Pin_6 | GPIO_Pin_7', 'GPIO_Mode_AF_OD', 'I2C_TIMEOUT', 'I2C1_Init', 'I2C1_Write(uint8_t address', 'I2C1_Read(uint8_t address')) {
    if (-not $transport.Contains($part)) { throw "Shared I2C missing: $part" }
}
if (-not $oled.Contains('I2C1_Write')) { throw 'OLED Port does not use shared I2C.' }
Write-Host 'Shared I2C checks passed.'
