#include <stdio.h>

#if defined(__is_libk)
#include <kernel/tty.h>
#endif

int putchar(int ic) {
    #if defined(__is_libk)
        char c = (char)ic;
        terminal_write(&c, sizeof(c));
    #else
        // TODO: Implement stdio and write the syscall.
    #endif
    return ic;
}
