#ifndef __IC_DRIVER_H__
#define __IC_DRIVER_H__ 1

#include "Master.h"

#define READ_REG 2

void initGPIO(unsigned int* board);

void disablePower(unsigned int board);

void enablePower(unsigned int board);

void disableDriver(unsigned int group);

void enableDriver(unsigned int group);

void initPWM(unsigned int* board);

unsigned int enablePWM(unsigned int board);

void disablePWM(unsigned int board);

void setLine(unsigned int group, unsigned int line, unsigned int power);

void setChannel(unsigned int group, unsigned int channel, unsigned int power1, unsigned int power2);

void setGroup(unsigned int group, const unsigned int* powers);

void print_gpio_registers(unsigned int board);

void print_pwm_registers(unsigned int board);

#endif
