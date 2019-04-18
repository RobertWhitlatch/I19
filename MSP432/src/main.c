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
uint8_t board;
uint8_t group;
uint8_t channel;
uint8_t line;


int getValue(void){
    while((input_strlen = getString(in_string,STRING_BUFF_SIZE)) == EOF);
    fprintf(uart, "\n");
    return(atoi(in_string));
}

void targetBoard(void){
    fprintf(uart, "Enter which Board to target (0-1).\n>");
    board = getValue();
}

void targetGroup(void){
    fprintf(uart, "Enter which Group to target (0-7).\n>");
    group = getValue();
}

void targetChannel(void){
    fprintf(uart, "Enter the Channel number within that group to target (0-1).\n>");
    channel = getValue();
}

void targetLine(void){
    fprintf(uart, "Enter the Line number within that group to target (0-3).\n>");
    group = getValue();
}

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
                case 'b':
                    fprintf(uart, "Welcome to the Brushed Menu!\n");
                    targetGroup();
                    targetChannel();
                    fprintf(uart, "Brushed>");
                    while(1){
                        if((in_char = fgetc(uart)) > -1){
                            fprintf(uart, "%c\n", in_char);
                            switch(in_char){
                                case 'x':
                                    targetGroup();
                                    targetChannel();
                                    break;
                                case 'h':
                                case 'H':
                                    fprintf(uart, "x - Retarget\n");
                                    fprintf(uart, "h - Help\n");
                                    fprintf(uart, "q - Return to Main Menu\n");
                                    fprintf(uart, "~ - Clear Screen\n");
                                    break;
                                case 'q':
                                    goto BrushedExit;
                                case '~':
                                    CLEAR_SCREEN
                                    break;
                                default:
                                    break;
                            }
                            fprintf(uart, "Brushed>");
                        }
                    }
                    BrushedExit:
                    break;
                case 'd':
                    fprintf(uart, "Welcome to Bridged Menu!\n");
                    targetGroup();
                    fprintf(uart, "Bridged>");
                    while(1){
                        if((in_char = fgetc(uart)) > -1){
                            fprintf(uart, "%c\n", in_char);
                            switch(in_char){
                                case '1':
                                    set_all_bridged(0, DECAY_FAST, DIRECTION_FORWARD, 2048, true);
                                    break;
                                case '2':
                                    set_all_bridged(0, DECAY_FAST, DIRECTION_BACKWARD, 2048, true);
                                    break;
                                case '5':
                                    set_speed_bridged(0, 0, true);
                                    break;
                                case 'x':
                                    targetGroup();
                                    break;
                                case 'h':
                                case 'H':
                                    fprintf(uart, "x - Retarget\n");
                                    fprintf(uart, "h - Help\n");
                                    fprintf(uart, "q - Return to Main Menu\n");
                                    fprintf(uart, "~ - Clear Screen\n");
                                    break;
                                case 'q':
                                    goto BridgedExit;
                                case '~':
                                    CLEAR_SCREEN
                                    break;
                                default:
                                    break;
                            }
                            fprintf(uart, "Bridged>");
                        }
                    }
                    BridgedExit:
                    break;
                case 's':
                    fprintf(uart, "Welcome to the Servo Menu!\n");
                    targetGroup();
                    targetLine();
                    fprintf(uart, "Servo>");
                    while(1){
                        if((in_char = fgetc(uart)) > -1){
                            fprintf(uart, "%c\n", in_char);
                            switch(in_char){
                                case 'x':
                                    targetGroup();
                                    targetLine();
                                    break;
                                case 'h':
                                case 'H':
                                    fprintf(uart, "x - Retarget\n");
                                    fprintf(uart, "h - Help\n");
                                    fprintf(uart, "q - Return to Main Menu\n");
                                    fprintf(uart, "~ - Clear Screen\n");
                                    break;
                                case 'q':
                                    goto ServoExit;
                                case '~':
                                    CLEAR_SCREEN
                                    break;
                                default:
                                    break;
                            }
                            fprintf(uart, "Servo>");
                        }
                    }
                    ServoExit:
                    break;
                case 't':
                    fprintf(uart, "Welcome to the Stepper Menu!\n");
                    targetGroup();
                    fprintf(uart, "Stepper>");
                    while(1){
                        if((in_char = fgetc(uart)) > -1){
                            fprintf(uart, "%c\n", in_char);
                            switch(in_char){
                                case 'x':
                                    targetGroup();
                                    break;
                                case 'h':
                                case 'H':
                                    fprintf(uart, "x - Retarget\n");
                                    fprintf(uart, "h - Help\n");
                                    fprintf(uart, "q - Return to Main Menu\n");
                                    fprintf(uart, "~ - Clear Screen\n");
                                    break;
                                case 'q':
                                    goto StepperExit;
                                case '~':
                                    CLEAR_SCREEN
                                    break;
                                default:
                                    break;
                            }
                            fprintf(uart, "Stepper>");
                        }
                    }
                    StepperExit:
                    break;
                case 'x':
                    fprintf(uart, "Welcome to the Register Menu!\n");
                    targetBoard();
                    fprintf(uart, "Register>");
                    while(1){
                        if((in_char = fgetc(uart)) > -1){
                            fprintf(uart, "%c\n", in_char);
                            switch(in_char){
                                case '1':
                                    print_gpio_registers(board);
                                    break;
                                case '2':
                                    print_pwm_registers(board);
                                    break;
                                case '3':
                                    fprintf(uart, "Enter GPIO register to write (0-3)\n>");
                                    reg = getValue();
                                    fprintf(uart, "Enter value to write (0-255)\n>");
                                    value = getValue();
                                    if(writeI2C(0x20+board, reg, &value, 1)){
                                        fprintf(uart, "Write successful\n");
                                    }else{
                                        fprintf(uart, "Write failed\n");
                                    }
                                    break;
                                case '4':
                                    fprintf(uart, "Enter PWM register to write (0-69, 250-255)\n>");
                                    reg = getValue();
                                    fprintf(uart, "Enter value to write (0-255)\n>");
                                    value = getValue();
                                    if(writeI2C(0x40+board, reg, &value, 1)){
                                        fprintf(uart, "Write successful\n");
                                    }else{
                                        fprintf(uart, "Write failed\n");
                                    }
                                    break;
                                case 'x':
                                    targetBoard();
                                    break;
                                case 'h':
                                case 'H':
                                    fprintf(uart, "1 - Read GPIO Registers\n");
                                    fprintf(uart, "2 - Read PWM Registers\n");
                                    fprintf(uart, "3 - Write GPIO Registers\n");
                                    fprintf(uart, "4 - Write PWM Registers\n");
                                    fprintf(uart, "x - Retarget\n");
                                    fprintf(uart, "h - Help\n");
                                    fprintf(uart, "q - Return to Main Menu\n");
                                    fprintf(uart, "~ - Clear Screen\n");
                                    break;
                                case 'q':
                                    goto RegisterExit;
                                case '~':
                                    CLEAR_SCREEN
                                    break;
                                default:
                                    break;
                            }
                            fprintf(uart, "Register>");
                        }
                    }
                    RegisterExit:
                    break;
                case 'i':
                    printMotorDepot();
                    break;
                case 'h':
                case 'H':
                    fprintf(uart, "b - Brushed Menu\n");
                    fprintf(uart, "d - Bridged Menu\n");
                    fprintf(uart, "s - Servo Menu\n");
                    fprintf(uart, "t - Stepper Menu\n");
                    fprintf(uart, "x - Register Menu\n");
                    fprintf(uart, "i - Information\n");
                    fprintf(uart, "h - Help\n");
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

// Replace "Sub" with menu name
/*
    case 'x':
        fprintf(uart, "Welcome to the Sub Menu!\nSub>");
        while(1){
            if((in_char = fgetc(uart)) > -1){
                fprintf(uart, "%c\n", in_char);
                switch(in_char){
                    case 'h':
                    case 'H':
                        fprintf(uart, "h - Help\n");
                        fprintf(uart, "q - Return to Main Menu\n");
                        fprintf(uart, "~ - Clear Screen\n");
                        break;
                    case 'q':
                        goto SubExit;
                    case '~':
                        CLEAR_SCREEN
                        break;
                    default:
                        break;
                }
                fprintf(uart, "Sub>");
            }
        }
        SubExit:
        break;
*/
