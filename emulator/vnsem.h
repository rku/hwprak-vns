#ifndef VNSEM_H
#define VNSEM_H 1

#include <stdint.h>

typedef struct _vnsem_configuration {
    uint8_t interactive_mode;
    uint8_t verbose_mode;
    uint16_t step_time_ms;
} vnsem_configuration;

#endif /* VNSEM_H */
