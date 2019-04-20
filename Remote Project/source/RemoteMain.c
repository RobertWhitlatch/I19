// RemoteMain.c

#include "Master.h"

uint8_t polarityFlag = 0;
uint8_t leftMessage = 0;
uint8_t rightMessage = 0;
uint8_t messageFlag = 0;
uint32_t colorIndex = 0;

void RemoteMain(void){

    DisableInterrupts();
    ADC_Init();
    GPIO_Init();
    xBee_Init();
    SysTick_Init();
    EnableInterrupts();

    fprintf(uart, "Remote Control Module\n");

    while(1){
        if(messageFlag){
            messageFlag = false;
            xBee_OutCmd(leftMessage, rightMessage);
            // fprintf(uart, "Left: %2X\tRight: %2X\n", leftMessage, rightMessage);
        }
        // LED = colorIndex << 1;
        // colorIndex = (colorIndex + 1) & 0x07;
        // for(int i = 0; i < 1000000; i++);
    }

}
