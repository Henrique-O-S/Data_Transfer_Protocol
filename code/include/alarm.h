#ifndef _ALARM_H_
#define _ALARM_H_

#include <stdio.h>
#include <signal.h>

#include "application_layer.h"
#include "macros.h"

int currentRetransmission, relay = FALSE, stop = FALSE; 

void alarmHandler(int signal);

#endif // _ALARM_H_