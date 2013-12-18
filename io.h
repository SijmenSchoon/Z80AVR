#pragma once

#include <avr/io.h>
#include <stdint.h>

void     UART_WriteByte(uint8_t data);
uint8_t  UART_ReadByte();
void     UART_WriteString(uint8_t *data);

uint8_t  SPI_Byte(uint8_t data);

void     IOEXP_Write16(uint8_t addr, uint16_t data);
void     IOEXP_Write8A(uint8_t addr, uint8_t data);
void     IOEXP_Write8B(uint8_t addr, uint8_t data);
uint16_t IOEXP_Read16(uint8_t addr);
uint8_t  IOEXP_Read8A(uint8_t addr);
uint8_t  IOEXP_Read8B(uint8_t addr);
