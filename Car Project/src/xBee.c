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
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P2, GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN4, GPIO_PRIMARY_MODULE_FUNCTION);
    UART_initModule(EUSCI_A1_BASE, &xBeeConfig);
    UART_enableModule(EUSCI_A1_BASE);
    NVIC_IPR4 = (NVIC_IPR4&0xFFFF00FF)|0x00004000;
    NVIC_ISER0 = 0x00020000;
    UCA1CTLW0 &= ~0x0001;
    UCA1IE = 0x0001;
}


//------------UART_InChar------------
// Wait for new serial port input
// Input: none
// Output: ASCII code for key typed
// spin if RxFifo is empty
uint8_t xBee_InByte(void){
    uint8_t letter;
    while(xBeeRxFifo_Get(&letter) == xBee_FAIL){};
    return(letter);
}

//------------UART_OutChar------------
// Output 8-bit to serial port
// Input: letter is an 8-bit ASCII character to be transferred
// Output: none
// spin if TxFifo is full
void xBee_OutByte(uint8_t byte){
    while(xBeeTxFifo_Put(byte) == xBee_FAIL){}; // spin if full
    UCA1IE = 0x0003;     // enable interrupts on transmit empty and receive full
}

// interrupt 16 occurs on either:
// UCTXIFG TX data register is empty
// UCRXIFG RX data register is full
// vector at 0x00000080 in startup_msp432.s
void EUSCIA1_IRQHandler(void){
        uint8_t data; 
    if(UCA1IFG&0x02){             // TX data register empty
        if(xBeeTxFifo_Get(&data) == xBee_FAIL){
        UCA1IE = 0x0001;         // disable interrupts on transmit empty
        }else{
        UCA1TXBUF = data;        // send data, acknowledge interrupt
        }
    }
    if(UCA1IFG&0x01){             // RX data register full
        xBeeRxFifo_Put((uint8_t)UCA1RXBUF);// clears UCRXIFG
    }
}
