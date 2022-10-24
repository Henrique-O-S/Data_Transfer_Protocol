#ifndef _MACROS_H_
#define _MACROS_H_


// MISC
#define FALSE 0
#define TRUE 1

// CAMPO DE ENDEREÇO - A
#define FIELD_A_T_INIT 0x03   //campo de endereço  Comandos enviados pelo Emissor e Respostas enviadas pelo Receptor
#define FIELD_A_R_INIT 0x01   //                   Comandos enviados pelo Receptor e Respostas enviadas pelo Emissor



#define FLAG 0x7E
#define FLAG_STUF1 0x7D
#define FLAG_STUF2 0x5E
#define ESC_BYTE 0x7D
#define ESC_BYTE_STUF1 0x7D
#define ESC_BYTE_STUF2 0x5D


#define TYPE_FILE_SIZE 0x00

#define TYPE_FILE_NAME 0x01

#define BYTE_SIZE 256

#define MAX_DATA_SIZE   1024 // max size of a data packet
#define MAX_PACK_SIZE   (MAX_DATA_SIZE + 4) // max size of a data packet + 4 bytes for packet head
#define MAX_SIZE        (MAX_PACK_SIZE + 6) // max size of data in a frame, + 4 bytes for packet head, + 6 bytes for frame header and tail
#define MAX_SIZE_FRAME  (((MAX_PACK_SIZE + 1) * 2) + 5) // max size of a frame, with byte stuffing considered ((1029 * 2) + 5)
                             // 1029 -> all bytes that can suffer byte stuffing (and therefore be "duplicated"), that is, the packet and the BCC2
                             // 5 -> the bytes that won't suffer byte stuffing (flags, bcc1, address and control bytes)

#define BUF_SIZE_SF    5 // size of a supervision frame

#define SUPERVISION 0
#define INFORMATION 1

// CONTROL FIELD - C - TRAMAS DE SUPERVISAO - S - TRAMAS NAO NUMERADAS - U
#define SET 0x03
#define DISC 0x0B
#define UA 0x07
#define RR0 0x05
#define RR1 0x85
#define REJ0 0x01
#define REJ1 0x81
#define NS0 0x00
#define NS1 0x40

#define DATA_TRANSFER 0x01
#define START_TRANSFER 0x02
#define END_TRANSFER 0x03

#endif // _MACROS_H_


