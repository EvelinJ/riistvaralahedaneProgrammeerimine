#include <stdio.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/atomic.h>
#include <util/delay.h>
#include "hmi_msg.h"
#include "uart_wrapper.h"
#include "cli_microrl.h"
#include "../lib/hd44780_111/hd44780.h"
#include "../lib/andygock_avr-uart/uart.h"
#include "../lib/helius_microrl/microrl.h"

#define BAUD 9600


/* Global seconds counter */
volatile uint32_t time;


/* Create microrl object and pointer on it */
microrl_t rl;
microrl_t *prl = &rl;


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
}


static inline void init_cli(void)
{
    /* Init microrl */
    microrl_init(prl, cli_print);
    /* Set callback for execute */
    microrl_set_execute_callback(prl, cli_execute);
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


void main(void)
{
    init_hw();
    print_startup_info();
    init_cli();

    while (1) {
        heartbeat();
        /* CLI commands are handled in cli_execute() */
        microrl_insert_char(prl, cli_get_char());
    }
}


/* System clock ISR */
ISR(TIMER5_COMPA_vect)
{
    time++;
}
