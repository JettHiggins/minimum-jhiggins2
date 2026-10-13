#include "minemu/uart.h"
#include "minemu/msh.h"
#include "minemu/irq.h"
#include "minemu/platform.h"

#include <stddef.h>

#define MSH_LINE_LIMIT 25

char line[MSH_LINE_LIMIT + 1] = { 0 };
size_t length = 0;

int cmpstr(char* s1, char* s2){
  while (*s1 && *s2){
    if (*s1 != *s2){
      return 0;
    }
    ++s1;
    ++s2;
  }
  return *s1 == *s2;
}

void execute_command(){
  if (length == 0){
    return;
  }
  // Command is from 0 to length
  char command[MSH_LINE_LIMIT + 1] = { 0 };
  size_t index = 0;
  line[length] = '\0';

  while (index < length){
    if (line[index] == ' '){
      break;
    }
    else {
      command[index] = line[index];
    }
    ++index;
  }

  command[index] = '\0';

  // from 0 to index is the first command
  if (cmpstr(command, "echo")){
    ++index; // skip initial whitespace
    size_t echo_index = 0;
    char echo[MSH_LINE_LIMIT + 1] = { 0 };
    while (index < length){
      echo[echo_index++] = line[index++];
    }

    uart_printf("%s\n", echo);
  }
  else{
    uart_printf("command not found: %s\n", command);
  }
}

void reset_line(){
  length = 0;

  for (int i = 0; i < (int)sizeof(line); ++i){
    line[i] = 0;
  }
  uart_printf("msh> ");
}

void msh(){
  uart_printf("msh> ");

  while(1){
    minemu_irq_disable();
    char c = get_byte();
    minemu_irq_enable();
    if (c == 0){
      // no byte
    }
    else if (c == 0x08 || c == 0x7f){
      if (length > 0){
        length--;
      }
    } 
    else if (c == '\n'){
      execute_command();
      reset_line();
    } 
    else if (length < MSH_LINE_LIMIT){
      //Regular character
      if (c == ' ' && length == 0){
        // ignore all leading whitespaces
      }
      else{
        line[length++] = c;
      }
    }
  }
}
