// Application layer protocol implementation

#include "application_layer.h"
#include "link_layer.h"
#include "aux.h"
#include "macros.h"

int buildDataPacket(unsigned char *packet, int sequenceNumber, unsigned char *data, int dataLength)
{
    int shift = 4;

    packet[0] = CTRL_DATA;

    packet[1] = (unsigned char)sequenceNumber;

    int l1, l2;
    util_get_MSB(dataLength, &l2);
    util_get_LSB(dataLength, &l1);

    packet[2] = (unsigned char)l2;

    packet[3] = (unsigned char)l1;

    for (int i = 0; i < dataLength; i++)
        packet[i + shift] = data[i];

    return dataLength + shift;
}

int buildControlPacket(unsigned char controlField, unsigned char *packet, int fileSize, char *fileName)
{

    packet[0] = controlField;

    packet[1] = TYPE_FILE_SIZE;


    int length = 0;
    int currentFileSize = fileSize;

    // cicle to separate file size (v1) in bytes
    while (currentFileSize > 0)
    {
        int rest = currentFileSize % 256;
        int div = currentFileSize / 256;
        length++;

        // shifts all bytes to the right, to make space for the new byte
        for (unsigned int i = 2 + length; i > 3; i--)
            packet[i] = packet[i - 1];

        packet[3] = (unsigned char)rest;

        currentFileSize = div;
    }

    packet[2] = (unsigned char)length;

    packet[3 + length] = TYPE_FILENAME;

    int fileNameStart = 5 + length; // beginning of v2

    packet[4 + length] = (unsigned char)(strlen(fileName) + 1); // adds file name length (including '\0)

    for (unsigned int j = 0; j < (strlen(fileName) + 1); j++)
    { // strlen(fileName) + 1 in order to add the '\0' char
        packet[fileNameStart + j] = fileName[j];
    }

    return 3 + length + 2 + strlen(fileName) + 1; // total length of the packet
}

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
