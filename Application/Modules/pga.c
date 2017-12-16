/*
 * pga.c
 *
 *  Created on: 15.03.2017
 *      Author: Kwarc
 */

#include "pga.h"
#include "spi.h"

/* PGA MCP6S21 driver */

static uint8_t data[2];
static const uint16_t timeout = 20;
const uint8_t pga_gain_val[8] = {1,2,4,5,8,10,16,32};
const uint8_t pga_gain_reg[8] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};

void PGA_NOP(void)
{
	data[0] = MCP6S21_NOP; data[1] = 0x00;
	PGA_CS_LOW();
	HAL_SPI_Transmit(&hspi4, data, 2, timeout);
	PGA_CS_HIGH();
}

void PGA_Shutdown(void)
{
	data[0] = MCP6S21_SHUTDOWN; data[1] = 0x00;
	PGA_CS_LOW();
	HAL_SPI_Transmit(&hspi4, data, 2, timeout);
	PGA_CS_HIGH();
}
void PGA_SetGain(uint8_t gain)
{
	pga_gain = pga_gain_val[gain];	// store gain value in global variable
	data[0] = MCP6S21_WRITE_GAIN; data[1] = pga_gain_reg[gain];
	PGA_CS_LOW();
	HAL_SPI_Transmit(&hspi4, data, 2, timeout);
	PGA_CS_HIGH();
}
void PGA_SetChannel(uint8_t channel)
{
	data[0] = MCP6S21_WRITE_CH; data[1] = channel;
	PGA_CS_LOW();
	HAL_SPI_Transmit(&hspi4, data, 2, timeout);
	PGA_CS_HIGH();
}
