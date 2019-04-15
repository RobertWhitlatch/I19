#include "Master.h"

#define STRING_BUFF_SIZE 64

char in_char;
char in_string[STRING_BUFF_SIZE];
int input_strlen;
uint8_t reg;
uint8_t value;
uint8_t data;

int main(void){

    STDIO_Init();
    initI2C();
    fprintf(uart, "\033[2J\033[1;1HWelcome to MotorDepot!\n>");

    while(1){
        if((in_char = fgetc(uart)) > -1){
            fprintf(uart, "%c\n", in_char);
            switch(in_char){
                case '1':
                    print_gpio_registers(0);
                    break;
                case '2':
                    print_pwm_registers(0);
                    break;
                case '3':
                    print_gpio_registers(1);
                    break;
                case '4':
                    print_pwm_registers(1);
                    break;
                case '!':
                    fprintf(uart, "Write Board 0 GPIO Registers\n");
                    fprintf(uart, "Enter register to write (0-3)\n>");
                    while((input_strlen = getString(in_string,STRING_BUFF_SIZE)) == EOF);
                    fprintf(uart, "\n");
                    reg = atoi(in_string);
                    fprintf(uart, "Enter value to write (0-255)\n>");
                    while((input_strlen = getString(in_string,STRING_BUFF_SIZE)) == EOF);
                    fprintf(uart, "\n");
                    value = atoi(in_string);
                    data = value;
                    writeI2C(0x20, reg, &data, 1);
                    data = 0;
                    readI2C(0x20, reg, &data, READ_REG);
                    if(value == data){
                        fprintf(uart, "Write successful\n");
                    }else{
                        fprintf(uart, "Write failed\n");
                    }
                    break;
                case '@':
                    fprintf(uart, "Write Board 0 PWM Registers\n");
                    fprintf(uart, "Enter register to write (0-69, 250-255)\n>");
                    while((input_strlen = getString(in_string,STRING_BUFF_SIZE)) == EOF);
                    fprintf(uart, "\n");
                    reg = atoi(in_string);
                    fprintf(uart, "Enter value to write (0-255)\n>");
                    while((input_strlen = getString(in_string,STRING_BUFF_SIZE)) == EOF);
                    fprintf(uart, "\n");
                    value = atoi(in_string);
                    data = value;
                    writeI2C(0x40, reg, &data, 1);
                    data = 0;
                    readI2C(0x40, reg, &data, READ_REG);
                    if(value == data){
                        fprintf(uart, "Write successful\n");
                    }else{
                        fprintf(uart, "Write failed\n");
                    }
                    break;
                case '#':
                    fprintf(uart, "Write Board 1 GPIO Registers\n");
                    fprintf(uart, "Enter register to write (0-3)\n>");
                    while((input_strlen = getString(in_string,STRING_BUFF_SIZE)) == EOF);
                    fprintf(uart, "\n");
                    reg = atoi(in_string);
                    fprintf(uart, "Enter value to write (0-255)\n>");
                    while((input_strlen = getString(in_string,STRING_BUFF_SIZE)) == EOF);
                    fprintf(uart, "\n");
                    value = atoi(in_string);
                    data = value;
                    writeI2C(0x21, reg, &data, 1);
                    data = 0;
                    readI2C(0x21, reg, &data, READ_REG);
                    if(value == data){
                        fprintf(uart, "Write successful\n");
                    }else{
                        fprintf(uart, "Write failed\n");
                    }
                    break;
                case '$':
                    fprintf(uart, "Write Board 1 PWM Registers\n");
                    fprintf(uart, "Enter register to write (0-69, 250-255)\n>");
                    while((input_strlen = getString(in_string,STRING_BUFF_SIZE)) == EOF);
                    fprintf(uart, "\n");
                    reg = atoi(in_string);
                    fprintf(uart, "Enter value to write (0-255)\n>");
                    while((input_strlen = getString(in_string,STRING_BUFF_SIZE)) == EOF);
                    fprintf(uart, "\n");
                    value = atoi(in_string);
                    data = value;
                    writeI2C(0x41, reg, &data, 1);
                    data = 0;
                    readI2C(0x41, reg, &data, READ_REG);
                    if(value == data){
                        fprintf(uart, "Write successful\n");
                    }else{
                        fprintf(uart, "Write failed\n");
                    }
                    break;
                case 'h':
                case 'H':
                    fprintf(uart, "1 - Print Board 0 GPIO Contents\n");
                    fprintf(uart, "2 - Print Board 0 PWM Contents\n");
                    fprintf(uart, "3 - Print Board 1 GPIO Contents\n");
                    fprintf(uart, "4 - Print Board 1 PWM Contents\n");
                    fprintf(uart, "5 - Write Board 0 GPIO Registers\n");
                    fprintf(uart, "6 - Write Board 0 PWM Registers\n");
                    fprintf(uart, "7 - Write Board 1 GPIO Registers\n");
                    fprintf(uart, "8 - Write Board 1 PWM Registers\n");
                    fprintf(uart, "h - Help Menu\n");
                    break;
                default:
                    break;
            }
            fprintf(uart, ">");
        }
    }
}
