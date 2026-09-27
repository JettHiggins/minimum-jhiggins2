#include "minemu/uart.h"
#include "minemu/platform.h"

void uart_put_character(char c){
  while ((MINEMU_UART0->status & MINEMU_UART_STATUS_TX_READY) == 0){
  }

  MINEMU_UART0->tx_data = (uint32_t)c;
}
