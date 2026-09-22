[CmdletBinding()]
param([ValidateSet('Master','Slave')][string]$Role = 'Master')

$ErrorActionPreference = 'Stop'
$projectRoot = [System.IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
& cmake --fresh -S $projectRoot -B (Join-Path $projectRoot 'build') -G Ninja "-DCMAKE_TOOLCHAIN_FILE=$projectRoot\cmake\arm-none-eabi-gcc.cmake"
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
$target = if ($Role -eq 'Master') { 'stm32f103-master' } else { 'stm32f103-slave' }
& cmake --build (Join-Path $projectRoot 'build') --target $target
exit $LASTEXITCODE
