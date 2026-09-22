#include "usbd_dfu.h"
#include "dfu_memory.h"
#include "dfu_memory_map.h"

static uint16_t dfu_init(void) { return 0U; }
static uint16_t dfu_deinit(void) { return 0U; }
static uint16_t dfu_erase(uint32_t address)
{
    return (address == DFU_APPLICATION_START && dfu_flash_erase_application() == 0) ? 0U : 1U;
}
static uint16_t dfu_write(uint8_t *source, uint8_t *destination, uint32_t length)
{
    return dfu_flash_write((uint32_t)destination, source, length) == 0 ? 0U : 1U;
}
static uint8_t *dfu_read(uint8_t *source, uint8_t *destination, uint32_t length)
{
    for (uint32_t i = 0U; i < length; ++i) { destination[i] = source[i]; }
    return destination;
}
static uint16_t dfu_status(uint32_t address, uint8_t command, uint8_t *buffer)
{
    (void)address; (void)command;
    buffer[0] = 0U; buffer[1] = 50U; buffer[2] = 0U; buffer[3] = 0U;
    return 0U;
}

USBD_DFU_MediaTypeDef USBD_DFU_Flash_fops = {
    (uint8_t *)"@Internal Flash /0x08003000/52*001Kg",
    dfu_init, dfu_deinit, dfu_erase, dfu_write, dfu_read, dfu_status
};
