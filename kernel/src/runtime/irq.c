#include "minemu/irq.h"
#include "minemu/platform.h"
#include "minemu/uart.h"


void (*handlers[])(void) = {
  [MINEMU_IRQ_UART0] = uart0_irq_handler,
};

struct minemu_trap_frame *minemu_irq_dispatch(struct minemu_trap_frame *frame){
  handlers[frame->exception_id]();
  MINEMU_INTERRUPT->eoi = frame->exception_id;
  return frame;
}
