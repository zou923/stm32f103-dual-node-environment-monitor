#include "stm32f1xx_hal.h"
#include "usb_dfu.h"
#include "usbd_core.h"
#include "usbd_dfu.h"

static volatile int g_usb_enumerated;
volatile uint32_t g_usb_init_status;
USBD_HandleTypeDef g_dfu_device;
extern USBD_DescriptorsTypeDef DFU_Desc;
extern USBD_DFU_MediaTypeDef USBD_DFU_Flash_fops;

int usb_dfu_init(void)
{
    RCC_OscInitTypeDef osc = {0};
    RCC_ClkInitTypeDef clk = {0};
    RCC_PeriphCLKInitTypeDef usbclk = {0};

    HAL_Init();
    g_usb_init_status = 1U;
    osc.OscillatorType = RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_HSI;
    /* HSI is the active reset clock until PLL/HSE becomes SYSCLK. */
    osc.HSIState = RCC_HSI_ON;
    osc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    osc.HSEState = RCC_HSE_ON;
    osc.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    osc.PLL.PLLState = RCC_PLL_ON;
    osc.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    osc.PLL.PLLMUL = RCC_PLL_MUL9;
    if (HAL_RCC_OscConfig(&osc) != HAL_OK) { g_usb_init_status = 2U; return 0; }
    g_usb_init_status = 3U;
    clk.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    clk.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    clk.AHBCLKDivider = RCC_SYSCLK_DIV1;
    clk.APB1CLKDivider = RCC_HCLK_DIV2;
    clk.APB2CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&clk, FLASH_LATENCY_2) != HAL_OK) { g_usb_init_status = 4U; return 0; }
    g_usb_init_status = 5U;
    usbclk.PeriphClockSelection = RCC_PERIPHCLK_USB;
    usbclk.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
    if (HAL_RCCEx_PeriphCLKConfig(&usbclk) != HAL_OK) { g_usb_init_status = 6U; return 0; }
    if (USBD_Init(&g_dfu_device, &DFU_Desc, 0U) != USBD_OK) { g_usb_init_status = 7U; return 0; }
    if (USBD_RegisterClass(&g_dfu_device, &USBD_DFU) != USBD_OK) { g_usb_init_status = 8U; return 0; }
    if (USBD_DFU_RegisterMedia(&g_dfu_device, &USBD_DFU_Flash_fops) != USBD_OK) { g_usb_init_status = 9U; return 0; }
    if (USBD_Start(&g_dfu_device) != USBD_OK) { g_usb_init_status = 10U; return 0; }
    g_usb_enumerated = 0;
    g_usb_init_status = 11U;
    return 1;
}

int usb_is_enumerated(void)
{
    return g_usb_enumerated;
}

void usb_dfu_host_detected(void)
{
    g_usb_enumerated = 1;
}
