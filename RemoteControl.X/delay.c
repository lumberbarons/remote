#include <xc.h>
#include "delay.h"

void delay_10ms(unsigned char count) {
    while (count-- != 0) {
        __delay_ms(10);
    }
}

void delay_s(unsigned char count) {
    while (count-- != 0) {
        delay_10ms(100);
    }
}
