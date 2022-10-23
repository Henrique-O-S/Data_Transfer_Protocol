// Link layer protocol implementation

#include "unistd.h"

#include "link_layer.h"
#include "auxiliar.h"
#include "frame.h"
#include "port_operations.h"
#include "macros.h"
#include "alarm.h"

// MISC
#define _POSIX_SOURCE 1 // POSIX compliant source

int fd;
unsigned char frame[MAX_SIZE_FRAME];
unsigned int frameLength;
typedef enum sequence_number {
    NS0,
    NS1
} sequence_number_st;
sequence_number_st seqNumber = 0;

////////////////////////////////////////////////
// LLOPEN
////////////////////////////////////////////////
int llopen(LinkLayer connectionParameters)
{

    if((fd = set_up_port(connectionParameters.serialPort)) == -1)
        return -1

    // install alarm handler here

    frameLength = BUF_SIZE_SF;

    if(connectionParameters.role == LlTx){

        unsigned char responseBuffer[BUF_SIZE_SF];

        if (createSFrame(frame, SET, FIELD_A_T_INIT) != 0){
            close_port(fd);
            return -1;
        }

        if (sendFrame(frame, fd, frameLength) == -1){
            close_port(fd);
            return -1;
        }

        printf("SET sent\n");

        int stop = FALSE;
        int bytes = -1
        relay = FALSE;


        while(!stop){
            bytes = readSFrame(frame, fd, UA, 1, 0x03)

            if(relay){
                sendFrame(frame, fd, frameLength);
                relay = FALSE;
            }

            if(bytes >= 0){
                alarm(0);
                stop = TRUE;
            }
        }

        if(bytes == -1){
            printf("Couldn't read UA. Closing\n")
            close_port(fd);
            return -1;
        }

        printf("UA received\n");

        return fd;
    }

    if(connectionParameters.role == LlRx){

        unsigned char controlByte[1] = {SET};

        if (readSFrame(frame, fd, controlByte, 1, FIELD_A_T_INIT) != 0){
            close_port(fd);
            return -1;
        }
        printf("SET received\n");

        if(createSFrame(frame, UA, FIELD_A_T_INIT) != 0){
            close_port(fd);
            return -1;
        }

        if(sendFrame(frame, fd, frameLength) < 0){
            close_port(fd);
            return -1;
        }
        printf("UA sent\n");

        return fd;
    }

    perror("Invalid role");
    close_port(fd);
    return -1;
}

////////////////////////////////////////////////
// LLWRITE
////////////////////////////////////////////////
int llwrite(const unsigned char *buf, int bufSize)
{
    unsigned char responseBuf[BUF_SIZE_SF];

    unsigned char controlByte;

    if(seqNumber == 0){
        controlByte = NS0;
    }
    else{
        controlByte = NS1;
    }

    if(createIFrame(frame, controlByte, buf, bufSize) != 0){
        close_port(fd);
        return -1;
    }

    int length;
    int frameSize = bufSize + 6;

    if((length = stuffIFrame(frame, frameSize)) < 0){
        close_port(fd);
        return -1;
    }
    
    frameLength = length;
    int bytesWritten;
    int finished = FALSE;

    while(!finished){
        if((bytesWritten = sendFrame(frame, fd, frameLength)) == -1){
            close_port(fd);
            return -1;
        }

        printf("Information sent\n");

        int bytesRead = -1;
        stop = 0;
        currentRetransmission = 0;
        relay = FALSE;

        alarm(linklayer.timeout);

        int controlBytes[2];

        if(controlByte == 0){
            controlBytes[0] = RR1;
            controlBytes[1] = REJ0;
        }
        else{
            controlBytes[0] = RR0;
            controlBytes[1] = REJ1;
        }

        while(!stop){
            bytesRead = readSFrame(responseBuf, fd, controlBytes, 2, FIELD_A_T_INIT);

            if(relay){
                if(sendFrame(frame, fd, frameLength) < 0){
                    close_port(fd);
                    return -1
                }
                relay = false;
            }

            if (bytesRead >= 0){
                alarm(0);
                stop = TRUE;
            }
        }

        if (bytesRead < 0){
            printf("Troubles reading. Closing\n");
            close_port(fd);
            return -1
        }
        else if(bytesRead == 0){
            finished = TRUE;
        }
        else{
            finished = FALSE;
        }

        printf("Response read\n");
    }

    if(seqNumber == NS0){
        seqNumber = NS1;
    }
    else{
        seqNumber = NS0;
    }

    return bytesWritten - 6;
}

////////////////////////////////////////////////
// LLREAD
////////////////////////////////////////////////
int llread(unsigned char *packet)
{
    unsigned char controlBytes[2];
    controlBytes[0] = NS0;
    controlBytes[1] = NS1;
    int packetSize, bytesRead, bufferFull = FALSE;
    
    while(!bufferFull){

        if((bytesRead = readIFrame(frame, fd, controlBytes, 2, FIELD_A_T_INIT)) < 0){
            printf("Troubles reading information. Closing\n");
            close_port(fd);
            return -1
        }

        if((packetSize = unstuffIFrame(frame, bytesRead)) < 0){
            printf("Troubles unstuffing. Closing\n");
            close_port(fd);
            return -1
        }

        int controlByte;
        if(frame[2] == NS0){
            controlByte = 0;
        }
        else{
            controlByte = 1;
        }
        int responseByte;

        if (frame[packetSize - 2] == dataBCC(&frame[4], packetSize - 6)){
            if(controlByte != seqNumber){
                if(controlByte == 0){
                    seqNumber = 1;
                    responseByte = RR1;
                }
                else{
                    seqNumber = 0;
                    responseByte == RR0;
                }
            }
            else{
                for (int i = 0; i < packetSize - 6; i++){
                    packet[i] = frame[4 + i];
                }

                bufferFull = TRUE;

                if(controlByte == 0){
                    seqNumber = 1;
                    responseByte = RR1;
                }
                else{
                    seqNumber = 0;
                    responseByte == RR0;
                }
            }
        }
        else{
            if(controlByte != seqNumber){
                if(controlByte == 0){
                    seqNumber = 1;
                    responseByte = RR1;
                }
                else{
                    seqNumber = 0;
                    responseByte == RR0;
                }
            }
            else{
                if(controlByte == 0){
                    seqNumber = 0;
                    responseByte = REJ0;
                }
                else{
                    seqNumber = 1;
                    responseByte == REJ1;
                }
            }
        }

        if((createSFrame(frame, FIELD_A_T_INIT, responseByte)) != 0){
            printf("Troubles sending response. Closing\n");
            close_port(fd);
            return -1
        }

        frameLength = BUF_SIZE_SF;

        if(sendFrame(frame, fd, frameLength) < 0){
            printf("Troubles sending response. Closing\n");
            close_port(fd);
            return -1
        }

        printf("Response sent\n");
    }

    return packetSize - 6;
}

////////////////////////////////////////////////
// LLCLOSE
////////////////////////////////////////////////
int llclose(int showStatistics)
{
    // TODO

    return 1;
}