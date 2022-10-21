#ifndef _ALARM_H_
#define _ALARM_H_

#include <stdio.h>
#include <signal.h>

#include "../src/application_layer.c"

#define FALSE 0
#define TRUE 1

int currentRetransmission;
int retransmite = FALSE; 

void alarmHandler(int signal);

#endif // _ALARM_H_