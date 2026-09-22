$ErrorActionPreference = 'Stop'

$projectRoot = Split-Path -Parent $PSScriptRoot

& cmake --fresh -S $projectRoot -B "$projectRoot\build" -G Ninja `
    "-DCMAKE_TOOLCHAIN_FILE=$projectRoot\cmake\arm-none-eabi-gcc.cmake"
if ($LASTEXITCODE -ne 0) {
    throw 'CMake configure failed.'
}

& cmake --build "$projectRoot\build"
if ($LASTEXITCODE -ne 0) {
    throw 'CMake build failed.'
}

foreach ($artifactName in 'stm32f103c8t6.elf', 'stm32f103c8t6.hex', 'stm32f103c8t6.bin') {
    $artifactPath = Join-Path "$projectRoot\build" $artifactName
    if (-not (Test-Path -LiteralPath $artifactPath) -or (Get-Item -LiteralPath $artifactPath).Length -eq 0) {
        throw "Missing or empty build artifact: $artifactName"
    }
}

$mainSource = Get-Content -Raw (Join-Path $projectRoot 'Core\Src\main.c')
foreach ($fragment in '__HAL_RCC_GPIOC_CLK_ENABLE', 'GPIO_PIN_13', 'GPIO_MODE_OUTPUT_PP', 'HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET)') {
    if ($mainSource -notlike "*$fragment*") {
        throw "PC13 configuration is missing: $fragment"
    }
}
