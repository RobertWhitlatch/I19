// ADC.c

#include "Master.h"

void ADC_Init(void){

    SYSCTL_RCGCGPIO_R |= 0x10;                                  // Clock on for Port E
    SYSCTL_RCGCADC_R |= 0x03;                                   // Clock on for ADC0 and ADC1
    SYSCTL_RCGCTIMER_R |= 0x01;                                 // Clock on for Timer0 and Timer1 (Timer1 for debounce)

    GPIO_PORTE_LOCK_R = GPIO_LOCK_KEY;                          // Unlock Port E
    GPIO_PORTE_CR_R = 0xFF;                                     // Enable Commits on Port E
    GPIO_PORTE_AMSEL_R |= 0x0C;                                 // Enable Analog Mode on PE2 and PE3 (AIN1 and AIN0)
    GPIO_PORTE_AFSEL_R |= 0x0C;                                 // Enable Alternate Functions on PE0, PE1, PE2, and PE3
    GPIO_PORTE_PCTL_R &= ~0x0000FF00;                           // Enable Analog Functionality for PE3, PE2

    ADC0_ACTSS_R = 0x00;                                        // Disable Sequencers
    ADC0_CC_R = 0x1;                                            // 16Mhz PIOSC Clock
    ADC0_EMUX_R = 0x5000;                                       // Set Sequencer 0 to Timer Triggered // Only Vertical Sticks ATM
    ADC0_SSPRI_R = 0x0123;                                      // Set ADC0, Sequencer 3 at highest priority
    ADC0_SSMUX3_R = 0x0;                                        // Set ADC0 Sequencer 3 to Channel 0 - AIN0 (Port E, Pin 3)
    ADC0_SSCTL3_R = 0x0006;                                     // Disable Internal Temp and Differential Input, Enable Interrupts and End of Sequence
    ADC0_PC_R = 0x01;                                           // Set ADC0 Sampling frequency at 125KHz
    ADC0_IM_R = 0x08;                                           // Enable Sequencer 3 Interrupts
    ADC0_ACTSS_R = 0x08;                                        // Enable Sequencers 3
    NVIC_EN0_R |= 0x1 << 17;                                    // Interrupt Numbers 17 (ADC0S3)
    NVIC_PRI4_R |= 0x00004000;                                  // Interrupt 17

    ADC1_ACTSS_R = 0x00;                                        // Disable Sequencers
    ADC1_CC_R = 0x1;                                            // 16Mhz PIOSC Clock
    ADC1_EMUX_R = 0x5000;                                       // Set Sequencer 0 to Timer Triggered // Only Vertical Sticks ATM
    ADC1_SSPRI_R = 0x0123;                                      // Set ADC1, Sequencer 3 at highest priority
    ADC1_SSMUX3_R = 0x1;                                        // Set ADC1 Sequencer 3 to Channel 1 - AIN1 (Port E, Pin 2)
    ADC1_SSCTL3_R = 0x0006;                                     // Disable Internal Temp and Differential Input, Enable Interrupts and End of Sequence
    ADC1_PC_R = 0x01;                                           // Set ADC1 Sampling frequency at 125KHz
    ADC1_IM_R = 0x08;                                           // Enable Sequencer 3 Interrupts
    ADC1_ACTSS_R = 0x08;                                        // Enable Sequencers 3
    NVIC_EN1_R |= 1 << 19;                                      // Interrupt Numbers 51 (ADC1S3)
    NVIC_PRI12_R |= 0x40000000;                                 // Interrupt 51

    TIMER0_CTL_R = 0x00;                                        // Disable Timer0
    TIMER0_CTL_R = 0x22;                                        // Set Timer0 as ADC Trigger, Stop Timer during Debug
    TIMER0_CFG_R = 0x00;                                        // Set Timer0 as 32-bit
    TIMER0_TAMR_R = 0x02;                                       // Set Timer0 as Periodic
    TIMER0_TAPR_R = 0x00;                                       // Disable Prescaler
    TIMER0_TAILR_R = 0x1F40;                                    // Corresponds to 100uSec at 80Mhz Clock
    TIMER0_CTL_R |= 0x01;                                       // Enable Timer0

}

uint32_t ADC_Normalization(uint32_t speedADC, uint8_t side){

    if(speedADC >= 2300){
        speedADC = speedADC - 2048;
        if(side == 'L'){
            Left_Sign = PLUS;
        }else{
            Right_Sign = PLUS;
        }
    }else if(speedADC <= 1900){
        speedADC = 2048 - speedADC;
        if(side == 'L'){
            Left_Sign = MINUS;
        }else{
            Right_Sign = MINUS;
        }
    }else{
        speedADC = 0;
    }
    return (speedADC);

}

void ADC0Seq3_Handler(void){

    ADC0_ISC_R |= 0x02;
    uint32_t L_speed = ADC0_SSFIFO3_R;
    Left_Speed = ADC_Normalization(L_speed, 'L');

}

void ADC1Seq3_Handler(void){

    ADC1_ISC_R |= 0x04;
    uint32_t R_speed = ADC1_SSFIFO3_R;
    Right_Speed = ADC_Normalization(R_speed, 'R');

}

