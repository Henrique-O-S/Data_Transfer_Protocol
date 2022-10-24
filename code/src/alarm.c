#include "alarm.h"

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