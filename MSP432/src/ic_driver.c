#include "Master.h"

#define GPIO_BASE 0x20
#define PWM_BASE 0x40

void initGPIO(unsigned int* board){
    uint8_t data = 0xE0;
    if(board[0]){
        writeI2C(GPIO_BASE, 3, &data, 1);
        writeI2C(GPIO_BASE, 1, &data, 1);
    }
    if(board[1]){
        writeI2C(GPIO_BASE+1, 3, &data, 1);
        writeI2C(GPIO_BASE+1, 1, &data, 1);
    }
}

void disablePower(unsigned int board){
    uint8_t data;
    readI2C(GPIO_BASE+board, 1, &data, READ_REG);
    data |= 0x10;
    writeI2C(GPIO_BASE+board, 1, &data, 1);
}

void enablePower(unsigned int board){
    uint8_t data;
    readI2C(GPIO_BASE+board, 1, &data, READ_REG);
    data &= ~0x10;
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

void initPWM(unsigned int* board, unsigned int* freq){
    uint8_t data;
    if(board[0]){
        data = 0x80;
        writeI2C(PWM_BASE, 0x00, &data, 1);
        if(24 <= freq[0] && freq[0] <= 1526){
            data = 0x30;
            writeI2C(PWM_BASE, 0x00, &data, 1);
            data = calculatePrescale(freq[0]);
            writeI2C(PWM_BASE, 0xFE, &data, 1);
        }
        data = 0x20;
        writeI2C(PWM_BASE, 0, &data, 1);
        data = 0x00;
        for(uint8_t reg = 2; reg < 6; reg++){
            writeI2C(PWM_BASE, reg, &data, 1);
        }
        for(uint8_t reg = 0x6; reg < 0x46; reg++){
            if((reg-1) % 4 == 0){
                data = 0x10;
            } else {
                data = 0x00;
            }
            writeI2C(PWM_BASE, reg, &data, 1);
        }
    }
    if(board[1]){
        data = 0x80;
        writeI2C(PWM_BASE+1, 0x00, &data, 1);
        if(24 <= freq[1] && freq[1] <= 1526){
            data = 0x30;
            writeI2C(PWM_BASE+1,0x00, &data, 1);
            data = calculatePrescale(freq[1]);
            writeI2C(PWM_BASE+1, 0xFE, &data, 1);
        }
        data = 0x20;
        writeI2C(PWM_BASE+1, 0, &data, 1);
        data = 0x00;
        for(uint8_t reg = 2; reg < 6; reg++){
            writeI2C(PWM_BASE+1, reg, &data, 1);
        }
        for(uint8_t reg = 0x06; reg < 0x46; reg++){
            if((reg-1) % 4 == 0){
                data = 0x10;
            } else {
                data = 0x00;
            }
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

void setServo(unsigned int group, unsigned int line, unsigned int power){
    uint8_t localGroup = group % 4;
    uint8_t board = group/4;
    uint8_t reg = 6 + localGroup*16 + line*4;
    uint8_t data[4] = {0x00, 0x10, 0x00, 0x10};
    setPower(data, power);
    writeI2C(PWM_BASE+board, reg, data, sizeof(data));
}

void setBrushed(unsigned int group, unsigned int channel, unsigned int power1, unsigned int power2){
    uint8_t localGroup = group % 4;
    uint8_t board = group/4;
    uint8_t reg = 6 + localGroup*16 + channel*8;
    uint8_t data[8] = {0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10};
    setPower(data, power1);
    setPower(data+4, power2);
    writeI2C(PWM_BASE+board, reg, data, sizeof(data));
}

void setBridged(unsigned int group, unsigned int power1, unsigned int power2){

    uint8_t localGroup = group % 4;
    uint8_t board = group/4;
    uint8_t reg = 6 + localGroup*16;
    uint8_t data[16] = {
                        0x00, 0x10, 0x00, 0x10,
                        0x00, 0x10, 0x00, 0x10,
                        0x00, 0x10, 0x00, 0x10,
                        0x00, 0x10, 0x00, 0x10,
                       };
    setPower(data, power1);
    setPower(data+4, power2);
    setPower(data+8, power1);
    setPower(data+12, power2);
    writeI2C(PWM_BASE+board, reg, data, sizeof(data));

}

void setStepper(unsigned int group, const unsigned int* powers){

    uint8_t localGroup = group % 4;
    uint8_t board = group/4;
    uint8_t reg = 7 + localGroup*16;
    uint8_t data;
    for(int i = 0; i < 4; i++){
        data = powers[i];
        writeI2C(PWM_BASE+board, reg+i*4, &data, 1);
    }

}

unsigned int calculatePrescale(unsigned int freq){

    unsigned int divisor = 4096*freq;
    unsigned int integer = 25000000/divisor;
    unsigned int remainder = 25000000 % divisor;
    if(remainder >= divisor/2){
        integer++;
    }
    integer--;
    return(integer);

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
