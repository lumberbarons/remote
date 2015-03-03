#define USE_AND_MASKS

#include <xc.h>
#include <string.h>
#include <plib/usart.h>

#include "config.h"
#include "delay.h"

#define FLASH_LED TRISB = 0; LATB4 = 1; delay_10ms(10); LATB4 = 0;

unsigned char UART1Config = 0;
unsigned char baud = 51; // 57600 at 48MHz with BRGH High

void init_serial() {
    TRISCbits.RC6 = 0;
    TRISCbits.RC7 = 1;

    UART1Config = USART_TX_INT_OFF & USART_RX_INT_ON & USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_BRGH_HIGH;
    OpenUSART(UART1Config, baud);

    RCIF = 0;
    RCIP = 0;
    RCIE = 1;

    PEIE = 1;
    GIE = 1;
}

void prompt() {
    putsUSART("\r\n> ");
}

void init() {
    init_serial();
    FLASH_LED;
    
    putsUSART("\r\n\nRemote Control v0.0.1");
    prompt();
}

char rx;

int index = 0;
char command[255] = "";

void process_command() {
    command[index] = 0;
    index = 0;

    if(strcmp(command, "flash") == 0) {
        FLASH_LED;
    }
}

void interrupt isr() {
    if(PIR1bits.RCIF == 1) {
        rx = ReadUSART();
        if(rx == '\r') {
            process_command();
            prompt();
        } else if(rx != '\b') {
            command[index++] = rx;
            WriteUSART(rx);
        }
        PIR1bits.RCIF = 0;
    }
}

void main() {
    init();
    while (1) {}
}
