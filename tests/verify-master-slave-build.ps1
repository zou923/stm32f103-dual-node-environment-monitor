$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot
$cmake = Get-Content (Join-Path $root 'CMakeLists.txt') -Raw
$tasks = Get-Content (Join-Path $root '.vscode\tasks.json') -Raw
$build = Get-Content (Join-Path $root 'scripts\build.ps1') -Raw
$flash = Get-Content (Join-Path $root 'scripts\flash.ps1') -Raw
foreach ($token in 'stm32f103-master', 'stm32f103-slave', '-Role', 'Flash Master', 'Flash Slave') {
 if (($cmake + $tasks + $build + $flash) -notlike "*$token*") { throw "Missing dual-firmware token: $token" }
}
Get-Content (Join-Path $root '.vscode\tasks.json') -Raw | ConvertFrom-Json | Out-Null
Write-Output 'Master/slave build checks passed.'
