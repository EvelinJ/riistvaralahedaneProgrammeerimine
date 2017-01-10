#ifndef _HMI_MSG_H_
#define _HMI_MSG_H_

#define VER_FW "Version: "GIT_DESCR" built on: "__DATE__" "__TIME__""
#define VER_LIBC "avr-libc version: "__AVR_LIBC_VERSION_STRING__""
#define VER_GCC "avr-gcc version: "__VERSION__""

#define STUD_NAME "Evelin Jõgi"
#define LBL_UPTIME "Uptime"

#define HELP_CMD "help"
#define HELP_HELP "Get help"
#define VER_CMD "version"
#define VER_HELP "Print FW version"
#define ASCII_CMD "ascii"
#define ASCII_HELP "print ASCII tables"
#define MONTH_CMD "month"
#define MONTH_HELP "Find matching month from lookup list. Usage: month <string>"
#define CLI_HELP_MSG "Implemented commands:"
#define CLI_ERROR_MSG "Command not implemented"
#define CLI_USE_HELP_MSG "Use <help> to get help"
#define CLI_ARGUMENTS_ERROR_MSG "To few or too many arguments for this command"

extern PGM_P const month_table[];

#endif /* _HMI_MSG_H_ */

