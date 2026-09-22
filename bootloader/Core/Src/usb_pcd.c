#include "stm32f1xx_hal.h"
#include "usbd_core.h"
#include "usb_dfu.h"

PCD_HandleTypeDef hpcd_USB_FS;
extern USBD_HandleTypeDef g_dfu_device;

void HAL_PCD_SetupStageCallback(PCD_HandleTypeDef *hpcd) { USBD_LL_SetupStage((USBD_HandleTypeDef *)hpcd->pData, (uint8_t *)hpcd->Setup); }
void HAL_PCD_DataOutStageCallback(PCD_HandleTypeDef *hpcd, uint8_t ep) { USBD_LL_DataOutStage((USBD_HandleTypeDef *)hpcd->pData, ep, hpcd->OUT_ep[ep].xfer_buff); }
void HAL_PCD_DataInStageCallback(PCD_HandleTypeDef *hpcd, uint8_t ep) { USBD_LL_DataInStage((USBD_HandleTypeDef *)hpcd->pData, ep, hpcd->IN_ep[ep].xfer_buff); }
void HAL_PCD_ResetCallback(PCD_HandleTypeDef *hpcd) { usb_dfu_host_detected(); USBD_LL_SetSpeed((USBD_HandleTypeDef *)hpcd->pData, USBD_SPEED_FULL); USBD_LL_Reset((USBD_HandleTypeDef *)hpcd->pData); }

void HAL_PCD_MspInit(PCD_HandleTypeDef *hpcd)
{
    GPIO_InitTypeDef gpio = {0};
    if (hpcd->Instance != USB) { return; }
    __HAL_RCC_GPIOA_CLK_ENABLE();
    gpio.Pin = GPIO_PIN_11 | GPIO_PIN_12;
    gpio.Mode = GPIO_MODE_AF_INPUT;
    gpio.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &gpio);
    __HAL_RCC_USB_CLK_ENABLE();
    HAL_NVIC_SetPriority(USB_LP_CAN1_RX0_IRQn, 0U, 0U);
    HAL_NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
}

void USB_LP_CAN1_RX0_IRQHandler(void)
{
    HAL_PCD_IRQHandler(&hpcd_USB_FS);
}

USBD_StatusTypeDef USBD_LL_Init(USBD_HandleTypeDef *pdev)
{
    hpcd_USB_FS.Instance = USB;
    hpcd_USB_FS.pData = pdev;
    pdev->pData = &hpcd_USB_FS;
    hpcd_USB_FS.Init.dev_endpoints = 8U;
    hpcd_USB_FS.Init.speed = PCD_SPEED_FULL;
    hpcd_USB_FS.Init.phy_itface = PCD_PHY_EMBEDDED;
    hpcd_USB_FS.Init.low_power_enable = 0U;
    if (HAL_PCD_Init(&hpcd_USB_FS) != HAL_OK) { return USBD_FAIL; }
    (void)HAL_PCDEx_PMAConfig(&hpcd_USB_FS, 0x00U, PCD_SNG_BUF, 0x18U);
    (void)HAL_PCDEx_PMAConfig(&hpcd_USB_FS, 0x80U, PCD_SNG_BUF, 0x58U);
    return USBD_OK;
}

USBD_StatusTypeDef USBD_LL_DeInit(USBD_HandleTypeDef *pdev) { return HAL_PCD_DeInit((PCD_HandleTypeDef *)pdev->pData) == HAL_OK ? USBD_OK : USBD_FAIL; }
USBD_StatusTypeDef USBD_LL_Start(USBD_HandleTypeDef *pdev) { return HAL_PCD_Start((PCD_HandleTypeDef *)pdev->pData) == HAL_OK ? USBD_OK : USBD_FAIL; }
USBD_StatusTypeDef USBD_LL_Stop(USBD_HandleTypeDef *pdev) { return HAL_PCD_Stop((PCD_HandleTypeDef *)pdev->pData) == HAL_OK ? USBD_OK : USBD_FAIL; }
USBD_StatusTypeDef USBD_LL_OpenEP(USBD_HandleTypeDef *pdev, uint8_t ep, uint8_t type, uint16_t size) { return HAL_PCD_EP_Open((PCD_HandleTypeDef *)pdev->pData, ep, size, type) == HAL_OK ? USBD_OK : USBD_FAIL; }
USBD_StatusTypeDef USBD_LL_CloseEP(USBD_HandleTypeDef *pdev, uint8_t ep) { return HAL_PCD_EP_Close((PCD_HandleTypeDef *)pdev->pData, ep) == HAL_OK ? USBD_OK : USBD_FAIL; }
USBD_StatusTypeDef USBD_LL_FlushEP(USBD_HandleTypeDef *pdev, uint8_t ep) { return HAL_PCD_EP_Flush((PCD_HandleTypeDef *)pdev->pData, ep) == HAL_OK ? USBD_OK : USBD_FAIL; }
USBD_StatusTypeDef USBD_LL_StallEP(USBD_HandleTypeDef *pdev, uint8_t ep) { return HAL_PCD_EP_SetStall((PCD_HandleTypeDef *)pdev->pData, ep) == HAL_OK ? USBD_OK : USBD_FAIL; }
USBD_StatusTypeDef USBD_LL_ClearStallEP(USBD_HandleTypeDef *pdev, uint8_t ep) { return HAL_PCD_EP_ClrStall((PCD_HandleTypeDef *)pdev->pData, ep) == HAL_OK ? USBD_OK : USBD_FAIL; }
uint8_t USBD_LL_IsStallEP(USBD_HandleTypeDef *pdev, uint8_t ep) { PCD_HandleTypeDef *pcd = (PCD_HandleTypeDef *)pdev->pData; return (ep & 0x80U) ? pcd->IN_ep[ep & 0x7FU].is_stall : pcd->OUT_ep[ep].is_stall; }
USBD_StatusTypeDef USBD_LL_SetUSBAddress(USBD_HandleTypeDef *pdev, uint8_t address) { return HAL_PCD_SetAddress((PCD_HandleTypeDef *)pdev->pData, address) == HAL_OK ? USBD_OK : USBD_FAIL; }
USBD_StatusTypeDef USBD_LL_Transmit(USBD_HandleTypeDef *pdev, uint8_t ep, uint8_t *data, uint16_t size) { return HAL_PCD_EP_Transmit((PCD_HandleTypeDef *)pdev->pData, ep, data, size) == HAL_OK ? USBD_OK : USBD_FAIL; }
USBD_StatusTypeDef USBD_LL_PrepareReceive(USBD_HandleTypeDef *pdev, uint8_t ep, uint8_t *data, uint16_t size) { return HAL_PCD_EP_Receive((PCD_HandleTypeDef *)pdev->pData, ep, data, size) == HAL_OK ? USBD_OK : USBD_FAIL; }
uint32_t USBD_LL_GetRxDataSize(USBD_HandleTypeDef *pdev, uint8_t ep) { return HAL_PCD_EP_GetRxCount((PCD_HandleTypeDef *)pdev->pData, ep); }
void USBD_LL_Delay(uint32_t ms) { HAL_Delay(ms); }
void *USBD_static_malloc(uint32_t size) { static uint32_t memory[64]; (void)size; return memory; }
void USBD_static_free(void *ptr) { (void)ptr; }
