#include <stdio.h>
#include <avr/pgmspace.h>
#include <string.h>
#include <avr/io.h>
#include <util/delay.h>
#include "uart.h"
#include "hmi_msg.h"
#include "print_helper.h"

#define BLINK_DELAY_MS 100

void main (void)
{
    /* Set pin 3 of PORTA for output */
    DDRA |= _BV(DDA3);
    /* Init UART0, init error console as stderr in UART3 and print user code info */
    uart0_initialize();
    uart3_initialize();
    stdin = stdout = &uart0_io;
    stderr = &uart3_out;
    fprintf_P(stderr, PSTR(VER_FW "\n"));
    fprintf_P(stderr, PSTR(VER_LIBC " " VER_GCC "\n"));
    /* End UART3 init and info print */
    /* Print STUD_NAME */
    fprintf(stdout, STUD_NAME "\n");
    /* Print ascii tables */
    print_ascii_tbl(stdout);
    unsigned char ascii[128] = {0};

    for (unsigned char i = 0; i < 128; i++) {
        ascii[i] = i;
    }

    print_for_human(stdout, ascii, 128);

    while (1) {
        /* Set pin 3 high to turn LED on */
        PORTA |= _BV(PORTA3);
        _delay_ms(BLINK_DELAY_MS);
        /* Month name comparison and print */
        char inBuf = ' ';
        fprintf(stdout, "Enter Month name first letter >");
        fscanf(stdin, "%c", &inBuf);
        fprintf(stdout, "%c\n", inBuf);
        int NAME_MONTH_COUNT = 6;

        for (int i = 0; i < NAME_MONTH_COUNT; i++) {
            if (!strncmp(&inBuf, nameMonth[i], 1)) {
                fprintf(stdout, "%s\n", nameMonth[i]);
            }
        }

        /* Set pin 3 low to turn LED off */
        PORTA &= ~_BV(PORTA3);
        _delay_ms(BLINK_DELAY_MS);
    }
}
