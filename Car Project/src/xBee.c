// xBee.c

#include "Master.h"
#include "uart.h"
#include "gpio.h"

#define xBee_SUCCESS 1
#define xBee_FAIL    0
AddIndexFifo(xBeeRx, xBee_FIFO_SIZE, uint8_t, xBee_SUCCESS, xBee_FAIL)
AddIndexFifo(xBeeTx, xBee_FIFO_SIZE, uint8_t, xBee_SUCCESS, xBee_FAIL)

const eUSCI_UART_Config xBeeConfig = {
    EUSCI_A_UART_CLOCKSOURCE_SMCLK,                 // SMCLK Clock Source -> 48MHz
    26,                                             // BRDIV = 26
    0,                                              // UCxBRF = 0
    111,                                            // UCxBRS = 111
    EUSCI_A_UART_NO_PARITY,                         // No Parity
    EUSCI_A_UART_LSB_FIRST,                         // LSB First
    EUSCI_A_UART_ONE_STOP_BIT,                      // One stop bit
    EUSCI_A_UART_MODE,                              // UART mode
    EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION   // Oversampling
};

void xBee_Init(void){
    xBeeRxFifo_Init();
    xBeeTxFifo_Init();
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P3, GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);
    UART_initModule(EUSCI_A2_BASE, &xBeeConfig);
    UART_enableModule(EUSCI_A2_BASE);
    NVIC_IPR4 = (NVIC_IPR4&0xFF00FFFF)|0x00400000;
    NVIC_ISER0 = 0x00040000;
    UCA2CTLW0 &= ~0x0001;
    UCA2IE = 0x0001;
}

uint8_t xBee_InByte(void){
    uint8_t byte;
    while(xBeeRxFifo_Get(&byte) == xBee_FAIL){};
    return(byte);
}

void xBee_OutByte(uint8_t byte){
    while(xBeeTxFifo_Put(byte) == xBee_FAIL){};
    UCA2IE = 0x0003;
}

void xBee_InCmd(uint8_t* left, uint8_t* right){
    uint8_t byte;
    while(xBeeRxFifo_Get(&byte) == xBee_FAIL){};
    (ID(byte)) ? (*right = byte) : (*left = byte);
    while(xBeeRxFifo_Get(&byte) == xBee_FAIL){};
    (ID(byte)) ? (*right = byte) : (*left = byte);
}

void EUSCIA2_IRQHandler(void){
    uint8_t data; 
    if(UCA2IFG&0x02){
        if(xBeeTxFifo_Get(&data) == xBee_FAIL){
            UCA2IE = 0x0001;
        }else{
            UCA2TXBUF = data;
        }
    }
    if(UCA2IFG&0x01){
        xBeeRxFifo_Put((uint8_t)UCA2RXBUF);
    }
}
