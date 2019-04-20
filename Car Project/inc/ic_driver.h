#ifndef __IC_DRIVER_H__
#define __IC_DRIVER_H__ 1

#include "Master.h"

#define READ_REG 2

void initGPIO(unsigned int* board);

void disablePower(unsigned int board);

void enablePower(unsigned int board);

void disableDriver(unsigned int group);

void enableDriver(unsigned int group);

void initPWM(unsigned int* board, unsigned int* freq);

unsigned int enablePWM(unsigned int board);

void disablePWM(unsigned int board);

void setServo(unsigned int group, unsigned int line, unsigned int power);

void setBrushed(unsigned int group, unsigned int channel, unsigned int power1, unsigned int power2);

void setBridged(unsigned int group, unsigned int power1, unsigned int power2);

void setStepper(unsigned int group, const unsigned int* powers);

unsigned int calculatePrescale(unsigned int freq);

void print_gpio_registers(unsigned int board);

void print_pwm_registers(unsigned int board);

#endif
