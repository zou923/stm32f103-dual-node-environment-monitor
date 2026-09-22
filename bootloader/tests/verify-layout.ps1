$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot
$script = Get-Content -Raw (Join-Path $root 'STM32F103_BOOT.ld')
foreach ($fragment in 'ORIGIN = 0x08000000, LENGTH = 12K', 'ORIGIN = 0x20000000, LENGTH = 20K') {
    if ($script -notlike "*$fragment*") { throw "Missing linker layout: $fragment" }
}
