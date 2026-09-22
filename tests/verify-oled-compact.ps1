$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot
$header = Get-Content (Join-Path $root 'Hardware\OLED\OLED.h') -Raw
$source = Get-Content (Join-Path $root 'Hardware\OLED\OLED.c') -Raw
if ($header -notmatch 'OLED_ShowString8') { throw 'Missing compact OLED public API.' }
if ($source -notmatch 'OLED_ShowString8') { throw 'Missing compact OLED renderer.' }
if ($source -notmatch 'OLED_CompactGlyph') { throw 'Missing compact glyph renderer.' }
Write-Output 'Compact OLED checks passed.'
