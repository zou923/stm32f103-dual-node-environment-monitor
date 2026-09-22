#ifndef DFU_MEMORY_H
#define DFU_MEMORY_H

#include <stdint.h>

int dfu_flash_erase_application(void);
int dfu_flash_write(uint32_t address, const uint8_t *data, uint32_t length);

#endif
