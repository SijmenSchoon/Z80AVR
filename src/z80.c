/*
The MIT License (MIT)

Copyright (c) 2013 Vijfhoek

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#define F_CPU 8000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

#define USART_BAUDRATE 38400
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

#include "io.h"

int mem_addr;
char binary_mode;

void Z80_RAM_SetAddress(uint16_t address)
{
	// Chip select RAM address register
	PORTC &= ~(1 << PORTC0);


}

uint8_t Z80_RAM_ReadByte(uint16_t addr)
{
	return 0xFF;
}

void Z80_RAM_WriteByte(uint16_t addr, uint8_t byte)
{
	// Chip Select RAM Address 
}


ISR(USART_RX_vect)
{
	char byte = UDR0;
	

	if (binary_mode)
	{
		switch (byte)
		{
			case '+':
			{
				UART_WriteByte(++mem_addr & 0xFF);
				UART_WriteByte(mem_addr >> 8);
				break;
			}

			case '-':
			{
				UART_WriteByte(--mem_addr & 0xFF);
				UART_WriteByte(mem_addr >> 8);
				break;
			}

			case 's':
			{
				while ((UCSR0A & (1 << RXC0)) == 0);
				mem_addr = UDR0;
				while ((UCSR0A & (1 << RXC0)) == 0);
				mem_addr |= UDR0 << 8;

				UART_WriteByte(mem_addr & 0xFF);
				UART_WriteByte(mem_addr >> 8);
				
				break;
			}

			case 'g':
			{
				UART_WriteByte(mem_addr & 0xFF);
				UART_WriteByte(mem_addr >> 8);
				break;
			}

			case 'r':
			{
				UART_WriteByte(Z80_RAM_ReadByte(mem_addr++));
				break;
			}

			case 'w':
			{
				while ((UCSR0A & (1 << RXC0)) == 0);
				Z80_RAM_WriteByte(mem_addr++, UDR0);
				UART_WriteByte(UDR0); // Echo byte
				break;
			}

			case 'b':
			{
				binary_mode = 0;
				UART_WriteString("Disabled binary mode.\r\n>");
			}
		}
	}
	else
	{
		// Return the char
		UART_WriteByte(byte);
		UART_WriteByte('\r');
		UART_WriteByte('\n');
		
		switch (byte)
		{
			case '?':
			{
				UART_WriteString("\
Commands:\r\n\
 ?  Show this help menu\r\n\
 +  Increase memory address counter\r\n\
 -  Decrease memory address counter\r\n\
 s  Set the memory address counter\r\n\
 g  Get the memory address counter\r\n\
 r  Read a byte from memory and increase address counter\r\n\
 w  Write a byte to memory and increase address counter\r\n\
 b  Toggle binary mode\r\n");
				break;
			}

			case '+':
			{
				char str[64];
				mem_addr++;
				sprintf(str, "0x%04x (% 3d)\r\n", mem_addr, mem_addr);
				UART_WriteString(str);
				break;
			}

			case '-':
			{
				char str[64];
				mem_addr--;
				sprintf(str, "0x%04x (% 3d)\r\n", mem_addr, mem_addr);
				UART_WriteString(str);
				break;
			}

			case 's':
			{
				char num[64], i = 0;
				long temp;

				while (1)
				{
					while ((UCSR0A & (1 << RXC0)) == 0);
					char b = UDR0;
					if (b == '\n' || b == '\r') { num[i] = 0; break; }
					UART_WriteByte(b);
					num[i++] = b;
				}
				UART_WriteString("\r\n");
				mem_addr = atoi(num);
				break;
			}

			case 'g':
			{
				char num[64];
				sprintf(num, "0x%04x (% 5d)\r\n", mem_addr, mem_addr);
				UART_WriteString(num);
				break;
			}

			case 'r':
			{
				char str[64];
				uint8_t i = Z80_RAM_ReadByte(mem_addr);
				sprintf(str, "[0x%04x] 0x%02x (%c)\r\n", mem_addr++, i, (i < 32 || i > 127) ? '.' : i);
				UART_WriteString(str);
				break;
			}

			case 'w':
			{
				char num[64], i = 0;
				long temp;

				while (1)
				{
					while ((UCSR0A & (1 << RXC0)) == 0);
					char b = UDR0;
					if (b == '\n' || b == '\r') { num[i] = 0; break; }
					UART_WriteByte(b);
					num[i++] = b;
				}
				UART_WriteString("\r\n");
				
				i = atoi(num);
				Z80_RAM_WriteByte(mem_addr, i);

				break;

			}

			case 'b':
			{
				UART_WriteByte(6);	// Send ACK character
				binary_mode = 1;
				return;
			}

			default:
			{
				UART_WriteString("Unknown command. Press ? for help.\r\n");
			}
		}
		UART_WriteByte('>');
	}
};

int main()
{
	UCSR0B = (1<<RXEN0) | (1<<TXEN0) | (1<<RXCIE0); 
	UCSR0C |= (1 << UCSZ00) | (1 << UCSZ01);
	UBRR0 = BAUD_PRESCALE;
	DDRB |= 1 << 5;

	UART_WriteString("Welcome. Write ? for help.\r\n>");

	sei();

	for (;;);

	return 0;
}
