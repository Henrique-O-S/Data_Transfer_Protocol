#include "alarm.h"

void alarmHandler(int signal){
    if(linklayer.nRetransmissions > currentRetransmission){
        retransmite = TRUE;
        alarm(linklayer.timeout);
        currentRetransmission++;
        printf("Timeout or invalide value: resending...\n");
    }

    else{
        printf("Timeout or invalide value: number of tries exceeded!\n");
    }
}