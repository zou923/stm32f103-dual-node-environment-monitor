$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot
$source = Get-Content -Raw (Join-Path $root 'Core\Src\dfu_memory.c')
foreach ($fragment in 'DFU_ADDRESS_IS_VALID', 'HAL_FLASHEx_Erase', 'HAL_FLASH_Program', 'FLASH_TYPEPROGRAM_HALFWORD') {
    if ($source -notlike "*$fragment*") { throw "Missing DFU Flash behavior: $fragment" }
}
