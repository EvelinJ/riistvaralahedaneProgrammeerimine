#ifndef _HMI_MSG_H_
#define _HMI_MSG_H_


#define VER_FW "Version: "GIT_DESCR" built on: "__DATE__" "__TIME__""
#define VER_LIBC "avr-libc version: "__AVR_LIBC_VERSION_STRING__""
#define VER_GCC "avr-gcc version: "__VERSION__""

#define STUD_NAME "Evelin Jõgi"
#define GET_LETTER "Enter Month name first letter >"

const char month_1[] PROGMEM = "January";
const char month_2[] PROGMEM = "February";
const char month_3[] PROGMEM = "March";
const char month_4[] PROGMEM = "April";
const char month_5[] PROGMEM = "May";
const char month_6[] PROGMEM = "June";

PGM_P const month_table[] PROGMEM = 
{
    month_1,
    month_2,
    month_3,
    month_4,
    month_5,
    month_6
};

#endif /* _HMI_MSG_H_ */

