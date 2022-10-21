// Application layer protocol implementation

#include "application_layer.h"
#include "link_layer.h"

LinkLayer linklayer;

void applicationLayer(const char *serialPort, const char *role, int baudRate,
                      int nTries, int timeout, const char *filename)
{
    strcpy(linklayer.serialPort, serialPort);
    linklayer.role = role;
    linklayer.baudRate = baudRate;
    linklayer.nRetransmissions = nTries;
    linklayer.timeout = timeout;

    llopen(linklayer);
}
