$ErrorActionPreference = 'Stop'
$source = Get-Content -Raw (Join-Path $PSScriptRoot '..\Hardware\SHT30\SHT30.c')
foreach ($part in @('SHT30_ADDRESS 0x44U', '0x2CU', '0x06U', '0x31U', '0xFFU', 'I2C1_Read', '1750', '450', '1000', 'sample[2]', 'sample[5]')) {
    if (-not $source.Contains($part)) { throw "SHT30 driver missing: $part" }
}
Write-Host 'SHT30 source checks passed.'
