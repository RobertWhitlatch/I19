#include "Master.h"
#include "interrupt.h"

#define STRING_BUFF_SIZE 64

char in_char;
char in_string[STRING_BUFF_SIZE];
int input_strlen;
uint8_t reg;
uint8_t value;
uint8_t data;
unsigned int input;
unsigned int input2;
unsigned int input3;
uint8_t board;
uint8_t group;
uint8_t channel;
uint8_t line;

int getValue(void){
    while((input_strlen = getString(in_string,STRING_BUFF_SIZE)) == EOF);
    fprintf(uart, "\n");
    return(atoi(in_string));
}

int validValueCheck(void){
    if(input_strlen == 0){
        return(1);
    }
    return(0);
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
    line = getValue();
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
                    if(validValueCheck() || group > 7) break;
                    targetChannel();
                    if(validValueCheck() || channel > 1) break;
                    fprintf(uart, "Brushed>");
                    while(1){
                        if((in_char = fgetc(uart)) > -1){
                            fprintf(uart, "%c\n", in_char);
                            switch(in_char){
                                case '1':
                                    fprintf(uart, "Enter power value. (0-4096)\nBrushed>");
                                    input = getValue();
                                    if(validValueCheck() || input > 4096) break;
                                    fprintf(uart, "Firing Group %d, Channel %d Forward with Fast Decay at power %d\n", group, channel, input);
                                    set_all_brushed(group, channel, DECAY_FAST, DIRECTION_FORWARD, input, true);
                                    break;
                                case '2':
                                    fprintf(uart, "Enter power value. (0-4096)\nBrushed>");
                                    input = getValue();
                                    if(validValueCheck() || input > 4096) break;
                                    fprintf(uart, "Firing Group %d, Channel %d Forward with Slow Decay at power %d\n", group, channel, input);
                                    set_all_brushed(group, channel, DECAY_SLOW, DIRECTION_FORWARD, input, true);
                                    break;
                                case '3':
                                    fprintf(uart, "Enter power value. (0-4096)\nBrushed>");
                                    input = getValue();
                                    if(validValueCheck() || input > 4096) break;
                                    fprintf(uart, "Firing Group %d, Channel %d Backward with Fast Decay at power %d\n", group, channel, input);
                                    set_all_brushed(group, channel, DECAY_FAST, DIRECTION_BACKWARD, input, true);
                                    break;
                                case '4':
                                    fprintf(uart, "Enter power value. (0-4096)\nBrushed>");
                                    input = getValue();
                                    if(validValueCheck() || input > 4096) break;
                                    fprintf(uart, "Firing Group %d, Channel %d Backward with Slow Decay at power %d\n", group, channel, input);
                                    set_all_brushed(group, channel, DECAY_SLOW, DIRECTION_BACKWARD, input, true);
                                    break;
                                case '0':
                                    set_speed_brushed(group, channel, 0, true);
                                    break;
                                case 'x':
                                    targetGroup();
                                    if(validValueCheck()) break;
                                    targetChannel();
                                    if(validValueCheck()) break;
                                    break;
                                case 'h':
                                case 'H':
                                    fprintf(uart, "1 - Fire Motor Forward with Fast Decay\n");
                                    fprintf(uart, "2 - Fire Motor Forward with Slow Decay\n");
                                    fprintf(uart, "3 - Fire Motor Backward with Fast Decay\n");
                                    fprintf(uart, "4 - Fire Motor Backward with Slow Decay\n");
                                    fprintf(uart, "0 - Stop Motor\n");
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
                    if(validValueCheck() || group > 7) break;
                    fprintf(uart, "Bridged>");
                    while(1){
                        if((in_char = fgetc(uart)) > -1){
                            fprintf(uart, "%c\n", in_char);
                            switch(in_char){
                                case '1':
                                    fprintf(uart, "Enter power value. (0-4096)\nBridged>");
                                    input = getValue();
                                    if(validValueCheck() || input > 4096) break;
                                    fprintf(uart, "Firing Group %d Forward with Fast Decay at power %d\n", group, input);
                                    set_all_bridged(group, DECAY_FAST, DIRECTION_FORWARD, input, true);
                                    break;
                                case '2':
                                    fprintf(uart, "Enter power value. (0-4096)\nBridged>");
                                    input = getValue();
                                    if(validValueCheck() || input > 4096) break;
                                    fprintf(uart, "Firing Group %d Forward with Slow Decay at power %d\n", group, input);
                                    set_all_bridged(group, DECAY_SLOW, DIRECTION_FORWARD, input, true);
                                    break;
                                case '3':
                                    fprintf(uart, "Enter power value. (0-4096)\nBridged>");
                                    input = getValue();
                                    if(validValueCheck() || input > 4096) break;
                                    fprintf(uart, "Firing Group %d Backward with Fast Decay at power %d\n", group, input);
                                    set_all_bridged(group, DECAY_FAST, DIRECTION_BACKWARD, input, true);
                                    break;
                                case '4':
                                    fprintf(uart, "Enter power value. (0-4096)\nBridged>");
                                    input = getValue();
                                    if(validValueCheck() || input > 4096) break;
                                    fprintf(uart, "Firing Group %d Backward with Slow Decay at power %d\n", group, input);
                                    set_all_bridged(group, DECAY_SLOW, DIRECTION_BACKWARD, input, true);
                                    break;
                                case '0':
                                    set_speed_bridged(group, 0, true);
                                    break;
                                case 'x':
                                    targetGroup();
                                    if(validValueCheck()) break;
                                    break;
                                case 'h':
                                case 'H':
                                    fprintf(uart, "1 - Fire Motor Forward with Fast Decay\n");
                                    fprintf(uart, "2 - Fire Motor Forward with Slow Decay\n");
                                    fprintf(uart, "3 - Fire Motor Backward with Fast Decay\n");
                                    fprintf(uart, "4 - Fire Motor Backward with Slow Decay\n");
                                    fprintf(uart, "0 - Stop Motor\n");
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
                    if(validValueCheck() || group > 7) break;
                    targetLine();
                    if(validValueCheck() || line > 3) break;
                    fprintf(uart, "Servo>");
                    while(1){
                        if((in_char = fgetc(uart)) > -1){
                            fprintf(uart, "%c\n", in_char);
                            switch(in_char){
                                case '1':
                                    fprintf(uart, "Enter lower bound for servo. (410-2048)\n");
                                    input = getValue();
                                    if(validValueCheck() || input < 410 || input > 2048) break;
                                    fprintf(uart, "Enter upper bound for servo. (%d-2048)\n", input);
                                    input2 = getValue();
                                    if(validValueCheck() || input2 < input || input2 > 2048) break;
                                    set_bounds_servo(group, line, input, input2);
                                    break;
                                case '2':
                                    input = get_lower_bound_servo(group, line);
                                    input2 = get_upper_bound_servo(group, line);
                                    fprintf(uart, "Enter new position for servo. (%d-%d)\n", input, input2);
                                    input3 = getValue();
                                    if(validValueCheck() || input3 < input || input3 > input2) break;
                                    set_position_servo(group, line, input3, UPDATE_IMMEDIATE);
                                    break;
                                case '3':
                                    input = get_lower_bound_servo(group, line);
                                    input2 = get_upper_bound_servo(group, line);
                                    input3 = (input + input2)/2;
                                    set_position_servo(group, line, input3, UPDATE_IMMEDIATE);
                                    for(int i = 0; i < 9000000; i++);
                                    set_position_servo(group, line, input, UPDATE_IMMEDIATE);
                                    for(int i = 0; i < 9000000; i++);
                                    set_position_servo(group, line, input2, UPDATE_IMMEDIATE);
                                    for(int i = 0; i < 9000000; i++);
                                    set_position_servo(group, line, input3, UPDATE_IMMEDIATE);
                                    break;
                                case '0':
                                    suspend_servo(group, line);
                                    break;
                                case 'x':
                                    targetGroup();
                                    if(validValueCheck()) break;
                                    targetLine();
                                    if(validValueCheck()) break;
                                    break;
                                case 'h':
                                case 'H':
                                    fprintf(uart, "1 - Set Bounds for Servo\n");
                                    fprintf(uart, "2 - Set Position for Servo\n");
                                    fprintf(uart, "3 - Sweep Servo\n");
                                    fprintf(uart, "0 - Stop Servo\n");
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
                    if(validValueCheck() || group > 7) break;
                    fprintf(uart, "Stepper>");
                    while(1){
                        if((in_char = fgetc(uart)) > -1){
                            fprintf(uart, "%c\n", in_char);
                            switch(in_char){
                                case '1':
                                    fprintf(uart, "Enter number of steps to move. (1-1000)\n");
                                    input = getValue();
                                    if(validValueCheck() || input > 1000) break;
                                    move_num_steps_blocking_stepper(group, input, DIRECTION_FORWARD);
                                    break;
                                case '2':
                                    fprintf(uart, "Enter number of steps to move. (1-1000)\n");
                                    input = getValue();
                                    if(validValueCheck() || input > 1000) break;
                                    move_num_steps_blocking_stepper(group, input, DIRECTION_BACKWARD);
                                    break;
                                case '3':
                                    fprintf(uart, "Enter number of steps to move. (1-1000)\n");
                                    input = getValue();
                                    if(validValueCheck() || input > 1000) break;
                                    move_num_steps_nonblocking_stepper(group, input, DIRECTION_FORWARD);
                                    break;
                                case '4':
                                    fprintf(uart, "Enter number of steps to move. (1-1000)\n");
                                    input = getValue();
                                    if(validValueCheck() || input > 1000) break;
                                    move_num_steps_nonblocking_stepper(group, input, DIRECTION_BACKWARD);
                                    break;
                                case '5':
                                    fprintf(uart, "Enter number of steps per second to rotate. (1-1000)\n");
                                    input = getValue();
                                    if(validValueCheck() || input > 1000) break;
                                    move_continuous_stepper(group, input, DIRECTION_FORWARD);
                                    break;
                                case '6':
                                    fprintf(uart, "Enter number of steps per second to rotate. (1-1000)\n");
                                    input = getValue();
                                    if(validValueCheck() || input > 1000) break;
                                    move_continuous_stepper(group, input, DIRECTION_BACKWARD);
                                    break;
                                case '7':
                                    select_method_stepper(group, STANDARD);
                                    break;
                                case '8':
                                    select_method_stepper(group, HI_TORQUE);
                                    break;
                                case '9':
                                    select_method_stepper(group, HALF_STEPS);
                                    break;
                                case '0':
                                    stop_continuous_stepper(group);
                                    break;
                                case 'x':
                                    targetGroup();
                                    if(validValueCheck()) break;
                                    break;
                                case 'h':
                                case 'H':
                                    fprintf(uart, "1 - Move Stepper X Steps CW (blocking)\n");
                                    fprintf(uart, "2 - Move Stepper X Steps CCW (blocking)\n");
                                    fprintf(uart, "3 - Move Stepper X Steps CW (non-blocking)\n");
                                    fprintf(uart, "4 - Move Stepper X Steps CCW (non-blocking)\n");
                                    fprintf(uart, "5 - Move Stepper continuously CW (non-blocking)\n");
                                    fprintf(uart, "6 - Move Stepper continuously CCW (non-blocking)\n");
                                    fprintf(uart, "7 - Set Stepping method to Standard\n");
                                    fprintf(uart, "8 - Set Stepping method to Hi-Torque\n");
                                    fprintf(uart, "9 - Set Stepping method to Half-Stepping\n");
                                    fprintf(uart, "0 - Stop Stepper Motor\n");
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
                    if(validValueCheck()) break;
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
                                    if(validValueCheck()) break;
                                    fprintf(uart, "Enter value to write (0-255)\n>");
                                    value = getValue();
                                    if(validValueCheck()) break;
                                    if(writeI2C(0x20+board, reg, &value, 1)){
                                        fprintf(uart, "Write successful\n");
                                    }else{
                                        fprintf(uart, "Write failed\n");
                                    }
                                    break;
                                case '4':
                                    fprintf(uart, "Enter PWM register to write (0-69, 250-255)\n>");
                                    reg = getValue();
                                    if(validValueCheck()) break;
                                    fprintf(uart, "Enter value to write (0-255)\n>");
                                    value = getValue();
                                    if(validValueCheck()) break;
                                    if(writeI2C(0x40+board, reg, &value, 1)){
                                        fprintf(uart, "Write successful\n");
                                    }else{
                                        fprintf(uart, "Write failed\n");
                                    }
                                    break;
                                case 'x':
                                    targetBoard();
                                    if(validValueCheck()) break;
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
                    fprintf(uart, "q - Shutdown MotorDepot\n");
                    break;
                case '~':
                    CLEAR_SCREEN
                    break;
                case 'q':
                    MotorDepot_Close();
                    fprintf(uart, "MotorDepot Shutdown\n");
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
