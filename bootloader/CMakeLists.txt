cmake_minimum_required(VERSION 3.20)
project(stm32f103_usb_dfu_bootloader LANGUAGES C ASM)

set(CUBE_F1_ROOT "${CMAKE_CURRENT_LIST_DIR}/../third_party/STM32CubeF1" CACHE PATH "STM32CubeF1 root")
set(USB_CORE "${CUBE_F1_ROOT}/Middlewares/ST/STM32_USB_Device_Library/Core")
set(USB_DFU "${CUBE_F1_ROOT}/Middlewares/ST/STM32_USB_Device_Library/Class/DFU")
set(HAL "${CUBE_F1_ROOT}/Drivers/STM32F1xx_HAL_Driver")

foreach(path
  "${USB_CORE}/Src/usbd_core.c"
  "${USB_DFU}/Src/usbd_dfu.c"
  "${HAL}/Src/stm32f1xx_hal_pcd.c")
  if(NOT EXISTS "${path}")
    message(FATAL_ERROR "Missing CubeF1 DFU component: ${path}")
  endif()
endforeach()

message(STATUS "CubeF1 USB DFU sources found; target integration follows.")

set(DFU_EXAMPLE "${CUBE_F1_ROOT}/Projects/STM3210C_EVAL/Applications/USB_Device/DFU_Standalone")
add_executable(stm32f103_usb_dfu_bootloader
  ../startup_stm32f103c8tx.s
  Core/Src/main.c
  Core/Src/usb_dfu.c
  Core/Src/usb_pcd.c
  Core/Src/dfu_memory.c
  Core/Src/usbd_dfu_flash.c
  "${CUBE_F1_ROOT}/Drivers/CMSIS/Device/ST/STM32F1xx/Source/Templates/system_stm32f1xx.c"
  "${DFU_EXAMPLE}/Src/usbd_desc.c"
  "${USB_CORE}/Src/usbd_core.c"
  "${USB_CORE}/Src/usbd_ctlreq.c"
  "${USB_CORE}/Src/usbd_ioreq.c"
  "${USB_DFU}/Src/usbd_dfu.c"
  "${HAL}/Src/stm32f1xx_hal.c"
  "${HAL}/Src/stm32f1xx_hal_cortex.c"
  "${HAL}/Src/stm32f1xx_hal_flash.c"
  "${HAL}/Src/stm32f1xx_hal_flash_ex.c"
  "${HAL}/Src/stm32f1xx_hal_gpio.c"
  "${HAL}/Src/stm32f1xx_hal_pcd.c"
  "${HAL}/Src/stm32f1xx_hal_pcd_ex.c"
  "${HAL}/Src/stm32f1xx_ll_usb.c"
  "${HAL}/Src/stm32f1xx_hal_rcc.c"
  "${HAL}/Src/stm32f1xx_hal_rcc_ex.c")
target_include_directories(stm32f103_usb_dfu_bootloader PRIVATE
  Core/Inc
  "${DFU_EXAMPLE}/Inc" "${USB_CORE}/Inc" "${USB_DFU}/Inc"
  "${HAL}/Inc" "${CUBE_F1_ROOT}/Drivers/CMSIS/Include"
  "${CUBE_F1_ROOT}/Drivers/CMSIS/Device/ST/STM32F1xx/Include")
target_compile_definitions(stm32f103_usb_dfu_bootloader PRIVATE STM32F103xB USE_HAL_DRIVER)
target_compile_options(stm32f103_usb_dfu_bootloader PRIVATE -mcpu=cortex-m3 -mthumb -Os -flto -ffunction-sections -fdata-sections)
target_link_options(stm32f103_usb_dfu_bootloader PRIVATE -mcpu=cortex-m3 -mthumb -flto -T${CMAKE_CURRENT_LIST_DIR}/STM32F103_BOOT.ld -nostartfiles --specs=nano.specs --specs=nosys.specs -Wl,--gc-sections -Wl,--print-memory-usage)
set_target_properties(stm32f103_usb_dfu_bootloader PROPERTIES SUFFIX ".elf")
