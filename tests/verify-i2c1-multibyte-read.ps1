$ErrorActionPreference = 'Stop'
$source = Get-Content -Raw (Join-Path $PSScriptRoot '..\Hardware\I2C\I2C1.c')
foreach ($part in @('length > 3U', 'I2C_EVENT_MASTER_BYTE_RECEIVED', 'I2C_FLAG_BTF', 'I2C_AcknowledgeConfig(I2C1, DISABLE)', 'I2C_GenerateSTOP(I2C1, ENABLE)')) {
    if (-not $source.Contains($part)) { throw "I2C multi-byte read missing: $part" }
}
Write-Host 'I2C multi-byte receive checks passed.'
