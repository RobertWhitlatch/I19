// SysTick.c

#include "Master.h"

void SysTick_Init(void){

    NVIC_ST_CTRL_R = 0;
    NVIC_ST_RELOAD_R = 0x0006C8;                                // Corresponds to 21.701uSec at 80Mhz Clock // 1736.1111
    NVIC_ST_CURRENT_R = 0;                                      // Reset Count Register
    NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x40000000;  // Priority 2
    NVIC_ST_CTRL_R = 0x07;                                      // Active SysTick with Interrupts

}


void SysTick_Handler(void){

    xBee_Packager();
    xBee_OutMessage(message);

}
