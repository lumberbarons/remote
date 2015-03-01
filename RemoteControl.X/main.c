#include <xc.h>
#include <plib/usart.h>

#include "config.h"
#include "delay.h"

unsigned char UART1Config = 0;
unsigned char baud = 312; // 9600 Baud at 48MHz

void init_serial() {
    TRISCbits.RC6 = 0;
    TRISCbits.RC7 = 1;

    UART1Config = USART_TX_INT_OFF & USART_TX_INT_OFF & USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_BRGH_HIGH;
    OpenUSART(UART1Config, baud);
}

void flash_led() {
    TRISB4 = 0;
    LATB4 = 1;
    delay_10ms(10);
    LATB4 = 0;
}

void init() {
    init_serial();
    flash_led();
}

void main() {
    init();

    while (1) {
        
    }
}
