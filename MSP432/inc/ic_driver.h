// ic_driver.h

#ifndef __IC_DRIVER_H__
#define __IC_DRIVER_H__ 1

#include "Master.h"

#define READ_REG 0x02
#define PWM_REG_BASE 0x06

void initGPIO(unsigned int* board);

void closeGPIO(unsigned int* board);

void disablePower(unsigned int board);

void enablePower(unsigned int board);

void disableDriver(unsigned int group);

void enableDriver(unsigned int group);

void initPWM(unsigned int* board, unsigned int* freq);

void closePWM(unsigned int* board);

unsigned int enablePWM(unsigned int board);

void disablePWM(unsigned int board);

void setServo(unsigned int group, unsigned int line, unsigned int power, unsigned int update);

void setBrushed(unsigned int group, unsigned int channel, unsigned int power1, unsigned int power2, unsigned int update);

void setBridged(unsigned int group, unsigned int power1, unsigned int power2, unsigned int update);

void setStepper(unsigned int group, const unsigned int* powers);

void setBoard(unsigned int board);

unsigned int calculatePrescale(unsigned int freq);

void print_gpio_registers(unsigned int board);

void print_pwm_registers(unsigned int board);

#endif
