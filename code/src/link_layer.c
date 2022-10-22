// Link layer protocol implementation

#include "link_layer.h"
#include "port_operations.h"

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

    if(connectionParameters.role == LlTx){
        unsigned char addressField[1] = SET;
    

    }


    if(connectionParameters.role == LlRx){
        unsigned char responseBuffer[BUF_SIZE_SF];

        createSFrame(unsigned char* frame, unsigned char SET, unsigned char SET);

        frameLength = BUF_SIZE_SF;
        
        
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
