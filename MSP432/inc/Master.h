#ifndef __MASTER_H__
#define __MASTER_H__ 1

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "msp432p401r.h"
#include "ti_i2c_driver.h"
#include "ic_driver.h"
#include "MotorDepot.h"
#include "MotorDepotConfig.h"

#define MIN(a,b) ((x)<(y))?(x):(y))
#define MAX(x,y) ((x)>(y))?(x):(y))

extern FILE* uart;

void STDIO_Init(void);

int getString(char* buff, int buff_length);

#endif
