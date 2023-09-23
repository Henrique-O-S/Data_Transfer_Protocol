#include "alarm.h"

int currentRetransmission, relay = FALSE, stop = FALSE;
extern int nRetransmissions, timeout;

void alarmHandler(int signal){
    if(nRetransmissions > currentRetransmission){
        relay = TRUE;
        alarm(timeout);
        currentRetransmission++;
        printf("Timeout or invalide value: resending...\n");
    }

    else{
        printf("Timeout or invalide value: number of tries exceeded!\n");
        stop = TRUE;
    }
}