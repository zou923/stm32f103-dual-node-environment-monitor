#ifndef DFU_MEMORY_MAP_H
#define DFU_MEMORY_MAP_H

#define DFU_APPLICATION_START 0x08003000U
#define DFU_APPLICATION_END   0x08010000U
#define DFU_FLASH_PAGE_SIZE   1024U

#define DFU_ADDRESS_IS_VALID(address, length) \
    ((address) >= DFU_APPLICATION_START && \
     (length) <= (DFU_APPLICATION_END - (address)))

#endif
