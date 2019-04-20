// GPIO.c

#include "Master.h"

void GPIO_Init(void){

    SYSCTL_RCGCGPIO_R |= 0x20;                                  // Clock on for Ports F
    SYSCTL_RCGCTIMER_R |= 0x02;                                 // Clock on for Timer1

    GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY;                          // Unlock Port F
    GPIO_PORTF_CR_R |= 0x1F;                                    // Enable Commits on Port F
    GPIO_PORTF_AMSEL_R &= ~0x11;                                // Disable Analog Mode
    GPIO_PORTF_AFSEL_R &= ~0x11;                                // Disable Alternate Functions
    GPIO_PORTF_PCTL_R &= ~0x000F000F;                           // Disable Peripherals
    GPIO_PORTF_DIR_R &= ~0x11;                                  // Set Port F as out
    GPIO_PORTF_DEN_R |= 0x11;                                   // Enable Digital Operations on PF0 and PF4
    GPIO_PORTF_PUR_R |= 0x11;                                   // Enable Pull Up Resistors on PF0 and PF4
    GPIO_PORTF_IS_R &= ~0x11;                                   // Interrupts are Edge-Sensitive
    GPIO_PORTF_IEV_R &= ~0x11;                                  // Interrupt on falling edge on PF0 and PF4
    GPIO_PORTF_IBE_R &= ~0x11;                                  // Disable Interrupts on both edges
    GPIO_PORTF_ICR_R |= 0x11;                                   // Clear old interrupts
    GPIO_PORTF_IM_R |= 0x11;                                    // Enable sending interrupts to Controller
    NVIC_EN0_R |= 0x40000000;                                   // Enable Interrupt 30
    NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00400000;          // Set Interrupt 30 priority at 2

}

void GPIOPortF_Handler(void){

    GPIO_PORTF_IM_R &= 0x0E;
    polarityFlag ^= 0x1;
    Timer1_Arm();

}

void Timer1A_Handler(void){

    TIMER1_IMR_R = 0x00000000;      // disarm timeout interrupt
    PortF_Rearm();                  // start GPIO

}

void Timer1_Arm(void){

    TIMER1_CTL_R = 0x00000000;    // 1) disable TIMER0A during setup
    TIMER1_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
    TIMER1_TAMR_R = 0x0000001;    // 3) 1-SHOT mode
    TIMER1_TAILR_R = 0x02625A00;  // 4) 500ms reload value
    TIMER1_TAPR_R = 0;            // 5) bus clock resolution
    TIMER1_ICR_R = 0x00000001;    // 6) clear TIMER0A timeout flag
    TIMER1_IMR_R = 0x00000001;    // 7) arm timeout interrupt
    NVIC_PRI5_R = (NVIC_PRI5_R&0xFFFF00FF)|0x00004000; // 8) priority 4
    NVIC_EN0_R |= 1<<21;           // 9) enable IRQ 19 in NVIC
    TIMER1_CTL_R = 0x00000001;

}

void PortF_Rearm(void){

    GPIO_PORTF_ICR_R = 0x11;
    GPIO_PORTF_IM_R |= 0x11;
    NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00400000;
    NVIC_EN0_R = 0x40000000;

}
