#ifndef _HMI_MSG_H_
#define _HMI_MSG_H_


#define VER_FW "Version: "GIT_DESCR" built on: "__DATE__" "__TIME__""
#define VER_LIBC "avr-libc version: "__AVR_LIBC_VERSION_STRING__""
#define VER_GCC "avr-gcc version: "__VERSION__""

#define STUD_NAME "Evelin Jõgi"

const char *nameMonth[] = {"January",
                           "February",
                           "March",
                           "April",
                           "May",
                           "June"};

#endif /* _HMI_MSG_H_ */

