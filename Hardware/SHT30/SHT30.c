#include "SHT30.h"
#include "I2C1.h"

#define SHT30_ADDRESS_LOW 0x44U
#define SHT30_ADDRESS_HIGH 0x45U

static uint8_t sht30_status;
static uint8_t sht30_address = SHT30_ADDRESS_LOW;

static uint8_t SHT30_CRC8(const uint8_t *data)
{
    uint8_t crc = 0xFFU;
    for (uint8_t index = 0U; index < 2U; ++index) {
        crc ^= data[index];
        for (uint8_t bit = 0U; bit < 8U; ++bit) {
            crc = (crc & 0x80U) ? (uint8_t)((crc << 1U) ^ 0x31U) : (uint8_t)(crc << 1U);
        }
    }
    return crc;
}

static void SHT30_DelayConversion(void)
{
    for (volatile uint32_t cycles = 0U; cycles < 160000U; ++cycles) { }
}

void SHT30_Init(void)
{
    I2C1_Init();
}

int SHT30_Read(int16_t *temperature_tenths, uint16_t *humidity_tenths)
{
    const uint8_t command[2] = {0x2CU, 0x06U};
    uint8_t sample[6];
    uint16_t raw_temperature;
    uint16_t raw_humidity;
    if (!I2C1_Write(sht30_address, command, 2U)) {
        uint8_t alternate = (sht30_address == SHT30_ADDRESS_LOW) ? SHT30_ADDRESS_HIGH : SHT30_ADDRESS_LOW;
        if (!I2C1_Write(alternate, command, 2U)) { sht30_status = 1U; return 0; }
        sht30_address = alternate;
    }
    SHT30_DelayConversion();
    if (!I2C1_Read(sht30_address, sample, 6U)) { sht30_status = 2U; return 0; }
    if (SHT30_CRC8(&sample[0]) != sample[2] || SHT30_CRC8(&sample[3]) != sample[5]) { sht30_status = 3U; return 0; }
    raw_temperature = (uint16_t)((uint16_t)sample[0] << 8U) | sample[1];
    raw_humidity = (uint16_t)((uint16_t)sample[3] << 8U) | sample[4];
    *temperature_tenths = (int16_t)(((int32_t)1750 * raw_temperature) / 65535 - 450);
    *humidity_tenths = (uint16_t)(((uint32_t)1000 * raw_humidity) / 65535);
    sht30_status = 0U;
    return 1;
}

uint8_t SHT30_GetStatus(void)
{
    return sht30_status;
}
