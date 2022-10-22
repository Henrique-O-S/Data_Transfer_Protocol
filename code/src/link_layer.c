// Link layer protocol implementation

#include "unistd.h"

#include "link_layer.h"
#include "port_operations.h"
#include "macros.h"

// MISC
#define _POSIX_SOURCE 1 // POSIX compliant source

unsigned char frame[MAX_SIZE_FRAME];
unsigned int frameLength;

////////////////////////////////////////////////
// LLOPEN
////////////////////////////////////////////////
int llopen(LinkLayer connectionParameters)
{
    int fd;

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
            printf("Couldn't read UA. Closing")
            close_port(fd);
            return -1;
        }

        printf("UA received");

        return fd;
    }

    if(connectionParameters.role == LlRx){

        unsigned char controlByte[1] = {SET};

        if (readSFrame(frame, fd, controlByte, 1, FIELD_A_T_INIT) != 0){
            close_port(fd);
            return -1;
        }
        printf("SET received");

        if(createSFrame(frame, UA, FIELD_A_T_INIT) != 0){
            close_port(fd);
            return -1;
        }

        if(sendFrame(frame, fd, frameLength) < 0){
            close_port(fd);
            return -1;
        }
        printf("UA sent");

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
    // TODO

    return 0;
}

////////////////////////////////////////////////
// LLREAD
////////////////////////////////////////////////
int llread(unsigned char *packet)
{
    // TODO

    return 0;
}

////////////////////////////////////////////////
// LLCLOSE
////////////////////////////////////////////////
int llclose(int showStatistics)
{
    // TODO

    return 1;
}
