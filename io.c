#include "io.h"

void UART_WriteByte(uint8_t data)
{
	// Wait for the previous byte to send
	while (!(UCSR0A & (1 << UDRE0)));

	// Send the byte
	UDR0 = data;
}

uint8_t UART_ReadByte()
{
	return UDR0;
}

void UART_WriteString(uint8_t *data)
{
	while (*data)
		UART_WriteByte(*data++);
}

uint8_t SPI_Byte(uint8_t data)
{
	SPDR = data;			// Load byte to data register
	while (!(SPSR & (1 << SPIF)));	// Wait for transmission to complete
	return SPDR;			// Return the returned byte
}

// Make sure to take care of chip select before and after this function!
void IOEXP_Write16(uint8_t addr, uint16_t data)
{
	addr <<= 1;		// Shift the address to the left once, to make
	addr |= 1;		// room for the R/W bit (1 == write)

	SPI_Byte(addr);		// Send address and opcode to the IO expander
	SPI_Byte(0x12);		// Select register GPIOA
	SPI_Byte(data >> 8);	// Send the high 8 bits of the data
	SPI_Byte(data & 0xFF);	// Send the low 8 bits of the data
}

void IOEXP_Write8A(uint8_t addr, uint8_t data)
{
	addr <<= 1;		// Shift the address to the left once, to make
	addr |= 1;		// room for the R/W bit (1 == write)

	SPI_Byte(addr);		// Send address and opcode to the IO expander
	SPI_Byte(0x12);		// Select register GPIOA
	SPI_Byte(data);		// Send the data
}

void IOEXP_Write8B(uint8_t addr, uint8_t data)
{
	addr <<= 1;		// Shift the address to the left once, to make
	addr |= 1;		// room for the R/W bit (1 == write)

	SPI_Byte(addr);		// Send address and opcode to the IO expander
	SPI_Byte(0x13);		// Select register GPIOA
	SPI_Byte(data);		// Send the data
}

uint16_t IOEXP_Read16(uint8_t addr)
{
	addr <<= 1;		// Make room for R/W bit (0 == read)

	SPI_Byte(addr);		// Send the address and opcode
	SPI_Byte(0x12);		// Select register GPIOA
	
	// Return two bytes (16 bit) of data
	return SPI_Byte(0xFF) | (SPI_Byte(0xFF) << 8);
}

uint8_t IOEXP_Read8A(uint8_t addr)
{
	addr <<= 1;		// Make room for R/W bit (0 == read)

	SPI_Byte(addr);		// Send the address and opcode
	SPI_Byte(0x12);		// Select register GPIOA
	
	return SPI_Byte(0xFF);	// Return a byte (8 bit) of data
}

uint8_t IOEXP_Read8B(uint8_t addr)
{
	addr <<= 1;		// Make room for R/W bit (0 == read)

	SPI_Byte(addr);		// Send the address and opcode
	SPI_Byte(0x13);		// Select register GPIOA
	
	return SPI_Byte(0xFF);	// Return a byte (8 bit) of data
}
