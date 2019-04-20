#ifndef __MASTER_H__
#define __MASTER_H__ 1

// Standard library includes
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

// Environmenal Includes
#include "msp432p401r.h"
#include "msp432p401r_classic.h"
#include "Output.h"
#include "FIFO.h"
#include "ti_i2c_driver.h"
#include "ic_driver.h"
#include "MotorDepot.h"
#include "MotorDepotConfig.h"

// Project includes
#include "CarMain.h"
#include "xBee.h"
#include "FIFO.h"

// Environmental Globals
extern FILE* uart;
extern FILE* null;

// Project Globals

// Project Defines
#define xBee_FIFO_SIZE  256
#define PLUS            0
#define MINUS           1
#define LEFT            0x00
#define RIGHT           0x80
#define ID(x)           (((x)&0x80)>>7)
#define ID_SET(x,y)     (y)=(((y)&0x7F)|((x&0x1)<<7))
#define SIGN(x)         (((x)&0x40)>>6)
#define SIGN_SET(x,y)   (y)=(((y)&0xBF)|((x&0x1)<<5))
#define SPEED(x)        (((x)&0x3F)>>0)
#define SPEED_SET(x,y)  (y)=(((y)&0xC0)|((x&0x3F)<<0))

// Useful Macros
#define MIN(a,b) ((x)<(y))?(x):(y))
#define MAX(x,y) ((x)>(y))?(x):(y))
#define RAND(x)  ((x) = (16807*(x))%2147483647)
#define CLEAR_SCREEN fprintf(uart, "\033[2J\033[1;1H");

// CPU Control Functions
#define WaitForInterrupt()              __sleep()
#define StartCritical()                 __get_interrupt_state()
#define EndCritical(x)                  __set_interrupt_state(x)
#define EnableInterrupts()              __enable_interrupts()
#define DisableInterrupts()             __disable_interrupts()
#define NoOperation()                   __no_operation()

// Additional Architecture definitions

//*****************************************************************************
// NVIC Registers
//*****************************************************************************
#define NVIC_ISER0                                         (HWREG32(0xE000E100)) /* Irq 0 to 31 Set Enable Register */
#define NVIC_ISER1                                         (HWREG32(0xE000E104)) /* Irq 32 to 63 Set Enable Register */
#define NVIC_ICER0                                         (HWREG32(0xE000E180)) /* Irq 0 to 31 Clear Enable Register */
#define NVIC_ICER1                                         (HWREG32(0xE000E184)) /* Irq 32 to 63 Clear Enable Register */
#define NVIC_ISPR0                                         (HWREG32(0xE000E200)) /* Irq 0 to 31 Set Pending Register */
#define NVIC_ISPR1                                         (HWREG32(0xE000E204)) /* Irq 32 to 63 Set Pending Register */
#define NVIC_ICPR0                                         (HWREG32(0xE000E280)) /* Irq 0 to 31 Clear Pending Register */
#define NVIC_ICPR1                                         (HWREG32(0xE000E284)) /* Irq 32 to 63 Clear Pending Register */
#define NVIC_IABR0                                         (HWREG32(0xE000E300)) /* Irq 0 to 31 Active Bit Register */
#define NVIC_IABR1                                         (HWREG32(0xE000E304)) /* Irq 32 to 63 Active Bit Register */
#define NVIC_IPR0                                          (HWREG32(0xE000E400)) /* Irq 0 to 3 Priority Register */
#define NVIC_IPR1                                          (HWREG32(0xE000E404)) /* Irq 4 to 7 Priority Register */
#define NVIC_IPR2                                          (HWREG32(0xE000E408)) /* Irq 8 to 11 Priority Register */
#define NVIC_IPR3                                          (HWREG32(0xE000E40C)) /* Irq 12 to 15 Priority Register */
#define NVIC_IPR4                                          (HWREG32(0xE000E410)) /* Irq 16 to 19 Priority Register */
#define NVIC_IPR5                                          (HWREG32(0xE000E414)) /* Irq 20 to 23 Priority Register */
#define NVIC_IPR6                                          (HWREG32(0xE000E418)) /* Irq 24 to 27 Priority Register */
#define NVIC_IPR7                                          (HWREG32(0xE000E41C)) /* Irq 28 to 31 Priority Register */
#define NVIC_IPR8                                          (HWREG32(0xE000E420)) /* Irq 32 to 35 Priority Register */
#define NVIC_IPR9                                          (HWREG32(0xE000E424)) /* Irq 36 to 39 Priority Register */
#define NVIC_IPR10                                         (HWREG32(0xE000E428)) /* Irq 40 to 43 Priority Register */
#define NVIC_IPR11                                         (HWREG32(0xE000E42C)) /* Irq 44 to 47 Priority Register */
#define NVIC_IPR12                                         (HWREG32(0xE000E430)) /* Irq 48 to 51 Priority Register */
#define NVIC_IPR13                                         (HWREG32(0xE000E434)) /* Irq 52 to 55 Priority Register */
#define NVIC_IPR14                                         (HWREG32(0xE000E438)) /* Irq 56 to 59 Priority Register */
#define NVIC_IPR15                                         (HWREG32(0xE000E43C)) /* Irq 60 to 63 Priority Register */
#define NVIC_STIR                                          (HWREG32(0xE000EF00)) /* Software Trigger Interrupt Register */

#endif
