$ErrorActionPreference = 'Stop'
$source = Get-Content -Raw (Join-Path $PSScriptRoot '..\Hardware\OLED\OLED.c')
$font = Get-Content -Raw (Join-Path $PSScriptRoot '..\Hardware\OLED\OLED_Font.h')
$required = @('OLED_Init', 'OLED_Clear', 'OLED_ShowString', '0x8DU', '0x14U', 'OLED_Port_WriteCommand')
foreach ($fragment in $required) {
    if (-not $source.Contains($fragment)) { throw "OLED display is missing: $fragment" }
}
if (-not $font.Contains('OLED_F8x16')) { throw 'OLED 8x16 font is missing.' }
Write-Host 'OLED display source checks passed.'
