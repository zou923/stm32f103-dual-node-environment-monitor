[CmdletBinding()]
param([ValidateSet('Master','Slave')][string]$Role = 'Master')

$ErrorActionPreference = 'Stop'
$projectRoot = [System.IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
& powershell -NoProfile -ExecutionPolicy Bypass -File (Join-Path $PSScriptRoot 'build.ps1') -Role $Role
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

$openOcd = 'C:\Users\Zou\Tools\stm32h750\xpack-openocd-0.12.0-7-win32-x64\xpack-openocd-0.12.0-7\bin\openocd.exe'
$config = Join-Path $projectRoot 'openocd\cmsis-dap-stm32f103.cfg'
$target = if ($Role -eq 'Master') { 'stm32f103-master' } else { 'stm32f103-slave' }
$elf = Join-Path $projectRoot "build\$target.elf"
& $openOcd -f $config -c "init; halt; program {$elf} verify; reset run; exit"
if ($LASTEXITCODE -ne 0) { throw "OpenOCD flash failed (exit $LASTEXITCODE). Check target power and SWD wiring." }
