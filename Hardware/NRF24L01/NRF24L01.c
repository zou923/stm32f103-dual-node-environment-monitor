#include "NRF24L01.h"
#include "stm32f1xx_hal.h"

#define NRF_CMD_R_REGISTER 0x00U
#define NRF_CMD_W_REGISTER 0x20U
#define NRF_CMD_R_RX_PAYLOAD 0x61U
#define NRF_CMD_W_TX_PAYLOAD 0xA0U
#define NRF_CMD_FLUSH_TX 0xE1U
#define NRF_CMD_FLUSH_RX 0xE2U
#define NRF_REG_CONFIG 0x00U
#define NRF_REG_EN_AA 0x01U
#define NRF_REG_EN_RXADDR 0x02U
#define NRF_REG_SETUP_AW 0x03U
#define NRF_REG_SETUP_RETR 0x04U
#define NRF_REG_RF_CH 0x05U
#define NRF_REG_RF_SETUP 0x06U
#define NRF_REG_STATUS 0x07U
#define NRF_REG_RX_ADDR_P0 0x0AU
#define NRF_REG_TX_ADDR 0x10U
#define NRF_REG_RX_PW_P0 0x11U
#define NRF_STATUS_RX_DR 0x40U
#define NRF_STATUS_TX_DS 0x20U
#define NRF_STATUS_MAX_RT 0x10U

static SPI_HandleTypeDef hspi1;
static uint8_t local_address[5];
static uint8_t peer_address[5];

static void csn(GPIO_PinState state) { HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, state); }
static void ce(GPIO_PinState state) { HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, state); }
static uint8_t xfer(uint8_t value) { uint8_t received; (void)HAL_SPI_TransmitReceive(&hspi1, &value, &received, 1U, 20U); return received; }
static uint8_t command(uint8_t value) { csn(GPIO_PIN_RESET); uint8_t status = xfer(value); csn(GPIO_PIN_SET); return status; }
static void write_reg(uint8_t reg, uint8_t value) { csn(GPIO_PIN_RESET); (void)xfer(NRF_CMD_W_REGISTER | reg); (void)xfer(value); csn(GPIO_PIN_SET); }
static uint8_t read_reg(uint8_t reg) { csn(GPIO_PIN_RESET); (void)xfer(NRF_CMD_R_REGISTER | reg); uint8_t value=xfer(0xFFU); csn(GPIO_PIN_SET); return value; }
static void write_addr(uint8_t reg, const uint8_t address[5]) { csn(GPIO_PIN_RESET); (void)xfer(NRF_CMD_W_REGISTER | reg); for(uint8_t i=0U;i<5U;++i)(void)xfer(address[i]); csn(GPIO_PIN_SET); }
static void receive_mode(void) { write_reg(NRF_REG_CONFIG, 0x0FU); ce(GPIO_PIN_SET); }

