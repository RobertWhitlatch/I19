#include "Master.h"
#include "uart.h"
#include "gpio.h"

const eUSCI_UART_Config uartConfig = {
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

FILE* uart = (FILE*)254;
FILE* null = (FILE*)255;

void STDIO_Init(void){
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN4, GPIO_PRIMARY_MODULE_FUNCTION);
    UART_initModule(EUSCI_A0_BASE, &uartConfig);
    UART_enableModule(EUSCI_A0_BASE);
}

int fputc(int ch, FILE *f){
    if(f == uart){
        UART_transmitData(EUSCI_A0_BASE,ch);
        if(ch == 10){
            UART_transmitData(EUSCI_A0_BASE,13);
        }
        return 1;
    }
    return(EOF);
}

int fgetc (FILE *f){
    char ch = EOF;
    if(f == uart){
        if(BITBAND_PERI(EUSCI_A_CMSIS(EUSCI_A0_BASE)->IFG, EUSCI_A_IFG_RXIFG_OFS)){
            ch = UART_receiveData(EUSCI_A0_BASE);
        }
    }
    return(ch);
}

int ferror(FILE *f){
    return EOF;
}

//-------------------getString--------------------//
// Constructs a string from Console input.
// Input: buffer to hold string, length of buffer
// Output: -1 if string not complete, strlen if enter pressed or length met
//------------------------------------------------//
int getString(char* buff, int buff_length) {
  static int index = 0;
  int ch = fgetc(uart);
  if (ch != (char)EOF) {
    fputc(ch, uart);
    if (ch == '\n' || ch == '\r' || index == buff_length) {
      buff[index] = 0;
      int result = index;
      index = 0;
      return (result);
    } else if (ch == 8) {
      index--;
    } else {
      buff[index] = ch;
      index++;
    }
  }
  return (-1);
}

void Output_Init(void){
    STDIO_Init();
}
