#include "Master.h"

#define GPIO_BASE 0x20
#define PWM_BASE 0x40

void initGPIO(unsigned int* board){
    uint8_t data;
    if(board[0]){
        data = 0x10;
        writeI2C(GPIO_BASE, 1, &data, 1);
        data = 0xE0;
        writeI2C(GPIO_BASE, 3, &data, 1);
    }
    if(board[1]){
        data = 0x10;
        writeI2C(GPIO_BASE+1, 1, &data, 1);
        data = 0xE0;
        writeI2C(GPIO_BASE+1, 3, &data, 1);
    }
}

void disablePower(unsigned int board){
    uint8_t data;
    readI2C(GPIO_BASE+board, 1, &data, READ_REG);
    data &= ~0x10;
    writeI2C(GPIO_BASE+board, 1, &data, 1);
}

void enablePower(unsigned int board){
    uint8_t data;
    readI2C(GPIO_BASE+board, 1, &data, READ_REG);
    data |= 0x10;
    writeI2C(GPIO_BASE+board, 1, &data, 1);
}

void disableDriver(unsigned int group){
    uint8_t localGroup = group % 4;
    uint8_t board = group/4;
    uint8_t data;
    readI2C(GPIO_BASE+board, 1, &data, READ_REG);
    data &= ~(0x1 << localGroup);
    writeI2C(GPIO_BASE+board, 1, &data, 1);
}

void enableDriver(unsigned int group){
    uint8_t localGroup = group % 4;
    uint8_t board = group/4;
    uint8_t data;
    readI2C(GPIO_BASE+board, 1, &data, READ_REG);
    data |= (0x1 << localGroup);
    writeI2C(GPIO_BASE+board, 1, &data, 1);
}

void initPWM(unsigned int* board){
    uint8_t data;
    if(board[0]){
        data = 0x20;
        writeI2C(PWM_BASE, 0, &data, 1);
        data = 0x00;
        for(uint8_t reg = 9; reg < 70; reg += 4){
            writeI2C(PWM_BASE, reg, &data, 1);
        }
    }
    if(board[1]){
        data = 0x20;
        writeI2C(PWM_BASE+1, 0, &data, 1);
        data = 0x00;
        for(uint8_t reg = 9; reg < 70; reg += 4){
            writeI2C(PWM_BASE+1, reg, &data, 1);
        }
    }
}

unsigned int enablePWM(unsigned int board){
    uint8_t data;
    readI2C(PWM_BASE+board, 0x00, &data, READ_REG);
    if(data & 0x80){
        data = 0x20;
        writeI2C(PWM_BASE+board, 0x00, &data, 1);
        return(NO_ERROR);
    }
    return(ERROR);
}

void disablePWM(unsigned int board){
    uint8_t data = 0x30;
    writeI2C(PWM_BASE+board, 0x00, &data, 1);
}

void setPower(uint8_t* data, unsigned int power){
    switch(power){
        case FULL_ON:
            data[3] = 0;
            break;
        case FULL_OFF:
            data[1] = 0;
            break;
        default:
            data[1] = 0x00;
            data[2] = (power & 0x00FF);
            data[3] = (power & 0x0F00) >> 8;
            break;
    }
}

#define PWMx_ON_L 0
#define PWMx_ON_H 1
#define PWMx_OFF_L 2
#define PWMx_OFF_H 3
void setLine(unsigned int group, unsigned int line, unsigned int power){
    uint8_t localGroup = group % 4;
    uint8_t board = group/4;
    uint8_t reg = 6 + localGroup*16 + line*4;
    uint8_t data[4] = {0x00, 0x10, 0x00, 0x10};
    setPower(data, power);
    writeI2C(PWM_BASE+board, reg, data, sizeof(data));
}

void setChannel(unsigned int group, unsigned int channel, unsigned int power1, unsigned int power2){
    uint8_t localGroup = group % 4;
    uint8_t board = group/4;
    uint8_t reg = 6 + localGroup*16 + channel*8;
    uint8_t data[8] = {0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10};
    setPower(data, power1);
    setPower(data+4, power2);
    writeI2C(PWM_BASE+board, reg, data, sizeof(data));
}

void setGroup(unsigned int group, const unsigned int* powers){
    uint8_t localGroup = group % 4;
    uint8_t board = group/4;
    uint8_t reg = 6 + localGroup*16;
    uint8_t data[16] = {0x00, 0x10, 0x00, 0x10,
                        0x00, 0x10, 0x00, 0x10,
                        0x00, 0x10, 0x00, 0x10,
                        0x00, 0x10, 0x00, 0x10};
    for(int i = 0; i < 4; i++){
        setPower(data+i*4, powers[i]);
    }
    writeI2C(PWM_BASE+board, reg, data, sizeof(data));
}

void print_gpio_registers(unsigned int board){
    uint8_t data;
    uint8_t reg[4][2];
    uint8_t index = 0;
    int installed = 0;
    if((board == 0 && BOARD_ZERO_STATUS) || (board == 1 && BOARD_ONE_STATUS)){
        installed = 1;
    }
    if(installed){
        fprintf(uart, "GPIO %d Contents\n", board);
        for(int x = 0x00; x < 0x04; x++){
            readI2C(GPIO_BASE+board, x, &data, READ_REG);
            reg[index][0] = x;
            reg[index++][1] = data;
        }
        for(index = 0; index < 4; index++){
            fprintf(uart, "0x%.2X = 0x%.2X\n", reg[index][0], reg[index][1]);
        }
    }
}

void print_pwm_registers(unsigned int board){
    uint8_t data;
    uint8_t reg[76][2];
    uint8_t index = 0;
    int installed = 0;
    if((board == 0 && BOARD_ZERO_STATUS) || (board == 1 && BOARD_ONE_STATUS)){
        installed = 1;
    }
    if(installed){
        fprintf(uart, "PWM %d Contents\n", board);
        for(int x = 0x00; x < 0x100; x++){
            if(x < 0x46 || x > 0xF9){
                readI2C(PWM_BASE+board, x, &data, READ_REG);
                reg[index][0] = x;
                reg[index++][1] = data;
            }
        }
        for(index = 0; index < 19; index++){
            fprintf(uart, "0x%.2X = 0x%.2X\t", reg[index][0], reg[index][1]);
            fprintf(uart, "0x%.2X = 0x%.2X\t", reg[index+19][0], reg[index+19][1]);
            fprintf(uart, "0x%.2X = 0x%.2X\t", reg[index+38][0], reg[index+38][1]);
            fprintf(uart, "0x%.2X = 0x%.2X\n", reg[index+57][0], reg[index+57][1]);
        }
    }
}