void NRF24L01_Init(const uint8_t local[5], const uint8_t peer[5])
{
    GPIO_InitTypeDef gpio={0};
    __HAL_RCC_GPIOA_CLK_ENABLE(); __HAL_RCC_GPIOB_CLK_ENABLE(); __HAL_RCC_SPI1_CLK_ENABLE();
    gpio.Pin=GPIO_PIN_5|GPIO_PIN_7; gpio.Mode=GPIO_MODE_AF_PP; gpio.Speed=GPIO_SPEED_FREQ_HIGH; HAL_GPIO_Init(GPIOA,&gpio);
    gpio.Pin=GPIO_PIN_6; gpio.Mode=GPIO_MODE_INPUT; gpio.Pull=GPIO_NOPULL; HAL_GPIO_Init(GPIOA,&gpio);
    gpio.Pin=GPIO_PIN_4; gpio.Mode=GPIO_MODE_OUTPUT_PP; HAL_GPIO_Init(GPIOA,&gpio);
    gpio.Pin=GPIO_PIN_10; gpio.Mode=GPIO_MODE_OUTPUT_PP; HAL_GPIO_Init(GPIOB,&gpio);
    hspi1.Instance=SPI1; hspi1.Init.Mode=SPI_MODE_MASTER; hspi1.Init.Direction=SPI_DIRECTION_2LINES; hspi1.Init.DataSize=SPI_DATASIZE_8BIT; hspi1.Init.CLKPolarity=SPI_POLARITY_LOW; hspi1.Init.CLKPhase=SPI_PHASE_1EDGE; hspi1.Init.NSS=SPI_NSS_SOFT; hspi1.Init.BaudRatePrescaler=SPI_BAUDRATEPRESCALER_8; hspi1.Init.FirstBit=SPI_FIRSTBIT_MSB; hspi1.Init.TIMode=SPI_TIMODE_DISABLE; hspi1.Init.CRCCalculation=SPI_CRCCALCULATION_DISABLE; (void)HAL_SPI_Init(&hspi1);
    for (uint8_t i = 0U; i < 5U; ++i) { local_address[i] = local[i]; peer_address[i] = peer[i]; }
    ce(GPIO_PIN_RESET); csn(GPIO_PIN_SET); HAL_Delay(5U);
    write_reg(NRF_REG_EN_AA,0x01U); write_reg(NRF_REG_EN_RXADDR,0x01U); write_reg(NRF_REG_SETUP_AW,0x03U); write_reg(NRF_REG_SETUP_RETR,0x2FU); write_reg(NRF_REG_RF_CH,76U); write_reg(NRF_REG_RF_SETUP,0x06U); write_addr(NRF_REG_RX_ADDR_P0, local_address); write_addr(NRF_REG_TX_ADDR, peer_address); write_reg(NRF_REG_RX_PW_P0,8U); (void)command(NRF_CMD_FLUSH_TX); (void)command(NRF_CMD_FLUSH_RX); write_reg(NRF_REG_STATUS,NRF_STATUS_RX_DR|NRF_STATUS_TX_DS|NRF_STATUS_MAX_RT); receive_mode();
}

int NRF24L01_SendTelemetry(int16_t t,uint16_t h,uint16_t l,uint8_t s)
{
    uint8_t p[8]={0x01U,(uint8_t)(t>>8),(uint8_t)t,(uint8_t)(h>>8),(uint8_t)h,(uint8_t)(l>>8),(uint8_t)l,s};
    ce(GPIO_PIN_RESET); write_reg(NRF_REG_CONFIG,0x0EU); csn(GPIO_PIN_RESET); (void)xfer(NRF_CMD_W_TX_PAYLOAD); for(uint8_t i=0U;i<8U;++i)(void)xfer(p[i]); csn(GPIO_PIN_SET); ce(GPIO_PIN_SET); HAL_Delay(1U); ce(GPIO_PIN_RESET);
    uint32_t start=HAL_GetTick(); while((read_reg(NRF_REG_STATUS)&(NRF_STATUS_TX_DS|NRF_STATUS_MAX_RT))==0U && HAL_GetTick()-start<20U){}
    uint8_t status=read_reg(NRF_REG_STATUS); write_reg(NRF_REG_STATUS,NRF_STATUS_TX_DS|NRF_STATUS_MAX_RT); if(status&NRF_STATUS_MAX_RT)(void)command(NRF_CMD_FLUSH_TX); receive_mode(); return (status&NRF_STATUS_TX_DS)!=0U;
}

int NRF24L01_ReadTelemetry(int16_t *temperature_tenths, uint16_t *humidity_tenths, uint16_t *lux, uint8_t *status_out)
{
    uint8_t radio_status = read_reg(NRF_REG_STATUS);
    uint8_t p[8];
    if ((radio_status & NRF_STATUS_RX_DR) == 0U) return 0;
    csn(GPIO_PIN_RESET); (void)xfer(NRF_CMD_R_RX_PAYLOAD); for(uint8_t i=0U;i<8U;++i)p[i]=xfer(0xFFU); csn(GPIO_PIN_SET);
    write_reg(NRF_REG_STATUS, NRF_STATUS_RX_DR);
    if (p[0] != 0x01U) return 0;
    *temperature_tenths = (int16_t)((uint16_t)((uint16_t)p[1] << 8U) | p[2]);
    *humidity_tenths = (uint16_t)((uint16_t)p[3] << 8U | p[4]);
    *lux = (uint16_t)((uint16_t)p[5] << 8U | p[6]);
    *status_out = p[7];
    return 1;
}
