#ifndef DEBUG_H_
#define DEBUG_H_

#include<stdarg.h>
#include<stdio.h>

#define DEBUG_FILE stdout

void debug_print(const char * function, int line, const char * fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    fprintf(DEBUG_FILE, "[DEBUG] %s:%d: ", function, line);
    vfprintf(DEBUG_FILE, fmt, ap);
    fprintf(DEBUG_FILE, "\n");
    va_end(ap);
}

#define ENABLE_DEBUG
#ifdef ENABLE_DEBUG
#define DEBUG(fmt, ...) debug_print(__FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#else
#define DEBUG(fmt, ...)
#endif

//typedef void (*panic_handler_function)();
//#define PANIC_HANDLER_FUNCTION ((panic_handler_function)NULL)

void panic(const char * file, const char * function, int line, const char * fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    fprintf(stderr, "[PANIC] in %s @ %s:%d: ", file, function, line);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);

    /*if(PANIC_HANDLER_FUNCTION) {
        PANIC_HANDLER_FUNCTION();
    }*/
}

#define PANIC(fmt, ...) panic(__FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)

#endif
