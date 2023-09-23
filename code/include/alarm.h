#ifndef _ALARM_H_
#define _ALARM_H_

#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#include "macros.h"

void alarmHandler(int signal);

#endif // _ALARM_H_