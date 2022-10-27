// Link layer protocol implementation

#include "unistd.h"
#include <sys/time.h>
#include <stdio.h>

#include "link_layer.h"
#include "frame.h"
#include "port_operations.h"

// MISC
#define _POSIX_SOURCE 1 // POSIX compliant source

int fd;
char *serialPort;
LinkLayerRole role;
int baudRate;
int nRetransmissions;
int timeout;
unsigned char frame[MAX_SIZE_FRAME];
unsigned int frameLength;
int seqNumber;
extern int currentRetransmission, relay, stop;
int bitsReceived = 0;
struct timeval start, end;

////////////////////////////////////////////////
// LLOPEN
////////////////////////////////////////////////
int llopen(LinkLayer connectionParameters)
{
    serialPort = connectionParameters.serialPort;
    role = connectionParameters.role;
    baudRate = connectionParameters.baudRate;
    nRetransmissions = connectionParameters.nRetransmissions;
    timeout = connectionParameters.timeout;

    if((fd = set_up_port(serialPort)) == -1){
        printf("Couldn't open port communication");
        return -1;
    }
    
    alarmHandlerInstaller();

    frameLength = BUF_SIZE_SF;

    if(connectionParameters.role == LlTx){

        unsigned char responseBuffer[BUF_SIZE_SF];

        if (createSFrame(frame, FIELD_A_T_INIT, SET) != 0){
            close_port(fd);
            return -1;
        }

        if (sendFrame(frame, fd, frameLength) == -1){
            close_port(fd);
            return -1;
        }

        printf("SET sent\n");

        stop = FALSE;
        int bytesRead = -1;
        currentRetransmission = 0;
        relay = FALSE;

        alarm(timeout);
        
        unsigned char controlByte[1] = {UA};
        while(!stop){
            bytesRead = readSFrame(responseBuffer, fd, controlByte, 1, FIELD_A_T_INIT);

            if(relay){
                sendFrame(frame, fd, frameLength);
                relay = FALSE;
            }

            if(bytesRead >= 0){
                alarm(0);
                stop = TRUE;
            }
        }

        if(bytesRead == -1){
            printf("Couldn't read UA. Closing file\n");
            close_port(fd);
            return -1;
        }

        printf("UA received\n");

        return 1;
    }

    if(connectionParameters.role == LlRx){

        unsigned char controlByte[1] = {SET};

        if (readSFrame(frame, fd, controlByte, 1, FIELD_A_T_INIT) != 0){
            close_port(fd);
            return -1;
        }
        printf("SET received\n");

        if(createSFrame(frame, FIELD_A_T_INIT, UA) != 0){
            close_port(fd);
            return -1;
        }

        if(sendFrame(frame, fd, frameLength) < 0){
            close_port(fd);
            return -1;
        }
        printf("UA sent\n");

        gettimeofday(&start, NULL);
        printf("Time spent =  %f\n", start.tv_sec);

        return 1;
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
        stop = FALSE;
        currentRetransmission = 0;
        relay = FALSE;

        alarm(timeout);

        unsigned char controlBytes[2];

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
                    printf("Troubles resending. Closing file\n");
                    close_port(fd);
                    return -1;
                }
                relay = FALSE;
            }

            if (bytesRead >= 0){
                alarm(0);
                stop = TRUE;
            }
        }

        if (bytesRead < 0){
            printf("Troubles reading. Closing file\n");
            close_port(fd);
            return -1;
        }
        else if(bytesRead == 0){
            finished = TRUE;
        }
        else{
            finished = FALSE;
        }

        printf("Response read\n");
    }

    if(seqNumber == 0){
        seqNumber = 1;
    }
    else{
        seqNumber = 0;
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

        bytesRead = readIFrame(frame, fd, controlBytes, 2, FIELD_A_T_INIT);

        if((packetSize = unstuffIFrame(frame, bytesRead)) < 0){
            printf("Troubles unstuffing. Closing file\n");
            close_port(fd);
            return -1;
        }

        int controlByte;
        if(frame[2] == NS0){
            controlByte = 0;
        }
        else if(frame[2] == NS1){
            controlByte = 1;
        }
        else{
            printf("Control byte wrongly defined. Closing file\n");
            close_port(fd);
            return -1;
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
                    responseByte = RR0;
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
                    responseByte = RR0;
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
                    responseByte = RR0;
                }
            }
            else{
                if(controlByte == 0){
                    seqNumber = 0;
                    responseByte = REJ0;
                }
                else{
                    seqNumber = 1;
                    responseByte = REJ1;
                }
            }
        }

        if((createSFrame(frame, FIELD_A_T_INIT, responseByte)) != 0){
            printf("Troubles sending response. Closing file\n");
            close_port(fd);
            return -1;
        }

        frameLength = BUF_SIZE_SF;

        if(sendFrame(frame, fd, frameLength) < 0){
            printf("Troubles sending response. Closing file\n");
            close_port(fd);
            return -1;
        }

        printf("Response sent\n");
    }

    bitsReceived += (packetSize - 6) * 8;
    return packetSize - 6;
}

