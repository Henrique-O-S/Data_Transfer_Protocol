// Read from serial port in non-canonical mode
//
// Modified by: Eduardo Nuno Almeida [enalmeida@fe.up.pt]

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <time.h>

// Baudrate settings are defined in <asm/termbits.h>, which is
// included by <termios.h>
#define BAUDRATE B38400
#define _POSIX_SOURCE 1 // POSIX compliant source

#define FALSE 0
#define TRUE 1

#define BUF_SIZE 256

volatile int STOP = FALSE;

int alarmEnabled = FALSE;
int alarmCount = 0;

unsigned char buf[BUF_SIZE] = {0}; // +1: Save space for the final '\0' char
unsigned int count = 0;
unsigned int bytes;
int fd, len, empty = 1;

/*
void alarmHandler(int signal)
{
    empty = 1;
    memset(buf, 0, sizeof(buf));
    memset(localbuf, 0, sizeof(localbuf));
    for (unsigned int count = 0; count < BUF_SIZE; count++)
    {
        bytes = read(fd, buf, 1);
        if (bytes == 1 && empty == 1)
        {
            empty = 0;
        }
        strcat(localbuf, buf);
        len++;
        if (buf == '\0')
        {
            break;
        }
    }
    if (empty == 0)
    {
        write(fd, localbuf, BUF_SIZE);
        sleep(1);
    }

    alarmEnabled = FALSE;
    alarmCount++;

    printf("Alarm #%d\n", alarmCount);
}
*/

int main(int argc, char *argv[])
{
    // Program usage: Uses either COM1 or COM2
    const char *serialPortName = argv[1];

    if (argc < 2)
    {
        printf("Incorrect program usage\n"
               "Usage: %s <SerialPort>\n"
               "Example: %s /dev/ttyS1\n",
               argv[0],
               argv[0]);
        exit(1);
    }

    // Open serial port device for reading and writing and not as controlling tty
    // because we don't want to get killed if linenoise sends CTRL-C.
    int fd = open(serialPortName, O_RDWR | O_NOCTTY);
    if (fd < 0)
    {
        perror(serialPortName);
        exit(-1);
    }

    struct termios oldtio;
    struct termios newtio;

    // Save current port settings
    if (tcgetattr(fd, &oldtio) == -1)
    {
        perror("tcgetattr");
        exit(-1);
    }

    // Clear struct for new port settings
    memset(&newtio, 0, sizeof(newtio));

    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    // Set input mode (non-canonical, no echo,...)
    newtio.c_lflag = 0;
    newtio.c_cc[VTIME] = 0; // Inter-character timer unused
    newtio.c_cc[VMIN] = 5;  // Blocking read until 5 chars received

    // VTIME e VMIN should be changed in order to protect with a
    // timeout the reception of the following character(s)

    // Now clean the line and activate the settings for the port
    // tcflush() discards data written to the object referred to
    // by fd but not transmitted, or data received but not read,
    // depending on the value of queue_selector:
    //   TCIFLUSH - flushes data received but not read.
    tcflush(fd, TCIOFLUSH);

    // Set new port settings
    if (tcsetattr(fd, TCSANOW, &newtio) == -1)
    {
        perror("tcsetattr");
        exit(-1);
    }

    printf("New termios structure set\n");

    // Loop for input

    unsigned char localbuf[BUF_SIZE] = {0};
    unsigned char localbufcpy[BUF_SIZE] = {0};

    clock_t start, end;
    double cpu_time_used; 
    unsigned char disclosure[BUF_SIZE] = "Fecha a loja";

    do{

        //memset(localbuf, 0, BUF_SIZE+1*sizeof(char));
        start = clock();
        bytes = read(fd, buf, BUF_SIZE);
        //printf("%s\n",buf);
        if (bytes == 0){
            end = clock();
            cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
            if (cpu_time_used > 3){
                printf("Read Time Exceeded");
                if (tcsetattr(fd, TCSANOW, &oldtio) == -1){
                    perror("tcsetattr");
                    exit(-1);
                }

                close(fd);

                return 0;
            }
        }
        else{
            strcpy(localbuf, buf);
            //printf("%s\n", localbuf);
        }

        //printf("%s\n", localbuf);
        STOP = strcmp(localbuf, disclosure) == 0; // SAME -> STOP = TRUE
        /*
        for(int i = 0; i < 20; i++){
            printf("|%c|", localbuf[i]);
        }
        printf("\n%s\n", localbuf);
        */

        if(STOP == FALSE){
            strcpy(localbufcpy, localbuf);
            //printf("%s\n", localbuf);
            write(fd, localbufcpy, BUF_SIZE);

            sleep(1);

            //printf("%s\n", localbufcpy);
        }
    } while(STOP == FALSE);

    printf("%s\n", localbufcpy);



    sleep(1);

    // The while() cycle should be changed in order to respect the specifications
    // of the protocol indicated in the Lab guide

    // Restore the old port settings
    if (tcsetattr(fd, TCSANOW, &oldtio) == -1)
    {
        perror("tcsetattr");
        exit(-1);
    }

    close(fd);

    return 0;
}

