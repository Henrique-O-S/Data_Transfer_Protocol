// Application layer protocol implementation

#include "application_layer.h"
#include "link_layer.h"
#include "aux.h"
#include "macros.h"

//BUILDERS CALLED BY WRITER AND PARSERS CALLED BY READER

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

    return dataLength + shift; //returns len of buffer
}

int parseDataPacket(unsigned char *packet, unsigned char *data, int *sequenceNumber)
{
    if(packet[0] != CTRL_DATA){
        return 1;
    }
    *sequenceNumber = (int) packet[1];

    int size;
    int shift = 4;

    util_join_bytes(&size, packet[2], packet[3]);

    for(int i = 0; i < size; i++){
        data[i] = packet[i + shift];
    }
    return 0;
}

int buildControlPacket(unsigned char controlField, unsigned char *packet, int fileSize, char *fileName)
{

    packet[0] = controlField;

    packet[1] = TYPE_FILE_SIZE;

    int remainingFileSize = fileSize;

    int byteCount;

    get_size_in_bytes(fileSize, &byteCount);

    packet[2] = byteCount;

    int fileSizeStart = 3;

    for(int i = byteCount - 1; i > 0; i--){
        packet[fileSizeStart + i] = (unsigned char) (fileSize);
        fileSize = fileSize >> 8;
    }

    packet[3 + byteCount] = TYPE_FILE_NAME;

    int filenameSize = strlen(fileName) + 1;
    packet[4 + byteCount] = (unsigned char) filenameSize;

    int fileNameStart = 5 + byteCount;

    for(int i = 0; i < filenameSize; i++){
        packet[fileNameStart + i] = fileName[i];
    }
    return 5 + byteCount + filenameSize + 1; // returns len of buffer
}

int parseControlPacket(unsigned char *packet, int *fileSize, char *fileName)
{

    if (packet[0] != CTRL_START && packet[0] != CTRL_END)
    {
        return 1;
    }

    int lengthFileSize, lengthFileName, fileNameStart;
    *fileSize = 0;
    

    if (packet[1] == TYPE_FILE_SIZE)
    {
        lengthFileSize = (int) packet[2];
        for (int i = 0; i < lengthFileSize; i++)
        {
            *fileSize = *fileSize | (unsigned char) packet[3 + i];
            if(i != lengthFileSize - 1){
                *fileSize = *fileSize << 8;
            }
        }
    }
    else
    {
        return 1;
    }

    fileNameStart = 5 + lengthFileSize;

    if (packet[fileNameStart - 2] == TYPE_FILE_NAME)
    {

        lengthFileName = (int) packet[4 + lengthFileSize];

        for (int i = 0; i < lengthFileName; i++)
        {
            fileName[i] = packet[fileNameStart + i];
        }
    }
    else
    {
        return -1;
    }

    return 0;
}


void applicationLayer(const char *serialPort, const char *role, int baudRate,
                      int nTries, int timeout, const char *filename)
{
    LinkLayer linklayer;
    strcpy(linklayer.serialPort, serialPort);
    linklayer.role = role;
    linklayer.baudRate = baudRate;
    linklayer.nRetransmissions = nTries;
    linklayer.timeout = timeout;

    llopen(linklayer);
}
