#include "Master.h"
#include "interrupt.h"

#define STRING_BUFF_SIZE 64

char in_char;
char in_string[STRING_BUFF_SIZE];
int input_strlen;
uint8_t reg;
uint8_t value;
uint8_t data;
uint8_t method = 0;

int main(void){

    STDIO_Init();
    MotorDepot_Init();
    CLEAR_SCREEN
    fprintf(uart, "Welcome to MotorDepot!\n>");
    Interrupt_enableMaster();

    while(1){
        if((in_char = fgetc(uart)) > -1){
            fprintf(uart, "%c\n", in_char);
            switch(in_char){
                case '1':
                    print_gpio_registers(0);
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
                case '2':
                    print_pwm_registers(0);
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
                case '3':
                    print_gpio_registers(1);
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
                case '4':
                    print_pwm_registers(1);
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
                case '5':
                    move_one_step_stepper(0, DIRECTION_FORWARD, true);
                    break;
                case '%':
                    move_one_step_stepper(0, DIRECTION_BACKWARD, true);
                    break;
                case '6':
                    move_num_steps_blocking_stepper(0, 200, DIRECTION_FORWARD);
                    break;
                case '^':
                    move_num_steps_blocking_stepper(0, 200, DIRECTION_BACKWARD);
                    break;
                case '7':
                    move_num_steps_nonblocking_stepper(0, 200, DIRECTION_FORWARD);
                    break;
                case '&':
                    move_num_steps_nonblocking_stepper(0, 200, DIRECTION_BACKWARD);
                    break;
                case '8':
                    move_continuous_stepper(0, 2000, DIRECTION_FORWARD);
                    break;
                case '*':
                    move_continuous_stepper(0, 2500, DIRECTION_BACKWARD);
                    break;
                case '9':
                    method = (method + 1) % 3;
                    select_method_stepper(0, method);
                    break;
                case '(':
                    stop_continuous_stepper(0);
                    break;
                case 'x':
                    fprintf(uart, "Welcome to Bridged Menu!\nBridged>");
                    while(1){
                        if((in_char = fgetc(uart)) > -1){
                            fprintf(uart, "%c\n", in_char);
                            switch(in_char){
                                case '1':
                                    set_all_bridged(0, DECAY_FAST, DIRECTION_FORWARD, 2048, true);
                                    break;
                                case '2':
                                    set_all_bridged(0, DECAY_SLOW, DIRECTION_FORWARD, 2048, true);
                                    break;
                                case '3':
                                    set_all_bridged(0, DECAY_FAST, DIRECTION_BACKWARD, 2048, true);
                                    break;
                                case '4':
                                    set_all_bridged(0, DECAY_SLOW, DIRECTION_BACKWARD, 2048, true);
                                    break;
                                case '5':
                                    suspend_bridged(0);
                                    break;
                                case '6':
                                    resume_bridged(0);
                                    break;
                                case '7':
                                    set_all_bridged(0, DECAY_FAST, DIRECTION_FORWARD, FULL_ON, true);
                                    break;
                                case '8':
                                    set_all_bridged(0, DECAY_SLOW, DIRECTION_FORWARD, FULL_ON, true);
                                    break;
                                case '9':
                                    set_all_bridged(0, DECAY_FAST, DIRECTION_BACKWARD, FULL_ON, true);
                                    break;
                                case '0':
                                    set_all_bridged(0, DECAY_SLOW, DIRECTION_BACKWARD, FULL_ON, true);
                                    break;
                                case 'h':
                                case 'H':
                                    fprintf(uart,
                                                  "1 - 2048, Fast, Forward\n"
                                                  "2 - 2048, Slow, Forward\n"
                                                  "3 - 2048, Fast, Backward\n"
                                                  "4 - 2048, Slow, Backward\n"
                                                  "5 - Stop\n"
                                                  "6 - Start\n"
                                                  "7 - Full On, Fast, Forward\n"
                                                  "8 - Full On, Slow, Forward\n"
                                                  "9 - Full On, Fast, Backward\n"
                                                  "0 - Full On, Slow, Backward\n"
                                                );
                                    fprintf(uart, "h - Help Menu\n");
                                    fprintf(uart, "q - Return to Main Menu\n");
                                    fprintf(uart, "~ - Clear Screen\n");
                                    break;
                                case 'q':
                                    goto MenuExit;
                                case '~':
                                    CLEAR_SCREEN
                                    break;
                                default:
                                    break;
                            }
                            fprintf(uart, "Bridged>");
                        }
                    }
                    MenuExit:
                    break;
                case 'h':
                case 'H':
                    fprintf(uart, "1/! - Read/Write Board 0 GPIO\n");
                    fprintf(uart, "2/! - Read/Write Board 0 PWM\n");
                    fprintf(uart, "3/! - Read/Write Board 1 GPIO\n");
                    fprintf(uart, "4/! - Read/Write Board 1 PWM\n");
                    fprintf(uart, "5/%% - Move Stepper one step left/right\n");
                    fprintf(uart, "6/^ - Move Stepper 200 steps left/right (blocking)\n");
                    fprintf(uart, "7/& - Move Stepper 200 steps left/right (nonblocking)\n");
                    fprintf(uart, "8/* - Move Stepper continuously left/right\n");
                    fprintf(uart, "9/( - Change Method/Stop Stepper\n");
                    fprintf(uart, "x - Bridged SubMenu\n");
                    fprintf(uart, "h - Help Menu\n");
                    fprintf(uart, "~ - Clear Screen\n");
                    break;
                case '~':
                    CLEAR_SCREEN
                    break;
                default:
                    break;
            }
            fprintf(uart, ">");
        }
    }
}

/*
    case 'x':
        fprintf(uart, "Welcome to SubMenu!\nSubMenu>");
        while(1){
            if((in_char = fgetc(uart)) > -1){
                fprintf(uart, "%c\n", in_char);
                switch(in_char){
                    case 'h':
                    case 'H':
                        fprintf(uart, "h - Help Menu\n");
                        fprintf(uart, "q - Return to Main Menu\n");
                        fprintf(uart, "~ - Clear Screen\n");
                        break;
                    case 'q':
                        goto MenuExit;
                    case '~':
                        CLEAR_SCREEN
                        break;
                    default:
                        break;
                }
                fprintf(uart, "SubMenu>");
            }
        }
        MenuExit:
        break;
*/
