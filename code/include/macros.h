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

