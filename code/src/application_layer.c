// Application layer protocol implementation

#include "application_layer.h"
#include "link_layer.h"
#include "auxiliar.h"
#include "macros.h"
#include "file.h"
#include "frame.h"

LinkLayer linklayer;

// BUILDERS CALLED BY WRITER AND PARSERS CALLED BY READER

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

    return dataLength + shift; // returns len of buffer
}

int parseDataPacket(unsigned char *packet, unsigned char *data, int *sequenceNumber)
{
    if (packet[0] != CTRL_DATA)
    {
        return 1;
    }
    *sequenceNumber = (int)packet[1];

    int size;
    int shift = 4;

    util_join_bytes(&size, packet[2], packet[3]);

    for (int i = 0; i < size; i++)
    {
        data[i] = packet[i + shift];
    }
    return 0;
}

int buildControlPacket(unsigned char *packet, unsigned char control, int fileSize, char *fileName)
{
    packet[0] = control;

    packet[1] = TYPE_FILE_SIZE;

    int remainingFileSize = fileSize;

    int byteCount;

    get_size_in_bytes(fileSize, &byteCount);

    packet[2] = byteCount;

    int fileSizeStart = 3;

    for (int i = byteCount - 1; i > 0; i--)
    {
        packet[fileSizeStart + i] = (unsigned char)(fileSize);
        fileSize = fileSize >> 8;
    }

    packet[3 + byteCount] = TYPE_FILE_NAME;

    int filenameSize = strlen(fileName) + 1;
    packet[4 + byteCount] = (unsigned char)filenameSize;

    int fileNameStart = 5 + byteCount;

    for (int i = 0; i < filenameSize; i++)
    {
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
        lengthFileSize = (int)packet[2];
        for (int i = 0; i < lengthFileSize; i++)
        {
            *fileSize = *fileSize | (unsigned char)packet[3 + i];
            if (i != lengthFileSize - 1)
            {
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

        lengthFileName = (int)packet[4 + lengthFileSize];

        for (int i = 0; i < lengthFileName; i++)
        {
            fileName[i] = packet[fileNameStart + i];
        }
    }
    else
    {
        return 1;
    }

    return 0;
}

void applicationLayer(const char *serialPort, const char *role, int baudRate,
                      int nTries, int timeout, const char *filename)
{
    strcpy(linklayer.serialPort, serialPort);
    linklayer.role = role;
    linklayer.baudRate = baudRate;
    linklayer.nRetransmissions = nTries;
    linklayer.timeout = timeout;

    if (role == LlTx)
    {
        if(sendFile(filename, serialPort)){
            return 1;
        }
    }
    else
    {
        if(receiveFile(filename, serialPort)){
            return 1;
        }
    }
}

int sendFile(char *filename, char *serialPort)
{
    FILE *file = openFile(filename, "r");
    if (file == NULL)
    {
        printf("ERROR OPENING FILE!\n");
        return 1;
    }
    if (llopen(linklayer))
    {
        return 1;
    }

    int fileSize = getFileSize(file);
    unsigned char cSPacket[MAX_PACK_SIZE];
    int packetSize = buildControlPacket(cSPacket, START, fileSize, filename);

    if (llwrite(cSPacket, packetSize))
    {
        if (closeFile(file))
        {
            return 1;
        }
        return 1;
    }

    unsigned char data[MAX_DATA_SIZE];
    int bytesRead;
    int n = 0;
    unsigned char dPacket[MAX_PACK_SIZE];

    while (!feof(file))
    {
        bytesRead = readBytesFromFile(file, data);
        if (bytesRead != MAX_DATA_SIZE && !feof(file))
        {
            printf("ERROR READING\n");
            if (closeFile(file))
            {
                return 1;
            }
            return 1;
        }
        packetSize = buildDataPacket(dPacket, getSequenceNumber(n), data, bytesRead);
        n++;
        if (llwrite(dPacket, packetSize))
        {
            if (closeFile(file))
            {
                return 1;
            }
            return 1;
        }
    }
    unsigned char cEPacket[MAX_PACK_SIZE];

    packetSize = buildControlPacket(cEPacket, END_TRANSFER, fileSize, filename);

    if (llwrite(cEPacket, packetSize))
    {
        return 1;
    }

    if (llclose(0))
    {
        return 1;
    }

    if (closeFile(file))
    {
        return 1;
    }
    return 0;
}

int receiveFile(char *filename, char *serialPort){
    unsigned char cSPacket[MAX_PACK_SIZE];
    char filename[255];
    int fileSize;

    int packetSize = llread(cSPacket);

    if (packetSize < 0){
        return 1;
    }

    if(parseControlPacket(cSPacket, &fileSize, filename && cSPacket[0] != CTRL_START)){
        return -1;
    }

    FILE *file = openFile(filename, "w");
    if(file == NULL){
        return 1;
    }

    int n = 0;
    int sequenceNumber;
    unsigned char dPacket[MAX_PACK_SIZE];
    unsigned char data[MAX_DATA_SIZE];
    do
    {
        packetSize = llread(dPacket);
        if(packetSize < 0){
            closeFile(file);
            return 1;
        }

        if(dPacket[0] == CTRL_DATA){
            if(parseDataPacket(dPacket, data, &sequenceNumber)){
                closeFile(file);
                return 1;
            }
            if(sequenceNumber != getSequenceNumber(n)){
                printf("Different sequence numbers");
                closeFile(file);
                return 1;
            }
        }
        if(writeBytesToFile(file, data, packetSize - 4) != packetSize - 4){
            closeFile(file);
            return 1;
        }
    } while (dPacket[0] != CTRL_END);
    closeFile(file);
    unsigned char cEPacket[MAX_PACK_SIZE];

    int newFileSize;
    unsigned char newFileName[255];
    if(parseControlPacket(cEPacket, &newFileSize, newFileName) < 0)
    {
        return -1;
    }

    if(strcmp(filename, newFileName) || fileSize != newFileSize){
        printf("Files aren't the same");
        return -1;
    }

    if(llclose(0)){
        return -1;
    }
    return 0;
}
