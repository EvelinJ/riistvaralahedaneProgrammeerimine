#include <stdio.h>
#include <avr/pgmspace.h>
#include <string.h>
#include <avr/io.h>
#include <util/delay.h>
#include "hmi_msg.h"
#include "uart.h"
#include "print_helper.h"
#include "../lib/hd44780_111/hd44780.h"

#define BLINK_DELAY_MS 100

void main (void)
{
    /* Set pin 3 of PORTA for output */
    DDRA |= _BV(DDA3);
    /* Init error console as stderr in UART3, init CLI in UART0, init lcd and print user code info */
    uart3_initialize();
    uart0_initialize();
    lcd_init();
    lcd_clrscr();
    stderr = &uart3_out;
    stdout = stdin = &uart0_io;
    fprintf_P(stderr, PSTR(VER_FW "\n"));
    fprintf_P(stderr, PSTR(VER_LIBC " " VER_GCC "\n"));
    /* End UART3 init, UART0 init, lcd init and info print */
    /* Print STUD_NAME to CLI and display on LCD row 1 */
    fprintf_P(stdout, PSTR(STUD_NAME "\n"));
    lcd_puts_P(PSTR(STUD_NAME));
    /* Print ascii tables to CLI */
    print_ascii_tbl(stdout);
    unsigned char ascii[128] = {0};

    for (unsigned char i = 0; i < sizeof(ascii); i++) {
        ascii[i] = i;
    }

    print_for_human(stdout, ascii, sizeof(ascii));

    while (1) {
        /* Set pin 3 high to turn LED on */
        PORTA |= _BV(PORTA3);
        _delay_ms(BLINK_DELAY_MS);
        /* Ask user input, try to find month beginning with letter from lookup list */
        /* Print months names to CLI and display on LCD row 2 */
        char inBuf;
        fprintf_P(stdout, PSTR(GET_LETTER));
        fscanf(stdin, "%c", &inBuf);
        fprintf(stdout, "%c\n", inBuf);
        lcd_goto(0x40);

        for (int i = 0; i < 6; i++) {
            if (!strncmp_P(&inBuf, (PGM_P)pgm_read_word(&(month_table[i])), 1)) {
                fprintf_P(stdout, PSTR("%S\n"), (PGM_P)pgm_read_word(&(month_table[i])));
                lcd_puts_P((PGM_P)pgm_read_word(&(month_table[i])));
                lcd_putc(' ');
            }
        }

        /* clear LCD row 2 */
        for (int i = 0; i < 16; i++) {
            lcd_putc(' ');
        }

        /* Set pin 3 low to turn LED off */
        PORTA &= ~_BV(PORTA3);
        _delay_ms(BLINK_DELAY_MS);
    }
}