////////////////////////////////////////////////
// LLCLOSE
////////////////////////////////////////////////
int llclose(int showStatistics)
{
    if(role == LlTx){

        if((createSFrame(frame, FIELD_A_T_INIT, DISC)) != 0){
            printf("Troubles sending command. Closing file\n");
            close_port(fd);
            return -1;
        }

        frameLength = BUF_SIZE_SF;
        if(sendFrame(frame, fd, frameLength) < 0){
            printf("Troubles sending command. Closing file\n");
            close_port(fd);
            return -1;
        }

        printf("DISC sent\n");

        stop = FALSE;
        int bytesRead = -1;
        currentRetransmission = 0;
        relay = FALSE;

        alarm(timeout);

        unsigned char controlByte[1] = {DISC};
        while(!stop){

            if((bytesRead = readSFrame(frame, fd, controlByte, 1, FIELD_A_R_INIT)) < 0){
                printf("Error reading DISC\n");
                return -1;
            }

            if(relay){
                sendFrame(frame, fd, frameLength);
                relay = FALSE;
            }

            if(bytesRead >= 0){
                alarm(0);
                stop = TRUE;
            }
        }

        if(bytesRead < 0){
            printf("Troubles reading command. Closing file\n");
            close_port(fd);
            return -1;
        }

        printf("DISC received\n");

        if((createSFrame(frame, FIELD_A_R_INIT, UA)) != 0){
            printf("Troubles sending response. Closing file\n");
            close_port(fd);
            return -1;
        }

        if(sendFrame(frame, fd, frameLength) < 0){
            printf("Troubles sending response. Closing file\n");
            close_port(fd);
            return -1;
        }

        printf("UA sent\n");
    }
    else if(role == LlRx){
        unsigned char controlByteCommand[1] = {DISC};

        if (readSFrame(frame, fd, controlByteCommand, 1, FIELD_A_T_INIT) != 0){
            printf("Troubles receiving command. Closing file\n");
            close_port(fd);
            return -1;
        }

        printf("DISC received\n");

        if(createSFrame(frame, FIELD_A_R_INIT, DISC) != 0){
            printf("Troubles sending command. Closing file\n");
            close_port(fd);
            return -1;
        }

        if(sendFrame(frame, fd, frameLength) < 0){
            printf("Troubles sending command. Closing file\n");
            close_port(fd);
            return -1;
        }
        printf("DISC sent\n");

        stop = FALSE;
        int bytesRead = -1;
        currentRetransmission = 0;
        relay = FALSE;
        frameLength = BUF_SIZE_SF;

        alarm(timeout);

        unsigned char controlByteResponse[1] = {UA};
        while(!stop){

            if((bytesRead = readSFrame(frame, fd, controlByteResponse, 1, FIELD_A_R_INIT)) < 0){
                printf("Error reading UA\n");
                return -1;
            }

            if(relay){
                sendFrame(frame, fd, frameLength);
                relay = FALSE;
            }

            if(bytesRead >= 0){
                alarm(0);
                stop = TRUE;
            }
        }

        if(bytesRead < 0){
            printf("Troubles reading response. Closing file\n");
            close_port(fd);
            return -1;
        }

        printf("UA received\n");

        gettimeofday(&end, NULL);
        printf("Time spent =  %f\n", start.tv_sec);
        double time_spent = end.tv_sec - start.tv_sec;
        time_spent += (end.tv_usec - start.tv_usec) / 1000000;

        if(showStatistics == TRUE){
            printf("Bits Received =  %d\n", bitsReceived);
            printf("Time spent =  %f\n", time_spent);
            double R =  bitsReceived/time_spent;
            double S = R / baudRate;

            printf("Debit received = %lf\n", R);
            printf("Efficiency S = %lf\n", S);
        }
    }
    else{
        perror("Invalid role");
        close_port(fd);
        return -1;
    }

    if(close_port(fd) < 0){
        return -1;
    }

    return 1;
}
