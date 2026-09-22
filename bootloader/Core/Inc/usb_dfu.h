#ifndef USB_DFU_H
#define USB_DFU_H

#include <stdint.h>

int usb_dfu_init(void);
int usb_is_enumerated(void);
void usb_dfu_host_detected(void);
extern volatile uint32_t g_usb_init_status;

#endif
