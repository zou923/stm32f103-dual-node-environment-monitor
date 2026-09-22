#include "stm32f1xx_hal.h"
#include "dfu_memory_map.h"

int dfu_flash_erase_application(void)
{
    FLASH_EraseInitTypeDef erase = {0};
    uint32_t page_error = 0U;

    erase.TypeErase = FLASH_TYPEERASE_PAGES;
    erase.PageAddress = DFU_APPLICATION_START;
    erase.NbPages = (DFU_APPLICATION_END - DFU_APPLICATION_START) / DFU_FLASH_PAGE_SIZE;
    HAL_FLASH_Unlock();
    return HAL_FLASHEx_Erase(&erase, &page_error) == HAL_OK ? 0 : -1;
}

int dfu_flash_write(uint32_t address, const uint8_t *data, uint32_t length)
{
    if ((data == 0) || ((length & 1U) != 0U) || !DFU_ADDRESS_IS_VALID(address, length)) {
        return -1;
    }
    for (uint32_t offset = 0U; offset < length; offset += 2U) {
        uint16_t value = (uint16_t)data[offset] | ((uint16_t)data[offset + 1U] << 8U);
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, address + offset, value) != HAL_OK) {
            return -1;
        }
    }
    return 0;
}
