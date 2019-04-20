// RemoteMain.c

#include "Master.h"

uint32_t polarityFlag = 0;
uint32_t Left_Speed = 0;
uint32_t Left_Sign = PLUS;
uint32_t Right_Speed = 0;
uint32_t Right_Sign = PLUS;
uint32_t message = 0;
uint32_t messageFlag = 0;

void RemoteMain(void){

    DisableInterrupts();
    PLL_Init(Bus80MHz);
    Output_Init();
    ADC_Init();
    GPIO_Init();
    xBee_Init();
    EnableInterrupts();
    while(1){
        if(messageFlag){
            xBee_OutMessage(message);
            message = 0;
            messageFlag = false;
        }
    }

}
