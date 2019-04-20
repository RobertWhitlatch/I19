// xBee.c

#include "Master.h"

#define xBee_SUCCESS 1
#define xBee_FAILURE 0
AddIndexFifo(xBeeTx, xBee_FIFO_SIZE, char, xBee_SUCCESS, xBee_FAILURE)
AddIndexFifo(xBeeRx, xBee_FIFO_SIZE, char, xBee_SUCCESS, xBee_FAILURE)

void xBee_Init(void){

    SYSCTL_RCGCGPIO_R |= 0x10;                                   // Clock on for Ports E
    SYSCTL_RCGCUART_R |= 0x80;                                   // Clock on for UART7

    GPIO_PORTE_LOCK_R = GPIO_LOCK_KEY;                          // Unlock Port E
    GPIO_PORTE_CR_R = 0xFF;                                     // Enable Commits on Port E
    GPIO_PORTE_AMSEL_R = 0x0C;                                  // Enable Analog Mode on PE2 and PE3 (AIN1 and AIN0)
    GPIO_PORTE_AFSEL_R = 0x0F;                                  // Enable Alternate Functions on PE0, PE1, PE2, and PE3
    GPIO_PORTE_PCTL_R = (GPIO_PORTE_PCTL_R & 0x000000FF) | 0x00000011; // Set PE1 and PE0 to UART7
    GPIO_PORTE_DIR_R &= ~0x03;                                    // Set Port E as out
    GPIO_PORTE_DEN_R |= 0x03;                                    // Enable Digital Operations on PE0 and PE1

    xBeeTxFifo_Init();
    xBeeRxFifo_Init();

    UART7_CTL_R &= ~UART_CTL_UARTEN;
    UART7_CC_R = UART_CC_CS_PIOSC;    // Use 16Mhz PIOSC Clock;
    UART7_IBRD_R = 8;
    UART7_FBRD_R = 44;
    UART7_LCRH_R = (UART_LCRH_WLEN_8|UART_LCRH_FEN);
    UART7_IFLS_R = (UART_IFLS_TX1_8|UART_IFLS_RX1_8);
    UART7_IM_R = (UART_IM_RXIM|UART_IM_TXIM|UART_IM_RTIM);
    UART7_CTL_R |= 0x301;
    NVIC_PRI15_R = (NVIC_PRI15_R&0x00FFFFFF)|0x40000000;
    NVIC_EN1_R |= NVIC_EN0_INT31;

}

static void xBeeCopyHardwareToSoftware(void) {
    char byte = 0;
    while(((UART7_FR_R&UART_FR_RXFE) == 0) && (xBeeRxFifo_Size() < (xBee_FIFO_SIZE-1))) {
        byte = UART7_DR_R;
        xBeeRxFifo_Put(byte);
    }
}

static void xBeeCopySoftwareToHardware(void) {
    char byte = 0;
    while(((UART7_FR_R&UART_FR_TXFF) == 0) && (xBeeTxFifo_Size() > 0)) {
        xBeeTxFifo_Get(&byte);
        UART7_DR_R = byte;
    }
}

uint8_t xBee_InByte(void) {
    char byte;
    while(xBeeRxFifo_Get(&byte) == xBee_FAILURE);
    return (byte);
}

void xBee_OutByte(uint8_t byte) {
    while(xBeeTxFifo_Put(byte) == xBee_FAILURE);
    UART7_IM_R &= ~UART_IM_TXIM;
    xBeeCopySoftwareToHardware();
    UART7_IM_R |= UART_IM_TXIM;
}

void UART7_Handler(void) {
    if(UART7_RIS_R&UART_RIS_TXRIS) {
        UART7_ICR_R = UART_ICR_TXIC;
        xBeeCopySoftwareToHardware();
        if(xBeeTxFifo_Size() == 0) {
            UART7_IM_R &= ~UART_IM_TXIM;
        }
    }
    if(UART7_RIS_R&UART_RIS_RXRIS) {
        UART7_ICR_R = UART_ICR_RXIC;
        xBeeCopyHardwareToSoftware();
    }
    if(UART7_RIS_R&UART_RIS_RTRIS) {
        UART7_ICR_R = UART_ICR_RTIC;
        xBeeCopyHardwareToSoftware();
    }
}

uint32_t xBee_InMessage(void){

    uint32_t msg = 0;
    uint8_t byte = 0;
    byte = xBee_InByte();
    msg += byte;
    msg <<= 8;
    byte = xBee_InByte();
    msg += byte;
    msg <<= 8;
    byte = xBee_InByte();
    msg += byte;
    msg <<= 8;
    byte = xBee_InByte();
    msg += byte;
    return (msg);
}

void xBee_OutMessage(uint32_t msg){

    uint16_t delay; 
    uint8_t byte = 0;
    byte = (msg & 0xFF000000) >> 24;
    xBee_OutByte(byte);
    delay = 1000;
    while(delay){
        delay--;
    }
    byte = (msg & 0x00FF0000) >> 16;
    xBee_OutByte(byte);
    delay = 1000;
    while(delay){
        delay--;
    }
    byte = (msg & 0x0000FF00) >> 8;
    xBee_OutByte(byte);
    delay = 1000;
    while(delay){
        delay--;
    }
    byte = (msg & 0x000000FF);
    xBee_OutByte(byte);
    delay = 400;
    while(delay){
        delay--;
    }

}

void xBee_Packager(void){

    Left_Speed = (Left_Speed > 0x7FF) ? 0x7FF : Left_Speed;
    Right_Speed = (Right_Speed > 0x7FF) ? 0x7FF : Right_Speed;
    if(polarityFlag){
        Right_Sign ^= 0x1;
        Left_Sign ^= 0x1;
    }
    message += (Left_Speed & SPEED_MASK) << LEFT_SPEED_OFFSET;
    message += (Left_Sign) << LEFT_SIGN_OFFSET;
    message += (Right_Speed & SPEED_MASK) << RIGHT_SPEED_OFFSET;
    message += (Right_Sign) << RIGHT_SIGN_OFFSET;

}
