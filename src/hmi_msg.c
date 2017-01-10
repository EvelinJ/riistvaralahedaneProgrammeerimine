#include <avr/pgmspace.h>
#include "hmi_msg.h"

static const char month_1[] PROGMEM = "January";
static const char month_2[] PROGMEM = "February";
static const char month_3[] PROGMEM = "March";
static const char month_4[] PROGMEM = "April";
static const char month_5[] PROGMEM = "May";
static const char month_6[] PROGMEM = "June";

PGM_P const month_table[] PROGMEM = {
    month_1,
    month_2,
    month_3,
    month_4,
    month_5,
    month_6
};
