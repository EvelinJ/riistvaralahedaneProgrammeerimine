#include <stdio.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <string.h>
#include <avr/io.h>
#include <util/atomic.h>
#include <util/delay.h>
#include "hmi_msg.h"
#include "uart_wrapper.h"
#include "print_helper.h"
#include "../lib/hd44780_111/hd44780.h"
#include "../lib/andygock_avr-uart/uart.h"

#define BAUD 9600

/* Global seconds counter */
volatile uint32_t time;


static inline void init_system_clock(void)
{
    TCCR5A = 0; // Clear control register A
    TCCR5B = 0; // Clear control register B
    TCCR5B |= _BV(WGM52) | _BV(CS52); // CTC and fCPU/256
    OCR5A = 62549; // 1 s
    TIMSK5 |= _BV(OCIE5A); // Output Compare A Match Interrupt Enable
}


static inline void init_hw(void)
{
    /* Set pin 3 of PORTA for output */
    DDRA |= _BV(DDA3);
    init_system_clock();
    /* Init error console as stderr in UART3, init CLI in UART0, init lcd */
    uart3_init(UART_BAUD_SELECT(BAUD, F_CPU));
    uart0_init(UART_BAUD_SELECT(BAUD, F_CPU));
    stderr = &uart3_out;
    stdout = stdin = &uart0_io;
    lcd_init();
    lcd_clrscr();
    /* End UART3 init, UART0 init, lcd init */
    /* System enable interrupts */
    sei();
}


static inline void print_startup_info(void)
{
    /* Print user code version info */
    fprintf_P(stderr, PSTR(VER_FW "\n"));
    fprintf_P(stderr, PSTR(VER_LIBC " " VER_GCC "\n"));
    /* End user code version info print */
    /* Print STUD_NAME to CLI and display STUD_NAME on LCD row 1 */
    fprintf_P(stdout, PSTR(STUD_NAME "\n"));
    lcd_puts_P(PSTR(STUD_NAME));
    /* Print ascii tables to CLI */
    print_ascii_tbl(stdout);
    unsigned char ascii[128];

    for (unsigned char i = 0; i < sizeof(ascii); i++) {
        ascii[i] = i;
    }

    print_for_human(stdout, ascii, sizeof(ascii));
    fprintf_P(stdout, PSTR(GET_LETTER));
}


static inline void find_month(void)
{
    /* Ask user input, try to find month beginning with letter from lookup list */
    /* Print months names to CLI and display on LCD row 2 */
    char inBuf;
    fscanf(stdin, "%c", &inBuf);
    fprintf(stdout, "%c\n", inBuf);
    lcd_clr(LCD_ROW_2_START, LCD_VISIBLE_COLS);
    lcd_goto(LCD_ROW_2_START);

    for (int i = 0; i < 6; i++) {
        if (!strncmp_P(&inBuf, (PGM_P)pgm_read_word(&(month_table[i])), 1)) {
            fprintf_P(stdout, PSTR("%S\n"), (PGM_P)pgm_read_word(&(month_table[i])));
            lcd_puts_P((PGM_P)pgm_read_word(&(month_table[i])));
            lcd_putc(' ');
        }
    }

    fprintf_P(stdout, PSTR(GET_LETTER));
}


static inline void heartbeat(void)
{
    static uint32_t time_prev;
    uint32_t time_cpy;
    ATOMIC_BLOCK(ATOMIC_FORCEON) {
        time_cpy = time;
    }

    if (time_cpy != time_prev) {
        /* Toggle LED in Arduino Mega pin 25 */
        PORTA ^= _BV(PORTA3);
        fprintf_P(stderr, PSTR(LBL_UPTIME": %lu s\n"), time_cpy);
        time_prev = time_cpy;
    }
}


void main (void)
{
    init_hw();
    print_startup_info();

    while (1) {
        heartbeat();

        if (uart0_available()) {
            find_month();
        }
    }
}


/* System clock ISR */
ISR(TIMER5_COMPA_vect)
{
    time++;
}
