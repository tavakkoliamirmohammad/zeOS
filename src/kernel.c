#include "kernel.h"
#include <stdint.h>
#include <stddef.h>
#include "idt/idt.h"
#include "memory/heap/kheap.h"

uint16_t *video_memory = 0;
uint16_t terminal_row = 0;
uint16_t terminal_col = 0;

uint16_t maker_char_terminal(char c, char color) {
    return (color << 8) | c;
}

void terminal_putchar(int x, int y, char c, char color) {
    video_memory[y * VGA_WIDTH + x] = maker_char_terminal(c, color);
}

void terminal_writechar(char c, char color) {
    if (c == '\n') {
        terminal_row++;
        terminal_col = 0;
        return;
    }
    terminal_putchar(terminal_col, terminal_row, c, color);
    terminal_col++;
    if (terminal_col >= VGA_WIDTH) {
        terminal_col = 0;
        terminal_row++;
    }
}

void terminal_initialize() {
    video_memory = (uint16_t * )(0xB8000);
    for (int y = 0; y < VGA_HEIGHT; ++y) {
        for (int x = 0; x < VGA_WIDTH; ++x) {
            terminal_putchar(x, y, ' ', 0);
        }
    }
}

size_t strlen(const char *str) {
    size_t len = 0;
    while (str[len]) ++len;
    return len;
}

void print(const char *str) {
    size_t len = strlen(str);
    for (int i = 0; i < len; ++i) {
        terminal_writechar(str[i], 15);
    }
}

void kernel_main() {
    terminal_initialize();
    print("Hello World!\nGood Bye!");
    kheap_init();
    idt_init();
}
