#include "minemu/user_abi.h"

#define MSH_LINE_LIMIT 25U

static void write_bytes(char *text, uint32_t length) {
    (void)ioctl(MINEMU_FD_UART0, MINEMU_OP_UART_WRITE, text, (int)length);
}

static void execute_command(char *line, uint32_t length) {
    uint32_t start = 0;
    while (start < length && line[start] == ' ') {
        ++start;
    }
    if (start == length) {
        return;
    }
    uint32_t end = start;
    while (end < length && line[end] != ' ') {
        ++end;
    }
    if (end - start == 4U && line[start] == 'e' && line[start + 1U] == 'c' &&
        line[start + 2U] == 'h' && line[start + 3U] == 'o') {
        /* Collapse ASCII-space runs and omit leading/trailing spaces. */
        char output[MSH_LINE_LIMIT + 1U];
        uint32_t used = 0;
        int separator = 0;
        for (uint32_t index = end; index < length; ++index) {
            if (line[index] == ' ') {
                separator = used != 0;
            } else {
                if (separator) {
                    output[used++] = ' ';
                    separator = 0;
                }
                output[used++] = line[index];
            }
        }
        output[used++] = '\n';
        write_bytes(output, used);
    } else {
        write_bytes("command not found: ", 19U);
        write_bytes(&line[start], end - start);
        write_bytes("\n", 1U);
    }
}

void minemu_user_main(void) {
    char line[MSH_LINE_LIMIT + 1U];
    uint32_t length = 0;
    int overlong = 0;
    write_bytes("msh> ", 5U);
    for (;;) {
        char byte;
        if (ioctl(MINEMU_FD_UART0, MINEMU_OP_UART_READ, &byte, 1) != 1) {
            continue;
        }
        if (byte == '\n') {
            /* Discard an overlong line instead of executing a truncated command. */
            if (!overlong) {
                line[length] = '\0';
                execute_command(line, length);
            }
            length = 0;
            overlong = 0;
            write_bytes("msh> ", 5U);
        } else if (overlong) {
            continue;
        } else if (byte == '\b' || byte == '\x7f') {
            if (length != 0) {
                --length;
            }
        } else if (byte == ' ' && length == 0) {
            continue;
        } else if (length < MSH_LINE_LIMIT) {
            line[length++] = byte;
        } else {
            overlong = 1;
        }
    }
}
