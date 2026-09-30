#ifndef UART_H
#define UART_H

void uart_put_character(char c);
int uart_printf(const char *format, ...);

void uart0_irq_handler();

char get_byte();

#endif

