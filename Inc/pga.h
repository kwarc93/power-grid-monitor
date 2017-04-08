#ifndef _PGA_H_
#define _PGA_H_

#include "stm32f4xx_hal.h"
/* MCP6S21 REGISTERS */

// Instructions
#define MCP6S21_NOP			0x00
#define MCP6S21_SHUTDOWN	0x20
#define MCP6S21_WRITE_GAIN	0x40
#define MCP6S21_WRITE_CH	0x41
// Gain setting
#define MCP6S21_GAINx1		0x00
#define MCP6S21_GAINx2		0x01
#define MCP6S21_GAINx4		0x02
#define MCP6S21_GAINx5		0x03
#define MCP6S21_GAINx8		0x04
#define MCP6S21_GAINx10		0x05
#define MCP6S21_GAINx16		0x06
#define MCP6S21_GAINx32		0x07
// Channel setting
#define MCP6S21_CH0			0x00

/* MCP6S21 MACROS */
#define PGA_CS_LOW()       (PGA_CS_GPIO_Port->BSRR = GPIO_BSRR_BR_4)
#define PGA_CS_HIGH()      (PGA_CS_GPIO_Port->BSRR = GPIO_BSRR_BS_4)

uint8_t pga_gain;
enum e_pga_gain {x1,x2,x4,x5,x8,x10,x16,x32};
extern const uint8_t pga_gain_val[8];

/* MCP6S21 FUNCTIONS */
void PGA_NOP(void);
void PGA_Shutdown(void);
void PGA_SetGain(uint8_t gain);
void PGA_SetChannel(uint8_t channel);

#endif /* _PGA_H_ */
