#include <stdio.h>
#include <string.h>
#include <avr/pgmspace.h>
#include "../lib/hd44780_111/hd44780.h"
#include "../lib/andygock_avr-uart/uart.h"
#include "../lib/helius_microrl/microrl.h"
#include "cli_microrl.h"
#include "hmi_msg.h"
#include "print_helper.h"


#define NUM_ELEMS(x) (sizeof(x) / sizeof((x)[0]))
#define UART_STATUS_MASK 0x00FF

void cli_print(const char * str);
char cli_get_char(void);
void cli_print_help(const char *const *argv);
void cli_print_ver(const char *const *argv);
void cli_print_ascii_tbls(const char *const *argv);
void cli_handle_month(const char *const *argv);
void cli_print_cmd_error(void);
void cli_print_cmd_arg_error(void);
/* Execute callback */
int cli_execute(int argc, const char *const *argv);


typedef struct cli_cmd {
    PGM_P cmd;
    PGM_P help;
    void (*func_p)();
    const uint8_t func_argc;
} cli_cmd_t;


const char help_cmd[] PROGMEM = HELP_CMD;
const char help_help[] PROGMEM = HELP_HELP;
const char ver_cmd[] PROGMEM = VER_CMD;
const char ver_help[] PROGMEM = VER_HELP;
const char ascii_cmd[] PROGMEM = ASCII_CMD;
const char ascii_help[] PROGMEM = ASCII_HELP;
const char month_cmd[] PROGMEM = MONTH_CMD;
const char month_help[] PROGMEM = MONTH_HELP;


const cli_cmd_t cli_cmds[] = {
    {help_cmd, help_help, cli_print_help, 0},
    {ver_cmd, ver_help, cli_print_ver, 0},
    {ascii_cmd, ascii_help, cli_print_ascii_tbls, 0},
    {month_cmd, month_help, cli_handle_month, 1}
};


void cli_print(const char *str)
{
    printf("%s", str);
}


char cli_get_char(void)
{
    if (uart0_peek() != UART_NO_DATA) {
        return uart0_getc() & UART_STATUS_MASK;
    } else {
        return 0x00;
    }
}


void cli_print_help(const char *const *argv)
{
    (void) argv;
    printf_P(PSTR("\n"));
    printf_P(PSTR(CLI_HELP_MSG "\n"));

    for (uint8_t i = 0; i < NUM_ELEMS(cli_cmds); i++) {
        printf_P(cli_cmds[i].cmd);
        printf_P(PSTR(" : "));
        printf_P(cli_cmds[i].help);
        printf_P(PSTR("\n"));
    }
}


void cli_print_ver(const char *const *argv)
{
    (void) argv;
    printf_P(PSTR("\n"));
    printf_P(PSTR(VER_FW "\n"));
    printf_P(PSTR(VER_LIBC " " VER_GCC "\n"));
}


void cli_print_ascii_tbls(const char *const *argv)
{
    (void) argv;
    printf_P(PSTR("\n"));
    /* ASCII tables print */
    print_ascii_tbl(stdout);
    unsigned char ascii[128];

    for (unsigned char i = 0; i < sizeof(ascii); i++) {
        ascii[i] = i;
    }

    print_for_human(stdout, ascii, sizeof(ascii));
}


void cli_handle_month(const char *const *argv)
{
    (void) argv;
    printf_P(PSTR("\n"));
    lcd_clr(LCD_ROW_2_START, LCD_VISIBLE_COLS);
    lcd_goto(LCD_ROW_2_START);

    for (int i = 0; i < 6; i++) {
        if (!strncmp_P(argv[1], (PGM_P)pgm_read_word(&(month_table[i])),
                       strlen(argv[1]))) {
            fprintf_P(stdout, PSTR("%S\n"), (PGM_P)pgm_read_word(&(month_table[i])));
            lcd_puts_P((PGM_P)pgm_read_word(&(month_table[i])));
            lcd_putc(' ');
        }
    }
}


void cli_print_cmd_error(void)
{
    printf_P(PSTR("\n"));
    printf_P(PSTR(CLI_ERROR_MSG "\n" CLI_USE_HELP_MSG "\n"));
}


void cli_print_cmd_arg_error(void)
{
    printf_P(PSTR("\n"));
    printf_P(PSTR(CLI_ARGUMENTS_ERROR_MSG "\n" CLI_USE_HELP_MSG "\n"));
}


int cli_execute(int argc, const char *const *argv)
{
    for (uint8_t i = 0; i < NUM_ELEMS(cli_cmds); i++) {
        if (!strcmp_P(argv[0], cli_cmds[i].cmd)) {
            // Test do we have correct arguments to run command
            // Function arguments count shall be defined in struct
            if ((argc - 1) != cli_cmds[i].func_argc) {
                cli_print_cmd_arg_error();
                return 0;
            }

            // Hand argv over to function pointer,
            // cross fingers and hope that funcion handles it properly
            cli_cmds[i].func_p (argv);
            return 0;
        }
    }

    cli_print_cmd_error();
    return 0;
}
