#ifndef _ALARM_H_
#define _ALARM_H_

#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#include "link_layer.h"
#include "macros.h"

int currentRetransmission, relay = FALSE, stop = FALSE;
extern nRetransmissions, timeout;

void alarmHandler(int signal);

#endif // _ALARM_H_