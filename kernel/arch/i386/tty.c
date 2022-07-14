#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <kernel/tty.h>

#include "vga.h"

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
static uint16_t* const VGA_MEMORY = (uint16_t*)0x0B8000;

static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_colour;
static uint16_t* terminal_buffer;

void terminal_initialise(void) {
    terminal_row = 0;
    terminal_column = 0;
    terminal_colour = vga_entry_colour(VGA_COLOUR_LIGHT_GREY, VGA_COLOUR_BLACK);
    terminal_buffer = VGA_MEMORY;
    for (size_t y = 0; y < VGA_HEIGHT; y++)
        for (size_t x = 0; x < VGA_WIDTH; x++)
            terminal_buffer[y * VGA_WIDTH + x] = vga_entry(' ', terminal_colour);
}

void terminal_setcolour(uint8_t colour) {
    terminal_colour = colour;
}

void terminal_putentryat(unsigned char character, uint8_t colour, size_t x, size_t y) {
    terminal_buffer[y * VGA_WIDTH + x] = vga_entry(character, colour);
}

void terminal_scroll(int line) {
    char* loop;
    char c;
    for (loop = line * VGA_WIDTH * 2 + 0x0B8000; loop < VGA_WIDTH; loop++) {
        c = *loop;
        *(loop - VGA_WIDTH * 2) = c;
    }
    
}

void terminal_delete_last_line() {
    int x, *ptr;
    for (x = 0; x < VGA_WIDTH; x++) {
        ptr = 0x0B8000 + VGA_WIDTH * 2 * (VGA_HEIGHT - 1) + x;
        *ptr = 0;
    }
}

void terminal_putchar(char c) {
    int line;
    unsigned char uc = c;

    terminal_putentryat(uc, terminal_colour, terminal_column, terminal_row);
    if (++terminal_column == VGA_WIDTH) {
        terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT) {
            for (line = 1; line <= VGA_HEIGHT - 1; line++)
                terminal_scroll(line);
            terminal_delete_last_line();
            terminal_row = VGA_HEIGHT - 1;
        }
    }
}

void terminal_write(const char* data, size_t size) {
    for (size_t i = 0; i < size; i++)
        terminal_putchar(data[i]);
}

void terminal_writestring(const char* data) {
    terminal_write(data, strlen(data));
}
