// Output.c

#include "Master.h"

FILE* const uart = (FILE*) 254;
FILE* const null = (FILE*) 255;

#define UART_SUCCESS 1
#define UART_FAILURE 0
AddIndexFifo(Tx, UART_FIFO_SIZE, char, UART_SUCCESS, UART_FAILURE)
AddIndexFifo(Rx, UART_FIFO_SIZE, char, UART_SUCCESS, UART_FAILURE)

void UART_Init(void) {

    SYSCTL_RCGCUART_R |= 0x01;
    SYSCTL_RCGCGPIO_R |= 0x01;

    TxFifo_Init();
    RxFifo_Init();

    UART0_CTL_R &= ~UART_CTL_UARTEN;
    UART0_CC_R = UART_CC_CS_PIOSC;    // Use 16Mhz PIOSC Clock;
    UART0_IBRD_R = 8;
    UART0_FBRD_R = 44;
    UART0_LCRH_R = (UART_LCRH_WLEN_8|UART_LCRH_FEN);
    UART0_IFLS_R = (UART_IFLS_TX1_8|UART_IFLS_RX1_8);
    UART0_IM_R = (UART_IM_RXIM|UART_IM_TXIM|UART_IM_RTIM);
    UART0_CTL_R |= 0x301;

    GPIO_PORTA_AFSEL_R |= 0x03;
    GPIO_PORTA_DEN_R |= 0x03;
    GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R&0xFFFFFF00)+0x00000011;
    GPIO_PORTA_AMSEL_R &= ~0x03;
    NVIC_PRI1_R = (NVIC_PRI1_R&0xFFFF00FF)|0x00000000;
    NVIC_EN0_R |= NVIC_EN0_INT5;

}

static void copyHardwareToSoftware(void) {
    char letter = 0;
    while(((UART0_FR_R&UART_FR_RXFE) == 0) && (RxFifo_Size() < (UART_FIFO_SIZE-1))) {
        letter = UART0_DR_R;
        RxFifo_Put(letter);
    }
}

static void copySoftwareToHardware(void) {
    char letter = 0;
    while(((UART0_FR_R&UART_FR_TXFF) == 0) && (TxFifo_Size() > 0)) {
        TxFifo_Get(&letter);
        UART0_DR_R = letter;
    }
}

char UART_InChar(void) {
    char letter;
    while(RxFifo_Get(&letter) == UART_FAILURE);
    return (letter);
}

void UART_OutChar(char data) {
    while(TxFifo_Put(data) == UART_FAILURE);
    UART0_IM_R &= ~UART_IM_TXIM;
    copySoftwareToHardware();
    UART0_IM_R |= UART_IM_TXIM;
}

void UART0_Handler(void) {
    if(UART0_RIS_R&UART_RIS_TXRIS) {
        UART0_ICR_R = UART_ICR_TXIC;
        copySoftwareToHardware();
        if(TxFifo_Size() == 0) {
            UART0_IM_R &= ~UART_IM_TXIM;
        }
    }
    if(UART0_RIS_R&UART_RIS_RXRIS) {
        UART0_ICR_R = UART_ICR_RXIC;
        copyHardwareToSoftware();
    }
    if(UART0_RIS_R&UART_RIS_RTRIS) {
        UART0_ICR_R = UART_ICR_RTIC;
        copyHardwareToSoftware();
    }
}

int fputc(int ch, FILE* f) {
    if(f == uart) {
        UART_OutChar(ch);
        if(ch == 10) {
            UART_OutChar(13);
        }
        return (0);
    }
    return (1);
}

int fgetc(FILE* f) {
    char ch;
    if(f == uart) {
        if(RxFifo_Size() > 0) {
            ch = UART_InChar();
        }
        return (ch);
    }
    return (EOF);
}

void Output_Init(void) {
    UART_Init();
}
