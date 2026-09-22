$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot
$main = Get-Content -Raw (Join-Path $root 'Core\Src\main.c')
foreach ($fragment in '0x08003000U', '0x20000000U', '__set_MSP', 'SCB->VTOR', 'DFU_WAIT_MS', 'usb_is_enumerated') {
    if ($main -notlike "*$fragment*") { throw "Missing bootloader safety logic: $fragment" }
}
