$ErrorActionPreference = 'Stop'
$script = Get-Content -Raw (Join-Path $PSScriptRoot '..\scripts\flash.ps1')
if ($script -notlike '*init; halt; program*') { throw 'Flash script must halt the connected target directly before programming.' }
if ($script -like '*reset halt*') { throw 'Flash script must not use reset halt; this board times out with that reset strategy.' }
if ($script -notlike '*verify; reset run; exit*') { throw 'Flash script must reset separately after verification.' }
